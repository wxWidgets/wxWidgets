/////////////////////////////////////////////////////////////////////////////
// Name:        text.cpp
// Purpose:     TextCtrl wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Julian Smart, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

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

    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    static bool ms_logKey;
    static bool ms_logChar;
    static bool ms_logMouse;
    static bool ms_logText;
    static bool ms_logFocus;
    static bool ms_logClip;

private:
    static inline wxChar GetChar(bool on, wxChar c) { return on ? c : wxT('-'); }

    void LogKeyEvent(const wxChar *name, wxKeyEvent& event) const;
    void LogClipEvent(const wxChar *what, wxClipboardTextEvent& event);

    bool m_hasCapture;

    DECLARE_EVENT_TABLE()
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
    // something non NULL
    wxTextCtrl *GetFocusedText() const;
};

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
#if wxUSE_TOOLTIPS
    void OnSetTooltipDelay(wxCommandEvent& event);
    void OnToggleTooltips(wxCommandEvent& event);
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    void OnPasteFromClipboard( wxCommandEvent& WXUNUSED(event) )
    {
        wxLogMessage(wxT("Pasting text from clipboard."));
        m_panel->DoPasteFromClipboard();
    }
    void OnCopyToClipboard( wxCommandEvent& WXUNUSED(event) )
    {
        wxLogMessage(wxT("Copying text to clipboard."));
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
    void OnMoveToEndOfEntry( wxCommandEvent& WXUNUSED(event) )
        { m_panel->DoMoveToEndOfEntry(); }

    void OnScrollLineDown(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->LineDown() )
        {
            wxLogMessage(wxT("Already at the bottom"));
        }
    }

    void OnScrollLineUp(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->LineUp() )
        {
            wxLogMessage(wxT("Already at the top"));
        }
    }

    void OnScrollPageDown(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->PageDown() )
        {
            wxLogMessage(wxT("Already at the bottom"));
        }
    }

    void OnScrollPageUp(wxCommandEvent& WXUNUSED(event))
    {
        if ( !m_panel->m_textrich->PageUp() )
        {
            wxLogMessage(wxT("Already at the top"));
        }
    }

    void OnGetLine(wxCommandEvent& WXUNUSED(event))
    {
        long nLine = wxGetNumberFromUser(wxT("Which line would you like to get?"),
                                         wxT("Enter which line you would like to get"),
                                         wxT("Get a line from the tabbed multiline text control") );

        wxMessageBox(m_panel->m_tab->GetLineText(nLine));
    }

    void OnGetLineLength(wxCommandEvent& WXUNUSED(event))
    {
        long nLine = wxGetNumberFromUser(wxT("Which line would you like to get?"),
                                         wxT("Enter which line you would like to get"),
                                         wxT("Get length of a line from the tabbed multiline text control") );

        wxMessageBox(wxString::Format(wxT("Length of line %i is:%i"),
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
        m_panel->m_text->SetValue(wxT("Hello, world! (what else did you expect?)"));
    }

    void OnChangeText(wxCommandEvent& WXUNUSED(event))
    {
        m_panel->m_text->ChangeValue(wxT("Changed, not set: no event"));
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
            text->AppendText(wxString::Format(wxT("Line %i\n"), i));
        }

        text->SetInsertionPoint(0);

        if ( freeze )
            text->Thaw();
    }

    MyPanel *m_panel;

    DECLARE_EVENT_TABLE()
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
    MyFrame *frame = new MyFrame((wxFrame *) NULL,
            wxT("Text wxWidgets sample"), 50, 50, 700, 550);
    frame->SetSizeHints( 500, 400 );

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(TEXT_SAVE, wxT("&Save file\tCtrl-S"),
                      wxT("Save the text control contents to file"));
    file_menu->Append(TEXT_LOAD, wxT("&Load file\tCtrl-O"),
                      wxT("Load the sample file into text control"));
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_RICH_TEXT_TEST, wxT("Show Rich Text Editor"));
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_ABOUT, wxT("&About\tAlt-A"));
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_QUIT, wxT("E&xit\tAlt-X"), wxT("Quit this sample"));

    wxMenuBar *menu_bar = new wxMenuBar( wxMB_DOCKABLE );
    menu_bar->Append(file_menu, wxT("&File"));

#if wxUSE_TOOLTIPS
    wxMenu *tooltip_menu = new wxMenu;
    tooltip_menu->Append(TEXT_TOOLTIPS_SETDELAY, wxT("Set &delay\tCtrl-D"));
    tooltip_menu->AppendSeparator();
    tooltip_menu->Append(TEXT_TOOLTIPS_ENABLE, wxT("&Toggle tooltips\tCtrl-T"),
            wxT("enable/disable tooltips"), true);
    tooltip_menu->Check(TEXT_TOOLTIPS_ENABLE, true);
    menu_bar->Append(tooltip_menu, wxT("&Tooltips"));
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    // notice that we use non default accelerators on purpose here to compare
    // their behaviour with the built in handling of standard Ctrl/Cmd-C/V
    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(TEXT_CLIPBOARD_COPY, wxT("&Copy\tCtrl-Shift-C"),
                          wxT("Copy the selection to the clipboard"));
    menuClipboard->Append(TEXT_CLIPBOARD_PASTE, wxT("&Paste\tCtrl-Shift-V"),
                          wxT("Paste from clipboard to the text control"));
    menuClipboard->AppendSeparator();
    menuClipboard->AppendCheckItem(TEXT_CLIPBOARD_VETO, wxT("Vet&o\tCtrl-Shift-O"),
                                   wxT("Veto all clipboard operations"));
    menu_bar->Append(menuClipboard, wxT("&Clipboard"));
