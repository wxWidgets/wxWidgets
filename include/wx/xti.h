/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xti.h
// Purpose:     runtime metadata information (extended class info)
// Author:      Stefan Csomor
// Modified by: 
// Created:     27/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Julian Smart
//              (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XTIH__
#define _WX_XTIH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "xti.h"
#endif

// We want to support properties, event sources and events sinks through
// explicit declarations, using templates and specialization to make the
// effort as painless as possible. 
//
// This means we have the following domains :
//
// - Type Information for categorizing built in types as well as custom types
// this includes information about enums, their values and names
// - Type safe value storage : a kind of wxVariant, called right now wxxVariant
// which will be merged with wxVariant 
// - Property Information and Property Accessors providing access to a class'
// values and exposed event delegates
// - Information about event handlers
// - extended Class Information for accessing all these

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/memory.h"
#include "wx/set.h"
#include "wx/string.h"

class WXDLLIMPEXP_BASE wxObject;
class WXDLLIMPEXP_BASE wxClassInfo;
class WXDLLIMPEXP_BASE wxHashTable;
class WXDLLIMPEXP_BASE wxObjectRefData;
class WXDLLIMPEXP_BASE wxEvent;

typedef void (wxObject::*wxObjectEventFunction)(wxEvent&);

// ----------------------------------------------------------------------------
// Enum Support
//
// In the header files there would no change from pure c++ code, in the 
// implementation, an enum would have
// to be enumerated eg :
//
// WX_BEGIN_ENUM( wxFlavor )
//   WX_ENUM_MEMBER( Vanilla )
//   WX_ENUM_MEMBER( Chocolate )
//   WX_ENUM_MEMBER( Strawberry )
// WX_END_ENUM( wxFlavor )
// ----------------------------------------------------------------------------

struct WXDLLIMPEXP_BASE wxEnumMemberData
{
    const wxChar*	m_name;
    int				m_value;
};

class WXDLLIMPEXP_BASE wxEnumData
{
public :
	wxEnumData( wxEnumMemberData* data ) ;

	// returns true if the member has been found and sets the int value
	// pointed to accordingly (if ptr != null )
	// if not found returns false, value left unchanged
	bool HasEnumMemberValue( const wxChar *name , int *value = NULL ) ;
	
	// returns the value of the member, if not found in debug mode an
	// assert is issued, in release 0 is returned
	int GetEnumMemberValue(const wxChar *name );
	
	// returns the name of the enum member having the passed in value
	// returns an emtpy string if not found
    const wxChar *GetEnumMemberName(int value);
	
	// returns the number of members in this enum
	int GetEnumCount() { return m_count ; }
	
	// returns the value of the nth member
	int GetEnumMemberValueByIndex( int n ) ;
	
	// returns the value of the nth member
	const wxChar *GetEnumMemberNameByIndex( int n ) ;
private :
    wxEnumMemberData *m_members;
	int m_count ;
};

#define WX_BEGIN_ENUM( e ) \
	wxEnumMemberData s_enumDataMembers##e[] = {

#define WX_ENUM_MEMBER( v ) { #v, v } ,

