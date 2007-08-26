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

#if wxUSE_EXTENDED_RTTI

#include "wx/xtistrm.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/event.h"
#endif

#include "wx/tokenzr.h"
#include "wx/txtstrm.h"

// STL headers:

#include "wx/beforestd.h"
#include <map>
#include <vector>
#include <string>
#include "wx/afterstd.h"
using namespace std;


// ----------------------------------------------------------------------------
// wxObjectWriter
// ----------------------------------------------------------------------------

struct wxObjectWriter::wxObjectWriterInternal
{
    map< const wxObject*, int > m_writtenObjects;
    int m_nextId;
};

wxObjectWriter::wxObjectWriter()
{
    m_data = new wxObjectWriterInternal;
    m_data->m_nextId = 0;
}

wxObjectWriter::~wxObjectWriter()
{
    delete m_data;
}

struct wxObjectWriter::wxObjectWriterInternalPropertiesData
{
    char nothing;
};

void wxObjectWriter::ClearObjectContext()
{
    delete m_data;
    m_data = new wxObjectWriterInternal();
    m_data->m_nextId = 0;
}

void wxObjectWriter::WriteObject(const wxObject *object, const wxClassInfo *classInfo, 
                           wxObjectReaderCallback *persister, const wxString &name, 
                           wxVariantBaseArray &metadata )
{
    DoBeginWriteTopLevelEntry( name );
    WriteObject( object, classInfo, persister, false, metadata);
    DoEndWriteTopLevelEntry( name );
}

void wxObjectWriter::WriteObject(const wxObject *object, const wxClassInfo *classInfo, 
                           wxObjectReaderCallback *persister, bool isEmbedded, 
                           wxVariantBaseArray &metadata )
{
    if ( !classInfo->BeforeWriteObject( object, this, persister, metadata) )
        return;

    if ( persister->BeforeWriteObject( this, object, classInfo, metadata) )
    {
        if ( object == NULL )
            DoWriteNullObject();
        else if ( IsObjectKnown( object ) )
            DoWriteRepeatedObject( GetObjectID(object) );
        else
        {
            int oid = m_data->m_nextId++;
            if ( !isEmbedded )
                m_data->m_writtenObjects[object] = oid;

            // in case this object is a wxDynamicObject we also have to insert is superclass
            // instance with the same id, so that object relations are streamed out correctly
            const wxDynamicObject* dynobj = wx_dynamic_cast(const wxDynamicObject*, object);
            if ( !isEmbedded && dynobj )
                m_data->m_writtenObjects[dynobj->GetSuperClassInstance()] = oid;

            DoBeginWriteObject( object, classInfo, oid, metadata );
            wxObjectWriterInternalPropertiesData data;
            WriteAllProperties( object, classInfo, persister, &data );
            DoEndWriteObject( object, classInfo, oid  );
        }
        persister->AfterWriteObject( this,object, classInfo );
    }
}

void wxObjectWriter::FindConnectEntry(const wxEvtHandler * evSource,
                                const wxEventSourceTypeInfo* dti, 
                                const wxObject* &sink, 
                                const wxHandlerInfo *&handler)
{
    wxList *dynamicEvents = evSource->GetDynamicEventTable();

    if ( dynamicEvents )
    {
        wxList::compatibility_iterator node = dynamicEvents->GetFirst();
        while (node)
        {
            wxDynamicEventTableEntry *entry = (wxDynamicEventTableEntry*)node->GetData();

            // find the match
            if ( entry->m_fn &&
                (dti->GetEventType() == entry->m_eventType) &&
                (entry->m_id == -1 ) &&
                (entry->m_eventSink != NULL ) )
            {
                sink = entry->m_eventSink;
                const wxClassInfo* sinkClassInfo = sink->GetClassInfo();
                const wxHandlerInfo* sinkHandler = sinkClassInfo->GetFirstHandler();
                while ( sinkHandler )
                {
                    if ( sinkHandler->GetEventFunction() == entry->m_fn )
                    {
                        handler = sinkHandler;
                        break;
                    }
                    sinkHandler = sinkHandler->GetNext();
                }
                break;
            }
            node = node->GetNext();
        }
    }
}
void wxObjectWriter::WriteAllProperties( const wxObject * obj, const wxClassInfo* ci, 
                                   wxObjectReaderCallback *persister, 
                                   wxObjectWriterInternalPropertiesData * data )
{
    wxPropertyInfoMap map;
    ci->GetProperties( map );
    for ( int i = 0; i < ci->GetCreateParamCount(); ++i )
    {
        wxString name = ci->GetCreateParamName(i);
        wxPropertyInfoMap::const_iterator iter = map.find(name);
        const wxPropertyInfo* prop = iter == map.end() ? NULL : iter->second;
        if ( prop )
        {
            WriteOneProperty( obj, prop->GetDeclaringClass(), prop, persister, data );
        }
        else
        {
            wxLogError( _("Create Parameter %s not found in declared RTTI Parameters"), name.c_str() );
        }
        map.erase( name );
    }
    { // Extra block for broken compilers
        for( wxPropertyInfoMap::iterator iter = map.begin(); iter != map.end(); ++iter )
        {
            const wxPropertyInfo* prop = iter->second;
            if ( prop->GetFlags() & wxPROP_OBJECT_GRAPH )
            {
                WriteOneProperty( obj, prop->GetDeclaringClass(), prop, persister, data );
            }
        }
    }
    { // Extra block for broken compilers
        for( wxPropertyInfoMap::iterator iter = map.begin(); iter != map.end(); ++iter )
        {
            const wxPropertyInfo* prop = iter->second;
            if ( !(prop->GetFlags() & wxPROP_OBJECT_GRAPH) )
            {
                WriteOneProperty( obj, prop->GetDeclaringClass(), prop, persister, data );
            }
        }
    }
}

