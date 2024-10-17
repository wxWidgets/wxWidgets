/////////////////////////////////////////////////////////////////////////////
// Name:        samples/richtext/richtext.cpp
// Purpose:     wxWidgets rich text editor sample
// Author:      Julian Smart
// Created:     2005-10-02
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


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/fontdlg.h"
#include "wx/splitter.h"
#include "wx/sstream.h"
#include "wx/html/htmlwin.h"
#include "wx/stopwatch.h"
#include "wx/sysopt.h"

#if wxUSE_FILESYSTEM
#include "wx/filesys.h"
#include "wx/fs_mem.h"
#endif

#if wxUSE_HELP
#include "wx/cshelp.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
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
#include "wx/richtext/richtextprint.h"
#include "wx/richtext/richtextimagedlg.h"

// A custom field type
class wxRichTextFieldTypePropertiesTest: public wxRichTextFieldTypeStandard
{
public:
    wxRichTextFieldTypePropertiesTest(const wxString& name, const wxString& label, int displayStyle = wxRICHTEXT_FIELD_STYLE_RECTANGLE):
        wxRichTextFieldTypeStandard(name, label, displayStyle)
    {
    }
    wxRichTextFieldTypePropertiesTest(const wxString& name, const wxBitmap& bitmap, int displayStyle = wxRICHTEXT_FIELD_STYLE_RECTANGLE):
        wxRichTextFieldTypeStandard(name, bitmap, displayStyle)
    {
    }

    virtual bool CanEditProperties(wxRichTextField* WXUNUSED(obj)) const override { return true; }
    virtual bool EditProperties(wxRichTextField* WXUNUSED(obj), wxWindow* WXUNUSED(parent), wxRichTextBuffer* WXUNUSED(buffer)) override
    {
        wxString label = GetLabel();
        wxMessageBox(wxString::Format("Editing %s", label));
        return true;
    }

    virtual wxString GetPropertiesMenuLabel(wxRichTextField* WXUNUSED(obj)) const override
    {
        return GetLabel();
    }
};

// A custom composite field type
class wxRichTextFieldTypeCompositeTest: public wxRichTextFieldTypePropertiesTest
{
public:
    wxRichTextFieldTypeCompositeTest(const wxString& name, const wxString& label):
        wxRichTextFieldTypePropertiesTest(name, label, wxRICHTEXT_FIELD_STYLE_COMPOSITE)
    {
    }

    virtual bool UpdateField(wxRichTextBuffer* buffer, wxRichTextField* obj) override
    {
        if (buffer)
        {
            wxRichTextAttr attr(buffer->GetAttributes());
            attr.GetTextBoxAttr().Reset();
            attr.SetParagraphSpacingAfter(0);
            attr.SetLineSpacing(10);
            obj->SetAttributes(attr);
        }
        obj->DeleteChildren();
        wxRichTextParagraph* para = new wxRichTextParagraph;
        wxRichTextPlainText* text = new wxRichTextPlainText(GetLabel());
        para->AppendChild(text);
        obj->AppendChild(para);
        return true;
   }
};

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyRichTextCtrl: public wxRichTextCtrl
{
public:
    MyRichTextCtrl( wxWindow* parent, wxWindowID id = -1, const wxString& value = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxRE_MULTILINE, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr):
      wxRichTextCtrl(parent, id, value, pos, size, style, validator, name)
    {
        m_lockId = 0;
        m_locked = false;
    }

    void SetLockId(long id) { m_lockId = id; }
    long GetLockId() const { return m_lockId; }

    void BeginLock() { m_lockId ++; m_locked = true; }
    void EndLock() { m_locked = false; }
    bool IsLocked() const { return m_locked; }

    static void SetEnhancedDrawingHandler();

    /**
        Prepares the content just before insertion (or after buffer reset). Called by the same function in wxRichTextBuffer.
        Currently is only called if undo mode is on.
    */
    virtual void PrepareContent(wxRichTextParagraphLayoutBox& container) override;

    /**
        Can we delete this range?
        Sends an event to the control.
    */
    virtual bool CanDeleteRange(wxRichTextParagraphLayoutBox& container, const wxRichTextRange& range) const override;

    /**
        Can we insert content at this position?
        Sends an event to the control.
    */
    virtual bool CanInsertContent(wxRichTextParagraphLayoutBox& container, long pos) const override;

    /**
        Finds a table,  either selected or near the cursor
    */
    wxRichTextTable* FindTable() const;

    /**
        Helper for FindTable()
    */
    wxRichTextObject* FindCurrentPosition() const;

    long    m_lockId;
    bool    m_locked;
};

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() override;
    virtual int OnExit() override;

    void CreateStyles();

    wxRichTextStyleSheet* GetStyleSheet() const { return m_styleSheet; }
    wxRichTextStyleSheet*   m_styleSheet;

#if wxUSE_PRINTING_ARCHITECTURE
    wxRichTextPrinting* GetPrinting() const { return m_printing; }
    wxRichTextPrinting*     m_printing;
#endif
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

    void OnStrikethrough(wxCommandEvent& event);
    void OnSuperscript(wxCommandEvent& event);
    void OnSubscript(wxCommandEvent& event);

    void OnUpdateBold(wxUpdateUIEvent& event);
    void OnUpdateItalic(wxUpdateUIEvent& event);
    void OnUpdateUnderline(wxUpdateUIEvent& event);
    void OnUpdateStrikethrough(wxUpdateUIEvent& event);
    void OnUpdateSuperscript(wxUpdateUIEvent& event);
    void OnUpdateSubscript(wxUpdateUIEvent& event);

    void OnAlignLeft(wxCommandEvent& event);
    void OnAlignCentre(wxCommandEvent& event);
    void OnAlignRight(wxCommandEvent& event);

    void OnUpdateAlignLeft(wxUpdateUIEvent& event);
    void OnUpdateAlignCentre(wxUpdateUIEvent& event);
    void OnUpdateAlignRight(wxUpdateUIEvent& event);

    void OnIndentMore(wxCommandEvent& event);
    void OnIndentLess(wxCommandEvent& event);

    void OnFont(wxCommandEvent& event);
    void OnImage(wxCommandEvent& event);
    void OnUpdateImage(wxUpdateUIEvent& event);
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

    void OnTableAddColumn(wxCommandEvent& event);
    void OnTableAddRow(wxCommandEvent& event);
    void OnTableDeleteColumn(wxCommandEvent& event);
    void OnTableDeleteRow(wxCommandEvent& event);
    void OnTableFocusedUpdateUI(wxUpdateUIEvent& event);
    void OnTableHasCellsUpdateUI(wxUpdateUIEvent& event);

    void OnReload(wxCommandEvent& event);

    void OnViewHTML(wxCommandEvent& event);

    void OnSwitchStyleSheets(wxCommandEvent& event);
    void OnManageStyles(wxCommandEvent& event);

    void OnInsertURL(wxCommandEvent& event);
    void OnURL(wxTextUrlEvent& event);
    void OnStyleSheetReplacing(wxRichTextEvent& event);

#if wxUSE_PRINTING_ARCHITECTURE
    void OnPrint(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);
#endif
    void OnPageSetup(wxCommandEvent& event);

    void OnInsertImage(wxCommandEvent& event);

    void OnSetFontScale(wxCommandEvent& event);
    void OnSetDimensionScale(wxCommandEvent& event);
