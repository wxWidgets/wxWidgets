/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.cpp
// Purpose:     wxGIFHandler
// Author:      Vaclav Slavik & Guillermo Rodriguez Garcia
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
  We don't put pragma implement in this file because it is already present in
  src/common/image.cpp
*/

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#ifndef WX_PRECOMP
#  include "wx/defs.h"
#endif

#if 1 // change this to #if wxUSE_GIF

#include "wx/image.h"
#include "wx/gifdecod.h"
#include "wx/wfstream.h"
#include "wx/module.h"
#include "wx/log.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxGIFHandler,wxImageHandler)
#endif

#if wxUSE_STREAMS

//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

bool wxGIFHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose )
{
    wxGIFDecoder *decod;
    bool ok;

    decod = new wxGIFDecoder(&stream, TRUE);
    
    if (decod->ReadGIF() != E_OK)
    {
        if (verbose)
            wxLogDebug(_T("Error reading GIF"));

        delete decod;
        return FALSE;
    }

    image->Destroy();
    ok = decod->ConvertToImage(image);

    delete decod;
    return ok;
}

bool wxGIFHandler::SaveFile( wxImage * WXUNUSED(image),
                             wxOutputStream& WXUNUSED(stream), bool verbose )
{
    if (verbose)
        wxLogDebug(_T("wxGIFHandler is read-only!!"));

    return FALSE;
}

bool wxGIFHandler::CanRead( wxInputStream& stream )
{
    wxGIFDecoder *decod;
    bool ok;

    decod = new wxGIFDecoder(&stream);
    ok = decod->CanRead();

    delete decod;
    return ok;
}

#endif
  // wxUSE_STREAMS

#endif
  // wxUSE_GIF