void wxObjectWriter::WriteOneProperty( const wxObject *obj, const wxClassInfo* ci, 
                                 const wxPropertyInfo* pi, wxObjectReaderCallback *persister, 
                                 wxObjectWriterInternalPropertiesData *WXUNUSED(data) )
{
    if ( pi->GetFlags() & wxPROP_DONT_STREAM )
        return;

    // make sure that we are picking the correct object for accessing the property
    const wxDynamicObject* dynobj = wx_dynamic_cast(const wxDynamicObject*, obj );
    if ( dynobj && (wx_dynamic_cast(const wxDynamicClassInfo*, ci) == NULL) )
        obj = dynobj->GetSuperClassInstance();

    if ( pi->GetTypeInfo()->GetKind() == wxT_COLLECTION )
    {
        wxVariantBaseArray data;
        pi->GetAccessor()->GetPropertyCollection(obj, data);
        const wxTypeInfo * elementType = 
            wx_dynamic_cast( const wxCollectionTypeInfo*, pi->GetTypeInfo() )->GetElementType();
        for ( size_t i = 0; i < data.GetCount(); ++i )
        {
            if ( i == 0 )
                DoBeginWriteProperty( pi );

            DoBeginWriteElement();
            wxVariantBase value = data[i];
            if ( persister->BeforeWriteProperty( this, obj, pi, value ) )
            {
                const wxClassTypeInfo* cti = 
                    wx_dynamic_cast( const wxClassTypeInfo*, elementType );
                if ( cti )
                {
                    const wxClassInfo* pci = cti->GetClassInfo();
                    wxObject *vobj = pci->VariantToInstance( value );
                    wxVariantBaseArray md;
                    WriteObject( vobj, (vobj ? vobj->GetClassInfo() : pci ), 
                                 persister, cti->GetKind()== wxT_OBJECT, md );
                }
                else
                {
                    DoWriteSimpleType( value );
                }
            }
            DoEndWriteElement();
            if ( i == data.GetCount() - 1 )
                 DoEndWriteProperty( pi );
       }
   }
    else
    {
        const wxEventSourceTypeInfo* dti = 
            wx_dynamic_cast( const wxEventSourceTypeInfo* , pi->GetTypeInfo() );
        if ( dti )
        {
            const wxObject* sink = NULL;
            const wxHandlerInfo *handler = NULL;

            const wxEvtHandler * evSource = wx_dynamic_cast(const wxEvtHandler *, obj);
            if ( evSource )
            {
                FindConnectEntry( evSource, dti, sink, handler );
                if ( persister->BeforeWriteDelegate( this, obj, ci, pi, sink, handler ) )
                {
                    if ( sink != NULL && handler != NULL )
                    {
                        DoBeginWriteProperty( pi );
                        if ( IsObjectKnown( sink ) )
                        {
                            DoWriteDelegate( obj, ci, pi, sink, GetObjectID( sink ), 
                                             sink->GetClassInfo(), handler );
                            DoEndWriteProperty( pi );
                        }
                        else
                        {
                            wxLogError( _T("Streaming delegates for not already ")
                                        _T("streamed objects not yet supported") );
                        }
                    }
                }
            }
            else
            {
                wxLogError(_("Illegal Object Class (Non-wxEvtHandler) as Event Source") );
            }
        }
        else
        {
            wxVariantBase value;
            pi->GetAccessor()->GetProperty(obj, value);

            // avoid streaming out void objects
            if( value.IsEmpty() )
                return;

            if ( pi->GetFlags() & wxPROP_ENUM_STORE_LONG )
            {
                const wxEnumTypeInfo *eti = 
                    wx_dynamic_cast(const wxEnumTypeInfo*,  pi->GetTypeInfo() );
                if ( eti )
                {
                    eti->ConvertFromLong( value.wxTEMPLATED_MEMBER_CALL(Get, long), value );
                }
                else
                {
                    wxLogError( _("Type must have enum - long conversion") );
                }
            }

            // avoid streaming out default values
            if ( pi->GetTypeInfo()->HasStringConverters() && 
                 !pi->GetDefaultValue().IsEmpty() )
            {
                if ( value.GetAsString() == pi->GetDefaultValue().GetAsString() )
                    return;
            }

            // avoid streaming out null objects
            const wxClassTypeInfo* cti = 
                wx_dynamic_cast( const wxClassTypeInfo* , pi->GetTypeInfo() );

            if ( cti && value.GetAsObject() == NULL )
                return;

            if ( persister->BeforeWriteProperty( this, obj, pi, value ) )
            {
                DoBeginWriteProperty( pi );
                if ( cti )
                {
                    const wxClassInfo* pci = cti->GetClassInfo();
                    wxObject *vobj = pci->VariantToInstance( value );
                    if ( vobj && pi->GetTypeInfo()->HasStringConverters() )
                    {
                        wxString stringValue;
                        cti->ConvertToString( value, stringValue );
                        wxVariantBase convertedValue(stringValue);
                        DoWriteSimpleType( convertedValue );
                    }
                    else
                    {
                        wxVariantBaseArray md;
                        WriteObject( vobj, (vobj ? vobj->GetClassInfo() : pci ), 
                                     persister, cti->GetKind()== wxT_OBJECT, md);
                    }
                }
                else
                {
                    DoWriteSimpleType( value );
                }
                DoEndWriteProperty( pi );
            }
        }
    }
}