protected:

    // Forward command events to the current rich text control, if any
    bool ProcessEvent(wxEvent& event) override;

    // Write text
    void WriteInitialText();

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();

    MyRichTextCtrl*         m_richTextCtrl;
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
    ID_FORMAT_STRIKETHROUGH,
    ID_FORMAT_SUPERSCRIPT,
    ID_FORMAT_SUBSCRIPT,
    ID_FORMAT_FONT,
    ID_FORMAT_IMAGE,
    ID_FORMAT_PARAGRAPH,
    ID_FORMAT_CONTENT,

    ID_RELOAD,

    ID_INSERT_SYMBOL,
    ID_INSERT_URL,
    ID_INSERT_IMAGE,

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

    ID_TABLE_ADD_COLUMN,
    ID_TABLE_ADD_ROW,
    ID_TABLE_DELETE_COLUMN,
    ID_TABLE_DELETE_ROW,

    ID_SET_FONT_SCALE,
    ID_SET_DIMENSION_SCALE,

    ID_VIEW_HTML,
    ID_SWITCH_STYLE_SHEETS,
    ID_MANAGE_STYLES,

    ID_PRINT,
    ID_PREVIEW,
    ID_PAGE_SETUP,

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
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit,  MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)

    EVT_MENU(wxID_OPEN,  MyFrame::OnOpen)
    EVT_MENU(wxID_SAVE,  MyFrame::OnSave)
    EVT_MENU(wxID_SAVEAS,  MyFrame::OnSaveAs)

    EVT_MENU(ID_FORMAT_BOLD,  MyFrame::OnBold)
    EVT_MENU(ID_FORMAT_ITALIC,  MyFrame::OnItalic)
    EVT_MENU(ID_FORMAT_UNDERLINE,  MyFrame::OnUnderline)

    EVT_MENU(ID_FORMAT_STRIKETHROUGH,  MyFrame::OnStrikethrough)
    EVT_MENU(ID_FORMAT_SUPERSCRIPT,  MyFrame::OnSuperscript)
    EVT_MENU(ID_FORMAT_SUBSCRIPT,  MyFrame::OnSubscript)

    EVT_UPDATE_UI(ID_FORMAT_BOLD,  MyFrame::OnUpdateBold)
    EVT_UPDATE_UI(ID_FORMAT_ITALIC,  MyFrame::OnUpdateItalic)
    EVT_UPDATE_UI(ID_FORMAT_UNDERLINE,  MyFrame::OnUpdateUnderline)

    EVT_UPDATE_UI(ID_FORMAT_STRIKETHROUGH,  MyFrame::OnUpdateStrikethrough)
    EVT_UPDATE_UI(ID_FORMAT_SUPERSCRIPT,  MyFrame::OnUpdateSuperscript)
    EVT_UPDATE_UI(ID_FORMAT_SUBSCRIPT,  MyFrame::OnUpdateSubscript)

    EVT_MENU(ID_FORMAT_ALIGN_LEFT,  MyFrame::OnAlignLeft)
    EVT_MENU(ID_FORMAT_ALIGN_CENTRE,  MyFrame::OnAlignCentre)
    EVT_MENU(ID_FORMAT_ALIGN_RIGHT,  MyFrame::OnAlignRight)

    EVT_UPDATE_UI(ID_FORMAT_ALIGN_LEFT,  MyFrame::OnUpdateAlignLeft)
    EVT_UPDATE_UI(ID_FORMAT_ALIGN_CENTRE,  MyFrame::OnUpdateAlignCentre)
    EVT_UPDATE_UI(ID_FORMAT_ALIGN_RIGHT,  MyFrame::OnUpdateAlignRight)

    EVT_MENU(ID_FORMAT_FONT,  MyFrame::OnFont)
    EVT_MENU(ID_FORMAT_IMAGE, MyFrame::OnImage)
    EVT_MENU(ID_FORMAT_PARAGRAPH,  MyFrame::OnParagraph)
    EVT_MENU(ID_FORMAT_CONTENT,  MyFrame::OnFormat)
    EVT_UPDATE_UI(ID_FORMAT_CONTENT,  MyFrame::OnUpdateFormat)
    EVT_UPDATE_UI(ID_FORMAT_FONT,  MyFrame::OnUpdateFormat)
    EVT_UPDATE_UI(ID_FORMAT_IMAGE, MyFrame::OnUpdateImage)
    EVT_UPDATE_UI(ID_FORMAT_PARAGRAPH,  MyFrame::OnUpdateFormat)
    EVT_MENU(ID_FORMAT_INDENT_MORE,  MyFrame::OnIndentMore)
    EVT_MENU(ID_FORMAT_INDENT_LESS,  MyFrame::OnIndentLess)

    EVT_MENU(ID_FORMAT_LINE_SPACING_HALF,  MyFrame::OnLineSpacingHalf)
    EVT_MENU(ID_FORMAT_LINE_SPACING_SINGLE,  MyFrame::OnLineSpacingSingle)
    EVT_MENU(ID_FORMAT_LINE_SPACING_DOUBLE,  MyFrame::OnLineSpacingDouble)

    EVT_MENU(ID_FORMAT_PARAGRAPH_SPACING_MORE,  MyFrame::OnParagraphSpacingMore)
    EVT_MENU(ID_FORMAT_PARAGRAPH_SPACING_LESS,  MyFrame::OnParagraphSpacingLess)

    EVT_MENU(ID_RELOAD,  MyFrame::OnReload)

    EVT_MENU(ID_INSERT_SYMBOL,  MyFrame::OnInsertSymbol)
    EVT_MENU(ID_INSERT_URL,  MyFrame::OnInsertURL)
    EVT_MENU(ID_INSERT_IMAGE, MyFrame::OnInsertImage)

    EVT_MENU(ID_FORMAT_NUMBER_LIST, MyFrame::OnNumberList)
    EVT_MENU(ID_FORMAT_BULLETS_AND_NUMBERING, MyFrame::OnBulletsAndNumbering)
    EVT_MENU(ID_FORMAT_ITEMIZE_LIST, MyFrame::OnItemizeList)
    EVT_MENU(ID_FORMAT_RENUMBER_LIST, MyFrame::OnRenumberList)
    EVT_MENU(ID_FORMAT_PROMOTE_LIST, MyFrame::OnPromoteList)
    EVT_MENU(ID_FORMAT_DEMOTE_LIST, MyFrame::OnDemoteList)
    EVT_MENU(ID_FORMAT_CLEAR_LIST, MyFrame::OnClearList)

    EVT_MENU(ID_TABLE_ADD_COLUMN, MyFrame::OnTableAddColumn)
    EVT_MENU(ID_TABLE_ADD_ROW, MyFrame::OnTableAddRow)
    EVT_MENU(ID_TABLE_DELETE_COLUMN, MyFrame::OnTableDeleteColumn)
    EVT_MENU(ID_TABLE_DELETE_ROW, MyFrame::OnTableDeleteRow)
    EVT_UPDATE_UI_RANGE(ID_TABLE_ADD_COLUMN, ID_TABLE_ADD_ROW, MyFrame::OnTableFocusedUpdateUI)
    EVT_UPDATE_UI_RANGE(ID_TABLE_DELETE_COLUMN, ID_TABLE_DELETE_ROW, MyFrame::OnTableHasCellsUpdateUI)

    EVT_MENU(ID_VIEW_HTML, MyFrame::OnViewHTML)
    EVT_MENU(ID_SWITCH_STYLE_SHEETS, MyFrame::OnSwitchStyleSheets)
    EVT_MENU(ID_MANAGE_STYLES, MyFrame::OnManageStyles)

#if wxUSE_PRINTING_ARCHITECTURE
    EVT_MENU(ID_PRINT, MyFrame::OnPrint)
    EVT_MENU(ID_PREVIEW, MyFrame::OnPreview)
#endif
    EVT_MENU(ID_PAGE_SETUP, MyFrame::OnPageSetup)

    EVT_TEXT_URL(wxID_ANY, MyFrame::OnURL)
    EVT_RICHTEXT_STYLESHEET_REPLACING(wxID_ANY, MyFrame::OnStyleSheetReplacing)

    EVT_MENU(ID_SET_FONT_SCALE, MyFrame::OnSetFontScale)
    EVT_MENU(ID_SET_DIMENSION_SCALE, MyFrame::OnSetDimensionScale)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_HELP
    wxHelpProvider::Set(new wxSimpleHelpProvider);
#endif

    m_styleSheet = new wxRichTextStyleSheet;
#if wxUSE_PRINTING_ARCHITECTURE
    m_printing = new wxRichTextPrinting("Test Document");

    m_printing->SetFooterText("@TITLE@", wxRICHTEXT_PAGE_ALL, wxRICHTEXT_PAGE_CENTRE);
    m_printing->SetFooterText("Page @PAGENUM@", wxRICHTEXT_PAGE_ALL, wxRICHTEXT_PAGE_RIGHT);
