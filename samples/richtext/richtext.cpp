/////////////////////////////////////////////////////////////////////////////
// Name:        samples/richtext/richtext.cpp
// Purpose:     wxWidgets rich text editor sample
// Author:      Julian Smart
// Modified by:
// Created:     2005-10-02
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/fontdlg.h"
#include "wx/splitter.h"
#include "wx/sstream.h"
#include "wx/html/htmlwin.h"

#if wxUSE_HELP
#include "wx/cshelp.h"
#endif

#ifndef __WXMSW__
    #include "../sample.xpm"
#endif

#include "bitmaps/smiley.xpm"
// #include "bitmaps/idea.xpm"
#include "bitmaps/zebra.xpm"

#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/undo.xpm"
#include "bitmaps/redo.xpm"
#include "bitmaps/bold.xpm"
#include "bitmaps/italic.xpm"
#include "bitmaps/underline.xpm"

#include "bitmaps/alignleft.xpm"
#include "bitmaps/alignright.xpm"
#include "bitmaps/centre.xpm"
#include "bitmaps/font.xpm"
#include "bitmaps/indentless.xpm"
#include "bitmaps/indentmore.xpm"

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/richtext/richtextxml.h"
#include "wx/richtext/richtexthtml.h"
#include "wx/richtext/richtextformatdlg.h"
#include "wx/richtext/richtextsymboldlg.h"
#include "wx/richtext/richtextstyledlg.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
    virtual int OnExit();

    void CreateStyles();

    wxRichTextStyleSheet* GetStyleSheet() const { return m_styleSheet; }

    wxRichTextStyleSheet* m_styleSheet;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);

    void OnBold(wxCommandEvent& event);
    void OnItalic(wxCommandEvent& event);
    void OnUnderline(wxCommandEvent& event);

    void OnUpdateBold(wxUpdateUIEvent& event);
    void OnUpdateItalic(wxUpdateUIEvent& event);
    void OnUpdateUnderline(wxUpdateUIEvent& event);

    void OnAlignLeft(wxCommandEvent& event);
    void OnAlignCentre(wxCommandEvent& event);
    void OnAlignRight(wxCommandEvent& event);

    void OnUpdateAlignLeft(wxUpdateUIEvent& event);
    void OnUpdateAlignCentre(wxUpdateUIEvent& event);
    void OnUpdateAlignRight(wxUpdateUIEvent& event);

    void OnIndentMore(wxCommandEvent& event);
    void OnIndentLess(wxCommandEvent& event);

    void OnFont(wxCommandEvent& event);
    void OnParagraph(wxCommandEvent& event);
    void OnFormat(wxCommandEvent& event);
    void OnUpdateFormat(wxUpdateUIEvent& event);

    void OnInsertSymbol(wxCommandEvent& event);

    void OnLineSpacingHalf(wxCommandEvent& event);
    void OnLineSpacingDouble(wxCommandEvent& event);
    void OnLineSpacingSingle(wxCommandEvent& event);

    void OnParagraphSpacingMore(wxCommandEvent& event);
    void OnParagraphSpacingLess(wxCommandEvent& event);

    void OnNumberList(wxCommandEvent& event);
    void OnBulletsAndNumbering(wxCommandEvent& event);
    void OnItemizeList(wxCommandEvent& event);
    void OnRenumberList(wxCommandEvent& event);
    void OnPromoteList(wxCommandEvent& event);
    void OnDemoteList(wxCommandEvent& event);
    void OnClearList(wxCommandEvent& event);

    void OnViewHTML(wxCommandEvent& event);

    void OnSwitchStyleSheets(wxCommandEvent& event);
    void OnManageStyles(wxCommandEvent& event);

    // Forward command events to the current rich text control, if any
    bool ProcessEvent(wxEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

    wxRichTextCtrl*         m_richTextCtrl;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    ID_Quit = wxID_EXIT,
    ID_About = wxID_ABOUT,

    ID_FORMAT_BOLD = 100,
    ID_FORMAT_ITALIC,
    ID_FORMAT_UNDERLINE,
    ID_FORMAT_FONT,
    ID_FORMAT_PARAGRAPH,
    ID_FORMAT_CONTENT,

    ID_INSERT_SYMBOL,

    ID_FORMAT_ALIGN_LEFT,
    ID_FORMAT_ALIGN_CENTRE,
    ID_FORMAT_ALIGN_RIGHT,

    ID_FORMAT_INDENT_MORE,
    ID_FORMAT_INDENT_LESS,

    ID_FORMAT_PARAGRAPH_SPACING_MORE,
    ID_FORMAT_PARAGRAPH_SPACING_LESS,

    ID_FORMAT_LINE_SPACING_HALF,
    ID_FORMAT_LINE_SPACING_DOUBLE,
    ID_FORMAT_LINE_SPACING_SINGLE,

    ID_FORMAT_NUMBER_LIST,
    ID_FORMAT_BULLETS_AND_NUMBERING,
    ID_FORMAT_ITEMIZE_LIST,
    ID_FORMAT_RENUMBER_LIST,
    ID_FORMAT_PROMOTE_LIST,
    ID_FORMAT_DEMOTE_LIST,
    ID_FORMAT_CLEAR_LIST,

    ID_VIEW_HTML,
    ID_SWITCH_STYLE_SHEETS,
    ID_MANAGE_STYLES,

    ID_RICHTEXT_CTRL,
    ID_RICHTEXT_STYLE_LIST,
    ID_RICHTEXT_STYLE_COMBO
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit,  MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)

    EVT_MENU(wxID_OPEN,  MyFrame::OnOpen)
    EVT_MENU(wxID_SAVE,  MyFrame::OnSave)
    EVT_MENU(wxID_SAVEAS,  MyFrame::OnSaveAs)

    EVT_MENU(ID_FORMAT_BOLD,  MyFrame::OnBold)
    EVT_MENU(ID_FORMAT_ITALIC,  MyFrame::OnItalic)
    EVT_MENU(ID_FORMAT_UNDERLINE,  MyFrame::OnUnderline)

    EVT_UPDATE_UI(ID_FORMAT_BOLD,  MyFrame::OnUpdateBold)
    EVT_UPDATE_UI(ID_FORMAT_ITALIC,  MyFrame::OnUpdateItalic)
    EVT_UPDATE_UI(ID_FORMAT_UNDERLINE,  MyFrame::OnUpdateUnderline)

    EVT_MENU(ID_FORMAT_ALIGN_LEFT,  MyFrame::OnAlignLeft)
    EVT_MENU(ID_FORMAT_ALIGN_CENTRE,  MyFrame::OnAlignCentre)
    EVT_MENU(ID_FORMAT_ALIGN_RIGHT,  MyFrame::OnAlignRight)

    EVT_UPDATE_UI(ID_FORMAT_ALIGN_LEFT,  MyFrame::OnUpdateAlignLeft)
    EVT_UPDATE_UI(ID_FORMAT_ALIGN_CENTRE,  MyFrame::OnUpdateAlignCentre)
    EVT_UPDATE_UI(ID_FORMAT_ALIGN_RIGHT,  MyFrame::OnUpdateAlignRight)

    EVT_MENU(ID_FORMAT_FONT,  MyFrame::OnFont)
    EVT_MENU(ID_FORMAT_PARAGRAPH,  MyFrame::OnParagraph)
    EVT_MENU(ID_FORMAT_CONTENT,  MyFrame::OnFormat)
    EVT_UPDATE_UI(ID_FORMAT_CONTENT,  MyFrame::OnUpdateFormat)
    EVT_UPDATE_UI(ID_FORMAT_FONT,  MyFrame::OnUpdateFormat)
    EVT_UPDATE_UI(ID_FORMAT_PARAGRAPH,  MyFrame::OnUpdateFormat)
    EVT_MENU(ID_FORMAT_INDENT_MORE,  MyFrame::OnIndentMore)
    EVT_MENU(ID_FORMAT_INDENT_LESS,  MyFrame::OnIndentLess)

    EVT_MENU(ID_FORMAT_LINE_SPACING_HALF,  MyFrame::OnLineSpacingHalf)
    EVT_MENU(ID_FORMAT_LINE_SPACING_SINGLE,  MyFrame::OnLineSpacingSingle)
    EVT_MENU(ID_FORMAT_LINE_SPACING_DOUBLE,  MyFrame::OnLineSpacingDouble)

    EVT_MENU(ID_FORMAT_PARAGRAPH_SPACING_MORE,  MyFrame::OnParagraphSpacingMore)
    EVT_MENU(ID_FORMAT_PARAGRAPH_SPACING_LESS,  MyFrame::OnParagraphSpacingLess)

    EVT_MENU(ID_INSERT_SYMBOL,  MyFrame::OnInsertSymbol)

    EVT_MENU(ID_FORMAT_NUMBER_LIST, MyFrame::OnNumberList)
    EVT_MENU(ID_FORMAT_BULLETS_AND_NUMBERING, MyFrame::OnBulletsAndNumbering)
    EVT_MENU(ID_FORMAT_ITEMIZE_LIST, MyFrame::OnItemizeList)
    EVT_MENU(ID_FORMAT_RENUMBER_LIST, MyFrame::OnRenumberList)
    EVT_MENU(ID_FORMAT_PROMOTE_LIST, MyFrame::OnPromoteList)
    EVT_MENU(ID_FORMAT_DEMOTE_LIST, MyFrame::OnDemoteList)
    EVT_MENU(ID_FORMAT_CLEAR_LIST, MyFrame::OnClearList)

    EVT_MENU(ID_VIEW_HTML, MyFrame::OnViewHTML)
    EVT_MENU(ID_SWITCH_STYLE_SHEETS, MyFrame::OnSwitchStyleSheets)
    EVT_MENU(ID_MANAGE_STYLES, MyFrame::OnManageStyles)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
