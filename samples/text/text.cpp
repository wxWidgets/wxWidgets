/////////////////////////////////////////////////////////////////////////////
// Name:        text.cpp
// Purpose:     TextCtrl wxWidgets sample
// Author:      Robert Roebling
// Copyright:   (c) Robert Roebling, Julian Smart, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_CLIPBOARD
    #include "wx/dataobj.h"
    #include "wx/clipbrd.h"
#endif

#if wxUSE_FILE
    #include "wx/file.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

// We test for wxUSE_DRAG_AND_DROP also, because data objects may not be
// implemented for compilers that can't cope with the OLE parts in
// wxUSE_DRAG_AND_DROP.
#if !wxUSE_DRAG_AND_DROP
    #undef wxUSE_CLIPBOARD
    #define wxUSE_CLIPBOARD 0
#endif

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/tokenzr.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit() override;
};

// a text ctrl which allows to call different wxTextCtrl functions
// interactively by pressing function keys in it
class MyTextCtrl : public wxTextCtrl
{
public:
    MyTextCtrl(wxWindow *parent, wxWindowID id, const wxString &value,
               const wxPoint &pos, const wxSize &size, int style = 0)
        : wxTextCtrl(parent, id, value, pos, size, style)
    {
        m_hasCapture = false;
    }

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);

    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTextURL(wxTextUrlEvent& event);
    void OnTextMaxLen(wxCommandEvent& event);

    void OnTextCut(wxClipboardTextEvent & event);
    void OnTextCopy(wxClipboardTextEvent & event);
    void OnTextPaste(wxClipboardTextEvent & event);

    void OnMouseEvent(wxMouseEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);

    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    static bool ms_logKey;
    static bool ms_logChar;
    static bool ms_logMouse;
    static bool ms_logText;
    static bool ms_logFocus;
    static bool ms_logClip;

private:
    static inline wxChar GetChar(bool on, wxChar c) { return on ? c : '-'; }

    void LogKeyEvent(const wxString& name, wxKeyEvent& event) const;
    void LogClipEvent(const wxString& what, wxClipboardTextEvent& event);

    bool m_hasCapture;

    wxDECLARE_EVENT_TABLE();
};

class MyPanel: public wxPanel
{
public:
    MyPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~MyPanel()
    {
#if wxUSE_LOG
        delete wxLog::SetActiveTarget(m_logOld);
#endif // wxUSE_LOG
    }

#if wxUSE_CLIPBOARD
    void DoPasteFromClipboard();
    void DoCopyToClipboard();
#endif // wxUSE_CLIPBOARD

    void DoRemoveText();
    void DoReplaceText();
    void DoSelectText();
    void DoMoveToEndOfText();
    void DoMoveToEndOfEntry();
    void DoGetWindowCoordinates();

    // return true if currently text control has any selection
    bool HasSelection() const
    {
        long from, to;
        GetFocusedText()->GetSelection(&from, &to);
        return from != to;
    }

    MyTextCtrl    *m_text;
    MyTextCtrl    *m_password;
    MyTextCtrl    *m_enter;
    MyTextCtrl    *m_tab;
    MyTextCtrl    *m_readonly;
    MyTextCtrl    *m_limited;

    MyTextCtrl    *m_multitext;
    MyTextCtrl    *m_horizontal;

    MyTextCtrl    *m_textrich;

#if wxUSE_LOG
    wxTextCtrl    *m_log;
    wxLog         *m_logOld;
#endif // wxUSE_LOG

private:
    // get the currently focused text control or return the default one
    // (m_multitext) is no text ctrl has focus -- in any case, returns
    // something non null
    wxTextCtrl *GetFocusedText() const;
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, int x, int y);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
#if wxUSE_TOOLTIPS
    void OnSetTooltipDelay(wxCommandEvent& event);
    void OnToggleTooltips(wxCommandEvent& event);
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    void OnPasteFromClipboard( wxCommandEvent& WXUNUSED(event) )
    {
        wxLogMessage("Pasting text from clipboard.");
        m_panel->DoPasteFromClipboard();
    }
    void OnCopyToClipboard( wxCommandEvent& WXUNUSED(event) )
    {
        wxLogMessage("Copying text to clipboard.");
        m_panel->DoCopyToClipboard();
    }

    void OnUpdatePasteFromClipboard(wxUpdateUIEvent& event)
    {
        wxClipboardLocker lockClip;

        event.Enable( wxTheClipboard->IsSupported(wxDF_TEXT) );
    }

    void OnUpdateCopyToClipboard(wxUpdateUIEvent& event)
    {
        event.Enable( m_panel->HasSelection() );
    }
#endif // wxUSE_CLIPBOARD

    void OnAddTextLine(wxCommandEvent& WXUNUSED(event))
    {
        static int s_n = 0;
        m_panel->m_textrich->AppendText(wxString::Format("Line %d\n", ++s_n));
    }

    void OnAddTextFreeze( wxCommandEvent& WXUNUSED(event) )
        { DoAddText(true); }
    void OnAddText( wxCommandEvent& WXUNUSED(event) )
        { DoAddText(false); }
    void OnRemoveText( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoRemoveText(); }
    void OnReplaceText( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoReplaceText(); }
    void OnSelectText( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoSelectText(); }

    void OnMoveToEndOfText( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoMoveToEndOfText(); }

    void OnGetWindowCoordinates( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoGetWindowCoordinates(); }

    void OnMoveToEndOfEntry( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoMoveToEndOfEntry(); }

    void OnScrollLineDown(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->LineDown() )
        {
            wxLogMessage("Already at the bottom");
        }
    }

    void OnScrollLineUp(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->LineUp() )
        {
            wxLogMessage("Already at the top");
        }
    }

    void OnScrollPageDown(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->PageDown() )
        {
            wxLogMessage("Already at the bottom");
        }
    }

    void OnScrollPageUp(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->PageUp() )
        {
            wxLogMessage("Already at the top");
        }
    }

    void OnGetLine(wxCommandEvent& WXUNUSED(event))
    {
        long nLine = wxGetNumberFromUser("Which line would you like to get?",
                                         "Enter which line you would like to get",
                                         "Get a line from the tabbed multiline text control" );

        wxMessageBox(m_panel->m_tab->GetLineText(nLine));
    }

    void OnGetLineLength(wxCommandEvent& WXUNUSED(event))
    {
        long nLine = wxGetNumberFromUser("Which line would you like to get?",
                                         "Enter which line you would like to get",
                                         "Get length of a line from the tabbed multiline text control" );

        wxMessageBox(wxString::Format("Length of line %i is:%i",
                                      (int) nLine,
                                      m_panel->m_tab->GetLineLength(nLine))
                    );
    }

#if wxUSE_LOG
    void OnLogClear(wxCommandEvent& event);
#endif // wxUSE_LOG
    void OnFileSave(wxCommandEvent& event);
    void OnFileLoad(wxCommandEvent& event);
    void OnRichTextTest(wxCommandEvent& event);

    void OnSetEditable(wxCommandEvent& event);
    void OnSetEnabled(wxCommandEvent& event);

    void OnLogKey(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logKey = event.IsChecked();
    }

    void OnLogChar(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logChar = event.IsChecked();
    }

    void OnLogMouse(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logMouse = event.IsChecked();
    }

    void OnLogText(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logText = event.IsChecked();
    }

    void OnLogFocus(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logFocus = event.IsChecked();
    }

    void OnLogClip(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logClip = event.IsChecked();
    }

    void OnSetText(wxCommandEvent& WXUNUSED(event))
    {
        m_panel->m_text->SetValue("Hello, world! (what else did you expect?)");
    }

    void OnChangeText(wxCommandEvent& WXUNUSED(event))
    {
        m_panel->m_text->ChangeValue("Changed, not set: no event");
    }

    void OnIdle( wxIdleEvent& event );

