/////////////////////////////////////////////////////////////////////////////
// Name:        controls.cpp
// Purpose:     Controls wxWindows sample
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "controls.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !defined( __WXMSW__ ) || defined( __WIN95__ )
#include "wx/spinbutt.h"
#endif
#include "wx/notebook.h"
#include "wx/imaglist.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #define USE_XPM
#endif

#ifdef USE_XPM
    #include "mondrian.xpm"
    #include "icons/choice.xpm"
    #include "icons/combo.xpm"
    #include "icons/list.xpm"
    #include "icons/radio.xpm"
    #include "icons/text.xpm"
    #include "icons/gauge.xpm"
#endif

#ifdef __WIN16__
    // Win16 doesn't have them
    #ifdef wxUSE_SPINBTN
    #undef wxUSE_SPINBTN
    #endif
    #define wxUSE_SPINBTN 0
#else
    #ifndef wxUSE_SPINBTN
    #define wxUSE_SPINBTN 1
    #endif
#endif // __WIN16__

#include "wx/progdlg.h"

// VZ: this is a temp. hack, will remove soon
#ifndef wxUSE_SPINCTRL
#define wxUSE_SPINCTRL 1
#endif

#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif // wxUSE_SPINCTRL

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit();
};

class MyPanel: public wxPanel
{
public:
    MyPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~MyPanel();

    void OnSize( wxSizeEvent& event );
    void OnListBox( wxCommandEvent &event );
    void OnListBoxDoubleClick( wxCommandEvent &event );
    void OnListBoxButtons( wxCommandEvent &event );
    void OnChoice( wxCommandEvent &event );
    void OnChoiceButtons( wxCommandEvent &event );
    void OnCombo( wxCommandEvent &event );
    void OnComboButtons( wxCommandEvent &event );
    void OnRadio( wxCommandEvent &event );
    void OnRadioButtons( wxCommandEvent &event );
    void OnSetFont( wxCommandEvent &event );
    void OnPageChanged( wxNotebookEvent &event );
    void OnPageChanging( wxNotebookEvent &event );
    void OnSliderUpdate( wxCommandEvent &event );
    void OnUpdateLabel( wxCommandEvent &event );
#if wxUSE_SPINBTN
    void OnSpinUp( wxSpinEvent &event );
    void OnSpinDown( wxSpinEvent &event );
    void OnSpinUpdate( wxSpinEvent &event );
    void OnUpdateShowProgress( wxUpdateUIEvent& event );
    void OnShowProgress( wxCommandEvent &event );
#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL
    void OnSpinCtrl(wxSpinEvent& event);
#endif // wxUSE_SPINCTRL

    void OnEnableAll(wxCommandEvent& event);
    void OnChangeColour(wxCommandEvent& event);

    wxListBox     *m_listbox,
                  *m_listboxSorted;
    wxChoice      *m_choice,
                  *m_choiceSorted;
    wxComboBox    *m_combo;
    wxRadioBox    *m_radio;
    wxGauge       *m_gauge;
    wxSlider      *m_slider;
    wxButton      *m_fontButton;
    wxButton      *m_lbSelectNum;
    wxButton      *m_lbSelectThis;
#if wxUSE_SPINBTN
    wxSpinButton  *m_spinbutton;
    wxButton      *m_btnProgress;
#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL
    wxSpinCtrl    *m_spinctrl;
#endif // wxUSE_SPINCTRL

    wxTextCtrl    *m_spintext;
    wxCheckBox    *m_checkbox;

    wxTextCtrl    *m_text;
    wxNotebook    *m_notebook;

    wxStaticText  *m_label;

private:
    DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

#if wxUSE_TOOLTIPS
    void OnSetTooltipDelay(wxCommandEvent& event);
    void OnToggleTooltips(wxCommandEvent& event);
#endif // wxUSE_TOOLTIPS

    void OnEnableAll(wxCommandEvent& event);

    void OnIdle( wxIdleEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnMove( wxMoveEvent& event );

private:
    void UpdateStatusBar(const wxPoint& pos, const wxSize& size)
    {
        wxString msg;
        msg.Printf(_("pos=(%d, %d), size=%dx%d"),
                   pos.x, pos.y, size.x, size.y);
        SetStatusText(msg, 1);
    }

    wxPanel *m_panel;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------
// other
//----------------------------------------------------------------------

static void SetControlClientData(const char *name,
                                 wxControlWithItems *control);

IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------

enum
{
    MINIMAL_QUIT   = 100,
    MINIMAL_TEXT,
    MINIMAL_ABOUT,

    // tooltip menu
    MINIMAL_SET_TOOLTIP_DELAY = 200,
    MINIMAL_ENABLE_TOOLTIPS,

    // panel menu
    MINIMAL_ENABLE_ALL
};

bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *) NULL,
            "Controls wxWindows App",
            50, 50, 530, 420);

    // Give it an icon
    // The wxICON() macros loads an icon from a resource under Windows
    // and uses an #included XPM image under GTK+ and Motif

    frame->SetIcon( wxICON(mondrian) );

    wxMenu *file_menu = new wxMenu("",  wxMENU_TEAROFF );
    file_menu->Append(MINIMAL_ABOUT, "&About\tF1");
    file_menu->AppendSeparator();
    file_menu->Append(MINIMAL_QUIT, "E&xit\tAlt-X", "Quit controls sample");

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");

