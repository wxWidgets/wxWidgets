/////////////////////////////////////////////////////////////////////////////
// Name:        edlist.cpp
// Purpose:     Resource editor project management tree
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "edlist.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"

#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/choice.h"
#include "wx/listbox.h"
#include "wx/radiobox.h"
#include "wx/statbox.h"
#include "wx/gauge.h"
#include "wx/slider.h"
#include "wx/textctrl.h"
#endif

#include "edlist.h"
#include "reseditr.h"

BEGIN_EVENT_TABLE(wxResourceEditorControlList, wxListCtrl)
END_EVENT_TABLE()

wxResourceEditorControlList::wxResourceEditorControlList(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style):
     wxListCtrl(parent, id, pos, size, style), m_imageList(16, 16, TRUE)
{
    Initialize();
}

wxResourceEditorControlList::~wxResourceEditorControlList()
{
    SetImageList(NULL, wxIMAGE_LIST_SMALL);
}

// Load icons and add to the list
void wxResourceEditorControlList::Initialize()
{
#ifdef __WXMSW__
    wxIcon icon1("ARROW_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon2("BUTTON_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon3("BMPBUTTON_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon4("STATICTEXT_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon5("STATICBMP_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon6("STATICBOX_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon7("TEXTSING_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon8("TEXTMULT_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon9("LISTBOX_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon10("CHOICE_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon11("COMBOBOX_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon12("CHECKBOX_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon13("SLIDER_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon14("GAUGE_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon15("RADIOBOX_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon16("RADIOBTN_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
    wxIcon icon17("SCROLBAR_ICON", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);
#else

#include "bitmaps/arrow.xpm"
    wxIcon icon1( arrow_xpm );
#include "bitmaps/button.xpm"
    wxIcon icon2( button_xpm );
#include "bitmaps/bmpbuttn.xpm"
    wxIcon icon3( bmpbuttn_xpm );
#include "bitmaps/stattext.xpm"
    wxIcon icon4( stattext_xpm );
#include "bitmaps/statbmp.xpm"
    wxIcon icon5( statbmp_xpm );
#include "bitmaps/statbox.xpm"
    wxIcon icon6( statbox_xpm );
#include "bitmaps/textsing.xpm"
    wxIcon icon7( textsing_xpm );
#include "bitmaps/textmult.xpm"
    wxIcon icon8( textmult_xpm );
#include "bitmaps/listbox.xpm"
    wxIcon icon9( listbox_xpm );
#include "bitmaps/choice.xpm"
    wxIcon icon10( choice_xpm );
#include "bitmaps/combobox.xpm"
    wxIcon icon11( combobox_xpm );
#include "bitmaps/checkbox.xpm"
    wxIcon icon12( checkbox_xpm );
#include "bitmaps/slider.xpm"
    wxIcon icon13( slider_xpm );
#include "bitmaps/gauge.xpm"
    wxIcon icon14( gauge_xpm );
#include "bitmaps/radiobox.xpm"
    wxIcon icon15( radiobox_xpm );
#include "bitmaps/radiobtn.xpm"
    wxIcon icon16( radiobtn_xpm );
#include "bitmaps/scrolbar.xpm"
    wxIcon icon17( scrolbar_xpm );
#endif

    m_imageList.Add(icon1);
    m_imageList.Add(icon2);
    m_imageList.Add(icon3);
    m_imageList.Add(icon4);
    m_imageList.Add(icon5);
    m_imageList.Add(icon6);
    m_imageList.Add(icon7);
    m_imageList.Add(icon8);
    m_imageList.Add(icon9);
    m_imageList.Add(icon10);
    m_imageList.Add(icon11);
    m_imageList.Add(icon12);
    m_imageList.Add(icon13);
    m_imageList.Add(icon14);
    m_imageList.Add(icon15);
    m_imageList.Add(icon16);
    m_imageList.Add(icon17);
    
    SetImageList(& m_imageList, wxIMAGE_LIST_SMALL);

    long id = InsertItem(0,             "Pointer", 0);
    id = InsertItem(1,                  "wxButton", 1);
    id = InsertItem(2,                  "wxBitmapButton", 2);
    id = InsertItem(3,                  "wxStaticText", 3);
    id = InsertItem(4,                  "wxStaticBitmap", 4);
    id = InsertItem(5,                  "wxStaticBox", 5);
    id = InsertItem(6,                  "wxTextCtrl", 6);
    id = InsertItem(7,                  "wxTextCtrl", 7);
    id = InsertItem(8,                  "wxListBox", 8);
    id = InsertItem(9,                  "wxChoice", 9);
    id = InsertItem(10,                  "wxComboBox", 10);
    id = InsertItem(11,                  "wxCheckBox", 11);
    id = InsertItem(12,                  "wxSlider", 12);
    id = InsertItem(13,                  "wxGauge", 13);
    id = InsertItem(14,                  "wxRadioBox", 14);
    id = InsertItem(15,                  "wxRadioButton", 15);
    id = InsertItem(16,                  "wxScrollBar", 16);

/*
    InsertItem(RESED_TREECTRL,          "wxTreeCtrl", 16);
    InsertItem(RESED_LISTCTRL,          "wxListCtrl", 17);
    InsertItem(RESED_SPINBUTTON,        "wxSpinButton", 18);
*/

//    SetColumnWidth(-1, 80);
}

// Get selection, or -1
long wxResourceEditorControlList::GetSelection() const
{
    return GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}