#endif // wxUSE_CLIPBOARD

    wxMenu *menuText = new wxMenu;
    menuText->Append(TEXT_ADD_SOME, wxT("&Append some text\tCtrl-A"));
    menuText->Append(TEXT_ADD_FREEZE, wxT("&Append text with freeze/thaw\tShift-Ctrl-A"));
    menuText->Append(TEXT_ADD_LINE, wxT("Append a new &line\tAlt-Shift-A"));
    menuText->Append(TEXT_REMOVE, wxT("&Remove first 10 characters\tCtrl-Y"));
    menuText->Append(TEXT_REPLACE, wxT("&Replace characters 4 to 8 with ABC\tCtrl-R"));
    menuText->Append(TEXT_SELECT, wxT("&Select characters 4 to 8\tCtrl-I"));
    menuText->Append(TEXT_SET, wxT("&Set the first text zone value\tCtrl-E"));
    menuText->Append(TEXT_CHANGE, wxT("&Change the first text zone value\tShift-Ctrl-E"));
    menuText->AppendSeparator();
    menuText->Append(TEXT_MOVE_ENDTEXT, wxT("Move cursor to the end of &text"));
    menuText->Append(TEXT_MOVE_ENDENTRY, wxT("Move cursor to the end of &entry"));
    menuText->AppendCheckItem(TEXT_SET_EDITABLE, wxT("Toggle &editable state"));
    menuText->AppendCheckItem(TEXT_SET_ENABLED, wxT("Toggle e&nabled state"));
    menuText->Check(TEXT_SET_EDITABLE, true);
    menuText->Check(TEXT_SET_ENABLED, true);
    menuText->AppendSeparator();
    menuText->Append(TEXT_LINE_DOWN, wxT("Scroll text one line down"));
    menuText->Append(TEXT_LINE_UP, wxT("Scroll text one line up"));
    menuText->Append(TEXT_PAGE_DOWN, wxT("Scroll text one page down"));
    menuText->Append(TEXT_PAGE_UP, wxT("Scroll text one page up"));
    menuText->AppendSeparator();
    menuText->Append(TEXT_GET_LINE, wxT("Get the text of a line of the tabbed multiline"));
    menuText->Append(TEXT_GET_LINELENGTH, wxT("Get the length of a line of the tabbed multiline"));
    menu_bar->Append(menuText, wxT("Te&xt"));

#if wxUSE_LOG
    wxMenu *menuLog = new wxMenu;
    menuLog->AppendCheckItem(TEXT_LOG_KEY, wxT("Log &key events"));
    menuLog->AppendCheckItem(TEXT_LOG_CHAR, wxT("Log &char events"));
    menuLog->AppendCheckItem(TEXT_LOG_MOUSE, wxT("Log &mouse events"));
    menuLog->AppendCheckItem(TEXT_LOG_TEXT, wxT("Log &text events"));
    menuLog->AppendCheckItem(TEXT_LOG_FOCUS, wxT("Log &focus events"));
    menuLog->AppendCheckItem(TEXT_LOG_CLIP, wxT("Log clip&board events"));
    menuLog->AppendSeparator();
    menuLog->Append(TEXT_CLEAR, wxT("&Clear the log\tCtrl-L"),
                    wxT("Clear the log window contents"));

    // select only the interesting events by default
    MyTextCtrl::ms_logClip =
    MyTextCtrl::ms_logText = true;

    menuLog->Check(TEXT_LOG_KEY, MyTextCtrl::ms_logKey);
    menuLog->Check(TEXT_LOG_CHAR, MyTextCtrl::ms_logChar);
    menuLog->Check(TEXT_LOG_TEXT, MyTextCtrl::ms_logText);

    menu_bar->Append(menuLog, wxT("&Log"));
#endif // wxUSE_LOG

    frame->SetMenuBar(menu_bar);

    frame->Show(true);

    // report success
    return true;
}

//----------------------------------------------------------------------
// MyTextCtrl
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyTextCtrl, wxTextCtrl)
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

    EVT_SET_FOCUS(MyTextCtrl::OnSetFocus)
    EVT_KILL_FOCUS(MyTextCtrl::OnKillFocus)
END_EVENT_TABLE()

bool MyTextCtrl::ms_logKey = false;
bool MyTextCtrl::ms_logChar = false;
bool MyTextCtrl::ms_logMouse = false;
bool MyTextCtrl::ms_logText = false;
bool MyTextCtrl::ms_logFocus = false;
bool MyTextCtrl::ms_logClip = false;

