/////////////////////////////////////////////////////////////////////////////
// Name:        xpmdecod.h
// Purpose:     wxXPMDecoder, XPM reader for wxImage and wxBitmap
// Author:      Vaclav Slavik
// CVS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XPMDECOD_H_
#define _WX_XPMDECOD_H_

#ifdef __GNUG__
#pragma interface "xpmdecod.h"
#endif

#include "wx/setup.h"


class WXDLLEXPORT wxImage;
class WXDLLEXPORT wxInputStream;

// --------------------------------------------------------------------------
// wxXPMDecoder class
// --------------------------------------------------------------------------

class WXDLLEXPORT wxXPMDecoder
{
public:
    // constructor, destructor, etc.
    wxXPMDecoder() {}
    ~wxXPMDecoder() {}

#if wxUSE_STREAMS
    // Is the stream XPM file?
    bool CanRead(wxInputStream& stream);
    // Read XPM file from the stream, parse it and create image from it
    wxImage ReadFile(wxInputStream& stream);
#endif
    // Read directly from XPM data (as passed to wxBitmap ctor):
    wxImage ReadData(const char **xpm_data);
};


#endif  // _WX_GIFDECOD_H_

