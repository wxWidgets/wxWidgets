///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.cpp
// Purpose:     wxDataObject class
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dataobj.h"
#endif

#include "wx/dataobj.h"

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS( wxDataObject, wxObject )

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxTextDataObject, wxDataObject )

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxFileDataObject, wxDataObject )

// ----------------------------------------------------------------------------
// wxBitmapDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxBitmapDataObject, wxDataObject )

// ----------------------------------------------------------------------------
// wxPrivateDataObject
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxPrivateDataObject, wxDataObject )

void wxPrivateDataObject::SetData( const char *data, size_t size )
{
    m_size = size;
    
    if (m_data) delete[] m_data;
    
    m_data = new char[size];

    memcpy( m_data, data, size );  
}

