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

// ----------------------------------------------------------------------------
// streaming xml out 
// ----------------------------------------------------------------------------

void WriteComponent(wxObject *Object, const wxClassInfo *ClassInfo, wxXmlNode *parent, const wxString& nodeName , int &nextId , map< wxObject* , int > &writtenObjects ) ;

void WriteComponentProperties( wxObject* obj , const wxClassInfo* ci , wxXmlNode *onode , int &nextId, map< wxObject* , int > &writtenObjects, map< string , int > &writtenProperties)
{
	const wxPropertyInfo *pi = ci->GetFirstProperty() ;
	while( pi ) 
	{
		if ( writtenProperties.find( pi->GetName() ) == writtenProperties.end() )
		{
			writtenProperties[ pi->GetName() ] = 1 ;
			const wxClassTypeInfo* cti = dynamic_cast< const wxClassTypeInfo* > ( pi->GetTypeInfo() ) ;
			if ( cti )
			{
				const wxClassInfo* pci = cti->GetClassInfo() ;
				WriteComponent( pci->VariantToInstance( pi->GetAccessor()->GetProperty(obj) ) , pci , onode , pi->GetName() , nextId , writtenObjects ) ;
			}
			else
			{
				const wxDelegateTypeInfo* dti = dynamic_cast< const wxDelegateTypeInfo* > ( pi->GetTypeInfo() ) ;
				if ( dti )
				{
					// in which form should we stream out these ?
				}
				else
				{
					wxXmlNode *pnode;
					pnode = new wxXmlNode(wxXML_ELEMENT_NODE, pi->GetName() );
					pi->GetAccessor()->WriteValue(pnode, obj ) ;
					onode->AddChild(pnode);
				}
			}
		}
		pi = pi->GetNext() ;
	}
	const wxClassInfo** parents = ci->GetParents() ;
	for ( int i = 0 ; parents[i] ; ++ i )
	{
		WriteComponentProperties( obj , parents[i] , onode , nextId , writtenObjects , writtenProperties ) ;
	}
}

/*
 Writing Components does have to take inheritance into account, that's why we are iterating
 over our parents as well
 */

void WriteComponent(wxObject *obj, const wxClassInfo *classInfo, wxXmlNode *parent, const wxString &nodeName)
{
	int nextid = 0 ; // 0 is the root element
	map< wxObject* , int > writtenobjects ;
	WriteComponent( obj , classInfo, parent, nodeName , nextid , writtenobjects ) ;
}

void WriteComponent(wxObject *obj, const wxClassInfo *classInfo, wxXmlNode *parent, const wxString& nodeName , int &nextId, map< wxObject* , int > &writtenObjects ) 
{
	map< string , int > writtenProperties ;
	wxXmlNode *onode;

    onode = new wxXmlNode(wxXML_ELEMENT_NODE, nodeName);

	onode->AddProperty(wxString("class"), wxString(classInfo->GetClassName()));
	if ( obj == NULL )
	{
	    wxXmlNode* nullnode = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, "null");
	    onode->AddChild(nullnode);
	}
	else
	{
		// if we have already written this object, just insert an id
		if ( writtenObjects.find( obj ) != writtenObjects.end() )
		{
			onode->AddProperty(wxString("id"), wxString::Format( "%d" , writtenObjects[obj] ) );
		}
		else
		{
			int id = nextId++ ;
			writtenObjects[obj] = id ;
			onode->AddProperty(wxString("id"), wxString::Format( "%d" , id ) );
			WriteComponentProperties( obj , classInfo , onode , nextId , writtenObjects, writtenProperties) ;
		}
	}

	parent->AddChild(onode);
}

// ----------------------------------------------------------------------------
// reading xml in 
// ----------------------------------------------------------------------------

