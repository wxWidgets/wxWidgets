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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "xtistrm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/hash.h"
#include "wx/object.h"
#endif

#include "wx/xml/xml.h"
#include "wx/tokenzr.h"
#include "wx/xtistrm.h"
#include "wx/txtstrm.h"

#if wxUSE_EXTENDED_RTTI

#include "wx/beforestd.h"
#include <map>
#include <vector>
#include <string>
#include "wx/afterstd.h"

using namespace std ;

struct wxWriter::wxWriterInternal
{
    map< const wxObject* , int > m_writtenObjects ;
    int m_nextId ;
} ;

wxWriter::wxWriter()
{
    m_data = new wxWriterInternal ;
    m_data->m_nextId = 0 ;
}

wxWriter::~wxWriter()
{
    delete m_data ;
}

struct wxWriter::wxWriterInternalPropertiesData
{
    map< string , int > m_writtenProperties ;
} ;

void wxWriter::ClearObjectContext()
{
    delete m_data ;
    m_data = new wxWriterInternal() ;
    m_data->m_nextId = 0 ;
}

void wxWriter::WriteObject(const wxObject *object, const wxClassInfo *classInfo , wxPersister *persister , const wxString &name )
{
    DoBeginWriteTopLevelEntry( name ) ;
    WriteObject( object , classInfo , persister , false ) ;
    DoEndWriteTopLevelEntry( name ) ;
}

void wxWriter::WriteObject(const wxObject *object, const wxClassInfo *classInfo , wxPersister *persister , bool isEmbedded)
{
    // hack to avoid writing out embedded windows, these are windows that are constructed as part of other windows, they would
    // doubly constructed afterwards

    const wxWindow * win = dynamic_cast<const wxWindow*>(object) ;
    if ( win && win->GetId() < 0 )
        return ;

    if ( persister->BeforeWriteObject( this , object , classInfo ) )
    {
        if ( object == NULL )
            DoWriteNullObject() ;
        else if ( IsObjectKnown( object ) )
            DoWriteRepeatedObject( GetObjectID(object) ) ;
        else
        {
            int oid = m_data->m_nextId++ ;
            if ( !isEmbedded )
                m_data->m_writtenObjects[object] = oid ;

            // in case this object is a wxDynamicObject we also have to insert is superclass
            // instance with the same id, so that object relations are streamed out correctly
            const wxDynamicObject* dynobj = dynamic_cast<const wxDynamicObject *>( object ) ;
            if ( !isEmbedded && dynobj )
                m_data->m_writtenObjects[dynobj->GetSuperClassInstance()] = oid ;

            DoBeginWriteObject( object , classInfo , oid ) ;
            wxWriterInternalPropertiesData data ;
            WriteAllProperties( object , classInfo , persister , &data ) ;
            DoEndWriteObject( object , classInfo , oid  ) ;
        }
        persister->AfterWriteObject( this ,object , classInfo ) ;
    }
}