#endif

    CreateStyles();

    MyRichTextCtrl::SetEnhancedDrawingHandler();

    // Add extra handlers (plain text is automatically added)
    wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    wxRichTextBuffer::AddHandler(new wxRichTextHTMLHandler);

    // Add field types

    wxRichTextBuffer::AddFieldType(new wxRichTextFieldTypePropertiesTest("rectangle", "RECTANGLE", wxRichTextFieldTypeStandard::wxRICHTEXT_FIELD_STYLE_RECTANGLE));

    wxRichTextFieldTypeStandard* s1 = new wxRichTextFieldTypeStandard("begin-section", "SECTION", wxRichTextFieldTypeStandard::wxRICHTEXT_FIELD_STYLE_START_TAG);
    s1->SetBackgroundColour(*wxBLUE);

    wxRichTextFieldTypeStandard* s2 = new wxRichTextFieldTypeStandard("end-section", "SECTION", wxRichTextFieldTypeStandard::wxRICHTEXT_FIELD_STYLE_END_TAG);
    s2->SetBackgroundColour(*wxBLUE);

    wxRichTextFieldTypeStandard* s3 = new wxRichTextFieldTypeStandard("bitmap", wxBitmap(paste_xpm), wxRichTextFieldTypeStandard::wxRICHTEXT_FIELD_STYLE_NO_BORDER);

    wxRichTextBuffer::AddFieldType(s1);
    wxRichTextBuffer::AddFieldType(s2);
    wxRichTextBuffer::AddFieldType(s3);

    wxRichTextFieldTypeCompositeTest* s4 = new wxRichTextFieldTypeCompositeTest("composite", "This is a field value");
    wxRichTextBuffer::AddFieldType(s4);

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

#if wxUSE_FILESYSTEM
    wxFileSystem::AddHandler( new wxMemoryFSHandler );
#endif

    // create the main application window
    wxSize size = wxGetDisplaySize();
    size.Scale(0.75, 0.75);
    MyFrame *frame = new MyFrame("wxRichTextCtrl Sample", wxID_ANY, wxDefaultPosition, size);

#if wxUSE_PRINTING_ARCHITECTURE
    m_printing->SetParentWindow(frame);
#endif

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
#if wxUSE_PRINTING_ARCHITECTURE
    delete m_printing;
#endif
    delete m_styleSheet;

    return 0;
}

void MyApp::CreateStyles()
{
    // Paragraph styles

    wxFont romanFont(wxFontInfo(12).Family(wxFONTFAMILY_ROMAN));
    wxFont swissFont(wxFontInfo(12).Family(wxFONTFAMILY_SWISS));

    wxRichTextParagraphStyleDefinition* normalPara = new wxRichTextParagraphStyleDefinition("Normal");
    wxRichTextAttr normalAttr;
    normalAttr.SetFontFaceName(romanFont.GetFaceName());
    normalAttr.SetFontSize(12);
    // Let's set all attributes for this style
    normalAttr.SetFlags(wxTEXT_ATTR_FONT | wxTEXT_ATTR_BACKGROUND_COLOUR | wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|
                            wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|
                            wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER);
    normalPara->SetStyle(normalAttr);

    m_styleSheet->AddParagraphStyle(normalPara);

    wxRichTextParagraphStyleDefinition* indentedPara = new wxRichTextParagraphStyleDefinition("Indented");
    wxRichTextAttr indentedAttr;
    indentedAttr.SetFontFaceName(romanFont.GetFaceName());
    indentedAttr.SetFontSize(12);
    indentedAttr.SetLeftIndent(100, 0);
    // We only want to affect indentation
    indentedAttr.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT);
    indentedPara->SetStyle(indentedAttr);

    m_styleSheet->AddParagraphStyle(indentedPara);

    wxRichTextParagraphStyleDefinition* indentedPara2 = new wxRichTextParagraphStyleDefinition("Red Bold Indented");
    wxRichTextAttr indentedAttr2;
    indentedAttr2.SetFontFaceName(romanFont.GetFaceName());
    indentedAttr2.SetFontSize(12);
    indentedAttr2.SetFontWeight(wxFONTWEIGHT_BOLD);
    indentedAttr2.SetTextColour(*wxRED);
    indentedAttr2.SetFontSize(12);
    indentedAttr2.SetLeftIndent(100, 0);
    // We want to affect indentation, font and text colour
    indentedAttr2.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_FONT|wxTEXT_ATTR_TEXT_COLOUR);
    indentedPara2->SetStyle(indentedAttr2);

    m_styleSheet->AddParagraphStyle(indentedPara2);

    wxRichTextParagraphStyleDefinition* flIndentedPara = new wxRichTextParagraphStyleDefinition("First Line Indented");
    wxRichTextAttr flIndentedAttr;
    flIndentedAttr.SetFontFaceName(swissFont.GetFaceName());
    flIndentedAttr.SetFontSize(12);
    flIndentedAttr.SetLeftIndent(100, -100);
    // We only want to affect indentation
    flIndentedAttr.SetFlags(wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT);
    flIndentedPara->SetStyle(flIndentedAttr);

    m_styleSheet->AddParagraphStyle(flIndentedPara);

    // Character styles

    wxRichTextCharacterStyleDefinition* boldDef = new wxRichTextCharacterStyleDefinition("Bold");
    wxRichTextAttr boldAttr;
    boldAttr.SetFontFaceName(romanFont.GetFaceName());
    boldAttr.SetFontSize(12);
    boldAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    // We only want to affect boldness
    boldAttr.SetFlags(wxTEXT_ATTR_FONT_WEIGHT);
    boldDef->SetStyle(boldAttr);

    m_styleSheet->AddCharacterStyle(boldDef);

    wxRichTextCharacterStyleDefinition* italicDef = new wxRichTextCharacterStyleDefinition("Italic");
    wxRichTextAttr italicAttr;
    italicAttr.SetFontFaceName(romanFont.GetFaceName());
    italicAttr.SetFontSize(12);
    italicAttr.SetFontStyle(wxFONTSTYLE_ITALIC);
    // We only want to affect italics
    italicAttr.SetFlags(wxTEXT_ATTR_FONT_ITALIC);
    italicDef->SetStyle(italicAttr);

    m_styleSheet->AddCharacterStyle(italicDef);

    wxRichTextCharacterStyleDefinition* redDef = new wxRichTextCharacterStyleDefinition("Red Bold");
    wxRichTextAttr redAttr;
    redAttr.SetFontFaceName(romanFont.GetFaceName());
    redAttr.SetFontSize(12);
    redAttr.SetFontWeight(wxFONTWEIGHT_BOLD);
    redAttr.SetTextColour(*wxRED);
    // We only want to affect colour, weight and face
    redAttr.SetFlags(wxTEXT_ATTR_FONT_FACE|wxTEXT_ATTR_FONT_WEIGHT|wxTEXT_ATTR_TEXT_COLOUR);
    redDef->SetStyle(redAttr);

    m_styleSheet->AddCharacterStyle(redDef);

    wxRichTextListStyleDefinition* bulletList = new wxRichTextListStyleDefinition("Bullet List 1");
    int i;
    for (i = 0; i < 10; i++)
    {
        wxString bulletText;
        if (i == 0)
            bulletText = "standard/circle";
        else if (i == 1)
            bulletText = "standard/square";
        else if (i == 2)
            bulletText = "standard/circle";
        else if (i == 3)
            bulletText = "standard/square";
        else
            bulletText = "standard/circle";

        bulletList->SetAttributes(i, (i+1)*60, 60, wxTEXT_ATTR_BULLET_STYLE_STANDARD, bulletText);
    }

    m_styleSheet->AddListStyle(bulletList);

    wxRichTextListStyleDefinition* numberedList = new wxRichTextListStyleDefinition("Numbered List 1");
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

        numberStyle |= wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT;

        numberedList->SetAttributes(i, (i+1)*60, 60, numberStyle);
    }

    m_styleSheet->AddListStyle(numberedList);

    wxRichTextListStyleDefinition* outlineList = new wxRichTextListStyleDefinition("Outline List 1");
    for (i = 0; i < 10; i++)
    {
        long numberStyle;
        if (i < 4)
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_OUTLINE|wxTEXT_ATTR_BULLET_STYLE_PERIOD;
        else
            numberStyle = wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD;

        outlineList->SetAttributes(i, (i+1)*120, 120, numberStyle);
    }

    m_styleSheet->AddListStyle(outlineList);
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
       : wxFrame(nullptr, id, title, pos, size, style)
{
    m_richTextCtrl = nullptr;

#ifdef __WXMAC__
    SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    // set the frame icon
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O", "Open a file");
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save a file");
    fileMenu->Append(wxID_SAVEAS, "&Save As...\tF12", "Save to a new file");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_RELOAD, "&Reload Text\tF2", "Reload the initial text");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_PAGE_SETUP, "Page Set&up...", "Page setup");