private:
    void DoAddText(bool freeze)
    {
        wxTextCtrl * const text = m_panel->m_textrich;

        if ( freeze )
            text->Freeze();

        text->Clear();

        for ( int i = 0; i < 100; i++ )
        {
            text->AppendText(wxString::Format("Line %i\n", i));
        }

        text->SetInsertionPoint(0);

        if ( freeze )
            text->Thaw();
    }

    MyPanel *m_panel;

    wxDECLARE_EVENT_TABLE();
};

/*
 * RichTextFrame is used to demonstrate rich text behaviour
 */

class RichTextFrame: public wxFrame
{
public:
    RichTextFrame(wxWindow* parent, const wxString& title);

// Event handlers

    void OnClose(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnLeftAlign(wxCommandEvent& event);
    void OnRightAlign(wxCommandEvent& event);
    void OnJustify(wxCommandEvent& event);
    void OnCentre(wxCommandEvent& event);
    void OnChangeFont(wxCommandEvent& event);
    void OnChangeTextColour(wxCommandEvent& event);
    void OnChangeBackgroundColour(wxCommandEvent& event);
    void OnLeftIndent(wxCommandEvent& event);
    void OnRightIndent(wxCommandEvent& event);
    void OnTabStops(wxCommandEvent& event);

private:
    wxTextCtrl *m_textCtrl;
    long m_currentPosition;

    wxDECLARE_EVENT_TABLE();
};

//----------------------------------------------------------------------
// main()
//----------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

//----------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------

enum
{
    TEXT_QUIT = wxID_EXIT,
    TEXT_ABOUT = wxID_ABOUT,
    TEXT_LOAD = 101,
    TEXT_SAVE,
    TEXT_CLEAR,
    TEXT_RICH_TEXT_TEST,

    // clipboard menu
    TEXT_CLIPBOARD_COPY = 200,
    TEXT_CLIPBOARD_PASTE,
    TEXT_CLIPBOARD_VETO,

    // tooltip menu
    TEXT_TOOLTIPS_SETDELAY = 300,
    TEXT_TOOLTIPS_ENABLE,

    // text menu
    TEXT_ADD_SOME = 400,
    TEXT_ADD_FREEZE,
    TEXT_ADD_LINE,
    TEXT_MOVE_ENDTEXT,
    TEXT_GET_WINDOW_COORD,
    TEXT_MOVE_ENDENTRY,
    TEXT_SET_EDITABLE,
    TEXT_SET_ENABLED,
    TEXT_LINE_DOWN,
    TEXT_LINE_UP,
    TEXT_PAGE_DOWN,
    TEXT_PAGE_UP,

    TEXT_GET_LINE,
    TEXT_GET_LINELENGTH,

    TEXT_REMOVE,
    TEXT_REPLACE,
    TEXT_SELECT,
    TEXT_SET,
    TEXT_CHANGE,

    // log menu
    TEXT_LOG_KEY,
    TEXT_LOG_CHAR,
    TEXT_LOG_MOUSE,
    TEXT_LOG_TEXT,
    TEXT_LOG_FOCUS,
    TEXT_LOG_CLIP,

    TEXT_END
};

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame *frame = new MyFrame("Text wxWidgets sample", 50, 50);

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(TEXT_SAVE, "&Save file\tCtrl-S",
                      "Save the text control contents to file");
    file_menu->Append(TEXT_LOAD, "&Load file\tCtrl-O",
                      "Load the sample file into text control");
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_RICH_TEXT_TEST, "Show Rich Text Editor");
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_ABOUT, "&About\tAlt-A");
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_QUIT, "E&xit\tAlt-X", "Quit this sample");

    wxMenuBar *menu_bar = new wxMenuBar( wxMB_DOCKABLE );
    menu_bar->Append(file_menu, "&File");

#if wxUSE_TOOLTIPS
    wxMenu *tooltip_menu = new wxMenu;
    tooltip_menu->Append(TEXT_TOOLTIPS_SETDELAY, "Set &delay\tCtrl-D");
    tooltip_menu->AppendSeparator();
    tooltip_menu->Append(TEXT_TOOLTIPS_ENABLE, "&Toggle tooltips\tCtrl-T",
            "enable/disable tooltips", true);
    tooltip_menu->Check(TEXT_TOOLTIPS_ENABLE, true);
    menu_bar->Append(tooltip_menu, "&Tooltips");
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    // notice that we use non default accelerators on purpose here to compare
    // their behaviour with the built in handling of standard Ctrl/Cmd-C/V
    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(TEXT_CLIPBOARD_COPY, "&Copy\tCtrl-Shift-C",
                          "Copy the selection to the clipboard");
    menuClipboard->Append(TEXT_CLIPBOARD_PASTE, "&Paste\tCtrl-Shift-V",
                          "Paste from clipboard to the text control");
    menuClipboard->AppendSeparator();
    menuClipboard->AppendCheckItem(TEXT_CLIPBOARD_VETO, "Vet&o\tCtrl-Shift-O",
                                   "Veto all clipboard operations");
    menu_bar->Append(menuClipboard, "&Clipboard");
#endif // wxUSE_CLIPBOARD

    wxMenu *menuText = new wxMenu;
    menuText->Append(TEXT_ADD_SOME, "&Append some text\tCtrl-A");
    menuText->Append(TEXT_ADD_FREEZE, "&Append text with freeze/thaw\tShift-Ctrl-A");
    menuText->Append(TEXT_ADD_LINE, "Append a new &line\tAlt-Shift-A");
    menuText->Append(TEXT_REMOVE, "&Remove first 10 characters\tCtrl-Y");
    menuText->Append(TEXT_REPLACE, "&Replace characters 4 to 8 with ABC\tCtrl-R");
    menuText->Append(TEXT_SELECT, "&Select characters 4 to 8\tCtrl-I");
    menuText->Append(TEXT_SET, "&Set the first text zone value\tCtrl-E");
    menuText->Append(TEXT_CHANGE, "&Change the first text zone value\tShift-Ctrl-E");
    menuText->AppendSeparator();
    menuText->Append(TEXT_MOVE_ENDTEXT, "Move cursor to the end of &text");
    menuText->Append(TEXT_MOVE_ENDENTRY, "Move cursor to the end of &entry");
    menuText->AppendCheckItem(TEXT_SET_EDITABLE, "Toggle &editable state");
    menuText->AppendCheckItem(TEXT_SET_ENABLED, "Toggle e&nabled state");
    menuText->Check(TEXT_SET_EDITABLE, true);
    menuText->Check(TEXT_SET_ENABLED, true);
    menuText->AppendSeparator();
    menuText->Append(TEXT_LINE_DOWN, "Scroll text one line down");
    menuText->Append(TEXT_LINE_UP, "Scroll text one line up");
    menuText->Append(TEXT_PAGE_DOWN, "Scroll text one page down");
    menuText->Append(TEXT_PAGE_UP, "Scroll text one page up");
    menuText->Append(TEXT_GET_WINDOW_COORD, "Get window coordinates");
    menuText->AppendSeparator();
    menuText->Append(TEXT_GET_LINE, "Get the text of a line of the tabbed multiline");
    menuText->Append(TEXT_GET_LINELENGTH, "Get the length of a line of the tabbed multiline");
    menu_bar->Append(menuText, "Te&xt");