void wxWriter::FindConnectEntry(const wxWindow * evSource,const wxDelegateTypeInfo* dti, const wxObject* &sink , const wxHandlerInfo *&handler)
{
    wxList *dynamicEvents = evSource->GetDynamicEventTable() ;

    if ( dynamicEvents )
    {
        wxList::compatibility_iterator node = dynamicEvents->GetFirst();
        while (node)
        {
            wxDynamicEventTableEntry *entry = (wxDynamicEventTableEntry*)node->GetData();

            // find the match
            if ( entry->m_fn && (dti->GetEventType() == entry->m_eventType) &&
                (entry->m_id == -1 ||
                (entry->m_lastId == -1 && evSource->GetId() == entry->m_id) ||
                (entry->m_lastId != -1 &&
                (evSource->GetId()  >= entry->m_id && evSource->GetId() <= entry->m_lastId) ) ) &&
                entry->m_eventSink
                )
            {
                sink = entry->m_eventSink ;
                const wxClassInfo* sinkClassInfo = sink->GetClassInfo() ;
                const wxHandlerInfo* sinkHandler = sinkClassInfo->GetFirstHandler() ;
                while ( sinkHandler )
                {
                    if ( sinkHandler->GetEventFunction() == entry->m_fn )
                    {
                        handler = sinkHandler ;
                        break ;
                    }
                    sinkHandler = sinkHandler->GetNext() ;
                }
                break ;
            }
            node = node->GetNext();
        }
    }
}
void wxWriter::WriteAllProperties( const wxObject * obj , const wxClassInfo* ci , wxPersister *persister, wxWriterInternalPropertiesData * data )
{
   // in case this object is wxDynamic object we have to hand over the streaming
    // of the properties of the superclasses to the real super class instance
    {
        const wxObject *iterobj = obj ;
        const wxDynamicObject* dynobj = dynamic_cast< const wxDynamicObject* > (iterobj ) ;
        if ( dynobj )
            iterobj = dynobj->GetSuperClassInstance() ;
        const wxClassInfo** parents = ci->GetParents() ;
        for ( int i = 0 ; parents[i] ; ++ i )
        {
            WriteAllProperties( iterobj , parents[i] , persister , data ) ;
        }
    }

    const wxPropertyInfo *pi = ci->GetFirstProperty() ;
    while( pi ) 
    {
        // this property was not written yet in this object and we don't get a veto
        // if ( data->m_writtenProperties.find( pi->GetName() ) == data->m_writtenProperties.end() )
        // we will have to handle property overrides differently 
        {
            data->m_writtenProperties[ pi->GetName() ] = 1 ;
            DoBeginWriteProperty( pi ) ;
            if ( pi->GetTypeInfo()->GetKind() == wxT_COLLECTION )
            {
                wxxVariantArray data ;
                pi->GetAccessor()->GetPropertyCollection(obj, data) ;
                const wxTypeInfo * elementType = dynamic_cast< const wxCollectionTypeInfo* >( pi->GetTypeInfo() )->GetElementType() ;
                for ( size_t i = 0 ; i < data.GetCount() ; ++i )
                {
                    DoBeginWriteElement() ;
                    wxxVariant value = data[i] ;
                    if ( persister->BeforeWriteProperty( this , pi , value ) )
                    {
                        const wxClassTypeInfo* cti = dynamic_cast< const wxClassTypeInfo* > ( elementType ) ;
                        if ( cti )
                        {
                            const wxClassInfo* pci = cti->GetClassInfo() ;
                            wxObject *vobj = pci->VariantToInstance( value ) ;
                            WriteObject( vobj , (vobj ? vobj->GetClassInfo() : pci ) , persister , cti->GetKind()== wxT_OBJECT ) ;
                        }
                        else
                        {                               
                            DoWriteSimpleType( value ) ;
                        }
                    }
                    DoEndWriteElement() ;
                }
            }
            else
            {
                const wxDelegateTypeInfo* dti = dynamic_cast< const wxDelegateTypeInfo* > ( pi->GetTypeInfo() ) ;
                if ( dti )
                {
                    const wxObject* sink = NULL ;
                    const wxHandlerInfo *handler = NULL ;

                    const wxWindow * evSource = dynamic_cast<const wxWindow *>(obj) ;
                    wxASSERT_MSG( evSource , wxT("Illegal Object Class (Non-Window) as Event Source") ) ;

                    FindConnectEntry( evSource , dti , sink , handler ) ;
                    if ( persister->BeforeWriteDelegate( this , obj , ci , pi , sink , handler ) )
                    {
                        if ( sink != NULL && handler != NULL )
                        {
                            wxASSERT_MSG( IsObjectKnown( sink ) , wxT("Streaming delegates for not already streamed objects not yet supported") ) ;
                            DoWriteDelegate( obj , ci , pi , sink , GetObjectID( sink ) , sink->GetClassInfo() , handler ) ;
                        }
                    }
                }
                else
                {
                    wxxVariant value ;
                    pi->GetAccessor()->GetProperty(obj, value) ;
                    if ( persister->BeforeWriteProperty( this , pi , value ) )
                    {
                        const wxClassTypeInfo* cti = dynamic_cast< const wxClassTypeInfo* > ( pi->GetTypeInfo() ) ;
                        if ( cti )
                        {
                            const wxClassInfo* pci = cti->GetClassInfo() ;
                            wxObject *vobj = pci->VariantToInstance( value ) ;
                            WriteObject( vobj , (vobj ? vobj->GetClassInfo() : pci ) , persister , cti->GetKind()== wxT_OBJECT ) ;
                        }
                        else
                        {                               
                            DoWriteSimpleType( value ) ;
                        }
                    }
                }
            }
            DoEndWriteProperty( pi ) ;
        }
        pi = pi->GetNext() ;
    }
 }

int wxWriter::GetObjectID(const wxObject *obj) 
{
    if ( !IsObjectKnown( obj ) )
        return wxInvalidObjectID ;

    return m_data->m_writtenObjects[obj] ;
}

