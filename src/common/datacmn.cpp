///////////////////////////////////////////////////////////////////////////////
// Name:        common/datacmn.cpp
// Purpose:     contains definitions of various global wxWidgets variables
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.04.03 (from src/*/data.cpp files)
// RCS-ID:      $Id$
// Copyright:   (c) 1997-2002 wxWidgets development team
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/treectrl.h"
#endif // WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

// Useful buffer, initialized in wxCommonInit
wxChar *wxBuffer = NULL;

// Windows List
wxWindowList wxTopLevelWindows;

// List of windows pending deletion
wxList WXDLLEXPORT wxPendingDelete;

int wxPageNumber;

// GDI Object Lists
wxFontList   *wxTheFontList = NULL;
wxPenList    *wxThePenList = NULL;
wxBrushList  *wxTheBrushList = NULL;
wxBitmapList *wxTheBitmapList = NULL;
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
#if wxUSE_ACCEL
wxAcceleratorTable wxNullAcceleratorTable;
#endif // wxUSE_ACCEL

wxBitmap  wxNullBitmap;
wxIcon    wxNullIcon;
wxCursor  wxNullCursor;
wxPen     wxNullPen;
wxBrush   wxNullBrush;
#if wxUSE_PALETTE
wxPalette wxNullPalette;
#endif // wxUSE_PALETTE
wxFont    wxNullFont;
wxColour  wxNullColour;

// Default window names
WXDLLEXPORT_DATA(const wxChar *) wxControlNameStr = wxT("control");
WXDLLEXPORT_DATA(const wxChar *) wxButtonNameStr = wxT("button");
WXDLLEXPORT_DATA(const wxChar *) wxButtonBarNameStr = wxT("buttonbar");
WXDLLEXPORT_DATA(const wxChar *) wxCanvasNameStr = wxT("canvas");
WXDLLEXPORT_DATA(const wxChar *) wxCheckBoxNameStr = wxT("check");
WXDLLEXPORT_DATA(const wxChar *) wxChoiceNameStr = wxT("choice");
WXDLLEXPORT_DATA(const wxChar *) wxComboBoxNameStr = wxT("comboBox");
WXDLLEXPORT_DATA(const wxChar *) wxDialogNameStr = wxT("dialog");
WXDLLEXPORT_DATA(const wxChar *) wxFrameNameStr = wxT("frame");
WXDLLEXPORT_DATA(const wxChar *) wxGaugeNameStr = wxT("gauge");
WXDLLEXPORT_DATA(const wxChar *) wxStaticBoxNameStr = wxT("groupBox");
WXDLLEXPORT_DATA(const wxChar *) wxListBoxNameStr = wxT("listBox");
WXDLLEXPORT_DATA(const wxChar *) wxListCtrlNameStr = wxT("listctrl");
WXDLLEXPORT_DATA(const wxChar *) wxStaticTextNameStr = wxT("message");
WXDLLEXPORT_DATA(const wxChar *) wxStaticBitmapNameStr = wxT("message");
WXDLLEXPORT_DATA(const wxChar *) wxMultiTextNameStr = wxT("multitext");
WXDLLEXPORT_DATA(const wxChar *) wxNotebookNameStr = wxT("notebook");
WXDLLEXPORT_DATA(const wxChar *) wxPanelNameStr = wxT("panel");
WXDLLEXPORT_DATA(const wxChar *) wxRadioBoxNameStr = wxT("radioBox");
WXDLLEXPORT_DATA(const wxChar *) wxRadioButtonNameStr = wxT("radioButton");
WXDLLEXPORT_DATA(const wxChar *) wxBitmapRadioButtonNameStr = wxT("radioButton");
WXDLLEXPORT_DATA(const wxChar *) wxScrollBarNameStr = wxT("scrollBar");
WXDLLEXPORT_DATA(const wxChar *) wxSliderNameStr = wxT("slider");
WXDLLEXPORT_DATA(const wxChar *) wxStaticNameStr = wxT("static");
WXDLLEXPORT_DATA(const wxChar *) wxStatusLineNameStr = wxT("status_line");
WXDLLEXPORT_DATA(const wxChar *) wxTextCtrlWindowNameStr = wxT("textWindow");
WXDLLEXPORT_DATA(const wxChar *) wxTextCtrlNameStr = wxT("text");
WXDLLEXPORT_DATA(const wxChar *) wxTreeCtrlNameStr = wxT("treeCtrl");
WXDLLEXPORT_DATA(const wxChar *) wxEnhDialogNameStr = wxT("Shell");
WXDLLEXPORT_DATA(const wxChar *) wxToolBarNameStr = wxT("toolbar");
WXDLLEXPORT_DATA(const wxChar *) wxVirtListBoxNameStr = wxT("virtListBox");

// Default messages
WXDLLEXPORT_DATA(const wxChar *) wxGetTextFromUserPromptStr = wxT("Input Text");
WXDLLEXPORT_DATA(const wxChar *) wxGetPasswordFromUserPromptStr = wxT("Enter Password");
WXDLLEXPORT_DATA(const wxChar *) wxMessageBoxCaptionStr = wxT("Message");
WXDLLEXPORT_DATA(const wxChar *) wxFileSelectorPromptStr = wxT("Select a file");

// Other default strings
WXDLLEXPORT_DATA(const wxChar *) wxFileSelectorDefaultWildcardStr =
#if defined(__WXMSW__) || defined(__WXOS2__)
    wxT("*.*")
#else // Unix/Mac
    wxT("*")
#endif
    ;
WXDLLEXPORT_DATA(const wxChar *) wxDirDialogNameStr = wxT("wxDirCtrl");
WXDLLEXPORT_DATA(const wxChar *) wxDirDialogDefaultFolderStr = wxT("/");

#if defined(__WXMSW__) || defined(__WXOS2__)
WXDLLEXPORT_DATA(const wxChar *) wxUserResourceStr = wxT("TEXT");
#endif


const wxSize wxDefaultSize(wxDefaultCoord, wxDefaultCoord);
const wxPoint wxDefaultPosition(wxDefaultCoord, wxDefaultCoord);