#if wxUSE_PRINTING_ARCHITECTURE
    fileMenu->Append(ID_PRINT, "&Print...\tCtrl+P", "Print");
    fileMenu->Append(ID_PREVIEW, "Print Pre&view", "Print preview");
#endif
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_VIEW_HTML, "&View as HTML", "View HTML");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_Quit, "E&xit\tAlt+X", "Quit this program");

    wxMenu* editMenu = new wxMenu;
    editMenu->Append(wxID_UNDO, _("&Undo\tCtrl+Z"));
    editMenu->Append(wxID_REDO, _("&Redo\tCtrl+Y"));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT, _("Cu&t\tCtrl+X"));
    editMenu->Append(wxID_COPY, _("&Copy\tCtrl+C"));
    editMenu->Append(wxID_PASTE, _("&Paste\tCtrl+V"));

    editMenu->AppendSeparator();
    editMenu->Append(wxID_SELECTALL, _("Select A&ll\tCtrl+A"));
    editMenu->AppendSeparator();
    editMenu->Append(ID_SET_FONT_SCALE, _("Set &Text Scale..."));
    editMenu->Append(ID_SET_DIMENSION_SCALE, _("Set &Dimension Scale..."));

    wxMenu* formatMenu = new wxMenu;
    formatMenu->AppendCheckItem(ID_FORMAT_BOLD, _("&Bold\tCtrl+B"));
    formatMenu->AppendCheckItem(ID_FORMAT_ITALIC, _("&Italic\tCtrl+I"));
    formatMenu->AppendCheckItem(ID_FORMAT_UNDERLINE, _("&Underline\tCtrl+U"));
    formatMenu->AppendSeparator();
    formatMenu->AppendCheckItem(ID_FORMAT_STRIKETHROUGH, _("Stri&kethrough"));
    formatMenu->AppendCheckItem(ID_FORMAT_SUPERSCRIPT, _("Superscrip&t"));
    formatMenu->AppendCheckItem(ID_FORMAT_SUBSCRIPT, _("Subscrip&t"));
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
    formatMenu->Append(ID_FORMAT_IMAGE, _("Image Property"));
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

    wxMenu* tableMenu = new wxMenu;
    tableMenu->Append(ID_TABLE_ADD_COLUMN, _("&Add Column"));
    tableMenu->Append(ID_TABLE_ADD_ROW, _("Add &Row"));
    tableMenu->Append(ID_TABLE_DELETE_COLUMN, _("Delete &Column"));
    tableMenu->Append(ID_TABLE_DELETE_ROW, _("&Delete Row"));

    wxMenu* insertMenu = new wxMenu;
    insertMenu->Append(ID_INSERT_SYMBOL, _("&Symbol...\tCtrl+I"));
    insertMenu->Append(ID_INSERT_URL, _("&URL..."));
    insertMenu->Append(ID_INSERT_IMAGE, _("&Image..."));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    menuBar->Append(formatMenu, "F&ormat");
    menuBar->Append(listsMenu, "&Lists");
    menuBar->Append(tableMenu, "&Tables");
    menuBar->Append(insertMenu, "&Insert");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    // but don't create it on limited screen space (mobile device)
    bool is_pda = wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;

#if wxUSE_STATUSBAR
    if ( !is_pda )
    {
        CreateStatusBar(2);
        SetStatusText("Welcome to wxRichTextCtrl!");
    }
#endif

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    // On Mac, don't create a 'native' wxToolBar because small bitmaps are not supported by native
    // toolbars. On Mac, a non-native, small-bitmap toolbar doesn't show unless it is explicitly
    // managed, hence the use of sizers. In a real application, use larger icons for the main
    // toolbar to avoid the need for this workaround. Or, use the toolbar in a container window
    // as part of a more complex hierarchy, and the toolbar will automatically be non-native.

    wxSystemOptions::SetOption("mac.toolbar.no-native", 1);

    wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                       wxNO_BORDER|wxTB_FLAT|wxTB_NODIVIDER|wxTB_NOALIGN);

    sizer->Add(toolBar, 0, wxEXPAND);

    toolBar->AddTool(wxID_OPEN, wxEmptyString, wxBitmap(open_xpm), _("Open"));
    toolBar->AddTool(wxID_SAVEAS, wxEmptyString, wxBitmap(save_xpm), _("Save"));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, wxEmptyString, wxBitmap(cut_xpm), _("Cut"));
    toolBar->AddTool(wxID_COPY, wxEmptyString, wxBitmap(copy_xpm), _("Copy"));
    toolBar->AddTool(wxID_PASTE, wxEmptyString, wxBitmap(paste_xpm), _("Paste"));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_UNDO, wxEmptyString, wxBitmap(undo_xpm), _("Undo"));
    toolBar->AddTool(wxID_REDO, wxEmptyString, wxBitmap(redo_xpm), _("Redo"));
    toolBar->AddSeparator();
    toolBar->AddCheckTool(ID_FORMAT_BOLD, wxEmptyString, wxBitmap(bold_xpm), wxNullBitmap, _("Bold"));
    toolBar->AddCheckTool(ID_FORMAT_ITALIC, wxEmptyString, wxBitmap(italic_xpm), wxNullBitmap, _("Italic"));
    toolBar->AddCheckTool(ID_FORMAT_UNDERLINE, wxEmptyString, wxBitmap(underline_xpm), wxNullBitmap, _("Underline"));
    toolBar->AddSeparator();
    toolBar->AddCheckTool(ID_FORMAT_ALIGN_LEFT, wxEmptyString, wxBitmap(alignleft_xpm), wxNullBitmap, _("Align Left"));
    toolBar->AddCheckTool(ID_FORMAT_ALIGN_CENTRE, wxEmptyString, wxBitmap(centre_xpm), wxNullBitmap, _("Centre"));
    toolBar->AddCheckTool(ID_FORMAT_ALIGN_RIGHT, wxEmptyString, wxBitmap(alignright_xpm), wxNullBitmap, _("Align Right"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_FORMAT_INDENT_LESS, wxEmptyString, wxBitmap(indentless_xpm), _("Indent Less"));
    toolBar->AddTool(ID_FORMAT_INDENT_MORE, wxEmptyString, wxBitmap(indentmore_xpm), _("Indent More"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_FORMAT_FONT, wxEmptyString, wxBitmap(font_xpm), _("Font"));
    toolBar->AddSeparator();

    wxRichTextStyleComboCtrl* combo = new wxRichTextStyleComboCtrl(toolBar, ID_RICHTEXT_STYLE_COMBO, wxDefaultPosition, wxSize(160, -1), wxCB_READONLY);
    toolBar->AddControl(combo);

    toolBar->Realize();

    wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
    sizer->Add(splitter, 1, wxEXPAND);

    m_richTextCtrl = new MyRichTextCtrl(splitter, ID_RICHTEXT_CTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL/*|wxWANTS_CHARS*/);
    wxASSERT(!m_richTextCtrl->GetBuffer().GetAttributes().HasFontPixelSize());

    wxFont font(wxFontInfo(12).Family(wxFONTFAMILY_ROMAN));

    m_richTextCtrl->SetFont(font);

    wxASSERT(!m_richTextCtrl->GetBuffer().GetAttributes().HasFontPixelSize());

    m_richTextCtrl->SetMargins(10, 10);

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
        int width = GetClientSize().GetWidth() * 4 / 5;
        splitter->SplitVertically(m_richTextCtrl, styleListCtrl, width);
        splitter->SetSashGravity(0.8);
    }

    Layout();

    splitter->UpdateSize();

    styleListCtrl->SetStyleSheet(wxGetApp().GetStyleSheet());
    styleListCtrl->SetRichTextCtrl(m_richTextCtrl);
    styleListCtrl->UpdateStyles();

    WriteInitialText();
}

