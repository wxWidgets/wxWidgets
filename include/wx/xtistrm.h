/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xtistrm.h
// Purpose:     streaming runtime metadata information (extended class info)
// Author:      Stefan Csomor
// Modified by: 
// Created:     27/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XTISTRMH__
#define _WX_XTISTRMH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xtistrm.h"
#endif

#include "wx/wx.h"

#if wxUSE_EXTENDED_RTTI

const int wxInvalidObjectID = -2 ;
const int wxNullObjectID = -1 ;

// Filer contains the interfaces for streaming objects in and out of XML,
// rendering them either to objects in memory, or to code.  Note:  We
// consider the process of generating code to be one of *depersisting* the
// object from xml, *not* of persisting the object to code from an object
// in memory.  This distincation can be confusing, and should be kept
// in mind when looking at the property streamers and callback interfaces
// listed below.

/*
Main interfaces for streaming out objects.
*/

class wxPersister
{
public :
    // will be called before a toplevel object is written, may veto even that (eg for objects that cannot be supported) by returning false
    virtual bool BeforeWriteObject( const wxObject *WXUNUSED(object) , const wxClassInfo *WXUNUSED(classInfo) , const wxString &WXUNUSED(name)) { return true ; } 

    // will be called before a property gets written, may change the value , eg replace a concrete wxSize by wxSize( -1 , -1 ) or veto
    // writing that property at all by returning false
    virtual bool BeforeWriteProperty( const wxObject *WXUNUSED(object) , const wxClassInfo *WXUNUSED(classInfo) , const wxPropertyInfo *WXUNUSED(propInfo) , wxxVariant &WXUNUSED(value) )  { return true ; } 

    // will be called before a property with an object value gets written, may change the value , eg replace the instance, void it or veto
    // writing that property at all by returning false
    virtual bool BeforeWritePropertyAsObject( const wxObject *WXUNUSED(object) , const wxClassInfo *WXUNUSED(classInfo) , const wxPropertyInfo *WXUNUSED(propInfo) , wxxVariant &WXUNUSED(value) )  { return true ; } 

    // will be called before a delegate property gets written, you may change all informations about the event sink
    virtual bool BeforeWriteDelegate( const wxObject *WXUNUSED(object),  const wxClassInfo* WXUNUSED(classInfo) , const wxPropertyInfo *WXUNUSED(propInfo) , 
        const wxObject *&WXUNUSED(eventSink) , const wxHandlerInfo* &WXUNUSED(handlerInfo) )  { return true ; } 
} ;

class wxWriter : public wxObject
{
public :
    wxWriter() ;
    ~wxWriter() ;

    // with this call you start writing out a new top-level object
    void WriteObject(const wxObject *object, const wxClassInfo *classInfo , wxPersister *persister , const wxString &name ) ;

    int GetObjectID(const wxObject *obj) ;
    bool IsObjectKnown( const wxObject *obj ) ;


    //
    // streaming callbacks
    //
    // these callbacks really write out the values in the stream format
    //

    // start of writing an toplevel object name param is used for unique identification within the container
    virtual void DoBeginWriteObject(const wxObject *object, const wxClassInfo *classInfo, int objectID , const wxString &name ) = 0 ;

    // end of writing an toplevel object name param is used for unique identification within the container
    virtual void DoEndWriteObject(const wxObject *object, const wxClassInfo *classInfo, int objectID , const wxString &name ) = 0 ;

    // start of writing an object used as param
    virtual void DoBeginWriteParamAsObject(const wxObject *parentObject, const wxClassInfo *parentClassInfo, const wxObject *valueObject, 
        const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) = 0 ;

    // end of writing an object used as param
    virtual void DoEndWriteParamAsObject(const wxObject *parentObject, const wxClassInfo *parentClassInfo, const wxObject *valueObject, 
        const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) = 0 ;

    // insert an object reference to an already written object
    virtual void DoWriteObjectReference(const wxObject *parentObject, const wxClassInfo *parentClassInfo, const wxObject *valueObject, 
        const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) = 0 ;

    // writes a property in the stream format
    virtual void DoWriteProperty( const wxObject *object, const wxClassInfo* classInfo , const wxPropertyInfo *propInfo , wxxVariant &value ) = 0 ;

    // writes a delegate in the stream format
    virtual void DoWriteDelegate( const wxObject *object,  const wxClassInfo* classInfo , const wxPropertyInfo *propInfo , 
        const wxObject *eventSink , int sinkObjectID , const wxClassInfo* eventSinkClassInfo , const wxHandlerInfo* handlerIndo ) = 0;
private :

    struct wxWriterInternal ;
    wxWriterInternal* m_data ;

    struct wxWriterInternalPropertiesData ;

    void WriteAllProperties( const wxObject * obj , const wxClassInfo* ci , wxPersister *persister, wxWriterInternalPropertiesData * data ) ;
} ;

