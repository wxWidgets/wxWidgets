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

#ifdef __GNUG__
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
#include <map>
#include <vector>
#include <string>

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

void wxWriter::WriteObject(const wxObject *object, const wxClassInfo *classInfo , wxPersister *persister , const wxString &name )
{
    if ( persister->BeforeWriteObject( object , classInfo , name ) )
    {
        int oid = m_data->m_nextId++ ;
        m_data->m_writtenObjects[object] = oid ;
        DoBeginWriteObject( object , classInfo , oid , name ) ;
        wxWriterInternalPropertiesData data ;
        WriteAllProperties( object , classInfo , persister , &data ) ;
        DoEndWriteObject( object , classInfo , oid , name  ) ;
    }
}

void wxWriter::WriteAllProperties( const wxObject * obj , const wxClassInfo* ci , wxPersister *persister, wxWriterInternalPropertiesData * data )
{
    const wxPropertyInfo *pi = ci->GetFirstProperty() ;
    while( pi ) 
    {
        if ( data->m_writtenProperties.find( pi->GetName() ) == data->m_writtenProperties.end() )
        {
            data->m_writtenProperties[ pi->GetName() ] = 1 ;
            const wxClassTypeInfo* cti = dynamic_cast< const wxClassTypeInfo* > ( pi->GetTypeInfo() ) ;
            if ( cti )
            {
                const wxClassInfo* pci = cti->GetClassInfo() ;
                wxxVariant value = pi->GetAccessor()->GetProperty(obj) ;
                if ( persister->BeforeWritePropertyAsObject( obj , ci , pi , value ) )
                {
                    wxObject *vobj = pci->VariantToInstance( value ) ;
                    bool embeddedObject = cti->GetKind() == wxT_OBJECT ;

                    if ( vobj == NULL || IsObjectKnown( vobj ) )
                    {
                        DoWriteObjectReference( obj , ci , vobj , pci , GetObjectID(vobj) , pi ) ;
                    }
                    else
                    {
                        int oid = m_data->m_nextId++ ;
                        if ( !embeddedObject)
                            m_data->m_writtenObjects[vobj] = oid ;

                        DoBeginWriteParamAsObject( obj , ci , vobj , pci , oid , pi ) ;
                        if ( vobj != NULL )
                        {
                            wxWriterInternalPropertiesData data ;
                            WriteAllProperties( vobj , pci , persister , &data ) ;
                        }
                        DoEndWriteParamAsObject( obj , ci , vobj , pci , oid , pi   ) ;
                    }
                }
            }
            else
            {
                const wxDelegateTypeInfo* dti = dynamic_cast< const wxDelegateTypeInfo* > ( pi->GetTypeInfo() ) ;
                if ( dti )
                {
                    const wxObject* sink = NULL ;
                    const wxHandlerInfo *handler = NULL ;
                    if ( persister->BeforeWriteDelegate( obj , ci , pi , sink , handler ) )
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
                    wxxVariant value = pi->GetAccessor()->GetProperty(obj) ;
                    if ( persister->BeforeWriteProperty( obj , ci , pi , value ) )
                    {
                        DoWriteProperty( obj , ci , pi , value ) ;
                    }
                }
            }
        }
        pi = pi->GetNext() ;
    }
    const wxClassInfo** parents = ci->GetParents() ;
    for ( int i = 0 ; parents[i] ; ++ i )
    {
        WriteAllProperties( obj , parents[i] , persister , data ) ;
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

// start of writing the root object
void wxXmlWriter::DoBeginWriteObject(const wxObject *WXUNUSED(object), const wxClassInfo *classInfo, int objectID , const wxString &name  ) 
{
    wxXmlNode *onode;
    onode = new wxXmlNode(wxXML_ELEMENT_NODE, name);
    onode->AddProperty(wxString("class"), wxString(classInfo->GetClassName()));
    onode->AddProperty(wxString("id"), wxString::Format( "%d" , objectID ) );

    m_data->m_current->AddChild(onode) ;
    m_data->m_objectStack.push_back( m_data->m_current ) ;
    m_data->m_current = onode ;
}

// end of writing the root object
void wxXmlWriter::DoEndWriteObject(const wxObject *WXUNUSED(object), const wxClassInfo *WXUNUSED(classInfo), int WXUNUSED(objectID) , const wxString &WXUNUSED(name) ) 
{
    m_data->m_current = m_data->m_objectStack.back() ;
    m_data->m_objectStack.pop_back() ;
}

// writes a property in the stream format
void wxXmlWriter::DoWriteProperty( const wxObject *WXUNUSED(obj), const wxClassInfo* WXUNUSED(classInfo) , const wxPropertyInfo *pi , wxxVariant &value ) 
{
    wxXmlNode *pnode;
    pnode = new wxXmlNode(wxXML_ELEMENT_NODE, pi->GetName() );
    wxXmlAddContentToNode( pnode ,value.GetAsString() ) ;
    m_data->m_current->AddChild(pnode);
}

void wxXmlWriter::DoBeginWriteParamAsObject(const wxObject *WXUNUSED(parentObject), const wxClassInfo *WXUNUSED(parentClassInfo), const wxObject *WXUNUSED(valueObject), 
                                            const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo )
{
    wxXmlNode *onode;
    onode = new wxXmlNode(wxXML_ELEMENT_NODE, propInfo->GetName());
    onode->AddProperty(wxString("class"), wxString(valueObjectClassInfo->GetClassName()));
    onode->AddProperty(wxString("id"), wxString::Format( "%d" , valueObjectID ) );
    m_data->m_current->AddChild(onode) ;
    m_data->m_objectStack.push_back( m_data->m_current ) ;
    m_data->m_current = onode ;
}

// insert an object reference to an already written object
void wxXmlWriter::DoWriteObjectReference(const wxObject *WXUNUSED(parentObject), const wxClassInfo *WXUNUSED(parentClassInfo), const wxObject *valueObject, 
                                         const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) 
{
    wxXmlNode *onode;
    onode = new wxXmlNode(wxXML_ELEMENT_NODE, propInfo->GetName());
    onode->AddProperty(wxString("class"), wxString(valueObjectClassInfo->GetClassName()));
    if ( valueObject == NULL )
    {
        wxXmlNode* nullnode = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, "null");
        onode->AddChild(nullnode);
    }
    else
    {
        onode->AddProperty(wxString("id"), wxString::Format( "%d" , valueObjectID ) );
    }

    m_data->m_current->AddChild(onode) ;
}

void wxXmlWriter::DoEndWriteParamAsObject(const wxObject *WXUNUSED(parentObject), const wxClassInfo *WXUNUSED(parentClassInfo), const wxObject *WXUNUSED(valueObject), 
                                          const wxClassInfo *WXUNUSED(valueObjectClassInfo), int WXUNUSED(valueObjectID) , const wxPropertyInfo *WXUNUSED(propInfo) )
{
    m_data->m_current = m_data->m_objectStack.back() ;
    m_data->m_objectStack.pop_back() ;
}


// writes a delegate in the stream format
void wxXmlWriter::DoWriteDelegate( const wxObject *WXUNUSED(object),  const wxClassInfo* WXUNUSED(classInfo) , const wxPropertyInfo *pi , 
                                  const wxObject *eventSink, int sinkObjectID , const wxClassInfo* WXUNUSED(eventSinkClassInfo) , const wxHandlerInfo* handlerInfo ) 
{
    if ( eventSink != NULL && handlerInfo != NULL )
    {
        wxXmlNode *pnode;
        pnode = new wxXmlNode(wxXML_ELEMENT_NODE, pi->GetName() );
        wxString s ;
        s.Printf(wxT("%d.%s"), sinkObjectID , handlerInfo->GetName() ) ;
        wxXmlAddContentToNode( pnode ,s ) ;
        m_data->m_current->AddChild(pnode);
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
    wxString className;
    wxClassInfo *classInfo;

    wxxVariant *createParams ;
    int *createParamOids ;
    const wxClassInfo** createClassInfos ;
    wxXmlNode *children;
    int objectID;

    children = node->GetChildren();
    if (!node->GetPropVal("class", &className))
    {
        // No class name.  Eek. FIXME: error handling
        return wxInvalidObjectID;
    }
    classInfo = wxClassInfo::FindClass(className);
    if (children && children->GetType() == wxXML_TEXT_NODE)
    {
        assert( wxXmlGetContentFromNode(node) == "null" ) ;
        // this must be a NULL component reference.  We just bail out
        return wxNullObjectID;
    }

    wxString ObjectIdString ;
    if (!node->GetPropVal("id", &ObjectIdString))
    {
        // No object id.  Eek. FIXME: error handling
        return wxInvalidObjectID;
    }

    objectID = atoi( ObjectIdString.c_str() ) ;
    // is this object already has been streamed in, return it here
    if ( HasObjectClassInfo( objectID ) )
        return objectID ;

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
        propertyNames.push_back( children->GetName().c_str() ) ;
        propertyNodes[children->GetName().c_str()] = children ;
        children = children->GetNext() ;
    }

    for ( int i = 0 ; i <classInfo->GetCreateParamCount() ; ++i )
    {
        const wxChar* paramName = classInfo->GetCreateParamName(i) ;
        PropertyNodes::iterator propiter = propertyNodes.find( paramName ) ;
        const wxPropertyInfo* pi = classInfo->FindPropertyInfo( paramName ) ;
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
                createParams[i] = ReadValue( prop , pi->GetAccessor() ) ;
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
                if ( pi->GetTypeInfo()->IsObjectType() )
                {
                    int valueId = ReadComponent( prop , callbacks ) ;
                    if ( callbacks )
                    {
                        if ( valueId != wxInvalidObjectID )
                        {
                            callbacks->SetPropertyAsObject( objectID , classInfo , pi , valueId ) ;
                            if ( pi->GetTypeInfo()->GetKind() == wxT_OBJECT && valueId != wxNullObjectID )
                                callbacks->DestroyObject( valueId , GetObjectClassInfo( valueId ) ) ;
                        }
                    }
                }
                else if ( pi->GetTypeInfo()->IsDelegateType() )
                {
                    wxString resstring = wxXmlGetContentFromNode(prop) ;
                    wxInt32 pos = resstring.Find('.') ;
                    assert( pos != wxNOT_FOUND ) ;
                    int sinkOid = atol(resstring.Left(pos)) ;
                    wxString handlerName = resstring.Mid(pos+1) ;
                    wxClassInfo* sinkClassInfo = GetObjectClassInfo( sinkOid ) ;

                    if (callbacks)
                        callbacks->SetConnect( objectID , classInfo , dynamic_cast<const wxDelegateTypeInfo*>(pi->GetTypeInfo()) , sinkClassInfo ,
                        sinkClassInfo->FindHandlerInfo(handlerName) ,  sinkOid ) ;

                }
                else
                {
                    if ( callbacks )
                        callbacks->SetProperty( objectID, classInfo ,pi , ReadValue( prop , pi->GetAccessor() ) ) ;
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
                                  wxPropertyAccessor *accessor )
{
    return accessor->ReadValue(wxXmlGetContentFromNode( node ) ) ;
}

int wxXmlReader::ReadObject(wxDepersister *callbacks)
{
    return ReadComponent( m_parent , callbacks ) ;
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
                                       const wxClassInfo *WXUNUSED(classInfo),
                                       const wxPropertyInfo* propertyInfo,
                                       const wxxVariant &value)
{
    wxObject *o;
    o = m_data->GetObject(objectID);
    propertyInfo->GetAccessor()->SetProperty( o , value ) ;
}

void wxRuntimeDepersister::SetPropertyAsObject(int objectID,
                                               const wxClassInfo *WXUNUSED(classInfo),
                                               const wxPropertyInfo* propertyInfo,
                                               int valueObjectId)
{
    wxObject *o, *valo;
    o = m_data->GetObject(objectID);
    valo = m_data->GetObject(valueObjectId);
    propertyInfo->GetAccessor()->SetProperty( o , 
        (dynamic_cast<const wxClassTypeInfo*>(propertyInfo->GetTypeInfo()))->GetClassInfo()->InstanceToVariant(valo) ) ;
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

#endif