#if wxUSE_HELP
    wxHelpProvider::Set(new wxSimpleHelpProvider);
#endif

    m_styleSheet = new wxRichTextStyleSheet;

    CreateStyles();

    // Add extra handlers (plain text is automatically added)
    wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    wxRichTextBuffer::AddHandler(new wxRichTextHTMLHandler);

    // Add image handlers
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif

#if wxUSE_LIBJPEG
    wxImage::AddHandler( new wxJPEGHandler );
#endif

#if wxUSE_GIF
    wxImage::AddHandler( new wxGIFHandler );
#endif

    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxRichTextCtrl Sample"), wxID_ANY, wxDefaultPosition, wxSize(700, 600));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

int MyApp::OnExit()
{
    delete m_styleSheet;
    return 0;
}

void MyApp::CreateStyles()
{
    // Paragraph styles

    wxFont romanFont(12, wxROMAN, wxNORMAL, wxNORMAL);
    wxFont swissFont(12, wxSWISS, wxNORMAL, wxNORMAL);

    wxRichTextParagraphStyleDefinition* normalPara = new wxRichTextParagraphStyleDefinition(wxT("Normal"));
    wxRichTextAttr normalAttr;
    normalAttr.SetFontFaceName(romanFont.GetFaceName());
    normalAttr.SetFontSize(12);
    // Let's set all attributes for this style
    normalAttr.SetFlags(wxTEXT_ATTR_FONT | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|
                            wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|
                            wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER);
    normalPara->SetStyle(normalAttr);

    m_styleSheet->AddParagraphStyle(normalPara);

    wxRichTextParagraphStyleDefinition* indentedPara = new wxRichTextParagraphStyleDefinition(wxT("Indented"));
    wxRichTextAttr indentedAttr;
    indentedAttr.SetFontFaceName(romanFont.GetFaceName());
    indentedAttr.SetFontSize(12);
    indentedAttr.SetLeftIndent(100, 0);
    // We only want to affect indentation
    indentedAttr.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT);
    indentedPara->SetStyle(indentedAttr);

    m_styleSheet->AddParagraphStyle(indentedPara);

    wxRichTextParagraphStyleDefinition* indentedPara2 = new wxRichTextParagraphStyleDefinition(wxT("Red Bold Indented"));
    wxRichTextAttr indentedAttr2;
    indentedAttr2.SetFontFaceName(romanFont.GetFaceName());
    indentedAttr2.SetFontSize(12);
    indentedAttr2.SetFontWeight(wxBOLD);
    indentedAttr2.SetTextColour(*wxRED);
    indentedAttr2.SetFontSize(12);
    indentedAttr2.SetLeftIndent(100, 0);
    // We want to affect indentation, font and text colour
    indentedAttr2.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_FONT|wxTEXT_ATTR_TEXT_COLOUR);
    indentedPara2->SetStyle(indentedAttr2);

    m_styleSheet->AddParagraphStyle(indentedPara2);

    wxRichTextParagraphStyleDefinition* flIndentedPara = new wxRichTextParagraphStyleDefinition(wxT("First Line Indented"));
    wxRichTextAttr flIndentedAttr;
    flIndentedAttr.SetFontFaceName(swissFont.GetFaceName());
    flIndentedAttr.SetFontSize(12);
    flIndentedAttr.SetLeftIndent(100, -100);
    // We only want to affect indentation
    flIndentedAttr.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT);
    flIndentedPara->SetStyle(flIndentedAttr);

    m_styleSheet->AddParagraphStyle(flIndentedPara);

    // Character styles

    wxRichTextCharacterStyleDefinition* boldDef = new wxRichTextCharacterStyleDefinition(wxT("Bold"));
    wxRichTextAttr boldAttr;
    boldAttr.SetFontFaceName(romanFont.GetFaceName());
    boldAttr.SetFontSize(12);
    boldAttr.SetFontWeight(wxBOLD);
    // We only want to affect boldness
    boldAttr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
    boldDef->SetStyle(boldAttr);

    m_styleSheet->AddCharacterStyle(boldDef);

    wxRichTextCharacterStyleDefinition* italicDef = new wxRichTextCharacterStyleDefinition(wxT("Italic"));
    wxRichTextAttr italicAttr;
    italicAttr.SetFontFaceName(romanFont.GetFaceName());
    italicAttr.SetFontSize(12);
    italicAttr.SetFontStyle(wxITALIC);
    // We only want to affect italics
    italicAttr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
    italicDef->SetStyle(italicAttr);

    m_styleSheet->AddCharacterStyle(italicDef);

    wxRichTextCharacterStyleDefinition* redDef = new wxRichTextCharacterStyleDefinition(wxT("Red Bold"));
    wxRichTextAttr redAttr;
    redAttr.SetFontFaceName(romanFont.GetFaceName());
    redAttr.SetFontSize(12);
    redAttr.SetFontWeight(wxBOLD);
    redAttr.SetTextColour(*wxRED);
    // We only want to affect colour, weight and face
    redAttr.SetFlags(wxTEXT_ATTR_FONT_FACE|wxTEXT_ATTR_FONT_WEIGHT|wxTEXT_ATTR_TEXT_COLOUR);
    redDef->SetStyle(redAttr);

    m_styleSheet->AddCharacterStyle(redDef);

    wxRichTextListStyleDefinition* bulletList = new wxRichTextListStyleDefinition(wxT("Bullet List 1"));
    int i;
    for (i = 0; i < 10; i++)
    {
        wxString bulletSymbol;
        if (i == 0)
            bulletSymbol = wxT("*");
        else if (i == 1)
            bulletSymbol = wxT("-");
        else if (i == 2)
            bulletSymbol = wxT("*");
        else if (i == 3)
            bulletSymbol = wxT("-");
        else
            bulletSymbol = wxT("*");

        bulletList->SetAttributes(i, (i+1)*60, 60, wxTEXT_ATTR_BULLET_STYLE_SYMBOL, bulletSymbol);
    }

    m_styleSheet->AddListStyle(bulletList);

    wxRichTextListStyleDefinition* numberedList = new wxRichTextListStyleDefinition(wxT("Numbered List 1"));
    for (i = 0; i < 10; i++)
    {
        long numberStyle;
        if (i == 0)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD;
        else if (i == 1)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER|wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else if (i == 2)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER|wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else if (i == 3)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER|wxTEXT_ATTR_BULLET_STYLE_PARENTHESES;
        else
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD;

        numberedList->SetAttributes(i, (i+1)*60, 60, numberStyle);
    }

    m_styleSheet->AddListStyle(numberedList);
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
       : wxFrame(NULL, id, title, pos, size, style)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(wxID_OPEN, _T("&Open\tCtrl+O"), _T("Open a file"));
    fileMenu->Append(wxID_SAVE, _T("&Save\tCtrl+S"), _T("Save a file"));
    fileMenu->Append(wxID_SAVEAS, _T("&Save As...\tF12"), _T("Save to a new file"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_VIEW_HTML, _T("&View as HTML"), _T("View HTML"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Quit, _T("E&xit\tAlt+X"), _T("Quit this program"));

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, _("&Undo\tCtrl+Z"));
    editMenu->Append(wxID_REDO, _("&Redo\tCtrl+Y"));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, _("Cu&t\tCtrl+X"));
    editMenu->Append(wxID_COPY, _("&Copy\tCtrl+C"));
    editMenu->Append(wxID_PASTE, _("&Paste\tCtrl+V"));

    editMenu->Append(wxID_CLEAR, _("&Delete\tDel"));

    editMenu->AppendSeparator();
    editMenu->Append(wxID_SELECTALL, _("Select A&ll\tCtrl+A"));