#if wxUSE_LOG
    wxMenu *menuLog = new wxMenu;
    menuLog->AppendCheckItem(TEXT_LOG_KEY, "Log &key events");
    menuLog->AppendCheckItem(TEXT_LOG_CHAR, "Log &char events");
    menuLog->AppendCheckItem(TEXT_LOG_MOUSE, "Log &mouse events");
    menuLog->AppendCheckItem(TEXT_LOG_TEXT, "Log &text events");
    menuLog->AppendCheckItem(TEXT_LOG_FOCUS, "Log &focus events");
    menuLog->AppendCheckItem(TEXT_LOG_CLIP, "Log clip&board events");
    menuLog->AppendSeparator();
    menuLog->Append(TEXT_CLEAR, "&Clear the log\tCtrl-L",
                    "Clear the log window contents");

    // select only the interesting events by default
    MyTextCtrl::ms_logClip =
    MyTextCtrl::ms_logText = true;

    menuLog->Check(TEXT_LOG_KEY, MyTextCtrl::ms_logKey);
    menuLog->Check(TEXT_LOG_CHAR, MyTextCtrl::ms_logChar);
    menuLog->Check(TEXT_LOG_TEXT, MyTextCtrl::ms_logText);

    menu_bar->Append(menuLog, "&Log");
#endif // wxUSE_LOG

    frame->SetMenuBar(menu_bar);

    frame->Show(true);

    // report success
    return true;
}

//----------------------------------------------------------------------
// MyTextCtrl
//----------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN(MyTextCtrl::OnKeyDown)
    EVT_KEY_UP(MyTextCtrl::OnKeyUp)
    EVT_CHAR(MyTextCtrl::OnChar)

    EVT_TEXT(wxID_ANY, MyTextCtrl::OnText)
    EVT_TEXT_ENTER(wxID_ANY, MyTextCtrl::OnTextEnter)
    EVT_TEXT_URL(wxID_ANY, MyTextCtrl::OnTextURL)
    EVT_TEXT_MAXLEN(wxID_ANY, MyTextCtrl::OnTextMaxLen)
    EVT_TEXT_CUT(wxID_ANY,   MyTextCtrl::OnTextCut)
    EVT_TEXT_COPY(wxID_ANY,  MyTextCtrl::OnTextCopy)
    EVT_TEXT_PASTE(wxID_ANY, MyTextCtrl::OnTextPaste)

    EVT_MOUSE_EVENTS(MyTextCtrl::OnMouseEvent)
    EVT_CONTEXT_MENU(MyTextCtrl::OnContextMenu)

    EVT_SET_FOCUS(MyTextCtrl::OnSetFocus)
    EVT_KILL_FOCUS(MyTextCtrl::OnKillFocus)
wxEND_EVENT_TABLE()

bool MyTextCtrl::ms_logKey = false;
bool MyTextCtrl::ms_logChar = false;
bool MyTextCtrl::ms_logMouse = false;
bool MyTextCtrl::ms_logText = false;
bool MyTextCtrl::ms_logFocus = false;
bool MyTextCtrl::ms_logClip = false;

void MyTextCtrl::LogKeyEvent(const wxString& name, wxKeyEvent& event) const
{
    wxString key;
    const int keycode = event.GetKeyCode();
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
            case WXK_ALT: key = "ALT"; break;
            case WXK_CONTROL: key = "CONTROL"; break;
            case WXK_MENU: key = "MENU"; break;
            case WXK_PAUSE: key = "PAUSE"; break;
            case WXK_CAPITAL: key = "CAPITAL"; break;
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
            case WXK_BROWSER_BACK: key = "BROWSER_BACK"; break;
            case WXK_BROWSER_FORWARD: key = "BROWSER_FORWARD"; break;
            case WXK_BROWSER_REFRESH: key = "BROWSER_REFRESH"; break;
            case WXK_BROWSER_STOP: key = "BROWSER_STOP"; break;
            case WXK_BROWSER_SEARCH: key = "BROWSER_SEARCH"; break;
            case WXK_BROWSER_FAVORITES: key = "BROWSER_FAVORITES"; break;
            case WXK_BROWSER_HOME: key = "BROWSER_HOME"; break;
            case WXK_VOLUME_MUTE: key = "VOLUME_MUTE"; break;
            case WXK_VOLUME_DOWN: key = "VOLUME_DOWN"; break;
            case WXK_VOLUME_UP: key = "VOLUME_UP"; break;
            case WXK_MEDIA_NEXT_TRACK: key = "MEDIA_NEXT_TRACK"; break;
            case WXK_MEDIA_PREV_TRACK: key = "MEDIA_PREV_TRACK"; break;
            case WXK_MEDIA_STOP: key = "MEDIA_STOP"; break;
            case WXK_MEDIA_PLAY_PAUSE: key = "MEDIA_PLAY_PAUSE"; break;
            case WXK_LAUNCH_MAIL: key = "LAUNCH_MAIL"; break;
            case WXK_LAUNCH_APP1: key = "LAUNCH_APP1"; break;
            case WXK_LAUNCH_APP2: key = "LAUNCH_APP2"; break;

            default:
            {
               if ( wxIsprint(keycode) )
                   key.Printf("'%c'", keycode);
               else if ( keycode > 0 && keycode < 27 )
                   key.Printf(_("Ctrl-%c"), 'A' + keycode - 1);
               else
                   key.Printf("unknown (%d)", keycode);
            }
        }
    }

    key += wxString::Format(" (Unicode: %#04x)", event.GetUnicodeKey());

    wxLogMessage( "%s event: %s (flags = %c%c%c%c)",
                  name,
                  key,
                  GetChar( event.ControlDown(), 'C' ),
                  GetChar( event.AltDown(), 'A' ),
                  GetChar( event.ShiftDown(), 'S' ),
                  GetChar( event.MetaDown(), 'M' ) );
}

static wxString GetMouseEventDesc(const wxMouseEvent& ev)
{
    // click event
    wxString button;
    bool dbl, up;
    if ( ev.LeftDown() || ev.LeftUp() || ev.LeftDClick() )
    {
        button = "Left";
        dbl = ev.LeftDClick();
        up = ev.LeftUp();
    }
    else if ( ev.MiddleDown() || ev.MiddleUp() || ev.MiddleDClick() )
    {
        button = "Middle";
        dbl = ev.MiddleDClick();
        up = ev.MiddleUp();
    }
    else if ( ev.RightDown() || ev.RightUp() || ev.RightDClick() )
    {
        button = "Right";
        dbl = ev.RightDClick();
        up = ev.RightUp();
    }
    else if ( ev.Aux1Down() || ev.Aux1Up() || ev.Aux1DClick() )
    {
        button = "Aux1";
        dbl = ev.Aux1DClick();
        up = ev.Aux1Up();
    }
    else if ( ev.Aux2Down() || ev.Aux2Up() || ev.Aux2DClick() )
    {
        button = "Aux2";
        dbl = ev.Aux2DClick();
        up = ev.Aux2Up();
    }
    else if ( ev.GetWheelRotation() )
    {
        return wxString::Format("%s wheel rotation %+d",
            ev.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL ? "Vertical" : "Horizontal",
            ev.GetWheelRotation());
    }
    else
    {
        return "Unknown mouse event";
    }
    wxASSERT(!(dbl && up));

    return wxString::Format("%s mouse button %s",
                            button,
                            dbl ? "double clicked"
                                : up ? "released" : "clicked");
}

