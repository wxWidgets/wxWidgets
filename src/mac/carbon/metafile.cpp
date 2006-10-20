/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/metafile.cpp
// Purpose:     wxMetaFile, wxMetaFileDC etc. These classes are optional.
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////
//
// Currently, the only purpose for making a metafile
// is to put it on the clipboard.


#include "wx/wxprec.h"

#if wxUSE_METAFILE

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/metafile.h"
#include "wx/clipbrd.h"
#include "wx/mac/private.h"

#include <stdio.h>
#include <string.h>

IMPLEMENT_DYNAMIC_CLASS(wxMetafile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetafileDC, wxDC)

class wxMetafileRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxMetafile;

public:
    wxMetafileRefData();
    virtual ~wxMetafileRefData();

private:
    PicHandle m_metafile;

#if wxMAC_USE_CORE_GRAPHICS
    QDPictRef m_qdPictRef;
#endif
};

wxMetafileRefData::wxMetafileRefData()
{
    m_metafile = NULL;

#if wxMAC_USE_CORE_GRAPHICS
    m_qdPictRef = NULL;
#endif
}

wxMetafileRefData::~wxMetafileRefData()
{
    if (m_metafile)
    {
#ifndef __LP64__
        KillPicture( (PicHandle)m_metafile );
        m_metafile = NULL;

#if wxMAC_USE_CORE_GRAPHICS
        QDPictRelease( m_qdPictRef );
        m_qdPictRef = NULL;
#endif
#endif
    }
}

wxMetaFile::wxMetaFile(const wxString& file)
{
    m_refData = new wxMetafileRefData;

    M_METAFILEDATA->m_metafile = NULL;
    wxASSERT_MSG( file.empty(), wxT("no file-based metafile support yet") );

#if 0
    if (!file.IsNull() && (file.Cmp("") == 0))
        M_METAFILEDATA->m_metafile = (WXHANDLE) GetMetaFile( file );
#endif
}

wxMetaFile::~wxMetaFile()
{
}

bool wxMetaFile::IsOk() const
{
    return (M_METAFILEDATA && (M_METAFILEDATA->m_metafile != NULL));
}

WXHMETAFILE wxMetaFile::GetHMETAFILE() const
{
    return (WXHMETAFILE) M_METAFILEDATA->m_metafile;
}

bool wxMetaFile::SetClipboard(int width, int height)
{
    bool success = true;

#if wxUSE_DRAG_AND_DROP
    // TODO: to finish this port, we need the data object first
    if (m_refData == NULL)
        return false;

    bool alreadyOpen = wxTheClipboard->IsOpened();
    if (!alreadyOpen)
    {
        wxTheClipboard->Open();
        wxTheClipboard->Clear();
    }

    wxDataObject *data = new wxMetafileDataObject( *this );
    success = wxTheClipboard->SetData( data );
    if (!alreadyOpen)
        wxTheClipboard->Close();
#endif

    return success;
}

void wxMetafile::SetHMETAFILE(WXHMETAFILE mf)
{
    UnRef();

    m_refData = new wxMetafileRefData;

    M_METAFILEDATA->m_metafile = (PicHandle)mf;

#if wxMAC_USE_CORE_GRAPHICS
    size_t sz = GetHandleSize( (Handle) M_METAFILEDATA->m_metafile );
    wxMemoryBuffer* membuf = new wxMemoryBuffer( sz );
    void *data = membuf->GetWriteBuf( sz );

    memcpy( data, *M_METAFILEDATA->m_metafile, sz );
    membuf->UngetWriteBuf( sz );
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        membuf, data, sz, wxMacMemoryBufferReleaseProc );
    M_METAFILEDATA->m_qdPictRef = NULL;

#ifndef __LP64__
    if (provider != NULL)
    {
        M_METAFILEDATA->m_qdPictRef = QDPictCreateWithProvider( provider );
        CGDataProviderRelease( provider );
    }
#endif
#endif
}

