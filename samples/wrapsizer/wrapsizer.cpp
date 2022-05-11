/////////////////////////////////////////////////////////////////////////////
// Name:        wrapsizer.cpp
// Purpose:     wxWidgets sample demonstrating wxWrapSizer use
// Author:      Arne Steinarson
// Created:     21.01.2008
// Copyright:   (c) Arne Steinarson
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/log.h"
#include "wx/wrapsizer.h"
#include "wx/artprov.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif


// ----------------------------------------------------------------------------
// definitions
// ----------------------------------------------------------------------------

class WrapSizerFrame : public wxFrame
{
public:
    WrapSizerFrame();

private:
    void OnButton(wxCommandEvent& WXUNUSED(event))
    {
        Close();
    }

    void AddToolBarButton(wxToolBar *tb,
                          const wxString& label,
                          const wxString& artid)
    {
        wxBitmap
            bm = wxArtProvider::GetBitmap(artid, wxART_TOOLBAR, wxSize(16, 16));
        tb->AddTool(wxID_ANY, label, bm);
    }

    wxToolBar *MakeToolBar()
    {
        wxToolBar *tb = new wxToolBar(m_panel, wxID_ANY,
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTB_NODIVIDER);
        AddToolBarButton(tb, "Help", wxART_HELP_BOOK);
        tb->AddSeparator( );
        AddToolBarButton(tb, "Open", wxART_FILE_OPEN);
        tb->AddSeparator( );
        AddToolBarButton(tb, "Up", wxART_GO_DIR_UP);
        AddToolBarButton(tb, "Execute", wxART_EXECUTABLE_FILE);

        tb->Realize( );
        return tb;
    }

    wxPanel *m_panel;
};

class WrapSizerApp : public wxApp
{
public:
    WrapSizerApp() {}

    virtual bool OnInit() wxOVERRIDE
    {
        new WrapSizerFrame;
        return true;
    }
};

wxIMPLEMENT_APP(WrapSizerApp);


// ----------------------------------------------------------------------------
// WrapSizerFrame
// ----------------------------------------------------------------------------

WrapSizerFrame::WrapSizerFrame()
        : wxFrame(NULL, wxID_ANY, "wxWrapSizer Sample")
{
    SetIcon(wxICON(sample));

    m_panel = new wxPanel(this);

    // Root sizer, vertical
    wxSizer * const sizerRoot = new wxBoxSizer(wxVERTICAL);

    // Some toolbars in a wrap sizer
    wxSizer * const sizerTop = new wxWrapSizer( wxHORIZONTAL );
    sizerTop->Add(MakeToolBar());
    sizerTop->Add(20, 1);
    sizerTop->Add(MakeToolBar());
    sizerTop->Add(20, 1);
    sizerTop->Add(MakeToolBar());
    sizerRoot->Add(sizerTop, wxSizerFlags().Expand().Border());

    // A number of checkboxes inside a wrap sizer
    wxSizer *sizerMid = new wxStaticBoxSizer(wxVERTICAL, m_panel,
                                                "With check-boxes");
    wxSizer * const sizerMidWrap = new wxWrapSizer(wxHORIZONTAL);
    for ( int nCheck = 0; nCheck < 6; nCheck++ )
    {
        wxCheckBox *chk = new wxCheckBox
                                (
                                m_panel,
                                wxID_ANY,
                                wxString::Format("Option %d", nCheck)
                                );

        sizerMidWrap->Add(chk, wxSizerFlags().Centre().Border());
    }

    sizerMid->Add(sizerMidWrap, wxSizerFlags(100).Expand());
    sizerRoot->Add(sizerMid, wxSizerFlags(100).Expand().Border());


    // A shaped item inside a box sizer
    wxSizer *sizerBottom = new wxStaticBoxSizer(wxVERTICAL, m_panel,
                                                "With wxSHAPED item");
    wxSizer *sizerBottomBox = new wxBoxSizer(wxHORIZONTAL);
    sizerBottom->Add(sizerBottomBox, wxSizerFlags(100).Expand());

    sizerBottomBox->Add(new wxListBox(m_panel, wxID_ANY,
                                        wxPoint(0, 0), wxSize(70, 70)),
                        wxSizerFlags().Expand().Shaped());
    sizerBottomBox->AddSpacer(10);
    sizerBottomBox->Add(new wxCheckBox(m_panel, wxID_ANY,
                                        "A much longer option..."),
                        wxSizerFlags(100).Border());
    sizerRoot->Add(sizerBottom, wxSizerFlags(100).Expand().Border());

    // OK Button
    sizerRoot->Add(new wxButton(m_panel, wxID_OK),
                    wxSizerFlags().Centre().DoubleBorder());
    Bind(wxEVT_BUTTON, &WrapSizerFrame::OnButton, this, wxID_OK);

    // Set sizer for the panel
    m_panel->SetSizer(sizerRoot);

    SetClientSize(m_panel->GetBestSize());

    Show();
}

