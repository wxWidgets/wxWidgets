/////////////////////////////////////////////////////////////////////////////
// Name:        metafile.cpp
// Purpose:     wxMetaFile, wxMetaFileDC etc. These classes are optional.
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "metafile.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

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

extern bool wxClipboardIsOpen;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMetafile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetafileDC, wxDC)
#endif

class wxMetafileRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxMetafile;
public:
    wxMetafileRefData(void);
    ~wxMetafileRefData(void);

private:
    PicHandle m_metafile;
#if wxMAC_USE_CORE_GRAPHICS
    QDPictRef m_qdPictRef ;
#endif
};


/*
 * Metafiles
 * Currently, the only purpose for making a metafile is to put
 * it on the clipboard.
 */

wxMetafileRefData::wxMetafileRefData(void)
{
    m_metafile = 0;
#if wxMAC_USE_CORE_GRAPHICS
    m_qdPictRef = NULL ;
#endif
}

wxMetafileRefData::~wxMetafileRefData(void)
{
    if (m_metafile)
    {
        KillPicture( (PicHandle) m_metafile ) ;
        m_metafile = 0;
#if wxMAC_USE_CORE_GRAPHICS
        QDPictRelease( m_qdPictRef ) ;
        m_qdPictRef = NULL ;
#endif
    }
}

wxMetaFile::wxMetaFile(const wxString& file)
{
    m_refData = new wxMetafileRefData;

    M_METAFILEDATA->m_metafile = 0;
    wxASSERT_MSG( file.IsEmpty() , wxT("no file based metafile support yet") ) ;
/*
    if (!file.IsNull() && (file.Cmp("") == 0))
        M_METAFILEDATA->m_metafile = (WXHANDLE) GetMetaFile(file);
*/
}

wxMetaFile::~wxMetaFile()
{
}

bool wxMetaFile::Ok() const 
{ 
    return (M_METAFILEDATA && (M_METAFILEDATA->m_metafile != 0)); 
}

WXHMETAFILE wxMetaFile::GetHMETAFILE() const 
{ 
    return (WXHMETAFILE) M_METAFILEDATA->m_metafile; 
}

bool wxMetaFile::SetClipboard(int width, int height)
{
#if wxUSE_DRAG_AND_DROP
    //TODO finishi this port , we need the data obj first
    if (!m_refData)
        return FALSE;
    
    bool alreadyOpen=wxTheClipboard->IsOpened() ;
    if (!alreadyOpen)
    {
        wxTheClipboard->Open();
        wxTheClipboard->Clear();
    }
    wxDataObject *data =
        new wxMetafileDataObject( *this) ;
    bool success = wxTheClipboard->SetData(data);
    if (!alreadyOpen) 
        wxTheClipboard->Close();
    return (bool) success;
#endif
    return TRUE ;
}

void wxMetafile::SetHMETAFILE(WXHMETAFILE mf)
{
    UnRef() ;
    
    m_refData = new wxMetafileRefData;

    M_METAFILEDATA->m_metafile = (PicHandle) mf;
#if wxMAC_USE_CORE_GRAPHICS
    size_t sz = GetHandleSize( (Handle) M_METAFILEDATA->m_metafile ) ;
    wxMemoryBuffer* membuf = new wxMemoryBuffer( sz ) ;
    void * data = membuf->GetWriteBuf(sz) ;
    memcpy( data , *M_METAFILEDATA->m_metafile , sz ) ;
    membuf->UngetWriteBuf(sz) ;
    CGDataProviderRef provider = CGDataProviderCreateWithData( membuf , data , sz ,
        wxMacMemoryBufferReleaseProc ) ;
    M_METAFILEDATA->m_qdPictRef = NULL ;
    if ( provider != NULL )
    {
        M_METAFILEDATA->m_qdPictRef = QDPictCreateWithProvider( provider ) ;
        CGDataProviderRelease( provider ) ;
    }
#endif
}

