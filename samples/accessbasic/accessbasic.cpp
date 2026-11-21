/////////////////////////////////////////////////////////////////////////////
// Name:        accessbasic.cpp
// Purpose:     Sample showing how to use wxAccessible
// Author:      Joshua Lee Ockert
// Created:     2026-11-09
// Copyright:   (c) 2026 Joshua Lee Ockert
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/access.h>

static const wxString kName    = "Main window area";
static const wxString kTextAcc = "Hello, Accessibility via wxAccessible";
static const wxString kTextScn = "Hello, Accessibility";
static const wxColour kTaupe(0xD8, 0xD0, 0xC6);

#if wxUSE_ACCESSIBILITY

class HelloAccessible : public wxAccessible
{
public:
    explicit HelloAccessible(wxWindow* win) : wxAccessible(win) { }

    wxAccStatus GetValue(int childId, wxString* value) override
    {
        if ( childId != wxACC_SELF || !value )
            return wxACC_FAIL;
        *value = kTextAcc;
        return wxACC_OK;
    }

    wxAccStatus GetName(int childId, wxString* name) override
    {
        if ( childId != wxACC_SELF || !name )
            return wxACC_FAIL;
        *name = kName;
        return wxACC_OK;
    }

    wxAccStatus GetRole(int childId, wxAccRole* role) override
    {
        if ( childId != wxACC_SELF || !role )
            return wxACC_FAIL;
        *role = wxROLE_SYSTEM_STATICTEXT;
        return wxACC_OK;
    }

    wxAccStatus GetState(int childId, long* state) override
    {
        if ( childId != wxACC_SELF || !state )
            return wxACC_FAIL;
        *state = wxACC_STATE_SYSTEM_READONLY;
        return wxACC_OK;
    }

    wxAccStatus GetChildCount(int* count) override
    {
        if ( !count )
            return wxACC_FAIL;
        *count = 0;
        return wxACC_OK;
    }
    // everything else falls through to wxACC_NOT_IMPLEMENTED
};

#endif // wxUSE_ACCESSIBILITY

class HelloWindow : public wxWindow
{
public:
    explicit HelloWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
    {
        SetBackgroundColour(kTaupe);
        Bind(wxEVT_PAINT, &HelloWindow::OnPaint, this);
#if wxUSE_ACCESSIBILITY
        SetAccessible(new HelloAccessible(this));   // window takes ownership
#endif
    }

private:
    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);
        dc.SetBackground(wxBrush(kTaupe));
        dc.Clear();

        wxFont font = GetFont();
        font.SetPointSize(font.GetPointSize() * 2);
        dc.SetFont(font);
        dc.SetTextForeground(*wxRED);

        const wxSize client = GetClientSize();
        const wxSize extent = dc.GetTextExtent(kTextScn);
        dc.DrawText(kTextScn,
                    (client.x - extent.x) / 2,
                    (client.y - extent.y) / 2);
    }
};

class MainFrame : public wxFrame
{
public:
    MainFrame()
        : wxFrame(nullptr, wxID_ANY, "wxAccessible Minimal Example",
                  wxDefaultPosition, wxSize(480, 200))
    {
        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(wxID_EXIT);

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(fileMenu, "&File");
        SetMenuBar(menuBar);

        Bind(wxEVT_MENU, [this](wxCommandEvent&) { Close(true); }, wxID_EXIT);

        new HelloWindow(this);   // sole child is auto-sized to the client area
    }
};

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {
        (new MainFrame)->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);