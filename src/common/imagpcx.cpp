/////////////////////////////////////////////////////////////////////////////
// Name:        imagpcx.cpp
// Purpose:     wxImage PCX handler
// Author:      Guillermo Rodriguez Garcia <guille@iies.es>
// Version:     1.00
// Last rev:    1999/08/24
// Copyright:   (c) Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
   We don't put pragma implement in this file because it is already present in
   src/common/image.cpp
*/

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/image.h"
#include "wx/wfstream.h"
#include "wx/module.h"
#include "wx/log.h"

//-----------------------------------------------------------------------------
// wxPCXHandler
//-----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPCXHandler,wxImageHandler)
#endif

#if wxUSE_STREAMS

bool wxPCXHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose )
{
    image->Destroy();

    if (verbose)
        wxLogDebug(_T("wxPCXHandler::LoadFile still not implemented"));

    return FALSE;
}

bool wxPCXHandler::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose )
{
    if (verbose)
        wxLogDebug(_T("wxPCXHandler::SaveFile still not implemented"));

    return FALSE;
}

bool wxPCXHandler::CanRead( wxInputStream& stream )
{
    return FALSE;
}


#endif // wxUSE_STREAMS


