/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/xtistrm.cpp
// Purpose:     streaming runtime metadata information
// Author:      Stefan Csomor
// Created:     27/07/03
// Copyright:   (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


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
    map<int,wstring> m_objectNames;

    void SetObjectName(int objectID, const wxString &name )
    {
        if ( m_objectNames.find(objectID) != m_objectNames.end() )
        {
            wxLogError( _("Passing an already registered object to SetObjectName") );
            return ;
        }
        m_objectNames[objectID] = (const wxChar *)name;
    }

    wxString GetObjectName( int objectID )
    {
        if ( objectID == wxNullObjectID )
            return "NULL";

        if ( m_objectNames.find(objectID) == m_objectNames.end() )
        {
            wxLogError( _("Passing an unknown object to GetObject") );
            return wxEmptyString;
        }
        return wxString( m_objectNames[objectID].c_str() );
    }
};

wxObjectCodeReaderCallback::wxObjectCodeReaderCallback(wxString& headerincludes, wxString &source)
: m_headerincludes(headerincludes),m_source(source)
{
    m_data = new wxObjectCodeReaderCallbackInternal;
}

wxObjectCodeReaderCallback::~wxObjectCodeReaderCallback()
{
    delete m_data;
}

void wxObjectCodeReaderCallback::AllocateObject(int objectID, wxClassInfo *classInfo,
                                       wxStringToAnyHashMap &WXUNUSED(metadata))
{
    if ( classInfo->GetIncludeName() != wxEmptyString)
    {
        // add corresponding header if not already included
        wxString include;
        include.Printf("#include \"%s\"\n",classInfo->GetIncludeName());
        if ( m_headerincludes.Find(include) == wxNOT_FOUND)
            m_headerincludes += include;
    }

    wxString objectName = wxString::Format( "LocalObject_%d", objectID );
    m_source += ( wxString::Format( "\t%s *%s = new %s;\n",
        classInfo->GetClassName(),
        objectName,
        classInfo->GetClassName()) );
    m_data->SetObjectName( objectID, objectName );
}

void wxObjectCodeReaderCallback::DestroyObject(int objectID, wxClassInfo *WXUNUSED(classInfo))
{
    m_source += ( wxString::Format( "\tdelete %s;\n",
        m_data->GetObjectName( objectID) ) );
}

class WXDLLIMPEXP_BASE wxObjectConstructorWriter: public wxObjectWriterFunctor
{
public:
    wxObjectConstructorWriter(const wxClassTypeInfo* cti,
        wxObjectCodeReaderCallback* writer) :
    m_cti(cti),m_writer(writer)
    {}

    virtual void operator()(const wxObject *vobj)
    {
        const wxClassInfo* ci = m_cti->GetClassInfo();

        for ( int i = 0; i < ci->GetCreateParamCount(); ++i )
        {
            wxString name = ci->GetCreateParamName(i);
            const wxPropertyInfo* prop = ci->FindPropertyInfo(name);
            if ( i > 0 )
                m_constructor += ", ";
            wxAny value;
            prop->GetAccessor()->GetProperty(vobj, value);
            m_constructor+= m_writer->ValueAsCode(value);
        }
    }

    const wxString& GetConstructorString() const { return m_constructor;}
private:
    const wxClassTypeInfo* m_cti;
    wxObjectCodeReaderCallback* m_writer;
    wxString m_constructor;
};

wxString wxObjectCodeReaderCallback::ValueAsCode( const wxAny &param )
{
    wxString value;

    const wxTypeInfo* type = param.GetTypeInfo();
    if ( type->GetKind() == wxT_CUSTOM )
    {
        const wxCustomTypeInfo* cti = wx_dynamic_cast(const wxCustomTypeInfo*, type);
        if ( cti )
        {
            value.Printf( "%s(%s)", cti->GetTypeName(),
                          wxAnyGetAsString(param) );
        }
        else
        {
            wxLogError ( "Internal error, illegal wxCustomTypeInfo" );
        }
    }
    else if ( type->GetKind() == wxT_STRING )
    {
        value.Printf( "\"%s\"", wxAnyGetAsString(param) );
    }
    else if ( type->GetKind() == wxT_OBJECT )
    {
        const wxClassTypeInfo* ctype = wx_dynamic_cast(const wxClassTypeInfo*,type);
        const wxClassInfo* ci = ctype->GetClassInfo();
        if( ci->NeedsDirectConstruction())
        {
            wxObjectConstructorWriter cw(ctype,this);

            ci->CallOnAny(param,&cw);

            value.Printf( "%s(%s)", ctype->GetClassInfo()->GetClassName(),
                cw.GetConstructorString() );
        }
    }
    else
    {
        value.Printf( "%s",  wxAnyGetAsString(param) );
    }

    return value;
}

