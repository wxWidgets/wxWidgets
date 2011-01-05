/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/xtistrm.cpp
// Purpose:     streaming runtime metadata information
// Author:      Stefan Csomor
// Modified by:
// Created:     27/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xtistrm.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/event.h"
#endif

#include <map>
#include <vector>
#include <string>
using namespace std;

#include "wx/tokenzr.h"
#include "wx/txtstrm.h"
#include "codereadercallback.h"

#if !wxUSE_EXTENDED_RTTI
    #error This sample requires XTI (eXtended RTTI) enabled
#endif

// ----------------------------------------------------------------------------
// wxObjectCodeReaderCallback - depersisting to code
// ----------------------------------------------------------------------------

struct wxObjectCodeReaderCallback::wxObjectCodeReaderCallbackInternal
{
#if wxUSE_UNICODE
    map<int,wstring> m_objectNames;
#else
    map<int,string> m_objectNames;
#endif

    void SetObjectName(int objectID, const wxString &name )
    {
        if ( m_objectNames.find(objectID) != m_objectNames.end() )
        {
            wxLogError( _("Passing a already registered object to SetObjectName") );
            return ;
        }
        m_objectNames[objectID] = (const wxChar *)name;
    }

    wxString GetObjectName( int objectID )
    {
        if ( objectID == wxNullObjectID )
            return wxT("NULL");

        if ( m_objectNames.find(objectID) == m_objectNames.end() )
        {
            wxLogError( _("Passing an unkown object to GetObject") );
            return wxEmptyString;
        }
        return wxString( m_objectNames[objectID].c_str() );
    }
};

wxObjectCodeReaderCallback::wxObjectCodeReaderCallback(wxTextOutputStream *out)
: m_fp(out)
{
    m_data = new wxObjectCodeReaderCallbackInternal;
}

wxObjectCodeReaderCallback::~wxObjectCodeReaderCallback()
{
    delete m_data;
}

void wxObjectCodeReaderCallback::AllocateObject(int objectID, wxClassInfo *classInfo,
                                       wxVariantBaseArray &WXUNUSED(metadata))
{
    wxString objectName = wxString::Format( wxT("LocalObject_%d"), objectID );
    m_fp->WriteString( wxString::Format( wxT("\t%s *%s = new %s;\n"),
        classInfo->GetClassName(),
        objectName.c_str(),
        classInfo->GetClassName()) );
    m_data->SetObjectName( objectID, objectName );
}

void wxObjectCodeReaderCallback::DestroyObject(int objectID, wxClassInfo *WXUNUSED(classInfo))
{
    m_fp->WriteString( wxString::Format( wxT("\tdelete %s;\n"),
        m_data->GetObjectName( objectID).c_str() ) );
}

wxString wxObjectCodeReaderCallback::ValueAsCode( const wxVariantBase &param )
{
    wxString value;
    const wxTypeInfo* type = param.GetTypeInfo();
    if ( type->GetKind() == wxT_CUSTOM )
    {
        const wxCustomTypeInfo* cti = wx_dynamic_cast(const wxCustomTypeInfo*, type);
        if ( cti )
        {
            value.Printf( wxT("%s(%s)"), cti->GetTypeName().c_str(),
                          param.GetAsString().c_str() );
        }
        else
        {
            wxLogError ( _("Internal error, illegal wxCustomTypeInfo") );
        }
    }
    else if ( type->GetKind() == wxT_STRING )
    {
        value.Printf( wxT("\"%s\""),param.GetAsString().c_str() );
    }
    else
    {
        value.Printf( wxT("%s"), param.GetAsString().c_str() );
    }
    return value;
}

void wxObjectCodeReaderCallback::CreateObject(int objectID,
                                     const wxClassInfo *WXUNUSED(classInfo),
                                     int paramCount,
                                     wxVariantBase *params,
                                     int *objectIDValues,
                                     const wxClassInfo **WXUNUSED(objectClassInfos),
                                     wxVariantBaseArray &WXUNUSED(metadata)
                                     )
{
    int i;
    m_fp->WriteString( wxString::Format( wxT("\t%s->Create("), 
                       m_data->GetObjectName(objectID).c_str() ) );
    for (i = 0; i < paramCount; i++)
    {
        if ( objectIDValues[i] != wxInvalidObjectID )
        {
            wxString str = 
                wxString::Format( wxT("%s"), 
                                  m_data->GetObjectName( objectIDValues[i] ).c_str() );
            m_fp->WriteString( str );
        }
        else
        {
            m_fp->WriteString( 
                wxString::Format( wxT("%s"), ValueAsCode(params[i]).c_str() ) );
        }
        if (i < paramCount - 1)
            m_fp->WriteString( wxT(", "));
    }
    m_fp->WriteString( wxT(");\n") );
}

