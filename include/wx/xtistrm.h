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
Main interface for streaming out an object to XML.
*/

void WriteComponent(wxObject *Object, const wxClassInfo *ClassInfo, wxXmlNode *parent, const wxString& nodeName );

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