int wxObjectWriter::GetObjectID(const wxObject *obj)
{
    if ( !IsObjectKnown( obj ) )
        return wxInvalidObjectID;

    return m_data->m_writtenObjects[obj];
}

bool wxObjectWriter::IsObjectKnown( const wxObject *obj )
{
    return m_data->m_writtenObjects.find( obj ) != m_data->m_writtenObjects.end();
}


// ----------------------------------------------------------------------------
// wxObjectReader
// ----------------------------------------------------------------------------

struct wxObjectReader::wxObjectReaderInternal
{
    map<int,wxClassInfo*> m_classInfos;
};

wxObjectReader::wxObjectReader()
{
    m_data = new wxObjectReaderInternal;
}

wxObjectReader::~wxObjectReader()
{
    delete m_data;
}

wxClassInfo* wxObjectReader::GetObjectClassInfo(int objectID)
{
    if ( objectID == wxNullObjectID || objectID == wxInvalidObjectID )
    {
        wxLogError( _("Invalid or Null Object ID passed to GetObjectClassInfo" ) );
        return NULL;
    }
    if ( m_data->m_classInfos.find(objectID) == m_data->m_classInfos.end() )
    {
        wxLogError( _("Unknown Object passed to GetObjectClassInfo" ) );
        return NULL;
    }
    return m_data->m_classInfos[objectID];
}

void wxObjectReader::SetObjectClassInfo(int objectID, wxClassInfo *classInfo )
{
    if ( objectID == wxNullObjectID || objectID == wxInvalidObjectID )
    {
        wxLogError( _("Invalid or Null Object ID passed to GetObjectClassInfo" ) );
        return;
    }
    if ( m_data->m_classInfos.find(objectID) != m_data->m_classInfos.end() )
    {
        wxLogError( _("Already Registered Object passed to SetObjectClassInfo" ) );
        return;
    }
    m_data->m_classInfos[objectID] = classInfo;
}

bool wxObjectReader::HasObjectClassInfo( int objectID )
{
    if ( objectID == wxNullObjectID || objectID == wxInvalidObjectID )
    {
        wxLogError( _("Invalid or Null Object ID passed to HasObjectClassInfo" ) );
        return false;
    }
    return m_data->m_classInfos.find(objectID) != m_data->m_classInfos.end();
}