bool wxMetaFile::Play(wxDC *dc)
{
    if (!m_refData)
        return false;

    if (!dc->Ok())
        return false;

    {
#if wxMAC_USE_CORE_GRAPHICS
#ifndef __LP64__
        QDPictRef cgPictRef = M_METAFILEDATA->m_qdPictRef;
        CGContextRef cg = (CGContextRef) dc->GetGraphicContext()->GetNativeContext();
        CGRect bounds = QDPictGetBounds( cgPictRef );

        CGContextSaveGState( cg );
        CGContextTranslateCTM( cg, 0, bounds.size.width );
        CGContextScaleCTM( cg, 1, -1 );
        QDPictDrawToCGContext( cg, bounds, cgPictRef );
        CGContextRestoreGState( cg );
#endif
#else
        PicHandle pict = (PicHandle)GetHMETAFILE();
        wxMacPortSetter helper( dc );
        Rect picFrame;
        DrawPicture( pict, wxMacGetPictureBounds( pict, &picFrame ) );
#endif
    }

    return true;
}

wxSize wxMetaFile::GetSize() const
{
    wxSize dataSize = wxDefaultSize;

    if (Ok())
    {
#ifndef __LP64__
       PicHandle pict = (PicHandle)GetHMETAFILE();
        Rect r;
        wxMacGetPictureBounds( pict, &r );
        dataSize.x = r.right - r.left;
        dataSize.y = r.bottom - r.top;
#endif
    }

    return dataSize;
}

// Metafile device context

// New constructor that takes origin and extent. If you use this, don't
// give origin/extent arguments to wxMakeMetaFilePlaceable.

wxMetaFileDC::wxMetaFileDC(
    const wxString& filename,
    int width, int height,
    const wxString& WXUNUSED(description) )
{
    wxASSERT_MSG( width <= 0 || height <= 0, wxT("no arbitration of metafile size supported") );
    wxASSERT_MSG( filename.empty(), wxT("no file based metafile support yet"));

    m_metaFile = new wxMetaFile( filename );

#if wxMAC_USE_CORE_GRAPHICS
#else
    Rect r = { 0, 0, height, width };

    RectRgn( (RgnHandle)m_macBoundaryClipRgn, &r );
    CopyRgn( (RgnHandle)m_macBoundaryClipRgn, (RgnHandle)m_macCurrentClipRgn );

    m_metaFile->SetHMETAFILE( (WXHMETAFILE)OpenPicture( &r ) );
    ::GetPort( (GrafPtr*)&m_macPort );

    m_ok = true;
#endif

    SetMapMode( wxMM_TEXT );
}

wxMetaFileDC::~wxMetaFileDC()
{
}

void wxMetaFileDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_metaFile, wxT("GetSize() doesn't work without a metafile") );

    wxSize sz = m_metaFile->GetSize();
    if (width)
        (*width) = sz.x;
    if (height)
        (*height) = sz.y;
}

wxMetaFile *wxMetaFileDC::Close()
{
#ifndef __LP64__
    ClosePicture();
#endif
    return m_metaFile;
}

#if wxUSE_DATAOBJ
size_t wxMetafileDataObject::GetDataSize() const
{
    return GetHandleSize( (Handle) (*((wxMetafile*)&m_metafile)).GetHMETAFILE() );
}

bool wxMetafileDataObject::GetDataHere(void *buf) const
{
    Handle pictH = (Handle)(*((wxMetafile*)&m_metafile)).GetHMETAFILE();
    bool result = (pictH != NULL);

    if (result)
        memcpy( buf, *pictH, GetHandleSize( pictH ) );

    return result;
}

bool wxMetafileDataObject::SetData(size_t len, const void *buf)
{
    Handle handle = NewHandle( len );
    SetHandleSize( handle, len );
    memcpy( *handle, buf, len );
    m_metafile.SetHMETAFILE( (WXHMETAFILE) handle );

    return true;
}
#endif

#endif
