///////////////////////////////////////////////////////////////////////////////
// Name:        os2/dataobj.cpp
// Purpose:     implementation of wx[I]DataObject class
// Author:      David Webster
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 David Webster
// Licence:     wxWindows licence
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
#include "wx/mstream.h"
#include "wx/image.h"

#define INCL_DOS
#include <os2.h>

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxDataFormat
// ----------------------------------------------------------------------------

wxString wxDataFormat::GetId() const
{
    char                            zBuf[256];
    wxString                        sRet;

    ::WinQueryAtomName( ::WinQuerySystemAtomTable()
                       ,m_uFormat
                       ,zBuf
                       ,256
                      );
    sRet = zBuf;
    return sRet;
} // end of wxDataFormat::GetId()

void wxDataFormat::SetId (
  const wxChar*                     zId
)
{
    m_uFormat = ::WinAddAtom( ::WinQuerySystemAtomTable()
                             ,zId
                            );
} // end of wxDataFormat::SetId

class CIDataObject
{
public:
    CIDataObject(wxDataObject* pDataObject);
    ~CIDataObject();

    //
    // Operations on the DRAGITEM struct
    //
    bool GetData( const wxDataFormat& rFormat
                 ,char*               pzBuffer
                 ,ULONG               ulLen
                );
    void GetDataHere( const wxDataFormat& rFormat
                     ,char*               pzBuffer
                     ,ULONG               ulLen
                    );
    void QueryGetData(const wxDataFormat& rFormat);
    void SetData( const wxDataFormat& rFormat
                 ,char*               pzBuffer
                );
private:
    wxDataObject*                   m_pDataObject;      // pointer to C++ class we belong to
    DRAGITEM                        m_vDragItem;
}; // end of CLASS CIDataObject

bool CIDataObject::GetData (
  const wxDataFormat&               rFormat
, char*                             pzBuffer
, ULONG                             ulLen
)
{
    QueryGetData(rFormat);
    if (rFormat.GetType() == wxDF_INVALID)
        return FALSE;

    ULONG                           ulSize = m_pDataObject->GetDataSize(rFormat);

    if (ulSize == 0)
    {
        //
        // It probably means that the method is just not implemented
        //
        return FALSE;
    }
    if (rFormat.GetType() == wxDF_PRIVATE)
    {
        //
        // For custom formats, put the size with the data - alloc the
        // space for it
        //
        ulSize += sizeof(ULONG);
    }

    if (ulSize > ulLen) // not enough room to copy
        return FALSE;

    //
    // Copy the data
    //
    GetDataHere( rFormat
                ,pzBuffer
                ,ulSize
               );
    return TRUE;
} // end of CIDataObject::GetData

void CIDataObject::GetDataHere(
  const wxDataFormat&               rFormat
, char*                             pzBuffer
, ULONG                             WXUNUSED(ulLen)
)
{
    m_pDataObject->GetDataHere( rFormat
                               ,(void*)pzBuffer
                              );
} // end of CIDataObject::GetDataHere

void CIDataObject::QueryGetData (
  const wxDataFormat&               rFormat
)
{
    m_pDataObject->IsSupportedFormat(rFormat);
} // end of CIDataObject::QueryGetData

void CIDataObject::SetData (
  const wxDataFormat&               rFormat
, char*                             pzBuffer
)
{
    ULONG                           ulSize;

    switch (rFormat.GetType())
    {
        case wxDF_TEXT:
        case wxDF_OEMTEXT:
        case wxDF_FILENAME:
        case wxDF_HTML:
            ulSize = strlen((const char *)pzBuffer);
            break;

#if wxUSE_UNICODE
        case wxDF_UNICODETEXT:
             ulSize = ::wcslen((const wchar_t *)pzBuffer);
             break;
#endif

        case wxDF_BITMAP:
        case wxDF_METAFILE:
        case wxDF_ENHMETAFILE:
        case wxDF_TIFF:
        case wxDF_DIB:
            ulSize = 0; // pass via a handle
            break;


        case wxDF_SYLK:
        case wxDF_DIF:
        case wxDF_PALETTE:
        case wxDF_PENDATA:
        case wxDF_RIFF:
        case wxDF_WAVE:
        case wxDF_LOCALE:
            //PUNT
            break;

        case wxDF_PRIVATE:
            size_t*                 p = (size_t *)pzBuffer;

            ulSize = *p++;
            pzBuffer = (char*)p;
            break;
    }
    m_pDataObject->SetData( rFormat
                           ,ulSize
                           ,(void*)pzBuffer
                          );
} // end of CIDataObject::SetData

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

wxDataObject::wxDataObject ()
{
    m_pDataObject = new DRAGITEM;
} // end of wxDataObject::wxDataObject

wxDataObject::~wxDataObject ()
{
    delete m_pDataObject;
} // end of wxDataObject::~wxDataObject

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

#if wxUSE_STREAMS
    wxMemoryInputStream             vMstream((char*)m_pngData, m_pngSize);
    wxImage                         vImage;
    wxPNGHandler                    vHandler;

    if (!vHandler.LoadFile(&vImage, vMstream))
    {
        return FALSE;
    }

    m_bitmap = wxBitmap(vImage);
#endif //wxUSE_STREAMS

    return m_bitmap.Ok();
}

void wxBitmapDataObject::DoConvertToPng()
{
    if (!m_bitmap.Ok())
        return;

#if wxUSE_STREAMS
    wxImage                         vImage = m_bitmap.ConvertToImage();
    wxPNGHandler                    vHandler;
    wxCountingOutputStream          vCount;

    vHandler.SaveFile(&vImage, vCount);

    m_pngSize = vCount.GetSize() + 100; // sometimes the size seems to vary ???
    m_pngData = malloc(m_pngSize);

    wxMemoryOutputStream            vMstream((char*) m_pngData, m_pngSize);

    vHandler.SaveFile(&vImage, vMstream );
#endif
}