bool wxWriter::IsObjectKnown( const wxObject *obj ) 
{
    return m_data->m_writtenObjects.find( obj ) != m_data->m_writtenObjects.end() ;
}

//
// XML Streaming
// 

// convenience functions

void wxXmlAddContentToNode( wxXmlNode* node , const wxString& data )
{
    node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "value", data ) );
}

wxString wxXmlGetContentFromNode( wxXmlNode *node )
{
    if ( node->GetChildren() )
        return node->GetChildren()->GetContent() ;
    else
        return wxEmptyString ;
}

struct wxXmlWriter::wxXmlWriterInternal
{
    wxXmlNode *m_root ;
    wxXmlNode *m_current ;
    vector< wxXmlNode * > m_objectStack ;

    void Push( wxXmlNode *newCurrent )
    {
        m_objectStack.push_back( m_current ) ;
        m_current = newCurrent ;
    }

    void Pop()
    {
        m_current = m_objectStack.back() ;
        m_objectStack.pop_back() ;
    }
} ;

wxXmlWriter::wxXmlWriter( wxXmlNode * rootnode ) 
{
    m_data = new wxXmlWriterInternal() ;
    m_data->m_root = rootnode ;
    m_data->m_current = rootnode ;
}

wxXmlWriter::~wxXmlWriter() 
{
    delete m_data ;
}

void wxXmlWriter::DoBeginWriteTopLevelEntry( const wxString &name ) 
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("entry"));
    pnode->AddProperty(wxString("name"), name);
    m_data->m_current->AddChild(pnode) ;
    m_data->Push( pnode ) ;
}

void wxXmlWriter::DoEndWriteTopLevelEntry( const wxString &WXUNUSED(name) )
{
    m_data->Pop() ;
}

void wxXmlWriter::DoBeginWriteObject(const wxObject *WXUNUSED(object), const wxClassInfo *classInfo, int objectID   ) 
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("object"));
    pnode->AddProperty(wxT("class"), wxString(classInfo->GetClassName()));
    pnode->AddProperty(wxT("id"), wxString::Format( "%d" , objectID ) );

    m_data->m_current->AddChild(pnode) ;
    m_data->Push( pnode ) ;
}

// end of writing the root object
void wxXmlWriter::DoEndWriteObject(const wxObject *WXUNUSED(object), const wxClassInfo *WXUNUSED(classInfo), int WXUNUSED(objectID) ) 
{
    m_data->Pop() ;
}

// writes a property in the stream format
void wxXmlWriter::DoWriteSimpleType( wxxVariant &value ) 
{
    wxXmlAddContentToNode( m_data->m_current ,value.GetAsString() ) ;
}

void wxXmlWriter::DoBeginWriteElement() 
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, "element" );
    m_data->m_current->AddChild(pnode) ;
    m_data->Push( pnode ) ;
}

void wxXmlWriter::DoEndWriteElement() 
{
    m_data->Pop() ;
}

void wxXmlWriter::DoBeginWriteProperty(const wxPropertyInfo *pi )
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, "prop" );
    pnode->AddProperty(wxT("name"), pi->GetName() );
    m_data->m_current->AddChild(pnode) ;
    m_data->Push( pnode ) ;
}

void wxXmlWriter::DoEndWriteProperty(const wxPropertyInfo *WXUNUSED(propInfo) )
{
    m_data->Pop() ;
}



// insert an object reference to an already written object
void wxXmlWriter::DoWriteRepeatedObject( int objectID ) 
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("object"));
    pnode->AddProperty(wxString("href"), wxString::Format( "%d" , objectID ) );
    m_data->m_current->AddChild(pnode) ;
}

// insert a null reference
void wxXmlWriter::DoWriteNullObject() 
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("object"));
    m_data->m_current->AddChild(pnode) ;
}

// writes a delegate in the stream format
void wxXmlWriter::DoWriteDelegate( const wxObject *WXUNUSED(object),  const wxClassInfo* WXUNUSED(classInfo) , const wxPropertyInfo *WXUNUSED(pi) , 
                                  const wxObject *eventSink, int sinkObjectID , const wxClassInfo* WXUNUSED(eventSinkClassInfo) , const wxHandlerInfo* handlerInfo ) 
{
    if ( eventSink != NULL && handlerInfo != NULL )
    {
        wxXmlAddContentToNode( m_data->m_current ,wxString::Format(wxT("%d.%s"), sinkObjectID , handlerInfo->GetName()) ) ;
    }
}

// ----------------------------------------------------------------------------
// reading objects in 
// ----------------------------------------------------------------------------

