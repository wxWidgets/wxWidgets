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
wxList wxTopLevelWindows;

/* List of windows pending deletion */
wxList wxPendingDelete;

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
wxPrintPaperDatabase* wxThePrintPaperDatabase = (wxPrintPaperDatabase *) NULL;

/* GDI Object Lists */
wxBrushList      *wxTheBrushList = (wxBrushList *) NULL;
wxPenList        *wxThePenList = (wxPenList *) NULL;
wxFontList       *wxTheFontList = (wxFontList *) NULL;
wxColourDatabase *wxTheColourDatabase = (wxColourDatabase *) NULL;
wxBitmapList     *wxTheBitmapList = (wxBitmapList *) NULL;
  
/* X only font names */
wxFontNameDirectory *wxTheFontNameDirectory;

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
wxRegion   wxNullRegion; 

/* Default window names */
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
const char *wxEmptyString = "";
const char *wxGetTextFromUserPromptStr = "Input Text";
const char *wxMessageBoxCaptionStr = "Message";
const char *wxFileSelectorPromptStr = "Select a file";
const char *wxFileSelectorDefaultWildcardStr = "*.*";
const char *wxInternalErrorStr = "wxWindows Internal Error";
const char *wxFatalErrorStr = "wxWindows Fatal Error";

/* See wx/utils.h */
const char *wxFloatToStringStr = "%.2f";
const char *wxDoubleToStringStr = "%.2f";

/* Dafaults for wxWindow etc. */
const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);
