/////////////////////////////////////////////////////////////////////////////
// Name:        data.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
// #pragma implementation
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "wx/accel.h"
#include "wx/dcps.h"

#define _MAXPATHLEN 500

/* Used for X resources */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

wxResourceCache *wxTheResourceCache = (wxResourceCache *) NULL;
XrmDatabase wxResourceDatabase;

/* Useful buffer, initialized in wxCommonInit */
char *wxBuffer = (char *) NULL;

/* Windows List */
wxWindowList wxTopLevelWindows;

/* List of windows pending deletion */
wxList wxPendingDelete;

#if wxUSE_THREADS
/* List of events pending processing */
wxList *wxPendingEvents = NULL;
wxCriticalSection *wxPendingEventsLocker = NULL;
#endif

/* Current cursor, in order to hang on to
 * cursor handle when setting the cursor globally */
wxCursor *g_globalCursor = (wxCursor *) NULL;

/* Don't allow event propagation during drag */
bool g_blockEventsOnDrag = FALSE;

/* Don't allow mouse event propagation during scroll */
bool g_blockEventsOnScroll = FALSE;

/* Message Strings for Internationalization */
char **wx_msg_str = (char**)NULL;

/* Custom OS version, as optionally placed in wx.ini/.wxrc
 * Currently this can be Win95, Windows, Win32s, WinNT.
 * For some systems, you can't tell until run-time what services you
 * have. See wxGetOsVersion, which uses this string if present. */
char *wxOsVersion = (char *) NULL;

/* For printing several pages */
int wxPageNumber;

// Now in prntbase.cpp
// wxPrintPaperDatabase* wxThePrintPaperDatabase = (wxPrintPaperDatabase *) NULL;

/* GDI Object Lists */
wxBrushList      *wxTheBrushList = (wxBrushList *) NULL;
wxPenList        *wxThePenList = (wxPenList *) NULL;
wxFontList       *wxTheFontList = (wxFontList *) NULL;
wxColourDatabase *wxTheColourDatabase = (wxColourDatabase *) NULL;
wxBitmapList     *wxTheBitmapList = (wxBitmapList *) NULL;

/* X only font names */
/*
wxFontNameDirectory *wxTheFontNameDirectory;
*/

/* Stock objects */
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
wxColour *wxGREY;
wxColour *wxRED;
wxColour *wxBLUE;
wxColour *wxGREEN;
wxColour *wxCYAN;
wxColour *wxLIGHT_GREY;

wxCursor *wxSTANDARD_CURSOR = (wxCursor *) NULL;
wxCursor *wxHOURGLASS_CURSOR = (wxCursor *) NULL;
wxCursor *wxCROSS_CURSOR = (wxCursor *) NULL;

/* 'Null' objects */
wxAcceleratorTable   wxNullAcceleratorTable;
wxBitmap   wxNullBitmap;
wxIcon     wxNullIcon;
wxCursor   wxNullCursor;
wxPen      wxNullPen;
wxBrush    wxNullBrush;
wxFont     wxNullFont;
wxColour   wxNullColour;
wxPalette  wxNullPalette;

/* Default window names */
const wxChar *wxButtonNameStr = _T("button");
const wxChar *wxCanvasNameStr = _T("canvas");
const wxChar *wxCheckBoxNameStr = _T("check");
const wxChar *wxChoiceNameStr = _T("choice");
const wxChar *wxComboBoxNameStr = _T("comboBox");
const wxChar *wxDialogNameStr = _T("dialog");
const wxChar *wxFrameNameStr = _T("frame");
const wxChar *wxGaugeNameStr = _T("gauge");
const wxChar *wxStaticBoxNameStr = _T("groupBox");
const wxChar *wxListBoxNameStr = _T("listBox");
const wxChar *wxStaticTextNameStr = _T("message");
const wxChar *wxStaticBitmapNameStr = _T("message");
const wxChar *wxMultiTextNameStr = _T("multitext");
const wxChar *wxPanelNameStr = _T("panel");
const wxChar *wxRadioBoxNameStr = _T("radioBox");
const wxChar *wxRadioButtonNameStr = _T("radioButton");
const wxChar *wxBitmapRadioButtonNameStr = _T("radioButton");
const wxChar *wxScrollBarNameStr = _T("scrollBar");
const wxChar *wxSliderNameStr = _T("slider");
const wxChar *wxStaticNameStr = _T("static");
const wxChar *wxTextCtrlWindowNameStr = _T("textWindow");
const wxChar *wxTextCtrlNameStr = _T("text");
const wxChar *wxVirtListBoxNameStr = _T("virtListBox");
const wxChar *wxButtonBarNameStr = _T("buttonbar");
const wxChar *wxEnhDialogNameStr = _T("Shell");
const wxChar *wxToolBarNameStr = _T("toolbar");
const wxChar *wxStatusLineNameStr = _T("status_line");
const wxChar *wxEmptyString = _T("");
const wxChar *wxGetTextFromUserPromptStr = _T("Input Text");
const wxChar *wxMessageBoxCaptionStr = _T("Message");
const wxChar *wxFileSelectorPromptStr = _T("Select a file");
const wxChar *wxFileSelectorDefaultWildcardStr = _T("*.*");
const wxChar *wxInternalErrorStr = _T("wxWindows Internal Error");
const wxChar *wxFatalErrorStr = _T("wxWindows Fatal Error");

/* See wx/utils.h */
const wxChar *wxFloatToStringStr = _T("%.2f");
const wxChar *wxDoubleToStringStr = _T("%.2f");

/* Dafaults for wxWindow etc. */
const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);
