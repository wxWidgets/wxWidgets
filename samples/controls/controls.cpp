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

#include "wx/spinbutt.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/spinbutt.h"

#if wxUSE_CLIPBOARD
    #include "wx/dataobj.h"
    #include "wx/clipbrd.h"
#endif

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
    #undef wxUSE_SPINBUTTON
    #define wxUSE_SPINBUTTON 0
#endif // __WIN16__

#include "wx/progdlg.h"

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit();
};

// a text ctrl which allows to call different wxTextCtrl functions
// interactively by pressing function keys in it
class MyTextCtrl : public wxTextCtrl
{
public:
    MyTextCtrl(wxWindow *parent, wxWindowID id, const wxString &value,
               const wxPoint &pos, const wxSize &size, int style = 0)
        : wxTextCtrl(parent, id, value, pos, size, style) { }

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);

private:
    static inline char GetChar(bool on, char c) { return on ? c : '-'; }
    void LogEvent(const char *name, wxKeyEvent& event) const;

    DECLARE_EVENT_TABLE()
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
#ifndef wxUSE_SPINBUTTON
    void OnSpinUpdate( wxSpinEvent &event );
    void OnUpdateShowProgress( wxUpdateUIEvent& event );
    void OnShowProgress( wxCommandEvent &event );
#endif // wxUSE_SPINBUTTON
    void OnPasteFromClipboard( wxCommandEvent &event );
    void OnCopyToClipboard( wxCommandEvent &event );
    void OnMoveToEndOfText( wxCommandEvent &event );
    void OnMoveToEndOfEntry( wxCommandEvent &event );

    wxListBox     *m_listbox;
    wxChoice      *m_choice;
    wxComboBox    *m_combo;
    wxRadioBox    *m_radio;
    wxGauge       *m_gauge;
    wxSlider      *m_slider;
    wxButton      *m_fontButton;
#ifndef wxUSE_SPINBUTTON
    wxSpinButton  *m_spinbutton;
    wxButton      *m_btnProgress;
#endif
    wxTextCtrl    *m_spintext;
    MyTextCtrl    *m_multitext;
    MyTextCtrl    *m_textentry;
    wxCheckBox    *m_checkbox;

    wxTextCtrl    *m_text;
    wxNotebook    *m_notebook;

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
    void OnIdle( wxIdleEvent& event );
    void OnSize( wxSizeEvent& event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------
// main()
//----------------------------------------------------------------------

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
    MINIMAL_ENABLE_TOOLTIPS
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

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(MINIMAL_ABOUT, "&About\tF1");
    file_menu->Append(MINIMAL_QUIT, "E&xit\tAlt-X", "Quit controls sample");

    wxMenuBar *menu_bar = new wxMenuBar( wxMB_DOCKABLE );
    menu_bar->Append(file_menu, "&File");

#if wxUSE_TOOLTIPS
    wxMenu *tooltip_menu = new wxMenu;
    tooltip_menu->Append(MINIMAL_SET_TOOLTIP_DELAY, "Set &delay\tCtrl-D");
    tooltip_menu->AppendSeparator();
    tooltip_menu->Append(MINIMAL_ENABLE_TOOLTIPS, "&Toggle tooltips\tCrtl-T",
            "enable/disable tooltips", TRUE);
    tooltip_menu->Check(MINIMAL_ENABLE_TOOLTIPS, TRUE);
    menu_bar->Append(tooltip_menu, "&Tooltips");
#endif // wxUSE_TOOLTIPS

    frame->SetMenuBar(menu_bar);

    frame->Show(TRUE);
    frame->SetCursor(wxCursor(wxCURSOR_HAND));

    SetTopWindow(frame);

    return TRUE;
}

//----------------------------------------------------------------------
// MyTextCtrl
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN(MyTextCtrl::OnKeyDown)
    EVT_KEY_UP(MyTextCtrl::OnKeyUp)
    EVT_CHAR(MyTextCtrl::OnChar)
END_EVENT_TABLE()

