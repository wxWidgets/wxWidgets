/////////////////////////////////////////////////////////////////////////////
// Name:        controls.cpp
// Purpose:     Controls wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/spinbutt.h"
#include "wx/tglbtn.h"
#include "wx/bookctrl.h"
#include "wx/imaglist.h"
#include "wx/artprov.h"
#include "wx/cshelp.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
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

#ifndef wxUSE_SPINBTN
    #define wxUSE_SPINBTN 1
#endif

#include "wx/progdlg.h"

#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif // wxUSE_SPINCTRL

#if !wxUSE_TOGGLEBTN
    #define wxToggleButton wxCheckBox
    #define EVT_TOGGLEBUTTON EVT_CHECKBOX
#endif

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

    void OnIdle( wxIdleEvent &event );
    void OnListBox( wxCommandEvent &event );
    void OnListBoxDoubleClick( wxCommandEvent &event );
    void OnListBoxButtons( wxCommandEvent &event );
#if wxUSE_CHOICE
    void OnChoice( wxCommandEvent &event );
    void OnChoiceButtons( wxCommandEvent &event );
#endif
    void OnCombo( wxCommandEvent &event );
    void OnComboTextChanged( wxCommandEvent &event );
    void OnComboTextEnter( wxCommandEvent &event );
    void OnComboButtons( wxCommandEvent &event );
    void OnRadio( wxCommandEvent &event );
    void OnRadioButtons( wxCommandEvent &event );
    void OnRadioButton1( wxCommandEvent &event );
    void OnRadioButton2( wxCommandEvent &event );
    void OnSetFont( wxCommandEvent &event );
    void OnPageChanged( wxBookCtrlEvent &event );
    void OnPageChanging( wxBookCtrlEvent &event );
    void OnSliderUpdate( wxCommandEvent &event );
    void OnUpdateLabel( wxCommandEvent &event );
#if wxUSE_SPINBTN
    void OnSpinUp( wxSpinEvent &event );
    void OnSpinDown( wxSpinEvent &event );
    void OnSpinUpdate( wxSpinEvent &event );
#if wxUSE_PROGRESSDLG
    void OnUpdateShowProgress( wxUpdateUIEvent& event );
    void OnShowProgress( wxCommandEvent &event );
#endif // wxUSE_PROGRESSDLG
#endif // wxUSE_SPINBTN
    void OnNewText( wxCommandEvent &event );
#if wxUSE_SPINCTRL
    void OnSpinCtrl(wxSpinEvent& event);
    void OnSpinCtrlUp(wxSpinEvent& event);
    void OnSpinCtrlDown(wxSpinEvent& event);
    void OnSpinCtrlText(wxCommandEvent& event);
#endif // wxUSE_SPINCTRL

    void OnEnableAll(wxCommandEvent& event);
    void OnChangeColour(wxCommandEvent& event);
    void OnTestButton(wxCommandEvent& event);
    void OnBmpButton(wxCommandEvent& event);
    void OnBmpButtonToggle(wxCommandEvent& event);

    void OnSizerCheck (wxCommandEvent &event);

    wxListBox     *m_listbox,
                  *m_listboxSorted;
#if wxUSE_CHOICE
    wxChoice      *m_choice,
                  *m_choiceSorted;
#endif // wxUSE_CHOICE

    wxComboBox    *m_combo;
    wxRadioBox    *m_radio;
#if wxUSE_GAUGE
    wxGauge       *m_gauge,
                  *m_gaugeVert;
#endif // wxUSE_GAUGE
#if wxUSE_SLIDER
    wxSlider      *m_slider;
#endif // wxUSE_SLIDER
    wxButton      *m_fontButton;
    wxButton      *m_lbSelectNum;
    wxButton      *m_lbSelectThis;
#if wxUSE_SPINBTN
    wxSpinButton  *m_spinbutton;
#if wxUSE_PROGRESSDLG
    wxButton      *m_btnProgress;
#endif // wxUSE_PROGRESSDLG
#endif // wxUSE_SPINBTN
    wxStaticText  *m_wrappingText;
    wxStaticText  *m_nonWrappingText;

#if wxUSE_SPINCTRL
    wxSpinCtrl    *m_spinctrl;
#endif // wxUSE_SPINCTRL

    wxTextCtrl    *m_spintext;
    wxCheckBox    *m_checkbox;

    wxTextCtrl    *m_text;
    wxBookCtrl    *m_book;

    wxStaticText  *m_label;

    wxBoxSizer    *m_buttonSizer;
    wxButton      *m_sizerBtn1;
    wxButton      *m_sizerBtn2;
    wxButton      *m_sizerBtn3;
    wxButton      *m_sizerBtn4;
    wxBoxSizer    *m_hsizer;
    wxButton      *m_bigBtn;

private:
    wxLog *m_logTargetOld;

    DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxChar *title, int x, int y);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClearLog(wxCommandEvent& event);

#if wxUSE_TOOLTIPS
    void OnSetTooltipDelay(wxCommandEvent& event);
    void OnToggleTooltips(wxCommandEvent& event);
#endif // wxUSE_TOOLTIPS

    void OnEnableAll(wxCommandEvent& event);
    void OnHideAll(wxCommandEvent& event);
    void OnHideList(wxCommandEvent& event);
    void OnContextHelp(wxCommandEvent& event);

    void OnIdle( wxIdleEvent& event );
    void OnIconized( wxIconizeEvent& event );
    void OnMaximized( wxMaximizeEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnMove( wxMoveEvent& event );

    MyPanel *GetPanel() const { return m_panel; }

private:
#if wxUSE_STATUSBAR
    void UpdateStatusBar(const wxPoint& pos, const wxSize& size)
    {
        if ( m_frameStatusBar )
        {
            wxString msg;
            wxSize sizeAll = GetSize(),
                   sizeCl = GetClientSize();
            msg.Printf(_("pos=(%d, %d), size=%dx%d or %dx%d (client=%dx%d)"),
                       pos.x, pos.y,
                       size.x, size.y,
                       sizeAll.x, sizeAll.y,
                       sizeCl.x, sizeCl.y);
            SetStatusText(msg, 1);
        }
    }
#endif // wxUSE_STATUSBAR

    MyPanel *m_panel;

    DECLARE_EVENT_TABLE()
};

// a button which intercepts double clicks (for testing...)
class MyButton : public wxButton
{
public:
    MyButton(wxWindow *parent,
             wxWindowID id,
             const wxString& label = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize)
        : wxButton(parent, id, label, pos, size)
    {
    }

