//
// imgiff.cc    - image handler for Amiga IFF images
//        parts of the source taken by xv source code.
//
// (c) Steffen Gutmann, 2002
//
// Creation date: 08.01.2002
// Last modified: 08.01.2002
//

#ifdef __GNUG__
#pragma implementation "imagiff.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#ifndef WX_PRECOMP
#  include "wx/defs.h"
#endif

#include "wx/imagiff.h"
#include "wx/iffdecod.h"
#include "wx/wfstream.h"
#include "wx/log.h"
#include "wx/intl.h"

#if wxUSE_IMAGE && wxUSE_IFF

IMPLEMENT_DYNAMIC_CLASS(wxIFFHandler, wxImageHandler)

//-----------------------------------------------------------------------------
// wxIFFHandler
//-----------------------------------------------------------------------------

#if wxUSE_STREAMS

bool wxIFFHandler::LoadFile(wxImage *image, wxInputStream& stream, 
    bool verbose, int WXUNUSED(index))
{
    wxIFFDecoder *decod;
    int error;
    bool ok;

    decod = new wxIFFDecoder(&stream);
    error = decod->ReadIFF();

    if ((error != wxIFF_OK) && (error != wxIFF_TRUNCATED))
    {
        if (verbose)
        {
            switch (error)
            {
                case wxIFF_INVFORMAT:
                    wxLogError(_("IFF: error in IFF image format."));
                    break;
                case wxIFF_MEMERR:
                    wxLogError(_("IFF: not enough memory."));
                    break;
                default:
                    wxLogError(_("IFF: unknown error!!!"));
                    break;
            }
        }
        delete decod;
        return FALSE;
    }

    if ((error == wxIFF_TRUNCATED) && verbose)
    {
        wxLogError(_("IFF: data stream seems to be truncated."));
        /* go on; image data is OK */
    }

    ok = decod->ConvertToImage(image);
    delete decod;

    return ok;
}

bool wxIFFHandler::SaveFile(wxImage * WXUNUSED(image),
    wxOutputStream& WXUNUSED(stream), bool verbose)
{
    if (verbose)
        wxLogDebug(wxT("IFF: the handler is read-only!!"));

    return FALSE;
}

bool wxIFFHandler::DoCanRead(wxInputStream& stream)
{
    wxIFFDecoder *decod;
    bool ok;

    decod = new wxIFFDecoder(&stream);
    ok = decod->CanRead();
    delete decod;

    return ok;
}

#endif  
#endif  

