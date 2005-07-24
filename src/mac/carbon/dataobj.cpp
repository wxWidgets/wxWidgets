///////////////////////////////////////////////////////////////////////////////
// Name:        mac/dataobj.cpp
// Purpose:     implementation of wxDataObject class
// Author:      Stefan Csomor
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma implementation "dataobj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAOBJ

#ifndef WX_PRECOMP
#include "wx/intl.h"
#endif

#include "wx/log.h"
#include "wx/dataobj.h"
#include "wx/dcmemory.h"
#include "wx/mstream.h"
#include "wx/image.h"
#include "wx/metafile.h"
#include "wx/mac/private.h"
#ifndef __DARWIN__
#include <Scrap.h>
#endif

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDataFormat
// ----------------------------------------------------------------------------

wxDataFormat::wxDataFormat()
{
    m_type = wxDF_INVALID;
    m_format = 0;
}

wxDataFormat::wxDataFormat(  wxDataFormatId   vType )
{
    SetType(vType);
}

wxDataFormat::wxDataFormat(  const wxChar*  zId)
{
    SetId(zId);
}

wxDataFormat::wxDataFormat(  const wxString&   rId)
{
    SetId(rId);
}

wxDataFormat::wxDataFormat( NativeFormat vFormat)
{
    SetId(vFormat);
}

void wxDataFormat::SetType(  wxDataFormatId  Type )
{
    m_type = Type;

    if (m_type == wxDF_TEXT )
        m_format = kScrapFlavorTypeText;
    else if (m_type == wxDF_UNICODETEXT )
        m_format = kScrapFlavorTypeUnicode ;
    else if (m_type == wxDF_BITMAP || m_type == wxDF_METAFILE )
        m_format = kScrapFlavorTypePicture;
    else if (m_type == wxDF_FILENAME)
        m_format = kDragFlavorTypeHFS ;
    else
    {
       wxFAIL_MSG( wxT("invalid dataformat") );

       // this is '????' but it can't be used in the code because ??' is
       // parsed as a trigraph!
       m_format = 0x3f3f3f3f;
    }
}

wxString wxDataFormat::GetId() const
{
    // note that m_format is not a pointer to string, it *is* itself a 4
    // character string
    char text[5] ;
    strncpy( text , (char*) &m_format , 4 ) ;
    text[4] = 0 ;

    return wxString::FromAscii( text ) ;
}

void wxDataFormat::SetId(  NativeFormat  format )
{
    m_format = format;

    if (m_format == kScrapFlavorTypeText)
        m_type = wxDF_TEXT;
    else if (m_format == kScrapFlavorTypeUnicode )
        m_type = wxDF_UNICODETEXT;
    else if (m_format == kScrapFlavorTypePicture)
        m_type = wxDF_BITMAP;
    else if (m_format == kDragFlavorTypeHFS )
        m_type = wxDF_FILENAME;
    else
        m_type = wxDF_PRIVATE;
}

void wxDataFormat::SetId( const wxChar* zId )
{
    m_type = wxDF_PRIVATE;
    m_format = 0;// TODO: get the format gdk_atom_intern( wxMBSTRINGCAST tmp.mbc_str(), FALSE );
}

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

wxDataObject::wxDataObject()
{
}

bool wxDataObject::IsSupportedFormat(
  const wxDataFormat&               rFormat
, Direction                         vDir
) const
{
    size_t                          nFormatCount = GetFormatCount(vDir);

    if (nFormatCount == 1)
    {
        return rFormat == GetPreferredFormat();
    }
    else
    {
        wxDataFormat* pFormats = new wxDataFormat[nFormatCount];
        GetAllFormats( pFormats
                      ,vDir
                     );

        size_t                      n;

        for (n = 0; n < nFormatCount; n++)
        {
            if (pFormats[n] == rFormat)
                break;
        }

        delete [] pFormats;

        // found?
        return n < nFormatCount;
    }
}

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