void MyTextCtrl::LogEvent(const char *name, wxKeyEvent& event) const
{
    wxString key;
    long keycode = event.KeyCode();
    if ( isprint((int)keycode) )
        key.Printf("'%c'", (char)keycode);
    else
    {
        switch ( keycode )
        {
            case WXK_BACK: key = "BACK"; break;
            case WXK_TAB: key = "TAB"; break;
            case WXK_RETURN: key = "RETURN"; break;
            case WXK_ESCAPE: key = "ESCAPE"; break;
            case WXK_SPACE: key = "SPACE"; break;
            case WXK_DELETE: key = "DELETE"; break;
            case WXK_START: key = "START"; break;
            case WXK_LBUTTON: key = "LBUTTON"; break;
            case WXK_RBUTTON: key = "RBUTTON"; break;
            case WXK_CANCEL: key = "CANCEL"; break;
            case WXK_MBUTTON: key = "MBUTTON"; break;
            case WXK_CLEAR: key = "CLEAR"; break;
            case WXK_SHIFT: key = "SHIFT"; break;
            case WXK_CONTROL: key = "CONTROL"; break;
            case WXK_MENU: key = "MENU"; break;
            case WXK_PAUSE: key = "PAUSE"; break;
            case WXK_CAPITAL: key = "CAPITAL"; break;
            case WXK_PRIOR: key = "PRIOR"; break;
            case WXK_NEXT: key = "NEXT"; break;
            case WXK_END: key = "END"; break;
            case WXK_HOME: key = "HOME"; break;
            case WXK_LEFT: key = "LEFT"; break;
            case WXK_UP: key = "UP"; break;
            case WXK_RIGHT: key = "RIGHT"; break;
            case WXK_DOWN: key = "DOWN"; break;
            case WXK_SELECT: key = "SELECT"; break;
            case WXK_PRINT: key = "PRINT"; break;
            case WXK_EXECUTE: key = "EXECUTE"; break;
            case WXK_SNAPSHOT: key = "SNAPSHOT"; break;
            case WXK_INSERT: key = "INSERT"; break;
            case WXK_HELP: key = "HELP"; break;
            case WXK_NUMPAD0: key = "NUMPAD0"; break;
            case WXK_NUMPAD1: key = "NUMPAD1"; break;
            case WXK_NUMPAD2: key = "NUMPAD2"; break;
            case WXK_NUMPAD3: key = "NUMPAD3"; break;
            case WXK_NUMPAD4: key = "NUMPAD4"; break;
            case WXK_NUMPAD5: key = "NUMPAD5"; break;
            case WXK_NUMPAD6: key = "NUMPAD6"; break;
            case WXK_NUMPAD7: key = "NUMPAD7"; break;
            case WXK_NUMPAD8: key = "NUMPAD8"; break;
            case WXK_NUMPAD9: key = "NUMPAD9"; break;
            case WXK_MULTIPLY: key = "MULTIPLY"; break;
            case WXK_ADD: key = "ADD"; break;
            case WXK_SEPARATOR: key = "SEPARATOR"; break;
            case WXK_SUBTRACT: key = "SUBTRACT"; break;
            case WXK_DECIMAL: key = "DECIMAL"; break;
            case WXK_DIVIDE: key = "DIVIDE"; break;
            case WXK_F1: key = "F1"; break;
            case WXK_F2: key = "F2"; break;
            case WXK_F3: key = "F3"; break;
            case WXK_F4: key = "F4"; break;
            case WXK_F5: key = "F5"; break;
            case WXK_F6: key = "F6"; break;
            case WXK_F7: key = "F7"; break;
            case WXK_F8: key = "F8"; break;
            case WXK_F9: key = "F9"; break;
            case WXK_F10: key = "F10"; break;
            case WXK_F11: key = "F11"; break;
            case WXK_F12: key = "F12"; break;
            case WXK_F13: key = "F13"; break;
            case WXK_F14: key = "F14"; break;
            case WXK_F15: key = "F15"; break;
            case WXK_F16: key = "F16"; break;
            case WXK_F17: key = "F17"; break;
            case WXK_F18: key = "F18"; break;
            case WXK_F19: key = "F19"; break;
            case WXK_F20: key = "F20"; break;
            case WXK_F21: key = "F21"; break;
            case WXK_F22: key = "F22"; break;
            case WXK_F23: key = "F23"; break;
            case WXK_F24: key = "F24"; break;
            case WXK_NUMLOCK: key = "NUMLOCK"; break;
            case WXK_SCROLL: key = "SCROLL"; break;
            case WXK_PAGEUP: key = "PAGEUP"; break;
            case WXK_PAGEDOWN: key = "PAGEDOWN"; break;
            case WXK_NUMPAD_SPACE: key = "NUMPAD_SPACE"; break;
            case WXK_NUMPAD_TAB: key = "NUMPAD_TAB"; break;
            case WXK_NUMPAD_ENTER: key = "NUMPAD_ENTER"; break;
            case WXK_NUMPAD_F1: key = "NUMPAD_F1"; break;
            case WXK_NUMPAD_F2: key = "NUMPAD_F2"; break;
            case WXK_NUMPAD_F3: key = "NUMPAD_F3"; break;
            case WXK_NUMPAD_F4: key = "NUMPAD_F4"; break;
            case WXK_NUMPAD_HOME: key = "NUMPAD_HOME"; break;
            case WXK_NUMPAD_LEFT: key = "NUMPAD_LEFT"; break;
            case WXK_NUMPAD_UP: key = "NUMPAD_UP"; break;
            case WXK_NUMPAD_RIGHT: key = "NUMPAD_RIGHT"; break;
            case WXK_NUMPAD_DOWN: key = "NUMPAD_DOWN"; break;
            case WXK_NUMPAD_PRIOR: key = "NUMPAD_PRIOR"; break;
            case WXK_NUMPAD_PAGEUP: key = "NUMPAD_PAGEUP"; break;
            case WXK_NUMPAD_PAGEDOWN: key = "NUMPAD_PAGEDOWN"; break;
            case WXK_NUMPAD_END: key = "NUMPAD_END"; break;
            case WXK_NUMPAD_BEGIN: key = "NUMPAD_BEGIN"; break;
            case WXK_NUMPAD_INSERT: key = "NUMPAD_INSERT"; break;
            case WXK_NUMPAD_DELETE: key = "NUMPAD_DELETE"; break;
            case WXK_NUMPAD_EQUAL: key = "NUMPAD_EQUAL"; break;
            case WXK_NUMPAD_MULTIPLY: key = "NUMPAD_MULTIPLY"; break;
            case WXK_NUMPAD_ADD: key = "NUMPAD_ADD"; break;
            case WXK_NUMPAD_SEPARATOR: key = "NUMPAD_SEPARATOR"; break;
            case WXK_NUMPAD_SUBTRACT: key = "NUMPAD_SUBTRACT"; break;
            case WXK_NUMPAD_DECIMAL: key = "NUMPAD_DECIMAL"; break;

            default:
                key.Printf("unknown (%ld)", keycode);
        }
    }

    wxLogMessage( _T("%s event: %s (flags = %c%c%c%c)"),
                  name,
                  key.c_str(),
                  GetChar(event.ControlDown(), 'C'),
                  GetChar(event.AltDown(), 'A'),
                  GetChar(event.ShiftDown(), 'S'),
                  GetChar(event.MetaDown(), 'M'));

}