void MyTextCtrl::OnMouseEvent(wxMouseEvent& ev)
{
    ev.Skip();

    if ( !ms_logMouse )
        return;

    if ( ev.GetEventType() != wxEVT_MOTION )
    {
        wxString msg;
        if ( ev.Entering() )
        {
            msg = "Mouse entered the window";
        }
        else if ( ev.Leaving() )
        {
            msg = "Mouse left the window";
        }
        else
        {
            msg = GetMouseEventDesc(ev);
        }

        msg << " at (" << ev.GetX() << ", " << ev.GetY() << ") ";

        long pos;
        wxTextCtrlHitTestResult rc = HitTest(ev.GetPosition(), &pos);
        wxString where;
        switch ( rc )
        {
            case wxTE_HT_UNKNOWN:
                break;

            case wxTE_HT_BEFORE:
                where = "before";
                break;

            case wxTE_HT_ON_TEXT:
                where = "at";
                break;

            case wxTE_HT_BELOW:
                where = "below";
                break;

            case wxTE_HT_BEYOND:
                where = "beyond";
                break;
        }

        if ( !where.empty() )
            msg << where << " position " << pos << " ";

        msg << "[Flags: "
            << GetChar( ev.LeftIsDown(), '1' )
            << GetChar( ev.MiddleIsDown(), '2' )
            << GetChar( ev.RightIsDown(), '3' )
            << GetChar( ev.Aux1IsDown(), 'x' )
            << GetChar( ev.Aux2IsDown(), 'X' )
            << GetChar( ev.ControlDown(), 'C' )
            << GetChar( ev.AltDown(), 'A' )
            << GetChar( ev.ShiftDown(), 'S' )
            << GetChar( ev.MetaDown(), 'M' )
            << ']';

        wxLogMessage(msg);
    }
    //else: we're not interested in mouse move events
}

void MyTextCtrl::OnContextMenu(wxContextMenuEvent& ev)
{
    ev.Skip();

    if ( !ms_logMouse )
        return;

    const wxPoint pos = ev.GetPosition();
    wxLogMessage("Context menu event at (%d, %d)", pos.x, pos.y);
}

void MyTextCtrl::OnSetFocus(wxFocusEvent& event)
{
    if ( ms_logFocus )
    {
        wxLogMessage( "%p got focus.", this);
    }

    event.Skip();
}

void MyTextCtrl::OnKillFocus(wxFocusEvent& event)
{
    if ( ms_logFocus )
    {
        wxLogMessage( "%p lost focus", this);
    }

    event.Skip();
}

void MyTextCtrl::OnText(wxCommandEvent& event)
{
    if ( !ms_logText )
        return;

    MyTextCtrl *win = (MyTextCtrl *)event.GetEventObject();
    const wxString changeVerb = win->IsModified() ? "changed"
                                                 : "set by program";
    const wxChar *data = (const wxChar *)(win->GetClientData());
    if ( data )
    {
        wxLogMessage("Text %s in control \"%s\"", changeVerb, data);
    }
    else
    {
        wxLogMessage("Text %s in some control", changeVerb);
    }
}

void MyTextCtrl::OnTextEnter(wxCommandEvent& event)
{
    if ( !ms_logText )
        return;

    MyTextCtrl *win = (MyTextCtrl *)event.GetEventObject();
    const wxChar *data = (const wxChar *)(win->GetClientData());
    if ( data )
    {
        wxLogMessage("Enter pressed in control '%s'", data);
    }
    else
    {
        wxLogMessage("Enter pressed in some control");
    }
}

void MyTextCtrl::OnTextMaxLen(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("You can't enter more characters into this control.");
}


void MyTextCtrl::OnTextCut(wxClipboardTextEvent& event)
{
    LogClipEvent("cut to", event);
}

void MyTextCtrl::OnTextCopy(wxClipboardTextEvent& event)
{
    LogClipEvent("copied to", event);
}

void MyTextCtrl::OnTextPaste(wxClipboardTextEvent& event)
{
    LogClipEvent("pasted from", event);
}

void MyTextCtrl::LogClipEvent(const wxString&what, wxClipboardTextEvent& event)
{
    wxFrame *frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
    wxCHECK_RET( frame, "no parent frame?" );

    const bool veto = frame->GetMenuBar()->IsChecked(TEXT_CLIPBOARD_VETO);
    if ( !veto )
        event.Skip();

    if ( ms_logClip )
    {
        wxLogMessage("Text %s%s the clipboard.",
                     veto ? "not " : "", what);
    }
}


void MyTextCtrl::OnTextURL(wxTextUrlEvent& event)
{
    const wxMouseEvent& ev = event.GetMouseEvent();

    // filter out mouse moves, too many of them
    if ( ev.Moving() )
        return;

    long start = event.GetURLStart(),
         end = event.GetURLEnd();

    wxLogMessage("Mouse event over URL '%s': %s",
                 GetValue().Mid(start, end - start),
                 GetMouseEventDesc(ev));
}

void MyTextCtrl::OnChar(wxKeyEvent& event)
{
    if ( ms_logChar )
        LogKeyEvent( "Char", event);

    event.Skip();
}

void MyTextCtrl::OnKeyUp(wxKeyEvent& event)
{
    if ( ms_logKey )
        LogKeyEvent( "Key up", event);

    event.Skip();
}