void MyTextCtrl::LogKeyEvent(const wxChar *name, wxKeyEvent& event) const
{
    wxString key;
    long keycode = event.GetKeyCode();
    {
        switch ( keycode )
        {
            case WXK_BACK: key = wxT("BACK"); break;
            case WXK_TAB: key = wxT("TAB"); break;
            case WXK_RETURN: key = wxT("RETURN"); break;
            case WXK_ESCAPE: key = wxT("ESCAPE"); break;
            case WXK_SPACE: key = wxT("SPACE"); break;
            case WXK_DELETE: key = wxT("DELETE"); break;
            case WXK_START: key = wxT("START"); break;
            case WXK_LBUTTON: key = wxT("LBUTTON"); break;
            case WXK_RBUTTON: key = wxT("RBUTTON"); break;
            case WXK_CANCEL: key = wxT("CANCEL"); break;
            case WXK_MBUTTON: key = wxT("MBUTTON"); break;
            case WXK_CLEAR: key = wxT("CLEAR"); break;
            case WXK_SHIFT: key = wxT("SHIFT"); break;
            case WXK_ALT: key = wxT("ALT"); break;
            case WXK_CONTROL: key = wxT("CONTROL"); break;
            case WXK_MENU: key = wxT("MENU"); break;
            case WXK_PAUSE: key = wxT("PAUSE"); break;
            case WXK_CAPITAL: key = wxT("CAPITAL"); break;
            case WXK_END: key = wxT("END"); break;
            case WXK_HOME: key = wxT("HOME"); break;
            case WXK_LEFT: key = wxT("LEFT"); break;
            case WXK_UP: key = wxT("UP"); break;
            case WXK_RIGHT: key = wxT("RIGHT"); break;
            case WXK_DOWN: key = wxT("DOWN"); break;
            case WXK_SELECT: key = wxT("SELECT"); break;
            case WXK_PRINT: key = wxT("PRINT"); break;
            case WXK_EXECUTE: key = wxT("EXECUTE"); break;
            case WXK_SNAPSHOT: key = wxT("SNAPSHOT"); break;
            case WXK_INSERT: key = wxT("INSERT"); break;
            case WXK_HELP: key = wxT("HELP"); break;
            case WXK_NUMPAD0: key = wxT("NUMPAD0"); break;
            case WXK_NUMPAD1: key = wxT("NUMPAD1"); break;
            case WXK_NUMPAD2: key = wxT("NUMPAD2"); break;
            case WXK_NUMPAD3: key = wxT("NUMPAD3"); break;
            case WXK_NUMPAD4: key = wxT("NUMPAD4"); break;
            case WXK_NUMPAD5: key = wxT("NUMPAD5"); break;
            case WXK_NUMPAD6: key = wxT("NUMPAD6"); break;
            case WXK_NUMPAD7: key = wxT("NUMPAD7"); break;
            case WXK_NUMPAD8: key = wxT("NUMPAD8"); break;
            case WXK_NUMPAD9: key = wxT("NUMPAD9"); break;
            case WXK_MULTIPLY: key = wxT("MULTIPLY"); break;
            case WXK_ADD: key = wxT("ADD"); break;
            case WXK_SEPARATOR: key = wxT("SEPARATOR"); break;
            case WXK_SUBTRACT: key = wxT("SUBTRACT"); break;
            case WXK_DECIMAL: key = wxT("DECIMAL"); break;
            case WXK_DIVIDE: key = wxT("DIVIDE"); break;
            case WXK_F1: key = wxT("F1"); break;
            case WXK_F2: key = wxT("F2"); break;
            case WXK_F3: key = wxT("F3"); break;
            case WXK_F4: key = wxT("F4"); break;
            case WXK_F5: key = wxT("F5"); break;
            case WXK_F6: key = wxT("F6"); break;
            case WXK_F7: key = wxT("F7"); break;
            case WXK_F8: key = wxT("F8"); break;
            case WXK_F9: key = wxT("F9"); break;
            case WXK_F10: key = wxT("F10"); break;
            case WXK_F11: key = wxT("F11"); break;
            case WXK_F12: key = wxT("F12"); break;
            case WXK_F13: key = wxT("F13"); break;
            case WXK_F14: key = wxT("F14"); break;
            case WXK_F15: key = wxT("F15"); break;
            case WXK_F16: key = wxT("F16"); break;
            case WXK_F17: key = wxT("F17"); break;
            case WXK_F18: key = wxT("F18"); break;
            case WXK_F19: key = wxT("F19"); break;
            case WXK_F20: key = wxT("F20"); break;
            case WXK_F21: key = wxT("F21"); break;
            case WXK_F22: key = wxT("F22"); break;
            case WXK_F23: key = wxT("F23"); break;
            case WXK_F24: key = wxT("F24"); break;
            case WXK_NUMLOCK: key = wxT("NUMLOCK"); break;
            case WXK_SCROLL: key = wxT("SCROLL"); break;
            case WXK_PAGEUP: key = wxT("PAGEUP"); break;
            case WXK_PAGEDOWN: key = wxT("PAGEDOWN"); break;
            case WXK_NUMPAD_SPACE: key = wxT("NUMPAD_SPACE"); break;
            case WXK_NUMPAD_TAB: key = wxT("NUMPAD_TAB"); break;
            case WXK_NUMPAD_ENTER: key = wxT("NUMPAD_ENTER"); break;
            case WXK_NUMPAD_F1: key = wxT("NUMPAD_F1"); break;
            case WXK_NUMPAD_F2: key = wxT("NUMPAD_F2"); break;
            case WXK_NUMPAD_F3: key = wxT("NUMPAD_F3"); break;
            case WXK_NUMPAD_F4: key = wxT("NUMPAD_F4"); break;
            case WXK_NUMPAD_HOME: key = wxT("NUMPAD_HOME"); break;
            case WXK_NUMPAD_LEFT: key = wxT("NUMPAD_LEFT"); break;
            case WXK_NUMPAD_UP: key = wxT("NUMPAD_UP"); break;
            case WXK_NUMPAD_RIGHT: key = wxT("NUMPAD_RIGHT"); break;
            case WXK_NUMPAD_DOWN: key = wxT("NUMPAD_DOWN"); break;
            case WXK_NUMPAD_PAGEUP: key = wxT("NUMPAD_PAGEUP"); break;
            case WXK_NUMPAD_PAGEDOWN: key = wxT("NUMPAD_PAGEDOWN"); break;
            case WXK_NUMPAD_END: key = wxT("NUMPAD_END"); break;
            case WXK_NUMPAD_BEGIN: key = wxT("NUMPAD_BEGIN"); break;
            case WXK_NUMPAD_INSERT: key = wxT("NUMPAD_INSERT"); break;
            case WXK_NUMPAD_DELETE: key = wxT("NUMPAD_DELETE"); break;
            case WXK_NUMPAD_EQUAL: key = wxT("NUMPAD_EQUAL"); break;
            case WXK_NUMPAD_MULTIPLY: key = wxT("NUMPAD_MULTIPLY"); break;
            case WXK_NUMPAD_ADD: key = wxT("NUMPAD_ADD"); break;
            case WXK_NUMPAD_SEPARATOR: key = wxT("NUMPAD_SEPARATOR"); break;
            case WXK_NUMPAD_SUBTRACT: key = wxT("NUMPAD_SUBTRACT"); break;
            case WXK_NUMPAD_DECIMAL: key = wxT("NUMPAD_DECIMAL"); break;

            default:
            {
               if ( wxIsprint((int)keycode) )
                   key.Printf(wxT("'%c'"), (char)keycode);
               else if ( keycode > 0 && keycode < 27 )
                   key.Printf(_("Ctrl-%c"), wxT('A') + keycode - 1);
               else
                   key.Printf(wxT("unknown (%ld)"), keycode);
            }
        }
    }

#if wxUSE_UNICODE
    key += wxString::Format(wxT(" (Unicode: %#04x)"), event.GetUnicodeKey());
#endif // wxUSE_UNICODE

    wxLogMessage( wxT("%s event: %s (flags = %c%c%c%c)"),
                  name,
                  key.c_str(),
                  GetChar( event.ControlDown(), wxT('C') ),
                  GetChar( event.AltDown(), wxT('A') ),
                  GetChar( event.ShiftDown(), wxT('S') ),
                  GetChar( event.MetaDown(), wxT('M') ) );
}