void MyTextCtrl::OnChar(wxKeyEvent& event)
{
    LogEvent("Char", event);

    event.Skip();
}

void MyTextCtrl::OnKeyUp(wxKeyEvent& event)
{
    LogEvent("Key up", event);

    event.Skip();
}

void MyTextCtrl::OnKeyDown(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_F1:
            // show current position and text length
            {
                long line, column, pos = GetInsertionPoint();
                PositionToXY(pos, &column, &line);

                wxLogMessage( _T("Current position: %ld\n"
                        "Current line, column: (%ld, %ld)\n"
                        "Number of lines: %ld\n"
                        "Current line length: %ld\n"
                        "Total text length: %ld"),
                        pos,
                        line, column,
                        GetNumberOfLines(),
                        GetLineLength(line),
                        GetLastPosition());
            }
            break;

        case WXK_F2:
            // go to the end
            SetInsertionPointEnd();
            break;

        case WXK_F3:
            // go to position 10
            SetInsertionPoint(10);
            break;
    }

    LogEvent("Key down", event);

    event.Skip();
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

const int  ID_CHOICE            = 120;
const int  ID_CHOICE_SEL_NUM    = 121;
const int  ID_CHOICE_SEL_STR    = 122;
const int  ID_CHOICE_CLEAR      = 123;
const int  ID_CHOICE_APPEND     = 124;
const int  ID_CHOICE_DELETE     = 125;
const int  ID_CHOICE_FONT       = 126;
const int  ID_CHOICE_ENABLE     = 127;