void MyTextCtrl::OnKeyDown(wxKeyEvent& event)
{
    if ( ms_logKey )
        LogKeyEvent( "Key down", event);

    event.Skip();

    // Only handle bare function keys below, notably let Alt-Fn perform their
    // usual default functions as intercepting them is annoying.
    if ( event.GetModifiers() != 0 )
        return;

    switch ( event.GetKeyCode() )
    {
        case WXK_F1:
            // show current position and text length
            {
                long line, column, pos = GetInsertionPoint();
                PositionToXY(pos, &column, &line);

                wxLogMessage("Current position: %ld\nCurrent line, column: (%ld, %ld)\nNumber of lines: %ld\nCurrent line length: %ld\nTotal text length: %u (%ld)",
                        pos,
                        line, column,
                        (long) GetNumberOfLines(),
                        (long) GetLineLength(line),
                        (unsigned int) GetValue().length(),
                        GetLastPosition());

                long from, to;
                GetSelection(&from, &to);

                wxString sel = GetStringSelection();

                wxLogMessage("Selection: from %ld to %ld.", from, to);
                wxLogMessage("Selection = '%s' (len = %u)",
                             sel,
                             (unsigned int) sel.length());

                const wxString text = GetLineText(line);
                wxLogMessage("Current line: \"%s\"; length = %lu",
                             text, text.length());
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

        case WXK_F4:
            if (!m_hasCapture)
            {
                wxLogDebug( "Now capturing mouse and events." );
                m_hasCapture = true;
                CaptureMouse();
            }
            else
            {
                wxLogDebug( "Stopped capturing mouse and events." );
                m_hasCapture = false;
                ReleaseMouse();
            }
            break;

        case WXK_F5:
            // insert a blank line
            WriteText("\n");
            break;

        case WXK_F6:
            wxLogMessage("IsModified() before SetValue(): %d",
                         IsModified());
            ChangeValue("ChangeValue() has been called");
            wxLogMessage("IsModified() after SetValue(): %d",
                         IsModified());
            break;

        case WXK_F7:
            wxLogMessage("Position 10 should be now visible.");
            ShowPosition(10);
            break;

        case WXK_F8:
            wxLogMessage("Control has been cleared");
            Clear();
            break;

        case WXK_F9:
            WriteText("WriteText() has been called");
            break;

        case WXK_F10:
            AppendText("AppendText() has been called");
            break;

        case WXK_F11:
            DiscardEdits();
            wxLogMessage("Control marked as non modified");
            break;
    }
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, wxID_ANY, wxPoint(x, y), wxSize(w, h) )
{
#if wxUSE_LOG
    m_log = new wxTextCtrl( this, wxID_ANY, "This is the log window.\n",
                            wxPoint(5,260), wxSize(630,100),
                            wxTE_MULTILINE | wxTE_READONLY);

    m_logOld = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
#endif // wxUSE_LOG

    // single line text controls

    m_text = new MyTextCtrl( this, wxID_ANY, "Single line.",
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_PROCESS_ENTER | wxTE_RICH2);
    m_text->SetForegroundColour(*wxBLUE);
    m_text->SetBackgroundColour(*wxLIGHT_GREY);
    (*m_text) << " Appended.";
    m_text->SetInsertionPoint(0);
    m_text->WriteText( "Prepended. " );

#if wxUSE_SPELLCHECK
    if ( m_text->EnableProofCheck(wxTextProofOptions::Default()) )
    {
        // Break the string in several parts to avoid misspellings in the sources.
        (*m_text) << " Mis"
                      "s"
                      "spelled.";
    }
#endif

    m_password = new MyTextCtrl( this, wxID_ANY, "",
      wxPoint(10,50), wxSize(140,wxDefaultCoord), wxTE_PASSWORD );
    m_password->SetHint("Don't use 12345 here");

    m_limited = new MyTextCtrl(this, wxID_ANY, "",
                              wxPoint(10, 90), wxDefaultSize);
    m_limited->SetHint("Max 8 ch");
    m_limited->SetMaxLength(8);
    wxSize size2 = m_limited->GetSizeFromTextSize(m_limited->GetTextExtent("WWWWWWWW"));
    m_limited->SetSizeHints(size2, size2);

    wxTextCtrl* upperOnly = new MyTextCtrl(this, wxID_ANY, "Only upper case",
                                           wxDefaultPosition, wxDefaultSize);
    upperOnly->ForceUpper();

    // multi line text controls

    wxString string3L("Read only\nMultiline\nFitted size");
    m_readonly = new MyTextCtrl( this, wxID_ANY, string3L,
               wxPoint(10, 120), wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
    wxWindowDC dc(m_readonly);
    size2 = m_readonly->GetSizeFromTextSize(dc.GetMultiLineTextExtent(string3L));
    m_readonly->SetMinSize(size2);

    m_horizontal = new MyTextCtrl( this, wxID_ANY, "Multiline text control with a horizontal scrollbar.\n",
      wxPoint(10,170), wxSize(140,70), wxTE_MULTILINE | wxHSCROLL);
    m_horizontal->SetHint("Enter multiline text here");

    // a little hack to use the command line argument for encoding testing
    if ( wxTheApp->argc == 2 )
    {
        switch ( (wxChar)wxTheApp->argv[1][0] )
        {
            case '2':
                m_horizontal->SetFont(wxFontInfo(18)
                                        .Family(wxFONTFAMILY_SWISS)
                                        .Encoding(wxFONTENCODING_ISO8859_2));
                m_horizontal->AppendText("\256lu\273ou\350k\375 k\371\362 zb\354sile \350e\271tina \253\273");
                break;

            case '1':
                m_horizontal->SetFont(wxFontInfo(18)
                                        .Family(wxFONTFAMILY_SWISS)
                                        .Encoding(wxFONTENCODING_CP1251));
                m_horizontal->AppendText("\317\360\350\342\345\362!");
                break;

            case '8':
                m_horizontal->SetFont(wxFontInfo(18)
                                        .Family(wxFONTFAMILY_SWISS)
                                        .Encoding(wxFONTENCODING_CP1251));
                m_horizontal->AppendText(L"\x0412\x0430\x0434\x0438\x043c \x0426");
        }
    }
    else
    {
        m_horizontal->AppendText("Text in default encoding");
    }

    m_multitext = new MyTextCtrl( this, wxID_ANY,
                                  "Multi line without vertical scrollbar.",
      wxPoint(180,10), wxSize(200,70), wxTE_MULTILINE | wxTE_NO_VSCROLL );
    m_multitext->SetFont(*wxITALIC_FONT);
    (*m_multitext) << " Appended.";
    m_multitext->SetInsertionPoint(0);
    m_multitext->WriteText( "Prepended. " );
    m_multitext->SetForegroundColour(*wxYELLOW);
    m_multitext->SetBackgroundColour(*wxLIGHT_GREY);

#if wxUSE_TOOLTIPS
    m_multitext->SetToolTip("Press Fn function keys here");
#endif

    m_tab = new MyTextCtrl( this, 100, "Multiline, allow <TAB> processing.",
      wxPoint(180,90), wxSize(200,70), wxTE_MULTILINE |  wxTE_PROCESS_TAB );
    m_tab->SetClientData(const_cast<void*>(static_cast<const void*>(wxS("tab"))));

    m_enter = new MyTextCtrl( this, 100, "Multiline, allow <ENTER> processing.",
      wxPoint(180,170), wxSize(200,70), wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_RICH2 );
    m_enter->SetClientData(const_cast<void*>(static_cast<const void*>(wxS("enter"))));

#if wxUSE_SPELLCHECK
    (*m_enter) << "\n";

    // Enable grammar check just for demonstration purposes (note that it's
    // only supported under Mac, but spell checking will be enabled under the
    // other platforms too, if supported). If we didn't want to enable it, we
    // could omit the EnableProofCheck() argument entirely.
    if ( !m_enter->EnableProofCheck(wxTextProofOptions::Default().GrammarCheck()) )
    {
        (*m_enter) << "Spell checking is not available on this platform, sorry.";
    }
    else
    {
        (*m_enter) << "Spell checking is enabled, mis"
                      "s"
                      "spelled words should be highlighted.";
    }
#endif

    m_textrich = new MyTextCtrl(this, wxID_ANY, "Allows more than 30Kb of text\n"
                                "(on all Windows versions)\n"
                                "and a very very very very very "
                                "very very very long line to test "
                                "wxHSCROLL style\n"
                                "\nAnd here is a link in quotation marks to "
                                "test wxTE_AUTO_URL: \"http://www.wxwidgets.org\"",
                                wxPoint(450, 10), wxSize(200, 230),
                                wxTE_RICH | wxTE_MULTILINE | wxTE_AUTO_URL);
    m_textrich->SetStyle(0, 10, *wxRED);
    m_textrich->SetStyle(10, 20, *wxBLUE);
    m_textrich->SetStyle(30, 40,
                         wxTextAttr(*wxGREEN, wxNullColour, *wxITALIC_FONT));
    m_textrich->SetDefaultStyle(wxTextAttr());
    m_textrich->AppendText("\n\nFirst 10 characters should be in red\n");
    m_textrich->AppendText("Next 10 characters should be in blue\n");
    m_textrich->AppendText("Next 10 characters should be normal\n");
    m_textrich->AppendText("And the next 10 characters should be green and italic\n");
    m_textrich->SetDefaultStyle(wxTextAttr(*wxCYAN, *wxBLUE));
    m_textrich->AppendText("This text should be cyan on blue\n");
    m_textrich->SetDefaultStyle(wxTextAttr(*wxBLUE, *wxWHITE));
    m_textrich->AppendText("And this should be in blue and the text you "
                           "type should be in blue as well.\n");
    m_textrich->SetDefaultStyle(wxTextAttr());
    wxTextAttr attr = m_textrich->GetDefaultStyle();
    attr.SetFontUnderlined(true);
    m_textrich->SetDefaultStyle(attr);
    m_textrich->AppendText("\nAnd there");
    attr.SetFontUnderlined(false);
    m_textrich->SetDefaultStyle(attr);
    m_textrich->AppendText(" is a ");
    attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_SPECIAL, *wxRED);
    m_textrich->SetDefaultStyle(attr);
    m_textrich->AppendText("mispeled");
    attr.SetFontUnderlined(false);
    m_textrich->SetDefaultStyle(attr);
    m_textrich->AppendText(" word.");
    attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_DOUBLE, *wxGREEN);
    const long endPos = m_textrich->GetLastPosition();
    m_textrich->SetStyle(endPos - 4, endPos - 2, attr);

    // lay out the controls
    wxBoxSizer *column1 = new wxBoxSizer(wxVERTICAL);
    column1->Add( m_text, 0, wxALL | wxEXPAND, 10 );
    column1->Add( m_password, 0, wxALL | wxEXPAND, 10 );
    column1->Add( m_readonly, 0, wxALL, 10 );
    column1->Add( m_limited, 0, wxALL, 10 );
    column1->Add( upperOnly, 0, wxALL, 10 );
    column1->Add( m_horizontal, 1, wxALL | wxEXPAND, 10 );

    wxBoxSizer *column2 = new wxBoxSizer(wxVERTICAL);
    column2->Add( m_multitext, 1, wxALL | wxEXPAND, 10 );
    column2->Add( m_tab, 0, wxALL | wxEXPAND, 10 );
    column2->Add( m_enter, 1, wxALL | wxEXPAND, 10 );

    wxBoxSizer *row1 = new wxBoxSizer(wxHORIZONTAL);
    row1->Add( column1, 0, wxALL | wxEXPAND, 10 );
    row1->Add( column2, 1, wxALL | wxEXPAND, 10 );
    row1->Add( m_textrich, 1, wxALL | wxEXPAND, 10 );

    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add( row1, 2, wxALL | wxEXPAND, 10 );

#if wxUSE_LOG
    topSizer->Add( m_log, 1, wxALL | wxEXPAND, 10 );
#endif

    SetSizer(topSizer);
}

