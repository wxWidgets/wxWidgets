/////////////////////////////////////////////////////////////////////////////
// Name:        imagtiff.h
// Purpose:     wxImage TIFF handler
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGTIFF_H_
#define _WX_IMAGTIFF_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "imagtiff.h"
#endif

#include "wx/defs.h"

//-----------------------------------------------------------------------------
// wxTIFFHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBTIFF

#include "wx/image.h"

// defines for wxImage::SetOption
#define wxIMAGE_OPTION_BITSPERSAMPLE               wxString(_T("BitsPerSample"))
#define wxIMAGE_OPTION_SAMPLESPERPIXEL             wxString(_T("SamplesPerPixel"))
#define wxIMAGE_OPTION_COMPRESSION                 wxString(_T("Compression"))
#define wxIMAGE_OPTION_IMAGEDESCRIPTOR             wxString(_T("ImageDescriptor"))

class WXDLLEXPORT wxTIFFHandler: public wxImageHandler
{
public:
    wxTIFFHandler();

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );
    virtual bool DoCanRead( wxInputStream& stream );
    virtual int GetImageCount( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxTIFFHandler)
};

#endif // wxUSE_LIBTIFF

#endif // _WX_IMAGTIFF_H_

