/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/xti.cpp
// Purpose:     runtime metadata information (extended class info
// Author:      Stefan Csomor
// Modified by:
// Created:     27/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Julian Smart
//              (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "xti.h"
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
#include "wx/list.h"
#include "wx/datetime.h"
#include <string.h>

#if wxUSE_EXTENDED_RTTI

#include "wx/beforestd.h"
#include <map>
#include <string>
#include "wx/afterstd.h"

using namespace std ;

// ----------------------------------------------------------------------------
// Enum Support
// ----------------------------------------------------------------------------

wxEnumData::wxEnumData( wxEnumMemberData* data )
{
	m_members = data ;
    for ( m_count = 0; m_members[m_count].m_name ; m_count++)
		{} ;
}

bool wxEnumData::HasEnumMemberValue(const wxChar *name, int *value)
{
    int i;
    for (i = 0; m_members[i].m_name ; i++ )
	{
		if (!strcmp(name, m_members[i].m_name))
		{
			if ( value )
				*value = m_members[i].m_value;
			return true ;
		}
	}
	return false ;
}

int wxEnumData::GetEnumMemberValue(const wxChar *name)
{
    int i;
    for (i = 0; m_members[i].m_name ; i++ )
	{
		if (!strcmp(name, m_members[i].m_name))
		{
			return m_members[i].m_value;
		}
	}
    return 0 ;
}

const wxChar *wxEnumData::GetEnumMemberName(int value)
{
    int i;
    for (i = 0; m_members[i].m_name ; i++)
		if (value == m_members[i].m_value)
			return m_members[i].m_name;

	return wxT("") ;
}

int wxEnumData::GetEnumMemberValueByIndex( int idx )
{
	// we should cache the count in order to avoid out-of-bounds errors
	return m_members[idx].m_value ;
}

const char * wxEnumData::GetEnumMemberNameByIndex( int idx )
{
	// we should cache the count in order to avoid out-of-bounds errors
	return m_members[idx].m_name ;
}

// ----------------------------------------------------------------------------
// Type Information
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// value streaming
// ----------------------------------------------------------------------------

// streamer specializations
// for all built-in types

// bool

template<> void wxStringReadValue(const wxString &s , bool &data )
{
	int intdata ;
	wxSscanf(s, _T("%d"), &intdata ) ;
	data = bool(intdata) ;
}

template<> void wxStringWriteValue(wxString &s , const bool &data )
{
	s = wxString::Format("%d", data ) ;
}

// char

template<> void wxStringReadValue(const wxString &s , char &data )
{
	int intdata ;
	wxSscanf(s, _T("%d"), &intdata ) ;
	data = char(intdata) ;
}

template<> void wxStringWriteValue(wxString &s , const char &data )
{
	s = wxString::Format("%d", data ) ;
}

// unsigned char

template<> void wxStringReadValue(const wxString &s , unsigned char &data )
{
	int intdata ;
	wxSscanf(s, _T("%d"), &intdata ) ;
	data = (unsigned char)(intdata) ;
}

template<> void wxStringWriteValue(wxString &s , const unsigned char &data )
{
	s = wxString::Format("%d", data ) ;
}

// int

template<> void wxStringReadValue(const wxString &s , int &data )
{
	wxSscanf(s, _T("%d"), &data ) ;
}

template<> void wxStringWriteValue(wxString &s , const int &data )
{
	s = wxString::Format("%d", data ) ;
}

// unsigned int

template<> void wxStringReadValue(const wxString &s , unsigned int &data )
{
	wxSscanf(s, _T("%d"), &data ) ;
}

template<> void wxStringWriteValue(wxString &s , const unsigned int &data )
{
	s = wxString::Format("%d", data ) ;
}

// long

template<> void wxStringReadValue(const wxString &s , long &data )
{
	wxSscanf(s, _T("%ld"), &data ) ;
}

template<> void wxStringWriteValue(wxString &s , const long &data )
{
	s = wxString::Format("%ld", data ) ;
}

// unsigned long

template<> void wxStringReadValue(const wxString &s , unsigned long &data )
{
	wxSscanf(s, _T("%ld"), &data ) ;
}