// ----------------------------------------------------------------------------
// reading xml in
// ----------------------------------------------------------------------------

/*
Reading components has not to be extended for components
as properties are always sought by typeinfo over all levels
and create params are always toplevel class only
*/


// ----------------------------------------------------------------------------
// wxObjectRuntimeReaderCallback - depersisting to memory
// ----------------------------------------------------------------------------

struct wxObjectRuntimeReaderCallback::wxObjectRuntimeReaderCallbackInternal
{
    map<int,wxObject *> m_objects;

    void SetObject(int objectID, wxObject *obj )
    {
        if ( m_objects.find(objectID) != m_objects.end() )
        {
            wxLogError( _("Passing a already registered object to SetObject") );
            return ;
        }
        m_objects[objectID] = obj;
    }
    wxObject* GetObject( int objectID )
    {
        if ( objectID == wxNullObjectID )
            return NULL;
        if ( m_objects.find(objectID) == m_objects.end() )
        {
            wxLogError( _("Passing an unkown object to GetObject") );
            return NULL;
        }

        return m_objects[objectID];
    }
};

wxObjectRuntimeReaderCallback::wxObjectRuntimeReaderCallback()
{
    m_data = new wxObjectRuntimeReaderCallbackInternal();
}

wxObjectRuntimeReaderCallback::~wxObjectRuntimeReaderCallback()
{
    delete m_data;
}

void wxObjectRuntimeReaderCallback::AllocateObject(int objectID, wxClassInfo *classInfo,
                                          wxVariantBaseArray &WXUNUSED(metadata))
{
    wxObject *O;
    O = classInfo->CreateObject();
    m_data->SetObject(objectID, O);
}

void wxObjectRuntimeReaderCallback::CreateObject(int objectID,
                                        const wxClassInfo *classInfo,
                                        int paramCount,
                                        wxVariantBase *params,
                                        int *objectIdValues,
                                        const wxClassInfo **objectClassInfos,
                                        wxVariantBaseArray &WXUNUSED(metadata))
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    for ( int i = 0; i < paramCount; ++i )
    {
        if ( objectIdValues[i] != wxInvalidObjectID )
        {
            wxObject *o;
            o = m_data->GetObject(objectIdValues[i]);
            // if this is a dynamic object and we are asked for another class
            // than wxDynamicObject we cast it down manually.
            wxDynamicObject *dyno = wx_dynamic_cast( wxDynamicObject *, o);
            if ( dyno!=NULL && (objectClassInfos[i] != dyno->GetClassInfo()) )
            {
                o = dyno->GetSuperClassInstance();
            }
            params[i] = objectClassInfos[i]->InstanceToVariant(o);
        }
    }
    classInfo->Create(o, paramCount, params);
}

void wxObjectRuntimeReaderCallback::ConstructObject(int objectID,
                                        const wxClassInfo *classInfo,
                                        int paramCount,
                                        wxVariantBase *params,
                                        int *objectIdValues,
                                        const wxClassInfo **objectClassInfos,
                                        wxVariantBaseArray &WXUNUSED(metadata))
{
    wxObject *o;
    for ( int i = 0; i < paramCount; ++i )
    {
        if ( objectIdValues[i] != wxInvalidObjectID )
        {
            wxObject *o;
            o = m_data->GetObject(objectIdValues[i]);
            // if this is a dynamic object and we are asked for another class
            // than wxDynamicObject we cast it down manually.
            wxDynamicObject *dyno = wx_dynamic_cast( wxDynamicObject *, o);
            if ( dyno!=NULL && (objectClassInfos[i] != dyno->GetClassInfo()) )
            {
                o = dyno->GetSuperClassInstance();
            }
            params[i] = objectClassInfos[i]->InstanceToVariant(o);
        }
    }
    o = classInfo->ConstructObject(paramCount, params);
    m_data->SetObject(objectID, o);
}


void wxObjectRuntimeReaderCallback::DestroyObject(int objectID, wxClassInfo *WXUNUSED(classInfo))
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    delete o;
}

void wxObjectRuntimeReaderCallback::SetProperty(int objectID,
                                       const wxClassInfo *classInfo,
                                       const wxPropertyInfo* propertyInfo,
                                       const wxVariantBase &value)
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    classInfo->SetProperty( o, propertyInfo->GetName(), value );
}

