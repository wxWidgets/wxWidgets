/////////////////////////////////////////////////////////////////////////////
// Name:        data.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
// #pragma implementation
#endif

#include "wx/wx.h"

#define _MAXPATHLEN 500

// Used for X resources

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

wxResourceCache *wxTheResourceCache = NULL;
XrmDatabase wxResourceDatabase;

// Useful buffer, initialized in wxCommonInit
char *wxBuffer = NULL;

// Windows List
wxList wxTopLevelWindows;

// List of windows pending deletion
wxList wxPendingDelete;

// Current cursor, in order to hang on to
// cursor handle when setting the cursor globally
wxCursor *g_globalCursor = NULL;

// Don't allow event propagation during drag
bool g_blockEventsOnDrag = FALSE;

// Message Strings for Internationalization
char **wx_msg_str = (char**)NULL;

// Custom OS version, as optionally placed in wx.ini/.wxrc
// Currently this can be Win95, Windows, Win32s, WinNT.
// For some systems, you can't tell until run-time what services you
// have. See wxGetOsVersion, which uses this string if present.
char *wxOsVersion = NULL;

// For printing several pages
int wxPageNumber;
wxPrintPaperDatabase* wxThePrintPaperDatabase = NULL;

// GDI Object Lists
wxBrushList      *wxTheBrushList = NULL;
wxPenList        *wxThePenList = NULL;
wxFontList       *wxTheFontList = NULL;
wxColourDatabase *wxTheColourDatabase = NULL;
wxBitmapList   *wxTheBitmapList = NULL;
  

// X only font names
wxFontNameDirectory *wxTheFontNameDirectory;

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
wxColour *wxGREY;            // Robert Roebling
wxColour *wxRED;
wxColour *wxBLUE;
wxColour *wxGREEN;
wxColour *wxCYAN;
wxColour *wxLIGHT_GREY;

wxCursor *wxSTANDARD_CURSOR = NULL;
wxCursor *wxHOURGLASS_CURSOR = NULL;
wxCursor *wxCROSS_CURSOR = NULL;

// 'Null' objects
wxBitmap wxNullBitmap;
wxIcon   wxNullIcon;  
wxCursor wxNullCursor;
wxPen    wxNullPen;
wxBrush  wxNullBrush;
wxFont   wxNullFont;
wxColour wxNullColour;
wxPalette wxNullPalette; 

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
const char *wxEmptyString = "";
const char *wxGetTextFromUserPromptStr = "Input Text";
const char *wxMessageBoxCaptionStr = "Message";
const char *wxFileSelectorPromptStr = "Select a file";
const char *wxFileSelectorDefaultWildcardStr = "*.*";
const char *wxInternalErrorStr = "wxWindows Internal Error";
const char *wxFatalErrorStr = "wxWindows Fatal Error";

// See wx/utils.h
const char *wxFloatToStringStr = "%.2f";
const char *wxDoubleToStringStr = "%.2f";

#ifdef wx_msw
const char *wxUserResourceStr = "TEXT";
#endif


#if USE_SHARED_LIBRARY
/*
 * For wxWindows to be made into a dynamic library (e.g. Sun),
 * all IMPLEMENT_... macros must be in one place.
 * But normally, the definitions are in the appropriate places.
 */

// Hand-coded IMPLEMENT... macro for wxObject (define static data)
wxClassInfo wxObject::classwxObject("wxObject", NULL, NULL, sizeof(wxObject), NULL);
wxClassInfo *wxClassInfo::first = NULL;

#include "wx/button.h"
#include "wx/bmpbuttn.h"
IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#include "wx/checkbox.h"
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)

#include "wx/choice.h"
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)

#if USE_CLIPBOARD
#include "wx/clipbrd.h"
IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxClipboardClient, wxObject)
#endif

#if USE_COMBOBOX
#include "wx/combobox.h"
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)
#endif

#include "wx/dc.h"
#include "wx/dcmemory.h"
#include "wx/dcclient.h"
#include "wx/dcscreen.h"
IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

#if defined(wx_msw)
#include "wx/dcprint.h"
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif

#include "wx/dialog.h"
IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxWindow)

#include "wx/frame.h"
IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)

#include "wx/mdi.h"
IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxWindow)

#include "wx/cmndata.h"
IMPLEMENT_DYNAMIC_CLASS(wxColourData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxFontData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintData, wxObject)