static wxString GetMouseEventDesc(const wxMouseEvent& ev)
{
    // click event
    wxString button;
    bool dbl, up;
    if ( ev.LeftDown() || ev.LeftUp() || ev.LeftDClick() )
    {
        button = wxT("Left");
        dbl = ev.LeftDClick();
        up = ev.LeftUp();
    }
    else if ( ev.MiddleDown() || ev.MiddleUp() || ev.MiddleDClick() )
    {
        button = wxT("Middle");
        dbl = ev.MiddleDClick();
        up = ev.MiddleUp();
    }
    else if ( ev.RightDown() || ev.RightUp() || ev.RightDClick() )
    {
        button = wxT("Right");
        dbl = ev.RightDClick();
        up = ev.RightUp();
    }
    else
    {
        return wxT("Unknown mouse event");
    }

    return wxString::Format(wxT("%s mouse button %s"),
                            button.c_str(),
                            dbl ? wxT("double clicked")
                                : up ? wxT("released") : wxT("clicked"));
}

void MyTextCtrl::OnMouseEvent(wxMouseEvent& ev)
{
    ev.Skip();

    if ( !ms_logMouse )
        return;

    if ( !ev.Moving() )
    {
        wxString msg;
        if ( ev.Entering() )
        {
            msg = wxT("Mouse entered the window");
        }
        else if ( ev.Leaving() )
        {
            msg = wxT("Mouse left the window");
        }
        else
        {
            msg = GetMouseEventDesc(ev);
        }

        msg << wxT(" at (") << ev.GetX() << wxT(", ") << ev.GetY() << wxT(") ");

        long pos;
        wxTextCtrlHitTestResult rc = HitTest(ev.GetPosition(), &pos);
        if ( rc != wxTE_HT_UNKNOWN )
        {
            msg << wxT("at position ") << pos << wxT(' ');
        }

        msg << wxT("[Flags: ")
            << GetChar( ev.LeftIsDown(), wxT('1') )
            << GetChar( ev.MiddleIsDown(), wxT('2') )
            << GetChar( ev.RightIsDown(), wxT('3') )
            << GetChar( ev.ControlDown(), wxT('C') )
            << GetChar( ev.AltDown(), wxT('A') )
            << GetChar( ev.ShiftDown(), wxT('S') )
            << GetChar( ev.MetaDown(), wxT('M') )
            << wxT(']');

        wxLogMessage(msg);
    }
    //else: we're not interested in mouse move events
}