struct wxReader::wxReaderInternal
{
    map<int,wxClassInfo*> m_classInfos;
};

wxReader::wxReader() 
{
    m_data = new wxReaderInternal;
}

wxReader::~wxReader()
{
    delete m_data;
}

wxClassInfo* wxReader::GetObjectClassInfo(int objectID)
{
    assert( m_data->m_classInfos.find(objectID) != m_data->m_classInfos.end() );
    return m_data->m_classInfos[objectID] ;
}

void wxReader::SetObjectClassInfo(int objectID, wxClassInfo *classInfo )
{
    assert(  m_data->m_classInfos.find(objectID) == m_data->m_classInfos.end()  ) ;
    m_data->m_classInfos[objectID] = classInfo ;
}

bool wxReader::HasObjectClassInfo( int objectID ) 
{
    return m_data->m_classInfos.find(objectID) != m_data->m_classInfos.end() ;
}


// ----------------------------------------------------------------------------
// reading xml in 
// ----------------------------------------------------------------------------

/* 
Reading components has not to be extended for components
as properties are always sought by typeinfo over all levels
and create params are always toplevel class only
*/

int wxXmlReader::ReadComponent(wxXmlNode *node, wxDepersister *callbacks)
{
    wxASSERT_MSG( callbacks , wxT("Does not support reading without a Depersistor") ) ;
    wxString className;
    wxClassInfo *classInfo;

    wxxVariant *createParams ;
    int *createParamOids ;
    const wxClassInfo** createClassInfos ;
    wxXmlNode *children;
    int objectID;
    wxString ObjectIdString ;

    children = node->GetChildren();
    if (!children)
    {
        // check for a null object or href
        if (node->GetPropVal("href" , &ObjectIdString ) )
        {
            objectID = atoi( ObjectIdString.c_str() ) ;
            wxASSERT_MSG( HasObjectClassInfo( objectID ) , wxT("Forward hrefs are not supported") ) ;
            return objectID ;
        }
        if ( !node->GetPropVal("id" , &ObjectIdString ) )
        {
            return wxNullObjectID;
        }
    }
    if (!node->GetPropVal("class", &className))
    {
        // No class name.  Eek. FIXME: error handling
        return wxInvalidObjectID;
    }
    classInfo = wxClassInfo::FindClass(className);
    wxASSERT_MSG( classInfo , wxString::Format(wxT("unknown class %s"),className ) ) ;
    wxASSERT_MSG( !children || children->GetType() != wxXML_TEXT_NODE , wxT("objects cannot have XML Text Nodes") ) ;
    if (!node->GetPropVal("id", &ObjectIdString))
    {
        wxASSERT_MSG(0,wxT("Objects must have an id attribute") ) ;
        // No object id.  Eek. FIXME: error handling
        return wxInvalidObjectID;
    }
    objectID = atoi( ObjectIdString.c_str() ) ;
    // is this object already has been streamed in, return it here
    wxASSERT_MSG( !HasObjectClassInfo( objectID ) , wxString::Format(wxT("Doubly used id : %d"), objectID ) ) ;

    // new object, start with allocation
    // first make the object know to our internal registry
    SetObjectClassInfo( objectID , classInfo ) ;

    callbacks->AllocateObject(objectID, classInfo);

    // 
    // stream back the Create parameters first
    createParams = new wxxVariant[ classInfo->GetCreateParamCount() ] ;
    createParamOids = new int[classInfo->GetCreateParamCount() ] ;
    createClassInfos = new const wxClassInfo*[classInfo->GetCreateParamCount() ] ;

    typedef map<string, wxXmlNode *> PropertyNodes ;
    typedef vector<string> PropertyNames ;

    PropertyNodes propertyNodes ;
    PropertyNames propertyNames ;

    while( children )
    {
        wxString name ;
        children->GetPropVal( wxT("name") , &name ) ;
        propertyNames.push_back( name.c_str() ) ;
        propertyNodes[name.c_str()] = children->GetChildren() ;
        children = children->GetNext() ;
    }

    for ( int i = 0 ; i <classInfo->GetCreateParamCount() ; ++i )
    {
        const wxChar* paramName = classInfo->GetCreateParamName(i) ;
        PropertyNodes::iterator propiter = propertyNodes.find( paramName ) ;
        const wxPropertyInfo* pi = classInfo->FindPropertyInfo( paramName ) ;
        wxASSERT_MSG(pi,wxString::Format("Unkown Property %s",paramName) ) ;
        // if we don't have the value of a create param set in the xml
        // we use the default value
        if ( propiter != propertyNodes.end() )
        {
            wxXmlNode* prop = propiter->second ;
            if ( pi->GetTypeInfo()->IsObjectType() )
            {
                createParamOids[i] = ReadComponent( prop , callbacks ) ;
                createClassInfos[i] = dynamic_cast<const wxClassTypeInfo*>(pi->GetTypeInfo())->GetClassInfo() ;
            }
            else
            {
                createParamOids[i] = wxInvalidObjectID ;
                createParams[i] = ReadValue( prop , pi->GetTypeInfo() ) ;
                createClassInfos[i] = NULL ;
            }

            for ( size_t j = 0 ; j < propertyNames.size() ; ++j )
            {
                if ( propertyNames[j] == paramName )
                {
                    propertyNames[j] = "" ;
                    break ;
                }
            }
        }
        else
        {
            createParams[i] = pi->GetDefaultValue() ;
        }
    }

    // got the parameters.  Call the Create method
    callbacks->CreateObject(objectID, classInfo,
        classInfo->GetCreateParamCount(),
        createParams, createParamOids, createClassInfos);

    // now stream in the rest of the properties, in the sequence their properties were written in the xml
    for ( size_t j = 0 ; j < propertyNames.size() ; ++j )
    {
        if ( propertyNames[j].length() )
        {
            PropertyNodes::iterator propiter = propertyNodes.find( propertyNames[j] ) ;
            if ( propiter != propertyNodes.end() )
            {
                wxXmlNode* prop = propiter->second ;
                const wxPropertyInfo* pi = classInfo->FindPropertyInfo( propertyNames[j].c_str() ) ;
                if ( pi->GetTypeInfo()->GetKind() == wxT_COLLECTION )
                {
                    const wxCollectionTypeInfo* collType = dynamic_cast< const wxCollectionTypeInfo* >( pi->GetTypeInfo() ) ;
                    const wxTypeInfo * elementType = collType->GetElementType() ;
                    while( prop )
                    {
                        wxASSERT_MSG(prop->GetName() == wxT("element") , wxT("A non empty collection must consist of 'element' nodes")) ;
                        wxXmlNode* elementContent = prop->GetChildren() ;
                        if ( elementContent )
                        {
                            // we skip empty elements
                            if ( elementType->IsObjectType() )
                            {
                                int valueId = ReadComponent( elementContent , callbacks ) ;
                                if ( valueId != wxInvalidObjectID )
                                {
                                    if ( pi->GetAccessor()->HasAdder() )
                                        callbacks->AddToPropertyCollectionAsObject( objectID , classInfo , pi , valueId ) ;
                                    // TODO for collections we must have a notation on taking over ownership or not 
                                    if ( elementType->GetKind() == wxT_OBJECT && valueId != wxNullObjectID )
                                        callbacks->DestroyObject( valueId , GetObjectClassInfo( valueId ) ) ;
                                }
                            }
                            else
                            {
                                wxxVariant elementValue = ReadValue( elementContent , elementType ) ;
                                if ( pi->GetAccessor()->HasAdder() )
                                    callbacks->AddToPropertyCollection( objectID , classInfo ,pi , elementValue ) ;
                            }
                        }
                        prop = prop->GetNext() ;
                    }
                }
                else if ( pi->GetTypeInfo()->IsObjectType() )
                {
                    int valueId = ReadComponent( prop , callbacks ) ;
                    if ( valueId != wxInvalidObjectID )
                    {
                        callbacks->SetPropertyAsObject( objectID , classInfo , pi , valueId ) ;
                        if ( pi->GetTypeInfo()->GetKind() == wxT_OBJECT && valueId != wxNullObjectID )
                            callbacks->DestroyObject( valueId , GetObjectClassInfo( valueId ) ) ;
                    }
                }
                else if ( pi->GetTypeInfo()->IsDelegateType() )
                {
                    if ( prop )
                    {
                        wxString resstring = prop->GetContent() ;
                        wxInt32 pos = resstring.Find('.') ;
                        assert( pos != wxNOT_FOUND ) ;
                        int sinkOid = atol(resstring.Left(pos)) ;
                        wxString handlerName = resstring.Mid(pos+1) ;
                        wxClassInfo* sinkClassInfo = GetObjectClassInfo( sinkOid ) ;

                        callbacks->SetConnect( objectID , classInfo , dynamic_cast<const wxDelegateTypeInfo*>(pi->GetTypeInfo()) , sinkClassInfo ,
                        sinkClassInfo->FindHandlerInfo(handlerName) ,  sinkOid ) ;
                    }

                }
                else
                {
                    wxxVariant nodeval ;
                    callbacks->SetProperty( objectID, classInfo ,pi , ReadValue( prop , pi->GetTypeInfo() ) ) ;
                }
            }
        }
    }

    delete[] createParams ;
    delete[] createParamOids ;
    delete[] createClassInfos ;

    return objectID;
}

