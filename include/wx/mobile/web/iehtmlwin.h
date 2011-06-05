/*! \file iehtmlwin.h
    \brief implements wxIEHtmlWin window class
*/
#ifndef _IEHTMLWIN_H_
#define _IEHTMLWIN_H_
#pragma warning( disable : 4101 4786)
#pragma warning( disable : 4786)


#include <wx/setup.h>
#include <wx/wx.h>

// Memory leak checking is not compatible with STL.
// Should configure wxWindows without leak checking
// but this makes it compile at least.
#ifdef new
#undef new
#endif

#include "wx/beforestd.h"

#include <exdisp.h>
#include <iostream>
using namespace std;

#include "iptk/web/wxactivex.h"


enum wxIEHtmlRefreshLevel
{
    wxIEHTML_REFRESH_NORMAL = 0,
    wxIEHTML_REFRESH_IFEXPIRED = 1,
    wxIEHTML_REFRESH_CONTINUE = 2,
    wxIEHTML_REFRESH_COMPLETELY = 3
};

class IStreamAdaptorBase;

class wxIEHtmlWin : public wxActiveXOld
{
public:
    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxNO_BORDER | wxWANTS_CHARS,
        const wxString& name = wxPanelNameStr);
    virtual ~wxIEHtmlWin();

    void LoadUrl(const wxString &_url, const wxString &_frame = wxEmptyString);
    bool LoadString(wxString html);
    bool LoadStream(istream *strm);
    bool LoadStream(wxInputStream *is);

    void SetCharset(wxString charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
    wxString GetText(bool asHTML = false);
    wxString GetRealLocation();

    bool GoBack();
    bool GoForward();
    bool GoHome();
    bool GoSearch();
    bool Refresh(wxIEHtmlRefreshLevel level);
    bool Stop();

    void RegisterAsDropTarget(bool _register);

    DECLARE_EVENT_TABLE();

protected:
    void SetupBrowser();
    bool LoadStream(IStreamAdaptorBase *pstrm);

    wxAutoOleInterface<IWebBrowser2>        m_webBrowser;
};

#endif /* _IEHTMLWIN_H_ */
