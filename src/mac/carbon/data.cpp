/////////////////////////////////////////////////////////////////////////////
// Name:        data.cpp
// Purpose:     Various data
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wx.h"

#if wxUSE_POSTSCRIPT
#include "wx/generic/dcpsg.h"
#endif

#define _MAXPATHLEN 500

// Useful buffer, initialized in CommonInit
char *wxBuffer = NULL;

// Windows List
wxWindowList wxTopLevelWindows;

// List of windows pending deletion
wxList wxPendingDelete;

int wxPageNumber;

// GDI Object Lists
wxBrushList *wxTheBrushList = NULL;
wxPenList   *wxThePenList = NULL;
wxFontList   *wxTheFontList = NULL;
wxBitmapList   *wxTheBitmapList = NULL;

wxColourDatabase *wxTheColourDatabase = NULL;

// Stock objects
wxFont *wxNORMAL_FONT;
wxFont *wxSMALL_FONT;
wxFont *wxITALIC_FONT;
wxFont *wxSWISS_FONT;
wxPen *wxRED_PEN;

wxPen *wxCYAN_PEN;
wxPen *wxGREEN_PEN;
wxPen *wxBLACK_PEN;
wxPen *wxWHITE_PEN;
wxPen *wxTRANSPARENT_PEN;
wxPen *wxBLACK_DASHED_PEN;
wxPen *wxGREY_PEN;
wxPen *wxMEDIUM_GREY_PEN;
wxPen *wxLIGHT_GREY_PEN;

wxBrush *wxBLUE_BRUSH;
wxBrush *wxGREEN_BRUSH;
wxBrush *wxWHITE_BRUSH;
wxBrush *wxBLACK_BRUSH;
wxBrush *wxTRANSPARENT_BRUSH;
wxBrush *wxCYAN_BRUSH;
wxBrush *wxRED_BRUSH;
wxBrush *wxGREY_BRUSH;
wxBrush *wxMEDIUM_GREY_BRUSH;
wxBrush *wxLIGHT_GREY_BRUSH;

wxColour *wxBLACK;
wxColour *wxWHITE;
wxColour *wxRED;
wxColour *wxBLUE;
wxColour *wxGREEN;
wxColour *wxCYAN;
wxColour *wxLIGHT_GREY;

wxCursor *wxSTANDARD_CURSOR = NULL;
wxCursor *wxHOURGLASS_CURSOR = NULL;
wxCursor *wxCROSS_CURSOR = NULL;

// 'Null' objects
wxAcceleratorTable wxNullAcceleratorTable;
wxBitmap wxNullBitmap;
wxIcon   wxNullIcon;
wxCursor wxNullCursor;
wxPen    wxNullPen;
wxBrush  wxNullBrush;
wxPalette wxNullPalette;
wxFont   wxNullFont;
wxColour wxNullColour;

// Default window names
const wxChar *wxButtonNameStr = wxT("button");
const wxChar *wxCanvasNameStr = wxT("canvas");
const wxChar *wxCheckBoxNameStr = wxT("check");
const wxChar *wxChoiceNameStr = wxT("choice");
const wxChar *wxComboBoxNameStr = wxT("comboBox");
const wxChar *wxDialogNameStr = wxT("dialog");
const wxChar *wxFrameNameStr = wxT("frame");
const wxChar *wxGaugeNameStr = wxT("gauge");
const wxChar *wxStaticBoxNameStr = wxT("groupBox");
const wxChar *wxListBoxNameStr = wxT("listBox");
const wxChar *wxStaticTextNameStr = wxT("statictext");
const wxChar *wxStaticBitmapNameStr = wxT("staticbitmap");
const wxChar *wxMultiTextNameStr = wxT("multitext");
const wxChar *wxPanelNameStr = wxT("panel");
const wxChar *wxRadioBoxNameStr = wxT("radioBox");
const wxChar *wxRadioButtonNameStr = wxT("radioButton");
const wxChar *wxBitmapRadioButtonNameStr = wxT("radioButton");
const wxChar *wxScrollBarNameStr = wxT("scrollBar");
const wxChar *wxSliderNameStr = wxT("slider");
const wxChar *wxStaticNameStr = wxT("static");
const wxChar *wxTextCtrlWindowNameStr = wxT("textWindow");
const wxChar *wxTextCtrlNameStr = wxT("text");
const wxChar *wxVirtListBoxNameStr = wxT("virtListBox");
const wxChar *wxButtonBarNameStr = wxT("buttonbar");
const wxChar *wxEnhDialogNameStr = wxT("Shell");
const wxChar *wxToolBarNameStr = wxT("toolbar");
const wxChar *wxStatusLineNameStr = wxT("status_line");

const wxChar *wxGetTextFromUserPromptStr = wxT("Input Text");
const wxChar *wxMessageBoxCaptionStr = wxT("Message");
const wxChar *wxFileSelectorPromptStr = wxT("Select a file");
const wxChar *wxFileSelectorDefaultWildcardStr = wxT("*.*");
const wxChar *wxInternalErrorStr = wxT("wxWindows Internal Error");
const wxChar *wxFatalErrorStr = wxT("wxWindows Fatal Error");
const wxChar *wxTreeCtrlNameStr = wxT("treeCtrl");
const wxChar *wxDirDialogNameStr = wxT("wxDirCtrl");
const wxChar *wxDirDialogDefaultFolderStr = wxT("/");

// See wx/utils.h
const wxChar *wxFloatToStringStr = wxT("%.2f");
const wxChar *wxDoubleToStringStr = wxT("%.2f");

#if wxUSE_SHARED_LIBRARY
///// Event tables (also must be in one, statically-linked file for shared libraries)

// This is the base, wxEvtHandler 'bootstrap' code which is expanded manually here
const wxEventTable *wxEvtHandler::GetEventTable() const { return &wxEvtHandler::sm_eventTable; }

const wxEventTable wxEvtHandler::sm_eventTable =
	{ NULL, &wxEvtHandler::sm_eventTableEntries[0] };

const wxEventTableEntry wxEvtHandler::sm_eventTableEntries[] = { { 0, 0, 0, NULL } };
#endif

const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);