void MyTextCtrl::OnSetFocus(wxFocusEvent& event)
{
    if ( ms_logFocus )
    {
        wxLogMessage( wxT("%p got focus."), this);
    }

    event.Skip();
}

void MyTextCtrl::OnKillFocus(wxFocusEvent& event)
{
    if ( ms_logFocus )
    {
        wxLogMessage( wxT("%p lost focus"), this);
    }

    event.Skip();
}

void MyTextCtrl::OnText(wxCommandEvent& event)
{
    if ( !ms_logText )
        return;

    MyTextCtrl *win = (MyTextCtrl *)event.GetEventObject();
    const wxChar *changeVerb = win->IsModified() ? wxT("changed")
                                                 : wxT("set by program");
    const wxChar *data = (const wxChar *)(win->GetClientData());
    if ( data )
    {
        wxLogMessage(wxT("Text %s in control \"%s\""), changeVerb, data);
    }
    else
    {
        wxLogMessage(wxT("Text %s in some control"), changeVerb);
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
        wxLogMessage(wxT("Enter pressed in control '%s'"), data);
    }
    else
    {
        wxLogMessage(wxT("Enter pressed in some control"));
    }
}

void MyTextCtrl::OnTextMaxLen(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage(wxT("You can't enter more characters into this control."));
}


void MyTextCtrl::OnTextCut(wxClipboardTextEvent& event)
{
    LogClipEvent(wxT("cut to"), event);
}

void MyTextCtrl::OnTextCopy(wxClipboardTextEvent& event)
{
    LogClipEvent(wxT("copied to"), event);
}

void MyTextCtrl::OnTextPaste(wxClipboardTextEvent& event)
{
    LogClipEvent(wxT("pasted from"), event);
}

