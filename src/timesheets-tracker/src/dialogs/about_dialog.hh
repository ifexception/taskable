#pragma once

#include <wx/wx.h>

namespace app::dialog
{
class about_dialog : public wxDialog
{
    wxDECLARE_DYNAMIC_CLASS(about_dialog);
    wxDECLARE_EVENT_TABLE();

  public:
    about_dialog() = default;
    explicit about_dialog(wxWindow* parent,
        const wxString& name = "about_dialog");

  private:
    bool create(wxWindow* parent,
        wxWindowID windowId,
        const wxString &caption,
        const wxPoint &position,
        const wxSize &size,
        long style,
        int tabToOpen,
        const wxString &name);
};
} // namespace app::dialog
