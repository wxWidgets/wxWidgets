///////////////////////////////////////////////////////////////////////////////
// Name:        common/dobjcmn.cpp
// Purpose:     implementation of data object methods common to all platforms
// Author:      Vadim Zeitlin, Robert Roebling
// Modified by:
// Created:     19.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows Team
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dataobjbase.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/debug.h"
#endif // WX_PRECOMP

#include "wx/dataobj.h"

// ----------------------------------------------------------------------------
// lists
// ----------------------------------------------------------------------------

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxSimpleDataObjectList);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDataObjectBase
// ----------------------------------------------------------------------------

wxDataObjectBase::~wxDataObjectBase()
{
}

// ----------------------------------------------------------------------------
// wxDataObjectComposite
// ----------------------------------------------------------------------------

wxDataObjectSimple *
wxDataObjectComposite::GetObject(const wxDataFormat& format) const
{
    wxSimpleDataObjectList::Node *node = m_dataObjects.GetFirst();
    while ( node )
    {
        wxDataObjectSimple *dataObj = node->GetData();

        if ( dataObj->GetFormat() == format )
        {
            return dataObj;
        }

        node = node->GetNext();
    }

    return (wxDataObjectSimple *)NULL;
}

void wxDataObjectComposite::Add(wxDataObjectSimple *dataObject, bool preferred)
{
    if ( preferred )
        m_preferred = m_dataObjects.GetCount();

    m_dataObjects.Append( dataObject );
}

wxDataFormat
wxDataObjectComposite::GetPreferredFormat(Direction WXUNUSED(dir)) const
{
    wxSimpleDataObjectList::Node *node = m_dataObjects.Item( m_preferred );

    wxCHECK_MSG( node, wxDF_INVALID, wxT("no preferred format") );

    wxDataObjectSimple* dataObj = node->GetData();

    return dataObj->GetFormat();
}

size_t wxDataObjectComposite::GetFormatCount(Direction WXUNUSED(dir)) const
{
    // TODO what about the Get/Set only formats?
    return m_dataObjects.GetCount();
}

void wxDataObjectComposite::GetAllFormats(wxDataFormat *formats,
                                          Direction WXUNUSED(dir)) const
{
    size_t n = 0;
    wxSimpleDataObjectList::Node *node;
    for ( node = m_dataObjects.GetFirst(); node; node = node->GetNext() )
    {
        // TODO if ( !outputOnlyToo ) && this one counts ...
        formats[n++] = node->GetData()->GetFormat();
    }
}

size_t wxDataObjectComposite::GetDataSize(const wxDataFormat& format) const
{
    wxDataObjectSimple *dataObj = GetObject(format);

    wxCHECK_MSG( dataObj, 0,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->GetDataSize();
}

bool wxDataObjectComposite::GetDataHere(const wxDataFormat& format,
                                        void *buf) const
{
    wxDataObjectSimple *dataObj = GetObject(format);

    wxCHECK_MSG( dataObj, FALSE,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->GetDataHere(buf);
}

bool wxDataObjectComposite::SetData(const wxDataFormat& format,
                                    size_t len,
                                    const void *buf)
{
    wxDataObjectSimple *dataObj = GetObject(format);

    wxCHECK_MSG( dataObj, FALSE,
                 wxT("unsupported format in wxDataObjectComposite"));

    return dataObj->SetData(len, buf);
}

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

size_t wxTextDataObject::GetDataSize() const
{
    return GetTextLength();
}

bool wxTextDataObject::GetDataHere(void *buf) const
{
    strcpy((char *)buf, GetText().mb_str());

    return TRUE;
}

bool wxTextDataObject::SetData(size_t WXUNUSED(len), const void *buf)
{
    SetText(wxString((const char *)buf));

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxFileDataObjectBase
// ----------------------------------------------------------------------------

wxString wxFileDataObjectBase::GetFilenames() const
{
    wxString str;
    size_t count = m_filenames.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        str << m_filenames[n] << wxT('\0');
    }

    return str;
}

void wxFileDataObjectBase::SetFilenames(const wxChar* filenames)
{
    m_filenames.Empty();

    wxString current;
    for ( const wxChar *pc = filenames; ; pc++ )
    {
        if ( *pc )
        {
            current += *pc;
        }
        else
        {
            if ( !current )
            {
                // 2 consecutive NULs - this is the end of the string
                break;
            }

            m_filenames.Add(current);
            current.Empty();
        }
    }
}

// ----------------------------------------------------------------------------
// wxCustomDataObject
// ----------------------------------------------------------------------------

wxCustomDataObject::~wxCustomDataObject()
{
    Free();
}

void wxCustomDataObject::TakeData(size_t size, void *data)
{
    Free();

    m_size = size;
    m_data = data;
}

void *wxCustomDataObject::Alloc(size_t size)
{
    return (void *)new char[size];
}

void wxCustomDataObject::Free()
{
    delete [] m_data;
    m_size = 0;
    m_data = (void *)NULL;
}

size_t wxCustomDataObject::GetDataSize() const
{
    return GetSize();
}

bool wxCustomDataObject::GetDataHere(void *buf) const
{
    void *data = GetData();
    if ( !data )
        return FALSE;

    memcpy(buf, data, GetSize());

    return TRUE;
}

bool wxCustomDataObject::SetData(size_t len, const void *buf)
{
    Free();

    m_data = Alloc(size);
    if ( !m_data )
        return FALSE;

    memcpy(m_data, buf, m_size = len);

    return TRUE;
}