#if 0
    editMenu->AppendSeparator();
    editMenu->Append(wxID_FIND, _("&Find...\tCtrl+F"));
    editMenu->Append(stID_FIND_REPLACE, _("&Replace...\tCtrl+R"));
#endif

    wxMenu* formatMenu = new wxMenu;
    formatMenu->AppendCheckItem(ID_FORMAT_BOLD, _("&Bold\tCtrl+B"));
    formatMenu->AppendCheckItem(ID_FORMAT_ITALIC, _("&Italic\tCtrl+I"));
    formatMenu->AppendCheckItem(ID_FORMAT_UNDERLINE, _("&Underline\tCtrl+U"));
    formatMenu->AppendSeparator();
    formatMenu->AppendCheckItem(ID_FORMAT_ALIGN_LEFT, _("L&eft Align"));
    formatMenu->AppendCheckItem(ID_FORMAT_ALIGN_RIGHT, _("&Right Align"));
    formatMenu->AppendCheckItem(ID_FORMAT_ALIGN_CENTRE, _("&Centre"));
    formatMenu->AppendSeparator();
    formatMenu->Append(ID_FORMAT_INDENT_MORE, _("Indent &More"));
    formatMenu->Append(ID_FORMAT_INDENT_LESS, _("Indent &Less"));
    formatMenu->AppendSeparator();
    formatMenu->Append(ID_FORMAT_PARAGRAPH_SPACING_MORE, _("Increase Paragraph &Spacing"));
    formatMenu->Append(ID_FORMAT_PARAGRAPH_SPACING_LESS, _("Decrease &Paragraph Spacing"));
    formatMenu->AppendSeparator();
    formatMenu->Append(ID_FORMAT_LINE_SPACING_SINGLE, _("Normal Line Spacing"));
    formatMenu->Append(ID_FORMAT_LINE_SPACING_HALF, _("1.5 Line Spacing"));
    formatMenu->Append(ID_FORMAT_LINE_SPACING_DOUBLE, _("Double Line Spacing"));
    formatMenu->AppendSeparator();
    formatMenu->Append(ID_FORMAT_FONT, _("&Font..."));
    formatMenu->Append(ID_FORMAT_PARAGRAPH, _("&Paragraph..."));
    formatMenu->Append(ID_FORMAT_CONTENT, _("Font and Pa&ragraph...\tShift+Ctrl+F"));
    formatMenu->AppendSeparator();
    formatMenu->Append(ID_SWITCH_STYLE_SHEETS, _("&Switch Style Sheets"));
    formatMenu->Append(ID_MANAGE_STYLES, _("&Manage Styles"));

    wxMenu* listsMenu = new wxMenu;
    listsMenu->Append(ID_FORMAT_BULLETS_AND_NUMBERING, _("Bullets and &Numbering..."));
    listsMenu->AppendSeparator();
    listsMenu->Append(ID_FORMAT_NUMBER_LIST, _("Number List"));
    listsMenu->Append(ID_FORMAT_ITEMIZE_LIST, _("Itemize List"));
    listsMenu->Append(ID_FORMAT_RENUMBER_LIST, _("Renumber List"));
    listsMenu->Append(ID_FORMAT_PROMOTE_LIST, _("Promote List Items"));
    listsMenu->Append(ID_FORMAT_DEMOTE_LIST, _("Demote List Items"));
    listsMenu->Append(ID_FORMAT_CLEAR_LIST, _("Clear List Formatting"));

    wxMenu* insertMenu = new wxMenu;
    insertMenu->Append(ID_INSERT_SYMBOL, _("&Symbol...\tCtrl+I"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(editMenu, _T("&Edit"));
    menuBar->Append(formatMenu, _T("F&ormat"));
    menuBar->Append(listsMenu, _T("&Lists"));
    menuBar->Append(insertMenu, _T("&Insert"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    // but don't create it on limited screen space (WinCE)
    bool is_pda = wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;

#if wxUSE_STATUSBAR
    if ( !is_pda )
    {
        CreateStatusBar(2);
        SetStatusText(_T("Welcome to wxRichTextCtrl!"));
    }
#endif

    wxToolBar* toolBar = CreateToolBar();

    toolBar->AddTool(wxID_OPEN, wxBitmap(open_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Open"));
    toolBar->AddTool(wxID_SAVEAS, wxBitmap(save_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Save"));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, wxBitmap(cut_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Cut"));
    toolBar->AddTool(wxID_COPY, wxBitmap(copy_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Copy"));
    toolBar->AddTool(wxID_PASTE, wxBitmap(paste_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Paste"));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_UNDO, wxBitmap(undo_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Undo"));
    toolBar->AddTool(wxID_REDO, wxBitmap(redo_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Redo"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_FORMAT_BOLD, wxBitmap(bold_xpm), wxNullBitmap, true, -1, -1, (wxObject *) NULL, _("Bold"));
    toolBar->AddTool(ID_FORMAT_ITALIC, wxBitmap(italic_xpm), wxNullBitmap, true, -1, -1, (wxObject *) NULL, _("Italic"));
    toolBar->AddTool(ID_FORMAT_UNDERLINE, wxBitmap(underline_xpm), wxNullBitmap, true, -1, -1, (wxObject *) NULL, _("Underline"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_FORMAT_ALIGN_LEFT, wxBitmap(alignleft_xpm), wxNullBitmap, true, -1, -1, (wxObject *) NULL, _("Align Left"));
    toolBar->AddTool(ID_FORMAT_ALIGN_CENTRE, wxBitmap(centre_xpm), wxNullBitmap, true, -1, -1, (wxObject *) NULL, _("Centre"));
    toolBar->AddTool(ID_FORMAT_ALIGN_RIGHT, wxBitmap(alignright_xpm), wxNullBitmap, true, -1, -1, (wxObject *) NULL, _("Align Right"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_FORMAT_INDENT_LESS, wxBitmap(indentless_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Indent Less"));
    toolBar->AddTool(ID_FORMAT_INDENT_MORE, wxBitmap(indentmore_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Indent More"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_FORMAT_FONT, wxBitmap(font_xpm), wxNullBitmap, false, -1, -1, (wxObject *) NULL, _("Font"));

    wxRichTextStyleComboCtrl* combo = new wxRichTextStyleComboCtrl(toolBar, ID_RICHTEXT_STYLE_COMBO, wxDefaultPosition, wxSize(200, -1));
    toolBar->AddControl(combo);

    toolBar->Realize();

    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, GetClientSize(), wxSP_NO_XP_THEME|wxSP_3D|wxSP_LIVE_UPDATE);

    wxFont textFont = wxFont(12, wxROMAN, wxNORMAL, wxNORMAL);
    wxFont boldFont = wxFont(12, wxROMAN, wxNORMAL, wxBOLD);
    wxFont italicFont = wxFont(12, wxROMAN, wxITALIC, wxNORMAL);

    m_richTextCtrl = new wxRichTextCtrl(splitter, ID_RICHTEXT_CTRL, wxEmptyString, wxDefaultPosition, wxSize(200, 200), wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
    wxFont font(12, wxROMAN, wxNORMAL, wxNORMAL);

    m_richTextCtrl->SetFont(font);

    m_richTextCtrl->SetStyleSheet(wxGetApp().GetStyleSheet());

    combo->SetStyleSheet(wxGetApp().GetStyleSheet());
    combo->SetRichTextCtrl(m_richTextCtrl);
    combo->UpdateStyles();

    wxRichTextStyleListCtrl* styleListCtrl = new wxRichTextStyleListCtrl(splitter, ID_RICHTEXT_STYLE_LIST);

    wxSize display = wxGetDisplaySize();
    if ( is_pda && ( display.GetWidth() < display.GetHeight() ) )
    {
        splitter->SplitHorizontally(m_richTextCtrl, styleListCtrl);
    }
    else
    {
        splitter->SplitVertically(m_richTextCtrl, styleListCtrl, 500);
    }

    splitter->UpdateSize();

    styleListCtrl->SetStyleSheet(wxGetApp().GetStyleSheet());
    styleListCtrl->SetRichTextCtrl(m_richTextCtrl);
    styleListCtrl->UpdateStyles();

    wxRichTextCtrl& r = *m_richTextCtrl;

    r.BeginSuppressUndo();

    r.BeginParagraphSpacing(0, 20);

    r.BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    r.BeginBold();

    r.BeginFontSize(14);
    r.WriteText(wxT("Welcome to wxRichTextCtrl, a wxWidgets control for editing and presenting styled text and images"));
    r.EndFontSize();
    r.Newline();

    r.BeginItalic();
    r.WriteText(wxT("by Julian Smart"));
    r.EndItalic();

    r.EndBold();

    r.Newline();
    r.WriteImage(wxBitmap(zebra_xpm));

    r.EndAlignment();

    r.Newline();
    r.Newline();

    r.WriteText(wxT("What can you do with this thing? "));
    r.WriteImage(wxBitmap(smiley_xpm));
    r.WriteText(wxT(" Well, you can change text "));

    r.BeginTextColour(wxColour(255, 0, 0));
    r.WriteText(wxT("colour, like this red bit."));
    r.EndTextColour();

    r.BeginTextColour(wxColour(0, 0, 255));
    r.WriteText(wxT(" And this blue bit."));
    r.EndTextColour();

    r.WriteText(wxT(" Naturally you can make things "));
    r.BeginBold();
    r.WriteText(wxT("bold "));
    r.EndBold();
    r.BeginItalic();
    r.WriteText(wxT("or italic "));
    r.EndItalic();
    r.BeginUnderline();
    r.WriteText(wxT("or underlined."));
    r.EndUnderline();

    r.BeginFontSize(14);
    r.WriteText(wxT(" Different font sizes on the same line is allowed, too."));
    r.EndFontSize();

    r.WriteText(wxT(" Next we'll show an indented paragraph."));

    r.BeginLeftIndent(60);
    r.Newline();

    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.EndLeftIndent();

    r.Newline();

    r.WriteText(wxT("Next, we'll show a first-line indent, achieved using BeginLeftIndent(100, -40)."));

    r.BeginLeftIndent(100, -40);
    r.Newline();

    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.EndLeftIndent();

    r.Newline();

    r.WriteText(wxT("Numbered bullets are possible, again using subindents:"));

    r.BeginNumberedBullet(1, 100, 60);
    r.Newline();

    r.WriteText(wxT("This is my first item. Note that wxRichTextCtrl can apply numbering and bullets automatically based on list styles, but this list is formatted explicitly by setting indents."));
    r.EndNumberedBullet();

    r.BeginNumberedBullet(2, 100, 60);
    r.Newline();

    r.WriteText(wxT("This is my second item."));
    r.EndNumberedBullet();

    r.Newline();

    r.WriteText(wxT("The following paragraph is right-indented:"));

    r.BeginRightIndent(200);
    r.Newline();

    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.EndRightIndent();

    r.Newline();

    r.WriteText(wxT("The following paragraph is right-aligned with 1.5 line spacing:"));

    r.BeginAlignment(wxTEXT_ALIGNMENT_RIGHT);
    r.BeginLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);
    r.Newline();

    r.WriteText(wxT("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable."));
    r.EndLineSpacing();
    r.EndAlignment();

    wxArrayInt tabs;
    tabs.Add(400);
    tabs.Add(600);
    tabs.Add(800);
    tabs.Add(1000);
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_TABS);
    attr.SetTabs(tabs);
    r.SetDefaultStyle(attr);

    r.Newline();
    r.WriteText(wxT("This line contains tabs:\tFirst tab\tSecond tab\tThird tab"));

    r.Newline();
    r.WriteText(wxT("Other notable features of wxRichTextCtrl include:"));

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("Compatibility with wxTextCtrl API"));
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("Easy stack-based BeginXXX()...EndXXX() style setting in addition to SetStyle()"));
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("XML loading and saving"));
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("Undo/Redo, with batching option and Undo suppressing"));
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("Clipboard copy and paste"));
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("wxRichTextStyleSheet with named character and paragraph styles, and control for applying named styles"));
    r.EndSymbolBullet();

    r.BeginSymbolBullet(wxT('*'), 100, 60);
    r.Newline();
    r.WriteText(wxT("A design that can easily be extended to other content types, ultimately with text boxes, tables, controls, and so on"));
    r.EndSymbolBullet();

    r.Newline();

    r.WriteText(wxT("Note: this sample content was generated programmatically from within the MyFrame constructor in the demo. The images were loaded from inline XPMs. Enjoy wxRichTextCtrl!"));

    r.EndParagraphSpacing();

    r.EndSuppressUndo();
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is a demo for wxRichTextCtrl, a control for editing styled text.\n(c) Julian Smart, 2005"));
    wxMessageBox(msg, _T("About wxRichTextCtrl Sample"), wxOK | wxICON_INFORMATION, this);
}

// Forward command events to the current rich text control, if any
bool MyFrame::ProcessEvent(wxEvent& event)
{
    if (event.IsCommandEvent() && !event.IsKindOf(CLASSINFO(wxChildFocusEvent)))
    {
        // Problem: we can get infinite recursion because the events
        // climb back up to this frame, and repeat.
        // Assume that command events don't cause another command event
        // to be called, so we can rely on inCommand not being overwritten

        static int s_eventType = 0;
        static wxWindowID s_id = 0;

        if (s_id != event.GetId() && s_eventType != event.GetEventType())
        {
            s_eventType = event.GetEventType();
            s_id = event.GetId();

            wxWindow* focusWin = wxFindFocusDescendant(this);
            if (focusWin && focusWin->ProcessEvent(event))
            {
                //s_command = NULL;
                s_eventType = 0;
                s_id = 0;
                return true;
            }

            s_eventType = 0;
            s_id = 0;
        }
        else
        {
            return false;
        }
    }

    return wxFrame::ProcessEvent(event);
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString path;
    wxString filename;
    wxArrayInt fileTypes;

    wxString filter = wxRichTextBuffer::GetExtWildcard(false, false, & fileTypes);
    if (!filter.empty())
        filter += wxT("|");
    filter += wxT("All files (*.*)|*.*");

    wxFileDialog dialog(this,
        _("Choose a filename"),
        path,
        filename,
        filter,
        wxFD_OPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString path = dialog.GetPath();

        if (!path.empty())
        {
            int filterIndex = dialog.GetFilterIndex();
            int fileType = (filterIndex < (int) fileTypes.GetCount())
                           ? fileTypes[filterIndex]
                           : wxRICHTEXT_TYPE_TEXT;
            m_richTextCtrl->LoadFile(path, fileType);
        }
    }
}

void MyFrame::OnSave(wxCommandEvent& event)
{
    if (m_richTextCtrl->GetFilename().empty())
    {
        OnSaveAs(event);
        return;
    }
    m_richTextCtrl->SaveFile();
}

void MyFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
    wxString filter = wxRichTextBuffer::GetExtWildcard(false, true);
    wxString path;
    wxString filename;

    wxFileDialog dialog(this,
        _("Choose a filename"),
        path,
        filename,
        filter,
        wxFD_SAVE);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString path = dialog.GetPath();

        if (!path.empty())
        {
            m_richTextCtrl->SaveFile(path);
        }
    }
}

void MyFrame::OnBold(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyBoldToSelection();
}

void MyFrame::OnItalic(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyItalicToSelection();
}

void MyFrame::OnUnderline(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyUnderlineToSelection();
}


void MyFrame::OnUpdateBold(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->IsSelectionBold());
}

void MyFrame::OnUpdateItalic(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->IsSelectionItalics());
}

void MyFrame::OnUpdateUnderline(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->IsSelectionUnderlined());
}

void MyFrame::OnAlignLeft(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_LEFT);
}

void MyFrame::OnAlignCentre(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_CENTRE);
}

