/////////////////////////////////////////////////////////////////////////////
// Name:        misc2.i
// Purpose:     Definitions of miscelaneous functions and classes that need
//              to know about wxWindow.  (So they can't be in misc.i or an
//              import loop will happen.)
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module misc2

%{
#include "helpers.h"
#include <wx/resource.h>
#include <wx/tooltip.h>
#include <wx/caret.h>
#include <wx/fontenum.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import windows.i
%import misc.i
%import gdi.i
%import events.i

//---------------------------------------------------------------------------
// Dialog Functions

wxString wxFileSelector(char* message,
                        char* default_path = NULL,
                        char* default_filename = NULL,
                        char* default_extension = NULL,
                        char* wildcard = "*.*",
                        int flags = 0,
                        wxWindow *parent = NULL,
                        int x = -1, int y = -1);

wxString wxGetTextFromUser(const wxString& message,
                           const wxString& caption = wxPyEmptyStr,
                           const wxString& default_value = wxPyEmptyStr,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE);


// TODO: Need to custom wrap this one...
// int wxGetMultipleChoice(char* message, char* caption,
//                         int LCOUNT, char** choices,
//                         int nsel, int *selection,
//                         wxWindow *parent = NULL, int x = -1, int y = -1,
//                         bool centre = TRUE, int width=150, int height=200);


wxString wxGetSingleChoice(const wxString& message, const wxString& caption,
                           int LCOUNT, wxString* choices,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);

int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int LCOUNT, wxString* choices,
                           wxWindow *parent = NULL,
                           int x = -1, int y = -1,
                           bool centre = TRUE,
                           int width=150, int height=200);


int wxMessageBox(const wxString& message,
                 const wxString& caption = wxPyEmptyStr,
                 int style = wxOK | wxCENTRE,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

long wxGetNumberFromUser(const wxString& message,
                         const wxString& prompt,
                         const wxString& caption,
                         long value,
                         long min = 0, long max = 100,
                         wxWindow *parent = NULL,
                         const wxPoint& pos = wxPyDefaultPosition);

//---------------------------------------------------------------------------
// GDI Functions

bool wxColourDisplay();
int wxDisplayDepth();
void wxSetCursor(wxCursor& cursor);

//----------------------------------------------------------------------
// Miscellaneous functions

wxWindow * wxFindWindowByLabel(const wxString& label, wxWindow *parent=NULL);
wxWindow * wxFindWindowByName(const wxString& name, wxWindow *parent=NULL);

void wxBeginBusyCursor(wxCursor *cursor = wxHOURGLASS_CURSOR);
wxWindow * wxGetActiveWindow();


//---------------------------------------------------------------------------
// Resource System

bool wxResourceAddIdentifier(char *name, int value);
void wxResourceClear(void);
wxBitmap  wxResourceCreateBitmap(char *resource);
wxIcon  wxResourceCreateIcon(char *resource);
wxMenuBar * wxResourceCreateMenuBar(char *resource);
int wxResourceGetIdentifier(char *name);
bool wxResourceParseData(char *resource, wxResourceTable *table = NULL);
bool wxResourceParseFile(char *filename, wxResourceTable *table = NULL);
bool wxResourceParseString(char *resource, wxResourceTable *table = NULL);

//---------------------------------------------------------------------------
// System Settings

enum {
    wxSYS_WHITE_BRUSH,
    wxSYS_LTGRAY_BRUSH,
    wxSYS_GRAY_BRUSH,
    wxSYS_DKGRAY_BRUSH,
    wxSYS_BLACK_BRUSH,
    wxSYS_NULL_BRUSH,
    wxSYS_HOLLOW_BRUSH,
    wxSYS_WHITE_PEN,
    wxSYS_BLACK_PEN,
    wxSYS_NULL_PEN,
    wxSYS_OEM_FIXED_FONT,
    wxSYS_ANSI_FIXED_FONT,
    wxSYS_ANSI_VAR_FONT,
    wxSYS_SYSTEM_FONT,
    wxSYS_DEVICE_DEFAULT_FONT,
    wxSYS_DEFAULT_PALETTE,
    wxSYS_SYSTEM_FIXED_FONT,
    wxSYS_DEFAULT_GUI_FONT,

    wxSYS_COLOUR_SCROLLBAR,
    wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_ACTIVECAPTION,
    wxSYS_COLOUR_INACTIVECAPTION,
    wxSYS_COLOUR_MENU,
    wxSYS_COLOUR_WINDOW,
    wxSYS_COLOUR_WINDOWFRAME,
    wxSYS_COLOUR_MENUTEXT,
    wxSYS_COLOUR_WINDOWTEXT,
    wxSYS_COLOUR_CAPTIONTEXT,
    wxSYS_COLOUR_ACTIVEBORDER,
    wxSYS_COLOUR_INACTIVEBORDER,
    wxSYS_COLOUR_APPWORKSPACE,
    wxSYS_COLOUR_HIGHLIGHT,
    wxSYS_COLOUR_HIGHLIGHTTEXT,
    wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_BTNTEXT,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_BTNHIGHLIGHT,

    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_INFOBK,

    wxSYS_COLOUR_DESKTOP,
    wxSYS_COLOUR_3DFACE,
    wxSYS_COLOUR_3DSHADOW,
    wxSYS_COLOUR_3DHIGHLIGHT,
    wxSYS_COLOUR_3DHILIGHT,
    wxSYS_COLOUR_BTNHILIGHT,

    wxSYS_MOUSE_BUTTONS,
    wxSYS_BORDER_X,
    wxSYS_BORDER_Y,
    wxSYS_CURSOR_X,
    wxSYS_CURSOR_Y,
    wxSYS_DCLICK_X,
    wxSYS_DCLICK_Y,
    wxSYS_DRAG_X,
    wxSYS_DRAG_Y,
    wxSYS_EDGE_X,
    wxSYS_EDGE_Y,
    wxSYS_HSCROLL_ARROW_X,
    wxSYS_HSCROLL_ARROW_Y,
    wxSYS_HTHUMB_X,
    wxSYS_ICON_X,
    wxSYS_ICON_Y,
    wxSYS_ICONSPACING_X,
    wxSYS_ICONSPACING_Y,
    wxSYS_WINDOWMIN_X,
    wxSYS_WINDOWMIN_Y,
    wxSYS_SCREEN_X,
    wxSYS_SCREEN_Y,
    wxSYS_FRAMESIZE_X,
    wxSYS_FRAMESIZE_Y,
    wxSYS_SMALLICON_X,
    wxSYS_SMALLICON_Y,
    wxSYS_HSCROLL_Y,
    wxSYS_VSCROLL_X,
    wxSYS_VSCROLL_ARROW_X,
    wxSYS_VSCROLL_ARROW_Y,
    wxSYS_VTHUMB_Y,
    wxSYS_CAPTION_Y,
    wxSYS_MENU_Y,
    wxSYS_NETWORK_PRESENT,
    wxSYS_PENWINDOWS_PRESENT,
    wxSYS_SHOW_SOUNDS,
    wxSYS_SWAP_BUTTONS,
};



%inline %{

    wxColour wxSystemSettings_GetSystemColour(int index) {
        return wxSystemSettings::GetSystemColour(index);
    }

    wxFont wxSystemSettings_GetSystemFont(int index) {
        return wxSystemSettings::GetSystemFont(index);
    }

    int wxSystemSettings_GetSystemMetric(int index) {
        return wxSystemSettings::GetSystemMetric(index);
    }
%}

//---------------------------------------------------------------------------
// wxToolTip

class wxToolTip {
public:
    wxToolTip(const wxString &tip);

    void SetTip(const wxString& tip);
    wxString GetTip();
    // *** Not in the "public" interface void SetWindow(wxWindow *win);
    wxWindow *GetWindow();
};


%inline %{
    void wxToolTip_Enable(bool flag) {
        wxToolTip::Enable(flag);
    }

    void wxToolTip_SetDelay(long milliseconds) {
        wxToolTip::SetDelay(milliseconds);
    }
%}

//----------------------------------------------------------------------

class wxCaret {
public:
    wxCaret(wxWindow* window, const wxSize& size);
    ~wxCaret();

    bool IsOk();
    bool IsVisible();
    %name(GetPositionTuple)void GetPosition(int *OUTPUT, int *OUTPUT);
    wxPoint GetPosition();
    %name(GetSizeTuple)void GetSize(int *OUTPUT, int *OUTPUT);
    wxSize GetSize();
    wxWindow *GetWindow();
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& pt);
    %name(SetSizeWH) void SetSize(int width, int height);
    void SetSize(const wxSize& size);
    void Show(int show = TRUE);
    void Hide();
};

