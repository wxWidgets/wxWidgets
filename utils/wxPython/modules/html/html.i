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
//%extern windows2.i
//%extern windows3.i
//%extern frames.i
//%extern misc.i
//%extern gdi.i
%extern controls.i
%extern controls2.i

%{
#ifdef __WXMSW__
static wxString wxPyEmptyStr("");
static wxPoint wxPyDefaultPosition(wxDefaultPosition);
static wxSize wxPyDefaultSize(wxDefaultSize);
#endif
static PyThreadState* wxPyThreadState;
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
	
	while (1) {
	   #ifdef WXP_WITH_THREAD
           PyEval_RestoreThread(wxPyThreadState);
	   #endif
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
	   SWIG_MakePtr(pbuf, m_WParser->GetWindow(), "_wxHtmlWindow_p");
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
	   #ifdef WXP_WITH_THREAD
	   PyEval_SaveThread();
	   #endif
	   wnd -> Show(TRUE);
	   m_WParser->OpenContainer()->InsertCell(new wxHtmlWidgetCell(wnd, fl));
	   return FALSE;
	}
	
	/* we got out of the loop. Must be an error. Show a box stating it. */
	
	#ifdef WXP_WITH_THREAD
	PyEval_SaveThread();
	#endif
	
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

// item of history list
class HtmlHistoryItem
{
    public:
        HtmlHistoryItem(const char* p, const char* a);
        int GetPos() const {return m_Pos;}
        void SetPos(int p) {m_Pos = p;}
        const wxString& GetPage() const ;
        const wxString& GetAnchor() const ;
};

class wxHtmlWindow : public wxScrolledWindow
{
    public:
        wxHtmlWindow(wxWindow *parent, int id = -1, 
                     const wxPoint& pos = wxPyDefaultPosition, 
		     const wxSize& size = wxPyDefaultSize,
		     int flags=wxHW_SCROLLBAR_AUTO, 
		     const char* name = "htmlWindow");
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

        wxString GetOpenedPage() const {return m_OpenedPage;}
                // Returns full location of opened page
        
        void SetRelatedFrame(wxFrame* frame, const char* format);
                // sets frame in which page title will  be displayed. Format is format of
                // frame title, e.g. "HtmlHelp : %s". It must contain exactly one %s 
        wxFrame* GetRelatedFrame() const;

        void SetRelatedStatusBar(int bar);
                // after(!) calling SetRelatedFrame, this sets statusbar slot where messages
                // will be displayed. Default is -1 = no messages.

        void SetFonts(wxString normal_face, int normal_italic_mode, wxString fixed_face, int fixed_italic_mode, int *sizes);
                // sets fonts to be used when displaying HTML page.
                // *_italic_mode can be either wxSLANT or wxITALIC

        void SetTitle(const char* title);
                // Sets the title of the window
                // (depending on the information passed to SetRelatedFrame() method)

        void SetBorders(int b);
                // Sets space between text and window borders.

        //virtual void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // saves custom settings into cfg config. it will use the path 'path'
                // if given, otherwise it will save info into currently selected path.
                // saved values : things set by SetFonts, SetBorders.
        //virtual void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // ...

        bool HistoryBack();
        bool HistoryForward();
                // Goes to previous/next page (in browsing history)
                // Returns TRUE if successful, FALSE otherwise
        void HistoryClear();
                // Resets history

        //wxHtmlContainerCell* GetInternalRepresentation() const;
                // Returns pointer to conteiners/cells structure.
                // It should be used ONLY when printing

        //static void AddFilter(wxHtmlFilter *filter);
                // Adds input filter

        /* This function needs to be eventified! */
        //virtual void OnLinkClicked(const char* link);
                // called when users clicked on hypertext link. Default behavior is to
                // call LoadPage(loc)
		
	//static void CleanUpStatics();
	        // cleans static variables

};

enum {
    wxID_HTML_PANEL,
    wxID_HTML_BACK,
    wxID_HTML_FORWARD,
    wxID_HTML_TREECTRL,
    wxID_HTML_INDEXPAGE,
    wxID_HTML_INDEXLIST,
    wxID_HTML_NOTEBOOK,
    wxID_HTML_SEARCHPAGE,
    wxID_HTML_SEARCHTEXT,
    wxID_HTML_SEARCHLIST,
    wxID_HTML_SEARCHBUTTON
};

class wxHtmlHelpController : public wxEvtHandler
{
public:
	wxHtmlHelpController();
        ~wxHtmlHelpController();

	// Images:
	enum {
	    IMG_Book = 0,
	    IMG_Folder,
	    IMG_Page
	};

        void SetTitleFormat(const wxString& format) {m_TitleFormat = format;}
                // Sets format of title of the frame. Must contain exactly one "%s"
                // (for title of displayed HTML page)

        void SetTempDir(const wxString& path);
                // Sets directory where temporary files are stored.
                // These temp files are index & contents file in binary (much faster to read)
                // form. These files are NOT deleted on program's exit.

        bool AddBook(const wxString& book, bool show_wait_msg = FALSE);
                // Adds new book. 'book' is location of .htb file (stands for "html book").
                // See documentation for details on its format.
                // Returns success.
                // If show_wait_msg == true then message window with "loading book..." is displayed

        void Display(const wxString& x);
                // Displays page x. If not found it will offect the user a choice of searching
                // books.
                // Looking for the page runs in these steps:
                // 1. try to locate file named x (if x is for example "doc/howto.htm")
                // 2. try to open starting page of book x
                // 3. try to find x in contents (if x is for example "How To ...")
                // 4. try to find x in index (if x is for example "How To ...")
                // 5. offer searching and if the user agree, run KeywordSearch
        %name(DisplayID) void Display(const int id);
	        // Alternative version that works with numeric ID.
	        // (uses extension to MS format, <param name="ID" value=id>, see docs)

        void DisplayContents();
                // Displays help window and focuses contents.

        void DisplayIndex();
                // Displays help window and focuses index.

        bool KeywordSearch(const wxString& keyword);
                // Searches for keyword. Returns TRUE and display page if found, return
                // FALSE otherwise
                // Syntax of keyword is Altavista-like:
                // * words are separated by spaces
                //   (but "\"hello world\"" is only one world "hello world")
                // * word may be pretended by + or -
                //   (+ : page must contain the word ; - : page can't contain the word)
                // * if there is no + or - before the word, + is default

        void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);
                // Assigns config object to the controller. This config is then
                // used in subsequent calls to Read/WriteCustomization of both help
                // controller and it's wxHtmlWindow

        void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // saves custom settings into cfg config. it will use the path 'path'
                // if given, otherwise it will save info into currently selected path.
                // saved values : things set by SetFonts, SetBorders.
        void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // ...
};

