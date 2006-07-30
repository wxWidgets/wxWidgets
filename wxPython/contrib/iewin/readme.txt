Lindsay Mathieson
Email : <lmathieson@optusnet.com.au>

This is prelimanary stuff - the controls need extra methods and events etc,
feel free to email with suggestions &/or patches.

Tested with wxWindows 2.3.2.
Built with MS Visual C++ 6.0 & DevStudio
Minor use of templates and STL

-----------------------------------------------------------
This sample illustrates using wxActiveX and wxIEHtmlWin too:
1. Host an arbitrary ActiveX control
2. Specifically host the MSHTML Control


wxActiveX:
==========
wxActiveX is used to host and siplay any activeX control, all the wxWindows developer
needs to know is either the ProgID or CLSID of the control in question.

Derived From:
- wxWindow

Include Files:
- wxactivex.h

Source Files:
- wxactivex.cpp

Event Handling:
---------------
- EVT_ACTIVEX(id, eventName, handler) (handler = void OnActiveX(wxActiveXEvent& event))
class wxActiveXEvent : public wxNotifyEvent
    int ParamCount() const;
    wxVariant  operator[] (int idx) const;	// parameter by index
    wxVariant& operator[] (int idx);
    wxVariant  operator[] (wxString name) const; // named parameters
    wxVariant& operator[] (wxString name);


Members:
--------
wxActiveX::wxActiveX(wxWindow * parent, REFCLSID clsid, wxWindowID id = -1);
- Creates a activeX control identified by clsid
e.g
  wxFrame *frame = new wxFrame(this, -1, "test");
  wxActiveX *X = new wxActiveX(frame, CLSID_WebBrowser);

wxActiveX::wxActiveX(wxWindow * parent, wxString progId, wxWindowID id = -1);
- Creates a activeX control identified by progId
e.g.
  wxFrame *frame = new wxFrame(this, -1, "test");
  wxActiveX *X = new wxActiveX(frame, "MSCAL.Calendar");


wxActiveX::~wxActiveX();
- Destroys the control
- disconnects all connection points

HRESULT wxActiveX::ConnectAdvise(REFIID riid, IUnknown *eventSink);
- Connects a event sink. Connections are automaticlly diconnected in the destructor
e.g.
  FS_DWebBrowserEvents2 *events = new FS_DWebBrowserEvents2(iecontrol);
  hret = iecontrol->ConnectAdvise(DIID_DWebBrowserEvents2, events);
  if (! SUCCEEDED(hret))
    delete events;


Sample Events:
--------------
EVT_ACTIVEX(ID_MSHTML, "BeforeNavigate2",   OnMSHTMLBeforeNavigate2X)

void wxIEFrame::OnMSHTMLBeforeNavigate2X(wxActiveXEvent& event)
{
  wxString url = event["Url"];

  int rc = wxMessageBox(url, "Allow open url ?", wxYES_NO);

  if (rc != wxYES)
    event["Cancel"] = true;
};


wxIEHtmlWin:
============
wxIEHtmlWin is a specialisation of the wxActiveX control for hosting the MSHTML control.

Derived From:
- wxActiveX
- wxWindow

Event Handling:
---------------
- class wxMSHTMLEvent

- EVT_MSHTML_BEFORENAVIGATE2
* url = event.m_text1
* event.Veto() to cancel
Generated before an attempt to browse a new url

- EVT_MSHTML_NEWWINDOW2
* event.Veto() to cancel
Generated when the control is asked create a new window (e.g a popup)

- EVT_MSHTML_DOCUMENTCOMPLETE
* url = event.m_text1
Generated after the document has finished loading

- EVT_MSHTML_PROGRESSCHANGE
* event.m_long1 = progress so far
* event.m_long2 = max range of progress

- EVT_MSHTML_STATUSTEXTCHANGE
* status = event.m_text1

- EVT_MSHTML_TITLECHANGE
* title = event.m_text1

Members:
--------
wxIEHtmlWin::wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1);
- Constructs and initialises the MSHTML control
- LoadUrl("about:blank") is called

wxIEHtmlWin::~wxIEHtmlWin();
- destroys the control

void wxIEHtmlWin::LoadUrl(const wxString&);
- Attempts to browse to the url, the control uses its internal (MS)
  network streams

bool wxIEHtmlWin::LoadString(wxString html);
- Load the passed HTML string

bool wxIEHtmlWin::LoadStream(istream *strm);
- load the passed HTML stream. The control takes ownership of
  the pointer, deleting when finished.

void wxIEHtmlWin::SetCharset(wxString charset);
- Sets the charset of the loaded document

void wxIEHtmlWin::SetEditMode(bool seton);
- Sets edit mode. 
  NOTE: This does work, but is bare bones - we need more events exposed before
        this is usable as an HTML editor.

bool wxIEHtmlWin::GetEditMode();
- Returns the edit mode setting

wxString wxIEHtmlWin::GetStringSelection(bool asHTML = false);
- Returns the currently selected text (plain or HTML text)

wxString GetText(bool asHTML = false);
- Returns the body text (plain or HTML text)

Lindsay Mathieson
Email : <lmathieson@optusnet.com.au>
