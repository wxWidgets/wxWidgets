///////////////////////////////////////////////////////////////////////////////
// Name:        os2/dataobj.cpp
// Purpose:     implementation of wx[I]DataObject class
// Author:      David Webster
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "dataobj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/intl.h"
#endif
#include "wx/defs.h"

#include "wx/log.h"
#include "wx/dataobj.h"

#define INCL_DOS
#include <os2.h>

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__
    static const wxChar *GetTymedName(DWORD tymed);
#endif // Debug

// ----------------------------------------------------------------------------
// wxDataFormat
// ----------------------------------------------------------------------------

wxDataFormat::wxDataFormat(
  wxDataFormatId                    vType
)
{
    PrepareFormats();
    m_vType = wxDF_INVALID;
    m_vFormat = 0;
}

wxDataFormat::wxDataFormat(
  wxDataFormatId                    vType
)
{
    PrepareFormats();
    SetType(vType);
}

wxDataFormat::wxDataFormat(
  const wxChar*                     zId
)
{
    PrepareFormats();
    SetId(zId);
}

wxDataFormat::wxDataFormat(
  const wxString&                   rId
)
{
    PrepareFormats();
    SetId(rId);
}

wxDataFormat::wxDataFormat(
  NativeFormat                      vFormat
)
{
    PrepareFormats();
    SetId(vFormat);
}

void wxDataFormat::SetType(
  wxDataFormatId                    vType
)
{
    m_vType = vType;

    if (m_vType == wxDF_TEXT)
        m_vFormat = 0;
    else
    if (m_vType == wxDF_BITMAP)
        m_vFormat = 0;
    else
    if (m_vType == wxDF_FILENAME)
        m_vFormat = 0;
    else
    {
       wxFAIL_MSG( wxT("invalid dataformat") );
    }
}

wxDataFormatId wxDataFormat::GetType() const
{
    return m_vType;
}

wxString wxDataFormat::GetId() const
{
    wxString                        sRet("");  // TODO: gdk_atom_name( m_format ) );
    return sRet;
}

void wxDataFormat::SetId(
  NativeFormat                      vFormat
)
{
    m_vFormat = vFormat;
// TODO:
/*
    if (m_format == g_textAtom)
        m_type = wxDF_TEXT;
    else
    if (m_format == g_pngAtom)
        m_type = wxDF_BITMAP;
    else
    if (m_format == g_fileAtom)
        m_type = wxDF_FILENAME;
    else
        m_type = wxDF_PRIVATE;
*/
}

void wxDataFormat::SetId(
  const wxChar*                     zId
)
{
    wxString                        tmp(zId);

    m_vType = wxDF_PRIVATE;
    m_vFormat = 0;// TODO: get the format gdk_atom_intern( wxMBSTRINGCAST tmp.mbc_str(), FALSE );
}

void wxDataFormat::PrepareFormats()
{
// TODO:
/*
    if (!g_textAtom)
        g_textAtom = gdk_atom_intern( "STRING", FALSE );
    if (!g_pngAtom)
        g_pngAtom = gdk_atom_intern( "image/png", FALSE );
    if (!g_fileAtom)
        g_fileAtom = gdk_atom_intern( "file:ALL", FALSE );
*/
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
        wxDataFormat*               pFormats = new wxDataFormat[nFormatCount];
        GetAllFormats( rFormats
                      ,vDir
                     );

        size_t                      n;

        for (n = 0; n < nFormatCount; n++)
        {
            if (rFormats[n] == rFormat)
                break;
        }

        delete [] rFormats;

        // found?
        return n < nFormatCount;
    }
}

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
        filenames += m_filenames[i];
        filenames += (wxChar)0;
    }

    memcpy(pBuf, filenames.mbc_str(), filenames.Len() + 1);
    return TRUE;
}

size_t wxFileDataObject::GetDataSize() const
{
    size_t                          nRes = 0;

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
    /* TODO */

    wxString                        sFile( (const char *)pBuf);  /* char, not wxChar */

    AddFile(sFile);

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
  const wxBitmap&                   rBitmap
)
: wxBitmapDataObjectBase(rBitmap)
{
    Init();

    DoConvertToPng();
}

wxBitmapDataObject::~wxBitmapDataObject()
{
    Clear();
}

void wxBitmapDataObject::SetBitmap(
  const wxBitmap&                   rBitmap
)
{
    ClearAll();
    wxBitmapDataObjectBase::SetBitmap(rBitmap);
    DoConvertToPng();
}

bool wxBitmapDataObject::GetDataHere(
  void*                             pBuf
) const
{
    if (!m_pngSize)
    {
        wxFAIL_MSG(wxT("attempt to copy empty bitmap failed"));
        return FALSE;
    }
    memcpy(pBuf, m_pngData, m_pngSize);
    return TRUE;
}

bool wxBitmapDataObject::SetData(
  size_t                            nSize
, const void*                       pBuf
)
{
    Clear();
    m_pngSize = nSize;
    m_pngData = malloc(m_pngSize);

    memcpy(m_pngData, pBuf, m_pngSize);

    wxMemoryInputStream             vMstream((char*)m_pngData, m_pngSize);
    wxImage                         vImage;
    wxPNGHandler                    vHandler;

    if (!vHandler.LoadFile(&vImage, vMstream))
    {
        return FALSE;
    }

    m_bitmap = vImage.ConvertToBitmap();
    return m_bitmap.Ok();
}

void wxBitmapDataObject::DoConvertToPng()
{
    if (!m_bitmap.Ok())
        return;

    wxImage                         vImage(m_bitmap);
    wxPNGHandler                    vHandler;
    wxCountingOutputStream          vCount;

    vHandler.SaveFile(&rImage, vCount);

    m_pngSize = vCount.GetSize() + 100; // sometimes the size seems to vary ???
    m_pngData = malloc(m_pngSize);

    wxMemoryOutputStream mstream((char*) m_pngData, m_pngSize);
    vHandler.SaveFile(&vImage, vMstream );
}

