/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     ??/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CBSTORE_G__
#define __CBSTORE_G__

#include "controlbar.h"
#include "objstore.h" // used for persistance of control-bars

// serializers for some additional classes placed here
#include "rowdragpl.h"
#include "toolwnd.h"
#include "newbmpbtn.h"
#include "dyntbar.h"
#include "controlarea.h"

// serialziers for common components of frame-layout engine

class wxFrameLayoutSerializer : public wxEvtHandlerSerializer
{
	DECLARE_SERIALIZER_CLASS( wxFrameLayoutSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );
};

class cbBarSpySerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbBarSpySerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );
};

class cbBarDimHandlerBaseSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbBarDimHandlerBaseSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class cbDimInfoSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbDimInfoSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class cbRowInfoSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbRowInfoSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};
class cbBarInfoSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbBarInfoSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};
class cbCommonPanePropertiesSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbCommonPanePropertiesSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class cbDockPaneSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbDockPaneSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class cbUpdatesManagerBaseSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbUpdatesManagerBaseSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class cbPluginBaseSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbPluginBaseSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );
};

class cbRowDragPluginSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbRowDragPluginSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );
};

class cbHiddenBarInfoSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( cbHiddenBarInfoSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class cbFloatedBarWindowSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( cbFloatedBarWindowSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );

	static void CreateFloatedBarWindowFn( cbFloatedBarWindow* fbar, wxWindow* parent, const wxWindowID id, 
										  const wxPoint& pos, const wxSize& size, long style,
										  const wxString& name );
};

/*** serializers for some additional classes (FOR NOW:: also placed here) ***/

class wxNewBitmapButtonSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxNewBitmapButtonSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );

	static void CreateNewBmpBtnWindowFn( wxNewBitmapButton* btn, wxWindow* parent, const wxWindowID id, 
										 const wxPoint& pos, const wxSize& size, long style,
										 const wxString& name );
};

class wxDynamicToolBarSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxDynamicToolBarSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );

	static void CreateDynTBarWindowFn( wxDynamicToolBar* btn, wxWindow* parent, const wxWindowID id, 
									   const wxPoint& pos, const wxSize& size, long style,
									   const wxString& name );
};

class wxDynToolInfoSerializer : public wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( wxDynToolInfoSerializer );

	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

class wxTabbedWindowSerializer : public wxWindowSerializer
{
	DECLARE_SERIALIZER_CLASS( wxTabbedWindowSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );

	static void Initialize( wxObject* pObj );
};

class twTabInfoSerializer : wxSerializerBase
{
	DECLARE_SERIALIZER_CLASS( twTabInfoSerializer );
public:
	static void Serialize( wxObject* pObj, wxObjectStorage& store );
};

#endif