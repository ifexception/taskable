// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "currencydata.h"

#include <spdlog/spdlog.h>

namespace app::data
{
CurrencyData::CurrencyData()
    : bBorrowedConnection(false)
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
    spdlog::get("msvc")->debug("ACQUIRE connection in CurrencyData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

CurrencyData::CurrencyData(std::shared_ptr<db::SqliteConnection> connection)
    : bBorrowedConnection(true)
{
    pConnection = connection;
    spdlog::get("msvc")->debug("BORROW connection in CurrencyData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

CurrencyData::~CurrencyData()
{
    if (!bBorrowedConnection) {
        db::ConnectionProvider::Get().Handle()->Release(pConnection);
        spdlog::get("msvc")->debug("RELEASE connection in CurrencyData|ConnectionTally: {0:d}",
            db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
    }
}

std::unique_ptr<model::CurrencyModel> CurrencyData::GetById(const int id)
{
    std::unique_ptr<model::CurrencyModel> currency = nullptr;

    *pConnection->DatabaseExecutableHandle() << CurrencyData::getCurrencyById << id >>
        [&](int currencyId, std::string name, std::string code, std::string symbol) {
            currency =
                std::make_unique<model::CurrencyModel>(currencyId, wxString(name), wxString(code), wxString(symbol));
        };

    return std::move(currency);
}

std::vector<std::unique_ptr<model::CurrencyModel>> CurrencyData::GetAll()
{
    std::vector<std::unique_ptr<model::CurrencyModel>> currencies;

    *pConnection->DatabaseExecutableHandle() << CurrencyData::getCurrencies >>
        [&](int currencyId, std::string name, std::string code, std::string symbol) {
            auto currency =
                std::make_unique<model::CurrencyModel>(currencyId, wxString(name), wxString(code), wxString(symbol));
            currencies.push_back(std::move(currency));
        };

    return currencies;
}

const std::string CurrencyData::getCurrencyById = "SELECT currency_id, "
                                                  "name, "
                                                  "code, "
                                                  "symbol "
                                                  "FROM currencies "
                                                  "WHERE currency_id = ?";

const std::string CurrencyData::getCurrencies = "SELECT currency_id, "
                                                "name, "
                                                "code, "
                                                "symbol "
                                                "FROM currencies";
} // namespace app::data