wxTextCtrl *MyPanel::GetFocusedText() const
{
    wxWindow *win = FindFocus();

    wxTextCtrl *text = win ? wxDynamicCast(win, wxTextCtrl) : nullptr;
    return text ? text : m_multitext;
}

#if wxUSE_CLIPBOARD
void MyPanel::DoPasteFromClipboard()
{
    // On X11, we want to get the data from the primary selection instead
    // of the normal clipboard (which isn't normal under X11 at all). This
    // call has no effect under MSW.
    wxTheClipboard->UsePrimarySelection();

    if (!wxTheClipboard->Open())
    {
#if wxUSE_LOG
        *m_log << "Error opening the clipboard.\n";
#endif // wxUSE_LOG
        return;
    }
    else
    {
#if wxUSE_LOG
        *m_log << "Successfully opened the clipboard.\n";
#endif // wxUSE_LOG
    }

    wxTextDataObject data;

    if (wxTheClipboard->IsSupported( data.GetFormat() ))
    {
#if wxUSE_LOG
        *m_log << "Clipboard supports requested format.\n";
#endif // wxUSE_LOG

        if (wxTheClipboard->GetData( data ))
        {
#if wxUSE_LOG
            *m_log << "Successfully retrieved data from the clipboard.\n";
#endif // wxUSE_LOG
            GetFocusedText()->AppendText(data.GetText());
        }
        else
        {
#if wxUSE_LOG
            *m_log << "Error getting data from the clipboard.\n";
#endif // wxUSE_LOG
        }
    }
    else
    {
#if wxUSE_LOG
        *m_log << "Clipboard doesn't support requested format.\n";
#endif // wxUSE_LOG
    }

    wxTheClipboard->Close();

#if wxUSE_LOG
    *m_log << "Closed the clipboard.\n";
#endif // wxUSE_LOG
}

void MyPanel::DoCopyToClipboard()
{
    // On X11, we want to get the data from the primary selection instead
    // of the normal clipboard (which isn't normal under X11 at all). This
    // call has no effect under MSW.
    wxTheClipboard->UsePrimarySelection();

    wxString text( GetFocusedText()->GetStringSelection() );

    if (text.IsEmpty())
    {
#if wxUSE_LOG
        *m_log << "No text to copy.\n";
#endif // wxUSE_LOG

        return;
    }

    if (!wxTheClipboard->Open())
    {
#if wxUSE_LOG
        *m_log << "Error opening the clipboard.\n";
#endif // wxUSE_LOG

        return;
    }
    else
    {
#if wxUSE_LOG
        *m_log << "Successfully opened the clipboard.\n";
#endif // wxUSE_LOG
    }

    wxTextDataObject *data = new wxTextDataObject( text );

    if (!wxTheClipboard->SetData( data ))
    {
#if wxUSE_LOG
        *m_log << "Error while copying to the clipboard.\n";
#endif // wxUSE_LOG
    }
    else
    {
#if wxUSE_LOG
        *m_log << "Successfully copied data to the clipboard.\n";
#endif // wxUSE_LOG
    }

    wxTheClipboard->Close();

#if wxUSE_LOG
    *m_log << "Closed the clipboard.\n";
#endif // wxUSE_LOG
}

#endif // wxUSE_CLIPBOARD

void MyPanel::DoMoveToEndOfText()
{
    m_multitext->SetInsertionPointEnd();
    m_multitext->SetFocus();
}

void MyPanel::DoGetWindowCoordinates()
{
#if wxUSE_LOG
    wxTextCtrl * const text = GetFocusedText();

    const wxPoint pt0 = text->PositionToCoords(0);
    const wxPoint ptCur = text->PositionToCoords(text->GetInsertionPoint());
    *m_log << "Current position coordinates: "
              "(" << ptCur.x << ", "  << ptCur.y << "), "
              "first position coordinates: "
              "(" << pt0.x << ", "  << pt0.y << ")\n";
#endif // wxUSE_LOG
}

void MyPanel::DoMoveToEndOfEntry()
{
    m_text->SetInsertionPointEnd();
    m_text->SetFocus();
}

void MyPanel::DoRemoveText()
{
    GetFocusedText()->Remove(0, 10);
}

void MyPanel::DoReplaceText()
{
    GetFocusedText()->Replace(3, 8, "ABC");
}

void MyPanel::DoSelectText()
{
    GetFocusedText()->SetSelection(3, 8);
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TEXT_QUIT,   MyFrame::OnQuit)
    EVT_MENU(TEXT_ABOUT,  MyFrame::OnAbout)
    EVT_MENU(TEXT_SAVE,   MyFrame::OnFileSave)
    EVT_MENU(TEXT_LOAD,   MyFrame::OnFileLoad)
    EVT_MENU(TEXT_RICH_TEXT_TEST, MyFrame::OnRichTextTest)

    EVT_MENU(TEXT_LOG_KEY,  MyFrame::OnLogKey)
    EVT_MENU(TEXT_LOG_CHAR, MyFrame::OnLogChar)
    EVT_MENU(TEXT_LOG_MOUSE,MyFrame::OnLogMouse)
    EVT_MENU(TEXT_LOG_TEXT, MyFrame::OnLogText)
    EVT_MENU(TEXT_LOG_FOCUS,MyFrame::OnLogFocus)
    EVT_MENU(TEXT_LOG_CLIP, MyFrame::OnLogClip)
#if wxUSE_LOG
    EVT_MENU(TEXT_CLEAR,    MyFrame::OnLogClear)
#endif // wxUSE_LOG

#if wxUSE_TOOLTIPS
    EVT_MENU(TEXT_TOOLTIPS_SETDELAY,  MyFrame::OnSetTooltipDelay)
    EVT_MENU(TEXT_TOOLTIPS_ENABLE,    MyFrame::OnToggleTooltips)
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    EVT_MENU(TEXT_CLIPBOARD_PASTE,    MyFrame::OnPasteFromClipboard)
    EVT_MENU(TEXT_CLIPBOARD_COPY,     MyFrame::OnCopyToClipboard)

    EVT_UPDATE_UI(TEXT_CLIPBOARD_PASTE, MyFrame::OnUpdatePasteFromClipboard)
    EVT_UPDATE_UI(TEXT_CLIPBOARD_COPY,  MyFrame::OnUpdateCopyToClipboard)
