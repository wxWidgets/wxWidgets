/////////////////////////////////////////////////////////////////////////////
// Name:        sckfile.cpp
// Purpose:     File protocol
// Author:      Guilhem Lavaux
// Modified by:
// Created:     20/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sckfile.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include <stdio.h>
#include <stdlib.h>
#include <wx/fstream.h>
#include "wx/protocol/file.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFileProto, wxProtocol)
IMPLEMENT_PROTOCOL(wxFileProto, "file", NULL, FALSE)
#endif

wxFileProto::wxFileProto()
  : wxProtocol()
{
}

wxFileProto::~wxFileProto()
{
}

wxInputStream *wxFileProto::GetInputStream(const wxString& path)
{
  return new wxFileInputStream(path);
}