wxxVariant wxReader::ReadPropertyValueNoAssign(wxXmlNode *Node,
				      wxClassInfo *ClassInfo,
					  const wxPropertyInfo * &pi ,
				      wxIDepersist *Callbacks)
{
    wxxVariant res;
    int ChildID;

    // form is:
    // <propname type=foo>value</propname>

    //ISSUE: NULL component references are streamed out as "null" text
    // node.  This is not in keeping with the XML mindset.

    pi = ClassInfo->FindPropertyInfo(Node->GetName());
    if (!pi)
    {
	// error handling, please
	assert(!"Property not found in extended class info");
    }

	const wxClassTypeInfo* cti = dynamic_cast< const wxClassTypeInfo* > ( pi->GetTypeInfo() ) ;
	if ( cti )
	{
		const wxClassInfo* eci = cti->GetClassInfo() ;

		ChildID = ReadComponent(Node , Callbacks);
		if (ChildID != -1)
		{
			if (genCode)
				res = wxxVariant(GetObjectName(ChildID));
			else
				res = eci->InstanceToVariant(GetObject(ChildID));
		}
		else
		{
			if (genCode)
				res = wxxVariant(wxString("NULL"));
			else
				res = eci->InstanceToVariant(NULL);
		}
	}
	else
	{
		const wxDelegateTypeInfo* dti = dynamic_cast< const wxDelegateTypeInfo* > ( pi->GetTypeInfo() ) ;
		if ( dti )
		{
			if (genCode)
			{
				// in which form should we code these ?
				res = wxxVariant( wxXmlGetContentFromNode(Node) ) ;
			}
			else
			{
				res = wxxVariant( wxXmlGetContentFromNode(Node) ) ;
			}
		}
		else
		{
			if (genCode)
			{
				if ( pi->GetTypeInfo()->GetKind() == wxT_STRING )
					res = wxxVariant( wxString::Format("wxString(\"%s\")",wxXmlGetContentFromNode(Node)));
				else
					res = wxxVariant( wxString::Format("%s(%s)",pi->GetTypeName(),wxXmlGetContentFromNode(Node) ) );
			}
			else
				res = pi->GetAccessor()->ReadValue(Node) ;
		}
	}
	return res ;
}

void wxReader::ReadPropertyValue(wxXmlNode *Node,
				      wxClassInfo *ClassInfo,
					  int ObjectID ,
				      wxIDepersist *Callbacks)
{
 	const wxPropertyInfo *pi;
	wxxVariant res = ReadPropertyValueNoAssign( Node , ClassInfo, pi , Callbacks ) ;

	const wxDelegateTypeInfo* dti = dynamic_cast< const wxDelegateTypeInfo* > ( pi->GetTypeInfo() ) ;

	if ( dti )
	{
		wxString resstring = res.Get<wxString>() ;
		wxInt32 pos = resstring.Find('.') ;
		assert( pos != wxNOT_FOUND ) ;
		int handlerOid = atol(resstring.Left(pos)) ;
		wxString handlerName = resstring.Mid(pos+1) ;
		
		if (Callbacks)
			Callbacks->SetConnect( ObjectID , ClassInfo , dti->GetEventType() , handlerName , handlerOid ) ;
	}
	else
	{
		if (Callbacks)
			Callbacks->SetProperty(ObjectID, ClassInfo, pi , res);
	}
}

struct wxReader::wxReaderInternal
{
    /*
	Code streamer will be storing names here.  Runtime object streamer
	will be storing actual pointers to objects here.  The two are never
	mixed.  So the Objects array either has data, or the ObjectNames
	array has data.  Never both. Keyed by ObjectID (int)
    */
    map<int,wxObject *> Objects;

    map<int,string> ObjectNames;
	// only used when generating code, since the names loose the type info
    map<int,wxClassInfo*> ObjectClasses;
};

wxReader::wxReader(bool GenerateCode) : genCode(GenerateCode)
{
    Data = new wxReaderInternal;
}

wxReader::~wxReader()
{
    delete Data;
}

wxObject *wxReader::GetObject(int id)
{
    assert( Data->Objects.find(id) != Data->Objects.end() );
    return Data->Objects[id];
}

wxString wxReader::GetObjectName(int id)
{
    assert( Data->ObjectNames.find(id) != Data->ObjectNames.end() );
    return wxString(Data->ObjectNames[id].c_str());
}

wxClassInfo* wxReader::GetObjectClassInfo(int id)
{
    assert( Data->ObjectClasses.find(id) != Data->ObjectClasses.end() );
    return Data->ObjectClasses[id] ;
}

void wxReader::SetObject(int id, wxObject *Object)
{
	assert(  Data->Objects.find(id) == Data->Objects.end()  ) ;
    Data->Objects[id] = Object;
}

void wxReader::SetObjectName(int id, const wxString &Name, wxClassInfo *ClassInfo )
{
	assert(  Data->ObjectNames.find(id) == Data->ObjectNames.end()  ) ;
    Data->ObjectNames[id] = (const char *)Name;
	Data->ObjectClasses[id] = ClassInfo ;
}

