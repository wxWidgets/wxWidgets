/////////////////////////////////////////////////////////////////////////////
// Name:        html.i
// Purpose:     SWIG definitions of html classes
//
// Author:      Robin Dunn
//
// Created:     25-nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module html

%{
#include "helpers.h"
#include <wx/html/htmlwin.h>
#include <wx/html/htmprint.h>
#include <wx/image.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>

#include "printfw.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i
%extern controls2.i
%extern printfw.i

%extern utils.i

//---------------------------------------------------------------------------

%{
#ifdef __WXMSW__
wxString wxPyEmptyStr("");
wxPoint wxPyDefaultPosition(wxDefaultPosition);
wxSize wxPyDefaultSize(wxDefaultSize);
#endif
%}

%pragma(python) code = "import wx"


//---------------------------------------------------------------------------

enum {
    wxHTML_ALIGN_LEFT,
    wxHTML_ALIGN_CENTER,
    wxHTML_ALIGN_RIGHT,
    wxHTML_ALIGN_BOTTOM,
    wxHTML_ALIGN_TOP,

    wxHTML_CLR_FOREGROUND,
    wxHTML_CLR_BACKGROUND,

    wxHTML_UNITS_PIXELS,
    wxHTML_UNITS_PERCENT,

    wxHTML_INDENT_LEFT,
    wxHTML_INDENT_RIGHT,
    wxHTML_INDENT_TOP,
    wxHTML_INDENT_BOTTOM,

    wxHTML_INDENT_HORIZONTAL,
    wxHTML_INDENT_VERTICAL,
    wxHTML_INDENT_ALL,

    wxHTML_COND_ISANCHOR,
    wxHTML_COND_ISIMAGEMAP,
    wxHTML_COND_USER,
};


//---------------------------------------------------------------------------

class wxHtmlLinkInfo {
public:
    wxHtmlLinkInfo(const wxString& href, const wxString& target = wxEmptyString);
    wxString GetHref();
    wxString GetTarget();
#ifdef POST_2_1_12
    wxMouseEvent* GetEvent();
    wxHtmlCell* GetHtmlCell();
#endif
};

//---------------------------------------------------------------------------

class wxHtmlTag {
public:
    // Never need to create a new tag from Python...
    //wxHtmlTag(const wxString& source, int pos, int end_pos, wxHtmlTagsCache* cache);

    wxString GetName();
    bool HasParam(const wxString& par);
    wxString GetParam(const wxString& par, int with_commas = FALSE);

    // Can't do this one as-is, but GetParam should be enough...
    //int ScanParam(const wxString& par, const char *format, void* param);

    wxString GetAllParams();
    bool IsEnding();
    bool HasEnding();
    int GetBeginPos();
    int GetEndPos1();
    int GetEndPos2();
};


//---------------------------------------------------------------------------

class wxHtmlParser {
public:
    // wxHtmlParser();  This is an abstract base class...

    void SetFS(wxFileSystem *fs);
    wxFileSystem* GetFS();
    wxObject* Parse(const wxString& source);
    void InitParser(const wxString& source);
    void DoneParser();
    void DoParsing(int begin_pos, int end_pos);
    // wxObject* GetProduct();
    void AddTagHandler(wxHtmlTagHandler *handler);
    wxString* GetSource();
    void PushTagHandler(wxHtmlTagHandler* handler, wxString tags);
    void PopTagHandler();


    // void AddText(const char* txt) = 0;
    // void AddTag(const wxHtmlTag& tag);
};


//---------------------------------------------------------------------------

class wxHtmlWinParser : public wxHtmlParser {
public:
    wxHtmlWinParser(wxWindow *wnd);

    void SetDC(wxDC *dc);
    wxDC* GetDC();
    int GetCharHeight();
    int GetCharWidth();
    wxWindow* GetWindow();
    void SetFonts(wxString normal_face, wxString fixed_face, int *LIST);

    wxHtmlContainerCell* GetContainer();
    wxHtmlContainerCell* OpenContainer();
    wxHtmlContainerCell *SetContainer(wxHtmlContainerCell *c);
    wxHtmlContainerCell* CloseContainer();

    int GetFontSize();
    void SetFontSize(int s);
    int GetFontBold();
    void SetFontBold(int x);
    int GetFontItalic();
    void SetFontItalic(int x);
    int GetFontUnderlined();
    void SetFontUnderlined(int x);
    int GetFontFixed();
    void SetFontFixed(int x);
    int GetAlign();
    void SetAlign(int a);
    const wxColour& GetLinkColor();
    void SetLinkColor(const wxColour& clr);
    const wxColour& GetActualColor();
    void SetActualColor(const wxColour& clr);
    void SetLink(const wxString& link);
    wxFont* CreateCurrentFont();
    wxHtmlLinkInfo GetLink();

};



//---------------------------------------------------------------------------

%{
class wxPyHtmlTagHandler : public wxHtmlTagHandler {
public:
    wxPyHtmlTagHandler() : wxHtmlTagHandler() {};

    wxHtmlParser* GetParser() { return m_Parser; }
    void ParseInner(const wxHtmlTag& tag) { wxHtmlTagHandler::ParseInner(tag); }

    DEC_PYCALLBACK_STRING__pure(GetSupportedTags);
    DEC_PYCALLBACK_BOOL_TAG_pure(HandleTag);

    PYPRIVATE;
};

IMP_PYCALLBACK_STRING__pure(wxPyHtmlTagHandler, wxHtmlTagHandler, GetSupportedTags);
IMP_PYCALLBACK_BOOL_TAG_pure(wxPyHtmlTagHandler, wxHtmlTagHandler, HandleTag);
%}


%name(wxHtmlTagHandler) class wxPyHtmlTagHandler {
public:
    wxPyHtmlTagHandler();

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    void SetParser(wxHtmlParser *parser);
    wxHtmlParser* GetParser();
    void ParseInner(const wxHtmlTag& tag);
};


//---------------------------------------------------------------------------

%{
class wxPyHtmlWinTagHandler : public wxHtmlWinTagHandler {
public:
    wxPyHtmlWinTagHandler() : wxHtmlWinTagHandler() {};

    wxHtmlWinParser* GetParser() { return m_WParser; }
    void ParseInner(const wxHtmlTag& tag)
        { wxHtmlWinTagHandler::ParseInner(tag); }

    DEC_PYCALLBACK_STRING__pure(GetSupportedTags);
    DEC_PYCALLBACK_BOOL_TAG_pure(HandleTag);

    PYPRIVATE;
};

IMP_PYCALLBACK_STRING__pure(wxPyHtmlWinTagHandler, wxHtmlWinTagHandler, GetSupportedTags);
IMP_PYCALLBACK_BOOL_TAG_pure(wxPyHtmlWinTagHandler, wxHtmlWinTagHandler, HandleTag);
%}


%name(wxHtmlWinTagHandler) class wxPyHtmlWinTagHandler : public wxPyHtmlTagHandler {
public:
    wxPyHtmlWinTagHandler();

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    void SetParser(wxHtmlParser *parser);
    wxHtmlWinParser* GetParser();
    void ParseInner(const wxHtmlTag& tag);
};


//---------------------------------------------------------------------------

%{

class wxPyHtmlTagsModule : public wxHtmlTagsModule {
public:
    wxPyHtmlTagsModule(PyObject* thc) : wxHtmlTagsModule() {
        m_tagHandlerClass = thc;
        Py_INCREF(m_tagHandlerClass);
        RegisterModule(this);
        wxHtmlWinParser::AddModule(this);
    }

    void OnExit() {
        Py_DECREF(m_tagHandlerClass);
        m_tagHandlerClass = NULL;
        for (size_t x=0; x < m_objArray.GetCount(); x++) {
            PyObject* obj = (PyObject*)m_objArray.Item(x);
            Py_DECREF(obj);
        }
    };

    void FillHandlersTable(wxHtmlWinParser *parser) {
        // Wave our magic wand...  (if it works it's a miracle!  ;-)

        // First, make a new instance of the tag handler
        bool doSave = wxPyRestoreThread();
        PyObject* arg = Py_BuildValue("()");
        PyObject* obj = PyInstance_New(m_tagHandlerClass, arg, NULL);
        Py_DECREF(arg);
        wxPySaveThread(doSave);

        // now figure out where it's C++ object is...
        wxPyHtmlWinTagHandler* thPtr;
        if (SWIG_GetPtrObj(obj, (void **)&thPtr, "_wxPyHtmlWinTagHandler_p"))
            return;

        // add it,
        parser->AddTagHandler(thPtr);

        // and track it.
        m_objArray.Add(obj);
    }

private:
    PyObject*           m_tagHandlerClass;
    wxArrayPtrVoid      m_objArray;

};
%}



%inline %{
    void wxHtmlWinParser_AddTagHandler(PyObject* tagHandlerClass) {
        // Dynamically create a new wxModule.  Refcounts tagHandlerClass
        // and adds itself to the wxModules list and to the wxHtmlWinParser.
        new wxPyHtmlTagsModule(tagHandlerClass);
    }
%}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxHtmlCell {
public:
    wxHtmlCell();

    int GetPosX();
    int GetPosY();
    int GetWidth();
    int GetHeight();
    int GetDescent();
    wxHtmlLinkInfo* GetLink(int x = 0, int y = 0);
    wxHtmlCell* GetNext();
    wxHtmlContainerCell* GetParent();
    void SetLink(const wxHtmlLinkInfo& link);
    void SetNext(wxHtmlCell *cell);
    void SetParent(wxHtmlContainerCell *p);
    void SetPos(int x, int y);
    void Layout(int w);
    void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
    void DrawInvisible(wxDC& dc, int x, int y);
    const wxHtmlCell* Find(int condition, const void* param);

    bool AdjustPagebreak(int * pagebreak);
    void SetCanLiveOnPagebreak(bool can);

};


class wxHtmlContainerCell : public wxHtmlCell {
public:
    wxHtmlContainerCell(wxHtmlContainerCell *parent);

    void InsertCell(wxHtmlCell *cell);
    void SetAlignHor(int al);
    int GetAlignHor();
    void SetAlignVer(int al);
    int GetAlignVer();
    void SetIndent(int i, int what, int units = wxHTML_UNITS_PIXELS);
    int GetIndent(int ind);
    int GetIndentUnits(int ind);
    void SetAlign(const wxHtmlTag& tag);
    void SetWidthFloat(int w, int units);
    %name(SetWidthFloatFromTag)void SetWidthFloat(const wxHtmlTag& tag);
    void SetMinHeight(int h, int align = wxHTML_ALIGN_TOP);
    void SetBackgroundColour(const wxColour& clr);
    void SetBorder(const wxColour& clr1, const wxColour& clr2);
    wxHtmlCell* GetFirstCell();
};



class wxHtmlColourCell : public wxHtmlCell {
public:
    wxHtmlColourCell(wxColour clr, int flags = wxHTML_CLR_FOREGROUND);

};



class wxHtmlWidgetCell : public wxHtmlCell {
public:
    wxHtmlWidgetCell(wxWindow* wnd, int w = 0);

};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// item of history list
class HtmlHistoryItem {
public:
    HtmlHistoryItem(const char* p, const char* a);

    int GetPos();
    void SetPos(int p);
    const wxString& GetPage();
    const wxString& GetAnchor();
};


//---------------------------------------------------------------------------
%{
class wxPyHtmlWindow : public wxHtmlWindow {
public:
    wxPyHtmlWindow(wxWindow *parent, wxWindowID id = -1,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxHW_SCROLLBAR_AUTO,
                   const wxString& name = "htmlWindow")
        : wxHtmlWindow(parent, id, pos, size, style, name)  {};

#ifdef POST_2_1_12
    void OnLinkClicked(const wxHtmlLinkInfo& link);
    void base_OnLinkClicked(const wxHtmlLinkInfo& link);
#else
    void OnLinkClicked(wxHtmlLinkInfo* link);
    void base_OnLinkClicked(wxHtmlLinkInfo* link);
#endif

    DEC_PYCALLBACK__STRING(OnSetTitle);
    PYPRIVATE;
};


IMP_PYCALLBACK__STRING(wxPyHtmlWindow, wxHtmlWindow, OnSetTitle);

#ifdef POST_2_1_12
 void wxPyHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link) {
#else
 void wxPyHtmlWindow::OnLinkClicked(wxHtmlLinkInfo* link) {
#endif
    bool doSave = wxPyRestoreThread();
    if (m_myInst.findCallback("OnLinkClicked")) {
#ifdef POST_2_1_12
        PyObject* obj = wxPyConstructObject((void*)&link, "wxHtmlLinkInfo");
#else
        PyObject* obj = wxPyConstructObject(link, "wxHtmlLinkInfo");
#endif
        m_myInst.callCallback(Py_BuildValue("(O)", obj));
    }
    else
        wxHtmlWindow::OnLinkClicked(link);
    wxPySaveThread(doSave);
}
#ifdef POST_2_1_12
void wxPyHtmlWindow::base_OnLinkClicked(const wxHtmlLinkInfo& link) {
#else
void wxPyHtmlWindow::base_OnLinkClicked(wxHtmlLinkInfo* link) {
#endif
    wxHtmlWindow::OnLinkClicked(link);
}
%}



%name(wxHtmlWindow) class wxPyHtmlWindow : public wxScrolledWindow {
public:
    wxPyHtmlWindow(wxWindow *parent, int id = -1,
                 wxPoint& pos = wxPyDefaultPosition,
                 wxSize& size = wxPyDefaultSize,
                 int flags=wxHW_SCROLLBAR_AUTO,
                 char* name = "htmlWindow");

    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"
    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:wx._StdOnScrollCallbacks(self)"


    bool SetPage(const char* source);
    bool LoadPage(const char* location);
    wxString GetOpenedPage();
    void SetRelatedFrame(wxFrame* frame, const char* format);
    wxFrame* GetRelatedFrame();
    void SetRelatedStatusBar(int bar);
    void SetFonts(wxString normal_face, wxString fixed_face, int *LIST);
    void SetTitle(const char* title);
    void SetBorders(int b);
    void ReadCustomization(wxConfigBase *cfg, char* path = "");
    void WriteCustomization(wxConfigBase *cfg, char* path = "");
    bool HistoryBack();
    bool HistoryForward();
    void HistoryClear();
    wxHtmlContainerCell* GetInternalRepresentation();
    wxHtmlWinParser* GetParser();

#ifdef POST_2_1_12
    void base_OnLinkClicked(const wxHtmlLinkInfo& link);
#else
    void base_OnLinkClicked(wxHtmlLinkInfo* link);
#endif
    void base_OnSetTitle(const char* title);
};

// Static methods are mapped to stand-alone functions
%inline %{
    void wxHtmlWindow_AddFilter(wxHtmlFilter *filter) {
        wxHtmlWindow::AddFilter(filter);
    }
%}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


class wxHtmlDCRenderer {
public:
    wxHtmlDCRenderer();
    ~wxHtmlDCRenderer();

    void SetDC(wxDC *dc, int maxwidth);
    void SetSize(int width, int height);
    void SetHtmlText(const wxString& html,
                     const wxString& basepath = wxEmptyString,
                     bool isdir = TRUE);
    int Render(int x, int y, int from = 0, int dont_render = FALSE);
    int GetTotalHeight();
                // returns total height of the html document
                // (compare Render's return value with this)
};

enum {
    wxPAGE_ODD,
    wxPAGE_EVEN,
    wxPAGE_ALL
};


class wxHtmlPrintout : public wxPyPrintout {
public:
    wxHtmlPrintout(const char* title = "Printout");
    ~wxHtmlPrintout();

    void SetHtmlText(const wxString& html,
                     const wxString &basepath = wxEmptyString,
                     bool isdir = TRUE);
    void SetHtmlFile(const wxString &htmlfile);
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL);
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);
    void SetMargins(float top = 25.2, float bottom = 25.2,
                    float left = 25.2, float right = 25.2,
                    float spaces = 5);
};



class wxHtmlEasyPrinting {
public:
    wxHtmlEasyPrinting(const char* name = "Printing",
                       wxFrame *parent_frame = NULL);
    ~wxHtmlEasyPrinting();

    void PreviewFile(const wxString &htmlfile);
    void PreviewText(const wxString &htmltext, const wxString& basepath = wxEmptyString);
    void PrintFile(const wxString &htmlfile);
    void PrintText(const wxString &htmltext, const wxString& basepath = wxEmptyString);
    void PrinterSetup();
    void PageSetup();
    void SetHeader(const wxString& header, int pg = wxPAGE_ALL);
    void SetFooter(const wxString& footer, int pg = wxPAGE_ALL);

    wxPrintData *GetPrintData() {return m_PrintData;}
    wxPageSetupDialogData *GetPageSetupData() {return m_PageSetupData;}

};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

%{
    extern "C" SWIGEXPORT(void) inithtmlhelpc();
%}


%init %{

    inithtmlhelpc();

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    // Until wxFileSystem is wrapped...
    #if wxUSE_FS_ZIP
       wxFileSystem::AddHandler(new wxZipFSHandler);
    #endif
%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_extras.py";

//---------------------------------------------------------------------------

