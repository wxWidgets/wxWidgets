/////////////////////////////////////////////////////////////////////////////
// Name:        imagall.cpp
// Purpose:     wxImage access all handler
// Author:      Sylvain Bougnoux
// RCS-ID:      $Id$
// Copyright:   (c) Sylvain Bougnoux
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

#ifndef WX_PRECOMP
#  include "wx/setup.h"
#endif

#include "wx/image.h"

#if wxUSE_IMAGE

//-----------------------------------------------------------------------------
// This function allows dynamic access to all image handlers compile within
// the library. This function should be in a separate file as some compilers
// link against the whole object file as long as just one of is function is called!

void wxInitAllImageHandlers()
{
#if wxUSE_LIBPNG
  wxImage::AddHandler( new wxPNGHandler );
#endif
#if wxUSE_LIBJPEG
  wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_LIBTIFF
  wxImage::AddHandler( new wxTIFFHandler );
#endif
#if wxUSE_GIF
  wxImage::AddHandler( new wxGIFHandler );
#endif
#if wxUSE_PNM
  wxImage::AddHandler( new wxPNMHandler );
#endif
#if wxUSE_PCX
  wxImage::AddHandler( new wxPCXHandler );
#endif
#if wxUSE_XPM
  wxImage::AddHandler( new wxXPMHandler );
#endif
}

#endif // wxUSE_IMAGE