// Write text
void MyFrame::WriteInitialText()
{
    MyRichTextCtrl& r = *m_richTextCtrl;

    r.SetDefaultStyle(wxRichTextAttr());

    r.Freeze();

    r.BeginSuppressUndo();

    r.BeginParagraphSpacing(0, 20);

    r.BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    r.BeginBold();

    r.BeginFontSize(14);

    wxString lineBreak = (wxChar) 29;

    r.WriteText(wxString("Welcome to wxRichTextCtrl, a wxWidgets control") + lineBreak + "for editing and presenting styled text and images\n");
    r.EndFontSize();

    r.BeginItalic();
    r.WriteText("by Julian Smart");
    r.EndItalic();

    r.EndBold();
    r.Newline();

    r.WriteImage(wxBitmap(zebra_xpm));

    r.Newline();
    r.Newline();

    r.EndAlignment();

#if 0
    r.BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    r.WriteText(wxString("This is a simple test for a floating left image test. The zebra image should be placed at the left side of the current buffer and all the text should flow around it at the right side. This is a simple test for a floating left image test. The zebra image should be placed at the left side of the current buffer and all the text should flow around it at the right side. This is a simple test for a floating left image test. The zebra image should be placed at the left side of the current buffer and all the text should flow around it at the right side."));
    r.Newline();
    r.EndAlignment();
#endif

    r.BeginAlignment(wxTEXT_ALIGNMENT_LEFT);
    wxRichTextAttr imageAttr;
    imageAttr.GetTextBoxAttr().SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_LEFT);
    r.WriteText(wxString("This is a simple test for a floating left image test. The zebra image should be placed at the left side of the current buffer and all the text should flow around it at the right side. This is a simple test for a floating left image test. The zebra image should be placed at the left side of the current buffer and all the text should flow around it at the right side. This is a simple test for a floating left image test. The zebra image should be placed at the left side of the current buffer and all the text should flow around it at the right side."));
    r.WriteImage(wxBitmap(zebra_xpm), wxBITMAP_TYPE_PNG, imageAttr);

    imageAttr.GetTextBoxAttr().GetTop().SetValue(200);
    imageAttr.GetTextBoxAttr().GetTop().SetUnits(wxTEXT_ATTR_UNITS_PIXELS);
    imageAttr.GetTextBoxAttr().SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
    r.WriteImage(wxBitmap(zebra_xpm), wxBITMAP_TYPE_PNG, imageAttr);
    r.WriteText(wxString("This is a simple test for a floating right image test. The zebra image should be placed at the right side of the current buffer and all the text should flow around it at the left side. This is a simple test for a floating left image test. The zebra image should be placed at the right side of the current buffer and all the text should flow around it at the left side. This is a simple test for a floating left image test. The zebra image should be placed at the right side of the current buffer and all the text should flow around it at the left side."));
    r.EndAlignment();
    r.Newline();

    r.WriteText("What can you do with this thing? ");

    r.WriteImage(wxBitmap(smiley_xpm));
    r.WriteText(" Well, you can change text ");

    r.BeginTextColour(*wxRED);
    r.WriteText("colour, like this red bit.");
    r.EndTextColour();

    wxRichTextAttr backgroundColourAttr;
    backgroundColourAttr.SetBackgroundColour(*wxGREEN);
    backgroundColourAttr.SetTextColour(*wxBLUE);
    r.BeginStyle(backgroundColourAttr);
    r.WriteText(" And this blue on green bit.");
    r.EndStyle();

    r.WriteText(" Naturally you can make things ");
    r.BeginBold();
    r.WriteText("bold ");
    r.EndBold();
    r.BeginItalic();
    r.WriteText("or italic ");
    r.EndItalic();
    r.BeginUnderline();
    r.WriteText("or underlined.");
    r.EndUnderline();

    r.BeginFontSize(14);
    r.WriteText(" Different font sizes on the same line is allowed, too.");
    r.EndFontSize();

    r.WriteText(" Next we'll show an indented paragraph.");

    r.Newline();

    r.BeginLeftIndent(60);
    r.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.");
    r.Newline();

    r.EndLeftIndent();

    r.WriteText("Next, we'll show a first-line indent, achieved using BeginLeftIndent(100, -40).");

    r.Newline();

    r.BeginLeftIndent(100, -40);

    r.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.");
    r.Newline();

    r.EndLeftIndent();

    r.WriteText("Numbered bullets are possible, again using subindents:");
    r.Newline();

    r.BeginNumberedBullet(1, 100, 60);
    r.WriteText("This is my first item. Note that wxRichTextCtrl can apply numbering and bullets automatically based on list styles, but this list is formatted explicitly by setting indents.");
    r.Newline();
    r.EndNumberedBullet();

    r.BeginNumberedBullet(2, 100, 60);
    r.WriteText("This is my second item.");
    r.Newline();
    r.EndNumberedBullet();

    r.WriteText("The following paragraph is right-indented:");
    r.Newline();

    r.BeginRightIndent(200);

    r.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.");
    r.Newline();

    r.EndRightIndent();

    r.WriteText("The following paragraph is right-aligned with 1.5 line spacing:");
    r.Newline();

    r.BeginAlignment(wxTEXT_ALIGNMENT_RIGHT);
    r.BeginLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);
    r.WriteText("It was in January, the most down-trodden month of an Edinburgh winter. An attractive woman came into the cafe, which is nothing remarkable.");
    r.Newline();
    r.EndLineSpacing();
    r.EndAlignment();

    wxArrayInt tabs;
    tabs.Add(400);
    tabs.Add(600);
    tabs.Add(800);
    tabs.Add(1000);
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_TABS);
    attr.SetTabs(tabs);
    r.SetDefaultStyle(attr);

    r.WriteText("This line contains tabs:\tFirst tab\tSecond tab\tThird tab");
    r.Newline();

    r.WriteText("Other notable features of wxRichTextCtrl include:");
    r.Newline();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("Compatibility with wxTextCtrl API");
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("Easy stack-based BeginXXX()...EndXXX() style setting in addition to SetStyle()");
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("XML loading and saving");
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("Undo/Redo, with batching option and Undo suppressing");
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("Clipboard copy and paste");
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("wxRichTextStyleSheet with named character and paragraph styles, and control for applying named styles");
    r.Newline();
    r.EndSymbolBullet();

    r.BeginSymbolBullet('*', 100, 60);
    r.WriteText("A design that can easily be extended to other content types, ultimately with text boxes, tables, controls, and so on");
    r.Newline();
    r.EndSymbolBullet();

    // Make a style suitable for showing a URL
    wxRichTextAttr urlStyle;
    urlStyle.SetTextColour(*wxBLUE);
    urlStyle.SetFontUnderlined(true);

    r.WriteText("wxRichTextCtrl can also display URLs, such as this one: ");
    r.BeginStyle(urlStyle);
    r.BeginURL("http://www.wxwidgets.org");
    r.WriteText("The wxWidgets Web Site");
    r.EndURL();
    r.EndStyle();
    r.WriteText(". Click on the URL to generate an event.");

    r.Newline();

    r.WriteText("Note: this sample content was generated programmatically from within the MyFrame constructor in the demo. The images were loaded from inline XPMs. Enjoy wxRichTextCtrl!\n");

    r.EndParagraphSpacing();

#if 1
    {
        // Add a text box

        r.Newline();

        wxRichTextAttr attr1;
        attr1.GetTextBoxAttr().GetMargins().GetLeft().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetMargins().GetTop().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetMargins().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetMargins().GetBottom().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);

        attr1.GetTextBoxAttr().GetBorder().SetColour(*wxBLACK);
        attr1.GetTextBoxAttr().GetBorder().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);

        wxRichTextBox* textBox = r.WriteTextBox(attr1);
        r.SetFocusObject(textBox);

        r.WriteText("This is a text box. Just testing! Once more unto the breach, dear friends, once more...");

        r.SetFocusObject(nullptr); // Set the focus back to the main buffer
        r.SetInsertionPointEnd();
    }
#endif

#if 1
    {
        // Add a table

        r.Newline();

        wxRichTextAttr attr1;
        attr1.GetTextBoxAttr().GetMargins().GetLeft().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetMargins().GetTop().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetMargins().GetRight().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetMargins().GetBottom().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetPadding() = attr.GetTextBoxAttr().GetMargins();

        attr1.GetTextBoxAttr().GetBorder().SetColour(*wxBLACK);
        attr1.GetTextBoxAttr().GetBorder().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        attr1.GetTextBoxAttr().GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);

        wxRichTextAttr cellAttr = attr1;
        cellAttr.GetTextBoxAttr().GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
        cellAttr.GetTextBoxAttr().GetHeight().SetValue(150, wxTEXT_ATTR_UNITS_PIXELS);

        wxRichTextTable* table = r.WriteTable(6, 4, attr1, cellAttr);

        int i, j;
        for (j = 0; j < table->GetRowCount(); j++)
        {
            for (i = 0; i < table->GetColumnCount(); i++)
            {
                wxString msg = wxString::Format("This is cell %d, %d", (j+1), (i+1));
                r.SetFocusObject(table->GetCell(j, i));
                r.WriteText(msg);
            }
        }

        // Demonstrate colspan and rowspan
        wxRichTextCell* cell = table->GetCell(1, 0);
        cell->SetColSpan(2);
        r.SetFocusObject(cell);
        cell->Clear();
        r.WriteText("This cell spans 2 columns");

        cell = table->GetCell(1, 3);
        cell->SetRowSpan(2);
        r.SetFocusObject(cell);
        cell->Clear();
        r.WriteText("This cell spans 2 rows");

        cell = table->GetCell(2, 1);
        cell->SetColSpan(2);
        cell->SetRowSpan(3);
        r.SetFocusObject(cell);
        cell->Clear();
        r.WriteText("This cell spans 2 columns and 3 rows");

        r.SetFocusObject(nullptr); // Set the focus back to the main buffer
        r.SetInsertionPointEnd();
    }
