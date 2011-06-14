/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_searchctrl_n.mm
// Purpose:     wxMoSearchCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/searchctrl.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoSearchCtrl, wxSearchCtrl)


// FIXME stub


#pragma mark -
#pragma mark Definition of absent wxSearch methods

BEGIN_EVENT_TABLE(wxSearchCtrl, wxSearchCtrlBase)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase)

wxSearchCtrl::wxSearchCtrl()
{
	// FIXME stub	
}

wxSearchCtrl::~wxSearchCtrl()
{
	// FIXME stub	
}

void wxSearchCtrl::Init() {
	// FIXME stub
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            			  const wxString& value,
            			  const wxPoint& pos,
            			  const wxSize& size,
            			  long style,
            			  const wxValidator& validator,
            			  const wxString& name)
{
	// FIXME stub
	
	return true;
}

wxSize wxSearchCtrl::DoGetBestSize() const {
	
	// FIXME stub
	
	wxSize empty(1, 1);
	return empty;
}

void wxSearchCtrl::ShowSearchButton( bool show ) {
	// FIXME stub
}

bool wxSearchCtrl::IsSearchButtonVisible() const {
	// FIXME stub
	return true;
}

void wxSearchCtrl::ShowCancelButton( bool show ) {
	// FIXME stub
}

bool wxSearchCtrl::IsCancelButtonVisible() const {
	// FIXME stub
	return true;
}

void wxSearchCtrl::SetMenu( wxMenu* menu ) {
	// FIXME stub
}

wxMenu* wxSearchCtrl::GetMenu() {
	// FIXME stub
	return NULL;
}

bool wxSearchCtrl::HandleSearchFieldSearchHit() {
	// FIXME stub
	return true;
}

bool wxSearchCtrl::HandleSearchFieldCancelHit() {
	// FIXME stub
	return true;
}