class wxXmlWriter : public wxWriter
{
public :

    wxXmlWriter( wxXmlNode * parent ) ;
    ~wxXmlWriter() ;

    //
    // streaming callbacks
    //
    // these callbacks really write out the values in the stream format
    //

    // start of writing an toplevel object name param is used for unique identification within the container
    virtual void DoBeginWriteObject(const wxObject *object, const wxClassInfo *classInfo, int objectID , const wxString &name ) ;

    // end of writing an toplevel object name param is used for unique identification within the container
    virtual void DoEndWriteObject(const wxObject *object, const wxClassInfo *classInfo, int objectID , const wxString &name ) ;

    // start of writing an object used as param
    virtual void DoBeginWriteParamAsObject(const wxObject *parentObject, const wxClassInfo *parentClassInfo, const wxObject *valueObject, 
        const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) ;

    // end of writing an object used as param
    virtual void DoEndWriteParamAsObject(const wxObject *parentObject, const wxClassInfo *parentClassInfo, const wxObject *valueObject, 
        const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) ;

    // insert an object reference to an already written object or to a null object
    virtual void DoWriteObjectReference(const wxObject *parentObject, const wxClassInfo *parentClassInfo, const wxObject *valueObject, 
        const wxClassInfo *valueObjectClassInfo, int valueObjectID , const wxPropertyInfo *propInfo ) ;

    // writes a property in the stream format
    virtual void DoWriteProperty( const wxObject *object, const wxClassInfo* classInfo , const wxPropertyInfo *propInfo , wxxVariant &value )  ;

    // writes a delegate in the stream format
    virtual void DoWriteDelegate( const wxObject *object,  const wxClassInfo* classInfo , const wxPropertyInfo *propInfo , 
        const wxObject *eventSink, int sinkObjectID , const wxClassInfo* eventSinkClassInfo , const wxHandlerInfo* handlerInfo ) ;
private :
    struct wxXmlWriterInternal ;
    wxXmlWriterInternal* m_data ;
} ;

/*
Streaming callbacks for depersisting XML to code, or running objects
*/

class wxDepersister ;

/*
wxReader handles streaming in a class from a arbitrary format. While walking through
it issues calls out to interfaces to depersist the guts from the underlying storage format.
*/

class wxReader : public wxObject
{
public :
    wxReader() ;
    ~wxReader() ;

    // the only thing wxReader knows about is the class info by object ID
    wxClassInfo *GetObjectClassInfo(int objectID) ;
    bool HasObjectClassInfo( int objectID ) ;
    void SetObjectClassInfo(int objectID, wxClassInfo* classInfo);

    // Reads the component the reader is pointed at from the underlying format.  
    // The return value is the root object ID, which can
    // then be used to ask the depersister about that object
    virtual int ReadObject( wxDepersister *depersist ) = 0 ;

private :
    struct wxReaderInternal;
    wxReaderInternal *m_data;
} ;

/*
wxXmlReader handles streaming in a class from XML
*/

class wxXmlReader : public wxReader
{
public:
    wxXmlReader(wxXmlNode *parent) { m_parent = parent ; }
    ~wxXmlReader() {}

    // Reads a component from XML.  The return value is the root object ID, which can
    // then be used to ask the depersister about that object

    int ReadObject(wxDepersister *callbacks);

private :
    int ReadComponent(wxXmlNode *parent, wxDepersister *callbacks);

    // accessor is only used as a temporary measure
    wxxVariant ReadValue(wxXmlNode *Node,
        wxPropertyAccessor *accessor );

    wxXmlNode * m_parent ;
};

// This abstract class matches the allocate-init/create model of creation of objects.
// At runtime, these will create actual instances, and manipulate them.
// When generating code, these will just create statements of C++
// code to create the objects.

class wxDepersister
{
public :
    // allocate the new object on the heap, that object will have the passed in ID
    virtual void AllocateObject(int ObjectID, wxClassInfo *ClassInfo) = 0;

    // initialize the already allocated object having the ID ObjectID with the Create method
    // creation parameters which are objects are having their Ids passed in objectIDValues
    // having objectId <> wxInvalidObjectID

    virtual void CreateObject(int ObjectID,
        const wxClassInfo *ClassInfo,
        int ParamCount,
        wxxVariant *VariantValues ,
        int *objectIDValues ,
        const wxClassInfo **objectClassInfos
        ) = 0;

    // destroy the heap-allocated object having the ID ObjectID, this may be used if an object
    // is embedded in another object and set via value semantics, so the intermediate
    // object can be destroyed after safely
    virtual void DestroyObject(int ObjectID, wxClassInfo *ClassInfo) = 0;

    // set the corresponding property
    virtual void SetProperty(int ObjectID,
        const wxClassInfo *ClassInfo,
        const wxPropertyInfo* PropertyInfo ,
        const wxxVariant &VariantValue) = 0;