const int  ID_COMBO             = 140;
const int  ID_COMBO_SEL_NUM     = 141;
const int  ID_COMBO_SEL_STR     = 142;
const int  ID_COMBO_CLEAR       = 143;
const int  ID_COMBO_APPEND      = 144;
const int  ID_COMBO_DELETE      = 145;
const int  ID_COMBO_FONT        = 146;
const int  ID_COMBO_ENABLE      = 147;

const int  ID_TEXT              = 150;
const int  ID_PASTE_TEXT        = 151;
const int  ID_COPY_TEXT         = 152;
const int  ID_MOVE_END_ENTRY    = 153;
const int  ID_MOVE_END_ZONE     = 154;

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

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
EVT_SIZE      (                         MyPanel::OnSize)
EVT_NOTEBOOK_PAGE_CHANGING(ID_NOTEBOOK, MyPanel::OnPageChanging)
EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK,  MyPanel::OnPageChanged)
EVT_LISTBOX   (ID_LISTBOX,              MyPanel::OnListBox)
EVT_LISTBOX_DCLICK(ID_LISTBOX,          MyPanel::OnListBoxDoubleClick)
EVT_BUTTON    (ID_LISTBOX_SEL_NUM,      MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_SEL_STR,      MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_CLEAR,        MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_APPEND,       MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_DELETE,       MyPanel::OnListBoxButtons)
EVT_BUTTON    (ID_LISTBOX_FONT,         MyPanel::OnListBoxButtons)
EVT_CHECKBOX  (ID_LISTBOX_ENABLE,       MyPanel::OnListBoxButtons)
EVT_CHOICE    (ID_CHOICE,               MyPanel::OnChoice)
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
#ifndef wxUSE_SPINBUTTON
EVT_SPIN      (ID_SPIN,                 MyPanel::OnSpinUpdate)
EVT_UPDATE_UI (ID_BTNPROGRESS,          MyPanel::OnUpdateShowProgress)
EVT_BUTTON    (ID_BTNPROGRESS,          MyPanel::OnShowProgress)
#endif
EVT_BUTTON    (ID_PASTE_TEXT,           MyPanel::OnPasteFromClipboard)
EVT_BUTTON    (ID_COPY_TEXT,            MyPanel::OnCopyToClipboard)
EVT_BUTTON    (ID_MOVE_END_ZONE,        MyPanel::OnMoveToEndOfText)
EVT_BUTTON    (ID_MOVE_END_ENTRY,       MyPanel::OnMoveToEndOfEntry)
END_EVENT_TABLE()

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) ),
         m_text(NULL), m_notebook(NULL)
{
    //  SetBackgroundColour("cadet blue");

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

    wxButton *button = (wxButton*) NULL;  /* who did this ? */
    wxPanel *panel = (wxPanel*) NULL;

    panel = new wxPanel(m_notebook);
    m_listbox = new wxListBox( panel, ID_LISTBOX, wxPoint(10,10), wxSize(120,70), 5, choices, wxLB_ALWAYS_SB );
    m_listbox->SetCursor(*wxCROSS_CURSOR);
#if wxUSE_TOOLTIPS
    m_listbox->SetToolTip( "This is a list box" );
#endif // wxUSE_TOOLTIPS

    (void)new wxButton( panel, ID_LISTBOX_SEL_NUM, "Select #2", wxPoint(180,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_SEL_STR, "Select 'This'", wxPoint(340,30), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_CLEAR, "Clear", wxPoint(180,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_APPEND, "Append 'Hi!'", wxPoint(340,80), wxSize(140,30) );
    (void)new wxButton( panel, ID_LISTBOX_DELETE, "Delete selected item", wxPoint(180,130), wxSize(140,30) );
    button = new wxButton( panel, ID_LISTBOX_FONT, "Set &Italic font", wxPoint(340,130), wxSize(140,30) );
#if wxUSE_TOOLTIPS
    button->SetToolTip( "Press here to set italic font" );
#endif // wxUSE_TOOLTIPS

    m_checkbox = new wxCheckBox( panel, ID_LISTBOX_ENABLE, "&Disable", wxPoint(20,130), wxSize(-1, -1), wxALIGN_RIGHT );
    m_checkbox->SetValue(FALSE);
#if wxUSE_TOOLTIPS
    m_checkbox->SetToolTip( "Click here to disable the listbox" );
#endif // wxUSE_TOOLTIPS
    m_notebook->AddPage(panel, "wxListBox", TRUE, Image_List);

    panel = new wxPanel(m_notebook);
    m_choice = new wxChoice( panel, ID_CHOICE, wxPoint(10,10), wxSize(120,-1), 5, choices );
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

    panel = new wxPanel(m_notebook);
    m_textentry = new MyTextCtrl( panel, -1, "Some text.", wxPoint(10,10), wxSize(320,28), //0);
    wxTE_PROCESS_ENTER);
    (*m_textentry) << " Appended.";
    m_textentry->SetInsertionPoint(0);
    m_textentry->WriteText( "Prepended. " );

    m_multitext = new MyTextCtrl( panel, ID_TEXT, "Some text.", wxPoint(10,50), wxSize(320,70), wxTE_MULTILINE );
    (*m_multitext) << " Appended.";
    m_multitext->SetInsertionPoint(0);
    m_multitext->WriteText( "Prepended. " );
    m_multitext->AppendText( "\nPress function keys for test different tests." );

    (*m_multitext) << "\nDoes it have cross cursor?";
    m_multitext->SetCursor(*wxCROSS_CURSOR);

    new MyTextCtrl( panel, -1, "This one is with wxTE_PROCESS_TAB style.", wxPoint(10,120), wxSize(320,70), wxTE_MULTILINE | wxTE_PROCESS_TAB);
#if wxUSE_TOOLTIPS
    m_multitext->AppendText( "\nThis ctrl has a tooltip. " );
    m_multitext->SetToolTip("Press F1 here.");
#endif // wxUSE_TOOLTIPS

    (void)new wxStaticBox( panel, -1, "&Move cursor to the end of:", wxPoint(345, 0), wxSize(160, 100) );
    (void)new wxButton( panel, ID_MOVE_END_ENTRY, "Text &entry", wxPoint(370, 20), wxSize(110, 30) );
    (void)new wxButton( panel, ID_MOVE_END_ZONE, "Text &zone", wxPoint(370, 60), wxSize(110, 30) );
    (void)new wxStaticBox( panel, -1, "wx&Clipboard", wxPoint(345,100), wxSize(160,100) );
    (void)new wxButton( panel, ID_COPY_TEXT, "C&opy line 1", wxPoint(370,120), wxSize(110,30) );
    (void)new wxButton( panel, ID_PASTE_TEXT, "&Paste text", wxPoint(370,160), wxSize(110,30) );
    m_notebook->AddPage( panel, "wxTextCtrl" , FALSE, Image_Text );

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
    wxRadioButton *rb = new wxRadioButton( panel, ID_RADIOBUTTON_1, "Radiobutton1", wxPoint(210,170), wxSize(110,30) );
    rb->SetValue( FALSE );
    (void)new wxRadioButton( panel, ID_RADIOBUTTON_2, "Radiobutton2", wxPoint(340,170), wxSize(110,30) );
    m_notebook->AddPage(panel, "wxRadioBox", FALSE, Image_Radio);

    panel = new wxPanel(m_notebook);
    (void)new wxStaticBox( panel, -1, "wxGauge and wxSlider", wxPoint(10,10), wxSize(180,130) );
    m_gauge = new wxGauge( panel, -1, 200, wxPoint(18,50), wxSize(155, 30) );
    m_slider = new wxSlider( panel, ID_SLIDER, 0, 0, 200, wxPoint(18,90), wxSize(155,-1), wxSL_LABELS );
    (void)new wxStaticBox( panel, -1, "Explanation", wxPoint(200,10), wxSize(290,130) );
#ifdef __WXMOTIF__
    // No wrapping text in wxStaticText yet :-(
    (void)new wxStaticText( panel, -1,
                            "Drag the slider!",
                            wxPoint(208,30),
                            wxSize(210, -1)
                          );
#else
    (void)new wxStaticText( panel, -1,
                            "In order see the gauge (aka progress bar)\n"
                            "control do something you have to drag the\n"
                            "handle of the slider to the right.\n"
                            "\n"
                            "This is also supposed to demonstrate how\n"
                            "to use static controls.\n",
                            wxPoint(208,25),
                            wxSize(250, 110)
                          );
#endif
    m_spintext = new wxTextCtrl( panel, -1, "0", wxPoint(20,160), wxSize(80,-1) );
#ifndef wxUSE_SPINBUTTON
    m_spinbutton = new wxSpinButton( panel, ID_SPIN, wxPoint(103,159), wxSize(-1,-1) );
    m_spinbutton->SetRange(-10,30);
    m_spinbutton->SetValue(-5);

    m_btnProgress = new wxButton( panel, ID_BTNPROGRESS, "Show progress dialog",
                                  wxPoint(208, 159) );
#endif
    m_notebook->AddPage(panel, "wxGauge", FALSE, Image_Gauge);
}

void MyPanel::OnPasteFromClipboard( wxCommandEvent &WXUNUSED(event) )
{
    // We test for wxUSE_DRAG_AND_DROP also, because data objects
    // may not be implemented for compilers that can't cope with the OLE
    // parts in wxUSE_DRAG_AND_DROP.

#if wxUSE_CLIPBOARD && wxUSE_DRAG_AND_DROP
    if (!wxTheClipboard->Open())
    {
        *m_text << "Error opening the clipboard.\n";

        return;
    }
    else
    {
        *m_text << "Successfully opened the clipboard.\n";
    }

    wxTextDataObject data;

    if (wxTheClipboard->IsSupported( data.GetFormat() ))
    {
        *m_text << "Clipboard supports requested format.\n";

        if (wxTheClipboard->GetData( &data ))
        {
            *m_text << "Successfully retrieved data from the clipboard.\n";
            *m_multitext << data.GetText() << "\n";
        }
        else
        {
            *m_text << "Error getting data from the clipboard.\n";
        }
    }
    else
    {
        *m_text << "Clipboard doesn't support requested format.\n";
    }

    wxTheClipboard->Close();

    *m_text << "Closed the clipboard.\n";
#else
    wxLogError("Your version of wxWindows is compiled without clipboard support.");
#endif
}

void MyPanel::OnCopyToClipboard( wxCommandEvent &WXUNUSED(event) )
{
#if wxUSE_CLIPBOARD && wxUSE_DRAG_AND_DROP
    wxString text( m_multitext->GetLineText(0) );

    if (text.IsEmpty())
    {
        *m_text << "No text to copy.\n";

        return;
    }

    if (!wxTheClipboard->Open())
    {
        *m_text << "Error opening the clipboard.\n";

        return;
    }
    else
    {
        *m_text << "Successfully opened the clipboard.\n";
    }

    wxTextDataObject *data = new wxTextDataObject( text );

    if (!wxTheClipboard->SetData( data ))
    {
        *m_text << "Error while copying to the clipboard.\n";
    }
    else
    {
        *m_text << "Successfully copied data to the clipboard.\n";
    }

    wxTheClipboard->Close();

    *m_text << "Closed the clipboard.\n";
#else
    wxLogError("Your version of wxWindows is compiled without clipboard support.");
#endif
}

void MyPanel::OnMoveToEndOfText( wxCommandEvent &event )
{
    m_multitext->SetInsertionPointEnd();
    m_multitext->SetFocus();
}

void MyPanel::OnMoveToEndOfEntry( wxCommandEvent &event )
{
    m_textentry->SetInsertionPointEnd();
    m_textentry->SetFocus();
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
    int selNew = event.GetSelection(),
        selOld = event.GetOldSelection();
    if ( selOld == 2 && selNew == 4 )
    {
        wxMessageBox("This demonstrates how a program may prevent the "
                     "page change from taking place - the current page will "
                     "stay the third one", "Conntrol sample",
                     wxICON_INFORMATION | wxOK);

        event.Veto();
    }
    else
    {
        *m_text << "Notebook selection is being changed from "
                << selOld << " to " << selNew << "\n";
    }
}

void MyPanel::OnPageChanged( wxNotebookEvent &event )
{
    *m_text << "Notebook selection is " << event.GetSelection() << "\n";
}

void MyPanel::OnListBox( wxCommandEvent &event )
{
    m_text->AppendText( "ListBox event selection string is: " );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "\n" );
    m_text->AppendText( "ListBox control selection string is: " );
    m_text->AppendText( m_listbox->GetStringSelection() );
    m_text->AppendText( "\n" );
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
                break;
            }
        case ID_LISTBOX_SEL_NUM:
            {
                m_listbox->SetSelection( 2 );
                break;
            }
        case ID_LISTBOX_SEL_STR:
            {
                m_listbox->SetStringSelection( "This" );
                break;
            }
        case ID_LISTBOX_CLEAR:
            {
                m_listbox->Clear();
                break;
            }
        case ID_LISTBOX_APPEND:
            {
                m_listbox->Append( "Hi!" );
                break;
            }
        case ID_LISTBOX_DELETE:
            {
                int idx = m_listbox->GetSelection();
                m_listbox->Delete( idx );
                break;
            }
        case ID_LISTBOX_FONT:
            {
                m_listbox->SetFont( *wxITALIC_FONT );
                m_checkbox->SetFont( *wxITALIC_FONT );
                break;
            }
    }
}