#if wxUSE_TOOLTIPS
    wxMenu *tooltip_menu = new wxMenu;
    tooltip_menu->Append(MINIMAL_SET_TOOLTIP_DELAY, "Set &delay\tCtrl-D");
    tooltip_menu->AppendSeparator();
    tooltip_menu->Append(MINIMAL_ENABLE_TOOLTIPS, "&Toggle tooltips\tCtrl-T",
            "enable/disable tooltips", TRUE);
    tooltip_menu->Check(MINIMAL_ENABLE_TOOLTIPS, TRUE);
    menu_bar->Append(tooltip_menu, "&Tooltips");
#endif // wxUSE_TOOLTIPS

    wxMenu *panel_menu = new wxMenu;
    panel_menu->Append(MINIMAL_ENABLE_ALL, "&Disable all\tCtrl-E",
                       "Enable/disable all panel controls", TRUE);
    menu_bar->Append(panel_menu, "&Panel");

    frame->SetMenuBar(menu_bar);

    frame->Show(TRUE);
    frame->SetCursor(wxCursor(wxCURSOR_HAND));

    SetTopWindow(frame);

    return TRUE;
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

const int  ID_NOTEBOOK          = 1000;

const int  ID_LISTBOX           = 130;
const int  ID_LISTBOX_SEL_NUM   = 131;
const int  ID_LISTBOX_SEL_STR   = 132;
const int  ID_LISTBOX_CLEAR     = 133;
const int  ID_LISTBOX_APPEND    = 134;
const int  ID_LISTBOX_DELETE    = 135;
const int  ID_LISTBOX_FONT      = 136;
const int  ID_LISTBOX_ENABLE    = 137;
const int  ID_LISTBOX_SORTED    = 138;

const int  ID_CHOICE            = 120;
const int  ID_CHOICE_SEL_NUM    = 121;
const int  ID_CHOICE_SEL_STR    = 122;
const int  ID_CHOICE_CLEAR      = 123;
const int  ID_CHOICE_APPEND     = 124;
const int  ID_CHOICE_DELETE     = 125;
const int  ID_CHOICE_FONT       = 126;
const int  ID_CHOICE_ENABLE     = 127;
const int  ID_CHOICE_SORTED     = 128;

const int  ID_COMBO             = 140;
const int  ID_COMBO_SEL_NUM     = 141;
const int  ID_COMBO_SEL_STR     = 142;
const int  ID_COMBO_CLEAR       = 143;
const int  ID_COMBO_APPEND      = 144;
const int  ID_COMBO_DELETE      = 145;
const int  ID_COMBO_FONT        = 146;
const int  ID_COMBO_ENABLE      = 147;

const int  ID_RADIOBOX          = 160;
const int  ID_RADIOBOX_SEL_NUM  = 161;
const int  ID_RADIOBOX_SEL_STR  = 162;
const int  ID_RADIOBOX_FONT     = 163;
const int  ID_RADIOBOX_ENABLE   = 164;

const int  ID_RADIOBUTTON_1     = 166;
const int  ID_RADIOBUTTON_2     = 167;

const int  ID_SET_FONT          = 170;

const int  ID_GAUGE             = 180;
const int  ID_SLIDER            = 181;

const int  ID_SPIN              = 182;
const int  ID_BTNPROGRESS       = 183;
const int  ID_BUTTON_LABEL      = 184;
const int  ID_SPINCTRL          = 185;