%inline %{
    int wxCaret_GetBlinkTime() {
        return wxCaret::GetBlinkTime();
    }

    void wxCaret_SetBlinkTime(int milliseconds) {
        wxCaret::SetBlinkTime(milliseconds);
    }
%}

//----------------------------------------------------------------------

%{
class wxPyFontEnumerator : public wxFontEnumerator {
public:
    wxPyFontEnumerator() {}
    ~wxPyFontEnumerator() {}

    DEC_PYCALLBACK_BOOL_STRING(OnFacename);
    DEC_PYCALLBACK_BOOL_STRINGSTRING(OnFontEncoding);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_STRING(wxPyFontEnumerator, wxFontEnumerator, OnFacename);
IMP_PYCALLBACK_BOOL_STRINGSTRING(wxPyFontEnumerator, wxFontEnumerator, OnFontEncoding);

%}

%name(wxFontEnumerator) class wxPyFontEnumerator {
public:
    wxPyFontEnumerator();
    ~wxPyFontEnumerator();
    void _setSelf(PyObject* self);
    %pragma(python) addtomethod = "__init__:self._setSelf(self)"

    bool EnumerateFacenames(
        wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
        bool fixedWidthOnly = FALSE);
    bool EnumerateEncodings(const char* facename = "");

    //wxArrayString* GetEncodings();
    //wxArrayString* GetFacenames();
    %addmethods {
        PyObject* GetEncodings() {
            wxArrayString* arr = self->GetEncodings();
            PyObject* list = PyList_New(0);
            for (size_t x=0; x<arr->GetCount(); x++)
                PyList_Append(list, PyString_FromString((*arr)[x]));
            return list;
        }

        PyObject* GetFacenames() {
            wxArrayString* arr = self->GetFacenames();
            PyObject* list = PyList_New(0);
            for (size_t x=0; x<arr->GetCount(); x++)
                PyList_Append(list, PyString_FromString((*arr)[x]));
            return list;
        }
    }
};

//----------------------------------------------------------------------

class  wxBusyCursor {
public:
    wxBusyCursor(wxCursor* cursor = wxHOURGLASS_CURSOR);
    ~wxBusyCursor();
};

//----------------------------------------------------------------------

void wxPostEvent(wxEvtHandler *dest, wxEvent& event);
void wxWakeUpIdle();

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