void MyPanel::OnChoice( wxCommandEvent &event )
{
    m_text->AppendText( "Choice event selection string is: " );
    m_text->AppendText( event.GetString() );
    m_text->AppendText( "\n" );
    m_text->AppendText( "Choice control selection string is: " );
    m_text->AppendText( m_choice->GetStringSelection() );
    m_text->AppendText( "\n" );
}

void MyPanel::OnChoiceButtons( wxCommandEvent &event )
{
    switch (event.GetId())
    {
        case ID_CHOICE_ENABLE:
            {
                m_choice->Enable( event.GetInt() == 0 );
                break;
            }
        case ID_CHOICE_SEL_NUM:
            {
                m_choice->SetSelection( 2 );
                break;
            }
        case ID_CHOICE_SEL_STR:
            {
                m_choice->SetStringSelection( "This" );
                break;
            }
        case ID_CHOICE_CLEAR:
            {
                m_choice->Clear();
                break;
            }
        case ID_CHOICE_APPEND:
            {
                m_choice->Append( "Hi!" );
                break;
            }
        case ID_CHOICE_DELETE:
            {
                int idx = m_choice->GetSelection();
                m_choice->Delete( idx );
                break;
            }
        case ID_CHOICE_FONT:
            {
                m_choice->SetFont( *wxITALIC_FONT );
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

void MyPanel::OnSliderUpdate( wxCommandEvent &WXUNUSED(event) )
{
    m_gauge->SetValue( m_slider->GetValue() );
}

#ifndef wxUSE_SPINBUTTON
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
                            FALSE,  // modal
                            TRUE);  // has abort button

    bool cont = TRUE;
    for ( int i = 0; i < max && cont; i++ )
    {
        wxSleep(1);
        if ( i == max / 2 )
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

#endif // wxUSE_SPINBUTTON

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
EVT_SIZE(MyFrame::OnSize)
EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h)
: wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    CreateStatusBar(2);

    (void)new MyPanel( this, 10, 10, 300, 100 );
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

void MyFrame::OnSize( wxSizeEvent& event )
{
    wxString msg;
    msg.Printf( _("%dx%d"), event.GetSize().x, event.GetSize().y);
    SetStatusText(msg, 1);

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
