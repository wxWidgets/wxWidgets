// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/gizmos/editlbox.h"
#include "wx/sizer.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
    wxDialog dlg(NULL, wxID_ANY, _("Test dialog"), wxDefaultPosition, wxDefaultSize,
                 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxEditableListBox(&dlg, wxID_ANY, _("Match these wildcards:"),
                                     wxDefaultPosition,wxSize(300,200)),
               1, wxEXPAND|wxALL, 10);

    sizer->Add(5,5);

    wxEditableListBox *lb = new wxEditableListBox(&dlg, wxID_ANY, _("Except:"),
                                     wxDefaultPosition,wxSize(300,200));
    wxArrayString ar;
    ar.Add(_T("*.cpp"));
    ar.Add(_T("*.h"));
    ar.Add(_T("*.c"));
    lb->SetStrings(ar);

    sizer->Add(lb, 1, wxEXPAND|wxALL, 10);

    sizer->Add(5,5);

    sizer->Add(new wxButton(&dlg, wxID_OK, _("OK")), 0, wxALIGN_RIGHT | wxALL, 10);
    dlg.SetSizer(sizer);
    sizer->Fit(&dlg);
    dlg.Centre();

    dlg.ShowModal();

    wxString res = _("'Except' contains these strings:\n\n");
    lb->GetStrings(ar);
    for (size_t i = 0; i < ar.GetCount(); i++)
        res << ar[i] << _T("\n");
    wxMessageBox(res);

    return false;
}