void MyFrame::OnAlignRight(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_RIGHT);
}

void MyFrame::OnUpdateAlignLeft(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_LEFT));
}

void MyFrame::OnUpdateAlignCentre(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTRE));
}

void MyFrame::OnUpdateAlignRight(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));
}

void MyFrame::OnFont(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextRange range;
    if (m_richTextCtrl->HasSelection())
        range = m_richTextCtrl->GetSelectionRange();
    else
        range = wxRichTextRange(0, m_richTextCtrl->GetLastPosition()+1);

    int pages = wxRICHTEXT_FORMAT_FONT;

    wxRichTextFormattingDialog formatDlg(pages, this);
    formatDlg.GetStyle(m_richTextCtrl, range);

    if (formatDlg.ShowModal() == wxID_OK)
    {
        formatDlg.ApplyStyle(m_richTextCtrl, range);
    }

    // Old method using wxFontDialog
#if 0
    if (!m_richTextCtrl->HasSelection())
        return;

    wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
    wxFontData fontData;

    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_FONT);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
        fontData.SetInitialFont(attr.GetFont());

    wxFontDialog dialog(this, fontData);
    if (dialog.ShowModal() == wxID_OK)
    {
        fontData = dialog.GetFontData();
        attr.SetFlags(wxTEXT_ATTR_FONT);
        attr.SetFont(fontData.GetChosenFont());
        if (attr.GetFont().Ok())
        {
            m_richTextCtrl->SetStyle(range, attr);
        }
    }