wxxVariant wxXmlReader::ReadValue(wxXmlNode *node,
                                  const wxTypeInfo *type )
{
    wxString content ;
    if ( node )
        content = node->GetContent() ;
    wxxVariant result ;
    type->ConvertFromString( content , result ) ;
    return result ;
}

int wxXmlReader::ReadObject( const wxString &name , wxDepersister *callbacks)
{
    wxXmlNode *iter = m_parent->GetChildren() ;
    while ( iter )
    {
        wxString entryName ;
        if ( iter->GetPropVal("name", &entryName) )
        {
            if ( entryName == name )
                return ReadComponent( iter->GetChildren() , callbacks ) ;
        }
        iter = iter->GetNext() ;
    }
    return wxInvalidObjectID ;
}

// ----------------------------------------------------------------------------
// depersisting to memory 
// ----------------------------------------------------------------------------

struct wxRuntimeDepersister::wxRuntimeDepersisterInternal
{
    map<int,wxObject *> m_objects;

    void SetObject(int objectID, wxObject *obj )
    {
        assert(  m_objects.find(objectID) == m_objects.end()  ) ;
        m_objects[objectID] = obj ;
    }
    wxObject* GetObject( int objectID )
    {
        if ( objectID == wxNullObjectID )
            return NULL ;

        assert(  m_objects.find(objectID) != m_objects.end()  ) ;
        return m_objects[objectID] ;
    }
} ;

