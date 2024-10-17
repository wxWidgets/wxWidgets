/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.h
// Purpose:     wxTaskBarIcon sample
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

class MyTaskBarIcon : public wxTaskBarIcon
{
public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
    :   wxTaskBarIcon(iconType)
#else
    MyTaskBarIcon()
#endif
    {}

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuCheckmark(wxCommandEvent&);
    void OnMenuUICheckmark(wxUpdateUIEvent&);
    void OnMenuSub(wxCommandEvent&);
    virtual wxMenu *CreatePopupMenu() override;

    wxDECLARE_EVENT_TABLE();
};


// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

class MyDialog: public wxDialog
{
public:
    MyDialog(const wxString& title);
    virtual ~MyDialog();

protected:
    void OnAbout(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon   *m_dockIcon;
#endif

    wxDECLARE_EVENT_TABLE();
};
