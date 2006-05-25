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
wxColourDatabase *wxTheColourDatabase = NULL;

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
extern WXDLLEXPORT_DATA(const wxChar) wxControlNameStr[] = wxT("control");
extern WXDLLEXPORT_DATA(const wxChar) wxButtonNameStr[] = wxT("button");
extern WXDLLEXPORT_DATA(const wxChar) wxCheckBoxNameStr[] = wxT("check");
extern WXDLLEXPORT_DATA(const wxChar) wxChoiceNameStr[] = wxT("choice");
extern WXDLLEXPORT_DATA(const wxChar) wxComboBoxNameStr[] = wxT("comboBox");
extern WXDLLEXPORT_DATA(const wxChar) wxDialogNameStr[] = wxT("dialog");
extern WXDLLEXPORT_DATA(const wxChar) wxFrameNameStr[] = wxT("frame");
extern WXDLLEXPORT_DATA(const wxChar) wxGaugeNameStr[] = wxT("gauge");
extern WXDLLEXPORT_DATA(const wxChar) wxStaticBoxNameStr[] = wxT("groupBox");
extern WXDLLEXPORT_DATA(const wxChar) wxListBoxNameStr[] = wxT("listBox");
extern WXDLLEXPORT_DATA(const wxChar) wxListCtrlNameStr[] = wxT("listCtrl");
extern WXDLLEXPORT_DATA(const wxChar) wxStaticTextNameStr[] = wxT("staticText");
extern WXDLLEXPORT_DATA(const wxChar) wxStaticBitmapNameStr[] = wxT("staticBitmap");
extern WXDLLEXPORT_DATA(const wxChar) wxNotebookNameStr[] = wxT("notebook");
extern WXDLLEXPORT_DATA(const wxChar) wxPanelNameStr[] = wxT("panel");
extern WXDLLEXPORT_DATA(const wxChar) wxRadioBoxNameStr[] = wxT("radioBox");
extern WXDLLEXPORT_DATA(const wxChar) wxRadioButtonNameStr[] = wxT("radioButton");
extern WXDLLEXPORT_DATA(const wxChar) wxBitmapRadioButtonNameStr[] = wxT("radioButton");
extern WXDLLEXPORT_DATA(const wxChar) wxScrollBarNameStr[] = wxT("scrollBar");
extern WXDLLEXPORT_DATA(const wxChar) wxSliderNameStr[] = wxT("slider");
extern WXDLLEXPORT_DATA(const wxChar) wxStatusLineNameStr[] = wxT("status_line");
extern WXDLLEXPORT_DATA(const wxChar) wxTextCtrlNameStr[] = wxT("text");
extern WXDLLEXPORT_DATA(const wxChar) wxTreeCtrlNameStr[] = wxT("treeCtrl");
extern WXDLLEXPORT_DATA(const wxChar) wxEnhDialogNameStr[] = wxT("Shell");
extern WXDLLEXPORT_DATA(const wxChar) wxToolBarNameStr[] = wxT("toolbar");
extern WXDLLEXPORT_DATA(const wxChar) wxDataViewCtrlNameStr[] = wxT("dataviewCtrl");

// Default messages
extern WXDLLEXPORT_DATA(const wxChar) wxGetTextFromUserPromptStr[] = wxT("Input Text");
extern WXDLLEXPORT_DATA(const wxChar) wxGetPasswordFromUserPromptStr[] = wxT("Enter Password");
extern WXDLLEXPORT_DATA(const wxChar) wxMessageBoxCaptionStr[] = wxT("Message");
extern WXDLLEXPORT_DATA(const wxChar) wxFileSelectorPromptStr[] = wxT("Select a file");

// Other default strings
extern WXDLLEXPORT_DATA(const wxChar) wxFileSelectorDefaultWildcardStr[] =
#if defined(__WXMSW__) || defined(__OS2__)
    wxT("*.*")
#else // Unix/Mac
    wxT("*")
#endif
    ;
extern WXDLLEXPORT_DATA(const wxChar) wxDirDialogNameStr[] = wxT("wxDirCtrl");
extern WXDLLEXPORT_DATA(const wxChar) wxDirDialogDefaultFolderStr[] = wxT("/");

#if defined(__WXMSW__) || defined(__OS2__)
WXDLLEXPORT_DATA(const wxChar *) wxUserResourceStr = wxT("TEXT");
#endif


const wxSize wxDefaultSize(wxDefaultCoord, wxDefaultCoord);
const wxPoint wxDefaultPosition(wxDefaultCoord, wxDefaultCoord);