/* 
	Reading components has not to be extended for components
	as properties are always sought by typeinfo over all levels
	and create params are always toplevel class only
*/

int wxReader::ReadComponent(wxXmlNode *Node, wxIDepersist *Callbacks)
{
    wxString ClassName;
    wxClassInfo *ClassInfo;

	wxxVariant *CreateParams ;
    wxXmlNode *Children;
    int ObjectID;

    Callbacks->NotifyReader(this);

    Children = Node->GetChildren();
    if (!Node->GetPropVal("class", &ClassName))
    {
		// No class name.  Eek. FIXME: error handling
		return -1;
    }
	ClassInfo = wxClassInfo::FindClass(ClassName);
    if (Node->GetType() == wxXML_TEXT_NODE)
    {
		assert( wxXmlGetContentFromNode(Node) == "null" ) ;
		// this must be a NULL component reference.  We just bail out
		return -1;
	}

 	wxString ObjectIdString ;
    if (!Node->GetPropVal("id", &ObjectIdString))
    {
		// No object id.  Eek. FIXME: error handling
		return -1;
    }

    ObjectID = atoi( ObjectIdString.c_str() ) ;
	// is this object already has been streamed in, return it here
	if ( genCode )
	{
		if ( Data->ObjectNames.find( ObjectID ) != Data->ObjectNames.end() )
			return ObjectID ;
	}
	else
	{
		if ( Data->Objects.find( ObjectID ) != Data->Objects.end() )
			return ObjectID ;
	}

	// new object, start with allocation
    Callbacks->AllocateObject(ObjectID, ClassInfo);

	// 
    // stream back the Create parameters first
	CreateParams = new wxxVariant[ ClassInfo->GetCreateParamCount() ] ;

	typedef map<string, wxXmlNode *> PropertyNodes ;
	typedef vector<string> PropertyNames ;

	PropertyNodes propertyNodes ;
	PropertyNames propertyNames ;

	while( Children )
	{
		propertyNames.push_back( Children->GetName().c_str() ) ;
		propertyNodes[Children->GetName().c_str()] = Children ;
		Children = Children->GetNext() ;
	}

	for ( int i = 0 ; i <ClassInfo->GetCreateParamCount() ; ++i )
	{
		const wxChar* paramName = ClassInfo->GetCreateParamName(i) ;
		PropertyNodes::iterator propiter = propertyNodes.find( paramName ) ;
		// if we don't have the value of a create param set in the xml
		// we use the default value
		if ( propiter != propertyNodes.end() )
		{
			wxXmlNode* prop = propiter->second ;
			wxPropertyInfo* pi ;
			CreateParams[i] = ReadPropertyValueNoAssign( prop , ClassInfo , pi , Callbacks ) ;
			// CreateParams[i] = ClassInfo->FindPropertyInfo( ClassInfo->GetCreateParamName(i) ->GetAccessor()->ReadValue( prop ) ;
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
			CreateParams[i] = ClassInfo->FindPropertyInfo( paramName )->GetDefaultValue() ;
		}
	}

    // got the parameters.  Call the Create method
    Callbacks->CreateObject(ObjectID,
			    ClassInfo,
			    ClassInfo->GetCreateParamCount(),
			    &CreateParams[0]);

    // now stream in the rest of the properties, in the sequence their properties were written in the xml
	for ( size_t j = 0 ; j < propertyNames.size() ; ++j )
	{
		if ( propertyNames[j].length() )
		{
			PropertyNodes::iterator propiter = propertyNodes.find( propertyNames[j] ) ;
			if ( propiter != propertyNodes.end() )
			{
				wxXmlNode* prop = propiter->second ;
				string name = propiter->first ;
				ReadPropertyValue( prop , ClassInfo , ObjectID , Callbacks  ) ;
			}
		}
	}
	/*
	for( PropertyNodes::iterator propiter = propertyNodes.begin() ; propiter != propertyNodes.end() ; propiter++ )
	{
		wxXmlNode* prop = propiter->second ;
		string name = propiter->first ;
		ReadPropertyValue( prop , ClassInfo , ObjectID , Callbacks  ) ;
	}
	*/

    // FIXME: if the list of children is not NULL now, then that means that
    // there were properties in the XML not represented in the meta data
    // this just needs error handling.
    assert(!Children);

	delete[] CreateParams ;

    return ObjectID;
}

