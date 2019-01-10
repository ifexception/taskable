#include "about_dialog.hh"

namespace app::dialog
{
wxIMPLEMENT_DYNAMIC_CLASS(about_dialog, wxDialog);

wxBEGIN_EVENT_TABLE(about_dialog, wxDialog)
wxEND_EVENT_TABLE()

about_dialog::about_dialog(wxWindow* parent, const wxString &name)
{

}
}