const int  ID_CHANGE_COLOUR     = 200;

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
EVT_SIZE      (                         MyPanel::OnSize)
EVT_NOTEBOOK_PAGE_CHANGING(ID_NOTEBOOK, MyPanel::OnPageChanging)
EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK,  MyPanel::OnPageChanged)
EVT_LISTBOX   (ID_LISTBOX,              MyPanel::OnListBox)
EVT_LISTBOX   (ID_LISTBOX_SORTED,       MyPanel::OnListBox)
EVT_LISTBOX_DCLICK(ID_LISTBOX,          MyPanel::OnListBoxDoubleClick)
EVT_BUTTON    (ID_LISTBOX_SEL_NUM,      MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_SEL_STR,      MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_CLEAR,        MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_APPEND,       MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_DELETE,       MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_FONT,         MyPanel::OnListBoxButtons)
EVT_CHECKBOX  (ID_LISTBOX_ENABLE,       MyPanel::OnListBoxButtons)
EVT_CHOICE    (ID_CHOICE,               MyPanel::OnChoice)
EVT_CHOICE    (ID_CHOICE_SORTED,        MyPanel::OnChoice)
EVT_BUTTON    (ID_CHOICE_SEL_NUM,       MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_SEL_STR,       MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_CLEAR,         MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_APPEND,        MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_DELETE,        MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_FONT,          MyPanel::OnChoiceButtons)
EVT_CHECKBOX  (ID_CHOICE_ENABLE,        MyPanel::OnChoiceButtons)
EVT_COMBOBOX  (ID_COMBO,                MyPanel::OnCombo)
EVT_BUTTON    (ID_COMBO_SEL_NUM,        MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_SEL_STR,        MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_CLEAR,          MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_APPEND,         MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_DELETE,         MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_FONT,           MyPanel::OnComboButtons)
EVT_CHECKBOX  (ID_COMBO_ENABLE,         MyPanel::OnComboButtons)
EVT_RADIOBOX  (ID_RADIOBOX,             MyPanel::OnRadio)
EVT_BUTTON    (ID_RADIOBOX_SEL_NUM,     MyPanel::OnRadioButtons)
EVT_BUTTON    (ID_RADIOBOX_SEL_STR,     MyPanel::OnRadioButtons)
EVT_BUTTON    (ID_RADIOBOX_FONT,        MyPanel::OnRadioButtons)
EVT_CHECKBOX  (ID_RADIOBOX_ENABLE,      MyPanel::OnRadioButtons)
EVT_BUTTON    (ID_SET_FONT,             MyPanel::OnSetFont)
EVT_SLIDER    (ID_SLIDER,               MyPanel::OnSliderUpdate)
#if wxUSE_SPINBTN
EVT_SPIN      (ID_SPIN,                 MyPanel::OnSpinUpdate)
EVT_SPIN_UP   (ID_SPIN,                 MyPanel::OnSpinUp)
EVT_SPIN_DOWN (ID_SPIN,                 MyPanel::OnSpinDown)
EVT_UPDATE_UI (ID_BTNPROGRESS,          MyPanel::OnUpdateShowProgress)
EVT_BUTTON    (ID_BTNPROGRESS,          MyPanel::OnShowProgress)
#endif // wxUSE_SPINBTN
#if wxUSE_SPINCTRL
EVT_SPIN      (ID_SPINCTRL,             MyPanel::OnSpinCtrl)
#endif // wxUSE_SPINCTRL
EVT_BUTTON    (ID_BUTTON_LABEL,         MyPanel::OnUpdateLabel)
EVT_CHECKBOX  (ID_CHANGE_COLOUR,        MyPanel::OnChangeColour)
END_EVENT_TABLE()

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) ),
         m_text(NULL), m_notebook(NULL)
{
    m_text = new wxTextCtrl( this, -1, "This is the log window.\n", wxPoint(0,50), wxSize(100,50), wxTE_MULTILINE );
    //  m_text->SetBackgroundColour("wheat");

    delete wxLog::SetActiveTarget(new wxLogStderr);

    m_notebook = new wxNotebook( this, ID_NOTEBOOK, wxPoint(0,0), wxSize(200,150) );

    wxString choices[] =
    {
        "This",
        "is one of my",
        "really",
        "wonderful",
        "examples."
    };

#ifdef USE_XPM
    // image ids
    enum
    {
        Image_List, Image_Choice, Image_Combo, Image_Text, Image_Radio, Image_Gauge, Image_Max
    };

    // fill the image list
    wxImageList *imagelist = new wxImageList(32, 32);

    imagelist-> Add( wxBitmap( list_xpm ));
    imagelist-> Add( wxBitmap( choice_xpm ));
    imagelist-> Add( wxBitmap( combo_xpm ));
    imagelist-> Add( wxBitmap( text_xpm ));
    imagelist-> Add( wxBitmap( radio_xpm ));
    imagelist-> Add( wxBitmap( gauge_xpm ));
    m_notebook->SetImageList(imagelist);
#elif defined(__WXMSW__)
    // load images from resources
    enum
    {
        Image_List, Image_Choice, Image_Combo, Image_Text, Image_Radio, Image_Gauge, Image_Max
    };
    wxImageList *imagelist = new wxImageList(16, 16, FALSE, Image_Max);

    static const char *s_iconNames[Image_Max] =
    {
        "list", "choice", "combo", "text", "radio", "gauge"
    };

    for ( size_t n = 0; n < Image_Max; n++ )
    {
        wxBitmap bmp(s_iconNames[n]);
        if ( !bmp.Ok() || (imagelist->Add(bmp) == -1) )
        {
            wxLogWarning("Couldn't load the image '%s' for the notebook page %d.",
                    s_iconNames[n], n);
        }
    }

    m_notebook->SetImageList(imagelist);
#else

    // No images for now
#define    Image_List -1
#define    Image_Choice -1
#define    Image_Combo -1
#define    Image_Text -1
#define    Image_Radio -1
#define    Image_Gauge -1
#define    Image_Max -1

#endif

    wxPanel *panel = new wxPanel(m_notebook);
    m_listbox = new wxListBox( panel, ID_LISTBOX,
                               wxPoint(10,10), wxSize(120,70),
                               5, choices, wxLB_ALWAYS_SB );
    m_listboxSorted = new wxListBox( panel, ID_LISTBOX_SORTED,
                                     wxPoint(10,90), wxSize(120,70),
                                     5, choices, wxLB_SORT );

    SetControlClientData("listbox", m_listbox);
    SetControlClientData("listbox", m_listboxSorted);

    m_listbox->SetCursor(*wxCROSS_CURSOR);
#if wxUSE_TOOLTIPS
    m_listbox->SetToolTip( "This is a list box" );
#endif // wxUSE_TOOLTIPS

    m_lbSelectNum = new wxButton( panel, ID_LISTBOX_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
    m_lbSelectThis = new wxButton( panel, ID_LISTBOX_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
    wxButton *button = new wxButton( panel, ID_LISTBOX_FONT, "Set &Italic font", wxPoint(340,130), wxSize(140,30) );
#if wxUSE_TOOLTIPS
    button->SetToolTip( "Press here to set italic font" );
#endif // wxUSE_TOOLTIPS

    m_checkbox = new wxCheckBox( panel, ID_LISTBOX_ENABLE, "&Disable", wxPoint(20,170) );
    m_checkbox->SetValue(FALSE);
#if wxUSE_TOOLTIPS
    m_checkbox->SetToolTip( "Click here to disable the listbox" );
#endif // wxUSE_TOOLTIPS
    (void)new wxCheckBox( panel, ID_CHANGE_COLOUR, "&Toggle colour",
                          wxPoint(110,170) );
    m_notebook->AddPage(panel, "wxListBox", TRUE, Image_List);

    panel = new wxPanel(m_notebook);
    m_choice = new wxChoice( panel, ID_CHOICE, wxPoint(10,10), wxSize(120,-1), 5, choices );
    m_choiceSorted = new wxChoice( panel, ID_CHOICE_SORTED, wxPoint(10,70), wxSize(120,-1),
                                   5, choices, wxCB_SORT );

    SetControlClientData("choice", m_choice);
    SetControlClientData("choice", m_choiceSorted);

    m_choice->SetSelection(2);
    m_choice->SetBackgroundColour( "red" );
    (void)new wxButton( panel, ID_CHOICE_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_FONT, "Set Italic font", wxPoint(340,130), wxSize(140,30) );
    (void)new wxCheckBox( panel, ID_CHOICE_ENABLE, "Disable", wxPoint(20,130), wxSize(140,30) );

    m_notebook->AddPage(panel, "wxChoice", FALSE, Image_Choice);

    panel = new wxPanel(m_notebook);
    m_combo = new wxComboBox( panel, ID_COMBO, "This", wxPoint(10,10), wxSize(120,-1), 5, choices, wxCB_READONLY );
    (void)new wxButton( panel, ID_COMBO_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_FONT, "Set Italic font", wxPoint(340,130), wxSize(140,30) );
    (void)new wxCheckBox( panel, ID_COMBO_ENABLE, "Disable", wxPoint(20,130), wxSize(140,30) );
    m_notebook->AddPage(panel, "wxComboBox", FALSE, Image_Combo);

    wxString choices2[] =
    {
        "First", "Second",
        /* "Third",
        "Fourth", "Fifth", "Sixth",
        "Seventh", "Eighth", "Nineth", "Tenth" */
    };

    panel = new wxPanel(m_notebook);
    (void)new wxRadioBox( panel, ID_RADIOBOX, "That", wxPoint(10,160), wxSize(-1,-1), WXSIZEOF(choices2), choices2, 1, wxRA_SPECIFY_ROWS );
    m_radio = new wxRadioBox( panel, ID_RADIOBOX, "This", wxPoint(10,10), wxSize(-1,-1), WXSIZEOF(choices), choices, 1, wxRA_SPECIFY_COLS );
    (void)new wxButton( panel, ID_RADIOBOX_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_RADIOBOX_SEL_STR, "Select 'This'", wxPoint(180,80), wxSize(140,30) );
    m_fontButton = new wxButton( panel, ID_SET_FONT, "Set more Italic font", wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_RADIOBOX_FONT, "Set Italic font", wxPoint(340,80), wxSize(140,30) );
    (void)new wxCheckBox( panel, ID_RADIOBOX_ENABLE, "Disable", wxPoint(340,130), wxSize(140,30) );
    wxRadioButton *rb = new wxRadioButton( panel, ID_RADIOBUTTON_1, "Radiobutton1", wxPoint(210,170), wxSize(110,30), wxRB_GROUP );
    rb->SetValue( FALSE );
    (void)new wxRadioButton( panel, ID_RADIOBUTTON_2, "Radiobutton2", wxPoint(340,170), wxSize(110,30) );
    m_notebook->AddPage(panel, "wxRadioBox", FALSE, Image_Radio);

    panel = new wxPanel(m_notebook);
    (void)new wxStaticBox( panel, -1, "wxGauge and wxSlider", wxPoint(10,10), wxSize(200,130) );
    m_gauge = new wxGauge( panel, -1, 200, wxPoint(18,50), wxSize(155, 30) );
    m_slider = new wxSlider( panel, ID_SLIDER, 0, 0, 200, wxPoint(18,90), wxSize(155,-1), wxSL_LABELS );
    (void)new wxStaticBox( panel, -1, "Explanation", wxPoint(220,10), wxSize(270,130) );
#ifdef __WXMOTIF__
    // No wrapping text in wxStaticText yet :-(
    (void)new wxStaticText( panel, -1,
                            "Drag the slider!",
                            wxPoint(228,30),
                            wxSize(240, -1)
                          );
#else
    (void)new wxStaticText( panel, -1,
                            "In order see the gauge (aka progress bar)\n"
                            "control do something you have to drag the\n"
                            "handle of the slider to the right.\n"
                            "\n"
                            "This is also supposed to demonstrate how\n"
                            "to use static controls.\n",
                            wxPoint(228,25),
                            wxSize(240, 110)
                          );
#endif
    int initialSpinValue = -5;
    wxString s;
    s << initialSpinValue;
    m_spintext = new wxTextCtrl( panel, -1, s, wxPoint(20,160), wxSize(80,-1) );
#if wxUSE_SPINBTN
    m_spinbutton = new wxSpinButton( panel, ID_SPIN, wxPoint(103,160), wxSize(80, -1) );
    m_spinbutton->SetRange(-10,30);
    m_spinbutton->SetValue(initialSpinValue);

    m_btnProgress = new wxButton( panel, ID_BTNPROGRESS, "Show progress dialog",
                                  wxPoint(300, 160) );
#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL
    m_spinctrl = new wxSpinCtrl( panel, ID_SPINCTRL, "", wxPoint(200, 160), wxSize(80, -1) );
    m_spinctrl->SetRange(10,30);
    m_spinctrl->SetValue(15);
#endif // wxUSE_SPINCTRL

    m_notebook->AddPage(panel, "wxGauge", FALSE, Image_Gauge);

    panel = new wxPanel(m_notebook);

#ifndef __WXMOTIF__  // wxStaticBitmap not working under Motif yet. MB
    wxIcon icon = wxTheApp->GetStdIcon(wxICON_INFORMATION);
    wxStaticBitmap *bmpStatic = new wxStaticBitmap(panel, -1, icon, wxPoint(10, 10));

    bmpStatic = new wxStaticBitmap(panel, -1, wxNullIcon, wxPoint(50, 10));
    bmpStatic->SetIcon(wxTheApp->GetStdIcon(wxICON_QUESTION));
#endif // !Motif

    wxBitmap bitmap( 100, 100 );
    wxMemoryDC dc;
    dc.SelectObject( bitmap );
    dc.SetPen(*wxGREEN_PEN);
    dc.DrawEllipse(5, 5, 90, 90);
    dc.DrawText("Bitmap", 20, 20);
    dc.SelectObject( wxNullBitmap );

    (void)new wxBitmapButton(panel, -1, bitmap, wxPoint(100, 20));
    (void)new wxButton(panel, ID_BUTTON_LABEL, "Toggle label", wxPoint(250, 20));
    m_label = new wxStaticText(panel, -1, "Label with some long text",
                               wxPoint(250, 60), wxDefaultSize,
                               wxALIGN_RIGHT | wxST_NO_AUTORESIZE);

    m_notebook->AddPage(panel, "wxBitmapXXX");

// --------------- TEST CODE ----------------------

  // layout constraints

  panel = new wxPanel(m_notebook);
  panel->SetAutoLayout( TRUE );

  wxLayoutConstraints *c;
  c = new wxLayoutConstraints;
  c->top.SameAs( panel, wxTop, 10 );
  c->height.AsIs( );
  c->left.SameAs( panel, wxLeft, 10 );
  c->width.PercentOf( panel, wxWidth, 40 );

  wxButton *pMyButton = new wxButton(panel, -1, "Test Button" );
  pMyButton->SetConstraints( c );

  c = new wxLayoutConstraints;
  c->top.SameAs( panel, wxTop, 10 );
  c->bottom.SameAs( panel, wxBottom, 10 );
  c->right.SameAs( panel, wxRight, 10 );
  c->width.PercentOf( panel, wxWidth, 40 );

  wxButton *pMyButton2 = new wxButton(panel, -1, "Test Button 2" );
  pMyButton2->SetConstraints( c );

  m_notebook->AddPage(panel, "wxLayoutConstraint");

  // sizer

  panel = new wxPanel(m_notebook);
  panel->SetAutoLayout( TRUE );

  wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );

  sizer->Add( new wxButton(panel, -1, "Test Button" ), 3, wxALL, 10 );
  sizer->Add( 20,20, 1 );
  sizer->Add( new wxButton(panel, -1, "Test Button 2" ), 3, wxGROW|wxALL, 10 );

  panel->SetSizer( sizer );

  m_notebook->AddPage(panel, "wxSizer");

// --------------- TEST CODE ----------------------

}

void MyPanel::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    int x = 0;
    int y = 0;
    GetClientSize( &x, &y );

    if (m_notebook) m_notebook->SetSize( 2, 2, x-4, y*2/3-4 );
    if (m_text) m_text->SetSize( 2, y*2/3+2, x-4, y/3-4 );
}