#include "wx/colordlg.h"
#include "wx/fontdlg.h"

#if !defined(wx_msw) || USE_GENERIC_DIALOGS_IN_MSW
#include "wx/generic/colordlg.h"
#include "wx/generic/fontdlg.h"
IMPLEMENT_DYNAMIC_CLASS(wxGenericColourDialog, wxDialog)
IMPLEMENT_DYNAMIC_CLASS(wxGenericFontDialog, wxDialog)
#endif

// X defines wxColourDialog to be wxGenericColourDialog
#ifndef wx_x
IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

#include "wx/gdicmn.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/palette.h"
#include "wx/icon.h"
#include "wx/cursor.h"

IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)
IMPLEMENT_CLASS(wxColourDatabase, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxFontList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxPenList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxBrushList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapList, wxList)

/*
#if (!USE_TYPEDEFS)
IMPLEMENT_DYNAMIC_CLASS(wxPoint, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxIntPoint, wxObject)
#endif
*/

#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)
IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)
#endif

#include "wx/hash.h"
IMPLEMENT_DYNAMIC_CLASS(wxHashTable, wxObject)

#include "wx/help.h"
IMPLEMENT_DYNAMIC_CLASS(wxHelpInstance, wxClient)
IMPLEMENT_CLASS(wxHelpConnection, wxConnection)

#include "wx/list.h"
IMPLEMENT_DYNAMIC_CLASS(wxNode, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxList, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxStringList, wxList)

#if USE_PRINTING_ARCHITECTURE
#include "wx/print.h"
IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialog)
IMPLEMENT_DYNAMIC_CLASS(wxPrinterBase, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPostScriptPrinter, wxPrinterBase)
IMPLEMENT_DYNAMIC_CLASS(wxWindowsPrinter, wxPrinterBase)
IMPLEMENT_ABSTRACT_CLASS(wxPrintout, wxObject)
IMPLEMENT_CLASS(wxPreviewCanvas, wxWindow)
IMPLEMENT_CLASS(wxPreviewControlBar, wxWindow)
IMPLEMENT_CLASS(wxPreviewFrame, wxFrame)
IMPLEMENT_CLASS(wxPrintPreviewBase, wxObject)
IMPLEMENT_CLASS(wxPostScriptPrintPreview, wxPrintPreviewBase)
IMPLEMENT_CLASS(wxWindowsPrintPreview, wxPrintPreviewBase)
IMPLEMENT_CLASS(wxGenericPrintDialog, wxDialog)
IMPLEMENT_CLASS(wxGenericPrintSetupDialog, wxDialog)
#endif

#if USE_POSTSCRIPT
#include "wx/postscrp.h"
IMPLEMENT_DYNAMIC_CLASS(wxPostScriptDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPrintSetupData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPageSetupData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperType, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperDatabase, wxList)
#endif

#if USE_WX_RESOURCES
#include "wx/resource.h"
IMPLEMENT_DYNAMIC_CLASS(wxItemResource, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxResourceTable, wxHashTable)
#endif

#include "wx/event.h"
IMPLEMENT_DYNAMIC_CLASS(wxEvtHandler, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxEvent, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxCommandEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxScrollEvent, wxCommandEvent)
IMPLEMENT_DYNAMIC_CLASS(wxMouseEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxKeyEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxSizeEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxPaintEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxEraseEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxMoveEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxFocusEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxCloseEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxMenuEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxJoystickEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxDropFilesEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxActivateEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxInitDialogEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxSysColourChangedEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxIdleEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxUpdateUIEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxProcessEvent, wxEvent)

#include "wx/utils.h"
IMPLEMENT_DYNAMIC_CLASS(wxPathList, wxList)

// IMPLEMENT_DYNAMIC_CLASS(wxRect, wxObject)

#include "wx/process.h"
IMPLEMENT_DYNAMIC_CLASS(wxProcess, wxEvtHandler)

#if USE_TIMEDATE
#include "wx/date.h"
IMPLEMENT_DYNAMIC_CLASS(wxDate, wxObject)
#endif

#if USE_DOC_VIEW_ARCHITECTURE
#include "wx/docview.h"
//IMPLEMENT_ABSTRACT_CLASS(wxDocItem, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxDocument, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxView, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxDocTemplate, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxDocManager, wxEvtHandler)
IMPLEMENT_CLASS(wxDocChildFrame, wxFrame)
IMPLEMENT_CLASS(wxDocParentFrame, wxFrame)
#if USE_PRINTING_ARCHITECTURE
IMPLEMENT_DYNAMIC_CLASS(wxDocPrintout, wxPrintout)
#endif
IMPLEMENT_CLASS(wxCommand, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxCommandProcessor, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxFileHistory, wxObject)
#endif