bool wxMetaFile::Play(wxDC *dc)
{
    if (!m_refData)
        return FALSE;
    
    if (!dc->Ok() )
        return FALSE;
        
    {
        PicHandle pict = (PicHandle) GetHMETAFILE() ;
#if wxMAC_USE_CORE_GRAPHICS
        QDPictRef cgPictRef = M_METAFILEDATA->m_qdPictRef ;
        CGContextRef cg = dynamic_cast<wxMacCGContext*>(dc->GetGraphicContext())->GetNativeContext() ;
        CGRect bounds = QDPictGetBounds( cgPictRef ) ;

        CGContextSaveGState(cg);    
        CGContextTranslateCTM(cg, 0 , bounds.size.width );
        CGContextScaleCTM(cg, 1, -1);
        QDPictDrawToCGContext( cg , bounds , cgPictRef ) ;
        CGContextRestoreGState( cg ) ;
#else
        wxMacPortSetter helper( dc ) ;
        DrawPicture( pict , &(**pict).picFrame ) ;
#endif
    }
    return TRUE;
}

wxSize wxMetaFile::GetSize() const
{
    wxSize size = wxDefaultSize ;
    if ( Ok() )
    {
        PicHandle pict = (PicHandle) GetHMETAFILE() ;
        Rect &r = (**pict).picFrame ;
        size.x = r.right - r.left ;
        size.y = r.bottom - r.top ;
    }

    return size;
}

/*
 * Metafile device context
 *
 */

// New constructor that takes origin and extent. If you use this, don't
// give origin/extent arguments to wxMakeMetaFilePlaceable.

wxMetaFileDC::wxMetaFileDC(const wxString& filename ,
                    int width , int height ,
                    const wxString& WXUNUSED(description) )
{
    wxASSERT_MSG( width == 0 || height == 0 , _T("no arbitration of metafilesize supported") ) ;
    wxASSERT_MSG( filename.IsEmpty() , _T("no file based metafile support yet")) ;
    
    m_metaFile = new wxMetaFile(filename) ;
#if wxMAC_USE_CORE_GRAPHICS
#else
    Rect r={0,0,height,width} ;
    
    RectRgn( (RgnHandle) m_macBoundaryClipRgn , &r ) ;
    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;

    m_metaFile->SetHMETAFILE( (WXHMETAFILE) OpenPicture( &r ) ) ;
    ::GetPort( (GrafPtr*) &m_macPort ) ;    
    m_ok = TRUE ;
#endif
    SetMapMode(wxMM_TEXT); 
}

wxMetaFileDC::~wxMetaFileDC()
{
}

void wxMetaFileDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_metaFile , _T("GetSize() doesn't work without a metafile") );

    wxSize sz = m_metaFile->GetSize() ;
    if (width) (*width) = sz.x;
    if (height) (*height) = sz.y;
}

wxMetaFile *wxMetaFileDC::Close()
{
    ClosePicture() ;
    return m_metaFile;
}

#if wxUSE_DATAOBJ
size_t wxMetafileDataObject::GetDataSize() const
{
    return GetHandleSize( (Handle) (*((wxMetafile*)&m_metafile)).GetHMETAFILE() ) ;
}

bool wxMetafileDataObject::GetDataHere(void *buf) const
{
    memcpy( buf , (*(PicHandle)(*((wxMetafile*)&m_metafile)).GetHMETAFILE()) ,
        GetHandleSize( (Handle) (*((wxMetafile*)&m_metafile)).GetHMETAFILE() ) ) ;
    return true ;
}

bool wxMetafileDataObject::SetData(size_t len, const void *buf)
{
    Handle handle = NewHandle( len ) ;
    SetHandleSize( handle , len ) ;
    memcpy( *handle , buf , len ) ;
    m_metafile.SetHMETAFILE( (WXHMETAFILE) handle ) ;
    return true ;
}
#endif

#endif
