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
const char *wxButtonNameStr = "button";
const char *wxCanvasNameStr = "canvas";
const char *wxCheckBoxNameStr = "check";
const char *wxChoiceNameStr = "choice";
const char *wxComboBoxNameStr = "comboBox";
const char *wxDialogNameStr = "dialog";
const char *wxFrameNameStr = "frame";
const char *wxGaugeNameStr = "gauge";
const char *wxStaticBoxNameStr = "groupBox";
const char *wxListBoxNameStr = "listBox";
const char *wxStaticTextNameStr = "message";
const char *wxStaticBitmapNameStr = "message";
const char *wxMultiTextNameStr = "multitext";
const char *wxPanelNameStr = "panel";
const char *wxRadioBoxNameStr = "radioBox";
const char *wxRadioButtonNameStr = "radioButton";
const char *wxBitmapRadioButtonNameStr = "radioButton";
const char *wxScrollBarNameStr = "scrollBar";
const char *wxSliderNameStr = "slider";
const char *wxStaticNameStr = "static";
const char *wxTextCtrlWindowNameStr = "textWindow";
const char *wxTextCtrlNameStr = "text";
const char *wxVirtListBoxNameStr = "virtListBox";
const char *wxButtonBarNameStr = "buttonbar";
const char *wxEnhDialogNameStr = "Shell";
const char *wxToolBarNameStr = "toolbar";
const char *wxStatusLineNameStr = "status_line";

const char *wxGetTextFromUserPromptStr = "Input Text";
const char *wxMessageBoxCaptionStr = "Message";
const char *wxFileSelectorPromptStr = "Select a file";
const char *wxFileSelectorDefaultWildcardStr = "*.*";
const char *wxInternalErrorStr = "wxWindows Internal Error";
const char *wxFatalErrorStr = "wxWindows Fatal Error";

// See wx/utils.h
const char *wxFloatToStringStr = "%.2f";
const char *wxDoubleToStringStr = "%.2f";

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