    // sets the corresponding property (value is an object)
    virtual void SetPropertyAsObject(int ObjectId,
        const wxClassInfo *ClassInfo,
        const wxPropertyInfo* PropertyInfo ,
        int valueObjectId) = 0;


    // sets the corresponding event handler
    virtual void SetConnect(int EventSourceObjectID,
        const wxClassInfo *EventSourceClassInfo,
        const wxDelegateTypeInfo *delegateInfo ,
        const wxClassInfo *EventSinkClassInfo ,
        const wxHandlerInfo* handlerInfo ,
        int EventSinkObjectID ) = 0;
};

/*
wxRuntimeDepersister implements the callbacks that will depersist
an object into a running memory image, as opposed to writing
C++ initialization code to bring the object to life.
*/
class wxRuntimeDepersister : public wxDepersister
{
    struct wxRuntimeDepersisterInternal ;
    wxRuntimeDepersisterInternal * m_data ;
public :
    wxRuntimeDepersister() ;
    ~wxRuntimeDepersister() ;

    // returns the object having the corresponding ID fully constructed 
    wxObject *GetObject(int objectID) ;

    // allocate the new object on the heap, that object will have the passed in ID
    virtual void AllocateObject(int objectID, wxClassInfo *classInfo) ;

    // initialize the already allocated object having the ID ObjectID with the Create method
    // creation parameters which are objects are having their Ids passed in objectIDValues
    // having objectId <> wxInvalidObjectID

    virtual void CreateObject(int ObjectID,
        const wxClassInfo *ClassInfo,
        int ParamCount,
        wxxVariant *VariantValues ,
        int *objectIDValues,
        const wxClassInfo **objectClassInfos
        ) ;

    // destroy the heap-allocated object having the ID ObjectID, this may be used if an object
    // is embedded in another object and set via value semantics, so the intermediate
    // object can be destroyed after safely
    virtual void DestroyObject(int objectID, wxClassInfo *classInfo) ;

    // set the corresponding property
    virtual void SetProperty(int objectID,
        const wxClassInfo *classInfo,
        const wxPropertyInfo* propertyInfo ,
        const wxxVariant &variantValue);

    // sets the corresponding property (value is an object)
    virtual void SetPropertyAsObject(int objectId,
        const wxClassInfo *classInfo,
        const wxPropertyInfo* propertyInfo ,
        int valueObjectId) ;


    // sets the corresponding event handler
    virtual void SetConnect(int eventSourceObjectID,
        const wxClassInfo *eventSourceClassInfo,
        const wxDelegateTypeInfo *delegateInfo ,
        const wxClassInfo *eventSinkClassInfo ,
        const wxHandlerInfo* handlerInfo ,
        int eventSinkObjectID ) ;
};

/*
wxDepersisterCode implements the callbacks that will depersist
an object into a C++ initialization function.
*/

class wxTextOutputStream ;

class wxCodeDepersister : public wxDepersister
{
private :
    struct wxCodeDepersisterInternal ;
    wxCodeDepersisterInternal * m_data ;
    wxTextOutputStream *m_fp;
    wxString ValueAsCode( const wxxVariant &param ) ;
public:
    wxCodeDepersister(wxTextOutputStream *out) ;
    ~wxCodeDepersister() ;

    // allocate the new object on the heap, that object will have the passed in ID
    virtual void AllocateObject(int objectID, wxClassInfo *classInfo) ;

    // initialize the already allocated object having the ID ObjectID with the Create method
    // creation parameters which are objects are having their Ids passed in objectIDValues
    // having objectId <> wxInvalidObjectID

    virtual void CreateObject(int objectID,
        const wxClassInfo *classInfo,
        int paramCount,
        wxxVariant *variantValues ,
        int *objectIDValues,
        const wxClassInfo **objectClassInfos
        ) ;

    // destroy the heap-allocated object having the ID ObjectID, this may be used if an object
    // is embedded in another object and set via value semantics, so the intermediate
    // object can be destroyed after safely
    virtual void DestroyObject(int objectID, wxClassInfo *classInfo) ;

    // set the corresponding property
    virtual void SetProperty(int objectID,
        const wxClassInfo *classInfo,
        const wxPropertyInfo* propertyInfo ,
        const wxxVariant &variantValue);

    // sets the corresponding property (value is an object)
    virtual void SetPropertyAsObject(int objectId,
        const wxClassInfo *classInfo,
        const wxPropertyInfo* propertyInfo ,
        int valueObjectId) ;


    // sets the corresponding event handler
    virtual void SetConnect(int eventSourceObjectID,
        const wxClassInfo *eventSourceClassInfo,
        const wxDelegateTypeInfo *delegateInfo ,
        const wxClassInfo *eventSinkClassInfo ,
        const wxHandlerInfo* handlerInfo ,
        int eventSinkObjectID ) ;
};

#endif // wxUSE_EXTENDED_RTTI

#endif