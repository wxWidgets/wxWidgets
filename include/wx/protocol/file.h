/////////////////////////////////////////////////////////////////////////////
// Name:        file.h
// Purpose:     File protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_PROTO_FILE_H__
#define __WX_PROTO_FILE_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "sckfile.h"
#endif

#include "wx/defs.h"

#if wxUSE_PROTOCOL_FILE

#include "wx/protocol/protocol.h"
#include "wx/url.h"

class WXDLLIMPEXP_BASE wxFileProto: public wxProtocol {
  DECLARE_DYNAMIC_CLASS(wxFileProto)
  DECLARE_PROTOCOL(wxFileProto)
protected:
  wxProtocolError m_error;
public:
  wxFileProto();
  ~wxFileProto();

  wxProtocolError GetError() { return m_error; }
  bool Abort() { return TRUE; }
  wxInputStream *GetInputStream(const wxString& path);
};

#endif // wxUSE_PROTOCOL_FILE

#endif // __WX_PROTO_FILE_H__