void MyPanel::OnPageChanging( wxNotebookEvent &event )
{
    int selOld = event.GetOldSelection();
    if ( selOld == 2 )
    {
        if ( wxMessageBox("This demonstrates how a program may prevent the\n"
                          "page change from taking place - if you select\n"
                          "[No] the current page will stay the third one\n",
                          "Control sample",
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();

            return;
        }
    }

    *m_text << "Notebook selection is being changed from " << selOld << "\n";
}

void MyPanel::OnPageChanged( wxNotebookEvent &event )
{
    *m_text << "Notebook selection is " << event.GetSelection() << "\n";
}

void MyPanel::OnChangeColour(wxCommandEvent& WXUNUSED(event))
{
    static wxColour s_colOld;

    // test panel colour changing and propagation to the subcontrols
    if ( s_colOld.Ok() )
    {
        SetBackgroundColour(s_colOld);
        s_colOld = wxNullColour;

        m_lbSelectThis->SetBackgroundColour("blue");
    }
    else
    {
        s_colOld = GetBackgroundColour();
        SetBackgroundColour("green");

        m_lbSelectThis->SetBackgroundColour("red");
    }

    m_lbSelectThis->Refresh();
    Refresh();
}

void MyPanel::OnListBox( wxCommandEvent &event )
{
    wxListBox *listbox = event.GetId() == ID_LISTBOX ? m_listbox
                                                     : m_listboxSorted;

    m_text->AppendText( "ListBox event selection string is: '" );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "'\n" );
    m_text->AppendText( "ListBox control selection string is: '" );
    m_text->AppendText( listbox->GetStringSelection() );
    m_text->AppendText( "'\n" );

    wxStringClientData *obj = ((wxStringClientData *)event.GetClientObject());
    m_text->AppendText( "ListBox event client data string is: '" );
    if (obj) // BC++ doesn't like use of '? .. : .. ' in this context
    	m_text->AppendText( obj->GetData() );
    else
    	m_text->AppendText( wxString("none") );

    m_text->AppendText( "'\n" );
    m_text->AppendText( "ListBox control client data string is: '" );
    obj = (wxStringClientData *)listbox->GetClientObject(listbox->GetSelection());
    if (obj)
    	m_text->AppendText( obj->GetData() );
    else
    	m_text->AppendText( wxString("none") );
    m_text->AppendText( "'\n" );
}