#endif

    r.Newline(); r.Newline();

    wxRichTextProperties properties;
    r.WriteText("This is a rectangle field: ");
    r.WriteField("rectangle", properties);
    r.WriteText(" and a begin section field: ");
    r.WriteField("begin-section", properties);
    r.WriteText("This is text between the two tags.");
    r.WriteField("end-section", properties);
    r.WriteText(" Now a bitmap. ");
    r.WriteField("bitmap", properties);
    r.WriteText(" Before we go, here's a composite field: ***");
    wxRichTextField* field = r.WriteField("composite", properties);
    field->UpdateField(& r.GetBuffer()); // Creates the composite value (sort of a text box)
    r.WriteText("*** End of composite field.");

    r.Newline();
    r.EndSuppressUndo();

    // Add some locked content first - needs Undo to be enabled
    {
        r.BeginLock();
        r.WriteText(wxString("This is a locked object."));
        r.EndLock();

        r.WriteText(wxString(" This is unlocked text. "));

        r.BeginLock();
        r.WriteText(wxString("More locked content."));
        r.EndLock();
        r.Newline();

        // Flush the Undo buffer
        r.GetCommandProcessor()->ClearCommands();
    }

    r.Thaw();
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
    msg.Printf( "This is a demo for wxRichTextCtrl, a control for editing styled text.\n(c) Julian Smart, 2005");
    wxMessageBox(msg, "About wxRichTextCtrl Sample", wxOK | wxICON_INFORMATION, this);
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
            if (!focusWin)
                focusWin = m_richTextCtrl;
            if (focusWin && focusWin->GetEventHandler()->ProcessEvent(event))
            {
                //s_command = nullptr;
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
        filter += "|";
    filter += "All files (*.*)|*.*";

    wxFileDialog dialog(this,
        _("Choose a filename"),
        path,
        filename,
        filter,
        wxFD_OPEN);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString path1 = dialog.GetPath();

        if (!path1.empty())
        {
            int filterIndex = dialog.GetFilterIndex();
            int fileType = (filterIndex < (int) fileTypes.GetCount())
                           ? fileTypes[filterIndex]
                           : wxRICHTEXT_TYPE_TEXT;
            m_richTextCtrl->LoadFile(path1, fileType);
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
        wxString path1 = dialog.GetPath();

        if (!path1.empty())
        {
            wxBusyCursor busy;
            wxStopWatch stopwatch;

            m_richTextCtrl->SaveFile(path1);

            long t = stopwatch.Time();
            wxLogDebug("Saving took %ldms", t);
            wxMessageBox(wxString::Format("Saving took %ldms", t));
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

void MyFrame::OnStrikethrough(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyTextEffectToSelection(wxTEXT_ATTR_EFFECT_STRIKETHROUGH);
}

void MyFrame::OnSuperscript(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyTextEffectToSelection(wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
}

void MyFrame::OnSubscript(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->ApplyTextEffectToSelection(wxTEXT_ATTR_EFFECT_SUBSCRIPT);
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

void MyFrame::OnUpdateStrikethrough(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->DoesSelectionHaveTextEffectFlag(wxTEXT_ATTR_EFFECT_STRIKETHROUGH));
}

void MyFrame::OnUpdateSuperscript(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->DoesSelectionHaveTextEffectFlag(wxTEXT_ATTR_EFFECT_SUPERSCRIPT));
}

void MyFrame::OnUpdateSubscript(wxUpdateUIEvent& event)
{
    event.Check(m_richTextCtrl->DoesSelectionHaveTextEffectFlag(wxTEXT_ATTR_EFFECT_SUBSCRIPT));
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
    formatDlg.SetOptions(wxRichTextFormattingDialog::Option_AllowPixelFontSize);
    formatDlg.GetStyle(m_richTextCtrl, range);

    if (formatDlg.ShowModal() == wxID_OK)
    {
        formatDlg.ApplyStyle(m_richTextCtrl, range, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_OPTIMIZE|wxRICHTEXT_SETSTYLE_CHARACTERS_ONLY);
    }
}

void MyFrame::OnImage(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextRange range;
    wxASSERT(m_richTextCtrl->HasSelection());

    range = m_richTextCtrl->GetSelectionRange();
    wxASSERT(range.ToInternal().GetLength() == 1);

    wxRichTextImage* image = wxDynamicCast(m_richTextCtrl->GetFocusObject()->GetLeafObjectAtPosition(range.GetStart()), wxRichTextImage);
    if (image)
    {
        wxRichTextObjectPropertiesDialog imageDlg(image, this);

        if (imageDlg.ShowModal() == wxID_OK)
        {
            imageDlg.ApplyStyle(m_richTextCtrl);
        }
    }
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

void MyFrame::OnUpdateImage(wxUpdateUIEvent& event)
{
    wxRichTextRange range;

    range = m_richTextCtrl->GetSelectionRange();
    if (range.ToInternal().GetLength() == 1)
    {
        wxRichTextObject *obj;
        obj = m_richTextCtrl->GetFocusObject()->GetLeafObjectAtPosition(range.GetStart());
        if (obj && obj->IsKindOf(CLASSINFO(wxRichTextImage)))
        {
            event.Enable(true);
            return;
        }
    }

    event.Enable(false);
}

void MyFrame::OnIndentMore(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        attr.SetLeftIndent(attr.GetLeftIndent() + 100);

        attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);
        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnIndentLess(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LEFT_INDENT);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        if (attr.GetLeftIndent() > 0)
        {
            attr.SetLeftIndent(wxMax(0, attr.GetLeftIndent() - 100));

            m_richTextCtrl->SetStyle(range, attr);
        }
    }
}

void MyFrame::OnLineSpacingHalf(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
        attr.SetLineSpacing(15);

        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnLineSpacingDouble(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
        attr.SetLineSpacing(20);

        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnLineSpacingSingle(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        attr.SetFlags(wxTEXT_ATTR_LINE_SPACING);
        attr.SetLineSpacing(0); // Can also use 10

        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnParagraphSpacingMore(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        attr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter() + 20);

        attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);
        m_richTextCtrl->SetStyle(range, attr);
    }
}

void MyFrame::OnParagraphSpacingLess(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);

    if (m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr))
    {
        wxRichTextRange range(m_richTextCtrl->GetInsertionPoint(), m_richTextCtrl->GetInsertionPoint());
        if (m_richTextCtrl->HasSelection())
            range = m_richTextCtrl->GetSelectionRange();

        if (attr.GetParagraphSpacingAfter() >= 20)
        {
            attr.SetParagraphSpacingAfter(attr.GetParagraphSpacingAfter() - 20);

            attr.SetFlags(wxTEXT_ATTR_PARA_SPACING_AFTER);
            m_richTextCtrl->SetStyle(range, attr);
        }
    }
}

void MyFrame::OnReload(wxCommandEvent& WXUNUSED(event))
{
    m_richTextCtrl->Clear();
    WriteInitialText();
}