void wxObjectCodeReaderCallback::CreateObject(int objectID,
                                     const wxClassInfo *WXUNUSED(classInfo),
                                     int paramCount,
                                     wxAny *params,
                                     int *objectIDValues,
                                     const wxClassInfo **WXUNUSED(objectClassInfos),
                                     wxStringToAnyHashMap &WXUNUSED(metadata)
                                     )
{
    int i;
    m_source += ( wxString::Format( "\t%s->Create(",
                       m_data->GetObjectName(objectID) ) );
    for (i = 0; i < paramCount; i++)
    {
        if ( objectIDValues[i] != wxInvalidObjectID )
        {
            wxString str =
                wxString::Format( "%s",
                                  m_data->GetObjectName( objectIDValues[i] ) );
            m_source += ( str );
        }
        else
        {
            m_source += (
                wxString::Format( "%s", ValueAsCode(params[i]) ) );
        }
        if (i < paramCount - 1)
            m_source += ( ", ");
    }
    m_source += ( ");\n" );
}

void wxObjectCodeReaderCallback::ConstructObject(int objectID,
                                     const wxClassInfo *classInfo,
                                     int paramCount,
                                     wxAny *params,
                                     int *objectIDValues,
                                     const wxClassInfo **WXUNUSED(objectClassInfos),
                                     wxStringToAnyHashMap &WXUNUSED(metadata)
                                     )
{
    wxString objectName = wxString::Format( "LocalObject_%d", objectID );
    m_source += ( wxString::Format( "\t%s *%s = new %s(",
        classInfo->GetClassName(),
        objectName,
        classInfo->GetClassName()) );
    m_data->SetObjectName( objectID, objectName );

    int i;
    for (i = 0; i < paramCount; i++)
    {
        if ( objectIDValues[i] != wxInvalidObjectID )
            m_source += ( wxString::Format( "%s",
                               m_data->GetObjectName( objectIDValues[i] ) ) );
        else
        {
            m_source += (
                wxString::Format( "%s", ValueAsCode(params[i]) ) );
        }
        if (i < paramCount - 1)
            m_source += ( ", " );
    }
    m_source += ( ");\n" );
}

void wxObjectCodeReaderCallback::SetProperty(int objectID,
                                    const wxClassInfo *WXUNUSED(classInfo),
                                    const wxPropertyInfo* propertyInfo,
                                    const wxAny &value)
{
    m_source += ( wxString::Format( "\t%s->%s(%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetSetterName(),
        ValueAsCode(value)) );
}

void wxObjectCodeReaderCallback::SetPropertyAsObject(int objectID,
                                            const wxClassInfo *WXUNUSED(classInfo),
                                            const wxPropertyInfo* propertyInfo,
                                            int valueObjectId)
{
    if ( propertyInfo->GetTypeInfo()->GetKind() == wxT_OBJECT )
        m_source += ( wxString::Format( "\t%s->%s(*%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetSetterName(),
        m_data->GetObjectName( valueObjectId) ) );
    else
        m_source += ( wxString::Format( "\t%s->%s(%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetSetterName(),
        m_data->GetObjectName( valueObjectId) ) );
}

void wxObjectCodeReaderCallback::AddToPropertyCollection( int objectID,
                                                const wxClassInfo *WXUNUSED(classInfo),
                                                const wxPropertyInfo* propertyInfo,
                                                const wxAny &value)
{
    m_source += ( wxString::Format( "\t%s->%s(%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetAdderName(),
        ValueAsCode(value)) );
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
                "\t%s->Connect( %s->GetId(), %d, "
                "(wxObjectEventFunction)(wxEventFunction) & %s::%s, NULL, %s );",
                ehsource, ehsource, eventType, ehsinkClass,
                handlerName, ehsink );

        m_source += ( code );
    }
    else
    {
        wxLogError(_("delegate has no type info"));
    }
}
