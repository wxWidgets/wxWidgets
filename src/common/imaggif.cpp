/////////////////////////////////////////////////////////////////////////////
// Name:        imaggif.cpp
// Purpose:     wxGIFHandler
// Author:      Vaclav Slavik & Guillermo Rodriguez Garcia
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik & Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
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

#if wxUSE_GIF

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

bool wxGIFHandler::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index) )
{
    wxGIFDecoder *decod;
    int error;
    bool ok;

    decod = new wxGIFDecoder(&stream, TRUE);

    if ((error = decod->ReadGIF()) != wxGIF_OK)
    {
        if (verbose)
        {
            switch (error)
            {
                case wxGIF_INVFORMAT: wxLogError(_("wxGIFHandler: error in GIF image format")); break;
                case wxGIF_MEMERR:    wxLogError(_("wxGIFHandler: couldn't allocate enough memory")); break;
                default:              wxLogError(_("wxGIFHandler: unknown error !!!"));
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
