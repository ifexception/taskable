//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "config.h"

#include <rapidxml/rapidxml.hpp>

#include "util.h"

namespace app::cfg
{
const std::string CFG_FILE = "tasks-tracker.cfg.xml";

Config& Config::Get()
{
    static Config instance;
    return instance;
}

std::string Config::GetConnectionString() const
{
    return mConnectionString;
}

Config::Config()
{
    auto cfgFileContents = util::ReadFile(CFG_FILE);

    rapidxml::xml_document<> cfgDocument;
    cfgDocument.parse<0>(&cfgFileContents[0]);

    auto configurationNode = cfgDocument.first_node("configuration");
    // FIXME Better way of reading configuration
    for (auto node = configurationNode->first_node(); node; node = node->next_sibling()) {
#ifdef _DEBUG
        if (std::string(node->first_attribute("type")->value()) == "debug") {
            auto connectionString = node->value();
            SetConnectionString(std::string(connectionString));
            break;
        }
#else
        if (node->first_attribute("type")->value() == "release") {
            auto connectionString = node->value();
            set_connection_string(std::string(connectionString));
            break;
        }
#endif
    }
}

void Config::SetConnectionString(const std::string& connectionString)
{
    mConnectionString = connectionString;
}
} // namespace app::cfg