// ----------------------------------------------------------------------------
// depersisting to memory 
// ----------------------------------------------------------------------------

void wxIDepersistRuntime::AllocateObject(int ObjectID, wxClassInfo *ClassInfo)
{
    wxObject *O;
    O = ClassInfo->CreateObject();
    Reader->SetObject(ObjectID, O);
}

void wxIDepersistRuntime::CreateObject(int ObjectID,
				       wxClassInfo *ClassInfo,
				       int ParamCount,
					   wxxVariant *Params)
{
    wxObject *O;
    O = Reader->GetObject(ObjectID);
    ClassInfo->Create(O, ParamCount, Params);
}

void wxIDepersistRuntime::SetProperty(int ObjectID,
				      wxClassInfo *WXUNUSED(ClassInfo),
				      const wxPropertyInfo* PropertyInfo,
				      const wxxVariant &Value)
{
    wxObject *O;
    O = Reader->GetObject(ObjectID);
	PropertyInfo->GetAccessor()->SetProperty( O , Value ) ;
}

void wxIDepersistRuntime::SetConnect(int EventSourceObjectID,
			     wxClassInfo *WXUNUSED(EventSourceClassInfo),
				 int eventType ,
				 const wxString &handlerName ,
				 int EventSinkObjectID ) 
{
		wxWindow *ehsource = dynamic_cast< wxWindow* >( Reader->GetObject( EventSourceObjectID ) ) ;
		wxEvtHandler *ehsink = dynamic_cast< wxEvtHandler *>(Reader->GetObject(EventSinkObjectID) ) ;

		if ( ehsource && ehsink )
		{
			ehsource->Connect( ehsource->GetId() , eventType , 
				ehsink->GetClassInfo()->FindHandlerInfo(handlerName)->GetEventFunction() , NULL /*user data*/ , 
				ehsink ) ;
		}
}

// ----------------------------------------------------------------------------
// depersisting to code 
// ----------------------------------------------------------------------------


void wxIDepersistCode::AllocateObject(int ObjectID, wxClassInfo *ClassInfo)
{
	wxString objectName = wxString::Format( "LocalObject_%d" , ObjectID ) ;
	fp->WriteString( wxString::Format( "\t%s *%s = new %s;\n",
	    ClassInfo->GetClassName(),
	    objectName,
	    ClassInfo->GetClassName()) );
    Reader->SetObjectName(ObjectID, objectName, ClassInfo);
}

void wxIDepersistCode::CreateObject(int ObjectID,
				    wxClassInfo *WXUNUSED(ClassInfo),
				    int ParamCount,
				    wxxVariant *Params)
{
    int i;
	fp->WriteString( wxString::Format( "\t%s->Create(", Reader->GetObjectName(ObjectID) ) );
    for (i = 0; i < ParamCount; i++)
    {
		fp->WriteString( wxString::Format( "%s", (const char *)Params[i].Get<wxString>() ) );
		if (i < ParamCount - 1)
			fp->WriteString( ", ");
    }
    fp->WriteString( ");\n");
}

void wxIDepersistCode::SetProperty(int ObjectID,
				   wxClassInfo *WXUNUSED(ClassInfo),
				   const wxPropertyInfo* PropertyInfo,
				   const wxxVariant &Value)
{
	wxString d = Value.Get<wxString>() ;
    fp->WriteString( wxString::Format( "\t%s->%s(%s);\n",
	    Reader->GetObjectName(ObjectID),
	    PropertyInfo->GetAccessor()->GetSetterName(),
	    d) );
}

void wxIDepersistCode::SetConnect(int EventSourceObjectID,
			     wxClassInfo *WXUNUSED(EventSourceClassInfo),
				 int eventType ,
				 const wxString &handlerName ,
				 int EventSinkObjectID ) 
{
	wxString ehsource = Reader->GetObjectName( EventSourceObjectID ) ;
	wxString ehsink = Reader->GetObjectName(EventSinkObjectID) ;
	wxString ehsinkClass = Reader->GetObjectClassInfo(EventSinkObjectID)->GetClassName() ;

	fp->WriteString( wxString::Format(  "\t%s->Connect( %s->GetId() , %d , (wxObjectEventFunction)(wxEventFunction) & %s::%s , NULL , %s ) ;" , 
		ehsource , ehsource , eventType , ehsinkClass , handlerName , ehsink ) );
}

#endif