void MyTextCtrl::LogClipEvent(const wxChar *what, wxClipboardTextEvent& event)
{
    wxFrame *frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
    wxCHECK_RET( frame, wxT("no parent frame?") );

    const bool veto = frame->GetMenuBar()->IsChecked(TEXT_CLIPBOARD_VETO);
    if ( !veto )
        event.Skip();

    if ( ms_logClip )
    {
        wxLogMessage(wxT("Text %s%s the clipboard."),
                     veto ? wxT("not ") : wxT(""), what);
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

    wxLogMessage(wxT("Mouse event over URL '%s': %s"),
                 GetValue().Mid(start, end - start).c_str(),
                 GetMouseEventDesc(ev).c_str());
}

void MyTextCtrl::OnChar(wxKeyEvent& event)
{
    if ( ms_logChar )
        LogKeyEvent( wxT("Char"), event);

    event.Skip();
}

void MyTextCtrl::OnKeyUp(wxKeyEvent& event)
{
    if ( ms_logKey )
        LogKeyEvent( wxT("Key up"), event);

    event.Skip();
}

void MyTextCtrl::OnKeyDown(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case WXK_F1:
            // show current position and text length
            {
                long line, column, pos = GetInsertionPoint();
                PositionToXY(pos, &column, &line);

                wxLogMessage(wxT("Current position: %ld\nCurrent line, column: (%ld, %ld)\nNumber of lines: %ld\nCurrent line length: %ld\nTotal text length: %u (%ld)"),
                        pos,
                        line, column,
                        (long) GetNumberOfLines(),
                        (long) GetLineLength(line),
                        (unsigned int) GetValue().length(),
                        GetLastPosition());

                long from, to;
                GetSelection(&from, &to);

                wxString sel = GetStringSelection();

                wxLogMessage(wxT("Selection: from %ld to %ld."), from, to);
                wxLogMessage(wxT("Selection = '%s' (len = %u)"),
                             sel.c_str(),
                             (unsigned int) sel.length());

                const wxString text = GetLineText(line);
                wxLogMessage(wxT("Current line: \"%s\"; length = %lu"),
                             text.c_str(), text.length());
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
                wxLogDebug( wxT("Now capturing mouse and events.") );
                m_hasCapture = true;
                CaptureMouse();
            }
            else
            {
                wxLogDebug( wxT("Stopped capturing mouse and events.") );
                m_hasCapture = false;
                ReleaseMouse();
            }
            break;

        case WXK_F5:
            // insert a blank line
            WriteText(wxT("\n"));
            break;

        case WXK_F6:
            wxLogMessage(wxT("IsModified() before SetValue(): %d"),
                         IsModified());
            ChangeValue(wxT("ChangeValue() has been called"));
            wxLogMessage(wxT("IsModified() after SetValue(): %d"),
                         IsModified());
            break;

        case WXK_F7:
            wxLogMessage(wxT("Position 10 should be now visible."));
            ShowPosition(10);
            break;

        case WXK_F8:
            wxLogMessage(wxT("Control has been cleared"));
            Clear();
            break;

        case WXK_F9:
            WriteText(wxT("WriteText() has been called"));
            break;

        case WXK_F10:
            AppendText(wxT("AppendText() has been called"));
            break;

        case WXK_F11:
            DiscardEdits();
            wxLogMessage(wxT("Control marked as non modified"));
            break;
    }

    if ( ms_logKey )
        LogKeyEvent( wxT("Key down"), event);

    event.Skip();
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, wxID_ANY, wxPoint(x, y), wxSize(w, h) )
{
#if wxUSE_LOG
    m_log = new wxTextCtrl( this, wxID_ANY, wxT("This is the log window.\n"),
                            wxPoint(5,260), wxSize(630,100),
                            wxTE_MULTILINE | wxTE_READONLY);

    m_logOld = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
#endif // wxUSE_LOG

    // single line text controls

    m_text = new MyTextCtrl( this, wxID_ANY, wxT("Single line."),
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_PROCESS_ENTER);
    m_text->SetForegroundColour(*wxBLUE);
    m_text->SetBackgroundColour(*wxLIGHT_GREY);
    (*m_text) << wxT(" Appended.");
    m_text->SetInsertionPoint(0);
    m_text->WriteText( wxT("Prepended. ") );

    m_password = new MyTextCtrl( this, wxID_ANY, wxT(""),
      wxPoint(10,50), wxSize(140,wxDefaultCoord), wxTE_PASSWORD );

    m_readonly = new MyTextCtrl( this, wxID_ANY, wxT("Read only"),
      wxPoint(10,90), wxSize(140,wxDefaultCoord), wxTE_READONLY );

    m_limited = new MyTextCtrl(this, wxID_ANY, wxT("Max 8 ch"),
                              wxPoint(10, 130), wxSize(140, wxDefaultCoord));
    m_limited->SetMaxLength(8);

    // multi line text controls

    m_horizontal = new MyTextCtrl( this, wxID_ANY, wxT("Multiline text control with a horizontal scrollbar.\n"),
      wxPoint(10,170), wxSize(140,70), wxTE_MULTILINE | wxHSCROLL);

    // a little hack to use the command line argument for encoding testing
    if ( wxTheApp->argc == 2 )
    {
        switch ( (wxChar)wxTheApp->argv[1][0] )
        {
            case '2':
                m_horizontal->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL,
                                             false, wxT(""),
                                             wxFONTENCODING_ISO8859_2));
                m_horizontal->AppendText(wxT("\256lu\273ou\350k\375 k\371\362 zb\354sile \350e\271tina \253\273"));
                break;

            case '1':
                m_horizontal->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL,
                                             false, wxT(""),
                                             wxFONTENCODING_CP1251));
                m_horizontal->AppendText(wxT("\317\360\350\342\345\362!"));
                break;

            case '8':
                m_horizontal->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL,
                                             false, wxT(""),
                                             wxFONTENCODING_CP1251));
#if wxUSE_UNICODE
                m_horizontal->AppendText(L"\x0412\x0430\x0434\x0438\x043c \x0426");
#else
                m_horizontal->AppendText("\313\301\326\305\324\323\321 \325\304\301\336\316\331\315");
#endif
        }
    }
    else
    {
        m_horizontal->AppendText(wxT("Text in default encoding"));
    }

    m_multitext = new MyTextCtrl( this, wxID_ANY,
                                  wxT("Multi line without vertical scrollbar."),
      wxPoint(180,10), wxSize(200,70), wxTE_MULTILINE | wxTE_NO_VSCROLL );
    m_multitext->SetFont(*wxITALIC_FONT);
    (*m_multitext) << wxT(" Appended.");
    m_multitext->SetInsertionPoint(0);
    m_multitext->WriteText( wxT("Prepended. ") );
    m_multitext->SetForegroundColour(*wxYELLOW);
    m_multitext->SetBackgroundColour(*wxLIGHT_GREY);

#if wxUSE_TOOLTIPS
    m_multitext->SetToolTip(wxT("Press Fn function keys here"));