#if USE_CONSTRAINTS
#include "wx/layout.h"
IMPLEMENT_DYNAMIC_CLASS(wxIndividualLayoutConstraint, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxLayoutConstraints, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSizer, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxRowColSizer, wxSizer)
#endif

#if USE_TOOLBAR
#include "wx/tbarbase.h"
IMPLEMENT_DYNAMIC_CLASS(wxToolBarTool, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxToolBarBase, wxControl)

#include "wx/tbarsmpl.h"
IMPLEMENT_DYNAMIC_CLASS(wxToolBarSimple, wxToolBarBase)

#ifdef wx_msw
#include "wx/tbarmsw.h"
IMPLEMENT_DYNAMIC_CLASS(wxToolBarMSW, wxToolBarBase)

#include "wx/tbar95.h"
IMPLEMENT_DYNAMIC_CLASS(wxToolBar95, wxToolBarBase)
#endif

#endif

#include "wx/statusbr.h"

IMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxWindow)

BEGIN_EVENT_TABLE(wxStatusBar, wxWindow)
  EVT_PAINT(wxStatusBar::OnPaint)
    EVT_SYS_COLOUR_CHANGED(wxStatusBar::OnSysColourChanged)
END_EVENT_TABLE()

#if USE_TIMEDATE
#include "wx/time.h"
IMPLEMENT_DYNAMIC_CLASS(wxTime, wxObject)
#endif

#if !USE_GNU_WXSTRING
#include "wx/string.h"
IMPLEMENT_DYNAMIC_CLASS(wxString, wxObject)
#endif

#ifdef wx_motif
IMPLEMENT_DYNAMIC_CLASS(wxXColormap, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxXFont, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxXCursor, wxObject)
#endif
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxPalette, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxPen, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

// This will presumably be implemented on other platforms too
#ifdef wx_msw
IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxBMPFileHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxXPMFileHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxXPMDataHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxICOFileHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxICOResourceHandler, wxBitmapHandler)
#endif

#include "wx/statbox.h"
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

#if USE_IPC
#include "wx/dde.h"
IMPLEMENT_ABSTRACT_CLASS(wxDDEObject, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxDDEServer, wxDDEObject)
IMPLEMENT_DYNAMIC_CLASS(wxDDEClient, wxDDEObject)
IMPLEMENT_CLASS(wxDDEConnection, wxObject)
#endif

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

#include "wx/listbox.h"
IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

#include "wx/menu.h"
IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxWindow)

#include "wx/stattext.h"
#include "wx/statbmp.h"
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)

#if USE_METAFILE
#include "wx/metafile.h"
IMPLEMENT_DYNAMIC_CLASS(wxMetaFile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetaFileDC, wxDC)
#endif

#include "wx/radiobox.h"
#include "wx/radiobut.h"
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)

IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)
// IMPLEMENT_DYNAMIC_CLASS(wxBitmapRadioButton, wxRadioButton)

#include "wx/scrolbar.h"
IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

#if WXWIN_COMPATIBILITY
BEGIN_EVENT_TABLE(wxScrollBar, wxControl)
  EVT_SCROLL(wxScrollBar::OnScroll)
END_EVENT_TABLE()
#endif

#include "wx/slider.h"
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxControl)

#if WXWIN_COMPATIBILITY
BEGIN_EVENT_TABLE(wxSlider, wxControl)
  EVT_SCROLL(wxSlider::OnScroll)
END_EVENT_TABLE()
#endif

#include "wx/timer.h"
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)

#include "wx/textctrl.h"
IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

#include "wx/window.h"
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxEvtHandler)

#include "wx/scrolwin.h"
IMPLEMENT_DYNAMIC_CLASS(wxScrolledWindow, wxWindow)

#include "wx/panel.h"
IMPLEMENT_DYNAMIC_CLASS(wxPanel, wxWindow)

#include "wx/msgbxdlg.h"
#include "wx/textdlg.h"
#include "wx/filedlg.h"
#include "wx/dirdlg.h"
#include "wx/choicdlg.h"