template<> void wxStringWriteValue(wxString &s , const unsigned long &data )
{
	s = wxString::Format("%ld", data ) ;
}

// float

template<> void wxStringReadValue(const wxString &s , float &data )
{
	wxSscanf(s, _T("%f"), &data ) ;
}

template<> void wxStringWriteValue(wxString &s , const float &data )
{
	s = wxString::Format("%f", data ) ;
}

// double

template<> void wxStringReadValue(const wxString &s , double &data )
{
	wxSscanf(s, _T("%lf"), &data ) ;
}

template<> void wxStringWriteValue(wxString &s , const double &data )
{
	s = wxString::Format("%lf", data ) ;
}

// wxString

template<> void wxStringReadValue(const wxString &s , wxString &data )
{
	data = s ;
}

template<> void wxStringWriteValue(wxString &s , const wxString &data )
{
	s = data ;
}

/*
	Custom Data Streaming / Type Infos
	we will have to add this for all wx non object types, but it is also an example
	for custom data structures
*/

// wxPoint

template<> void wxStringReadValue(const wxString &s , wxPoint &data )
{
	wxSscanf(s, _T("%d,%d"), &data.x , &data.y ) ;
}

template<> void wxStringWriteValue(wxString &s , const wxPoint &data )
{
	s = wxString::Format("%d,%d", data.x , data.y ) ;
}

template<> void wxStringReadValue(const wxString & , wxPoint* & )
{
    assert(0) ;
}

template<> void wxStringWriteValue(wxString & , wxPoint* const & )
{
    assert(0) ;
}

WX_CUSTOM_TYPE_INFO(wxPoint)

template<> void wxStringReadValue(const wxString &s , wxSize &data )
{
	wxSscanf(s, _T("%d,%d"), &data.x , &data.y ) ;
}

template<> void wxStringWriteValue(wxString &s , const wxSize &data )
{
	s = wxString::Format("%d,%d", data.x , data.y ) ;
}

template<> void wxStringReadValue(const wxString & , wxSize* & )
{
    assert(0) ;
}

template<> void wxStringWriteValue(wxString & , wxSize * const & )
{
    assert(0) ;
}

WX_CUSTOM_TYPE_INFO(wxSize)

template<> void wxStringReadValue(const wxString &s , wxDateTime &data )
{
    data.ParseFormat(s,wxT("%Y-%m-%d %H:%M:%S")) ;
}

template<> void wxStringWriteValue(wxString &s , const wxDateTime &data )
{
    s = data.Format(wxT("%Y-%m-%d %H:%M:%S")) ;
}

WX_CUSTOM_TYPE_INFO(wxDateTime)

//
// built-ins
//