#endif

    m_tab = new MyTextCtrl( this, 100, wxT("Multiline, allow <TAB> processing."),
      wxPoint(180,90), wxSize(200,70), wxTE_MULTILINE |  wxTE_PROCESS_TAB );
    m_tab->SetClientData((void *)wxT("tab"));

    m_enter = new MyTextCtrl( this, 100, wxT("Multiline, allow <ENTER> processing."),
      wxPoint(180,170), wxSize(200,70), wxTE_MULTILINE);
    m_enter->SetClientData((void *)wxT("enter"));

    m_textrich = new MyTextCtrl(this, wxID_ANY, wxT("Allows more than 30Kb of text\n")
                                wxT("(even under broken Win9x)\n")
                                wxT("and a very very very very very ")
                                wxT("very very very long line to test ")
                                wxT("wxHSCROLL style\n")
                                wxT("\nAnd here is a link in quotation marks to ")
                                wxT("test wxTE_AUTO_URL: \"http://www.wxwidgets.org\""),
                                wxPoint(450, 10), wxSize(200, 230),
                                wxTE_RICH | wxTE_MULTILINE | wxTE_AUTO_URL);
    m_textrich->SetStyle(0, 10, *wxRED);
    m_textrich->SetStyle(10, 20, *wxBLUE);
    m_textrich->SetStyle(30, 40,
                         wxTextAttr(*wxGREEN, wxNullColour, *wxITALIC_FONT));
    m_textrich->SetDefaultStyle(wxTextAttr());
    m_textrich->AppendText(wxT("\n\nFirst 10 characters should be in red\n"));
    m_textrich->AppendText(wxT("Next 10 characters should be in blue\n"));
    m_textrich->AppendText(wxT("Next 10 characters should be normal\n"));
    m_textrich->AppendText(wxT("And the next 10 characters should be green and italic\n"));
    m_textrich->SetDefaultStyle(wxTextAttr(*wxCYAN, *wxBLUE));
    m_textrich->AppendText(wxT("This text should be cyan on blue\n"));
    m_textrich->SetDefaultStyle(wxTextAttr(*wxBLUE, *wxWHITE));
    m_textrich->AppendText(wxT("And this should be in blue and the text you ")
                           wxT("type should be in blue as well"));


    // lay out the controls
    wxBoxSizer *column1 = new wxBoxSizer(wxVERTICAL);
    column1->Add( m_text, 0, wxALL | wxEXPAND, 10 );
    column1->Add( m_password, 0, wxALL | wxEXPAND, 10 );
    column1->Add( m_readonly, 0, wxALL | wxEXPAND, 10 );
    column1->Add( m_limited, 0, wxALL | wxEXPAND, 10 );
    column1->Add( m_horizontal, 1, wxALL | wxEXPAND, 10 );

    wxBoxSizer *column2 = new wxBoxSizer(wxVERTICAL);
    column2->Add( m_multitext, 1, wxALL | wxEXPAND, 10 );
    column2->Add( m_tab, 1, wxALL | wxEXPAND, 10 );
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

    wxTextCtrl *text = win ? wxDynamicCast(win, wxTextCtrl) : NULL;
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
        *m_log << wxT("Error opening the clipboard.\n");
#endif // wxUSE_LOG
        return;
    }
    else
    {
#if wxUSE_LOG
        *m_log << wxT("Successfully opened the clipboard.\n");
#endif // wxUSE_LOG
    }

    wxTextDataObject data;

    if (wxTheClipboard->IsSupported( data.GetFormat() ))
    {
#if wxUSE_LOG
        *m_log << wxT("Clipboard supports requested format.\n");
#endif // wxUSE_LOG

        if (wxTheClipboard->GetData( data ))
        {
#if wxUSE_LOG
            *m_log << wxT("Successfully retrieved data from the clipboard.\n");
#endif // wxUSE_LOG
            GetFocusedText()->AppendText(data.GetText());
        }
        else
        {
#if wxUSE_LOG
            *m_log << wxT("Error getting data from the clipboard.\n");
#endif // wxUSE_LOG
        }
    }
    else
    {
#if wxUSE_LOG
        *m_log << wxT("Clipboard doesn't support requested format.\n");
#endif // wxUSE_LOG
    }

    wxTheClipboard->Close();

#if wxUSE_LOG
    *m_log << wxT("Closed the clipboard.\n");
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
        *m_log << wxT("No text to copy.\n");
#endif // wxUSE_LOG

        return;
    }

    if (!wxTheClipboard->Open())
    {
#if wxUSE_LOG
        *m_log << wxT("Error opening the clipboard.\n");
#endif // wxUSE_LOG

        return;
    }
    else
    {
#if wxUSE_LOG
        *m_log << wxT("Successfully opened the clipboard.\n");
#endif // wxUSE_LOG
    }

    wxTextDataObject *data = new wxTextDataObject( text );

    if (!wxTheClipboard->SetData( data ))
    {
#if wxUSE_LOG
        *m_log << wxT("Error while copying to the clipboard.\n");
#endif // wxUSE_LOG
    }
    else
    {
#if wxUSE_LOG
        *m_log << wxT("Successfully copied data to the clipboard.\n");
#endif // wxUSE_LOG
    }

    wxTheClipboard->Close();

#if wxUSE_LOG
    *m_log << wxT("Closed the clipboard.\n");