#endif
}

void MyFrame::OnParagraph(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextRange range;
    if (m_richTextCtrl->HasSelection())
        range = m_richTextCtrl->GetSelectionRange();
    else
        range = wxRichTextRange(0, m_richTextCtrl->GetLastPosition()+1);

    int pages = wxRICHTEXT_FORMAT_INDENTS_SPACING|wxRICHTEXT_FORMAT_TABS|wxRICHTEXT_FORMAT_BULLETS;

    wxRichTextFormattingDialog formatDlg(pages, this);
    formatDlg.GetStyle(m_richTextCtrl, range);

    if (formatDlg.ShowModal() == wxID_OK)
    {
        formatDlg.ApplyStyle(m_richTextCtrl, range);
    }
}

void MyFrame::OnFormat(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextRange range;
    if (m_richTextCtrl->HasSelection())
        range = m_richTextCtrl->GetSelectionRange();
    else
        range = wxRichTextRange(0, m_richTextCtrl->GetLastPosition()+1);

    int pages = wxRICHTEXT_FORMAT_FONT|wxRICHTEXT_FORMAT_INDENTS_SPACING|wxRICHTEXT_FORMAT_TABS|wxRICHTEXT_FORMAT_BULLETS;

    wxRichTextFormattingDialog formatDlg(pages, this);
    formatDlg.GetStyle(m_richTextCtrl, range);

    if (formatDlg.ShowModal() == wxID_OK)
    {
        formatDlg.ApplyStyle(m_richTextCtrl, range);
    }
}

