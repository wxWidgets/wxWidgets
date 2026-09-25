/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmlsvg.h
// Purpose:     Helper for loading SVG images from streams in wxHTML
// Author:      wxWidgets development team
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HTML_HTMLSVG_H_
#define _WX_HTML_HTMLSVG_H_

#include "wx/defs.h"

#include "wx/bmpbndl.h"
#include "wx/stream.h"

#if wxUSE_ZLIB
#    include "wx/zstream.h"
#endif

#if wxUSE_HTML && wxUSE_STREAMS

// ----------------------------------------------------------------------------
// wxHtmlLoadSVGBundle
// ----------------------------------------------------------------------------

// Return true if the given lower-case extension is that of an SVG image, i.e.
// "svg" or, if zlib support is available, "svgz", reading the image from the
// given stream into the bundle in this case.
//
// Notice that the stream is used for reading the image whenever this function
// returns true, even if reading failed and the bundle is invalid (the caller
// can and should check this using its IsOk() method), so it can't be reused
// for reading anything else in this case. If this function returns false, the
// stream was not read at all and the caller can still try to load any other
// kind of image from it.
inline bool wxHtmlLoadSVGBundle(wxInputStream& stream, const wxString& ext,
                                wxBitmapBundle& bundle)
{
#    ifdef wxHAS_SVG
#        if wxUSE_ZLIB
    if ( ext == "svgz" )
    {
        wxZlibInputStream zlibStream(stream);
        bundle = wxBitmapBundle::FromSVG(zlibStream, wxDefaultSize);
        return true;
    }
#        endif // wxUSE_ZLIB

    if ( ext == "svg" )
    {
        bundle = wxBitmapBundle::FromSVG(stream, wxDefaultSize);
        return true;
    }
#    else  // !wxHAS_SVG
    wxUnusedVar(stream);
    wxUnusedVar(ext);
    wxUnusedVar(bundle);
#    endif // wxHAS_SVG/!wxHAS_SVG

    return false;
}

#endif // wxUSE_HTML && wxUSE_STREAMS

#endif // _WX_HTML_HTMLSVG_H_
