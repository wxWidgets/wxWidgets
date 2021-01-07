/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/clntdata.cpp
// Purpose:     A mixin class for holding a wxClientData or void pointer
// Author:      Robin Dunn
// Modified by:
// Created:     9-Oct-2001
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/clntdata.h"


// ----------------------------------------------------------------------------


wxClientDataContainer::wxClientDataContainer()
{
    // no client data (yet)
    m_clientData = NULL;
    m_clientDataType = wxClientData_None;
}

wxClientDataContainer::~wxClientDataContainer()
{
    // we only delete object data, not untyped
    if ( m_clientDataType == wxClientData_Object )
        delete m_clientObject;
}

void wxClientDataContainer::DoSetClientObject( wxClientData *data )
{
    wxASSERT_MSG( m_clientDataType != wxClientData_Void,
                  wxT("can't have both object and void client data") );

    delete m_clientObject;
    m_clientObject = data;
    m_clientDataType = wxClientData_Object;
}

wxClientData *wxClientDataContainer::DoGetClientObject() const
{
    // it's not an error to call GetClientObject() on a window which doesn't
    // have client data at all - NULL will be returned
    wxASSERT_MSG( m_clientDataType != wxClientData_Void,
                  wxT("this window doesn't have object client data") );

    return m_clientObject;
}

void wxClientDataContainer::DoSetClientData( void *data )
{
    wxASSERT_MSG( m_clientDataType != wxClientData_Object,
                  wxT("can't have both object and void client data") );

    m_clientData = data;
    m_clientDataType = wxClientData_Void;
}

void *wxClientDataContainer::DoGetClientData() const
{
    // it's not an error to call GetClientData() on a window which doesn't have
    // client data at all - NULL will be returned
    wxASSERT_MSG( m_clientDataType != wxClientData_Object,
                  wxT("this window doesn't have void client data") );

    return m_clientData;
}


// ----------------------------------------------------------------------------