void MyFrame::OnUpdateFormat(wxUpdateUIEvent& event)
{
    event.Enable(m_richTextCtrl->HasSelection());
}

void MyFrame::OnIndentMore(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        wxFontData fontData;
        attr.SetLeftIndent(attr.GetLeftIndent() + 100);

        attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);
        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnIndentLess(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        if (attr.GetLeftIndent() >= 100)
        {
            wxFontData fontData;
            attr.SetLeftIndent(attr.GetLeftIndent() - 100);

            m_richTextCtrl->SetStyle(range, attr);
        }
    }
}

void MyFrame::OnLineSpacingHalf(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        wxFontData fontData;
        attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
        attr.SetLineSpacing(15);

        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnLineSpacingDouble(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        wxFontData fontData;
        attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
        attr.SetLineSpacing(20);

        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnLineSpacingSingle(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        wxFontData fontData;
        attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
        attr.SetLineSpacing(0); // Can also use 10

        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnParagraphSpacingMore(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        wxFontData fontData;
        attr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter() + 20);

        attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);
        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnParagraphSpacingLess(wxCommandEvent& WXUNUSED(event))
{
    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        if (attr.GetParagraphSpacingAfter() >= 20)
        {
            wxFontData fontData;
            attr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter() - 20);

            attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);
            m_richTextCtrl->SetStyle(range, attr);
        }
    }
}