wxRuntimeDepersister::wxRuntimeDepersister()
{
    m_data = new wxRuntimeDepersisterInternal() ;
}

wxRuntimeDepersister::~wxRuntimeDepersister()
{
    delete m_data ;
}

void wxRuntimeDepersister::AllocateObject(int objectID, wxClassInfo *classInfo)
{
    wxObject *O;
    O = classInfo->CreateObject();
    m_data->SetObject(objectID, O);
}

void wxRuntimeDepersister::CreateObject(int objectID,
                                        const wxClassInfo *classInfo,
                                        int paramCount,
                                        wxxVariant *params,
                                        int *objectIdValues,
                                        const wxClassInfo **objectClassInfos)
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    for ( int i = 0 ; i < paramCount ; ++i )
    {
        if ( objectIdValues[i] != wxInvalidObjectID )
        {
            wxObject *o;
            o = m_data->GetObject(objectIdValues[i]);
            // if this is a dynamic object and we are asked for another class
            // than wxDynamicObject we cast it down manually.
            wxDynamicObject *dyno = dynamic_cast< wxDynamicObject * > (o) ;
            if ( dyno!=NULL && (objectClassInfos[i] != dyno->GetClassInfo()) )
            {
                o = dyno->GetSuperClassInstance() ;
            }
            params[i] = objectClassInfos[i]->InstanceToVariant(o) ;
        }
    }
    classInfo->Create(o, paramCount, params);
}

void wxRuntimeDepersister::DestroyObject(int objectID, wxClassInfo *WXUNUSED(classInfo)) 
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    delete o ;
}

void wxRuntimeDepersister::SetProperty(int objectID,
                                       const wxClassInfo *classInfo,
                                       const wxPropertyInfo* propertyInfo,
                                       const wxxVariant &value)
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    classInfo->SetProperty( o , propertyInfo->GetName() , value ) ;
}

void wxRuntimeDepersister::SetPropertyAsObject(int objectID,
                                               const wxClassInfo *classInfo,
                                               const wxPropertyInfo* propertyInfo,
                                               int valueObjectId)
{
    wxObject *o, *valo;
    o = m_data->GetObject(objectID);
    valo = m_data->GetObject(valueObjectId);
    const wxClassInfo* valClassInfo = (dynamic_cast<const wxClassTypeInfo*>(propertyInfo->GetTypeInfo()))->GetClassInfo() ;
    // if this is a dynamic object and we are asked for another class
    // than wxDynamicObject we cast it down manually.
    wxDynamicObject *dynvalo = dynamic_cast< wxDynamicObject * > (valo) ;
    if ( dynvalo!=NULL  && (valClassInfo != dynvalo->GetClassInfo()) )
    {
        valo = dynvalo->GetSuperClassInstance() ;
    }

    classInfo->SetProperty( o , propertyInfo->GetName() , valClassInfo->InstanceToVariant(valo) ) ;
}

