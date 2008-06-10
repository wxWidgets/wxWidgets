/////////////////////////////////////////////////////////////////////////////
// Name:        wxwallctrltest.cpp
// Purpose:     
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     02/06/2008 12:13:29
// RCS-ID:      
// Copyright:   Copyright 2008 by Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/WallCtrl.h"
#include "wx/WallCtrlPlaneSurface.h"
#include "wx/WallCtrlBitmapSource.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "wxwallctrltest.h"

////@begin XPM images
////@end XPM images


/*!
 * wxWallCtrlTest type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxWallCtrlTest, wxDialog )


/*!
 * wxWallCtrlTest event table definition
 */

BEGIN_EVENT_TABLE( wxWallCtrlTest, wxDialog )

////@begin wxWallCtrlTest event table entries
////@end wxWallCtrlTest event table entries

END_EVENT_TABLE()


/*!
 * wxWallCtrlTest constructors
 */

wxWallCtrlTest::wxWallCtrlTest()
{
    Init();
}

wxWallCtrlTest::wxWallCtrlTest( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * wxWallCtrlTest creator
 */

bool wxWallCtrlTest::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxWallCtrlTest creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end wxWallCtrlTest creation
    return true;
}


/*!
 * wxWallCtrlTest destructor
 */

wxWallCtrlTest::~wxWallCtrlTest()
{
////@begin wxWallCtrlTest destruction
////@end wxWallCtrlTest destruction
}


/*!
 * Member initialisation
 */

void wxWallCtrlTest::Init()
{
////@begin wxWallCtrlTest member initialisation
////@end wxWallCtrlTest member initialisation
}


/*!
 * Control creation for wxWallCtrlTest
 */

void wxWallCtrlTest::CreateControls()
{    
////@begin wxWallCtrlTest content construction
    wxWallCtrlTest* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

	wxWallCtrlBitmapSource * bitmapSource = new wxWallCtrlBitmapSource();
	wxBitmap testBitmap;

	// Just add bitmaps manually
	testBitmap.LoadFile("S:\\Wall1.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall2.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall3.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);

	testBitmap.LoadFile("S:\\Wall1.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall2.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall3.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);

	testBitmap.LoadFile("S:\\Wall1.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall2.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall3.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);

	testBitmap.LoadFile("S:\\Wall1.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall2.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall3.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);

	testBitmap.LoadFile("S:\\Wall1.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall2.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);
	testBitmap.LoadFile("S:\\Wall3.bmp", wxBITMAP_TYPE_BMP);
	bitmapSource->AppendBitmap(testBitmap);

	wxWallCtrlSurface * surface = new wxWallCtrlPlaneSurface(bitmapSource);
	wxWallCtrl* wallCtrl1 = new wxWallCtrl(this, surface, wxID_ANY,	wxDefaultPosition, wxDefaultSize);
	itemBoxSizer2->Add(wallCtrl1, 1, wxGROW|wxALL, 5);

 /*   wxButton* itemButton3 = new wxButton( itemDialog1, ID_BUTTON2, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemButton3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxScrolledWindow* itemScrolledWindow5 = new wxScrolledWindow( itemDialog1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxSize(100, 100), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer4->Add(itemScrolledWindow5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    itemScrolledWindow5->SetScrollbars(1, 1, 0, 0);
    wxButton* itemButton6 = new wxButton( itemScrolledWindow5, ID_BUTTON3, _("Button"), wxDefaultPosition, wxSize(40, 40), 0 );

    wxButton* itemButton7 = new wxButton( itemScrolledWindow5, ID_BUTTON1, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );

    wxPanel* itemPanel8 = new wxPanel( itemDialog1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
*/
////@end wxWallCtrlTest content construction
}


/*!
 * Should we show tooltips?
 */

bool wxWallCtrlTest::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxWallCtrlTest::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxWallCtrlTest bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxWallCtrlTest bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxWallCtrlTest::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxWallCtrlTest icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxWallCtrlTest icon retrieval
}
