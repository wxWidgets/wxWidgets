/////////////////////////////////////////////////////////////////////////////
// Name:        xmlrsall.cpp
// Purpose:     wxXmlResource::InitAllHandlers
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
// -- Already done in xmlres.cpp
//#ifdef __GNUG__
//#pragma implementation "xmlres.h"
//#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_all.h"

void wxXmlResource::InitAllHandlers()
{
    AddHandler(new wxBitmapXmlHandler);
    AddHandler(new wxIconXmlHandler);
    AddHandler(new wxMenuXmlHandler);
    AddHandler(new wxMenuBarXmlHandler);
    AddHandler(new wxDialogXmlHandler);
    AddHandler(new wxPanelXmlHandler);
    AddHandler(new wxSizerXmlHandler);
    AddHandler(new wxButtonXmlHandler);
    AddHandler(new wxBitmapButtonXmlHandler);
    AddHandler(new wxStaticTextXmlHandler);
    AddHandler(new wxStaticBoxXmlHandler);
    AddHandler(new wxStaticBitmapXmlHandler);
    AddHandler(new wxTreeCtrlXmlHandler);
#if wxUSE_CALENDARCTRL
    AddHandler(new wxCalendarCtrlXmlHandler);
#endif
    AddHandler(new wxListCtrlXmlHandler);
#if wxUSE_CHECKLISTBOX
    AddHandler(new wxCheckListXmlHandler);
#endif
#if wxUSE_CHOICE
    AddHandler(new wxChoiceXmlHandler);
#endif
#if wxUSE_SLIDER
    AddHandler(new wxSliderXmlHandler);
#endif
#if wxUSE_GAUGE
    AddHandler(new wxGaugeXmlHandler);
#endif
#if wxUSE_CHECKBOX
    AddHandler(new wxCheckBoxXmlHandler);
#endif    
#if wxUSE_HTML
    AddHandler(new wxHtmlWindowXmlHandler);
#endif    
#if wxUSE_SPINBTN
    AddHandler(new wxSpinButtonXmlHandler);
#endif    
#if wxUSE_SPINCTRL
    AddHandler(new wxSpinCtrlXmlHandler);
#endif    
#if wxUSE_SCROLLBAR
    AddHandler(new wxScrollBarXmlHandler);
#endif
#if wxUSE_RADIOBOX
    AddHandler(new wxRadioBoxXmlHandler);
    AddHandler(new wxRadioButtonXmlHandler);
#endif
#if wxUSE_COMBOBOX
    AddHandler(new wxComboBoxXmlHandler);
#endif
#if wxUSE_NOTEBOOK
    AddHandler(new wxNotebookXmlHandler);
#endif
    AddHandler(new wxTextCtrlXmlHandler);
#if wxUSE_LISTBOX
    AddHandler(new wxListBoxXmlHandler);
#endif
#if wxUSE_TOOLBAR
    AddHandler(new wxToolBarXmlHandler);
#endif
#if wxUSE_STATLINE
    AddHandler(new wxStaticLineXmlHandler);
#endif
    AddHandler(new wxUnknownWidgetXmlHandler);
#if wxUSE_DIRDLG
    AddHandler(new wxGenericDirCtrlXmlHandler);
#endif
    AddHandler(new wxFrameXmlHandler);
    AddHandler(new wxScrolledWindowXmlHandler);
}