void wxObjectRuntimeReaderCallback::SetPropertyAsObject(int objectID,
                                               const wxClassInfo *classInfo,
                                               const wxPropertyInfo* propertyInfo,
                                               int valueObjectId)
{
    wxObject *o, *valo;
    o = m_data->GetObject(objectID);
    valo = m_data->GetObject(valueObjectId);
    const wxClassInfo* valClassInfo = 
        (wx_dynamic_cast(const wxClassTypeInfo*,propertyInfo->GetTypeInfo()))->GetClassInfo();

    // if this is a dynamic object and we are asked for another class
    // than wxDynamicObject we cast it down manually.
    wxDynamicObject *dynvalo = wx_dynamic_cast( wxDynamicObject *, valo);
    if ( dynvalo!=NULL  && (valClassInfo != dynvalo->GetClassInfo()) )
    {
        valo = dynvalo->GetSuperClassInstance();
    }

    classInfo->SetProperty( o, propertyInfo->GetName(), 
                            valClassInfo->InstanceToVariant(valo) );
}

void wxObjectRuntimeReaderCallback::SetConnect(int eventSourceObjectID,
                                      const wxClassInfo *WXUNUSED(eventSourceClassInfo),
                                      const wxPropertyInfo *delegateInfo,
                                      const wxClassInfo *WXUNUSED(eventSinkClassInfo),
                                      const wxHandlerInfo* handlerInfo,
                                      int eventSinkObjectID )
{
    wxEvtHandler *ehsource = 
        wx_dynamic_cast( wxEvtHandler* , m_data->GetObject( eventSourceObjectID ) );
    wxEvtHandler *ehsink = 
        wx_dynamic_cast( wxEvtHandler *,m_data->GetObject(eventSinkObjectID) );

    if ( ehsource && ehsink )
    {
        const wxEventSourceTypeInfo *delegateTypeInfo = 
            wx_dynamic_cast(const wxEventSourceTypeInfo*,delegateInfo->GetTypeInfo());
        if( delegateTypeInfo && delegateTypeInfo->GetLastEventType() == -1 )
        {
            ehsource->Connect( -1, delegateTypeInfo->GetEventType(),
                handlerInfo->GetEventFunction(), NULL /*user data*/,
                ehsink );
        }
        else
        {
            for ( wxEventType iter = delegateTypeInfo->GetEventType(); 
                  iter <= delegateTypeInfo->GetLastEventType(); ++iter )
            {
                ehsource->Connect( -1, iter,
                    handlerInfo->GetEventFunction(), NULL /*user data*/,
                    ehsink );
            }
        }
    }
}

wxObject *wxObjectRuntimeReaderCallback::GetObject(int objectID)
{
    return m_data->GetObject( objectID );
}

void wxObjectRuntimeReaderCallback::AddToPropertyCollection( int objectID,
                                                   const wxClassInfo *classInfo,
                                                   const wxPropertyInfo* propertyInfo,
                                                   const wxVariantBase &value)
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    classInfo->AddToPropertyCollection( o, propertyInfo->GetName(), value );
}

void wxObjectRuntimeReaderCallback::AddToPropertyCollectionAsObject(int objectID,
                                                           const wxClassInfo *classInfo,
                                                           const wxPropertyInfo* propertyInfo,
                                                           int valueObjectId)
{
    wxObject *o, *valo;
    o = m_data->GetObject(objectID);
    valo = m_data->GetObject(valueObjectId);
    const wxCollectionTypeInfo * collectionTypeInfo = 
        wx_dynamic_cast( const wxCollectionTypeInfo *, propertyInfo->GetTypeInfo() );
    const wxClassInfo* valClassInfo = 
        (wx_dynamic_cast(const wxClassTypeInfo*,collectionTypeInfo->GetElementType()))->GetClassInfo();

    // if this is a dynamic object and we are asked for another class
    // than wxDynamicObject we cast it down manually.
    wxDynamicObject *dynvalo = wx_dynamic_cast( wxDynamicObject *, valo);
    if ( dynvalo!=NULL  && (valClassInfo != dynvalo->GetClassInfo()) )
    {
        valo = dynvalo->GetSuperClassInstance();
    }

    classInfo->AddToPropertyCollection( o, propertyInfo->GetName(), 
                                        valClassInfo->InstanceToVariant(valo) );
}

#if TEST_XVARIANT
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxVariantBaseArray);
#endif

#endif // wxUSE_EXTENDED_RTTI
