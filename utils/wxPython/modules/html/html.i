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
#include <wx/image.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>
#include "helpsys.h"
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

%extern utils.i

//---------------------------------------------------------------------------

%{
#ifdef __WXMSW__
wxString wxPyEmptyStr("");
wxPoint wxPyDefaultPosition(wxDefaultPosition);
wxSize wxPyDefaultSize(wxDefaultSize);
#endif
%}

%pragma(python) code = "import wx,htmlhelper"
%pragma(python) code = "widget = htmlc"

%{

static PyObject* mod_dict = NULL; // will be set by init

#include <wx/html/mod_templ.h>

TAG_HANDLER_BEGIN(PYTHONTAG, "PYTHON")
    TAG_HANDLER_PROC(tag)
    {
        wxWindow *wnd;
        wxString errmsg;
        char pbuf[256];

        int fl = 0;

        bool doSave = wxPyRestoreThread();
        while (1) {
           if (tag.HasParam("FLOAT"))
               tag.ScanParam("FLOAT", "%i", &fl);
           PyObject* pyfunc = PyDict_GetItemString(mod_dict, "WidgetStarter");
           if (pyfunc == NULL) {
               errmsg = "Could not find object WidgetStarter";
               break;
           }
           if (! PyCallable_Check(pyfunc)) {
               errmsg = "WidgetStarter does not appear to be callable";
               break;
           }
           SWIG_MakePtr(pbuf, m_WParser->GetWindow(), "_wxPyHtmlWindow_p");
           PyObject* arglist = Py_BuildValue("(s,s)", pbuf,
                               (const char*)tag.GetAllParams());
           if (! arglist) {
               errmsg = "Failed making argument list";
               break;
           }
           PyObject* ret = PyEval_CallObject(pyfunc, arglist);
           Py_DECREF(arglist);
           if (ret == NULL) {
              errmsg = "An error occured while calling WidgetStarter";
              if (PyErr_Occurred())
                 PyErr_Print();
              break;
           }
           wnd = NULL;
           if (PyString_Check(ret)) {
              char* thisc = PyString_AsString(ret);
              SWIG_GetPtr(thisc, (void**)&wnd, "_wxWindow_p");
           }
           Py_DECREF(ret);
           if (! wnd) {
               errmsg = "Could not make a wxWindow pointer from return ptr";
               break;
           }
           wxPySaveThread(doSave);
           wnd -> Show(TRUE);
           m_WParser->OpenContainer()->InsertCell(new wxHtmlWidgetCell(wnd, fl));
           return FALSE;
        }

        wxPySaveThread(doSave);

        /* we got out of the loop. Must be an error. Show a box stating it. */
        wnd = new wxTextCtrl( m_WParser -> GetWindow(), -1,
                              errmsg, wxPoint(0,0),
                              wxSize(300, 100), wxTE_MULTILINE );
        wnd -> Show(TRUE);
        m_WParser->OpenContainer()->InsertCell(new wxHtmlWidgetCell(wnd, 100));
        return FALSE;
    }

TAG_HANDLER_END(PYTHONTAG)

TAGS_MODULE_BEGIN(PythonTag)

    TAGS_MODULE_ADD(PYTHONTAG)

TAGS_MODULE_END(PythonTag)

// Note: see also the init function where we add the module!

%}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class wxHtmlTag {
public:
    // Never need to create a new tag...
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
    void SetFonts(wxString normal_face, int normal_italic_mode,
                  wxString fixed_face, int fixed_italic_mode, int *LIST);

    wxHtmlContainerCell* GetContainer();
    wxHtmlContainerCell* OpenContainer();
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
    const wxString& GetLink();
    void SetLink(const wxString& link);
    wxFont* CreateCurrentFont();
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
        for (int x=0; x < m_objArray.GetCount(); x++) {
            PyObject* obj = (PyObject*)m_objArray.Item(x);
            Py_DECREF(obj);
        }
    };

    void FillHandlersTable(wxHtmlWinParser *parser) {
        // Wave our magic wand...  (if it works it's a miracle!  ;-)

        // First, make a new instance of the tag handler
        PyObject* arg = Py_BuildValue("()");
        PyObject* obj = PyInstance_New(m_tagHandlerClass, arg, NULL);
        Py_DECREF(arg);

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

    DEC_PYCALLBACK__STRING(OnLinkClicked);

    PYPRIVATE;
};

