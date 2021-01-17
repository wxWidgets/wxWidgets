/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/image_wic.h
// Purpose:     Implementation of wxImageHandler using
//              Windows Imaging Component
// Author:      Tobias Taschner
// Created:     2021-01-15
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGE_WIC_H_
#define _WX_IMAGE_WIC_H_

//TODO: Move to defs
#define wxUSE_WIC 1 

#include "wx/defs.h"

#ifdef wxUSE_WIC

#include "wx/image.h"

class WXDLLIMPEXP_CORE wxImageHandlerWIC : public wxImageHandler
{
public:
    wxImageHandlerWIC() { }
    wxImageHandlerWIC(wxBitmapType bitmapType);

    static void InitAllSupportedHandlers();

#if wxUSE_STREAMS
    virtual bool LoadFile(wxImage *image, wxInputStream& stream, bool verbose = true, int index = -1) wxOVERRIDE;
    virtual bool SaveFile(wxImage *image, wxOutputStream& stream, bool verbose = true) wxOVERRIDE;
protected:
    virtual bool DoCanRead(wxInputStream& stream) wxOVERRIDE;
#endif

private:
    const GUID* m_containerFormat;

    wxDECLARE_DYNAMIC_CLASS(wxImageHandlerWIC);
};


#endif // wxUSE_WIC

#endif // _WX_IMAGE_WIC_H_