void MyPanel::OnListBoxDoubleClick( wxCommandEvent &event )
{
    m_text->AppendText( "ListBox double click string is: " );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "\n" );
}

void MyPanel::OnListBoxButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_LISTBOX_ENABLE:
            {
                m_text->AppendText("Checkbox clicked.\n");
                wxCheckBox *cb = (wxCheckBox*)event.GetEventObject();
#if wxUSE_TOOLTIPS
                if (event.GetInt())
                    cb->SetToolTip( "Click to enable listbox" );
                else
                    cb->SetToolTip( "Click to disable listbox" );
#endif // wxUSE_TOOLTIPS
                m_listbox->Enable( event.GetInt() == 0 );
                m_listboxSorted->Enable( event.GetInt() == 0 );
                break;
            }
        case ID_LISTBOX_SEL_NUM:
            {
                m_listbox->SetSelection( 2 );
                m_listboxSorted->SetSelection( 2 );
                m_lbSelectThis->WarpPointer( 40, 14 );
                break;
            }
        case ID_LISTBOX_SEL_STR:
            {
                m_listbox->SetStringSelection( "This" );
                m_listboxSorted->SetStringSelection( "This" );
                m_lbSelectNum->WarpPointer( 40, 14 );
                break;
            }
        case ID_LISTBOX_CLEAR:
            {
                m_listbox->Clear();
                m_listboxSorted->Clear();
                break;
            }
        case ID_LISTBOX_APPEND:
            {
                m_listbox->Append( "Hi!" );
                m_listboxSorted->Append( "Hi!" );
                break;
            }
        case ID_LISTBOX_DELETE:
            {
                int idx;
                idx = m_listbox->GetSelection();
                m_listbox->Delete( idx );
                idx = m_listboxSorted->GetSelection();
                m_listboxSorted->Delete( idx );
                break;
            }
        case ID_LISTBOX_FONT:
            {
                m_listbox->SetFont( *wxITALIC_FONT );
                m_listboxSorted->SetFont( *wxITALIC_FONT );
                m_checkbox->SetFont( *wxITALIC_FONT );
                break;
            }
    }
}