#if !defined(wx_msw) || USE_GENERIC_DIALOGS_IN_MSW
#include "wx/generic/msgdlgg.h"
IMPLEMENT_CLASS(wxGenericMessageDialog, wxDialog)
#endif

IMPLEMENT_CLASS(wxTextEntryDialog, wxDialog)
IMPLEMENT_CLASS(wxSingleChoiceDialog, wxDialog)
IMPLEMENT_CLASS(wxFileDialog, wxDialog)
IMPLEMENT_CLASS(wxDirDialog, wxDialog)

#ifdef wx_msw
IMPLEMENT_CLASS(wxMessageDialog)
#endif

#if USE_GAUGE
#ifdef wx_motif
#include "../../contrib/xmgauge/gauge.h"
#endif
#include "wx_gauge.h"
IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxControl)
#endif

#include "wx/grid.h"
IMPLEMENT_DYNAMIC_CLASS(wxGenericGrid, wxPanel)

///// Event tables (also must be in one, statically-linked file for shared libraries)

// This is the base, wxEvtHandler 'bootstrap' code which is expanded manually here
const wxEventTable *wxEvtHandler::GetEventTable() const { return &wxEvtHandler::sm_eventTable; }

const wxEventTable wxEvtHandler::sm_eventTable =
  { NULL, &wxEvtHandler::sm_eventTableEntries[0] };

const wxEventTableEntry wxEvtHandler::sm_eventTableEntries[] = { { 0, 0, 0, NULL } };

BEGIN_EVENT_TABLE(wxFrame, wxWindow)
  EVT_SIZE(wxFrame::OnSize)
  EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
  EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
  EVT_IDLE(wxFrame::OnIdle)
  EVT_CLOSE(wxFrame::OnCloseWindow)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxDialog, wxPanel)
  EVT_BUTTON(wxID_OK, wxDialog::OnOK)
  EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
  EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
  EVT_CHAR_HOOK(wxDialog::OnCharHook)
  EVT_SYS_COLOUR_CHANGED(wxDialog::OnSysColourChanged)
  EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
  EVT_CHAR(wxWindow::OnChar)
  EVT_SIZE(wxWindow::Size)
  EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxScrolledWindow, wxWindow)
  EVT_SCROLL(wxScrolledWindow::OnScroll)
  EVT_SIZE(wxScrolledWindow::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxPanel, wxWindow)
  EVT_SYS_COLOUR_CHANGED(wxPanel::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
  EVT_CHAR(wxTextCtrl::OnChar)
  EVT_DROP_FILES(wxTextCtrl::OnDropFiles)
  EVT_ERASE_BACKGROUND(wxTextCtrl::OnEraseBackground)
END_EVENT_TABLE()

#ifdef wx_msw
BEGIN_EVENT_TABLE(wxMDIParentWindow, wxFrame)
  EVT_SIZE(wxMDIParentWindow::OnSize)
  EVT_ACTIVATE(wxMDIParentWindow::OnActivate)
  EVT_SYS_COLOUR_CHANGED(wxMDIParentWindow::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxWindow)
  EVT_SCROLL(wxMDIClientWindow::OnScroll)
END_EVENT_TABLE()
#endif

BEGIN_EVENT_TABLE(wxToolBarBase, wxControl)
  EVT_SCROLL(wxToolBarBase::OnScroll)
  EVT_SIZE(wxToolBarBase::OnSize)
  EVT_IDLE(wxToolBarBase::OnIdle)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxToolBarSimple, wxToolBarBase)
  EVT_SIZE(wxToolBarSimple::OnSize)
  EVT_PAINT(wxToolBarSimple::OnPaint)
  EVT_KILL_FOCUS(wxToolBarSimple::OnKillFocus)
  EVT_MOUSE_EVENTS(wxToolBarSimple::OnMouseEvent)
END_EVENT_TABLE()

#ifdef wx_msw
BEGIN_EVENT_TABLE(wxToolBarMSW, wxToolBarBase)
  EVT_SIZE(wxToolBarMSW::OnSize)
  EVT_PAINT(wxToolBarMSW::OnPaint)
  EVT_MOUSE_EVENTS(wxToolBarMSW::OnMouseEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxToolBar95, wxToolBarBase)
  EVT_SIZE(wxToolBar95::OnSize)
  EVT_PAINT(wxToolBar95::OnPaint)
  EVT_KILL_FOCUS(wxToolBar95::OnKillFocus)
  EVT_MOUSE_EVENTS(wxToolBar95::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxToolBar95::OnSysColourChanged)