#define WX_END_ENUM( e ) { NULL , 0 } } ; \
	wxEnumData s_enumData##e( s_enumDataMembers##e ) ; \
	wxEnumData *wxGetEnumData(e) { return &s_enumData##e ; } \
	template<> const wxTypeInfo* wxGetTypeInfo( e * ){ static wxEnumTypeInfo s_typeInfo(wxT_ENUM , &s_enumData##e) ; return &s_typeInfo ; } \
	template<> void wxStringReadValue(const wxString& s , e &data ) \
	{ \
		data = (e) s_enumData##e.GetEnumMemberValue(s) ; \
	} \
	template<> void wxStringWriteValue(wxString &s , const e &data ) \
	{ \
		s =  s_enumData##e.GetEnumMemberName((int)data) ; \
	}

// ----------------------------------------------------------------------------
// Set Support
// 
// in the header :
// 
// enum wxFlavor
// {  
// 	Vanilla,
// 	Chocolate,
// 	Strawberry,
// };
// 
// typedef wxSet<wxFlavor> wxCoupe ;
// 
// in the implementation file :
// 
// WX_BEGIN_ENUM( wxFlavor )
// 	WX_ENUM_MEMBER( Vanilla )
// 	WX_ENUM_MEMBER( Chocolate )
// 	WX_ENUM_MEMBER( Strawberry )
// WX_END_ENUM( wxFlavor )
// 
// WX_IMPLEMENT_SET_STREAMING( wxCoupe , wxFlavor ) 
//
// implementation note : no partial specialization for streaming, but a delegation to a 
// different class
//
// ----------------------------------------------------------------------------

// in order to remove dependancy on string tokenizer
void wxSetStringToArray( const wxString &s , wxArrayString &array ) ;

template<typename e>
void wxSetFromString(const wxString &s , wxSet<e> &data )  
{
	wxEnumData* edata = wxGetEnumData((e) 0) ;
	data.Clear() ;

	wxArrayString array ;
	wxSetStringToArray( s , array ) ;
    wxString flag;
    for ( int i = 0 ; i < array.Count() ; ++i )
    {
        flag = array[i] ;
		int ivalue ;
		if ( edata->HasEnumMemberValue( flag , &ivalue ) )
		{
			data.Set( (e) ivalue ) ;
		}
    }
}

template<typename e>
void wxSetToString( wxString &s , const wxSet<e> &data )
{
	wxEnumData* edata = wxGetEnumData((e) 0) ;
	int count = edata->GetEnumCount() ;
	int i ;
	s.Clear() ;
	for ( i = 0 ; i < count ; i++ )
	{
		e value = (e) edata->GetEnumMemberValueByIndex(i) ;
		if ( data.Contains( value ) )
		{
			// this could also be done by the templated calls
			if ( !s.IsEmpty() )
				s +="|" ;
			s += edata->GetEnumMemberNameByIndex(i) ;
		}
	}
}

//  if the wxSet specialization above does not work for all compilers, add this to the WX_IMPLEMENT_SET_STREAMING macro
//	template<> const wxTypeInfo* wxGetTypeInfo( SetName * ){ static wxEnumTypeInfo s_typeInfo(wxT_SET , &s_enumData##e) ; return &s_typeInfo ; }\

#define WX_IMPLEMENT_SET_STREAMING(SetName,e) \
	template<> void wxStringReadValue(const wxString &s , wxSet<e> &data ) \
	{ \
		wxSetFromString( s , data ) ; \
	} \
	template<> void wxStringWriteValue( wxString &s , const wxSet<e> &data ) \
	{ \
		wxSetToString( s , data ) ; \
	} \


// ----------------------------------------------------------------------------
// Type Information
// ----------------------------------------------------------------------------

enum wxTypeKind
{
    wxT_VOID = 0, // unknown type
    wxT_BOOL,
    wxT_CHAR,
    wxT_UCHAR,
    wxT_INT,
    wxT_UINT,
    wxT_LONG,
    wxT_ULONG,
    wxT_FLOAT,
    wxT_DOUBLE,
    wxT_STRING, // must be wxString
    wxT_SET, // must be wxSet<> template
    wxT_ENUM,
    wxT_OBJECT, // must be a component (pointer !!!)
    wxT_CUSTOM, // user defined type (e.g. wxPoint)
	wxT_DELEGATE , // for connecting against an event source
    wxT_LAST_TYPE_KIND // sentinel for bad data, asserts, debugging
};

class WXDLLIMPEXP_BASE wxTypeInfo
{
public :
	wxTypeInfo() : m_kind( wxT_VOID) {}
	virtual ~wxTypeInfo() {}
	wxTypeKind GetKind() const { return m_kind ; }
protected :
	wxTypeKind m_kind ;
};

class WXDLLIMPEXP_BASE wxBuiltInTypeInfo : public wxTypeInfo
{
public :
	wxBuiltInTypeInfo( wxTypeKind kind ) { assert( kind < wxT_SET ) ; m_kind = kind ;}
} ;

class WXDLLIMPEXP_BASE wxCustomTypeInfo : public wxTypeInfo
{
public :
	wxCustomTypeInfo( const wxChar *typeName ) 
	{ m_kind = wxT_CUSTOM ; m_typeName = typeName ;}
	const wxChar *GetTypeName() const { assert( m_kind == wxT_CUSTOM ) ; return m_typeName ; }
private :
	const wxChar *m_typeName; // Kind == wxT_CUSTOM
} ;

class WXDLLIMPEXP_BASE wxEnumTypeInfo : public wxTypeInfo
{
public :
	wxEnumTypeInfo( wxTypeKind kind , wxEnumData* enumInfo ) 
	{ assert( kind == wxT_ENUM || kind == wxT_SET ) ; m_kind = kind ; m_enumInfo = enumInfo ;}
	const wxEnumData* GetEnumData() const { assert( m_kind == wxT_ENUM || m_kind == wxT_SET ) ; return m_enumInfo ; }
private :
	wxEnumData *m_enumInfo; // Kind == wxT_ENUM or Kind == wxT_SET
} ;

class WXDLLIMPEXP_BASE wxClassTypeInfo : public wxTypeInfo
{
public :
	wxClassTypeInfo( wxClassInfo* classInfo ) 
	{ m_kind = wxT_OBJECT ; m_classInfo = classInfo ;}
	const wxClassInfo *GetClassInfo() const { assert( m_kind == wxT_OBJECT ) ; return m_classInfo ; }
private :
	wxClassInfo *m_classInfo; // Kind == wxT_OBJECT - could be NULL
} ;

// a delegate is an exposed event source 

class WXDLLIMPEXP_BASE wxDelegateTypeInfo : public wxTypeInfo
{
public :
	wxDelegateTypeInfo( int eventType , wxClassInfo* eventClass ) 
	{ m_kind = wxT_DELEGATE ; m_eventClass = eventClass ; m_eventType = eventType ;}
	const wxClassInfo *GetEventClass() const { assert( m_kind == wxT_DELEGATE ) ; return m_eventClass ; }
	int GetEventType() const { assert( m_kind == wxT_DELEGATE ) ; return m_eventType ; }
private :
	const wxClassInfo *m_eventClass; // (extended will merge into classinfo)
	int m_eventType ;
} ;

template<typename T> const wxTypeInfo* wxGetTypeInfo( T * ) ;

template<typename T> const wxTypeInfo* wxGetTypeInfo( wxSet<T> * ) 
{
	static wxEnumTypeInfo s_typeInfo(wxT_SET , wxGetEnumData((T) 0) ) ; return &s_typeInfo ; 
}

// this macro is for usage with custom, non-object derived classes and structs, wxPoint is such a custom type

#define WX_CUSTOM_TYPE_INFO( e ) \
	template<> const wxTypeInfo* wxGetTypeInfo( e * ){ static wxCustomTypeInfo s_typeInfo(#e) ; return &s_typeInfo ; } \

// ----------------------------------------------------------------------------
// value streaming 
//
// streaming is defined for xml constructs right now, the aim is to make this
// pluggable in the future
// ----------------------------------------------------------------------------

// convenience function (avoids including xml headers in users code)

class wxXmlNode ;
void wxXmlAddContentToNode( wxXmlNode* node , const wxString& data ) ;
wxString wxXmlGetContentFromNode( wxXmlNode *node ) ;

// templated streaming, every type must have their specialization for these methods

template<typename T>
void wxStringReadValue( const wxString &s , T &data ) ;

template<typename T>
void wxStringWriteValue( wxString &s , const T &data) ;

// for simple types this default implementation is ok, composited structures will have to
// loop through their properties 

template<typename T>
void wxXmlReadValue( wxXmlNode *node , T &data ) 
{
	wxStringReadValue<T>( wxXmlGetContentFromNode( node ) , data ) ;
}

template<typename T>
void wxXmlWriteValue( wxXmlNode *node , const T &data) 
{
	wxString s ;
	wxStringWriteValue<T>( s, data ) ;
	wxXmlAddContentToNode( node ,s ) ;
}

// ----------------------------------------------------------------------------
// wxxVariant as typesafe data holder
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxxVariantData
{
public:
	virtual ~wxxVariantData() {}

	// return a heap allocated duplicate
	virtual wxxVariantData* Clone() const = 0 ;

	// returns the type info of the contentc
	virtual const wxTypeInfo* GetTypeInfo() const = 0 ;

	// write the value into an xml node
	virtual void Write( wxXmlNode* node ) const = 0 ;

	// read the value from the xml node
	virtual void Read( wxXmlNode* node ) = 0 ;

	// write the value into a string
	virtual void Write( wxString &s ) const = 0 ;

	// read the value from a string
	virtual void Read( const wxString &s) = 0 ;
} ;

template<typename T> class WXDLLIMPEXP_BASE wxxVariantDataT : public wxxVariantData
{
public:
    wxxVariantDataT(T d) : m_data(d) {}
	virtual ~wxxVariantDataT() {}

	// get a copy of the stored data
    T Get() const { return m_data; }

	// set the data
    void Set(T d) { m_data =  d; }

	// return a heap allocated duplicate
	virtual wxxVariantData* Clone() const { return new wxxVariantDataT<T>( Get() ) ; }

	// returns the type info of the contentc
	virtual const wxTypeInfo* GetTypeInfo() const { return wxGetTypeInfo( (T*) NULL ) ; }

	// write the value into an xml node
	virtual void Write( wxXmlNode* node ) const { wxXmlWriteValue( node , m_data ) ; }

	// read the value from the xml node
	virtual void Read( wxXmlNode* node ) { wxXmlReadValue( node , m_data ) ; }

	// write the value into a string
	virtual void Write( wxString &s ) const { wxStringWriteValue( s , m_data ) ; }

	// read the value from a string
	virtual void Read( const wxString &s) { wxStringReadValue( s , m_data ) ; }

private:
    T m_data;
};

class WXDLLIMPEXP_BASE wxxVariant
{
public :
	wxxVariant() { m_data = NULL ; }
	wxxVariant( wxxVariantData* data , const wxString& name = wxT("") ) : m_data(data) , m_name(name) {}
	wxxVariant( const wxxVariant &d ) { if ( d.m_data ) m_data = d.m_data->Clone() ; else m_data = NULL ; m_name = d.m_name ; }

	template<typename T> wxxVariant( T data , const wxString& name = wxT("") ) : 
		m_data(new wxxVariantDataT<T>(data) ), m_name(name) {}
	~wxxVariant() { delete m_data ; }

	// get a copy of the stored data
	template<typename T> T Get() const
	{
		wxxVariantDataT<T> *dataptr = dynamic_cast<wxxVariantDataT<T>*> (m_data) ;
		assert( dataptr ) ;
		return dataptr->Get() ;
	}
	
	// stores the data
	template<typename T> Set(T data) const
	{
		delete m_data ;
		m_data = new wxxVariantDataT<T>(data) ;
	}

	wxxVariant& operator=(const wxxVariant &d)
	{
		m_data = d.m_data->Clone() ; 
		m_name = d.m_name ;
		return *this ;
	}
	
	// gets the stored data casted to a wxObject* , returning NULL if cast is not possible
	wxObject* GetAsObject() const ;

	// write the value into an xml node
	void Write( wxXmlNode* node ) const { m_data->Write( node ) ; }

	// read the value from the xml node
	void Read( wxXmlNode* node ) { m_data->Read( node ) ; }

	// write the value into a string
	void Write( wxString &s ) const { m_data->Write( s ) ; }

	// read the value from a string
	void Read( const wxString &s) { m_data->Read( s ) ; }

	// returns this value as string
	wxString GetAsString() const
	{
		wxString s ;
		Write( s ) ;
		return s ;
	}

	void SetFromString( const wxString &s)
	{
		Read( s ) ;
	}
private :
	wxxVariantData* m_data ;
	wxString m_name ;
} ;

// ----------------------------------------------------------------------------
// Property Support
//
// wxPropertyInfo is used to inquire of the property by name.  It doesn't
// provide access to the property, only information about it.  If you
// want access, look at wxPropertyAccessor.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxPropertyAccessor
{
public :
    virtual void SetProperty(wxObject *object, const wxxVariant &value) const = 0 ;
    virtual wxxVariant GetProperty(wxObject *object) const = 0 ;
    virtual bool HasSetter() const = 0 ;
    virtual bool HasGetter() const = 0 ;
	const char * GetGetterName() const { return m_setterName ; }
	const char * GetSetterName() const { return m_getterName ; }
	virtual wxxVariant ReadValue( wxXmlNode* node ) const = 0 ;
	virtual void WriteValue( wxXmlNode* node , wxObject *o ) const = 0 ;
	virtual wxxVariant ReadValue( const wxString &value ) const = 0 ;
	virtual void WriteValue( wxString& value , wxObject *o ) const = 0 ;
protected :
	const char *m_setterName ;
	const char *m_getterName ;
};

template<typename T>
void wxXmlReadValue( wxXmlNode *node , T &data ) ;

template<typename T>
void wxXmlWriteValue( wxXmlNode *node , const T &data) ;

template<class Klass, typename T>
class WXDLLIMPEXP_BASE wxPropertyAccessorT : public wxPropertyAccessor
{
public:
    typedef void (Klass::*setter_t)(T value);
    typedef void (Klass::*setter_ref_t)(T& value);
    typedef T (Klass::*getter_t)() const;
    typedef const T& (Klass::*getter_ref_t)() const;

    wxPropertyAccessorT(setter_t setter, getter_t getter, const char *g, const char *s)
        : m_setter(setter), m_setter_ref(NULL), m_getter(getter) ,m_getter_ref(NULL) {m_setterName = s;m_getterName=g ;}
    wxPropertyAccessorT(setter_ref_t setter, getter_t getter, const char *g, const char *s)
        : m_setter(NULL), m_setter_ref(setter), m_getter(getter) , m_getter_ref(NULL){m_setterName = s;m_getterName=g ;}
    wxPropertyAccessorT(setter_ref_t setter, getter_ref_t getter, const char *g, const char *s)
        : m_setter(NULL), m_setter_ref(setter), m_getter(NULL) , m_getter_ref(getter){m_setterName = s;m_getterName=g ;}
    wxPropertyAccessorT(setter_t setter, getter_ref_t getter, const char *g, const char *s)
        : m_setter(NULL), m_setter(setter), m_getter(NULL) ,  m_getter_ref(getter){m_setterName = s;m_getterName=g ;}

	// returns true if this accessor has a setter
	bool HasSetter() const { return m_setter != NULL || m_setter_ref != NULL ; }

	// return true if this accessor has a getter
	bool HasGetter() const { return m_getter != NULL || m_getter_ref != NULL ; }

	// set the property this accessor is responsible for in an object
	void SetProperty(wxObject *o, const wxxVariant &v) const
    {
        Klass *obj = dynamic_cast<Klass*>(o);
        T value = v.Get<T>();
        if (m_setter)
            (obj->*(m_setter))(value);
        else
            (obj->*(m_setter_ref))(value);
    }

	// gets the property this accessor is responsible for from an object
    wxxVariant GetProperty(wxObject *o) const
    {
		return wxxVariant( (wxxVariantData* ) DoGetProperty( o ) ) ;
     }

	// write the property this accessor is responsible for from an object into
	// a xml node
	void WriteValue( wxXmlNode* node , wxObject *o ) const
	{
		DoGetProperty( o )->Write( node ) ;
	}

	// write the property this accessor is responsible for from an object into
	// a string
	void WriteValue( wxString& s , wxObject *o ) const
	{
		DoGetProperty( o )->Write( s ) ;
	}

	// read a wxxVariant having the correct type for the property this accessor
	// is responsible for from an xml node
	wxxVariant ReadValue( wxXmlNode* node ) const
	{
		T data ;
		wxXmlReadValue( node , data ) ;
		return wxxVariant( data ) ;
	}

	// read a wxxVariant having the correct type for the property this accessor
	// is responsible for from a string
	wxxVariant ReadValue( const wxString &value ) const
	{
		T data ;
		wxStringReadValue( value , data ) ;
		return wxxVariant( data ) ;
	}

private :
    wxxVariantDataT<T>* DoGetProperty(wxObject *o) const
    {
        Klass *obj = dynamic_cast<Klass*>(o);
		if ( m_getter )
			return new wxxVariantDataT<T>( (obj->*(m_getter))() ) ;
		else
			return new wxxVariantDataT<T>( (obj->*(m_getter_ref))() ) ;
    }

    setter_t m_setter;
    setter_ref_t m_setter_ref;
    getter_t m_getter;
	getter_ref_t m_getter_ref ;
};

class WXDLLIMPEXP_BASE wxPropertyInfo
{
public :
	wxPropertyInfo( wxPropertyInfo* &iter , const char *name , const char *typeName , const wxTypeInfo* typeInfo , wxPropertyAccessor *accessor , wxxVariant dv ) :
	   m_name( name ) , m_typeName(typeName) , m_typeInfo( typeInfo ) , m_accessor( accessor ) , m_defaultValue( dv )
	{
		m_next = iter ;
		iter = this ;
	}
	// return the name of this property
	const char *		GetName() const { return m_name ; }

	// return the typename of this property
	const char *		GetTypeName() const { return m_typeName ; }

	// return the type info of this property
	const wxTypeInfo *	GetTypeInfo() const { return m_typeInfo ; }

	// return the accessor for this property
	wxPropertyAccessor* GetAccessor() const { return m_accessor ; }

	// returns NULL if this is the last property of this class
	wxPropertyInfo*     GetNext() const { return m_next ; }
private : 
	const char *		m_name;
	const char *		m_typeName ;
	const wxTypeInfo*	m_typeInfo ;
	wxPropertyAccessor* m_accessor ;
    wxxVariant m_defaultValue; 
	// string representation of the default value
	//  to be assigned by the designer to the property
	//  when the component is dropped on the container.
	wxPropertyInfo*		m_next ;
};

#define WX_BEGIN_PROPERTIES_TABLE(theClass) \
    const wxPropertyInfo *theClass::GetPropertiesStatic()  \
    {  \
        typedef theClass class_t; \
		static wxPropertyInfo* first = NULL ;

#define WX_END_PROPERTIES_TABLE() \
		return first ; } 

#define WX_PROPERTY( name , type , setter , getter ,defaultValue ) \
	static wxPropertyAccessorT<class_t , type> _accessor##name( setter , getter , #setter , #getter ) ; \
	static wxPropertyInfo _propertyInfo##name( first , #name , #type , wxGetTypeInfo( (type*) NULL ) ,&_accessor##name , wxxVariant(defaultValue) ) ;

#define WX_DELEGATE( name , eventType , eventClass ) \
	static wxDelegateTypeInfo _typeInfo##name( eventType , CLASSINFO( eventClass ) ) ; \
	static wxPropertyInfo _propertyInfo##name( first , #name , NULL , &_typeInfo##name , NULL , wxxVariant() ) ; \

// ----------------------------------------------------------------------------
// Handler Info
//
// this is describing an event sink
// ----------------------------------------------------------------------------

class wxHandlerInfo
{
public :
	wxHandlerInfo( wxHandlerInfo* &iter , const wxChar *name , wxObjectEventFunction address , const wxClassInfo* eventClassInfo ) :
	   m_name( name ) , m_eventClassInfo( eventClassInfo ) , m_eventFunction( address )  
	{
		m_next = iter ;
		iter = this ;
	}

	// get the name of the handler method
	const char *		GetName() const { return m_name ; }

	// return the class info of the event
	const wxClassInfo *	GetEventClassInfo() const { return m_eventClassInfo ; }

	// get the handler function pointer
	wxObjectEventFunction	GetEventFunction() const { return m_eventFunction ; }

	// returns NULL if this is the last handler of this class
	wxHandlerInfo*     GetNext() const { return m_next ; }
private : 
	const wxChar *		m_name;
	wxObjectEventFunction m_eventFunction ;
	const wxClassInfo*	m_eventClassInfo ;
	wxHandlerInfo*		m_next ;
};

#define WX_HANDLER(name,eventClassType) \
	static wxHandlerInfo _handlerInfo##name( first , #name , (wxObjectEventFunction) &name , CLASSINFO( eventClassType ) ) ;

#define WX_BEGIN_HANDLERS_TABLE(theClass) \
    const wxHandlerInfo *theClass::GetHandlersStatic()  \
    {  \
        typedef theClass class_t; \
		static wxHandlerInfo* first = NULL ;

#define WX_END_HANDLERS_TABLE() \
		return first ; } 

// ----------------------------------------------------------------------------
// Constructor Bridges
//
// allow to set up constructors with params during runtime
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxConstructorBridge
{
public :
    virtual void Create(wxObject *o, wxxVariant *args) = 0;
};

// Creator Bridges for all Numbers of Params

// no params

template<typename Class>
struct wxConstructorBridge_0 : public wxConstructorBridge
{
    void Create(wxObject *o, wxxVariant *)
    {
        Class *obj = dynamic_cast<Class*>(o);
        obj->Create();
    }
};

struct wxConstructorBridge_Dummy : public wxConstructorBridge
{
    void Create(wxObject *, wxxVariant *)
    {
    }
} ;

#define WX_CONSTRUCTOR_0(klass) \
	wxConstructorBridge_0<klass> constructor##klass ; \
	wxConstructorBridge* klass::sm_constructor##klass = &constructor##klass ; \
	const char *klass::sm_constructorProperties##klass[] = { NULL } ; \
	const int klass::sm_constructorPropertiesCount##klass = 0 ;

#define WX_CONSTRUCTOR_DUMMY(klass) \
	wxConstructorBridge_Dummy constructor##klass ; \
	wxConstructorBridge* klass::sm_constructor##klass = &constructor##klass ; \
	const char *klass::sm_constructorProperties##klass[] = { NULL } ; \
	const int klass::sm_constructorPropertiesCount##klass = 0 ;

// 1 param

template<typename Class, typename T0>
struct wxConstructorBridge_1 : public wxConstructorBridge
{
    void Create(wxObject *o, wxxVariant *args)
    {
        Class *obj = dynamic_cast<Class*>(o);
        obj->Create(
                    args[0].Get<T0>()
                   );
    }
};

#define WX_CONSTRUCTOR_1(klass,t0,v0) \
	wxConstructorBridge_1<klass,t0> constructor##klass ; \
	wxConstructorBridge* klass::sm_constructor##klass = &constructor##klass ; \
	const char *klass::sm_constructorProperties##klass[] = { #v0 } ; \
	const int klass::sm_constructorPropertiesCount##klass = 1 ;

// 2 params

template<typename Class,
         typename T0, typename T1>
struct wxConstructorBridge_2 : public wxConstructorBridge
{
    void Create(wxObject *o, wxxVariant *args[])
    {
        Class *obj = dynamic_cast<Class*>(o);
        obj->Create(
                    args[0].Get<T0>() ,
                    args[1].Get<T1>() 
                   );
    }
};

#define WX_CONSTRUCTOR_2(klass,t0,v0,t1,v1) \
	wxConstructorBridge_2<klass,t0,t1> constructor##klass ; \
	klass::sm_constructor##klass = &constructor##klass ; \
	klass::sm_constructorProperties##klass[] = { #v0 , #v1 } ; \
	klass::sm_constructorPropertiesCount##klass = 2;

// 3 params

template<typename Class,
         typename T0, typename T1, typename T2>
struct wxConstructorBridge_3 : public wxConstructorBridge
{
    void Create(wxObject *o, wxxVariant *args[])
    {
        Class *obj = dynamic_cast<Class*>(o);
        obj->Create(
                    args[0].Get<T0>() ,
                    args[1].Get<T1>() ,
                    args[2].Get<T2>() 
                   );
    }
};

#define WX_CONSTRUCTOR_3(klass,t0,v0,t1,v1,t2,v2) \
	wxConstructorBridge_3<klass,t0,t1,t2> constructor##klass ; \
	klass::sm_constructor##klass = &constructor##klass ; \
	klass::sm_constructorProperties##klass[] = { #v0 , #v1 , #v2 } ; \
	klass::sm_constructorPropertiesCount##klass = 3 ;

// 4 params

template<typename Class,
         typename T0, typename T1, typename T2, typename T3>
struct wxConstructorBridge_4 : public wxConstructorBridge
{
    void Create(wxObject *o, wxxVariant *args[])
    {
        Class *obj = dynamic_cast<Class*>(o);
        obj->Create(
                    args[0].Get<T0>() ,
                    args[1].Get<T1>() ,
                    args[2].Get<T2>() ,
                    args[3].Get<T3>() 
                   );
    }
};

#define WX_CONSTRUCTOR_4(klass,t0,v0,t1,v1,t2,v2,t3,v3) \
	wxConstructorBridge_4<klass,t0,t1,t2,t3> constructor##klass ; \
	klass::sm_constructor##klass = &constructor##klass ; \
	klass::sm_constructorProperties##klass[] = { #v0 , #v1 , #v2 , #v3 } ; \
	klass::sm_constructorPropertiesCount##klass = 4 ;

// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

typedef wxObject *(*wxObjectConstructorFn)(void);
typedef wxObject* (*wxVariantToObjectConverter)( const wxxVariant &data ) ;
typedef wxxVariant (*wxObjectToVariantConverter)( wxObject* ) ;

class WXDLLIMPEXP_BASE wxClassInfo
{
public:
    wxClassInfo(const wxClassInfo **_Parents,
			const char *_UnitName,
			const char *_ClassName,
            int size,
            wxObjectConstructorFn ctor ,
			const wxPropertyInfo *_Props ,
			wxConstructorBridge* _Constructor ,
			const char ** _ConstructorProperties , 
			const int _ConstructorPropertiesCount ,
			wxVariantToObjectConverter _Converter1 ,
			wxObjectToVariantConverter _Converter2 
			) : m_parents(_Parents) , m_unitName(_UnitName) ,m_className(_ClassName),
			m_objectSize(size), m_objectConstructor(ctor) , m_firstProperty(_Props ) , m_constructor( _Constructor ) ,
			m_constructorProperties(_ConstructorProperties) , m_constructorPropertiesCount(_ConstructorPropertiesCount),
			m_variantToObjectConverter( _Converter1 ) , m_objectToVariantConverter( _Converter2 ) , m_next(sm_first)
	{
		sm_first = this; 
		Register( m_className , this ) ;
	}

    ~wxClassInfo() ;

    wxObject *CreateObject() { return m_objectConstructor ? (*m_objectConstructor)() : 0; }

    const wxChar       *GetClassName() const { return m_className; }
    const wxClassInfo **GetParents() const { return m_parents; }
    int                 GetSize() const { return m_objectSize; }

    wxObjectConstructorFn      GetConstructor() const { return m_objectConstructor; }
    static const wxClassInfo  *GetFirst() { return sm_first; }
    const wxClassInfo         *GetNext() const { return m_next; }
    static wxClassInfo        *FindClass(const wxChar *className);

        // Climb upwards through inheritance hierarchy.
        // Dual inheritance is catered for.

    bool IsKindOf(const wxClassInfo *info) const
    {
		if ( info != 0 )
		{
			if ( info == this )
				return true ;

			for ( int i = 0 ; m_parents[i] ; ++ i )
			{
				if ( m_parents[i]->IsKindOf( info ) )
					return true ;
			}
		}
		return false ;
    }

    // Initializes parent pointers and hash table for fast searching.
	// this is going to be removed by Register/Unregister calls
	// in Constructor / Destructor together with making the hash map private

    static void     InitializeClasses();

    // Cleans up hash table used for fast searching.

    static void     CleanUpClasses();

	// returns the first property
	const wxPropertyInfo* GetFirstProperty() const { return m_firstProperty ; } 

	// returns the first handler 
	const wxHandlerInfo* GetFirstHandler() const { return m_firstHandler ; } 

    // Call the Create method for a class
    virtual void Create (wxObject *object, int ParamCount, wxxVariant *Params) 
	{
		wxASSERT( ParamCount == m_constructorPropertiesCount ) ;
		m_constructor->Create( object , Params ) ;
	}

	// get number of parameters for constructor
	virtual int GetCreateParamCount() const { return m_constructorPropertiesCount; }

	// get i-th constructor parameter
	virtual const wxChar* GetCreateParamName(int i) const { return m_constructorProperties[i] ; }

   // Runtime access to objects by property name, and variant data
    virtual void SetProperty (wxObject *object, const char *PropertyName, const wxxVariant &Value);
    virtual wxxVariant GetProperty (wxObject *object, const char *PropertyName);

	// we must be able to cast variants to wxObject pointers, templates seem not to be suitable
	wxObject* VariantToInstance( const wxxVariant &data ) const { return m_variantToObjectConverter( data ) ; }
	wxxVariant InstanceToVariant( wxObject *object ) const { return m_objectToVariantConverter( object ) ; }
    
	// find property by name
    virtual const wxPropertyInfo *FindPropInfo (const char *PropertyName) const ;

public:
    const wxChar            *m_className;
    int                      m_objectSize;
    wxObjectConstructorFn    m_objectConstructor;

    // class info object live in a linked list:
    // pointers to its head and the next element in it

    static wxClassInfo      *sm_first;
    wxClassInfo             *m_next;

	// FIXME: this should be private (currently used directly by way too
    //        many clients)
    static wxHashTable      *sm_classTable;

private:
	const wxClassInfo**		m_parents ;
 	const wxPropertyInfo *	m_firstProperty ;
  	const wxHandlerInfo *	m_firstHandler ;
    const wxChar*			m_unitName;

	wxConstructorBridge*	m_constructor ;
	const wxChar **			m_constructorProperties ; 
	const int				m_constructorPropertiesCount ;
	wxVariantToObjectConverter m_variantToObjectConverter ; 
	wxObjectToVariantConverter m_objectToVariantConverter ;

    const wxPropertyAccessor *FindAccessor (const char *propertyName);

	// registers the class
	static void Register(const wxChar *name, wxClassInfo *info);
	
	static void Unregister(const wxChar *name);

	// InitializeClasses() helper
    static wxClassInfo *GetBaseByName(const wxChar *name);

    DECLARE_NO_COPY_CLASS(wxClassInfo)
};

WXDLLIMPEXP_BASE wxObject *wxCreateDynamicObject(const wxChar *name);

// ----------------------------------------------------------------------------
// Dynamic class macros
// ----------------------------------------------------------------------------

#define _DECLARE_DYNAMIC_CLASS(name)           \
 public:                                      \
  static wxClassInfo sm_class##name;          \
  static const wxClassInfo* sm_classParents##name[] ; \
  static const wxPropertyInfo* GetPropertiesStatic() ; \
  static const wxHandlerInfo* GetHandlersStatic() ; \
  virtual wxClassInfo *GetClassInfo() const   \
   { return &name::sm_class##name; }

#define DECLARE_DYNAMIC_CLASS(name)           \
	_DECLARE_DYNAMIC_CLASS(name) \
	static wxConstructorBridge* sm_constructor##name ; \
	static const char * sm_constructorProperties##name[] ; \
	static const int sm_constructorPropertiesCount##name ;

#define DECLARE_DYNAMIC_CLASS_NO_ASSIGN(name)   \
    DECLARE_NO_ASSIGN_CLASS(name)               \
    DECLARE_DYNAMIC_CLASS(name)

#define DECLARE_DYNAMIC_CLASS_NO_COPY(name)   \
    DECLARE_NO_COPY_CLASS(name)               \
    DECLARE_DYNAMIC_CLASS(name)

#define DECLARE_ABSTRACT_CLASS(name) _DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_CLASS(name) DECLARE_DYNAMIC_CLASS(name)

// -----------------------------------
// for concrete classes
// -----------------------------------

    // Single inheritance with one base class

#define _IMPLEMENT_DYNAMIC_CLASS(name, basename, unit)                 \
 wxObject* wxConstructorFor##name()                             \
  { return new name; }                                          \
 const wxClassInfo* name::sm_classParents##name[] = { &basename::sm_class##basename ,NULL } ; \
 wxObject* wxVariantToObjectConverter##name ( const wxxVariant &data ) { return data.Get<name*>() ; } \
 wxxVariant wxObjectToVariantConverter##name ( wxObject *data ) { return wxxVariant( dynamic_cast<name*> (data)  ) ; } \
 wxClassInfo name::sm_class##name(sm_classParents##name , wxT(unit) , wxT(#name),   \
            (int) sizeof(name),                              \
            (wxObjectConstructorFn) wxConstructorFor##name   ,   \
			name::GetPropertiesStatic(),name::sm_constructor##name , name::sm_constructorProperties##name ,     \
			name::sm_constructorPropertiesCount##name , wxVariantToObjectConverter##name , wxObjectToVariantConverter##name);    \
 template<> void wxStringReadValue(const wxString & , name * & ){assert(0) ;}\
 template<> void wxStringWriteValue(wxString & , name* const & ){assert(0) ;}\
 template<> const wxTypeInfo* wxGetTypeInfo( name ** ){ static wxClassTypeInfo s_typeInfo(&name::sm_class##name) ; return &s_typeInfo ; }

#define IMPLEMENT_DYNAMIC_CLASS( name , basename ) \
_IMPLEMENT_DYNAMIC_CLASS( name , basename , "" ) \
const wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) NULL ; } \
WX_CONSTRUCTOR_DUMMY( name )

#define IMPLEMENT_DYNAMIC_CLASS_XTI( name , basename , unit ) \
_IMPLEMENT_DYNAMIC_CLASS( name , basename , unit )

// this is for classes that do not derive from wxobject, there are no creators for these 

#define IMPLEMENT_DYNAMIC_CLASS_NO_WXOBJECT_NO_BASE_XTI( name , unit ) \
 const wxClassInfo* name::sm_classParents##name[] = { NULL } ; \
 wxClassInfo name::sm_class##name(sm_classParents##name , wxT("") , wxT(#name),   \
            (int) sizeof(name),                              \
            (wxObjectConstructorFn) 0   ,   \
			name::GetPropertiesStatic(),0 , 0 ,     \
			0 , 0 , 0 );    \
 template<> void wxStringReadValue(const wxString & , name * & ){assert(0) ;}\
 template<> void wxStringWriteValue(wxString & , name* const & ){assert(0) ;}\
 template<> const wxTypeInfo* wxGetTypeInfo( name ** ){ static wxClassTypeInfo s_typeInfo(&name::sm_class##name) ; return &s_typeInfo ; }

// this is for subclasses that still do not derive from wxobject

#define IMPLEMENT_DYNAMIC_CLASS_NO_WXOBJECT_XTI( name , basename, unit ) \
 const wxClassInfo* name::sm_classParents##name[] = { &basename::sm_class##basename ,NULL } ; \
 wxClassInfo name::sm_class##name(sm_classParents##name , wxT("") , wxT(#name),   \
            (int) sizeof(name),                              \
            (wxObjectConstructorFn) 0   ,   \
			name::GetPropertiesStatic(),0 , 0 ,     \
			0 , 0 , 0 );    \
 template<> void wxStringReadValue(const wxString & , name * & ){assert(0) ;}\
 template<> void wxStringWriteValue(wxString & , name* const & ){assert(0) ;}\
 template<> const wxTypeInfo* wxGetTypeInfo( name ** ){ static wxClassTypeInfo s_typeInfo(&name::sm_class##name) ; return &s_typeInfo ; }

    // Multiple inheritance with two base classes

#define _IMPLEMENT_DYNAMIC_CLASS2(name, basename, basename2, unit)                 \
 wxObject* wxConstructorFor##name()                             \
  { return new name; }                                          \
  const wxClassInfo* name::sm_classParents##name[] = { &basename::sm_class##basename ,&basename2::sm_class##basename2 , NULL } ; \
 wxObject* wxVariantToObjectConverter##name ( const wxxVariant &data ) { return data.Get<name*>() ; } \
 wxxVariant wxObjectToVariantConverter##name ( wxObject *data ) { return wxxVariant( dynamic_cast<name*> (data)  ) ; } \
 wxClassInfo name::sm_class##name(sm_classParents##name , wxT(unit) , wxT(#name),   \
            (int) sizeof(name),                              \
            (wxObjectConstructorFn) wxConstructorFor##name   ,   \
			name::GetPropertiesStatic(),name::sm_constructor##name , name::sm_constructorProperties##name ,     \
			name::sm_constructorPropertiesCount##name , wxVariantToObjectConverter##name , wxObjectToVariantConverter##name);    \
 template<> void wxStringReadValue(const wxString & , name * & ){assert(0) ;}\
 template<> void wxStringWriteValue(wxString & , name* const & ){assert(0) ;}\
 template<> const wxTypeInfo* wxGetTypeInfo( name ** ){ static wxClassTypeInfo s_typeInfo(&name::sm_class##name) ; return &s_typeInfo ; }

#define IMPLEMENT_DYNAMIC_CLASS2( name , basename , basename2) \
_IMPLEMENT_DYNAMIC_CLASS2( name , basename , basename2 , "") \
const wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) NULL ; } \
WX_CONSTRUCTOR_DUMMY( name )

#define IMPLEMENT_DYNAMIC_CLASS2_XTI( name , basename , basename2, unit) \
	_IMPLEMENT_DYNAMIC_CLASS2( name , basename , basename2 , unit) 

// -----------------------------------
// for abstract classes
// -----------------------------------

    // Single inheritance with one base class

#define _IMPLEMENT_ABSTRACT_CLASS(name, basename)                \
 const wxClassInfo* name::sm_classParents##name[] = { &basename::sm_class##basename ,NULL } ; \
 wxObject* wxVariantToObjectConverter##name ( const wxxVariant &data ) { return data.Get<name*>() ; } \
 wxxVariant wxObjectToVariantConverter##name ( wxObject *data ) { return wxxVariant( dynamic_cast<name*> (data)  ) ; } \
 wxClassInfo name::sm_class##name(sm_classParents##name , wxT("") , wxT(#name),   \
            (int) sizeof(name),                              \
            (wxObjectConstructorFn) 0   ,   \
			name::GetPropertiesStatic(),0 , 0 ,     \
			0 , wxVariantToObjectConverter##name , wxObjectToVariantConverter##name);    \
 template<> void wxStringReadValue(const wxString & , name * & ){assert(0) ;}\
 template<> void wxStringWriteValue(wxString & , name* const & ){assert(0) ;}\
 template<> const wxTypeInfo* wxGetTypeInfo( name ** ){ static wxClassTypeInfo s_typeInfo(&name::sm_class##name) ; return &s_typeInfo ; }

#define IMPLEMENT_ABSTRACT_CLASS( name , basename ) \
_IMPLEMENT_ABSTRACT_CLASS( name , basename ) \
const wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) NULL ; } 

    // Multiple inheritance with two base classes

#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)   \
 wxClassInfo name::sm_class##name(wxT(#name), wxT(#basename1),  \
            wxT(#basename2), (int) sizeof(name),                \
            (wxObjectConstructorFn) 0);

#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#endif