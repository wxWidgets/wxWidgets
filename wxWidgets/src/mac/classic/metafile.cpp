/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/metafile.cpp
// Purpose:     wxMetaFile, wxMetaFileDC etc. These classes are optional.
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_METAFILE

#include "wx/metafile.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/clipbrd.h"

#include "wx/mac/private.h"

#include <stdio.h>
#include <string.h>

extern bool wxClipboardIsOpen;

IMPLEMENT_DYNAMIC_CLASS(wxMetafile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetafileDC, wxDC)

/*
 * Metafiles
 * Currently, the only purpose for making a metafile is to put
 * it on the clipboard.
 */

wxMetafileRefData::wxMetafileRefData(void)
{
    m_metafile = 0;
}

wxMetafileRefData::~wxMetafileRefData(void)
{
    if (m_metafile)
    {
        KillPicture( (PicHandle) m_metafile ) ;
        m_metafile = 0;
    }
}

wxMetaFile::wxMetaFile(const wxString& file)
{
    m_refData = new wxMetafileRefData;


    M_METAFILEDATA->m_metafile = 0;
    wxASSERT_MSG( file.IsEmpty() , wxT("no file based metafile support yet") ) ;
/*
    if (!file.empty())
        M_METAFILEDATA->m_metafile = (WXHANDLE) GetMetaFile(file);
*/
}

wxMetaFile::~wxMetaFile()
{
}

bool wxMetaFile::SetClipboard(int width, int height)
{
#if wxUSE_DRAG_AND_DROP
    //TODO finishi this port , we need the data obj first
    if (!m_refData)
        return false;

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
    return true ;
}

void wxMetafile::SetHMETAFILE(WXHMETAFILE mf)
{
    if (!m_refData)
        m_refData = new wxMetafileRefData;
    if ( M_METAFILEDATA->m_metafile )
        KillPicture( (PicHandle) M_METAFILEDATA->m_metafile ) ;

    M_METAFILEDATA->m_metafile = mf;
}

bool wxMetaFile::Play(wxDC *dc)
{
    if (!m_refData)
        return false;

    if (!dc->Ok() )
        return false;

    {
        wxMacPortSetter helper( dc ) ;
        PicHandle pict = (PicHandle) GetHMETAFILE() ;
        DrawPicture( pict , &(**pict).picFrame ) ;
    }
    return true;
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
    wxASSERT_MSG( filename.empty() , _T("no file based metafile support yet")) ;

    m_metaFile = new wxMetaFile(filename) ;
    Rect r={0,0,height,width} ;

    RectRgn( (RgnHandle) m_macBoundaryClipRgn , &r ) ;
    CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;

    m_metaFile->SetHMETAFILE( OpenPicture( &r ) ) ;
    ::GetPort( (GrafPtr*) &m_macPort ) ;
    m_ok = true ;

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
    m_metafile.SetHMETAFILE( handle ) ;
    return true ;
}
#endif

#endif