void MyFrame::OnViewHTML(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dialog(this, wxID_ANY, "HTML", wxDefaultPosition, wxSize(500, 400), wxDEFAULT_DIALOG_STYLE);

    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
    dialog.SetSizer(boxSizer);

    wxHtmlWindow* win = new wxHtmlWindow(& dialog, wxID_ANY, wxDefaultPosition, wxSize(500, 400), wxSUNKEN_BORDER);
    boxSizer->Add(win, 1, wxALL, 5);

    wxButton* cancelButton = new wxButton(& dialog, wxID_CANCEL, "&Close");
    boxSizer->Add(cancelButton, 0, wxALL|wxCENTRE, 5);

    wxString text;
    wxStringOutputStream strStream(& text);

    wxRichTextHTMLHandler htmlHandler;
    htmlHandler.SetFlags(wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY);

    wxArrayInt fontSizeMapping;
    fontSizeMapping.Add(7);
    fontSizeMapping.Add(9);
    fontSizeMapping.Add(11);
    fontSizeMapping.Add(12);
    fontSizeMapping.Add(14);
    fontSizeMapping.Add(22);
    fontSizeMapping.Add(100);

    htmlHandler.SetFontSizeMapping(fontSizeMapping);

    if (htmlHandler.SaveFile(& m_richTextCtrl->GetBuffer(), strStream))
    {
        win->SetPage(text);
    }

    boxSizer->Fit(& dialog);

    dialog.ShowModal();

    // Now delete the temporary in-memory images
    htmlHandler.DeleteTemporaryImages();
}

// Demonstrates how you can change the style sheets and have the changes
// reflected in the control content without wiping out character formatting.

void MyFrame::OnSwitchStyleSheets(wxCommandEvent& WXUNUSED(event))
{
    static wxRichTextStyleSheet* gs_AlternateStyleSheet = nullptr;

    wxRichTextStyleListCtrl *styleList = (wxRichTextStyleListCtrl*) FindWindow(ID_RICHTEXT_STYLE_LIST);
    wxRichTextStyleComboCtrl* styleCombo = (wxRichTextStyleComboCtrl*) FindWindow(ID_RICHTEXT_STYLE_COMBO);

    wxRichTextStyleSheet* sheet = m_richTextCtrl->GetStyleSheet();

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

    m_richTextCtrl->SetStyleSheet(sheet);
    m_richTextCtrl->ApplyStyleSheet(sheet); // Makes the control reflect the new style definitions

    styleList->SetStyleSheet(sheet);
    styleList->UpdateStyles();

    styleCombo->SetStyleSheet(sheet);
    styleCombo->UpdateStyles();
}

void MyFrame::OnManageStyles(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextStyleSheet* sheet = m_richTextCtrl->GetStyleSheet();

    int flags = wxRICHTEXT_ORGANISER_CREATE_STYLES|wxRICHTEXT_ORGANISER_EDIT_STYLES;

    wxRichTextStyleOrganiserDialog dlg(flags, sheet, nullptr, this, wxID_ANY, _("Style Manager"));
    dlg.ShowModal();
}

void MyFrame::OnInsertSymbol(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_FONT);
    m_richTextCtrl->GetStyle(m_richTextCtrl->GetInsertionPoint(), attr);

    wxString currentFontName;
    if (attr.HasFont() && attr.GetFont().IsOk())
        currentFontName = attr.GetFont().GetFaceName();

    // Don't set the initial font in the dialog (so the user is choosing
    // 'normal text', i.e. the current font) but do tell the dialog
    // what 'normal text' is.

    wxSymbolPickerDialog dlg("*", wxEmptyString, currentFontName, this);

    if (dlg.ShowModal() == wxID_OK)
    {
        if (dlg.HasSelection())
        {
            long insertionPoint = m_richTextCtrl->GetInsertionPoint();

            m_richTextCtrl->WriteText(dlg.GetSymbol());

            if (!dlg.UseNormalFont())
            {
                wxFont font(attr.GetFont());
                font.SetFaceName(dlg.GetFontName());
                attr.SetFont(font);
                m_richTextCtrl->SetStyle(insertionPoint, insertionPoint+1, attr);
            }
        }
    }
}

void MyFrame::OnNumberList(wxCommandEvent& WXUNUSED(event))
{
    if (m_richTextCtrl->HasSelection())
    {
        wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
        m_richTextCtrl->SetListStyle(range, "Numbered List 1", wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RENUMBER);
    }
}

void MyFrame::OnBulletsAndNumbering(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextStyleSheet* sheet = m_richTextCtrl->GetStyleSheet();

    int flags = wxRICHTEXT_ORGANISER_BROWSE_NUMBERING;

    wxRichTextStyleOrganiserDialog dlg(flags, sheet, m_richTextCtrl, this, wxID_ANY, _("Bullets and Numbering"));
    if (dlg.ShowModal() == wxID_OK)
    {
        if (dlg.GetSelectedStyleDefinition())
            dlg.ApplyStyle();
    }
}

void MyFrame::OnItemizeList(wxCommandEvent& WXUNUSED(event))
{
    if (m_richTextCtrl->HasSelection())
    {
        wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
        m_richTextCtrl->SetListStyle(range, "Bullet List 1");
    }
}

void MyFrame::OnRenumberList(wxCommandEvent& WXUNUSED(event))
{
    if (m_richTextCtrl->HasSelection())
    {
        wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
        m_richTextCtrl->NumberList(range, nullptr, wxRICHTEXT_SETSTYLE_WITH_UNDO|wxRICHTEXT_SETSTYLE_RENUMBER);
    }
}

void MyFrame::OnPromoteList(wxCommandEvent& WXUNUSED(event))
{
    if (m_richTextCtrl->HasSelection())
    {
        wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
        m_richTextCtrl->PromoteList(1, range, nullptr);
    }
}

void MyFrame::OnDemoteList(wxCommandEvent& WXUNUSED(event))
{
    if (m_richTextCtrl->HasSelection())
    {
        wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
        m_richTextCtrl->PromoteList(-1, range, nullptr);
    }
}

void MyFrame::OnClearList(wxCommandEvent& WXUNUSED(event))
{
    if (m_richTextCtrl->HasSelection())
    {
        wxRichTextRange range = m_richTextCtrl->GetSelectionRange();
        m_richTextCtrl->ClearListStyle(range);
    }
}

void MyFrame::OnTableAddColumn(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextTable* table = wxDynamicCast(m_richTextCtrl->FindTable(), wxRichTextTable);
    if (table)
    {
        wxRichTextAttr cellAttr = table->GetCell(0, 0)->GetAttributes();
        table->AddColumns(table->GetColumnCount(), 1, cellAttr);
    }
}

void MyFrame::OnTableAddRow(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextTable* table = wxDynamicCast(m_richTextCtrl->FindTable(), wxRichTextTable);
    if (table)
    {
        wxRichTextAttr cellAttr = table->GetCell(0, 0)->GetAttributes();
        table->AddRows(table->GetRowCount(), 1, cellAttr);
    }
}

void MyFrame::OnTableDeleteColumn(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextTable* table = wxDynamicCast(m_richTextCtrl->FindTable(), wxRichTextTable);
    if (table)
    {
        int col = table->GetFocusedCell().GetCol();
        if (col == -1)
        {
            col = table->GetColumnCount() - 1;
        }

        table->DeleteColumns(col, 1);
    }
}

void MyFrame::OnTableDeleteRow(wxCommandEvent& WXUNUSED(event))
{
    wxRichTextTable* table = wxDynamicCast(m_richTextCtrl->FindTable(), wxRichTextTable);
    if (table)
    {
        int row = table->GetFocusedCell().GetRow();
        if (row == -1)
        {
            row = table->GetRowCount() - 1;
        }

        table->DeleteRows(row, 1);
    }
}

void MyFrame::OnTableFocusedUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_richTextCtrl->FindTable() != nullptr);
}

void MyFrame::OnTableHasCellsUpdateUI(wxUpdateUIEvent& event)
{
    bool enable(false);
    wxRichTextTable* table = wxDynamicCast(m_richTextCtrl->FindTable(), wxRichTextTable);
    if (table)
    {
        if (event.GetId() == ID_TABLE_DELETE_COLUMN)
        {
            enable = table->GetColumnCount() > 1;
        }
        else
        {
            enable = table->GetRowCount() > 1;
        }
    }

    event.Enable(enable);
}

void MyFrame::OnInsertURL(wxCommandEvent& WXUNUSED(event))
{
    wxString url = wxGetTextFromUser(_("URL:"), _("Insert URL"));
    if (!url.IsEmpty())
    {
        // Make a style suitable for showing a URL
        wxRichTextAttr urlStyle;
        urlStyle.SetTextColour(*wxBLUE);
        urlStyle.SetFontUnderlined(true);

        m_richTextCtrl->BeginStyle(urlStyle);
        m_richTextCtrl->BeginURL(url);
        m_richTextCtrl->WriteText(url);
        m_richTextCtrl->EndURL();
        m_richTextCtrl->EndStyle();
    }
}

