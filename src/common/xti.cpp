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

#ifdef __GNUG__
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

#include <string.h>

#if wxUSE_EXTENDED_RTTI

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

template<> const wxTypeInfo* wxGetTypeInfo( void * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_VOID ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( bool * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_BOOL ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( char * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_CHAR ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( unsigned char * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_UCHAR ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( int * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_CHAR ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( unsigned int * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_UCHAR ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( long * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_CHAR ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( unsigned long * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_UCHAR ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( float * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_FLOAT ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( double * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_DOUBLE ) ;
	return &s_typeInfo ;
}

template<> const wxTypeInfo* wxGetTypeInfo( wxString * )
{
	static wxBuiltInTypeInfo s_typeInfo( wxT_STRING ) ;
	return &s_typeInfo ;
}

// this are compiler induced specialization which are never used anywhere

// char const *

template<> const wxTypeInfo* wxGetTypeInfo( char const ** )
{
	assert(0) ;
	static wxBuiltInTypeInfo s_typeInfo( wxT_VOID ) ;
	return &s_typeInfo ;
}

template<> void wxStringReadValue(const wxString & , const char* & )
{
	assert(0) ;
}

template<> void wxStringWriteValue(wxString & , char const * const & )
{
	assert(0) ;
}

// char *

template<> const wxTypeInfo* wxGetTypeInfo( char ** )
{
        assert(0) ;
        static wxBuiltInTypeInfo s_typeInfo( wxT_VOID ) ;
        return &s_typeInfo ;
}

template<> void wxStringReadValue(const wxString & , char* & )
{
        assert(0) ;
}

template<> void wxStringWriteValue(wxString & , char * const & )
{
        assert(0) ;
}

// ----------------------------------------------------------------------------
// value streaming
// ----------------------------------------------------------------------------

// convenience function (avoids including xml headers in users code)

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

WX_CUSTOM_TYPE_INFO(wxPoint)

template<> void wxStringReadValue(const wxString &s , wxSize &data )
{
	wxSscanf(s, _T("%d,%d"), &data.x , &data.y ) ;
}

template<> void wxStringWriteValue(wxString &s , const wxSize &data )
{
	s = wxString::Format("%d,%d", data.x , data.y ) ;
}

WX_CUSTOM_TYPE_INFO(wxSize)

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


void wxClassInfo::Register(const char *WXUNUSED(name), wxClassInfo *WXUNUSED(info))
{
	/*
    if (!ExtendedTypeMap)
	ExtendedTypeMap = new ClassMap;
    (*ExtendedTypeMap)[string(Name)] = Info;
	*/
}

void wxClassInfo::Unregister(const char *WXUNUSED(name))
{
	/*
    assert(ExtendedTypeMap);
    ExtendedTypeMap->erase(Name);
	*/
}

const wxPropertyAccessor *wxClassInfo::FindAccessor(const char *PropertyName)
{
    const wxPropertyInfo* info = FindPropertyInfo( PropertyName ) ;

	if ( info )
		return info->GetAccessor() ;

	return NULL ;
}

const wxPropertyInfo *wxClassInfo::FindPropertyInfo (const char *PropertyName) const
{
	const wxPropertyInfo* info = GetFirstProperty() ;

	while( info )
	{
		if ( strcmp( info->GetName() , PropertyName ) == 0 )
			return info ;
		info = info->GetNext() ;
	}

	const wxClassInfo** parents = GetParents() ;
	for ( int i = 0 ; parents[i] ; ++ i )
	{
		if ( ( info = parents[i]->FindPropertyInfo( PropertyName ) ) != NULL )
			return info ;
	}

    return 0;
}

const wxHandlerInfo *wxClassInfo::FindHandlerInfo (const char *PropertyName) const
{
	const wxHandlerInfo* info = GetFirstHandler() ;

	while( info )
	{
		if ( strcmp( info->GetName() , PropertyName ) == 0 )
			return info ;
		info = info->GetNext() ;
	}

	const wxClassInfo** parents = GetParents() ;
	for ( int i = 0 ; parents[i] ; ++ i )
	{
		if ( ( info = parents[i]->FindHandlerInfo( PropertyName ) ) != NULL )
			return info ;
	}

    return 0;
}


void wxClassInfo::SetProperty(wxObject *object, const char *propertyName, const wxxVariant &value)
{
    const wxPropertyAccessor *accessor;

    accessor = FindAccessor(propertyName);
    wxASSERT(accessor->HasSetter());
	accessor->SetProperty( object , value ) ;
}

wxxVariant wxClassInfo::GetProperty(wxObject *object, const char *propertyName)
{
    const wxPropertyAccessor *accessor;

    accessor = FindAccessor(propertyName);
    wxASSERT(accessor->HasGetter());
    return accessor->GetProperty(object);
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


#endif
