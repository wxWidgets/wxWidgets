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

class wxReader;
/*
    Streaming callbacks for depersisting XML to code, or running objects
*/

struct wxIDepersist ;

/*
    wxReader handles streaming in a class from XML.  Maintains a list of
    objects, and names, and issues calls out to interfaces to depersist the
    guts from the XML tree.
*/
class wxReader : wxObject
{
    struct wxReaderInternal;
    wxReaderInternal *Data;

    wxxVariant ReadPropertyValueNoAssign(wxXmlNode *Node,
				wxClassInfo *ClassInfo,
				const wxPropertyInfo *& propertyInfo ,
				wxIDepersist *Callbacks = NULL);

    void ReadPropertyValue(wxXmlNode *Node,
				wxClassInfo *ClassInfo,
				int ObjectId ,
				wxIDepersist *Callbacks = NULL );

    bool genCode; // true if the reader should generate code.
		  // ISSUE: ick!
		  // this interface is getting crufty.  Why the
		  // different depersist callbacks in here, if there
	    	  // is another place that knows that we're generating
	    	  // code?  Needs some repair work.
public:
    wxReader(bool GenerateCode = false);
    ~wxReader();

    // Reads a component from XML.  The return is the object ID, which can
    // be used in calls to GetObject or GetObjectName.
    //
    // ISSUE: Still needs to implement references to objects.
    //  requires a bit of additional design in the XML (minor).
    int ReadComponent(wxXmlNode *parent, wxIDepersist *Callbacks);

    // When streaming in, we may we depersisting to code, or to objects
    // in memory.  The depersist callbacks for generating code will
    // not create the objects, but will create names for them instead.
    // So internally, we keep track of IDs, not pointers.  Depending
    // on who you are in your callbacks, you can query the names or
    // pointers of the objects as need be.  You should not mix both,
    // because you will die if you do.

    wxObject *GetObject(int id);
    wxString GetObjectName(int id);
	wxClassInfo *GetObjectClassInfo(int id) ;

    void SetObject(int id, wxObject *Object);
    void SetObjectName(int id, const wxString &Name, wxClassInfo* ClassInfo);

    // Returns the result of a top level ReadComponent call.  Used
    // typically after you have instructed the reader to stream in an
    // object, and you want the object back now.  Only really valid if
    // you are reading back in to an object in memory, as opposed to code.
	wxObject *GetRoot() { return GetObject( 0 ) ; }
};

struct wxIDepersist
{
    // NotifyReader is called by wxReader so that we can have access to the
    // object store functions in the reader when we are called back.  Hmm.
    // probably should have just made the callback functions each take a
    // wxReader.
    virtual void NotifyReader(wxReader *Reader) = 0;

    // The next three callbacks match the ACS model of creation of objects.
    // At runtime, these will create actual instances, and manipulate them.
    // When generating code, these will just create statements of C++
    // code to create the objects.
    virtual void AllocateObject(int ObjectID, wxClassInfo *ClassInfo) = 0;
    virtual void CreateObject(int ObjectID,
			      wxClassInfo *ClassInfo,
			      int ParamCount,
			      wxxVariant *VariantValues) = 0;
    virtual void SetProperty(int ObjectID,
			     wxClassInfo *ClassInfo,
				 const wxPropertyInfo* PropertyInfo ,
			     const wxxVariant &VariantValue) = 0;
    virtual void SetConnect(int EventSourceObjectID,
			     wxClassInfo *EventSourceClassInfo,
				 int eventType ,
				 const wxString &handlerName ,
				 int EventSinkObjectID ) = 0;
};

/*
    wxIDepersistRuntime implements the callbacks that will depersist
    an object into a running memory image, as opposed to writing
    C++ initialization code to bring the object to life.
*/
class wxIDepersistRuntime : public wxIDepersist
{
    wxReader *Reader;
public:
    virtual void NotifyReader(wxReader *_Reader)
    {
	Reader = _Reader;
    }
    virtual void AllocateObject(int ObjectID, wxClassInfo *ClassInfo);
    virtual void CreateObject(int ObjectID, wxClassInfo *ClassInfo, int ParamCount, wxxVariant *VariantValues);
    virtual void SetProperty(int ObjectID, wxClassInfo *ClassInfo, const wxPropertyInfo* PropertyInfo, const wxxVariant &VariantValue);
    virtual void SetConnect(int EventSourceObjectID,
			     wxClassInfo *EventSourceClassInfo,
				 int eventType ,
				 const wxString &handlerName ,
				 int EventSinkObjectID ) ;
};

/*
    wxIDepersistCode implements the callbacks that will depersist
    an object into a C++ initialization function.
*/

class wxTextOutputStream ;

class wxIDepersistCode : public wxIDepersist
{
    wxReader *Reader;
    wxTextOutputStream *fp;
public:
    wxIDepersistCode(wxTextOutputStream *out) : fp(out) { }
    virtual void NotifyReader(wxReader *_Reader)
    {
	Reader = _Reader;
    }
    virtual void AllocateObject(int ObjectID, wxClassInfo *ClassInfo);
    virtual void CreateObject(int ObjectID, wxClassInfo *ClassInfo, int ParamCount, wxxVariant *VariantValues);
    virtual void SetProperty(int ObjectID, wxClassInfo *ClassInfo, const wxPropertyInfo* PropertyInfo, const wxxVariant &VariantValue);
    virtual void SetConnect(int EventSourceObjectID,
			     wxClassInfo *EventSourceClassInfo,
				 int eventType ,
				 const wxString &handlerName ,
				 int EventSinkObjectID ) ;
};

#endif // wxUSE_EXTENDED_RTTI

#endif