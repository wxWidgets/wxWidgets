#include "IEHtmlWin.h"
#include <wx/strconv.h>
#include <wx/string.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/mstream.h>
#include <oleidl.h>
#include <winerror.h>
#include <exdispid.h>
#include <exdisp.h>
#include <olectl.h>
#include <Mshtml.h>
#include <sstream>
using namespace std;

DEFINE_EVENT_TYPE(wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2);
DEFINE_EVENT_TYPE(wxEVT_COMMAND_MSHTML_NEWWINDOW2);
DEFINE_EVENT_TYPE(wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE);
DEFINE_EVENT_TYPE(wxEVT_COMMAND_MSHTML_PROGRESSCHANGE);
DEFINE_EVENT_TYPE(wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE);
DEFINE_EVENT_TYPE(wxEVT_COMMAND_MSHTML_TITLECHANGE);

IMPLEMENT_DYNAMIC_CLASS(wxMSHTMLEvent, wxNotifyEvent);


//////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxIEHtmlWin, wxActiveX)
END_EVENT_TABLE()

class FS_DWebBrowserEvents2 : public IDispatch
{
private:
    DECLARE_OLE_UNKNOWN(FS_DWebBrowserEvents2);


    wxIEHtmlWin *m_iewin;

public:
    FS_DWebBrowserEvents2(wxIEHtmlWin *iewin) : m_iewin(iewin) {}
	virtual ~FS_DWebBrowserEvents2()
    {
    }

	//IDispatch
	STDMETHODIMP GetIDsOfNames(REFIID r, OLECHAR** o, unsigned int i, LCID l, DISPID* d)
	{
        return E_NOTIMPL;
    };

	STDMETHODIMP GetTypeInfo(unsigned int i, LCID l, ITypeInfo** t)
	{
        return E_NOTIMPL;
    };

	STDMETHODIMP GetTypeInfoCount(unsigned int* i)
	{
        return E_NOTIMPL;
    };

	void Post(WXTYPE etype, wxString text, long l1 = 0, long l2 = 0)
	{
		if (! m_iewin || ! m_iewin->GetParent())
			return;

		wxMSHTMLEvent event;
		event.SetId(m_iewin->GetId());
		event.SetEventType(etype);
		event.m_text1 = text;
		event.m_long1 = l1;
		event.m_long2 = l2;

		m_iewin->GetParent()->AddPendingEvent(event);
	};

	bool Process(WXTYPE etype, wxString text = wxEmptyString, long l1 = 0, long l2 = 0)
	{
		if (! m_iewin || ! m_iewin->GetParent())
			return true;

		wxMSHTMLEvent event;
		event.SetId(m_iewin->GetId());
		event.SetEventType(etype);
		event.m_text1 = text;
		event.m_long1 = l1;
		event.m_long2 = l2;

		m_iewin->GetParent()->ProcessEvent(event);

		return event.IsAllowed();
	};

	wxString GetStrArg(VARIANT& v)
	{
		VARTYPE vt = v.vt & ~VT_BYREF;

		if (vt == VT_VARIANT)
			return GetStrArg(*v.pvarVal);
		else if (vt == VT_BSTR)
		{
			if (v.vt & VT_BYREF)
				return (v.pbstrVal ? *v.pbstrVal : L"");
			else
				return v.bstrVal;
		}
		else
			return wxEmptyString;
	};

#define STR_ARG(arg) GetStrArg(pDispParams->rgvarg[arg])

#define LONG_ARG(arg)\
			(pDispParams->rgvarg[arg].lVal)


	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
						  WORD wFlags, DISPPARAMS * pDispParams,
						  VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
						  unsigned int * puArgErr)
	{
	    if (wFlags & DISPATCH_PROPERTYGET)
            return E_NOTIMPL;

	    switch (dispIdMember)
	    {
		    case DISPID_BEFORENAVIGATE2:
				if (Process(wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, STR_ARG(5)))
					*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
				else
					*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				break;

		    case DISPID_NEWWINDOW2:
				if (Process(wxEVT_COMMAND_MSHTML_NEWWINDOW2))
					*pDispParams->rgvarg->pboolVal = VARIANT_FALSE;
				else
					*pDispParams->rgvarg->pboolVal = VARIANT_TRUE;
				break;

            case DISPID_PROGRESSCHANGE:
				Post(wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, wxEmptyString, LONG_ARG(1), LONG_ARG(0));
				break;

            case DISPID_DOCUMENTCOMPLETE:
				Post(wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, STR_ARG(0));
				break;

            case DISPID_STATUSTEXTCHANGE:
				Post(wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, STR_ARG(0));
				break;

            case DISPID_TITLECHANGE:
				Post(wxEVT_COMMAND_MSHTML_TITLECHANGE, STR_ARG(0));
				break;
	    }

    	return S_OK;
    }
};