#endif // wxUSE_CLIPBOARD

    EVT_MENU(TEXT_REMOVE,             MyFrame::OnRemoveText)
    EVT_MENU(TEXT_REPLACE,            MyFrame::OnReplaceText)
    EVT_MENU(TEXT_SELECT,             MyFrame::OnSelectText)
    EVT_MENU(TEXT_ADD_SOME,           MyFrame::OnAddText)
    EVT_MENU(TEXT_ADD_FREEZE,         MyFrame::OnAddTextFreeze)
    EVT_MENU(TEXT_ADD_LINE,           MyFrame::OnAddTextLine)
    EVT_MENU(TEXT_MOVE_ENDTEXT,       MyFrame::OnMoveToEndOfText)
    EVT_MENU(TEXT_GET_WINDOW_COORD,   MyFrame::OnGetWindowCoordinates)
    EVT_MENU(TEXT_MOVE_ENDENTRY,      MyFrame::OnMoveToEndOfEntry)

    EVT_MENU(TEXT_SET_EDITABLE,       MyFrame::OnSetEditable)
    EVT_MENU(TEXT_SET_ENABLED,        MyFrame::OnSetEnabled)

    EVT_MENU(TEXT_LINE_DOWN,          MyFrame::OnScrollLineDown)
    EVT_MENU(TEXT_LINE_UP,            MyFrame::OnScrollLineUp)
    EVT_MENU(TEXT_PAGE_DOWN,          MyFrame::OnScrollPageDown)
    EVT_MENU(TEXT_PAGE_UP,            MyFrame::OnScrollPageUp)

    EVT_MENU(TEXT_GET_LINE,           MyFrame::OnGetLine)
    EVT_MENU(TEXT_GET_LINELENGTH,     MyFrame::OnGetLineLength)

    EVT_MENU(TEXT_SET,                MyFrame::OnSetText)
    EVT_MENU(TEXT_CHANGE,             MyFrame::OnChangeText)

    EVT_IDLE(MyFrame::OnIdle)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(const wxString& title, int x, int y)
       : wxFrame(nullptr, wxID_ANY, title, wxPoint(x, y))
{
    SetIcon(wxICON(sample));

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    m_panel = new MyPanel( this, 10, 10, 300, 100 );
    m_panel->GetSizer()->Fit(this);
}

void MyFrame::OnQuit (wxCommandEvent& WXUNUSED(event) )
{
    Close(true);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxBeginBusyCursor();

    wxMessageDialog dialog(this,
      "This is a text control sample. It demonstrates the many different\n"
      "text control styles, the use of the clipboard, setting and handling\n"
      "tooltips and intercepting key and char events.\n"
      "\n"
        "Copyright (c) 1999, Robert Roebling, Julian Smart, Vadim Zeitlin",
        "About wxTextCtrl Sample",
        wxOK | wxICON_INFORMATION);

    dialog.ShowModal();

    wxEndBusyCursor();
}

#if wxUSE_TOOLTIPS

void MyFrame::OnSetTooltipDelay(wxCommandEvent& WXUNUSED(event))
{
    static long s_delay = 5000;

    wxString delay;
    delay.Printf( "%ld", s_delay);

    delay = wxGetTextFromUser("Enter delay (in milliseconds)",
                              "Set tooltip delay",
                              delay,
                              this);
    if ( !delay )
        return; // cancelled

    wxSscanf(delay, "%ld", &s_delay);

    wxToolTip::SetDelay(s_delay);

    wxLogStatus(this, "Tooltip delay set to %ld milliseconds", s_delay);
}

void MyFrame::OnToggleTooltips(wxCommandEvent& WXUNUSED(event))
{
    static bool s_enabled = true;

    s_enabled = !s_enabled;

    wxToolTip::Enable(s_enabled);

    wxLogStatus(this, "Tooltips %sabled", s_enabled ? "en" : "dis" );
}
#endif // tooltips

#if wxUSE_LOG
void MyFrame::OnLogClear(wxCommandEvent& WXUNUSED(event))
{
    m_panel->m_log->Clear();
}
#endif // wxUSE_LOG

void MyFrame::OnSetEditable(wxCommandEvent& WXUNUSED(event))
{
    static bool s_editable = true;

    s_editable = !s_editable;
    m_panel->m_text->SetEditable(s_editable);
    m_panel->m_password->SetEditable(s_editable);
    m_panel->m_multitext->SetEditable(s_editable);
    m_panel->m_textrich->SetEditable(s_editable);
}

void MyFrame::OnSetEnabled(wxCommandEvent& WXUNUSED(event))
{
    bool enabled = m_panel->m_text->IsEnabled();
    enabled = !enabled;

    m_panel->m_text->Enable(enabled);
    m_panel->m_password->Enable(enabled);
    m_panel->m_multitext->Enable(enabled);
    m_panel->m_readonly->Enable(enabled);
    m_panel->m_limited->Enable(enabled);
    m_panel->m_textrich->Enable(enabled);
}

void MyFrame::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
    if ( m_panel->m_textrich->SaveFile("dummy.txt") )
    {
#if wxUSE_FILE
        // verify that the file length is correct
        wxFile file("dummy.txt");
        wxLogStatus(this,
                    "Successfully saved file (text len = %lu, file size = %ld)",
                    (unsigned long)m_panel->m_textrich->GetValue().length(),
                    (long) file.Length());
#endif
    }
    else
        wxLogStatus(this, "Couldn't save the file");
}

void MyFrame::OnFileLoad(wxCommandEvent& WXUNUSED(event))
{
    if ( m_panel->m_textrich->LoadFile("dummy.txt") )
    {
        wxLogStatus(this, "Successfully loaded file");
    }
    else
    {
        wxLogStatus(this, "Couldn't load the file");
    }
}

void MyFrame::OnRichTextTest(wxCommandEvent& WXUNUSED(event))
{
    RichTextFrame* frame = new RichTextFrame(this, "Rich Text Editor");
    frame->Show(true);
}

void MyFrame::OnIdle( wxIdleEvent& event )
{
    // track the window which has the focus in the status bar
    static wxWindow *s_windowFocus = nullptr;
    wxWindow *focus = wxWindow::FindFocus();
    if ( focus && (focus != s_windowFocus) )
    {
        s_windowFocus = focus;

        wxString msg;
        msg.Printf(
#ifdef __WXMSW__
                "Focus: wxWindow = %p, HWND = %p",
#else
                "Focus: wxWindow = %p",
#endif
                s_windowFocus
#ifdef __WXMSW__
                , s_windowFocus->GetHWND()
#endif
                  );

#if wxUSE_STATUSBAR
        SetStatusText(msg);
#endif // wxUSE_STATUSBAR
    }
    event.Skip();
}

/*
 * RichTextFrame is used to demonstrate rich text behaviour
 */

enum
{
    RICHTEXT_CLOSE = 1000,
    RICHTEXT_LEFT_ALIGN,
    RICHTEXT_RIGHT_ALIGN,
    RICHTEXT_CENTRE,
    RICHTEXT_JUSTIFY,
    RICHTEXT_CHANGE_FONT,
    RICHTEXT_CHANGE_TEXT_COLOUR,
    RICHTEXT_CHANGE_BACKGROUND_COLOUR,
    RICHTEXT_LEFT_INDENT,
    RICHTEXT_RIGHT_INDENT,
    RICHTEXT_TAB_STOPS
};

