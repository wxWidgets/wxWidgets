/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.cpp
// Purpose:     wxGIFHandler
// Author:      Vaclav Slavik & Guillermo Rodriguez Garcia
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik & Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "imaggif.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#ifndef WX_PRECOMP
#  include "wx/defs.h"
#endif

#if wxUSE_IMAGE && wxUSE_GIF

#include "wx/imaggif.h"
#include "wx/gifdecod.h"
#include "wx/wfstream.h"
#include "wx/log.h"
#include "wx/intl.h"

IMPLEMENT_DYNAMIC_CLASS(wxGIFHandler,wxImageHandler)

//-----------------------------------------------------------------------------
// wxGIFHandler
//-----------------------------------------------------------------------------

#if wxUSE_STREAMS

bool wxGIFHandler::LoadFile(wxImage *image, wxInputStream& stream,
                            bool verbose, int index)
{
    wxGIFDecoder *decod;
    int error;
    bool ok = true;

//    image->Destroy();
    decod = new wxGIFDecoder(&stream, true);
    error = decod->ReadGIF();

    if ((error != wxGIF_OK) && (error != wxGIF_TRUNCATED))
    {
        if (verbose)
        {
            switch (error)
            {
                case wxGIF_INVFORMAT:
                    wxLogError(_("GIF: error in GIF image format."));
                    break;
                case wxGIF_MEMERR:
                    wxLogError(_("GIF: not enough memory."));
                    break;
                default:
                    wxLogError(_("GIF: unknown error!!!"));
                    break;
            }
        }
        delete decod;
        return false;
    }

    if ((error == wxGIF_TRUNCATED) && verbose)
    {
        wxLogError(_("GIF: data stream seems to be truncated."));
        /* go on; image data is OK */
    }

    if (index != -1)
    {
        // We're already on index = 0 by default. So no need
        // to call GoFrame(0) then. On top of that GoFrame doesn't
        // accept an index of 0. (Instead GoFirstFrame() should be used)
        // Also if the gif image has only one frame, calling GoFrame(0)
        // fails because GoFrame() only works with gif animations.
        // (It fails if IsAnimation() returns false)
        // All valid reasons to NOT call GoFrame when index equals 0.
        if (index != 0)
        {
            ok = decod->GoFrame(index);
        }
    }

    if (ok)
    {
        ok = decod->ConvertToImage(image);
    }
    else
    {
        wxLogError(_("GIF: Invalid gif index."));
    }

    delete decod;

    return ok;
}

bool wxGIFHandler::SaveFile( wxImage * WXUNUSED(image),
                             wxOutputStream& WXUNUSED(stream), bool verbose )
{
    if (verbose)
        wxLogDebug(wxT("GIF: the handler is read-only!!"));

    return false;
}

bool wxGIFHandler::DoCanRead( wxInputStream& stream )
{
    wxGIFDecoder decod(&stream);
    return decod.CanRead();
}

#endif  // wxUSE_STREAMS

#endif  // wxUSE_GIF