#undef STR_ARG

DEFINE_OLE_TABLE(FS_DWebBrowserEvents2)
	OLE_IINTERFACE(IUnknown)
	OLE_INTERFACE(DIID_DWebBrowserEvents2, DWebBrowserEvents2)
END_OLE_TABLE;


static const CLSID CLSID_MozillaBrowser =
{ 0x1339B54C, 0x3453, 0x11D2,
  { 0x93, 0xB9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00 } };


//#define PROGID "Shell.Explorer"
#define PROGID CLSID_WebBrowser
//#define PROGID CLSID_MozillaBrowser
//#define PROGID CLSID_HTMLDocument
//#define PROGID "MSCAL.Calendar"
//#define PROGID "WordPad.Document.1"
//#define PROGID "SoftwareFX.ChartFX.20"

wxIEHtmlWin::wxIEHtmlWin(wxWindow * parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) :
    wxActiveX(parent, PROGID, id, pos, size, style, name)
{
    SetupBrowser();
}


wxIEHtmlWin::~wxIEHtmlWin()
{
}

void wxIEHtmlWin::SetupBrowser()
{
	HRESULT hret;

	// Get IWebBrowser2 Interface
	hret = m_webBrowser.QueryInterface(IID_IWebBrowser2, m_ActiveX);
	assert(SUCCEEDED(hret));

	// Web Browser Events
	FS_DWebBrowserEvents2 *events = new FS_DWebBrowserEvents2(this);
	hret = ConnectAdvise(DIID_DWebBrowserEvents2, events);
	if (! SUCCEEDED(hret))
		delete events;

	// web browser setup
	m_webBrowser->put_MenuBar(VARIANT_FALSE);
	m_webBrowser->put_AddressBar(VARIANT_FALSE);
	m_webBrowser->put_StatusBar(VARIANT_FALSE);
	m_webBrowser->put_ToolBar(VARIANT_FALSE);

	m_webBrowser->put_RegisterAsBrowser(VARIANT_TRUE);
	m_webBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);

    m_webBrowser->Navigate( L"about:blank", NULL, NULL, NULL, NULL );
}


void wxIEHtmlWin::SetEditMode(bool seton)
{
    m_bAmbientUserMode = ! seton;
    AmbientPropertyChanged(DISPID_AMBIENT_USERMODE);
};

bool wxIEHtmlWin::GetEditMode()
{
    return ! m_bAmbientUserMode;
};


void wxIEHtmlWin::SetCharset(wxString charset)
{
	// HTML Document ?
	IDispatch *pDisp = NULL;
	HRESULT hret = m_webBrowser->get_Document(&pDisp);
	wxAutoOleInterface<IDispatch> disp(pDisp);

	if (disp.Ok())
	{
		wxAutoOleInterface<IHTMLDocument2> doc(IID_IHTMLDocument2, disp);
		if (doc.Ok())
            doc->put_charset((BSTR) (const wchar_t *) charset.wc_str(wxConvUTF8));
			//doc->put_charset((BSTR) wxConvUTF8.cMB2WC(charset).data());
	};
};


class IStreamAdaptorBase : public IStream
{
private:
    DECLARE_OLE_UNKNOWN(IStreamAdaptorBase);

public:
    IStreamAdaptorBase() {}
    virtual ~IStreamAdaptorBase() {}

    // ISequentialStream
    HRESULT STDMETHODCALLTYPE Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead) = 0;
    HRESULT STDMETHODCALLTYPE Write(const void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbWritten) {return E_NOTIMPL;}

    // IStream
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER __RPC_FAR *plibNewPosition) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CopyTo(IStream __RPC_FAR *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER __RPC_FAR *pcbRead, ULARGE_INTEGER __RPC_FAR *pcbWritten) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Revert(void) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Stat(STATSTG __RPC_FAR *pstatstg, DWORD grfStatFlag) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Clone(IStream __RPC_FAR *__RPC_FAR *ppstm) {return E_NOTIMPL;}
};

DEFINE_OLE_TABLE(IStreamAdaptorBase)
	OLE_IINTERFACE(IUnknown)
	OLE_IINTERFACE(ISequentialStream)
	OLE_IINTERFACE(IStream)
END_OLE_TABLE;

class IStreamAdaptor : public IStreamAdaptorBase
{
private:
    istream *m_is;

public:

    IStreamAdaptor(istream *is)	: IStreamAdaptorBase(), m_is(is)
    {
        wxASSERT(m_is != NULL);
    }
    ~IStreamAdaptor()
    {
        delete m_is;
    }

    // ISequentialStream
    HRESULT STDMETHODCALLTYPE Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead)
	{
		m_is->read((char *) pv, cb);
		if (pcbRead)
			*pcbRead = m_is->gcount();

		return S_OK;
	};
};

class IwxStreamAdaptor : public IStreamAdaptorBase
{
private:
    wxInputStream *m_is;

public:

    IwxStreamAdaptor(wxInputStream *is)	: IStreamAdaptorBase(), m_is(is)
    {
        wxASSERT(m_is != NULL);
    }
    ~IwxStreamAdaptor()
    {
        delete m_is;
    }

    // ISequentialStream
    HRESULT STDMETHODCALLTYPE Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead)
	{
		m_is->Read((char *) pv, cb);
		if (pcbRead)
			*pcbRead = m_is->LastRead();

		return S_OK;
	};
};

void wxIEHtmlWin::LoadUrl(const wxString& url)
{
	VARIANTARG navFlag, targetFrame, postData, headers;
	navFlag.vt = VT_EMPTY;
	navFlag.vt = VT_I2;
	navFlag.iVal = navNoReadFromCache;
	targetFrame.vt = VT_EMPTY;
	postData.vt = VT_EMPTY;
	headers.vt = VT_EMPTY;

	HRESULT hret = 0;
	hret = m_webBrowser->Navigate((BSTR) (const wchar_t *) url.wc_str(wxConvUTF8),
		&navFlag, &targetFrame, &postData, &headers);
};

class wxOwnedMemInputStream : public wxMemoryInputStream
{
public:
    char *m_data;

    wxOwnedMemInputStream(char *data, size_t len) :
        wxMemoryInputStream(data, len), m_data(data)
    {}
    ~wxOwnedMemInputStream()
    {
        free(m_data);
    }
};

bool  wxIEHtmlWin::LoadString(wxString html)
{
    char *data = NULL;
    size_t len = html.length();
#ifdef UNICODE
    len *= 2;
#endif
    data = (char *) malloc(len);
    memcpy(data, html.c_str(), len);
	return LoadStream(new wxOwnedMemInputStream(data, len));
};

bool wxIEHtmlWin::LoadStream(IStreamAdaptorBase *pstrm)
{
	wxAutoOleInterface<IStream>	strm(pstrm);

    // Document Interface
    IDispatch *pDisp = NULL;
    HRESULT hret = m_webBrowser->get_Document(&pDisp);
	if (! pDisp)
		return false;
	wxAutoOleInterface<IDispatch> disp(pDisp);


	// get IPersistStreamInit
    wxAutoOleInterface<IPersistStreamInit>
		pPersistStreamInit(IID_IPersistStreamInit, disp);

    if (pPersistStreamInit.Ok())
    {
        HRESULT hr = pPersistStreamInit->InitNew();
        if (SUCCEEDED(hr))
            hr = pPersistStreamInit->Load(strm);

		return SUCCEEDED(hr);
    }
	else
	    return false;
};

bool  wxIEHtmlWin::LoadStream(istream *is)
{
	// wrap reference around stream
    IStreamAdaptor *pstrm = new IStreamAdaptor(is);
	pstrm->AddRef();

    return LoadStream(pstrm);
};

bool wxIEHtmlWin::LoadStream(wxInputStream *is)
{
	// wrap reference around stream
    IwxStreamAdaptor *pstrm = new IwxStreamAdaptor(is);
	pstrm->AddRef();

    return LoadStream(pstrm);
};


