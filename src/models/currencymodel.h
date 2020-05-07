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

#pragma once

#include <memory>
#include <vector>

#include <wx/datetime.h>
#include <wx/string.h>

#include "../common/constants.h"

namespace app::model
{
class CurrencyModel final
{
public:
    CurrencyModel();
    CurrencyModel(const int currencyId);
    CurrencyModel(int currencyId, wxString name, wxString code, wxString symbol);
    ~CurrencyModel() = default;

    const int GetCurrencyId() const;
    const wxString GetName() const;
    const wxString GetCode() const;
    const wxString GetSymbol() const;

    void SetCurrencyId(const int currencyId);
    void SetName(const wxString& name);
    void SetCode(const wxString& code);
    void SetSymbol(const wxString& symbol);

private:
    int mCurrencyId;
    wxString mName;
    wxString mCode;
    wxString mSymbol;
};
} // namespace app::model
