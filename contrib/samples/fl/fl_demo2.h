/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     04/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __FLDEMO_G__
#define __FLDEMO_G__

// ID for the menu commands

#define MINIMAL_QUIT    1
#define MINIMAL_ABOUT 102

#define ID_LOAD      103
#define ID_STORE     104
#define ID_AUTOSAVE  105
//#define ID_SETTINGS  106
#define ID_REMOVE    107
#define ID_REMOVEALL 108
#define ID_RECREATE  109
#define ID_ACTIVATE  110
#define ID_FIRST     111
#define ID_SECOND    112
#define ID_THIRD     113

#define ID_SAY_ITSOK 114
#define ID_BTN_YES   115
#define ID_BTN_NO    116
#define ID_BTN_ESC   117

#define MAX_LAYOUTS   3

#define FIRST_LAYOUT  0
#define SECOND_LAYOUT 1
#define THIRD_LAYOUT  2

class wxFrameLayout;
class wxObjectStorage;

// FOR NOW::
typedef wxPanel MyTestPanel;

// Define a new application type

class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

// Define a new frame type

class MyFrame: public wxFrame
{
protected:

    wxFrameLayout* mLayouts[MAX_LAYOUTS];

    wxFrameLayout* mpNestedLayout;
    wxFrameLayout* mpAboutBoxLayout;

    int            mActiveLayoutNo;
    bool           mAutoSave;
    bool           mSavedAlready;

    // container windows:

    wxTextCtrl*    mpClntWindow;
    wxPanel*       mpInternalFrm;

    wxImageList    mImageList;

    wxFrame       mAboutBox;

    // helpers for control-creation

    wxTextCtrl* CreateTxtCtrl ( const wxString& txt = wxT("wxTextCtrl"), wxWindow* parent = NULL  );
    wxTreeCtrl* CreateTreeCtrl( const wxString& label = wxT("TreeCtrl") );
    wxChoice*   CreateChoice  ( const wxString& txt = wxT("Choice1") );
    wxButton*   CreateButton  ( const wxString& label = wxT("wxButton"), wxWindow* pParent = NULL, long id = ID_SAY_ITSOK );

    // helpers for layout-creation

    void AddSearchToolbars( wxFrameLayout& layout, wxWindow* pParent );
    wxWindow* CreateDevLayout( wxFrameLayout& layout, wxWindow* pParent );

    void DropInSomeBars( int layoutNo );
    void CreateLayout( int layoutNo );
    void RemoveLayout( int layoutNo );

    void InitAboutBox();

    void ActivateLayout( int layoutNo );

public: /* public */

    MyFrame( wxFrame *frame, const wxChar *title, int x, int y, int w, int h);

    ~MyFrame();

    void SyncMenuBarItems();

    // event handlers

    bool OnClose(void);

    void OnLoad( wxCommandEvent& event );
    void OnStore( wxCommandEvent& event );
    void OnAutoSave( wxCommandEvent& event );
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    //void OnSettings( wxCommandEvent& event );
    void OnRemove( wxCommandEvent& event );
    void OnRemoveAll( wxCommandEvent& event );
    void OnRecreate( wxCommandEvent& event );
    void OnFirst( wxCommandEvent& event );
    void OnSecond( wxCommandEvent& event );
    void OnThird( wxCommandEvent& event );

    void OnSayItsOk( wxCommandEvent& event );
    void OnBtnYes( wxCommandEvent& event );
    void OnBtnNo( wxCommandEvent& event );
    void OnBtnEsc( wxCommandEvent& event );

    void OnChar( wxKeyEvent& event );

    DECLARE_EVENT_TABLE()
};

// Define a new button type, StartButton95 (Just for fun)

class StartButton95 : public wxPanel
{
    DECLARE_DYNAMIC_CLASS( StartButton95 )

    bool m_bPressed;
    wxBitmap m_PBmp;
    wxBitmap m_DBmp;

public:
    StartButton95(void) : m_bPressed(false) {}

    StartButton95(wxWindow* parent)
        : m_bPressed(false) { wxPanel::Create(parent,wxID_ANY); }

    void OnMouseDown( wxMouseEvent& event );
    void OnMouseUp( wxMouseEvent& event );
    void OnPaint( wxPaintEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif

