/*
                wxActiveX Library Licence, Version 3
                ====================================

  Copyright (C) 2003 Lindsay Mathieson [, ...]

  Everyone is permitted to copy and distribute verbatim copies
  of this licence document, but changing it is not allowed.

                       wxActiveX LIBRARY LICENCE
     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
  
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public Licence as published by
  the Free Software Foundation; either version 2 of the Licence, or (at
  your option) any later version.
  
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
  General Public Licence for more details.

  You should have received a copy of the GNU Library General Public Licence
  along with this software, usually in a file named COPYING.LIB.  If not,
  write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA 02111-1307 USA.

  EXCEPTION NOTICE

  1. As a special exception, the copyright holders of this library give
  permission for additional uses of the text contained in this release of
  the library as licenced under the wxActiveX Library Licence, applying
  either version 3 of the Licence, or (at your option) any later version of
  the Licence as published by the copyright holders of version 3 of the
  Licence document.

  2. The exception is that you may use, copy, link, modify and distribute
  under the user's own terms, binary object code versions of works based
  on the Library.

  3. If you copy code from files distributed under the terms of the GNU
  General Public Licence or the GNU Library General Public Licence into a
  copy of this library, as this licence permits, the exception does not
  apply to the code that you add in this way.  To avoid misleading anyone as
  to the status of such modified files, you must delete this exception
  notice from such code and/or adjust the licensing conditions notice
  accordingly.

  4. If you write modifications of your own for this library, it is your
  choice whether to permit this exception to apply to your modifications. 
  If you do not wish that, you must delete the exception notice from such
  code and/or adjust the licensing conditions notice accordingly.
*/

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
#include <IEHtmlStream.h>
using namespace std;



//////////////////////////////////////////////////////////////////////
// Stream adapters and such

HRESULT STDMETHODCALLTYPE IStreamAdaptorBase::Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead)
{
    if (prepend.size() > 0)
    {
        int n = wxMin(prepend.size(), cb);
        prepend.copy((char *) pv, n);
        prepend = prepend.substr(n);
        if (pcbRead)
            *pcbRead = n;

        return S_OK;
    };

    int rc = Read((char *) pv, cb);
    if (pcbRead)
        *pcbRead = rc;

    return S_OK;
};

DEFINE_OLE_TABLE(IStreamAdaptorBase)
    OLE_IINTERFACE(IUnknown)
    OLE_IINTERFACE(ISequentialStream)
    OLE_IINTERFACE(IStream)
END_OLE_TABLE;


IStreamAdaptor::IStreamAdaptor(istream *is)
    : IStreamAdaptorBase(), m_is(is)
{
    wxASSERT(m_is != NULL);
}

IStreamAdaptor::~IStreamAdaptor()   
{
    delete m_is;
}

int IStreamAdaptor::Read(char *buf, int cb)
{
    m_is->read(buf, cb);
    return m_is->gcount();
}


IwxStreamAdaptor::IwxStreamAdaptor(wxInputStream *is)
    : IStreamAdaptorBase(), m_is(is) 
{
    wxASSERT(m_is != NULL);
}
IwxStreamAdaptor::~IwxStreamAdaptor() 
{
    delete m_is;
}

// ISequentialStream
int IwxStreamAdaptor::Read(char *buf, int cb)
{
    m_is->Read(buf, cb);
    return m_is->LastRead();
};

wxOwnedMemInputStream::wxOwnedMemInputStream(char *data, size_t len)
    :  wxMemoryInputStream(data, len), m_data(data)
{}

wxOwnedMemInputStream::~wxOwnedMemInputStream() 
{
    free(m_data);
}


//////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxIEHtmlWin, wxActiveX)
END_EVENT_TABLE()


static const CLSID CLSID_MozillaBrowser =
{ 0x1339B54C, 0x3453, 0x11D2,
  { 0x93, 0xB9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00 } };


//#define PROGID "Shell.Explorer"
#define PROGID CLSID_WebBrowser
//#define PROGID CLSID_MozillaBrowser
//#define PROGID CLSID_HTMLDocument
//#define PROGID "MSCAL.Calendar"
//#define PROGID ""
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


void wxIEHtmlWin::SetCharset(const wxString& charset)
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


bool  wxIEHtmlWin::LoadString(const wxString& html)
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
    // need to prepend this as poxy MSHTML will not recognise a HTML comment
    // as starting a html document and treats it as plain text
    // Does nayone know how to force it to html mode ?
    pstrm->prepend = "<html>";

    // strip leading whitespace as it can confuse MSHTML
    wxAutoOleInterface<IStream> strm(pstrm);

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
    try
    {
        CallMethod(_T("GoHome"));
        return true;
    }
    catch(exception&)
    {
        return false;
    };
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

wxAutoOleInterface<IHTMLTxtRange> wxieGetSelRange(IOleObject *oleObject)
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
    wxAutoOleInterface<IHTMLTxtRange> tr(wxieGetSelRange(m_oleObject));
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
