/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/sckfile.cpp
// Purpose:     File protocol
// Author:      Guilhem Lavaux
// Created:     20/07/97
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STREAMS && wxUSE_PROTOCOL_FILE

#ifndef WX_PRECOMP
#endif

#include "wx/uri.h"
#include "wx/wfstream.h"
#include "wx/protocol/file.h"


// ----------------------------------------------------------------------------
// wxFileProto
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxFileProto, wxProtocol);
IMPLEMENT_PROTOCOL(wxFileProto, wxT("file"), nullptr, false)

wxFileProto::wxFileProto()
           : wxProtocol()
{
}

wxFileProto::~wxFileProto()
{
}

wxInputStream *wxFileProto::GetInputStream(const wxString& path)
{
    wxFileInputStream *retval = new wxFileInputStream(wxURI::Unescape(path));
    if ( retval->IsOk() )
    {
        m_lastError = wxPROTO_NOERR;
        return retval;
    }

    m_lastError = wxPROTO_NOFILE;
    delete retval;

    return nullptr;
}

#endif // wxUSE_STREAMS && wxUSE_PROTOCOL_FILE