void MyPanel::OnChoice( wxCommandEvent &event )
{
    wxChoice *choice = event.GetId() == ID_CHOICE ? m_choice
                                                  : m_choiceSorted;

    m_text->AppendText( "Choice event selection string is: '" );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "'\n" );
    m_text->AppendText( "Choice control selection string is: '" );
    m_text->AppendText( choice->GetStringSelection() );
    m_text->AppendText( "'\n" );

    wxStringClientData *obj = ((wxStringClientData *)event.GetClientObject());
    m_text->AppendText( "Choice event client data string is: '" );

    if (obj)
       m_text->AppendText( obj->GetData() );
    else
       m_text->AppendText( wxString("none") );

    m_text->AppendText( "'\n" );
    m_text->AppendText( "Choice control client data string is: '" );
    obj = (wxStringClientData *)choice->GetClientObject(choice->GetSelection());

    if (obj)
       m_text->AppendText( obj->GetData() );
    else
       m_text->AppendText( wxString("none") );
    m_text->AppendText( "'\n" );
}

void MyPanel::OnChoiceButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_CHOICE_ENABLE:
            {
                m_choice->Enable( event.GetInt() == 0 );
                m_choiceSorted->Enable( event.GetInt() == 0 );
                break;
            }
        case ID_CHOICE_SEL_NUM:
            {
                m_choice->SetSelection( 2 );
                m_choiceSorted->SetSelection( 2 );
                break;
            }
        case ID_CHOICE_SEL_STR:
            {
                m_choice->SetStringSelection( "This" );
                m_choiceSorted->SetStringSelection( "This" );
                break;
            }
        case ID_CHOICE_CLEAR:
            {
                m_choice->Clear();
                m_choiceSorted->Clear();
                break;
            }
        case ID_CHOICE_APPEND:
            {
                m_choice->Append( "Hi!" );
                m_choiceSorted->Append( "Hi!" );
                break;
            }
        case ID_CHOICE_DELETE:
            {
                int idx = m_choice->GetSelection();
                m_choice->Delete( idx );
                idx = m_choiceSorted->GetSelection();
                m_choiceSorted->Delete( idx );
                break;
            }
        case ID_CHOICE_FONT:
            {
                m_choice->SetFont( *wxITALIC_FONT );
                m_choiceSorted->SetFont( *wxITALIC_FONT );
                break;
            }
    }
}

