/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.cpp
// Purpose:     wxGIFHandler
// Author:      Vaclav Slavik & Guillermo Rodriguez Garcia
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik & Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
  We don't put pragma implement in this file because it is already present in
  src/common/image.cpp
*/

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#ifndef WX_PRECOMP
#  include "wx/defs.h"
#endif

#if wxUSE_GIF

#include "wx/image.h"
#include "wx/gifdecod.h"
#include "wx/wfstream.h"
#include "wx/log.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxGIFHandler,wxImageHandler)
#endif

//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

#if wxUSE_STREAMS

bool wxGIFHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index) )
{
    wxGIFDecoder *decod;
    int error;
    bool ok;

    decod = new wxGIFDecoder(&stream, TRUE);

    if ((error = decod->ReadGIF()) != E_OK)
    {
        if (verbose)
        {
            switch (error)
            {
                case E_FORMATO: wxLogError(_("GIF: Error in image format.")); break;
                case E_MEMORIA: wxLogError(_("GIF: Couldn't allocate memory.")); break;
                default:        wxLogError(_("GIF: Unknown error."));
            }
        }
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
        wxLogDebug(wxT("wxGIFHandler is read-only!!"));

    return FALSE;
}

bool wxGIFHandler::DoCanRead( wxInputStream& stream )
{
    wxGIFDecoder *decod;
    bool ok;

    decod = new wxGIFDecoder(&stream);
    ok = decod->CanRead();

    delete decod;
    return ok;
}

#endif  // wxUSE_STREAMS

#endif  // wxUSE_GIF