template<> const wxTypeInfo* wxGetTypeInfo( void * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_VOID ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( bool * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_BOOL , &wxToStringConverter<bool> , &wxFromStringConverter<bool>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( char * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_CHAR , &wxToStringConverter<char> , &wxFromStringConverter<char>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( unsigned char * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_UCHAR , &wxToStringConverter< unsigned char > , &wxFromStringConverter<unsigned char>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( int * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_CHAR , &wxToStringConverter<int> , &wxFromStringConverter<int>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( unsigned int * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_UCHAR , &wxToStringConverter<unsigned int> , &wxFromStringConverter<unsigned int>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( long * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_LONG , &wxToStringConverter<long> , &wxFromStringConverter<long>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( unsigned long * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_ULONG , &wxToStringConverter<unsigned long> , &wxFromStringConverter<unsigned long>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( float * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_FLOAT , &wxToStringConverter<float> , &wxFromStringConverter<float>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( double * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_DOUBLE , &wxToStringConverter<double> , &wxFromStringConverter<double>) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( wxString * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_STRING , &wxToStringConverter<wxString> , &wxFromStringConverter<wxString>) ;
	return &s_typeInfo ;
}

// this are compiler induced specialization which are never used anywhere

WX_ILLEGAL_TYPE_SPECIALIZATION( char const * )
WX_ILLEGAL_TYPE_SPECIALIZATION( char * )
WX_ILLEGAL_TYPE_SPECIALIZATION( unsigned char * )
WX_ILLEGAL_TYPE_SPECIALIZATION( int * )
WX_ILLEGAL_TYPE_SPECIALIZATION( bool * )
WX_ILLEGAL_TYPE_SPECIALIZATION( long * )
WX_ILLEGAL_TYPE_SPECIALIZATION( wxString * )

//

// make wxWindowList known

template<> const wxTypeInfo* wxGetTypeInfo( wxArrayString * )
{
    static wxCollectionTypeInfo s_typeInfo( (wxTypeInfo*) wxGetTypeInfo( (wxString *) NULL) ) ;
    return &s_typeInfo ;
}

template<> void wxCollectionToVariantArray( wxArrayString const &theArray, wxxVariantArray &value)
{
    wxArrayCollectionToVariantArray( theArray , value ) ;
}



/*

template<> void wxStringReadValue(const wxString &s , wxColour &data )
{
	// copied from VS xrc
	unsigned long tmp = 0;

    if (s.Length() != 7 || s[0u] != wxT('#') ||
        wxSscanf(s.c_str(), wxT("#%lX"), &tmp) != 1)
    {
		wxLogError(_("String To Colour : Incorrect colour specification : %s"),
                   s.c_str() );
        data = wxNullColour;
    }
	else
	{
		data = wxColour((unsigned char) ((tmp & 0xFF0000) >> 16) ,
                    (unsigned char) ((tmp & 0x00FF00) >> 8),
                    (unsigned char) ((tmp & 0x0000FF)));
	}
}

template<> void wxStringWriteValue(wxString &s , const wxColour &data )
{
	s = wxString::Format("#%2X%2X%2X", data.Red() , data.Green() , data.Blue() ) ;
}

WX_CUSTOM_TYPE_INFO(wxColour)

*/

// removing header dependancy on string tokenizer

void wxSetStringToArray( const wxString &s , wxArrayString &array )
{
    wxStringTokenizer tokenizer(s, wxT("| \t\n"), wxTOKEN_STRTOK);
    wxString flag;
	array.Clear() ;
    while (tokenizer.HasMoreTokens())
    {
		array.Add(tokenizer.GetNextToken()) ;
	}
}

// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

const wxPropertyAccessor *wxClassInfo::FindAccessor(const char *PropertyName) const
{
    const wxPropertyInfo* info = FindPropertyInfo( PropertyName ) ;

	if ( info )
		return info->GetAccessor() ;

	return NULL ;
}

const wxPropertyInfo *wxClassInfo::FindPropertyInfoInThisClass (const char *PropertyName) const
{
	const wxPropertyInfo* info = GetFirstProperty() ;

	while( info )
	{
		if ( strcmp( info->GetName() , PropertyName ) == 0 )
			return info ;
		info = info->GetNext() ;
	}

    return 0;
}

const wxPropertyInfo *wxClassInfo::FindPropertyInfo (const char *PropertyName) const
{
	const wxPropertyInfo* info = FindPropertyInfoInThisClass( PropertyName ) ;
    if ( info )
        return info ;

	const wxClassInfo** parents = GetParents() ;
	for ( int i = 0 ; parents[i] ; ++ i )
	{
		if ( ( info = parents[i]->FindPropertyInfo( PropertyName ) ) != NULL )
			return info ;
	}

    return 0;
}

const wxHandlerInfo *wxClassInfo::FindHandlerInfoInThisClass (const char *PropertyName) const
{
	const wxHandlerInfo* info = GetFirstHandler() ;

	while( info )
	{
		if ( strcmp( info->GetName() , PropertyName ) == 0 )
			return info ;
		info = info->GetNext() ;
	}

    return 0;
}

const wxHandlerInfo *wxClassInfo::FindHandlerInfo (const char *PropertyName) const
{
	const wxHandlerInfo* info = FindHandlerInfoInThisClass( PropertyName ) ;

    if ( info )
        return info ;

	const wxClassInfo** parents = GetParents() ;
	for ( int i = 0 ; parents[i] ; ++ i )
	{
		if ( ( info = parents[i]->FindHandlerInfo( PropertyName ) ) != NULL )
			return info ;
	}

    return 0;
}


void wxClassInfo::SetProperty(wxObject *object, const char *propertyName, const wxxVariant &value) const
{
    const wxPropertyAccessor *accessor;

    accessor = FindAccessor(propertyName);
    wxASSERT(accessor->HasSetter());
	accessor->SetProperty( object , value ) ;
}

wxxVariant wxClassInfo::GetProperty(wxObject *object, const char *propertyName) const
{
    const wxPropertyAccessor *accessor;

    accessor = FindAccessor(propertyName);
    wxASSERT(accessor->HasGetter());
    wxxVariant result ;
    accessor->GetProperty(object,result);
    return result ;
}

wxxVariantArray wxClassInfo::GetPropertyCollection(wxObject *object, const wxChar *propertyName) const
{
    const wxPropertyAccessor *accessor;

    accessor = FindAccessor(propertyName);
    wxASSERT(accessor->HasGetter());
    wxxVariantArray result ;
    accessor->GetPropertyCollection(object,result);
    return result ;
}

void wxClassInfo::AddToPropertyCollection(wxObject *object, const wxChar *propertyName , const wxxVariant& value) const
{
    const wxPropertyAccessor *accessor;

    accessor = FindAccessor(propertyName);
    wxASSERT(accessor->HasAdder());
    accessor->AddToPropertyCollection( object , value ) ;
}

/*
VARIANT TO OBJECT
*/

wxObject* wxxVariant::GetAsObject()
{
	const wxClassTypeInfo *ti = dynamic_cast<const wxClassTypeInfo*>( m_data->GetTypeInfo() ) ;
	if ( ti )
		return ti->GetClassInfo()->VariantToInstance(*this) ;
	else
		return NULL ;
}

// ----------------------------------------------------------------------------
// wxDynamicObject support
// ----------------------------------------------------------------------------
//
// Dynamic Objects are objects that have a real superclass instance and carry their
// own attributes in a hash map. Like this it is possible to create the objects and
// stream them, as if their class information was already available from compiled data

struct wxDynamicObject::wxDynamicObjectInternal
{
    map<string,wxxVariant> m_properties ;
} ;

// instantiates this object with an instance of its superclass
wxDynamicObject::wxDynamicObject(wxObject* superClassInstance, const wxDynamicClassInfo *info)
{
    m_superClassInstance = superClassInstance ;
    m_classInfo = info ;
    m_data = new wxDynamicObjectInternal ;
}

wxDynamicObject::~wxDynamicObject()
{
    delete m_data ;
    delete m_superClassInstance ;
}

void wxDynamicObject::SetProperty (const wxChar *propertyName, const wxxVariant &value)
{
    wxASSERT_MSG(m_classInfo->FindPropertyInfoInThisClass(propertyName),wxT("Accessing Unknown Property in a Dynamic Object") ) ;
    m_data->m_properties[propertyName] = value ;
}

wxxVariant wxDynamicObject::GetProperty (const wxChar *propertyName) const
{
   wxASSERT_MSG(m_classInfo->FindPropertyInfoInThisClass(propertyName),wxT("Accessing Unknown Property in a Dynamic Object") ) ;
   return m_data->m_properties[propertyName] ;
}

// ----------------------------------------------------------------------------
// wxDynamiClassInfo
// ----------------------------------------------------------------------------

wxDynamicClassInfo::wxDynamicClassInfo( const wxChar *unitName, const wxChar *className , const wxClassInfo* superClass ) :
    wxClassInfo( unitName, className , new const wxClassInfo*[2])
{
    GetParents()[0] = superClass ;
    GetParents()[1] = NULL ;
}

wxDynamicClassInfo::~wxDynamicClassInfo()
{
    delete[] GetParents() ;
}

wxObject *wxDynamicClassInfo::AllocateObject() const
{
    wxObject* parent = GetParents()[0]->AllocateObject() ;
    return new wxDynamicObject( parent , this ) ;
}

void wxDynamicClassInfo::Create (wxObject *object, int paramCount, wxxVariant *params) const
{
    wxDynamicObject *dynobj = dynamic_cast< wxDynamicObject *>( object ) ;
    wxASSERT_MSG( dynobj , wxT("cannot call wxDynamicClassInfo::Create on an object other than wxDynamicObject") ) ;
    GetParents()[0]->Create( dynobj->GetSuperClassInstance() , paramCount , params ) ;
}

// get number of parameters for constructor
int wxDynamicClassInfo::GetCreateParamCount() const
{
    return GetParents()[0]->GetCreateParamCount() ;
}

// get i-th constructor parameter
const wxChar* wxDynamicClassInfo::GetCreateParamName(int i) const
{
    return GetParents()[0]->GetCreateParamName( i ) ;
}

void wxDynamicClassInfo::SetProperty(wxObject *object, const char *propertyName, const wxxVariant &value) const
{
    wxDynamicObject* dynobj = dynamic_cast< wxDynamicObject * >( object ) ;
    wxASSERT_MSG( dynobj , wxT("cannot call wxDynamicClassInfo::SetProperty on an object other than wxDynamicObject") ) ;
    if ( FindPropertyInfoInThisClass(propertyName) )
        dynobj->SetProperty( propertyName , value ) ;
    else
        GetParents()[0]->SetProperty( dynobj->GetSuperClassInstance() , propertyName , value ) ;
}

wxxVariant wxDynamicClassInfo::GetProperty(wxObject *object, const char *propertyName) const
{
    wxDynamicObject* dynobj = dynamic_cast< wxDynamicObject * >( object ) ;
    wxASSERT_MSG( dynobj , wxT("cannot call wxDynamicClassInfo::SetProperty on an object other than wxDynamicObject") ) ;
    if ( FindPropertyInfoInThisClass(propertyName) )
        return dynobj->GetProperty( propertyName ) ;
    else
        return GetParents()[0]->GetProperty( dynobj->GetSuperClassInstance() , propertyName ) ;
}

void wxDynamicClassInfo::AddProperty( const wxChar *propertyName , const wxTypeInfo* typeInfo )
{
    new wxPropertyInfo( m_firstProperty , propertyName , typeInfo , new wxGenericPropertyAccessor( propertyName ) , wxxVariant() ) ;
}

void wxDynamicClassInfo::AddHandler( const wxChar *handlerName , wxObjectEventFunction address , const wxClassInfo* eventClassInfo )
{
    new wxHandlerInfo( m_firstHandler , handlerName , address , eventClassInfo ) ;
}

// ----------------------------------------------------------------------------
// wxGenericPropertyAccessor
// ----------------------------------------------------------------------------

struct wxGenericPropertyAccessor::wxGenericPropertyAccessorInternal
{
    char filler ;
} ;

wxGenericPropertyAccessor::wxGenericPropertyAccessor( const wxString& propertyName )
: wxPropertyAccessor( NULL , NULL , NULL , NULL )
{
    m_data = new wxGenericPropertyAccessorInternal ;
    m_propertyName = propertyName ;
    m_getterName = wxT("Get")+propertyName ;
    m_setterName = wxT("Set")+propertyName ;
}

wxGenericPropertyAccessor::~wxGenericPropertyAccessor()
{
    delete m_data ;
}
void wxGenericPropertyAccessor::SetProperty(wxObject *object, const wxxVariant &value) const
{
    wxDynamicObject* dynobj = dynamic_cast< wxDynamicObject * >( object ) ;
    wxASSERT_MSG( dynobj , wxT("cannot call wxDynamicClassInfo::SetProperty on an object other than wxDynamicObject") ) ;
    dynobj->SetProperty(m_propertyName , value ) ;
}

void wxGenericPropertyAccessor::GetProperty(const wxObject *object, wxxVariant& value) const
{
    const wxDynamicObject* dynobj = dynamic_cast< const wxDynamicObject * >( object ) ;
    wxASSERT_MSG( dynobj , wxT("cannot call wxDynamicClassInfo::SetProperty on an object other than wxDynamicObject") ) ;
    value = dynobj->GetProperty( m_propertyName ) ;
}
#endif