void wxObjectCodeReaderCallback::ConstructObject(int objectID,
                                     const wxClassInfo *classInfo,
                                     int paramCount,
                                     wxVariantBase *params,
                                     int *objectIDValues,
                                     const wxClassInfo **WXUNUSED(objectClassInfos),
                                     wxVariantBaseArray &WXUNUSED(metadata)
                                     )
{
    wxString objectName = wxString::Format( wxT("LocalObject_%d"), objectID );
    m_fp->WriteString( wxString::Format( wxT("\t%s *%s = new %s("),
        classInfo->GetClassName(),
        objectName.c_str(),
        classInfo->GetClassName()) );
    m_data->SetObjectName( objectID, objectName );

    int i;
    for (i = 0; i < paramCount; i++)
    {
        if ( objectIDValues[i] != wxInvalidObjectID )
            m_fp->WriteString( wxString::Format( wxT("%s"), 
                               m_data->GetObjectName( objectIDValues[i] ).c_str() ) );
        else
        {
            m_fp->WriteString( 
                wxString::Format( wxT("%s"), ValueAsCode(params[i]).c_str() ) );
        }
        if (i < paramCount - 1)
            m_fp->WriteString( wxT(", ") );
    }
    m_fp->WriteString( wxT(");\n") );
}

void wxObjectCodeReaderCallback::SetProperty(int objectID,
                                    const wxClassInfo *WXUNUSED(classInfo),
                                    const wxPropertyInfo* propertyInfo,
                                    const wxVariantBase &value)
{
    m_fp->WriteString( wxString::Format( wxT("\t%s->%s(%s);\n"),
        m_data->GetObjectName(objectID).c_str(),
        propertyInfo->GetAccessor()->GetSetterName().c_str(),
        ValueAsCode(value).c_str()) );
}

void wxObjectCodeReaderCallback::SetPropertyAsObject(int objectID,
                                            const wxClassInfo *WXUNUSED(classInfo),
                                            const wxPropertyInfo* propertyInfo,
                                            int valueObjectId)
{
    if ( propertyInfo->GetTypeInfo()->GetKind() == wxT_OBJECT )
        m_fp->WriteString( wxString::Format( wxT("\t%s->%s(*%s);\n"),
        m_data->GetObjectName(objectID).c_str(),
        propertyInfo->GetAccessor()->GetSetterName().c_str(),
        m_data->GetObjectName( valueObjectId).c_str() ) );
    else
        m_fp->WriteString( wxString::Format( wxT("\t%s->%s(%s);\n"),
        m_data->GetObjectName(objectID).c_str(),
        propertyInfo->GetAccessor()->GetSetterName().c_str(),
        m_data->GetObjectName( valueObjectId).c_str() ) );
}

void wxObjectCodeReaderCallback::AddToPropertyCollection( int objectID,
                                                const wxClassInfo *WXUNUSED(classInfo),
                                                const wxPropertyInfo* propertyInfo,
                                                const wxVariantBase &value)
{
    m_fp->WriteString( wxString::Format( wxT("\t%s->%s(%s);\n"),
        m_data->GetObjectName(objectID).c_str(),
        propertyInfo->GetAccessor()->GetAdderName().c_str(),
        ValueAsCode(value).c_str()) );
}

// sets the corresponding property (value is an object)
void wxObjectCodeReaderCallback::
    AddToPropertyCollectionAsObject(int WXUNUSED(objectID),
                                    const wxClassInfo *WXUNUSED(classInfo),
                                    const wxPropertyInfo* WXUNUSED(propertyInfo),
                                    int WXUNUSED(valueObjectId))
{
    // TODO
}

void wxObjectCodeReaderCallback::SetConnect(int eventSourceObjectID,
                                   const wxClassInfo *WXUNUSED(eventSourceClassInfo),
                                   const wxPropertyInfo *delegateInfo,
                                   const wxClassInfo *eventSinkClassInfo,
                                   const wxHandlerInfo* handlerInfo,
                                   int eventSinkObjectID )
{
    wxString ehsource = m_data->GetObjectName( eventSourceObjectID );
    wxString ehsink = m_data->GetObjectName(eventSinkObjectID);
    wxString ehsinkClass = eventSinkClassInfo->GetClassName();
    const wxEventSourceTypeInfo *delegateTypeInfo = 
        wx_dynamic_cast(const wxEventSourceTypeInfo*, delegateInfo->GetTypeInfo());
    if ( delegateTypeInfo )
    {
        int eventType = delegateTypeInfo->GetEventType();
        wxString handlerName = handlerInfo->GetName();

        wxString code =
            wxString::Format( 
                wxT("\t%s->Connect( %s->GetId(), %d, ")
                wxT("(wxObjectEventFunction)(wxEventFunction) & %s::%s, NULL, %s );"),
                ehsource.c_str(), ehsource.c_str(), eventType, ehsinkClass.c_str(),
                handlerName.c_str(), ehsink.c_str() );

        m_fp->WriteString( code );
    }
    else
    {
        wxLogError(_("delegate has no type info"));
    }
}
