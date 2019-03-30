//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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
//
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "app.hh"

#include "common/constants.hh"
#include "frame/main_frame.hh"

namespace app
{
app::~app()
{
}

bool app::OnInit()
{
    bool loggingInitialize = init_logging();
    if (!loggingInitialize) {
        return false;
    }

    frame::main_frame* mf = new frame::main_frame();
    mf->Show(true);
    return true;
}

bool app::init_logging()
{
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    spdlog::flush_every(std::chrono::seconds(3));
    try {
        pLogger = spdlog::daily_logger_st(Constants::LoggerName, "logs/TimesheetsTracker.log.txt");
        pLogger->info(Constants::Info::LoggerInitialized);
    } catch (const spdlog::spdlog_ex& e) {
        wxString error;
        error.Printf(wxT("Error initializing logger: %s"), e.what());
        wxMessageBox(error, wxT("Error"), wxOK_DEFAULT | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}
} // namespace app

wxIMPLEMENT_APP(app::app);