bool wxIEHtmlWin::GoBack()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoBack();
    return hret == S_OK;
}

bool wxIEHtmlWin::GoForward()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoForward();
    return hret == S_OK;
}

bool wxIEHtmlWin::GoHome()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoHome();
    return hret == S_OK;
}

bool wxIEHtmlWin::GoSearch()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoSearch();
    return hret == S_OK;
}

bool wxIEHtmlWin::Refresh(wxIEHtmlRefreshLevel level)
{
    VARIANTARG levelArg;
    HRESULT hret = 0;

    levelArg.vt = VT_I2;
    levelArg.iVal = level;
    hret = m_webBrowser->Refresh2(&levelArg);
    return hret == S_OK;
}

bool wxIEHtmlWin::Stop()
{
    HRESULT hret = 0;
    hret = m_webBrowser->Stop();
    return hret == S_OK;
}


///////////////////////////////////////////////////////////////////////////////

static wxAutoOleInterface<IHTMLSelectionObject> GetSelObject(IOleObject *oleObject)
{
	// Query for IWebBrowser interface
    wxAutoOleInterface<IWebBrowser2> wb(IID_IWebBrowser2, oleObject);
    if (! wb.Ok())
    	return wxAutoOleInterface<IHTMLSelectionObject>();

	IDispatch *iDisp = NULL;
    HRESULT hr = wb->get_Document(&iDisp);
    if (hr != S_OK)
    	return wxAutoOleInterface<IHTMLSelectionObject>();

	// Query for Document Interface
    wxAutoOleInterface<IHTMLDocument2> hd(IID_IHTMLDocument2, iDisp);
    iDisp->Release();

    if (! hd.Ok())
    	return wxAutoOleInterface<IHTMLSelectionObject>();

    IHTMLSelectionObject *_so = NULL;
    hr = hd->get_selection(&_so);

    // take ownership of selection object
	wxAutoOleInterface<IHTMLSelectionObject> so(_so);

    return so;
};

static wxAutoOleInterface<IHTMLTxtRange> GetSelRange(IOleObject *oleObject)
{
	wxAutoOleInterface<IHTMLTxtRange> tr;

    wxAutoOleInterface<IHTMLSelectionObject> so(GetSelObject(oleObject));
    if (! so)
    	return tr;

	IDispatch *iDisp = NULL;
    HRESULT hr = so->createRange(&iDisp);
    if (hr != S_OK)
    	return tr;

	// Query for IHTMLTxtRange interface
	tr.QueryInterface(IID_IHTMLTxtRange, iDisp);
    iDisp->Release();
    return tr;
};


wxString wxIEHtmlWin::GetStringSelection(bool asHTML)
{
	wxAutoOleInterface<IHTMLTxtRange> tr(GetSelRange(m_oleObject));
    if (! tr)
    	return wxEmptyString;

    BSTR text = NULL;
    HRESULT hr = E_FAIL;

	if (asHTML)
		hr = tr->get_htmlText(&text);
	else
		hr = tr->get_text(&text);
    if (hr != S_OK)
    	return wxEmptyString;

    wxString s = text;
    SysFreeString(text);

    return s;
};

wxString wxIEHtmlWin::GetText(bool asHTML)
{
	if (! m_webBrowser.Ok())
		return wxEmptyString;

	// get document dispatch interface
	IDispatch *iDisp = NULL;
    HRESULT hr = m_webBrowser->get_Document(&iDisp);
    if (hr != S_OK)
    	return wxEmptyString;

	// Query for Document Interface
    wxAutoOleInterface<IHTMLDocument2> hd(IID_IHTMLDocument2, iDisp);
    iDisp->Release();

    if (! hd.Ok())
		return wxEmptyString;

	// get body element
	IHTMLElement *_body = NULL;
	hd->get_body(&_body);
	if (! _body)
		return wxEmptyString;
	wxAutoOleInterface<IHTMLElement> body(_body);

	// get inner text
    BSTR text = NULL;
    hr = E_FAIL;

	if (asHTML)
		hr = body->get_innerHTML(&text);
	else
		hr = body->get_innerText(&text);
    if (hr != S_OK)
    	return wxEmptyString;

    wxString s = text;
    SysFreeString(text);

    return s;
};