#endif // wxUSE_LOG
}

#endif // wxUSE_CLIPBOARD

void MyPanel::DoMoveToEndOfText()
{
    m_multitext->SetInsertionPointEnd();
    m_multitext->SetFocus();
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
    GetFocusedText()->Replace(3, 8, wxT("ABC"));
}

void MyPanel::DoSelectText()
{
    GetFocusedText()->SetSelection(3, 8);
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
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
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h)
       : wxFrame(frame, wxID_ANY, title, wxPoint(x, y), wxSize(w, h) )
{
    SetIcon(wxICON(sample));

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
    wxBeginBusyCursor();

    wxMessageDialog dialog(this,
      wxT("This is a text control sample. It demonstrates the many different\n")
      wxT("text control styles, the use of the clipboard, setting and handling\n")
      wxT("tooltips and intercepting key and char events.\n")
      wxT("\n")
        wxT("Copyright (c) 1999, Robert Roebling, Julian Smart, Vadim Zeitlin"),
        wxT("About wxTextCtrl Sample"),
        wxOK | wxICON_INFORMATION);

    dialog.ShowModal();

    wxEndBusyCursor();
}

#if wxUSE_TOOLTIPS

void MyFrame::OnSetTooltipDelay(wxCommandEvent& WXUNUSED(event))
{
    static long s_delay = 5000;

    wxString delay;
    delay.Printf( wxT("%ld"), s_delay);

    delay = wxGetTextFromUser(wxT("Enter delay (in milliseconds)"),
                              wxT("Set tooltip delay"),
                              delay,
                              this);
    if ( !delay )
        return; // cancelled

    wxSscanf(delay, wxT("%ld"), &s_delay);

    wxToolTip::SetDelay(s_delay);

    wxLogStatus(this, wxT("Tooltip delay set to %ld milliseconds"), s_delay);
}

void MyFrame::OnToggleTooltips(wxCommandEvent& WXUNUSED(event))
{
    static bool s_enabled = true;

    s_enabled = !s_enabled;

    wxToolTip::Enable(s_enabled);

    wxLogStatus(this, wxT("Tooltips %sabled"), s_enabled ? wxT("en") : wxT("dis") );
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
    if ( m_panel->m_textrich->SaveFile(wxT("dummy.txt")) )
    {
#if wxUSE_FILE
        // verify that the fil length is correct (it wasn't under Win95)
        wxFile file(wxT("dummy.txt"));
        wxLogStatus(this,
                    wxT("Successfully saved file (text len = %lu, file size = %ld)"),
                    (unsigned long)m_panel->m_textrich->GetValue().length(),
                    (long) file.Length());
#endif
    }
    else
        wxLogStatus(this, wxT("Couldn't save the file"));
}

void MyFrame::OnFileLoad(wxCommandEvent& WXUNUSED(event))
{
    if ( m_panel->m_textrich->LoadFile(wxT("dummy.txt")) )
    {
        wxLogStatus(this, wxT("Successfully loaded file"));
    }
    else
    {
        wxLogStatus(this, wxT("Couldn't load the file"));
    }
}

void MyFrame::OnRichTextTest(wxCommandEvent& WXUNUSED(event))
{
    RichTextFrame* frame = new RichTextFrame(this, wxT("Rich Text Editor"));
    frame->Show(true);
}

void MyFrame::OnIdle( wxIdleEvent& event )
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
                wxT("Focus: wxWindow = %p, HWND = %p"),
#else
                wxT("Focus: wxWindow = %p"),
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

BEGIN_EVENT_TABLE(RichTextFrame, wxFrame)
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
END_EVENT_TABLE()

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
            value << wxT("Hello, welcome to a very simple rich text editor. You can set some character and paragraph styles from the Edit menu. ");
        }
        value << wxT("\n\n");
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
    dialog.SetTitle(wxT("Choose the text colour"));
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
    dialog.SetTitle(wxT("Choose the text background colour"));
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

    wxStringTokenizer tokens(tabsStr, wxT(" "));
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
            wxString facename(wxT("unknown"));
            if (attr.GetFont().IsOk())
            {
                facename = attr.GetFont().GetFaceName();
            }
            wxString alignment(wxT("unknown alignment"));
            if (attr.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
                alignment = wxT("centred");
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
                alignment = wxT("right-aligned");
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
                alignment = wxT("left-aligned");
            else if (attr.GetAlignment() == wxTEXT_ALIGNMENT_JUSTIFIED)
                alignment = wxT("justified");

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
                if (attr.GetFont().GetWeight() == wxBOLD)
                    msg += wxT(" BOLD");
                else if (attr.GetFont().GetWeight() == wxNORMAL)
                    msg += wxT(" NORMAL");

                if (attr.GetFont().GetStyle() == wxITALIC)
                    msg += wxT(" ITALIC");

                if (attr.GetFont().GetUnderlined())
                    msg += wxT(" UNDERLINED");
            }

            SetStatusText(msg);
        }
#endif // wxUSE_STATUSBAR
        m_currentPosition = insertionPoint;
    }
}