void wxRuntimeDepersister::SetConnect(int eventSourceObjectID,
                                      const wxClassInfo *WXUNUSED(eventSourceClassInfo),
                                      const wxDelegateTypeInfo *delegateInfo ,
                                      const wxClassInfo *WXUNUSED(eventSinkClassInfo) ,
                                      const wxHandlerInfo* handlerInfo ,
                                      int eventSinkObjectID )
{
    wxWindow *ehsource = dynamic_cast< wxWindow* >( m_data->GetObject( eventSourceObjectID ) ) ;
    wxEvtHandler *ehsink = dynamic_cast< wxEvtHandler *>(m_data->GetObject(eventSinkObjectID) ) ;

    if ( ehsource && ehsink )
    {
        ehsource->Connect( ehsource->GetId() , delegateInfo->GetEventType() , 
            handlerInfo->GetEventFunction() , NULL /*user data*/ , 
            ehsink ) ;
    }
}

wxObject *wxRuntimeDepersister::GetObject(int objectID)
{
    return m_data->GetObject( objectID ) ;
}

// adds an element to a property collection
void wxRuntimeDepersister::AddToPropertyCollection( int objectID ,
    const wxClassInfo *classInfo,
    const wxPropertyInfo* propertyInfo ,
    const wxxVariant &value) 
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    classInfo->AddToPropertyCollection( o , propertyInfo->GetName() , value ) ;
}

// sets the corresponding property (value is an object)
void wxRuntimeDepersister::AddToPropertyCollectionAsObject(int objectID,
    const wxClassInfo *classInfo,
    const wxPropertyInfo* propertyInfo ,
    int valueObjectId) 
{
    wxObject *o, *valo;
    o = m_data->GetObject(objectID);
    valo = m_data->GetObject(valueObjectId);
    const wxCollectionTypeInfo * collectionTypeInfo = dynamic_cast< const wxCollectionTypeInfo * >(propertyInfo->GetTypeInfo() ) ;
    const wxClassInfo* valClassInfo = (dynamic_cast<const wxClassTypeInfo*>(collectionTypeInfo->GetElementType()))->GetClassInfo() ;
    // if this is a dynamic object and we are asked for another class
    // than wxDynamicObject we cast it down manually.
    wxDynamicObject *dynvalo = dynamic_cast< wxDynamicObject * > (valo) ;
    if ( dynvalo!=NULL  && (valClassInfo != dynvalo->GetClassInfo()) )
    {
        valo = dynvalo->GetSuperClassInstance() ;
    }

    classInfo->AddToPropertyCollection( o , propertyInfo->GetName() , valClassInfo->InstanceToVariant(valo) ) ;
}

// ----------------------------------------------------------------------------
// depersisting to code 
// ----------------------------------------------------------------------------

struct wxCodeDepersister::wxCodeDepersisterInternal
{
    map<int,string> m_objectNames ;

    void SetObjectName(int objectID, const wxString &name )
    {
        assert(  m_objectNames.find(objectID) == m_objectNames.end()  ) ;
        m_objectNames[objectID] = (const char *)name;
    }
    wxString GetObjectName( int objectID )
    {
        if ( objectID == wxNullObjectID )
            return "NULL" ;

        assert(  m_objectNames.find(objectID) != m_objectNames.end()  ) ;
        return wxString( m_objectNames[objectID].c_str() ) ;
    }
} ;

wxCodeDepersister::wxCodeDepersister(wxTextOutputStream *out) 
: m_fp(out)
{
    m_data = new wxCodeDepersisterInternal ;
}

wxCodeDepersister::~wxCodeDepersister()
{
    delete m_data ;
}

void wxCodeDepersister::AllocateObject(int objectID, wxClassInfo *classInfo)
{
    wxString objectName = wxString::Format( "LocalObject_%d" , objectID ) ;
    m_fp->WriteString( wxString::Format( "\t%s *%s = new %s;\n",
        classInfo->GetClassName(),
        objectName,
        classInfo->GetClassName()) );
    m_data->SetObjectName( objectID , objectName ) ;
}

void wxCodeDepersister::DestroyObject(int objectID, wxClassInfo *WXUNUSED(classInfo)) 
{
    m_fp->WriteString( wxString::Format( "\tdelete %s;\n",
        m_data->GetObjectName( objectID) ) );
}

