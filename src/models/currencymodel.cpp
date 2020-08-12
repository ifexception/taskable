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

#include "currencymodel.h"

namespace app::model
{
CurrencyModel::CurrencyModel()
    : mCurrencyId(-1)
    , mName(wxGetEmptyString())
    , mCode(wxGetEmptyString())
    , mSymbol(wxGetEmptyString())
{
}

CurrencyModel::CurrencyModel(const int currencyId)
    : CurrencyModel()
{
    mCurrencyId = currencyId;
}

CurrencyModel::CurrencyModel(int currencyId, wxString name, wxString code, wxString symbol)
    : CurrencyModel()
{
    mCurrencyId = currencyId;
    mName = name;
    mCode = code;
    mSymbol = symbol;
}

const int CurrencyModel::GetCurrencyId() const
{
    return mCurrencyId;
}

const wxString CurrencyModel::GetName() const
{
    return mName;
}

const wxString CurrencyModel::GetCode() const
{
    return mCode;
}

const wxString CurrencyModel::GetSymbol() const
{
    return mSymbol;
}

void CurrencyModel::SetCurrencyId(const int currencyId)
{
    mCurrencyId = currencyId;
}

void CurrencyModel::SetName(const wxString& name)
{
    mName = name;
}

void CurrencyModel::SetCode(const wxString& code)
{
    mCode = code;
}

void CurrencyModel::SetSymbol(const wxString& symbol)
{
    mSymbol = symbol;
}
} // namespace app::model