void MyPanel::OnCombo( wxCommandEvent &event )
{
    m_text->AppendText( "ComboBox event selection string is: " );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "\n" );
    m_text->AppendText( "ComboBox control selection string is: " );
    m_text->AppendText( m_combo->GetStringSelection() );
    m_text->AppendText( "\n" );
}

void MyPanel::OnComboButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_COMBO_ENABLE:
            {
                m_combo->Enable( event.GetInt() == 0 );
                break;
            }
        case ID_COMBO_SEL_NUM:
            {
                m_combo->SetSelection( 2 );
                break;
            }
        case ID_COMBO_SEL_STR:
            {
                m_combo->SetStringSelection( "This" );
                break;
            }
        case ID_COMBO_CLEAR:
            {
                m_combo->Clear();
                break;
            }
        case ID_COMBO_APPEND:
            {
                m_combo->Append( "Hi!" );
                break;
            }
        case ID_COMBO_DELETE:
            {
                int idx = m_combo->GetSelection();
                m_combo->Delete( idx );
                break;
            }
        case ID_COMBO_FONT:
            {
                m_combo->SetFont( *wxITALIC_FONT );
                break;
            }
    }
}

void MyPanel::OnRadio( wxCommandEvent &event )
{
    m_text->AppendText( "RadioBox selection string is: " );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "\n" );
}

void MyPanel::OnRadioButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_RADIOBOX_ENABLE:
            {
                m_radio->Enable( event.GetInt() == 0 );
                break;
            }
        case ID_RADIOBOX_SEL_NUM:
            {
                m_radio->SetSelection( 2 );
                break;
            }
        case ID_RADIOBOX_SEL_STR:
            {
                m_radio->SetStringSelection( "This" );
                break;
            }
        case ID_RADIOBOX_FONT:
            {
                m_radio->SetFont( *wxITALIC_FONT );
                break;
            }
    }
}

void MyPanel::OnSetFont( wxCommandEvent &WXUNUSED(event) )
{
    m_fontButton->SetFont( *wxITALIC_FONT );
    m_text->SetFont( *wxITALIC_FONT );
}

void MyPanel::OnUpdateLabel( wxCommandEvent &WXUNUSED(event) )
{
    static bool s_long = TRUE;

    s_long = !s_long;
    m_label->SetLabel(s_long ? "very very very long text" : "shorter text");
}

void MyPanel::OnSliderUpdate( wxCommandEvent &WXUNUSED(event) )
{
    m_gauge->SetValue( m_slider->GetValue() );
}

#if wxUSE_SPINCTRL

void MyPanel::OnSpinCtrl(wxSpinEvent& event)
{
    wxString s;
    s.Printf(_T("Current value of spin ctrl is %d\n"), m_spinctrl->GetValue());
    m_text->AppendText(s);
}

#endif // wxUSE_SPINCTRL

#if wxUSE_SPINBTN
void MyPanel::OnSpinUp( wxSpinEvent &event )
{
    wxString value;
    value.Printf( _T("Spin control up: current = %d\n"),
                 m_spinbutton->GetValue());

    if ( m_spinbutton->GetValue() > 17 )
    {
        value += _T("Preventing the spin button from going above 17.\n");

        event.Veto();
    }

    m_text->AppendText(value);
}

void MyPanel::OnSpinDown( wxSpinEvent &event )
{
    wxString value;
    value.Printf( _T("Spin control down: current = %d\n"),
                 m_spinbutton->GetValue());

    if ( m_spinbutton->GetValue() < -17 )
    {
        value += _T("Preventing the spin button from going below -17.\n");

        event.Veto();
    }

    m_text->AppendText(value);
}

