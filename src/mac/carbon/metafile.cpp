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

#ifdef __GNUG__
#pragma implementation "metafile.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/setup.h"
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
    if (!file.IsNull() && (file.Cmp("") == 0))
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
    if (!m_refData)
        m_refData = new wxMetafileRefData;

    M_METAFILEDATA->m_metafile = mf;
}

bool wxMetaFile::Play(wxDC *dc)
{
    if (!m_refData)
        return FALSE;
    
    if (!dc->Ok() )
        return FALSE;
        
    {
        wxMacPortSetter helper( dc ) ;
        PicHandle pict = (PicHandle) GetHMETAFILE() ;
        DrawPicture( pict , &(**pict).picFrame ) ;
    }
    return TRUE;
}

/*
 * Metafile device context
 *
 */

// Original constructor that does not takes origin and extent. If you use this,
// *DO* give origin/extent arguments to wxMakeMetaFilePlaceable.
wxMetaFileDC::wxMetaFileDC(const wxString& file)
{
    m_metaFile = NULL;
    m_minX = 10000;
    m_minY = 10000;
    m_maxX = -10000;
    m_maxY = -10000;
    
    wxASSERT_MSG( file.IsEmpty() , wxT("no file based metafile support yet") ) ;
    
    m_metaFile = new wxMetaFile(wxEmptyString) ;
    Rect r={0,0,1000,1000} ;
    
    m_metaFile->SetHMETAFILE( OpenPicture( &r ) ) ;
    ::GetPort( (GrafPtr*) &m_macPort ) ;    
    m_ok = TRUE ;
    
    SetMapMode(wxMM_TEXT); 
}

// New constructor that takes origin and extent. If you use this, don't
// give origin/extent arguments to wxMakeMetaFilePlaceable.

wxMetaFileDC::wxMetaFileDC(const wxString& file, int xext, int yext, int xorg, int yorg)
{
    m_minX = 10000;
    m_minY = 10000;
    m_maxX = -10000;
    m_maxY = -10000;
    
    wxASSERT_MSG( file.IsEmpty() , wxT("no file based metafile support yet")) ;
    
    m_metaFile = new wxMetaFile(wxEmptyString) ;
    Rect r={yorg,xorg,yorg+yext,xorg+xext} ;
    
    m_metaFile->SetHMETAFILE( OpenPicture( &r ) ) ;
    ::GetPort( (GrafPtr*) &m_macPort ) ;    
    m_ok = TRUE ;
    
    SetMapMode(wxMM_TEXT); 
}

wxMetaFileDC::~wxMetaFileDC()
{
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
    Handle handle = (Handle) m_metafile.GetHMETAFILE() ;
    SetHandleSize( handle , len ) ;
    memcpy( *handle , buf , len ) ;
    return true ;
}
#endif

#endif