void MyFrame::OnInsertImage(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dialog(this, _("Choose an image"), "", "", "BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png|JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg");
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString path = dialog.GetPath();
        wxImage image;
        if (image.LoadFile(path) && image.GetType() != wxBITMAP_TYPE_INVALID)
            m_richTextCtrl->WriteImage(path, image.GetType());
    }
}

void MyFrame::OnURL(wxTextUrlEvent& event)
{
    wxMessageBox(event.GetString());
}

// Veto style sheet replace events when loading from XML, since we want
// to keep the original style sheet.
void MyFrame::OnStyleSheetReplacing(wxRichTextEvent& event)
{
    event.Veto();
}

#if wxUSE_PRINTING_ARCHITECTURE
void MyFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GetPrinting()->PrintBuffer(m_richTextCtrl->GetBuffer());
}

void MyFrame::OnPreview(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GetPrinting()->PreviewBuffer(m_richTextCtrl->GetBuffer());
}
#endif

void MyFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dialog(this, wxID_ANY, "Testing", wxPoint(10, 10), wxSize(400, 300), wxDEFAULT_DIALOG_STYLE);

    wxNotebook* nb = new wxNotebook(& dialog, wxID_ANY, wxPoint(5, 5), wxSize(300, 250));
    wxPanel* panel = new wxPanel(nb, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    wxPanel* panel2 = new wxPanel(nb, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    new wxRichTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(5, 5), wxSize(200, 150), wxVSCROLL|wxTE_READONLY);
    nb->AddPage(panel, "Page 1");

    new wxRichTextCtrl(panel2, wxID_ANY, wxEmptyString, wxPoint(5, 5), wxSize(200, 150), wxVSCROLL|wxTE_READONLY);
    nb->AddPage(panel2, "Page 2");

    new wxButton(& dialog, wxID_OK, "OK", wxPoint(5, 180));

    dialog.ShowModal();

//    wxGetApp().GetPrinting()->PageSetup();
}

void MyFrame::OnSetFontScale(wxCommandEvent& WXUNUSED(event))
{
    wxString value = wxString::Format("%g", m_richTextCtrl->GetFontScale());
    wxString text = wxGetTextFromUser("Enter a text scale factor:", "Text Scale Factor", value, wxGetTopLevelParent(this));
    if (!text.IsEmpty() && value != text)
    {
        double scale = 1.0;
        wxSscanf(text, "%lf", & scale);
        m_richTextCtrl->SetFontScale(scale, true);
    }
}

void MyFrame::OnSetDimensionScale(wxCommandEvent& WXUNUSED(event))
{
    wxString value = wxString::Format("%g", m_richTextCtrl->GetDimensionScale());
    wxString text = wxGetTextFromUser("Enter a dimension scale factor:", "Dimension Scale Factor", value, wxGetTopLevelParent(this));
    if (!text.IsEmpty() && value != text)
    {
        double scale = 1.0;
        wxSscanf(text, "%lf", & scale);
        m_richTextCtrl->SetDimensionScale(scale, true);
    }
}

void MyRichTextCtrl::PrepareContent(wxRichTextParagraphLayoutBox& container)
{
    if (IsLocked())
    {
        // Lock all content that's about to be added to the control
        wxRichTextObjectList::compatibility_iterator node = container.GetChildren().GetFirst();
        while (node)
        {
            wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
            if (para)
            {
                wxRichTextObjectList::compatibility_iterator childNode = para->GetChildren().GetFirst();
                while (childNode)
                {
                    wxRichTextObject* obj = childNode->GetData();
                    obj->GetProperties().SetProperty("Lock", m_lockId);

                    childNode = childNode->GetNext();
                }
            }
            node = node->GetNext();
        }
    }
}

bool MyRichTextCtrl::CanDeleteRange(wxRichTextParagraphLayoutBox& container, const wxRichTextRange& range) const
{
    long i;
    for (i = range.GetStart(); i < range.GetEnd(); i++)
    {
        wxRichTextObject* obj = container.GetLeafObjectAtPosition(i);
        if (obj && obj->GetProperties().HasProperty("Lock"))
        {
            return false;
        }
    }
    return true;
}

bool MyRichTextCtrl::CanInsertContent(wxRichTextParagraphLayoutBox& container, long pos) const
{
    wxRichTextObject* child1 = container.GetLeafObjectAtPosition(pos);
    wxRichTextObject* child2 = container.GetLeafObjectAtPosition(pos-1);

    long lock1 = -1, lock2 = -1;

    if (child1 && child1->GetProperties().HasProperty("Lock"))
        lock1 = child1->GetProperties().GetPropertyLong("Lock");
    if (child2 && child2->GetProperties().HasProperty("Lock"))
        lock2 = child2->GetProperties().GetPropertyLong("Lock");

    if (lock1 != -1 && lock1 == lock2)
        return false;

    // Don't allow insertion before a locked object if it's at the beginning of the buffer.
    if (pos == 0 && lock1 != -1)
        return false;

    return true;
}


class wxRichTextEnhancedDrawingHandler: public wxRichTextDrawingHandler
{
public:
    wxRichTextEnhancedDrawingHandler()
    {
        SetName("enhanceddrawing");
        m_lockBackgroundColour = wxColour(220, 220, 220);
    }

    /**
        Returns @true if this object has virtual attributes that we can provide.
    */
    virtual bool HasVirtualAttributes(wxRichTextObject* obj) const override;

    /**
        Provides virtual attributes that we can provide.
    */
    virtual bool GetVirtualAttributes(wxRichTextAttr& attr, wxRichTextObject* obj) const override;

    /**
        Gets the count for mixed virtual attributes for individual positions within the object.
        For example, individual characters within a text object may require special highlighting.
    */
    virtual int GetVirtualSubobjectAttributesCount(wxRichTextObject* WXUNUSED(obj)) const override { return 0; }

    /**
        Gets the mixed virtual attributes for individual positions within the object.
        For example, individual characters within a text object may require special highlighting.
        Returns the number of virtual attributes found.
    */
    virtual int GetVirtualSubobjectAttributes(wxRichTextObject* WXUNUSED(obj), wxArrayInt& WXUNUSED(positions), wxRichTextAttrArray& WXUNUSED(attributes)) const override  { return 0; }

    /**
        Do we have virtual text for this object? Virtual text allows an application
        to replace characters in an object for editing and display purposes, for example
        for highlighting special characters.
    */
    virtual bool HasVirtualText(const wxRichTextPlainText* WXUNUSED(obj)) const override { return false; }

    /**
        Gets the virtual text for this object.
    */
    virtual bool GetVirtualText(const wxRichTextPlainText* WXUNUSED(obj), wxString& WXUNUSED(text)) const override { return false; }

    wxColour    m_lockBackgroundColour;
};

bool wxRichTextEnhancedDrawingHandler::HasVirtualAttributes(wxRichTextObject* obj) const
{
    return obj->GetProperties().HasProperty("Lock");
}

bool wxRichTextEnhancedDrawingHandler::GetVirtualAttributes(wxRichTextAttr& attr, wxRichTextObject* obj) const
{
    if (obj->GetProperties().HasProperty("Lock"))
    {
        attr.SetBackgroundColour(m_lockBackgroundColour);
        return true;
    }
    return false;
}

void MyRichTextCtrl::SetEnhancedDrawingHandler()
{
    wxRichTextBuffer::AddDrawingHandler(new wxRichTextEnhancedDrawingHandler);
}

wxRichTextObject* MyRichTextCtrl::FindCurrentPosition() const
{
    long position = -1;

    if (HasSelection())  // First see if there's a selection
    {
        wxRichTextRange range = GetSelectionRange();
        if (range.ToInternal().GetLength() == 1)
        {
            position = range.GetStart();
        }
    }
    if (position == -1)  // Failing that, near cursor
    {
        position = GetAdjustedCaretPosition(GetCaretPosition());
    }


    wxRichTextObject* obj = GetFocusObject()->GetLeafObjectAtPosition(position);

    return obj;
}

wxRichTextTable* MyRichTextCtrl::FindTable() const
{
    wxRichTextObject* obj = FindCurrentPosition();

    // It could be a table or a cell (or neither)
    wxRichTextTable* table = wxDynamicCast(obj, wxRichTextTable);
    if (table)
    {
        return table;
    }

    while (obj)
    {
        obj = obj->GetParent();
        wxRichTextTable* table1 = wxDynamicCast(obj, wxRichTextTable);
        if (table1)
        {
            return table1;
        }
    }

    return nullptr;
}