END_EVENT_TABLE()
#endif

BEGIN_EVENT_TABLE(wxGenericGrid, wxPanel)
  EVT_SIZE(wxGenericGrid::OnSize)
  EVT_PAINT(wxGenericGrid::OnPaint)
  EVT_MOUSE_EVENTS(wxGenericGrid::OnMouseEvent)
    EVT_TEXT(wxGRID_TEXT_CTRL, wxGenericGrid::OnText)
    EVT_COMMAND_SCROLL(wxGRID_HSCROLL, wxGenericGrid::OnGridScroll)
    EVT_COMMAND_SCROLL(wxGRID_VSCROLL, wxGenericGrid::OnGridScroll)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxControl, wxWindow)
  EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()

#if !defined(wx_msw) || USE_GENERIC_DIALOGS_IN_MSW
BEGIN_EVENT_TABLE(wxGenericMessageDialog, wxDialog)
  EVT_BUTTON(wxID_YES, wxGenericMessageDialog::OnYes)
  EVT_BUTTON(wxID_NO, wxGenericMessageDialog::OnNo)
  EVT_BUTTON(wxID_CANCEL, wxGenericMessageDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxGenericColourDialog, wxDialog)
  EVT_BUTTON(wxID_ADD_CUSTOM, wxGenericColourDialog::OnAddCustom)
  EVT_SLIDER(wxID_RED_SLIDER, wxGenericColourDialog::OnRedSlider)
  EVT_SLIDER(wxID_GREEN_SLIDER, wxGenericColourDialog::OnGreenSlider)
  EVT_SLIDER(wxID_BLUE_SLIDER, wxGenericColourDialog::OnBlueSlider)
  EVT_PAINT(wxGenericColourDialog::OnPaint)
  EVT_MOUSE_EVENTS(wxGenericColourDialog::OnMouseEvent)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxGenericFontDialog, wxDialog)
  EVT_CHECKBOX(wxID_FONT_UNDERLINE, wxGenericFontDialog::OnChangeFont)
  EVT_CHOICE(wxID_FONT_STYLE, wxGenericFontDialog::OnChangeFont)
  EVT_CHOICE(wxID_FONT_WEIGHT, wxGenericFontDialog::OnChangeFont)
  EVT_CHOICE(wxID_FONT_FAMILY, wxGenericFontDialog::OnChangeFont)
  EVT_CHOICE(wxID_FONT_COLOUR, wxGenericFontDialog::OnChangeFont)
  EVT_CHOICE(wxID_FONT_SIZE, wxGenericFontDialog::OnChangeFont)
  EVT_PAINT(wxGenericFontDialog::OnPaint)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxGenericPrintDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxGenericPrintDialog::OnOK)
    EVT_BUTTON(wxPRINTID_SETUP, wxGenericPrintDialog::OnSetup)
    EVT_RADIOBOX(wxPRINTID_RANGE, wxGenericPrintDialog::OnRange)
END_EVENT_TABLE()

#endif

BEGIN_EVENT_TABLE(wxTextEntryDialog, wxDialog)
  EVT_BUTTON(wxID_OK, wxTextEntryDialog::OnOK)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxSingleChoiceDialog, wxDialog)
  EVT_BUTTON(wxID_OK, wxSingleChoiceDialog::OnOK)
END_EVENT_TABLE()

#include "wx/prntbase.h"

BEGIN_EVENT_TABLE(wxPrintAbortDialog, wxDialog)
  EVT_BUTTON(wxID_CANCEL, wxPrintAbortDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxPreviewControlBar, wxWindow)
  EVT_BUTTON(wxID_PREVIEW_CLOSE,     wxPreviewControlBar::OnClose)
  EVT_BUTTON(wxID_PREVIEW_PRINT,     wxPreviewControlBar::OnPrint)
  EVT_BUTTON(wxID_PREVIEW_PREVIOUS,   wxPreviewControlBar::OnPrevious)
  EVT_BUTTON(wxID_PREVIEW_NEXT,     wxPreviewControlBar::OnNext)
  EVT_CHOICE(wxID_PREVIEW_ZOOM,     wxPreviewControlBar::OnZoom)
END_EVENT_TABLE()

#endif


const wxSize wxDefaultSize(-1, -1);
const wxPoint wxDefaultPosition(-1, -1);