void MyFrame::OnViewHTML(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dialog(this, wxID_ANY, _("HTML"), wxDefaultPosition, wxSize(500, 400), wxDEFAULT_DIALOG_STYLE);

    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
    dialog.SetSizer(boxSizer);

    wxHtmlWindow* win = new wxHtmlWindow(& dialog, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSUNKEN_BORDER);
    boxSizer->Add(win, 1, wxALL, 5);

    wxButton* cancelButton = new wxButton(& dialog, wxID_CANCEL, wxT("&Close"));
    boxSizer->Add(cancelButton, 0, wxALL|wxCENTRE, 5);

    wxString text;
    wxStringOutputStream strStream(& text);

    wxRichTextHTMLHandler htmlHandler;
    if (htmlHandler.SaveFile(& m_richTextCtrl->GetBuffer(), strStream))
    {
        win->SetPage(text);
    }

    boxSizer->Fit(& dialog);

    dialog.ShowModal();
}

// Demonstrates how you can change the style sheets and have the changes
// reflected in the control content without wiping out character formatting.

void MyFrame::OnSwitchStyleSheets(wxCommandEvent& WXUNUSED(event))
{
    static wxRichTextStyleSheet* gs_AlternateStyleSheet = NULL;

    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);
    wxRichTextStyleListCtrl *styleList = (wxRichTextStyleListCtrl*) FindWindow(ID_RICHTEXT_STYLE_LIST);
    wxRichTextStyleComboCtrl* styleCombo = (wxRichTextStyleComboCtrl*) FindWindow(ID_RICHTEXT_STYLE_COMBO);

    wxRichTextStyleSheet* sheet = ctrl->GetStyleSheet();

    // One-time creation of an alternate style sheet
    if (!gs_AlternateStyleSheet)
    {
        gs_AlternateStyleSheet = new wxRichTextStyleSheet(*sheet);

        // Make some modifications
        for (int i = 0; i < (int) gs_AlternateStyleSheet->GetParagraphStyleCount(); i++)
        {
            wxRichTextParagraphStyleDefinition* def = gs_AlternateStyleSheet->GetParagraphStyle(i);

            if (def->GetStyle().HasTextColour())
                def->GetStyle().SetTextColour(*wxBLUE);

            if (def->GetStyle().HasAlignment())
            {
                if (def->GetStyle().GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
                    def->GetStyle().SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
                else if (def->GetStyle().GetAlignment() == wxTEXT_ALIGNMENT_LEFT)
                    def->GetStyle().SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
            }
            if (def->GetStyle().HasLeftIndent())
            {
                def->GetStyle().SetLeftIndent(def->GetStyle().GetLeftIndent() * 2);
            }
        }
    }

    // Switch sheets
    wxRichTextStyleSheet* tmp = gs_AlternateStyleSheet;
    gs_AlternateStyleSheet = sheet;
    sheet = tmp;

    ctrl->SetStyleSheet(sheet);
    ctrl->ApplyStyleSheet(sheet); // Makes the control reflect the new style definitions

    styleList->SetStyleSheet(sheet);
    styleList->UpdateStyles();

    styleCombo->SetStyleSheet(sheet);
    styleCombo->UpdateStyles();
}

