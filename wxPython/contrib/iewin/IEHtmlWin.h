#ifndef _IEHTMLWIN_H_
#define _IEHTMLWIN_H_
#pragma warning( disable : 4101 4786)
#pragma warning( disable : 4786)


#include <wx/setup.h>
#include <wx/wx.h>
#include <exdisp.h>
#include <iostream>
using namespace std;

#include "wxactivex.h"

class wxMSHTMLEvent : public wxNotifyEvent
{
public:
	wxMSHTMLEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
		: wxNotifyEvent(commandType, id)
	{}

	wxString GetText1() { return m_text1; }
	long GetLong1() { return m_long1; }
	long GetLong2() { return m_long2; }


	wxString	m_text1;
	long		m_long1, m_long2;

	virtual wxEvent *Clone() const { return new wxMSHTMLEvent(*this); }

private:
	DECLARE_DYNAMIC_CLASS(wxMSHTMLEvent)

};

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, 0)
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_COMMAND_MSHTML_NEWWINDOW2, 0)
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, 0)
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, 0)
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, 0)
	DECLARE_LOCAL_EVENT_TYPE(wxEVT_COMMAND_MSHTML_TITLECHANGE, 0)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxMSHTMLEventFunction)(wxMSHTMLEvent&);

#define EVT_MSHTML_BEFORENAVIGATE2(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxMSHTMLEventFunction, & fn ), NULL ),
#define EVT_MSHTML_NEWWINDOW2(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MSHTML_NEWWINDOW2, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxMSHTMLEventFunction, & fn ), NULL ),
#define EVT_MSHTML_DOCUMENTCOMPLETE(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxMSHTMLEventFunction, & fn ), NULL ),
#define EVT_MSHTML_PROGRESSCHANGE(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxMSHTMLEventFunction, & fn ), NULL ),
#define EVT_MSHTML_STATUSTEXTCHANGE(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxMSHTMLEventFunction, & fn ), NULL ),
#define EVT_MSHTML_TITLECHANGE(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MSHTML_TITLECHANGE, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxMSHTMLEventFunction, & fn ), NULL ),


enum wxIEHtmlRefreshLevel 
{
	wxIEHTML_REFRESH_NORMAL = 0,
	wxIEHTML_REFRESH_IFEXPIRED = 1,
	wxIEHTML_REFRESH_CONTINUE = 2,
	wxIEHTML_REFRESH_COMPLETELY = 3
};

class IStreamAdaptorBase;

class wxIEHtmlWin : public wxActiveX
{

public:
    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxPanelNameStr);
	virtual ~wxIEHtmlWin();

	void LoadUrl(const wxString&);
    bool LoadString(wxString html);
    bool LoadStream(istream *strm);
    bool LoadStream(wxInputStream *is);

	void SetCharset(wxString charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
	wxString GetText(bool asHTML = false);

	bool GoBack();
	bool GoForward();
	bool GoHome();
	bool GoSearch();
	bool Refresh(wxIEHtmlRefreshLevel level);
	bool Stop();

	DECLARE_EVENT_TABLE();

protected:
    void SetupBrowser();
    bool LoadStream(IStreamAdaptorBase *pstrm);

	wxAutoOleInterface<IWebBrowser2>		m_webBrowser;
};

#endif /* _IEHTMLWIN_H_ */
