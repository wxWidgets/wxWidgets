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
#include "wx/icon.h"

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
wxCursor g_globalCursor;

/* Don't allow event propagation during drag */
bool g_blockEventsOnDrag = FALSE;

/* Don't allow mouse event propagation during scroll */
bool g_blockEventsOnScroll = FALSE;

/* TRUE when the message queue is empty. this gets set to
   FALSE by all event callbacks before anything else is done */
bool g_isIdle = FALSE;

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
#if wxUSE_ACCEL
    wxAcceleratorTable   wxNullAcceleratorTable;
#endif // wxUSE_ACCEL

wxBitmap   wxNullBitmap;
wxIcon     wxNullIcon;
wxCursor   wxNullCursor;
wxPen      wxNullPen;
wxBrush    wxNullBrush;
wxFont     wxNullFont;
wxColour   wxNullColour;
wxPalette  wxNullPalette;

/* Default window names */
const wxChar *wxButtonNameStr = T("button");
const wxChar *wxCanvasNameStr = T("canvas");
const wxChar *wxCheckBoxNameStr = T("check");
const wxChar *wxChoiceNameStr = T("choice");
const wxChar *wxComboBoxNameStr = T("comboBox");
const wxChar *wxDialogNameStr = T("dialog");
const wxChar *wxFrameNameStr = T("frame");
const wxChar *wxGaugeNameStr = T("gauge");
const wxChar *wxStaticBoxNameStr = T("groupBox");
const wxChar *wxListBoxNameStr = T("listBox");
const wxChar *wxStaticTextNameStr = T("message");
const wxChar *wxStaticBitmapNameStr = T("message");
const wxChar *wxMultiTextNameStr = T("multitext");
const wxChar *wxPanelNameStr = T("panel");
const wxChar *wxRadioBoxNameStr = T("radioBox");
const wxChar *wxRadioButtonNameStr = T("radioButton");
const wxChar *wxBitmapRadioButtonNameStr = T("radioButton");
const wxChar *wxScrollBarNameStr = T("scrollBar");
const wxChar *wxSliderNameStr = T("slider");
const wxChar *wxStaticNameStr = T("static");
const wxChar *wxTextCtrlWindowNameStr = T("textWindow");
const wxChar *wxTextCtrlNameStr = T("text");
const wxChar *wxVirtListBoxNameStr = T("virtListBox");
const wxChar *wxButtonBarNameStr = T("buttonbar");
const wxChar *wxEnhDialogNameStr = T("Shell");
const wxChar *wxToolBarNameStr = T("toolbar");
const wxChar *wxStatusLineNameStr = T("status_line");
const wxChar *wxGetTextFromUserPromptStr = T("Input Text");
const wxChar *wxMessageBoxCaptionStr = T("Message");
const wxChar *wxFileSelectorPromptStr = T("Select a file");
const wxChar *wxFileSelectorDefaultWildcardStr = T("*");
const wxChar *wxInternalErrorStr = T("wxWindows Internal Error");
const wxChar *wxFatalErrorStr = T("wxWindows Fatal Error");
const wxChar *wxDirDialogNameStr = T("wxDirCtrl");
const wxChar *wxDirDialogDefaultFolderStr = T("/");

/* See wx/utils.h */
const wxChar *wxFloatToStringStr = T("%.2f");
const wxChar *wxDoubleToStringStr = T("%.2f");

/* Dafaults for wxWindow etc. */
const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);
