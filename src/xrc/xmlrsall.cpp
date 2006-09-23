/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xmlrsall.cpp
// Purpose:     wxXmlResource::InitAllHandlers
// Author:      Vaclav Slavik
// Created:     2000/03/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

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
    AddHandler(new wxStdDialogButtonSizerXmlHandler);
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
    AddHandler(new wxCheckListBoxXmlHandler);
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
#if wxUSE_TOGGLEBTN
    AddHandler(new wxToggleButtonXmlHandler);
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
#if wxUSE_ODCOMBOBOX
    AddHandler(new wxOwnerDrawnComboBoxXmlHandler);
#endif
#if wxUSE_BITMAPCOMBOBOX
    AddHandler(new wxBitmapComboBoxXmlHandler);
#endif
#if wxUSE_NOTEBOOK
    AddHandler(new wxNotebookXmlHandler);
#endif
#if wxUSE_LISTBOOK
    AddHandler(new wxListbookXmlHandler);
#endif
#if wxUSE_CHOICEBOOK
    AddHandler(new wxChoicebookXmlHandler);
#endif
#if wxUSE_TREEBOOK
    AddHandler(new wxTreebookXmlHandler);
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
    AddHandler(new wxSplitterWindowXmlHandler);
#if wxUSE_WIZARDDLG
    AddHandler(new wxWizardXmlHandler);
#endif
#if wxUSE_STATUSBAR
    AddHandler(new wxStatusBarXmlHandler);
#endif
#if wxUSE_DATEPICKCTRL
    AddHandler(new wxDateCtrlXmlHandler);
#endif
#if wxUSE_COLOURPICKERCTRL
    AddHandler(new wxColourPickerCtrlXmlHandler);
#endif
#if wxUSE_FILEPICKERCTRL
    AddHandler(new wxFilePickerCtrlXmlHandler);
#endif
#if wxUSE_DIRPICKERCTRL
    AddHandler(new wxDirPickerCtrlXmlHandler);
#endif
#if wxUSE_FONTPICKERCTRL
    AddHandler(new wxFontPickerCtrlXmlHandler);
#endif
#if wxUSE_MDI
    AddHandler(new wxMdiXmlHandler);
#endif
#if wxUSE_GRID
    AddHandler( new wxGridXmlHandler);
#endif
#if wxUSE_HYPERLINKCTRL
    AddHandler( new wxHyperlinkCtrlXmlHandler);
#endif
}

#endif // wxUSE_XRC
