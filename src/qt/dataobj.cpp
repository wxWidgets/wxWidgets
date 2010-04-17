/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dataobj.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dataobj.h"


wxDataFormat::wxDataFormat()
{
}

wxDataFormat::wxDataFormat( wxDataFormatId formatId )
{
}

wxDataFormat::wxDataFormat(const wxString &id)
{
}

wxDataFormat::wxDataFormat(const wxChar *id)
{
}


void wxDataFormat::SetId( const wxChar *id )
{
}

bool wxDataFormat::operator==(wxDataFormatId format) const
{
    return false;
}

bool wxDataFormat::operator!=(wxDataFormatId format) const
{
    return false;
}

bool wxDataFormat::operator==(const wxDataFormat& format) const
{
    return false;
}

bool wxDataFormat::operator!=(const wxDataFormat& format) const
{
    return false;
}

//#############################################################################

wxDataObject::wxDataObject()
{
}


wxDataFormat wxDataObject::GetPreferredFormat(Direction dir ) const
{
    return wxDataFormat();
}

size_t wxDataObject::GetFormatCount(Direction dir ) const
{
    return 0;
}

void wxDataObject::GetAllFormats(wxDataFormat *formats, Direction dir ) const
{
}

size_t wxDataObject::GetDataSize(const wxDataFormat& format) const
{
    return 0;
}

bool wxDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
{
    return false;
}

//#############################################################################

wxBitmapDataObject::wxBitmapDataObject()
{
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap &bitmap )
{
}

//#############################################################################

wxFileDataObject::wxFileDataObject()
{
}