void MyFrame::OnManageStyles(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);
    wxRichTextStyleSheet* sheet = ctrl->GetStyleSheet();

    int flags = wxRICHTEXT_ORGANISER_CREATE_STYLES|wxRICHTEXT_ORGANISER_EDIT_STYLES;

    wxRichTextStyleOrganiserDialog dlg(flags, sheet, NULL, this, wxID_ANY, _("Style Manager"));
    dlg.ShowModal();
}

void MyFrame::OnInsertSymbol(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);

    wxTextAttrEx attr;
    attr.SetFlags(wxTEXT_ATTR_FONT);
    ctrl->GetStyle(ctrl->GetInsertionPoint(), attr);

    wxString currentFontName;
    if (attr.HasFont() && attr.GetFont().Ok())
        currentFontName = attr.GetFont().GetFaceName();

    // Don't set the initial font in the dialog (so the user is choosing
    // 'normal text', i.e. the current font) but do tell the dialog
    // what 'normal text' is.

    wxSymbolPickerDialog dlg(wxT("*"), wxEmptyString, currentFontName, this);

    if (dlg.ShowModal() == wxID_OK)
    {
        if (dlg.HasSelection())
        {
            long insertionPoint = ctrl->GetInsertionPoint();

            ctrl->WriteText(dlg.GetSymbol());

            if (!dlg.UseNormalFont())
            {
                wxFont font(attr.GetFont());
                font.SetFaceName(dlg.GetFontName());
                attr.SetFont(font);
                ctrl->SetStyle(insertionPoint, insertionPoint+1, attr);
            }
        }
    }
}

void MyFrame::OnNumberList(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);
    
    if (ctrl->HasSelection())
    {
        wxRichTextRange range = ctrl->GetSelectionRange();
        ctrl->SetListStyle(range, wxT("Numbered List 1"), wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RENUMBER);
    }
}

void MyFrame::OnBulletsAndNumbering(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);
    wxRichTextStyleSheet* sheet = ctrl->GetStyleSheet();

    int flags = wxRICHTEXT_ORGANISER_BROWSE_NUMBERING;

    wxRichTextStyleOrganiserDialog dlg(flags, sheet, ctrl, this, wxID_ANY, _("Bullets and Numbering"));
    if (dlg.ShowModal() == wxID_OK)
    {
        if (dlg.GetSelectedStyleDefinition())
            dlg.ApplyStyle();
    }
}

void MyFrame::OnItemizeList(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);

    if (ctrl->HasSelection())
    {
        wxRichTextRange range = ctrl->GetSelectionRange();
        ctrl->SetListStyle(range, wxT("Bullet List 1"));
    }
}

void MyFrame::OnRenumberList(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);

    if (ctrl->HasSelection())
    {
        wxRichTextRange range = ctrl->GetSelectionRange();
        ctrl->NumberList(range, NULL, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RENUMBER);
    }
}

void MyFrame::OnPromoteList(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);

    if (ctrl->HasSelection())
    {
        wxRichTextRange range = ctrl->GetSelectionRange();
        ctrl->PromoteList(1, range, NULL);
    }
}

void MyFrame::OnDemoteList(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);

    if (ctrl->HasSelection())
    {
        wxRichTextRange range = ctrl->GetSelectionRange();
        ctrl->PromoteList(-1, range, NULL);
    }
}

void MyFrame::OnClearList(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextCtrl* ctrl = (wxRichTextCtrl*) FindWindow(ID_RICHTEXT_CTRL);

    if (ctrl->HasSelection())
    {
        wxRichTextRange range = ctrl->GetSelectionRange();
        ctrl->ClearListStyle(range);
    }
}

