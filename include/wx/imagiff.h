//
// imgiff.h	- image handler for Amiga IFF images
// 		  parts of the source taken by xv source code.
//
// (c) Steffen Gutmann, 2002
//
// Creation date: 08.01.2002
// Last modified: 08.01.2002
//

#ifndef WX_IMAGE_IFF_H
#define WX_IMAGE_IFF_H

#ifdef __GNUG__
#pragma interface "imagiff.h"
#endif

#include "wx/image.h"
#define wxUSE_IFF 1

//-----------------------------------------------------------------------------
// wxIFFHandler
//-----------------------------------------------------------------------------

#if wxUSE_IFF

class WXDLLEXPORT wxIFFHandler : public wxImageHandler
{
    DECLARE_DYNAMIC_CLASS(wxIFFHandler)

public:

    wxIFFHandler()
    {
	m_name = "IFF file";
	m_extension = "iff";
  	m_type = wxBITMAP_TYPE_ANY;
    	// m_mime = "image/iff";
    };

#if wxUSE_STREAMS
    virtual bool LoadFile(wxImage *image, wxInputStream& stream, 
	    bool verbose=TRUE, int index=0);
    virtual bool SaveFile(wxImage *image, wxOutputStream& stream, 
	    bool verbose=TRUE);
    virtual bool DoCanRead(wxInputStream& stream);
#endif
};
#endif

#endif