wxString wxCodeDepersister::ValueAsCode( const wxxVariant &param )
{
    wxString value ;
    const wxTypeInfo* type = param.GetTypeInfo() ;
    if ( type->GetKind() == wxT_CUSTOM )
    {
        const wxCustomTypeInfo* cti = dynamic_cast<const wxCustomTypeInfo*>(type) ;
        wxASSERT_MSG( cti , wxT("Internal error, illegal wxCustomTypeInfo") ) ;
        value.Printf( "%s(%s)",cti->GetTypeName(),param.GetAsString() );
    }
    else if ( type->GetKind() == wxT_STRING )
    {
        value.Printf( "\"%s\"",param.GetAsString() );
    }
    else
    {
        value.Printf( "%s", param.GetAsString() );
    }
    return value ;
}

void wxCodeDepersister::CreateObject(int objectID,
                                     const wxClassInfo *WXUNUSED(classInfo),
                                     int paramCount,
                                     wxxVariant *params,
                                     int *objectIDValues,
                                     const wxClassInfo **WXUNUSED(objectClassInfos)
                                     )
{
    int i;
    m_fp->WriteString( wxString::Format( "\t%s->Create(", m_data->GetObjectName(objectID) ) );
    for (i = 0; i < paramCount; i++)
    {
        if ( objectIDValues[i] != wxInvalidObjectID )
            m_fp->WriteString( wxString::Format( "%s", m_data->GetObjectName( objectIDValues[i] ) ) );
        else
        {
            m_fp->WriteString( wxString::Format( "%s", ValueAsCode(params[i]) ) );
        }
        if (i < paramCount - 1)
            m_fp->WriteString( ", ");
    }
    m_fp->WriteString( ");\n");
}

void wxCodeDepersister::SetProperty(int objectID,
                                    const wxClassInfo *WXUNUSED(classInfo),
                                    const wxPropertyInfo* propertyInfo,
                                    const wxxVariant &value)
{
    m_fp->WriteString( wxString::Format( "\t%s->%s(%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetSetterName(),
        ValueAsCode(value)) );
}

void wxCodeDepersister::SetPropertyAsObject(int objectID,
                                            const wxClassInfo *WXUNUSED(classInfo),
                                            const wxPropertyInfo* propertyInfo,
                                            int valueObjectId)
{
    if ( propertyInfo->GetTypeInfo()->GetKind() == wxT_OBJECT )
        m_fp->WriteString( wxString::Format( "\t%s->%s(*%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetSetterName(),
        m_data->GetObjectName( valueObjectId) ) );
    else
        m_fp->WriteString( wxString::Format( "\t%s->%s(%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetSetterName(),
        m_data->GetObjectName( valueObjectId) ) );
}

void wxCodeDepersister::AddToPropertyCollection( int objectID ,
    const wxClassInfo *classInfo,
    const wxPropertyInfo* propertyInfo ,
    const wxxVariant &value) 
{
    m_fp->WriteString( wxString::Format( "\t%s->%s(%s);\n",
        m_data->GetObjectName(objectID),
        propertyInfo->GetAccessor()->GetAdderName(),
        ValueAsCode(value)) );
}

// sets the corresponding property (value is an object)
void wxCodeDepersister::AddToPropertyCollectionAsObject(int objectID,
    const wxClassInfo *classInfo,
    const wxPropertyInfo* propertyInfo ,
    int valueObjectId) 
{
    // TODO
}

void wxCodeDepersister::SetConnect(int eventSourceObjectID,
                                   const wxClassInfo *WXUNUSED(eventSourceClassInfo),
                                   const wxDelegateTypeInfo *delegateInfo ,
                                   const wxClassInfo *eventSinkClassInfo ,
                                   const wxHandlerInfo* handlerInfo ,
                                   int eventSinkObjectID )
{
    wxString ehsource = m_data->GetObjectName( eventSourceObjectID ) ;
    wxString ehsink = m_data->GetObjectName(eventSinkObjectID) ;
    wxString ehsinkClass = eventSinkClassInfo->GetClassName() ;
    int eventType = delegateInfo->GetEventType() ;
    wxString handlerName = handlerInfo->GetName() ;

    m_fp->WriteString( wxString::Format(  "\t%s->Connect( %s->GetId() , %d , (wxObjectEventFunction)(wxEventFunction) & %s::%s , NULL , %s ) ;" , 
        ehsource , ehsource , eventType , ehsinkClass , handlerName , ehsink ) );
}

#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(wxxVariantArray);

#endif