class wxHtmlHelpSystem : public wxHtmlHelpController
{
 public:
   wxHtmlHelpSystem() {};
   ~wxHtmlHelpSystem() {};
   
   bool AddBookParam(const wxString& title, const wxString& contfile, 
		     const wxString& indexfile=wxEmptyString, const wxString& deftopic=wxEmptyString, 
		     const wxString& path=wxEmptyString, bool show_wait_msg=FALSE);
   // Alternative to AddBook(wxString& hhpfile)
   wxToolBar* CreateToolBar(wxFrame* frame);
   // creates a dockable toolbar for the frame, containing hide/show, back and forward buttons
   wxTreeCtrl* CreateContentsTree(wxWindow* parent);
   // creates a treecontrol with imagelist for books, folders etc and id wxID_HTML_TREECTRL
   wxListBox* CreateIndexList(wxWindow* parent);
   // creates a listbox with the right id
   virtual void CreateHelpWindow();
   // Slightly different version than in wxHtmlHelpController; uses helpers above
   // Do nothing if the window already exists
   void SetControls(wxFrame* frame, wxHtmlWindow* htmlwin, 
		    wxTreeCtrl* contents=NULL, wxListBox* index=NULL,
		    wxListBox* searchlist=NULL);
   // alternative for CreateHelpWindow(), sets frame, htmlwindow, contents tree, index
   // listbox and searchlist listbox. If null, their functionality won't be used

   // Some extra accessor functions
   wxFrame* GetFrame() { return m_Frame; }
   wxHtmlWindow* GetHtmlWindow() { return m_HtmlWin; }
   wxTreeCtrl* GetContentsTree() { return m_ContentsBox; }
   wxListBox* GetIndexList() { return m_IndexBox; }
   wxListBox* GetSearchList() { return m_SearchList; }
   wxImageList* GetContentsImageList() { return m_ContentsImageList; }
   // public interface for wxHtmlHelpControllers handlers, so wxPython can call them
   void OnToolbar(wxCommandEvent& event);
   void OnContentsSel(wxTreeEvent& event) {wxHtmlHelpController::OnContentsSel(event);}
   void OnIndexSel(wxCommandEvent& event) {wxHtmlHelpController::OnIndexSel(event);}
   void OnSearchSel(wxCommandEvent& event) {wxHtmlHelpController::OnSearchSel(event);}
   void OnSearch(wxCommandEvent& event) {wxHtmlHelpController::OnSearch(event);}
   void OnCloseWindow(wxCloseEvent& event);

   // some more protected functions that should be accessible from wxPython
   void RefreshLists();
   void CreateContents() { if (!m_IndexBox) return; wxHtmlHelpController::CreateContents(); }
   // Adds items to m_Contents tree control
   void CreateIndex() { if (! m_ContentsBox) return; wxHtmlHelpController::CreateIndex(); }
   // Adds items to m_IndexList
};

// just for fun, an Altavista-like search engine (the gems that Vaclav has hidden in wxHtml...)
// but not for wxMSW because it's not DLL exported
//#ifndef __WXMSW__
#ifdef THIS_IS_NOT_DEFINED_OKAY
class wxSearchEngine
{
    public:
        wxSearchEngine() : wxObject() {m_Keyword = NULL;}
        ~wxSearchEngine() {if (m_Keyword) free(m_Keyword);}

        void LookFor(const wxString& keyword);
            // Sets the keyword we will be searching for

        bool Scan(wxInputStream *stream);
            // Scans the stream for the keyword.
            // Returns TRUE if the stream contains keyword, fALSE otherwise
	%addmethods {
		bool ScanFile(const wxString& filename) {
			if (filename.IsEmpty())
				return false;
			wxFileInputStream istr(filename);
			return self->Scan(&istr);
		}
	}
};
#endif

//---------------------------------------------------------------------------

%init %{

    /* This is a bit cheesy. SWIG happens to call the dictionary d... 
     * I save it here, 'cause I don't know how to get it back later! */
    mod_dict = d;
    wxPyThreadState = PyThreadState_Get();
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
