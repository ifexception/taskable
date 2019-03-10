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
    ~about_dialog();

  private:
    bool create(wxWindow* parent,
        wxWindowID windowId,
        const wxString& title,
        const wxPoint& point,
        const wxSize& size,
        long style,
        const wxString& name);

    void create_controls();

    void on_ok(wxCommandEvent& event);

    wxString GetText();
    wxTextCtrl* pTextCtrl;
};
} // namespace app::dialog