    void OnDClick(wxMouseEvent& event)
    {
        wxLogMessage(_T("MyButton::OnDClick"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

// a combo which intercepts chars (to test Windows behaviour)
class MyComboBox : public wxComboBox
{
public:
    MyComboBox(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
        : wxComboBox(parent, id, value, pos, size, n, choices, style,
                     validator, name) { }

protected:
    void OnChar(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnFocusGot(wxFocusEvent& event)
    {
        wxLogMessage(_T("MyComboBox::OnFocusGot"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

// a radiobox which handles focus set/kill (for testing)
class MyRadioBox : public wxRadioBox
{
public:
    MyRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = NULL,
               int majorDim = 1,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr)
        : wxRadioBox(parent, id, title, pos, size, n, choices, majorDim,
                     style, validator, name)
    {
    }

protected:
    void OnFocusGot(wxFocusEvent& event)
    {
        wxLogMessage(_T("MyRadioBox::OnFocusGot"));

        event.Skip();
    }

    void OnFocusLost(wxFocusEvent& event)
    {
        wxLogMessage(_T("MyRadioBox::OnFocusLost"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};

// a choice which handles focus set/kill (for testing)
class MyChoice : public wxChoice
{
public:
    MyChoice(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxChoiceNameStr )
        : wxChoice(parent, id, pos, size, n, choices,
                     style, validator, name) { }

protected:
    void OnFocusGot(wxFocusEvent& event)
    {
        wxLogMessage(_T("MyChoice::OnFocusGot"));

        event.Skip();
    }

    void OnFocusLost(wxFocusEvent& event)
    {
        wxLogMessage(_T("MyChoice::OnFocusLost"));

        event.Skip();
    }

private:
    DECLARE_EVENT_TABLE()
};



//----------------------------------------------------------------------
// other
//----------------------------------------------------------------------

static void SetListboxClientData(const wxChar *name, wxListBox *control);

#if wxUSE_CHOICE
static void SetChoiceClientData(const wxChar *name, wxChoice *control);
#endif // wxUSE_CHOICE

IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------

enum
{
    CONTROLS_QUIT   = wxID_EXIT,
    CONTROLS_ABOUT = wxID_ABOUT,
    CONTROLS_TEXT = 100,
    CONTROLS_CLEAR_LOG,

    // tooltip menu
    CONTROLS_SET_TOOLTIP_DELAY = 200,
    CONTROLS_ENABLE_TOOLTIPS,

    // panel menu
    CONTROLS_ENABLE_ALL,
    CONTROLS_HIDE_ALL,
    CONTROLS_HIDE_LIST,
    CONTROLS_CONTEXT_HELP
};

bool MyApp::OnInit()
{
    // use standard command line handling:
    if ( !wxApp::OnInit() )
        return false;

    // parse the cmd line
    int x = 50,
        y = 50;
    if ( argc == 3 )
    {
        wxSscanf(wxString(argv[1]), wxT("%d"), &x);
        wxSscanf(wxString(argv[2]), wxT("%d"), &y);
    }

#if wxUSE_HELP
    wxHelpProvider::Set( new wxSimpleHelpProvider );
#endif // wxUSE_HELP

    // Create the main frame window
    MyFrame *frame = new MyFrame(_T("Controls wxWidgets App"), x, y);
    frame->Show(true);

    return true;
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

const int  ID_BOOK              = 1000;

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
const int  ID_COMBO_SET_TEXT    = 148;

const int  ID_RADIOBOX          = 160;
const int  ID_RADIOBOX_SEL_NUM  = 161;
const int  ID_RADIOBOX_SEL_STR  = 162;
const int  ID_RADIOBOX_FONT     = 163;
const int  ID_RADIOBOX_ENABLE   = 164;

const int  ID_RADIOBUTTON_1     = 166;
const int  ID_RADIOBUTTON_2     = 167;

const int  ID_SET_FONT          = 170;

#if wxUSE_GAUGE
const int  ID_GAUGE             = 180;
#endif // wxUSE_GAUGE

#if wxUSE_SLIDER
const int  ID_SLIDER            = 181;
#endif // wxUSE_SLIDER

const int  ID_SPIN              = 182;
#if wxUSE_PROGRESSDLG
const int  ID_BTNPROGRESS       = 183;
#endif // wxUSE_PROGRESSDLG
const int  ID_BUTTON_LABEL      = 184;
const int  ID_SPINCTRL          = 185;
const int  ID_BTNNEWTEXT        = 186;

const int  ID_BUTTON_TEST1      = 190;
const int  ID_BUTTON_TEST2      = 191;
const int  ID_BITMAP_BTN        = 192;
const int  ID_BITMAP_BTN_ENABLE = 193;

const int  ID_CHANGE_COLOUR     = 200;

const int  ID_SIZER_CHECK1      = 201;
const int  ID_SIZER_CHECK2      = 202;
const int  ID_SIZER_CHECK3      = 203;
const int  ID_SIZER_CHECK4      = 204;
const int  ID_SIZER_CHECK14     = 205;
const int  ID_SIZER_CHECKBIG    = 206;

const int  ID_HYPERLINK         = 300;

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
EVT_IDLE      (                         MyPanel::OnIdle)
EVT_BOOKCTRL_PAGE_CHANGING(ID_BOOK,     MyPanel::OnPageChanging)
EVT_BOOKCTRL_PAGE_CHANGED(ID_BOOK,      MyPanel::OnPageChanged)
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
#if wxUSE_CHOICE
EVT_CHOICE    (ID_CHOICE,               MyPanel::OnChoice)
EVT_CHOICE    (ID_CHOICE_SORTED,        MyPanel::OnChoice)
EVT_BUTTON    (ID_CHOICE_SEL_NUM,       MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_SEL_STR,       MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_CLEAR,         MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_APPEND,        MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_DELETE,        MyPanel::OnChoiceButtons)
EVT_BUTTON    (ID_CHOICE_FONT,          MyPanel::OnChoiceButtons)
EVT_CHECKBOX  (ID_CHOICE_ENABLE,        MyPanel::OnChoiceButtons)
#endif
EVT_COMBOBOX  (ID_COMBO,                MyPanel::OnCombo)
EVT_TEXT      (ID_COMBO,                MyPanel::OnComboTextChanged)
EVT_TEXT_ENTER(ID_COMBO,                MyPanel::OnComboTextEnter)
EVT_BUTTON    (ID_COMBO_SEL_NUM,        MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_SEL_STR,        MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_CLEAR,          MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_APPEND,         MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_DELETE,         MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_FONT,           MyPanel::OnComboButtons)
EVT_BUTTON    (ID_COMBO_SET_TEXT,       MyPanel::OnComboButtons)
EVT_CHECKBOX  (ID_COMBO_ENABLE,         MyPanel::OnComboButtons)
EVT_RADIOBOX  (ID_RADIOBOX,             MyPanel::OnRadio)
EVT_BUTTON    (ID_RADIOBOX_SEL_NUM,     MyPanel::OnRadioButtons)
EVT_BUTTON    (ID_RADIOBOX_SEL_STR,     MyPanel::OnRadioButtons)
EVT_BUTTON    (ID_RADIOBOX_FONT,        MyPanel::OnRadioButtons)
EVT_CHECKBOX  (ID_RADIOBOX_ENABLE,      MyPanel::OnRadioButtons)
EVT_RADIOBUTTON(ID_RADIOBUTTON_1,       MyPanel::OnRadioButton1)
EVT_RADIOBUTTON(ID_RADIOBUTTON_2,       MyPanel::OnRadioButton2)
EVT_BUTTON    (ID_SET_FONT,             MyPanel::OnSetFont)
#if wxUSE_SLIDER
EVT_SLIDER    (ID_SLIDER,               MyPanel::OnSliderUpdate)
#endif // wxUSE_SLIDER
#if wxUSE_SPINBTN
EVT_SPIN      (ID_SPIN,                 MyPanel::OnSpinUpdate)
EVT_SPIN_UP   (ID_SPIN,                 MyPanel::OnSpinUp)
EVT_SPIN_DOWN (ID_SPIN,                 MyPanel::OnSpinDown)
#if wxUSE_PROGRESSDLG
EVT_UPDATE_UI (ID_BTNPROGRESS,          MyPanel::OnUpdateShowProgress)
EVT_BUTTON    (ID_BTNPROGRESS,          MyPanel::OnShowProgress)
#endif // wxUSE_PROGRESSDLG
#endif // wxUSE_SPINBTN
#if wxUSE_SPINCTRL
EVT_SPINCTRL  (ID_SPINCTRL,             MyPanel::OnSpinCtrl)
EVT_SPIN_UP   (ID_SPINCTRL,             MyPanel::OnSpinCtrlUp)
EVT_SPIN_DOWN (ID_SPINCTRL,             MyPanel::OnSpinCtrlDown)
EVT_TEXT      (ID_SPINCTRL,             MyPanel::OnSpinCtrlText)
#endif // wxUSE_SPINCTRL
EVT_BUTTON    (ID_BTNNEWTEXT,           MyPanel::OnNewText)
EVT_TOGGLEBUTTON(ID_BUTTON_LABEL,       MyPanel::OnUpdateLabel)
EVT_CHECKBOX  (ID_CHANGE_COLOUR,        MyPanel::OnChangeColour)
EVT_BUTTON    (ID_BUTTON_TEST1,         MyPanel::OnTestButton)
EVT_BUTTON    (ID_BUTTON_TEST2,         MyPanel::OnTestButton)
EVT_BUTTON    (ID_BITMAP_BTN,           MyPanel::OnBmpButton)
EVT_TOGGLEBUTTON(ID_BITMAP_BTN_ENABLE,  MyPanel::OnBmpButtonToggle)

EVT_CHECKBOX  (ID_SIZER_CHECK1,         MyPanel::OnSizerCheck)
EVT_CHECKBOX  (ID_SIZER_CHECK2,         MyPanel::OnSizerCheck)
EVT_CHECKBOX  (ID_SIZER_CHECK3,         MyPanel::OnSizerCheck)
EVT_CHECKBOX  (ID_SIZER_CHECK4,         MyPanel::OnSizerCheck)
EVT_CHECKBOX  (ID_SIZER_CHECK14,        MyPanel::OnSizerCheck)
EVT_CHECKBOX  (ID_SIZER_CHECKBIG,       MyPanel::OnSizerCheck)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyButton, wxButton)
    EVT_LEFT_DCLICK(MyButton::OnDClick)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyComboBox, wxComboBox)
    EVT_CHAR(MyComboBox::OnChar)
    EVT_KEY_DOWN(MyComboBox::OnKeyDown)
    EVT_KEY_UP(MyComboBox::OnKeyUp)

    EVT_SET_FOCUS(MyComboBox::OnFocusGot)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyRadioBox, wxRadioBox)
    EVT_SET_FOCUS(MyRadioBox::OnFocusGot)
    EVT_KILL_FOCUS(MyRadioBox::OnFocusLost)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyChoice, wxChoice)
    EVT_SET_FOCUS(MyChoice::OnFocusGot)
    EVT_KILL_FOCUS(MyChoice::OnFocusLost)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, wxID_ANY, wxPoint(x, y), wxSize(w, h) )
{
    m_listbox = NULL;
    m_listboxSorted = NULL;
#if wxUSE_CHOICE
    m_choice = NULL;
    m_choiceSorted = NULL;
#endif // wxUSE_CHOICE
    m_combo = NULL;
    m_radio = NULL;
#if wxUSE_GAUGE
    m_gauge = NULL;
    m_gaugeVert = NULL;
#endif // wxUSE_GAUGE
#if wxUSE_SLIDER
    m_slider = NULL;
#endif // wxUSE_SLIDER
    m_fontButton = NULL;
    m_lbSelectNum = NULL;
    m_lbSelectThis = NULL;
#if wxUSE_SPINBTN
    m_spinbutton = NULL;
#if wxUSE_PROGRESSDLG
    m_btnProgress = NULL;
#endif // wxUSE_PROGRESSDLG
#endif // wxUSE_SPINBTN
#if wxUSE_SPINCTRL
    m_spinctrl = NULL;
#endif // wxUSE_SPINCTRL
    m_spintext = NULL;
    m_checkbox = NULL;
    m_text = NULL;
    m_book = NULL;
    m_label = NULL;

    m_text = new wxTextCtrl(this, wxID_ANY, _T("This is the log window.\n"),
                            wxPoint(0, 250), wxSize(100, 50), wxTE_MULTILINE);

    m_logTargetOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_text));

    m_book = new wxBookCtrl(this, ID_BOOK);

    wxString choices[] =
    {
        _T("This"),
        _T("is one of my"),
        _T("really"),
        _T("wonderful"),
        _T("examples.")
    };

#ifdef USE_XPM
    // image ids
    enum
    {
        Image_List,
        Image_Choice,
        Image_Combo,
        Image_Text,
        Image_Radio,
#if wxUSE_GAUGE
        Image_Gauge,
#endif // wxUSE_GAUGE
        Image_Max
    };

    // fill the image list
    wxBitmap bmp(list_xpm);

    wxImageList *imagelist = new wxImageList(bmp.GetWidth(), bmp.GetHeight());

    imagelist-> Add( bmp );
    imagelist-> Add( wxBitmap( choice_xpm ));
    imagelist-> Add( wxBitmap( combo_xpm ));
    imagelist-> Add( wxBitmap( text_xpm ));
    imagelist-> Add( wxBitmap( radio_xpm ));
#if wxUSE_GAUGE
    imagelist-> Add( wxBitmap( gauge_xpm ));
#endif // wxUSE_GAUGE
    m_book->SetImageList(imagelist);
#elif defined(__WXMSW__)
    // load images from resources
    enum
    {
        Image_List,
        Image_Choice,
        Image_Combo,
        Image_Text,
        Image_Radio,
#if wxUSE_GAUGE
        Image_Gauge,
#endif // wxUSE_GAUGE
        Image_Max
    };
    wxImageList *imagelist = new wxImageList(16, 16, false, Image_Max);

    static const wxChar *s_iconNames[Image_Max] =
    {
        _T("list")
        , _T("choice")
        , _T("combo")
        , _T("text")
        , _T("radio")
#if wxUSE_GAUGE
        , _T("gauge")
#endif // wxUSE_GAUGE
    };

    for ( size_t n = 0; n < Image_Max; n++ )
    {
        wxBitmap bmp(s_iconNames[n]);
        if ( !bmp.Ok() || (imagelist->Add(bmp) == -1) )
        {
            wxLogWarning(wxT("Couldn't load the image '%s' for the book control page %d."),
                    s_iconNames[n], n);
        }
    }

    m_book->SetImageList(imagelist);
#else

    // No images for now
#define    Image_List -1
#define    Image_Choice -1
#define    Image_Combo -1
#define    Image_Text -1
#define    Image_Radio -1
#if wxUSE_GAUGE
#define    Image_Gauge -1
#endif // wxUSE_GAUGE
#define    Image_Max -1

#endif

    wxPanel *panel = new wxPanel(m_book);
    m_listbox = new wxListBox( panel, ID_LISTBOX,
                               wxPoint(10,10), wxSize(120,70),
                               5, choices, wxLB_MULTIPLE |wxLB_ALWAYS_SB );
    m_listboxSorted = new wxListBox( panel, ID_LISTBOX_SORTED,
                                     wxPoint(10,90), wxSize(120,70),
                                     5, choices, wxLB_SORT );

    SetListboxClientData(wxT("listbox"), m_listbox);
    SetListboxClientData(wxT("listbox"), m_listboxSorted);

    m_listbox->SetCursor(*wxCROSS_CURSOR);
#if wxUSE_TOOLTIPS
    m_listbox->SetToolTip( _T("This is a list box") );
#endif // wxUSE_TOOLTIPS

    m_lbSelectNum = new wxButton( panel, ID_LISTBOX_SEL_NUM, _T("Select #&2"), wxPoint(180,30), wxSize(140,30) );
    m_lbSelectThis = new wxButton( panel, ID_LISTBOX_SEL_STR, _T("&Select 'This'"), wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_CLEAR, _T("&Clear"), wxPoint(180,80), wxSize(140,30) );
    (void)new MyButton( panel, ID_LISTBOX_APPEND, _T("&Append 'Hi!'"), wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_DELETE, _T("D&elete selected item"), wxPoint(180,130), wxSize(140,30) );
    wxButton *button = new MyButton( panel, ID_LISTBOX_FONT, _T("Set &Italic font"), wxPoint(340,130), wxSize(140,30) );

    button->SetDefault();

#if wxUSE_TOOLTIPS
    button->SetToolTip( _T("Press here to set italic font") );
#endif // wxUSE_TOOLTIPS

    m_checkbox = new wxCheckBox( panel, ID_LISTBOX_ENABLE, _T("&Disable"), wxPoint(20,170) );
    m_checkbox->SetValue(false);
    button->MoveAfterInTabOrder(m_checkbox);
#if wxUSE_TOOLTIPS
    m_checkbox->SetToolTip( _T("Click here to disable the listbox") );
#endif // wxUSE_TOOLTIPS
    (void)new wxCheckBox( panel, ID_CHANGE_COLOUR, _T("&Toggle colour"),
                          wxPoint(110,170) );
    panel->SetCursor(wxCursor(wxCURSOR_HAND));
    m_book->AddPage(panel, _T("wxListBox"), true, Image_List);

#if wxUSE_CHOICE
    panel = new wxPanel(m_book);
    m_choice = new MyChoice( panel, ID_CHOICE, wxPoint(10,10), wxSize(120,wxDefaultCoord), 5, choices );
    m_choiceSorted = new MyChoice( panel, ID_CHOICE_SORTED, wxPoint(10,70), wxSize(120,wxDefaultCoord),
                                   5, choices, wxCB_SORT );

    SetChoiceClientData(wxT("choice"), m_choice);
    SetChoiceClientData(wxT("choice"), m_choiceSorted);

    m_choice->SetSelection(2);
    (void)new wxButton( panel, ID_CHOICE_SEL_NUM, _T("Select #&2"), wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_SEL_STR, _T("&Select 'This'"), wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_CLEAR, _T("&Clear"), wxPoint(180,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_APPEND, _T("&Append 'Hi!'"), wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_DELETE, _T("D&elete selected item"), wxPoint(180,130), wxSize(140,30) );
    (void)new wxButton( panel, ID_CHOICE_FONT, _T("Set &Italic font"), wxPoint(340,130), wxSize(140,30) );
    (void)new wxCheckBox( panel, ID_CHOICE_ENABLE, _T("&Disable"), wxPoint(20,130), wxSize(140,30) );

    m_book->AddPage(panel, _T("wxChoice"), false, Image_Choice);
#endif // wxUSE_CHOICE

    panel = new wxPanel(m_book);
    (void)new wxStaticBox( panel, wxID_ANY, _T("&Box around combobox"),
                           wxPoint(5, 5), wxSize(150, 100));
    m_combo = new MyComboBox( panel, ID_COMBO, _T("This"),
                              wxPoint(20,25), wxSize(120, wxDefaultCoord),
                              5, choices,
                              wxTE_PROCESS_ENTER);

    (void)new wxButton( panel, ID_COMBO_SEL_NUM, _T("Select #&2"), wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_SEL_STR, _T("&Select 'This'"), wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_CLEAR, _T("&Clear"), wxPoint(180,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_APPEND, _T("&Append 'Hi!'"), wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_DELETE, _T("D&elete selected item"), wxPoint(180,130), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_FONT, _T("Set &Italic font"), wxPoint(340,130), wxSize(140,30) );
    (void)new wxButton( panel, ID_COMBO_SET_TEXT, _T("Set 'Hi!' at #2"), wxPoint(340,180), wxSize(140,30) );
    (void)new wxCheckBox( panel, ID_COMBO_ENABLE, _T("&Disable"), wxPoint(20,130), wxSize(140,30) );
    m_book->AddPage(panel, _T("wxComboBox"), false, Image_Combo);

    wxString choices2[] =
    {
        _T("First"), _T("Second"),
        /* "Third",
        "Fourth", "Fifth", "Sixth",
        "Seventh", "Eighth", "Nineth", "Tenth" */
    };

    panel = new wxPanel(m_book);
#if wxUSE_TOOLTIPS
    wxRadioBox *radio2 =
#endif // wxUSE_TOOLTIPS
        new MyRadioBox( panel, ID_RADIOBOX, _T("&That"), wxPoint(10,160), wxDefaultSize, WXSIZEOF(choices2), choices2, 1, wxRA_SPECIFY_ROWS );
    m_radio = new wxRadioBox( panel, ID_RADIOBOX, _T("T&his"), wxPoint(10,10), wxDefaultSize, WXSIZEOF(choices), choices, 1, wxRA_SPECIFY_COLS );

#if wxUSE_TOOLTIPS
    m_combo->SetToolTip(_T("This is a natural\ncombobox - can you believe me?"));
    radio2->SetToolTip(_T("Ever seen a radiobox?"));

    //m_radio->SetToolTip(_T("Tooltip for the entire radiobox"));
    for ( unsigned int nb = 0; nb < WXSIZEOF(choices); nb++ )
    {
        m_radio->SetItemToolTip(nb, _T("tooltip for\n") + choices[nb]);
    }

    // remove the tooltip for one of the items
    m_radio->SetItemToolTip(2, _T(""));
#endif // wxUSE_TOOLTIPS

#if wxUSE_HELP
    for( unsigned int item = 0; item < WXSIZEOF(choices); ++item )
        m_radio->SetItemHelpText( item, wxString::Format( _T("Help text for \"%s\""), choices[item].c_str() ) );

    // erase help text for the second item
    m_radio->SetItemHelpText( 1, _T("") );
    // set default help text for control
    m_radio->SetHelpText( _T("Default helptext for wxRadioBox") );
#endif // wxUSE_HELP

    (void)new wxButton( panel, ID_RADIOBOX_SEL_NUM, _T("Select #&2"), wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_RADIOBOX_SEL_STR, _T("&Select 'This'"), wxPoint(180,80), wxSize(140,30) );
    m_fontButton = new wxButton( panel, ID_SET_FONT, _T("Set &more Italic font"), wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_RADIOBOX_FONT, _T("Set &Italic font"), wxPoint(340,80), wxSize(140,30) );
    (void)new wxCheckBox( panel, ID_RADIOBOX_ENABLE, _T("&Disable"), wxPoint(340,130), wxDefaultSize );

    wxRadioButton *rb = new wxRadioButton( panel, ID_RADIOBUTTON_1, _T("Radiobutton1"), wxPoint(210,170), wxDefaultSize, wxRB_GROUP );
    rb->SetValue( false );
    (void)new wxRadioButton( panel, ID_RADIOBUTTON_2, _T("&Radiobutton2"), wxPoint(340,170), wxDefaultSize );
    m_book->AddPage(panel, _T("wxRadioBox"), false, Image_Radio);


#if wxUSE_SLIDER && wxUSE_GAUGE
    panel = new wxPanel(m_book);

    wxBoxSizer *main_sizer = new wxBoxSizer( wxHORIZONTAL );
    panel->SetSizer( main_sizer );

    wxStaticBoxSizer *gauge_sizer = new wxStaticBoxSizer( wxHORIZONTAL, panel, _T("&wxGauge and wxSlider") );
    main_sizer->Add( gauge_sizer, 0, wxALL, 5 );
    wxBoxSizer *sz = new wxBoxSizer( wxVERTICAL );
    gauge_sizer->Add( sz );
    m_gauge = new wxGauge( panel, wxID_ANY, 200, wxDefaultPosition, wxSize(155, 30), wxGA_HORIZONTAL|wxNO_BORDER );
    sz->Add( m_gauge, 0, wxALL, 10 );
    m_slider = new wxSlider( panel, ID_SLIDER, 0, 0, 200,
                             wxDefaultPosition, wxSize(155,wxDefaultCoord),
                             wxSL_AUTOTICKS | wxSL_LABELS);
    m_slider->SetTickFreq(40, 0);
#if wxUSE_TOOLTIPS
    m_slider->SetToolTip(_T("This is a sliding slider"));
#endif // wxUSE_TOOLTIPS
    sz->Add( m_slider, 0, wxALL, 10 );

    m_gaugeVert = new wxGauge( panel, wxID_ANY, 100,
                               wxDefaultPosition, wxSize(wxDefaultCoord, 90),
                               wxGA_VERTICAL | wxGA_SMOOTH | wxNO_BORDER );
    gauge_sizer->Add( m_gaugeVert, 0, wxALL, 10 );



    wxStaticBox *sb = new wxStaticBox( panel, wxID_ANY, _T("&Explanation"),
                           wxDefaultPosition, wxDefaultSize ); //, wxALIGN_CENTER );
    wxStaticBoxSizer *wrapping_sizer = new wxStaticBoxSizer( sb, wxVERTICAL );
    main_sizer->Add( wrapping_sizer, 0, wxALL, 5 );

#ifdef __WXMOTIF__
    // No wrapping text in wxStaticText yet :-(
    m_wrappingText = new wxStaticText( panel, wxID_ANY,
                            _T("Drag the slider!"),
                            wxPoint(250,30),
                            wxSize(240, wxDefaultCoord)
                          );
#else
    m_wrappingText = new wxStaticText( panel, wxID_ANY,
                            _T("In order see the gauge (aka progress bar) ")
                            _T("control do something you have to drag the ")
                            _T("handle of the slider to the right.")
                            _T("\n\n")
                            _T("This is also supposed to demonstrate how ")
                            _T("to use static controls with line wrapping."),
                            wxDefaultPosition,
                            wxSize(240, wxDefaultCoord)
                          );
#endif
    wrapping_sizer->Add( m_wrappingText );

    wxStaticBoxSizer *non_wrapping_sizer = new wxStaticBoxSizer( wxVERTICAL, panel, wxT("Non-wrapping") );
    main_sizer->Add( non_wrapping_sizer, 0, wxALL, 5 );

    m_nonWrappingText = new wxStaticText( panel, wxID_ANY,
                            _T("This static text has two lines.\nThey do not wrap."),
                            wxDefaultPosition,
                            wxDefaultSize
                          );
    non_wrapping_sizer->Add( m_nonWrappingText );

    (void)new wxButton( panel, ID_BTNNEWTEXT, wxT("New text"), wxPoint(450, 160) );

    int initialSpinValue = -5;
    wxString s;
    s << initialSpinValue;
    m_spintext = new wxTextCtrl( panel, wxID_ANY, s, wxPoint(20,160), wxSize(80,wxDefaultCoord) );
#if wxUSE_SPINBTN
    m_spinbutton = new wxSpinButton( panel, ID_SPIN, wxPoint(103,160) );
    m_spinbutton->SetRange(-40,30);
    m_spinbutton->SetValue(initialSpinValue);

#if wxUSE_PROGRESSDLG
    m_btnProgress = new wxButton( panel, ID_BTNPROGRESS, _T("&Show progress dialog"),
                                  wxPoint(300, 160) );
#endif // wxUSE_PROGRESSDLG
#endif // wxUSE_SPINBTN

#if wxUSE_SPINCTRL
    m_spinctrl = new wxSpinCtrl( panel, ID_SPINCTRL, wxEmptyString, wxPoint(200, 160), wxSize(80, wxDefaultCoord) );
    m_spinctrl->SetRange(-10,30);
    m_spinctrl->SetValue(15);
#endif // wxUSE_SPINCTRL

    m_book->AddPage(panel, _T("wxGauge"), false, Image_Gauge);
#endif // wxUSE_SLIDER && wxUSE_GAUGE


    panel = new wxPanel(m_book);

#if !defined(__WXMOTIF__) // wxStaticBitmap not working under Motif yet.
    wxIcon icon = wxArtProvider::GetIcon(wxART_INFORMATION);
    (void) new wxStaticBitmap( panel, wxID_ANY, icon, wxPoint(10, 10) );

    // VZ: don't leak memory
    // bmpStatic = new wxStaticBitmap(panel, wxID_ANY, wxNullIcon, wxPoint(50, 10));
    // bmpStatic->SetIcon(wxArtProvider::GetIcon(wxART_QUESTION));
#endif // !Motif

    wxBitmap bitmap( 100, 100 );
    wxMemoryDC dc;
    dc.SelectObject( bitmap );
    dc.SetBackground(*wxGREEN);
    dc.SetPen(*wxRED_PEN);
    dc.Clear();
    dc.DrawEllipse(5, 5, 90, 90);
    dc.DrawText(_T("Bitmap"), 30, 40);
    dc.SelectObject( wxNullBitmap );

    (void)new wxBitmapButton(panel, ID_BITMAP_BTN, bitmap, wxPoint(100, 20));
    (void)new wxToggleButton(panel, ID_BITMAP_BTN_ENABLE,
                             _T("Enable/disable &bitmap"), wxPoint(100, 140));

#if defined(__WXMSW__) || defined(__WXMOTIF__)
    // test for masked bitmap display
    bitmap = wxBitmap(_T("test2.bmp"), wxBITMAP_TYPE_BMP);
    if (bitmap.Ok())
    {
       bitmap.SetMask(new wxMask(bitmap, *wxBLUE));

       (void)new wxStaticBitmap(panel, wxID_ANY, bitmap, wxPoint(300, 120));
    }
#endif

    wxBitmap bmp1(wxArtProvider::GetBitmap(wxART_INFORMATION)),
             bmp2(wxArtProvider::GetBitmap(wxART_WARNING)),
             bmp3(wxArtProvider::GetBitmap(wxART_QUESTION));
    wxBitmapButton *bmpBtn = new wxBitmapButton
                                 (
                                  panel, wxID_ANY,
                                  bmp1,
                                  wxPoint(30, 70)
                                 );

    bmpBtn->SetBitmapSelected(bmp2);
    bmpBtn->SetBitmapFocus(bmp3);

    (void)new wxToggleButton(panel, ID_BUTTON_LABEL,
                             _T("&Toggle label"), wxPoint(250, 20));

    m_label = new wxStaticText(panel, wxID_ANY, _T("Label with some long text"),
                               wxPoint(250, 60), wxDefaultSize,
                               wxALIGN_RIGHT /*| wxST_NO_AUTORESIZE*/);
    m_label->SetForegroundColour( *wxBLUE );

    m_book->AddPage(panel, _T("wxBitmapXXX"));

    // sizer
    panel = new wxPanel(m_book);
    panel->SetAutoLayout( true );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer *csizer =
      new wxStaticBoxSizer (new wxStaticBox (panel, wxID_ANY, _T("Show Buttons")), wxHORIZONTAL );

    wxCheckBox *check1, *check2, *check3, *check4, *check14, *checkBig;
    check1 = new wxCheckBox (panel, ID_SIZER_CHECK1, _T("1"));
    check1->SetValue (true);
    csizer->Add (check1);
    check2 = new wxCheckBox (panel, ID_SIZER_CHECK2, _T("2"));
    check2->SetValue (true);
    csizer->Add (check2);
    check3 = new wxCheckBox (panel, ID_SIZER_CHECK3, _T("3"));
    check3->SetValue (true);
    csizer->Add (check3);
    check4 = new wxCheckBox (panel, ID_SIZER_CHECK4, _T("4"));
    check4->SetValue (true);
    csizer->Add (check4);
    check14 = new wxCheckBox (panel, ID_SIZER_CHECK14, _T("1-4"));
    check14->SetValue (true);
    csizer->Add (check14);
    checkBig = new wxCheckBox (panel, ID_SIZER_CHECKBIG, _T("Big"));
    checkBig->SetValue (true);
    csizer->Add (checkBig);

    sizer->Add (csizer);

    m_hsizer = new wxBoxSizer( wxHORIZONTAL );

    m_buttonSizer = new wxBoxSizer (wxVERTICAL);

    m_sizerBtn1 = new wxButton(panel, wxID_ANY, _T("Test Button &1 (tab order 1)") );
    m_buttonSizer->Add( m_sizerBtn1, 0, wxALL, 10 );
    m_sizerBtn2 = new wxButton(panel, wxID_ANY, _T("Test Button &2 (tab order 3)") );
    m_buttonSizer->Add( m_sizerBtn2, 0, wxALL, 10 );
    m_sizerBtn3 = new wxButton(panel, wxID_ANY, _T("Test Button &3 (tab order 2)") );
    m_buttonSizer->Add( m_sizerBtn3, 0, wxALL, 10 );
    m_sizerBtn4 = new wxButton(panel, wxID_ANY, _T("Test Button &4 (tab order 4)") );
    m_buttonSizer->Add( m_sizerBtn4, 0, wxALL, 10 );

    m_sizerBtn3->MoveBeforeInTabOrder(m_sizerBtn2);

    m_hsizer->Add (m_buttonSizer);
    m_hsizer->Add( 20,20, 1 );
    m_bigBtn = new wxButton(panel, wxID_ANY, _T("Multiline\nbutton") );
    m_hsizer->Add( m_bigBtn , 3, wxGROW|wxALL, 10 );

    sizer->Add (m_hsizer, 1, wxGROW);

    panel->SetSizer( sizer );

    m_book->AddPage(panel, _T("wxSizer"));

    // set the sizer for the panel itself
    sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_book, wxSizerFlags().Border().Expand());
    sizer->Add(m_text, wxSizerFlags(1).Border().Expand());
    SetSizer(sizer);
}

void MyPanel::OnIdle(wxIdleEvent& event)
{
    static const int INVALID_SELECTION = -2;

    static int s_selCombo = INVALID_SELECTION;

    if (!m_combo || !m_choice)
    {
        event.Skip();
        return;
    }

    int sel = m_combo->GetSelection();
    if ( sel != s_selCombo )
    {
        if ( s_selCombo != INVALID_SELECTION )
        {
            wxLogMessage(_T("EVT_IDLE: combobox selection changed from %d to %d"),
                         s_selCombo, sel);
        }

        s_selCombo = sel;
    }

    static int s_selChoice = INVALID_SELECTION;
    sel = m_choice->GetSelection();
    if ( sel != s_selChoice )
    {
        if ( s_selChoice != INVALID_SELECTION )
        {
            wxLogMessage(_T("EVT_IDLE: choice selection changed from %d to %d"),
                         s_selChoice, sel);
        }

        s_selChoice = sel;
    }

    event.Skip();
}

void MyPanel::OnPageChanging( wxBookCtrlEvent &event )
{
    int selOld = event.GetOldSelection();
    if ( selOld == 2 )
    {
        if ( wxMessageBox(_T("This demonstrates how a program may prevent the\n")
                          _T("page change from taking place - if you select\n")
                          _T("[No] the current page will stay the third one\n"),
                          _T("Control sample"),
                          wxICON_QUESTION | wxYES_NO, this) != wxYES )
        {
            event.Veto();

            return;
        }
    }

    *m_text << _T("Book selection is being changed from ") << selOld
            << _T(" to ") << event.GetSelection()
            << _T(" (current page from book is ")
            << m_book->GetSelection() << _T(")\n");
}

void MyPanel::OnPageChanged( wxBookCtrlEvent &event )
{
    *m_text << _T("Book selection is now ") << event.GetSelection()
            << _T(" (from book: ") << m_book->GetSelection()
            << _T(")\n");
}

void MyPanel::OnTestButton(wxCommandEvent& event)
{
    wxLogMessage(_T("Button %c clicked."),
                 event.GetId() == ID_BUTTON_TEST1 ? _T('1') : _T('2'));
}

void MyPanel::OnBmpButton(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(_T("Bitmap button clicked."));
}

void MyPanel::OnBmpButtonToggle(wxCommandEvent& event)
{
    FindWindow(ID_BITMAP_BTN)->Enable(!event.IsChecked());
}

void MyPanel::OnChangeColour(wxCommandEvent& WXUNUSED(event))
{
    static wxColour s_colOld;

    SetThemeEnabled(false);
    // test panel colour changing and propagation to the subcontrols
    if ( s_colOld.Ok() )
    {
        SetBackgroundColour(s_colOld);
        s_colOld = wxNullColour;

        m_lbSelectThis->SetForegroundColour(wxNullColour);
        m_lbSelectThis->SetBackgroundColour(wxNullColour);
    }
    else
    {
        s_colOld = wxColour(wxT("red"));
        SetBackgroundColour(wxT("white"));

        m_lbSelectThis->SetForegroundColour(wxT("white"));
        m_lbSelectThis->SetBackgroundColour(wxT("red"));
    }

    m_lbSelectThis->Refresh();
    Refresh();
}

void MyPanel::OnListBox( wxCommandEvent &event )
{
    if (event.GetInt() == -1)
    {
        m_text->AppendText( _T("ListBox has no selections anymore\n") );
        return;
    }

    wxListBox *listbox = event.GetId() == ID_LISTBOX ? m_listbox
                                                     : m_listboxSorted;

    m_text->AppendText( _T("ListBox event selection string is: '") );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( _T("'\n") );

    // can't use GetStringSelection() with multiple selections, there could be
    // more than one of them
    if ( !listbox->HasFlag(wxLB_MULTIPLE) )
    {
        m_text->AppendText( _T("ListBox control selection string is: '") );
        m_text->AppendText( listbox->GetStringSelection() );
        m_text->AppendText( _T("'\n") );
    }

    wxStringClientData *obj = ((wxStringClientData *)event.GetClientObject());
    m_text->AppendText( _T("ListBox event client data string is: '") );
    if (obj) // BC++ doesn't like use of '? .. : .. ' in this context
        m_text->AppendText( obj->GetData() );
    else
        m_text->AppendText( wxString(_T("none")) );

    m_text->AppendText( _T("'\n") );
    m_text->AppendText( _T("ListBox control client data string is: '") );
    obj = (wxStringClientData *)listbox->GetClientObject(event.GetInt());
    if (obj)
        m_text->AppendText( obj->GetData() );
    else
        m_text->AppendText( wxString(_T("none")) );
    m_text->AppendText( _T("'\n") );
}

void MyPanel::OnListBoxDoubleClick( wxCommandEvent &event )
{
    m_text->AppendText( _T("ListBox double click string is: ") );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( _T("\n") );
}

void MyPanel::OnListBoxButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_LISTBOX_ENABLE:
            {
                m_text->AppendText(_T("Checkbox clicked.\n"));
#if wxUSE_TOOLTIPS
                wxCheckBox *cb = (wxCheckBox*)event.GetEventObject();
                if (event.GetInt())
                    cb->SetToolTip( _T("Click to enable listbox") );
                else
                    cb->SetToolTip( _T("Click to disable listbox") );
#endif // wxUSE_TOOLTIPS
                m_listbox->Enable( event.GetInt() == 0 );
                m_lbSelectThis->Enable( event.GetInt() == 0 );
                m_lbSelectNum->Enable( event.GetInt() == 0 );
                m_listboxSorted->Enable( event.GetInt() == 0 );
                FindWindow(ID_CHANGE_COLOUR)->Enable( event.GetInt() == 0 );
                break;
            }
        case ID_LISTBOX_SEL_NUM:
            {
                if (m_listbox->GetCount() > 2)
                    m_listbox->SetSelection( 2 );
                if (m_listboxSorted->GetCount() > 2)
                    m_listboxSorted->SetSelection( 2 );
                m_lbSelectThis->WarpPointer( 40, 14 );
                break;
            }
        case ID_LISTBOX_SEL_STR:
            {
                if (m_listbox->FindString(_T("This")) != wxNOT_FOUND)
                    m_listbox->SetStringSelection( _T("This") );
                if (m_listboxSorted->FindString(_T("This")) != wxNOT_FOUND)
                    m_listboxSorted->SetStringSelection( _T("This") );
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
                m_listbox->Append( _T("Hi!") );
                m_listboxSorted->Append( _T("Hi!") );
                break;
            }
        case ID_LISTBOX_DELETE:
            {
                int idx;
                idx = m_listbox->GetSelection();
                if ( idx != wxNOT_FOUND )
                    m_listbox->Delete( idx );
                idx = m_listboxSorted->GetSelection();
                if ( idx != wxNOT_FOUND )
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

#if wxUSE_CHOICE

static const wxChar *GetDataString(wxClientData *data)
{
    return data ? wx_static_cast(wxStringClientData *, data)->GetData().c_str()
                : _T("none");
}

void MyPanel::OnChoice( wxCommandEvent &event )
{
    wxChoice *choice = event.GetId() == ID_CHOICE ? m_choice
                                                  : m_choiceSorted;

    const int sel = choice->GetSelection();

    wxClientData *dataEvt = event.GetClientObject(),
                 *dataCtrl = choice->GetClientObject(sel);

    wxLogMessage(_T("EVT_CHOICE: item %d/%d (event/control), ")
                 _T("string \"%s\"/\"%s\", ")
                 _T("data \"%s\"/\"%s\""),
                 (int)event.GetInt(),
                 sel,
                 event.GetString().c_str(),
                 choice->GetStringSelection().c_str(),
                 GetDataString(dataEvt),
                 GetDataString(dataCtrl));
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
                m_choice->SetStringSelection( _T("This") );
                m_choiceSorted->SetStringSelection( _T("This") );
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
                m_choice->Append( _T("Hi!") );
                m_choiceSorted->Append( _T("Hi!") );
                break;
            }
        case ID_CHOICE_DELETE:
            {
                int idx = m_choice->GetSelection();
                if ( idx != wxNOT_FOUND )
                    m_choice->Delete( idx );
                idx = m_choiceSorted->GetSelection();
                if ( idx != wxNOT_FOUND )
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
#endif // wxUSE_CHOICE

void MyPanel::OnCombo( wxCommandEvent &event )
{
    if (!m_combo)
        return;
    
    wxLogMessage(_T("EVT_COMBOBOX: item %d/%d (event/control), string \"%s\"/\"%s\""),
                 (int)event.GetInt(),
                 m_combo->GetSelection(),
                 event.GetString().c_str(),
                 m_combo->GetStringSelection().c_str());
}

void MyPanel::OnComboTextChanged(wxCommandEvent& event)
{
    if (m_combo)
        wxLogMessage(wxT("EVT_TEXT for the combobox: \"%s\" (event) or \"%s\" (control)."),
                     event.GetString().c_str(),
                     m_combo->GetValue().c_str());
}

void MyPanel::OnComboTextEnter(wxCommandEvent& WXUNUSED(event))
{
    if (m_combo)
        wxLogMessage(_T("Enter pressed in the combobox: value is '%s'."),
                     m_combo->GetValue().c_str());
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
                m_combo->SetStringSelection( _T("This") );
                break;
            }
        case ID_COMBO_CLEAR:
            {
                m_combo->Clear();
                break;
            }
        case ID_COMBO_APPEND:
            {
                m_combo->Append( _T("Hi!") );
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
        case ID_COMBO_SET_TEXT:
            {
                m_combo->SetString( 2, wxT("Hi!") );
                break;
            }
    }
}

void MyPanel::OnRadio( wxCommandEvent &event )
{
    m_text->AppendText( _T("RadioBox selection string is: ") );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( _T("\n") );
}

void MyPanel::OnRadioButton1( wxCommandEvent & WXUNUSED(event) )
{
    wxMessageBox(_T("First wxRadioButton selected."), _T("wxControl sample"));
}

void MyPanel::OnRadioButton2( wxCommandEvent & WXUNUSED(event) )
{
    m_text->AppendText(_T("Second wxRadioButton selected.\n"));
}

void MyPanel::OnRadioButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_RADIOBOX_ENABLE:
            m_radio->Enable( event.GetInt() == 0 );
            break;

        case ID_RADIOBOX_SEL_NUM:
            m_radio->SetSelection( 2 );
            break;

        case ID_RADIOBOX_SEL_STR:
            m_radio->SetStringSelection( _T("This") );
            break;

        case ID_RADIOBOX_FONT:
            m_radio->SetFont( *wxITALIC_FONT );
            break;
    }
}

void MyPanel::OnSetFont( wxCommandEvent &WXUNUSED(event) )
{
    m_fontButton->SetFont( *wxITALIC_FONT );
    m_text->SetFont( *wxITALIC_FONT );
}

void MyPanel::OnUpdateLabel( wxCommandEvent &event )
{
    m_label->SetLabel(event.GetInt() ? _T("Very very very very very long text.")
                                     : _T("Shorter text."));
}

#if wxUSE_SLIDER

void MyPanel::OnSliderUpdate( wxCommandEvent &WXUNUSED(event) )
{
#if wxUSE_GAUGE
    m_gauge->SetValue( m_slider->GetValue() );
    m_gaugeVert->SetValue( m_slider->GetValue() / 2 );
#endif // wxUSE_GAUGE
}

#endif // wxUSE_SLIDER

#if wxUSE_SPINCTRL

void MyPanel::OnSpinCtrlText(wxCommandEvent& event)
{
    if ( m_spinctrl )
    {
        wxString s;
        s.Printf( _T("Spin ctrl text changed: now %d (from event: %s)\n"),
                 m_spinctrl->GetValue(), event.GetString().c_str() );
        m_text->AppendText(s);
    }
}

void MyPanel::OnSpinCtrl(wxSpinEvent& event)
{
    if ( m_spinctrl )
    {
        wxString s;
        s.Printf( _T("Spin ctrl changed: now %d (from event: %d)\n"),
                 m_spinctrl->GetValue(), event.GetInt() );
        m_text->AppendText(s);
    }
}

void MyPanel::OnSpinCtrlUp(wxSpinEvent& event)
{
    if ( m_spinctrl )
    {
        m_text->AppendText( wxString::Format(
            _T("Spin up: %d (from event: %d)\n"),
            m_spinctrl->GetValue(), event.GetInt() ) );
    }
}

void MyPanel::OnSpinCtrlDown(wxSpinEvent& event)
{
    if ( m_spinctrl )
    {
        m_text->AppendText( wxString::Format(
            _T("Spin down: %d (from event: %d)\n"),
            m_spinctrl->GetValue(), event.GetInt() ) );
    }
}

#endif // wxUSE_SPINCTRL

#if wxUSE_SPINBTN
void MyPanel::OnSpinUp( wxSpinEvent &event )
{
    wxString value;
    value.Printf( _T("Spin control up: current = %d\n"),
                 m_spinbutton->GetValue());

    if ( event.GetPosition() > 17 )
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

    if ( event.GetPosition() < -17 )
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

void MyPanel::OnNewText( wxCommandEvent& /* event */)
{
    m_nonWrappingText->SetLabel( wxT("This text is short\nbut still spans\nover three lines.") );
    m_wrappingText->SetLabel( wxT("This text is short but will still be wrapped if it is too long.") );
    m_wrappingText->GetParent()->Layout();
}

#if wxUSE_PROGRESSDLG

void MyPanel::OnUpdateShowProgress( wxUpdateUIEvent& event )
{
    event.Enable( m_spinbutton->GetValue() > 0 );
}

void MyPanel::OnShowProgress( wxCommandEvent& WXUNUSED(event) )
{
    int max = m_spinbutton->GetValue();

    if ( max <= 0 )
    {
        wxLogError(_T("You must set positive range!"));
        return;
    }

    wxProgressDialog dialog(_T("Progress dialog example"),
                            _T("An informative message"),
                            max,    // range
                            this,   // parent
                            wxPD_CAN_ABORT |
                            wxPD_AUTO_HIDE |
                            wxPD_APP_MODAL |
                            wxPD_ELAPSED_TIME |
                            wxPD_ESTIMATED_TIME |
                            wxPD_REMAINING_TIME);


    bool cont = true;
    for ( int i = 0; i <= max && cont; i++ )
    {
        wxSleep(1);
        if ( i == max )
        {
            cont = dialog.Update(i, _T("That's all, folks!"));
        }
        else if ( i == max / 2 )
        {
            cont = dialog.Update(i, _T("Only a half left (very long message)!"));
        }
        else
        {
            cont = dialog.Update(i);
        }
    }

    if ( !cont )
    {
        *m_text << _T("Progress dialog aborted!\n");
    }
    else
    {
        *m_text << _T("Countdown from ") << max << _T(" finished.\n");
    }
}

#endif // wxUSE_PROGRESSDLG
#endif // wxUSE_SPINBTN

void MyPanel::OnSizerCheck( wxCommandEvent &event)
{
  switch (event.GetId ()) {
  case ID_SIZER_CHECK1:
    m_buttonSizer->Show (m_sizerBtn1, event.IsChecked ());
    m_buttonSizer->Layout ();
    break;
  case ID_SIZER_CHECK2:
    m_buttonSizer->Show (m_sizerBtn2, event.IsChecked ());
    m_buttonSizer->Layout ();
    break;
  case ID_SIZER_CHECK3:
    m_buttonSizer->Show (m_sizerBtn3, event.IsChecked ());
    m_buttonSizer->Layout ();
    break;
  case ID_SIZER_CHECK4:
    m_buttonSizer->Show (m_sizerBtn4, event.IsChecked ());
    m_buttonSizer->Layout ();
    break;
  case ID_SIZER_CHECK14:
    m_hsizer->Show (m_buttonSizer, event.IsChecked ());
    m_hsizer->Layout ();
    break;
  case ID_SIZER_CHECKBIG:
    m_hsizer->Show (m_bigBtn, event.IsChecked ());
    m_hsizer->Layout ();
    break;
  }

}

MyPanel::~MyPanel()
{
    //wxLog::RemoveTraceMask(_T("focus"));
    delete wxLog::SetActiveTarget(m_logTargetOld);

    delete m_book->GetImageList();
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(CONTROLS_QUIT,   MyFrame::OnQuit)
    EVT_MENU(CONTROLS_ABOUT,  MyFrame::OnAbout)
    EVT_MENU(CONTROLS_CLEAR_LOG,  MyFrame::OnClearLog)
#if wxUSE_TOOLTIPS
    EVT_MENU(CONTROLS_SET_TOOLTIP_DELAY,  MyFrame::OnSetTooltipDelay)
    EVT_MENU(CONTROLS_ENABLE_TOOLTIPS,  MyFrame::OnToggleTooltips)
#endif // wxUSE_TOOLTIPS

    EVT_MENU(CONTROLS_ENABLE_ALL, MyFrame::OnEnableAll)
    EVT_MENU(CONTROLS_HIDE_ALL,   MyFrame::OnHideAll)
    EVT_MENU(CONTROLS_HIDE_LIST,   MyFrame::OnHideList)
    EVT_MENU(CONTROLS_CONTEXT_HELP, MyFrame::OnContextHelp)

    EVT_ICONIZE(MyFrame::OnIconized)
    EVT_MAXIMIZE(MyFrame::OnMaximized)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MOVE(MyFrame::OnMove)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame(const wxChar *title, int x, int y)
       : wxFrame(NULL, wxID_ANY, title, wxPoint(x, y), wxSize(700, 450))
{
    SetHelpText( _T("Controls sample demonstrating various widgets") );

    // Give it an icon
    // The wxICON() macros loads an icon from a resource under Windows
    // and uses an #included XPM image under GTK+ and Motif

#ifdef USE_XPM
    SetIcon( wxICON(mondrian) );
#endif

    wxMenu *file_menu = new wxMenu;

    file_menu->Append(CONTROLS_CLEAR_LOG, _T("&Clear log\tCtrl-L"));
    file_menu->AppendSeparator();
    file_menu->Append(CONTROLS_ABOUT, _T("&About\tF1"));
    file_menu->AppendSeparator();
    file_menu->Append(CONTROLS_QUIT, _T("E&xit\tAlt-X"), _T("Quit controls sample"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));

#if wxUSE_TOOLTIPS
    wxMenu *tooltip_menu = new wxMenu;
    tooltip_menu->Append(CONTROLS_SET_TOOLTIP_DELAY, _T("Set &delay\tCtrl-D"));
    tooltip_menu->AppendSeparator();
    tooltip_menu->Append(CONTROLS_ENABLE_TOOLTIPS, _T("&Toggle tooltips\tCtrl-T"),
            _T("enable/disable tooltips"), true);
    tooltip_menu->Check(CONTROLS_ENABLE_TOOLTIPS, true);
    menu_bar->Append(tooltip_menu, _T("&Tooltips"));
#endif // wxUSE_TOOLTIPS

    wxMenu *panel_menu = new wxMenu;
    panel_menu->Append(CONTROLS_ENABLE_ALL, _T("&Disable all\tCtrl-E"),
                       _T("Enable/disable all panel controls"), true);
    panel_menu->Append(CONTROLS_HIDE_ALL, _T("&Hide all\tCtrl-I"),
                       _T("Show/hide thoe whole panel controls"), true);
    panel_menu->Append(CONTROLS_HIDE_LIST, _T("Hide &list ctrl\tCtrl-S"),
                       _T("Enable/disable all panel controls"), true);
    panel_menu->Append(CONTROLS_CONTEXT_HELP, _T("&Context help...\tCtrl-H"),
                       _T("Get context help for a control"));
    menu_bar->Append(panel_menu, _T("&Panel"));

    SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    m_panel = new MyPanel( this, 10, 10, 300, 100 );
}

void MyFrame::OnQuit (wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxBusyCursor bc;

    wxMessageDialog dialog(this, _T("This is a control sample"), _T("About Controls"), wxOK );
    dialog.ShowModal();
}

void MyFrame::OnClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_panel->m_text->Clear();
}

#if wxUSE_TOOLTIPS
void MyFrame::OnSetTooltipDelay(wxCommandEvent& WXUNUSED(event))
{
    static long s_delay = 5000;

    wxString delay;
    delay.Printf( _T("%ld"), s_delay);

    delay = wxGetTextFromUser(_T("Enter delay (in milliseconds)"),
            _T("Set tooltip delay"),
            delay,
            this);
    if ( !delay )
        return; // cancelled

    wxSscanf(delay, _T("%ld"), &s_delay);

    wxToolTip::SetDelay(s_delay);

    wxLogStatus(this, _T("Tooltip delay set to %ld milliseconds"), s_delay);
}

void MyFrame::OnToggleTooltips(wxCommandEvent& WXUNUSED(event))
{
    static bool s_enabled = true;

    s_enabled = !s_enabled;

    wxToolTip::Enable(s_enabled);

    wxLogStatus(this, _T("Tooltips %sabled"), s_enabled ? _T("en") : _T("dis") );
}
#endif // tooltips

void MyFrame::OnEnableAll(wxCommandEvent& WXUNUSED(event))
{
    static bool s_enable = true;

    s_enable = !s_enable;
    m_panel->Enable(s_enable);
    static bool s_enableCheckbox = true;
    if ( !s_enable )
    {
        // this is a test for correct behaviour of either enabling or disabling
        // a child when its parent is disabled: the checkbox should have the
        // correct state when the parent is enabled back
        m_panel->m_checkbox->Enable(s_enableCheckbox);
        s_enableCheckbox = !s_enableCheckbox;
    }
}

void MyFrame::OnHideAll(wxCommandEvent& WXUNUSED(event))
{
    static bool s_show = true;

    s_show = !s_show;
    m_panel->Show(s_show);
}

void MyFrame::OnHideList(wxCommandEvent& WXUNUSED(event))
{
    static bool s_show = true;

    s_show = !s_show;
    m_panel->m_listbox->Show(s_show);
}

void MyFrame::OnContextHelp(wxCommandEvent& WXUNUSED(event))
{
    // starts a local event loop
    wxContextHelp chelp(this);
}

void MyFrame::OnMove( wxMoveEvent& event )
{
#if wxUSE_STATUSBAR
    UpdateStatusBar(event.GetPosition(), GetSize());
#endif // wxUSE_STATUSBAR

    event.Skip();
}

void MyFrame::OnIconized( wxIconizeEvent& event )
{
    wxLogMessage(_T("Frame %s"), event.Iconized() ? _T("iconized")
                                                  : _T("restored"));
    event.Skip();
}

void MyFrame::OnMaximized( wxMaximizeEvent& WXUNUSED(event) )
{
    wxLogMessage(_T("Frame maximized"));
}

void MyFrame::OnSize( wxSizeEvent& event )
{
#if wxUSE_STATUSBAR
    UpdateStatusBar(GetPosition(), event.GetSize());
#endif // wxUSE_STATUSBAR

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
                _T("Focus: %s, HWND = %08x"),
#else
                _T("Focus: %s"),
#endif
                s_windowFocus->GetClassInfo()->GetClassName()
#ifdef __WXMSW__
                , (unsigned int) s_windowFocus->GetHWND()
#endif
                  );

#if wxUSE_STATUSBAR
        SetStatusText(msg);
#endif // wxUSE_STATUSBAR
    }
}

void MyComboBox::OnChar(wxKeyEvent& event)
{
    wxLogMessage(_T("MyComboBox::OnChar"));

    if ( event.GetKeyCode() == 'w' )
        wxLogMessage(_T("MyComboBox: 'w' will be ignored."));
    else
        event.Skip();
}

void MyComboBox::OnKeyDown(wxKeyEvent& event)
{
    wxLogMessage(_T("MyComboBox::OnKeyDown"));

    if ( event.GetKeyCode() == 'w' )
        wxLogMessage(_T("MyComboBox: 'w' will be ignored."));
    else
        event.Skip();
}

void MyComboBox::OnKeyUp(wxKeyEvent& event)
{
    wxLogMessage(_T("MyComboBox::OnKeyUp"));

    event.Skip();
}

static void SetListboxClientData(const wxChar *name, wxListBox *control)
{
    size_t count = control->GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxString s;
        s.Printf(wxT("%s client data for '%s'"),
                 name, control->GetString(n).c_str());

        control->SetClientObject(n, new wxStringClientData(s));
    }
}

#if wxUSE_CHOICE

static void SetChoiceClientData(const wxChar *name, wxChoice *control)
{
    size_t count = control->GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        wxString s;
        s.Printf(wxT("%s client data for '%s'"),
                 name, control->GetString(n).c_str());

        control->SetClientObject(n, new wxStringClientData(s));
    }
}

#endif // wxUSE_CHOICE