wxBEGIN_EVENT_TABLE(RichTextFrame, wxFrame)
    EVT_IDLE(RichTextFrame::OnIdle)
    EVT_MENU(RICHTEXT_CLOSE, RichTextFrame::OnClose)
    EVT_MENU(RICHTEXT_LEFT_ALIGN, RichTextFrame::OnLeftAlign)
    EVT_MENU(RICHTEXT_RIGHT_ALIGN, RichTextFrame::OnRightAlign)
    EVT_MENU(RICHTEXT_CENTRE, RichTextFrame::OnCentre)
    EVT_MENU(RICHTEXT_JUSTIFY, RichTextFrame::OnJustify)
    EVT_MENU(RICHTEXT_CHANGE_FONT, RichTextFrame::OnChangeFont)
    EVT_MENU(RICHTEXT_CHANGE_TEXT_COLOUR, RichTextFrame::OnChangeTextColour)
    EVT_MENU(RICHTEXT_CHANGE_BACKGROUND_COLOUR, RichTextFrame::OnChangeBackgroundColour)
    EVT_MENU(RICHTEXT_LEFT_INDENT, RichTextFrame::OnLeftIndent)
    EVT_MENU(RICHTEXT_RIGHT_INDENT, RichTextFrame::OnRightIndent)
    EVT_MENU(RICHTEXT_TAB_STOPS, RichTextFrame::OnTabStops)
wxEND_EVENT_TABLE()

RichTextFrame::RichTextFrame(wxWindow* parent, const wxString& title):
    wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(300, 400))
{
    m_currentPosition = -1;
    m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
            wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2);

    wxString value;
    int i;
    for (i = 0; i < 10; i++)
    {
        int j;
        for (j = 0; j < 10; j++)
        {
            value << "Hello, welcome to a very simple rich text editor. You can set some character and paragraph styles from the Edit menu. ";
        }
        value << "\n\n";
    }
    m_textCtrl->SetValue(value);

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(RICHTEXT_CLOSE, _("Close\tCtrl+W"));
    menuBar->Append(fileMenu, _("File"));

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(RICHTEXT_LEFT_ALIGN, _("Left Align"));
    editMenu->Append(RICHTEXT_RIGHT_ALIGN, _("Right Align"));
    editMenu->Append(RICHTEXT_CENTRE, _("Centre"));
    editMenu->Append(RICHTEXT_JUSTIFY, _("Justify"));
    editMenu->AppendSeparator();
    editMenu->Append(RICHTEXT_CHANGE_FONT, _("Change Font"));
    editMenu->Append(RICHTEXT_CHANGE_TEXT_COLOUR, _("Change Text Colour"));
    editMenu->Append(RICHTEXT_CHANGE_BACKGROUND_COLOUR, _("Change Background Colour"));
    editMenu->AppendSeparator();
    editMenu->Append(RICHTEXT_LEFT_INDENT, _("Left Indent"));
    editMenu->Append(RICHTEXT_RIGHT_INDENT, _("Right Indent"));
    editMenu->Append(RICHTEXT_TAB_STOPS, _("Tab Stops"));
    menuBar->Append(editMenu, _("Edit"));

    SetMenuBar(menuBar);
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR
}

// Event handlers

void RichTextFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void RichTextFrame::OnLeftAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnRightAlign(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnJustify(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnCentre(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnChangeFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;

    wxFontDialog dialog(this, data);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxFontData retData = dialog.GetFontData();
        wxFont font = retData.GetChosenFont();

        wxTextAttr attr;
        attr.SetFont(font);

        long start, end;
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnChangeTextColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetColour(* wxBLACK);
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle("Choose the text colour");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();

        wxTextAttr attr;
        attr.SetTextColour(col);

        long start, end;
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnChangeBackgroundColour(wxCommandEvent& WXUNUSED(event))
{
    wxColourData data;
    data.SetColour(* wxWHITE);
    data.SetChooseFull(true);
    for (int i = 0; i < 16; i++)
    {
        wxColour colour((unsigned char)(i*16), (unsigned char)(i*16), (unsigned char)(i*16));
        data.SetCustomColour(i, colour);
    }

    wxColourDialog dialog(this, &data);
    dialog.SetTitle("Choose the text background colour");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxColourData retData = dialog.GetColourData();
        wxColour col = retData.GetColour();

        wxTextAttr attr;
        attr.SetBackgroundColour(col);

        long start, end;
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnLeftIndent(wxCommandEvent& WXUNUSED(event))
{
    wxString indentStr = wxGetTextFromUser
                         (
                            _("Please enter the left indent in tenths of a millimetre."),
                            _("Left Indent"),
                            wxEmptyString,
                            this
                         );
    if (!indentStr.IsEmpty())
    {
        int indent = wxAtoi(indentStr);

        wxTextAttr attr;
        attr.SetLeftIndent(indent);

        long start, end;
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnRightIndent(wxCommandEvent& WXUNUSED(event))
{
    wxString indentStr = wxGetTextFromUser
                         (
                            _("Please enter the right indent in tenths of a millimetre."),
                            _("Right Indent"),
                            wxEmptyString,
                            this
                         );
    if (!indentStr.IsEmpty())
    {
        int indent = wxAtoi(indentStr);

        wxTextAttr attr;
        attr.SetRightIndent(indent);

        long start, end;
        m_textCtrl->GetSelection(& start, & end);
        m_textCtrl->SetStyle(start, end, attr);

        m_currentPosition = -1;
    }
}

void RichTextFrame::OnTabStops(wxCommandEvent& WXUNUSED(event))
{
    wxString tabsStr = wxGetTextFromUser
                         (
                            _("Please enter the tab stop positions in tenths of a millimetre, separated by spaces.\nLeave empty to reset tab stops."),
                            _("Tab Stops"),
                            wxEmptyString,
                            this
                         );

    wxArrayInt tabs;

    wxStringTokenizer tokens(tabsStr, " ");
    while (tokens.HasMoreTokens())
    {
        wxString token = tokens.GetNextToken();
        tabs.Add(wxAtoi(token));
    }

    wxTextAttr attr;
    attr.SetTabs(tabs);

    long start, end;
    m_textCtrl->GetSelection(& start, & end);
    m_textCtrl->SetStyle(start, end, attr);

    m_currentPosition = -1;
}

void RichTextFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    long insertionPoint = m_textCtrl->GetInsertionPoint();
    if (insertionPoint != m_currentPosition)
    {
#if wxUSE_STATUSBAR
        wxTextAttr attr;
        if (m_textCtrl->GetStyle(insertionPoint, attr))
        {
            wxString msg;
            wxString facename("unknown");
            if (attr.GetFont().IsOk())
            {
                facename = attr.GetFont().GetFaceName();
            }
            wxString alignment("unknown alignment");
            if (attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
                alignment = "centred";
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
                alignment = "right-aligned";
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
                alignment = "left-aligned";
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_JUSTIFIED)
                alignment = "justified";

            msg.Printf("Facename: %s", facename);

            if (attr.HasTextColour())
            {
                msg += wxString::Format(", colour: %s",
                                        attr.GetTextColour().GetAsString());
            }
            else
            {
                msg += ", no colour";
            }

            msg << ", " << alignment;

            if (attr.HasFont())
            {
                if (attr.GetFont().GetWeight() == wxFONTWEIGHT_BOLD)
                    msg += " BOLD";
                else if (attr.GetFont().GetWeight() == wxFONTWEIGHT_NORMAL)
                    msg += " NORMAL";

                if (attr.GetFont().GetStyle() == wxFONTSTYLE_ITALIC)
                    msg += " ITALIC";

                if (attr.GetFont().GetUnderlined())
                    msg += " UNDERLINED";
            }

            SetStatusText(msg);
        }
#endif // wxUSE_STATUSBAR
        m_currentPosition = insertionPoint;
    }
}
