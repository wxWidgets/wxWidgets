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
//%extern controls.i


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
        wxHtmlWindow(wxWindow *parent, wxWindowID id = -1, 
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