#if wxUSE_UNICODE
void wxTextDataObject::GetAllFormats(wxDataFormat *formats, wxDataObjectBase::Direction dir) const
{
    *formats++ = wxDataFormat( wxDF_TEXT );
    *formats = wxDataFormat( wxDF_UNICODETEXT );
}

#endif

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

bool wxFileDataObject::GetDataHere(
  void*                             pBuf
) const
{
    wxString                        sFilenames;

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        sFilenames += m_filenames[i];
        sFilenames += (wxChar)0;
    }

    memcpy(pBuf, sFilenames.mbc_str(), sFilenames.Len() + 1);
    return TRUE;
}

size_t wxFileDataObject::GetDataSize() const
{
    size_t nRes = 0;

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        nRes += m_filenames[i].Len();
        nRes += 1;
    }

    return nRes + 1;
}

bool wxFileDataObject::SetData(
  size_t                            WXUNUSED(nSize)
, const void*                       pBuf
)
{
    m_filenames.Empty();

    // only add if this is not an empty string
    // we can therefore clear the list by just setting an empty string
    if ( (*(char*)pBuf) != 0 )
        AddFile(wxString::FromAscii((char*)pBuf));

    return TRUE;
}

void wxFileDataObject::AddFile(
  const wxString&                   rFilename
)
{
    m_filenames.Add(rFilename);
}

// ----------------------------------------------------------------------------
// wxBitmapDataObject
// ----------------------------------------------------------------------------

wxBitmapDataObject::wxBitmapDataObject()
{
    Init();
}

wxBitmapDataObject::wxBitmapDataObject(
  const wxBitmap& rBitmap
)
: wxBitmapDataObjectBase(rBitmap)
{
    Init();
    if ( m_bitmap.Ok() )
    {
        m_pictHandle = m_bitmap.GetBitmapData()->GetPictHandle() ;
        m_pictCreated = false ;
    }
}

wxBitmapDataObject::~wxBitmapDataObject()
{
    Clear();
}

void wxBitmapDataObject::SetBitmap(
  const wxBitmap&                   rBitmap
)
{
    Clear();
    wxBitmapDataObjectBase::SetBitmap(rBitmap);
    if ( m_bitmap.Ok() )
    {
        m_pictHandle = m_bitmap.GetBitmapData()->GetPictHandle() ;
        m_pictCreated = false ;
    }
}

void wxBitmapDataObject::Init() 
{ 
    m_pictHandle = NULL ;
    m_pictCreated = false ;
} 

void wxBitmapDataObject::Clear() 
{
    if ( m_pictCreated && m_pictHandle )
    {
        KillPicture( (PicHandle) m_pictHandle ) ;
    }
    m_pictHandle = NULL ;
}

bool wxBitmapDataObject::GetDataHere(
  void*                             pBuf
) const
{
    if (!m_pictHandle)
    {
        wxFAIL_MSG(wxT("attempt to copy empty bitmap failed"));
        return FALSE;
    }
    memcpy(pBuf, *(Handle)m_pictHandle, GetHandleSize((Handle)m_pictHandle));
    return TRUE;
}

size_t wxBitmapDataObject::GetDataSize() const
{
    return GetHandleSize((Handle)m_pictHandle) ;
}

bool wxBitmapDataObject::SetData(
  size_t                            nSize
, const void*                       pBuf
)
{
    Clear();
    PicHandle picHandle = (PicHandle) NewHandle( nSize ) ;
    memcpy( *picHandle , pBuf , nSize ) ;
    m_pictHandle = picHandle ;
    // ownership is transferred to the bitmap
    m_pictCreated = false ;
    Rect frame = (**picHandle).picFrame ;
    
    wxMetafile mf ;
    mf.SetHMETAFILE( (WXHMETAFILE) m_pictHandle ) ;
    wxMemoryDC mdc ;
    m_bitmap.Create( frame.right - frame.left ,frame.bottom - frame.top ) ;
    mdc.SelectObject(m_bitmap ) ;
    mf.Play( &mdc ) ;
    mdc.SelectObject( wxNullBitmap ) ;
    
    return m_bitmap.Ok();
}

#endif