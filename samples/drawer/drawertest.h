/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.h
// Purpose:     wxTaskBarIcon sample
// Author:      Julian Smart
// Modified by: Ryan Norton (OSX 10.2+ Drawer)
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )
    #define USE_DRAWER_CODE 1
#else
    #define USE_DRAWER_CODE 0
#endif

class MyTaskBarIcon: public wxTaskBarIcon
{
public:
    MyTaskBarIcon() {};

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);

    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuSetOldIcon(wxCommandEvent&);

#if USE_DRAWER_CODE
    class MyDrawer* m_pMyDrawer;
    MyTaskBarIcon(class MyDrawer* p) {m_pMyDrawer = p;}

    void OnMenuOpenDrawer(wxCommandEvent&);
    void OnMenuCloseDrawer(wxCommandEvent&);

    void OnMenuLeftDrawer(wxCommandEvent&);
    void OnMenuRightDrawer(wxCommandEvent&);
    void OnMenuTopDrawer(wxCommandEvent&);
    void OnMenuBottomDrawer(wxCommandEvent&);
#endif // USE_DRAWER_CODE

    virtual wxMenu *CreatePopupMenu();

DECLARE_EVENT_TABLE()
};


// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

class MyDialog: public wxDialog
{
public:
    MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long windowStyle = wxDEFAULT_DIALOG_STYLE);
    ~MyDialog();

    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void Init(void);

protected:
    MyTaskBarIcon   *m_taskBarIcon;

    DECLARE_EVENT_TABLE()
};