void MyPanel::OnSpinUpdate( wxSpinEvent &event )
{
    wxString value;
    value.Printf( _T("%d"), event.GetPosition() );
    m_spintext->SetValue( value );

    value.Printf( _T("Spin control range: (%d, %d), current = %d\n"),
                 m_spinbutton->GetMin(), m_spinbutton->GetMax(),
                 m_spinbutton->GetValue());

    m_text->AppendText(value);
}

void MyPanel::OnUpdateShowProgress( wxUpdateUIEvent& event )
{
    event.Enable( m_spinbutton->GetValue() > 0 );
}

void MyPanel::OnShowProgress( wxCommandEvent& WXUNUSED(event) )
{
    int max = m_spinbutton->GetValue();
    wxProgressDialog dialog("Progress dialog example",
                            "An informative message",
                            max,    // range
                            this,   // parent
                            wxPD_CAN_ABORT |
                            wxPD_APP_MODAL |
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME);


    bool cont = TRUE;
    for ( int i = 0; i < max && cont; i++ )
    {
        wxSleep(1);
        if ( i == max - 1 )
        {
            cont = dialog.Update(i, "That's all, folks!");
        }
        else if ( i == max / 2 )
        {
            cont = dialog.Update(i, "Only a half left!");
        }
        else
        {
            cont = dialog.Update(i);
        }
    }

    if ( !cont )
    {
        *m_text << "Progress dialog aborted!\n";
    }
    else
    {
        *m_text << "Countdown from " << max << " finished.\n";
    }
}

#endif // wxUSE_SPINBTN

MyPanel::~MyPanel()
{
    delete m_notebook->GetImageList();
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MINIMAL_QUIT,   MyFrame::OnQuit)
    EVT_MENU(MINIMAL_ABOUT,  MyFrame::OnAbout)
#if wxUSE_TOOLTIPS
    EVT_MENU(MINIMAL_SET_TOOLTIP_DELAY,  MyFrame::OnSetTooltipDelay)
    EVT_MENU(MINIMAL_ENABLE_TOOLTIPS,  MyFrame::OnToggleTooltips)
#endif // wxUSE_TOOLTIPS

    EVT_MENU(MINIMAL_ENABLE_ALL, MyFrame::OnEnableAll)

    EVT_SIZE(MyFrame::OnSize)
    EVT_MOVE(MyFrame::OnMove)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
: wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    CreateStatusBar(2);

    m_panel = new MyPanel( this, 10, 10, 300, 100 );
}

void MyFrame::OnQuit (wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxBeginBusyCursor();

    wxMessageDialog dialog(this, "This is a control sample", "About Controls", wxOK );
    dialog.ShowModal();

    wxEndBusyCursor();
}

#if wxUSE_TOOLTIPS
void MyFrame::OnSetTooltipDelay(wxCommandEvent& event)
{
    static long s_delay = 5000;

    wxString delay;
    delay.Printf( _T("%ld"), s_delay);

    delay = wxGetTextFromUser("Enter delay (in milliseconds)",
            "Set tooltip delay",
            delay,
            this);
    if ( !delay )
        return; // cancelled

    wxSscanf(delay, _T("%ld"), &s_delay);

    wxToolTip::SetDelay(s_delay);

    wxLogStatus(this, _T("Tooltip delay set to %ld milliseconds"), s_delay);
}

void MyFrame::OnToggleTooltips(wxCommandEvent& event)
{
    static bool s_enabled = TRUE;

    s_enabled = !s_enabled;

    wxToolTip::Enable(s_enabled);

    wxLogStatus(this, _T("Tooltips %sabled"), s_enabled ? _T("en") : _T("dis") );
}
#endif // tooltips

void MyFrame::OnEnableAll(wxCommandEvent& WXUNUSED(event))
{
    static bool s_enable = TRUE;

    s_enable = !s_enable;
    m_panel->Enable(s_enable);
}

void MyFrame::OnMove( wxMoveEvent& event )
{
    UpdateStatusBar(event.GetPosition(), GetSize());

    event.Skip();
}

void MyFrame::OnSize( wxSizeEvent& event )
{
    UpdateStatusBar(GetPosition(), event.GetSize());

    event.Skip();
}

void MyFrame::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    // track the window which has the focus in the status bar
    static wxWindow *s_windowFocus = (wxWindow *)NULL;
    wxWindow *focus = wxWindow::FindFocus();
    if ( focus && (focus != s_windowFocus) )
    {
        s_windowFocus = focus;

        wxString msg;
        msg.Printf(
#ifdef __WXMSW__
                _T("Focus: wxWindow = %p, HWND = %08x"),
#else
                _T("Focus: wxWindow = %p"),
#endif
                s_windowFocus
#ifdef __WXMSW__
                , s_windowFocus->GetHWND()
#endif
                  );

        SetStatusText(msg);
    }
}

static void SetControlClientData(const char *name,
                                 wxControlWithItems *control)
{
    size_t count = control->GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxString s;
        s.Printf("%s client data for '%s'",
                 name, control->GetString(n).c_str());

        control->SetClientObject(n, new wxStringClientData(s));
    }
}