IMP_PYCALLBACK__STRING(wxPyHtmlWindow, wxHtmlWindow, OnLinkClicked);

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
                // Set HTML page and display it. !! source is HTML document itself,
                // it is NOT address/filename of HTML document. If you want to
                // specify document location, use LoadPage() istead
                // Return value : FALSE if an error occured, TRUE otherwise

    bool LoadPage(const char* location);
                // Load HTML page from given location. Location can be either
                // a) /usr/wxGTK2/docs/html/wx.htm
                // b) http://www.somewhere.uk/document.htm
                // c) ftp://ftp.somesite.cz/pub/something.htm
                // In case there is no prefix (http:,ftp:), the method
                // will try to find it itself (1. local file, then http or ftp)
                // After the page is loaded, the method calls SetPage() to display it.
                // Note : you can also use path relative to previously loaded page
                // Return value : same as SetPage

    wxString GetOpenedPage();
                // Returns full location of opened page

    void SetRelatedFrame(wxFrame* frame, const char* format);
                // sets frame in which page title will  be displayed. Format is format of
                // frame title, e.g. "HtmlHelp : %s". It must contain exactly one %s
    wxFrame* GetRelatedFrame();

    void SetRelatedStatusBar(int bar);
                // after(!) calling SetRelatedFrame, this sets statusbar slot where messages
                // will be displayed. Default is -1 = no messages.

    void SetFonts(wxString normal_face, int normal_italic_mode,
                  wxString fixed_face, int fixed_italic_mode, int *LIST);
                // sets fonts to be used when displaying HTML page.
                // *_italic_mode can be either wxSLANT or wxITALIC

    void SetTitle(const char* title);
                // Sets the title of the window
                // (depending on the information passed to SetRelatedFrame() method)

    void SetBorders(int b);
                // Sets space between text and window borders.

    void ReadCustomization(wxConfigBase *cfg, char* path = "");
                // saves custom settings into cfg config. it will use the path 'path'
                // if given, otherwise it will save info into currently selected path.
                // saved values : things set by SetFonts, SetBorders.

    void WriteCustomization(wxConfigBase *cfg, char* path = "");
                // ...

    bool HistoryBack();
    bool HistoryForward();
                // Goes to previous/next page (in browsing history)
                // Returns TRUE if successful, FALSE otherwise
    void HistoryClear();
                // Resets history

    // wxHtmlContainerCell* GetInternalRepresentation() const;
                // Returns pointer to conteiners/cells structure.
                // It should be used ONLY when printing


    void base_OnLinkClicked(const char* link);
                // called when users clicked on hypertext link. Default behavior is to
                // call LoadPage(loc)

};

// Static methods are mapped to stand-alone functions
%inline %{
    void wxHtmlWindow_AddFilter(wxHtmlFilter *filter) {
        wxHtmlWindow::AddFilter(filter);
    }
%}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

%{
    extern "C" SWIGEXPORT(void) inithtmlhelpc();
%}


%init %{

    /* This is a bit cheesy. SWIG happens to call the dictionary d...
     * I save it here, 'cause I don't know how to get it back later! */
    mod_dict = d;

    //inithtmlhelpc();

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    /* specifically add our python tag handler; it doesn't seem to
     * happen by itself... */
    wxHtmlWinParser::AddModule(new HTML_ModulePythonTag());

    // Until wxFileSystem is wrapped...
    #if wxUSE_FS_ZIP
       wxFileSystem::AddHandler(new wxZipFSHandler);
    #endif
%}

//---------------------------------------------------------------------------
