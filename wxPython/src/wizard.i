/////////////////////////////////////////////////////////////////////////////
// Name:        wizard.i
// Purpose:     Classes for wxWizard and etc.
//
// Author:      Robin Dunn
//
// Created:     16-Aug-2002
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module wizard

%{
#include "wxPython.h"
#include <wx/wizard.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import windows.i
%import frames.i
%import misc.i
%import controls.i


//----------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
%}
//----------------------------------------------------------------------

enum {
    wxWIZARD_EX_HELPBUTTON,

    wxEVT_WIZARD_PAGE_CHANGED,
    wxEVT_WIZARD_PAGE_CHANGING,
    wxEVT_WIZARD_CANCEL,
    wxEVT_WIZARD_HELP,
    wxEVT_WIZARD_FINISHED
};


%pragma(python) code = "
# wizard events
def EVT_WIZARD_PAGE_CHANGED(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_PAGE_CHANGED, func)

def EVT_WIZARD_PAGE_CHANGING(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_PAGE_CHANGING, func)

def EVT_WIZARD_CANCEL(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_CANCEL, func)

def EVT_WIZARD_HELP(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_HELP, func)

def EVT_WIZARD_FINISHED(win, id, func):
    win.Connect(id, -1, wxEVT_WIZARD_FINISHED, func)

"

//----------------------------------------------------------------------

class wxWizardEvent : public wxNotifyEvent
{
public:
    wxWizardEvent(wxEventType type = wxEVT_NULL,
                  int id = -1,
                  bool direction = TRUE,
                  wxWizardPage* page = NULL);

    // for EVT_WIZARD_PAGE_CHANGING, return TRUE if we're going forward or
    // FALSE otherwise and for EVT_WIZARD_PAGE_CHANGED return TRUE if we came
    // from the previous page and FALSE if we returned from the next one
    // (this function doesn't make sense for CANCEL events)
    bool GetDirection() const { return m_direction; }

    wxWizardPage*   GetPage() const { return m_page; }
};


//----------------------------------------------------------------------

// wxWizardPage is one of the wizards screen: it must know what are the
// following and preceding pages (which may be NULL for the first/last page).
//
// Other than GetNext/Prev() functions, wxWizardPage is just a panel and may be
// used as such (i.e. controls may be placed directly on it &c).
class wxWizardPage : public wxPanel
{
public:
//     // ctor accepts an optional bitmap which will be used for this page instead
//     // of the default one for this wizard (should be of the same size). Notice
//     // that no other parameters are needed because the wizard will resize and
//     // reposition the page anyhow
//     wxWizardPage(wxWizard *parent,
//                  const wxBitmap& bitmap = wxNullBitmap,
//                  const char* resource = NULL);
//     %name(wxPreWizardPage)wxWizardPage();

    %addmethods {
        bool Create(wxWizard *parent,
                    const wxBitmap& bitmap = wxNullBitmap,
                    const wxString& resource = wxPyEmptyString) {
            wxChar* res = NULL;
            if (resource.Length())
                res = (wxChar*)resource.c_str();
            return self->Create(parent, bitmap, res);
        }
    }


    // these functions are used by the wizard to show another page when the
    // user chooses "Back" or "Next" button
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;

    // default GetBitmap() will just return m_bitmap which is ok in 99% of
    // cases - override this method if you want to create the bitmap to be used
    // dynamically or to do something even more fancy. It's ok to return
    // wxNullBitmap from here - the default one will be used then.
    virtual wxBitmap GetBitmap() const;
};



%{  // C++ Version of a Python aware class
class wxPyWizardPage : public wxWizardPage {
    DECLARE_ABSTRACT_CLASS(wxPyWizardPage);
public:
    wxPyWizardPage() : wxWizardPage() {}
    wxPyWizardPage(wxWizard *parent,
                   const wxBitmap& bitmap = wxNullBitmap,
                   const wxChar* resource = NULL)
        : wxWizardPage(parent, bitmap, resource) {}

    DEC_PYCALLBACK_WIZPG__pure(GetPrev);
    DEC_PYCALLBACK_WIZPG__pure(GetNext);
    DEC_PYCALLBACK_BITMAP__pure(GetBitmap);

    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    PYPRIVATE;
};


IMPLEMENT_ABSTRACT_CLASS(wxPyWizardPage, wxWizardPage);

IMP_PYCALLBACK_WIZPG__pure(wxPyWizardPage, wxWizardPage, GetPrev);
IMP_PYCALLBACK_WIZPG__pure(wxPyWizardPage, wxWizardPage, GetNext);
IMP_PYCALLBACK_BITMAP__pure(wxPyWizardPage, wxWizardPage, GetBitmap);

IMP_PYCALLBACK_VOID_INT4(wxPyWizardPage, wxWizardPage, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyWizardPage, wxWizardPage, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyWizardPage, wxWizardPage, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyWizardPage, wxWizardPage, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWizardPage, wxWizardPage, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWizardPage, wxWizardPage, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWizardPage, wxWizardPage, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyWizardPage, wxWizardPage, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyWizardPage, wxWizardPage, DoGetBestSize);

IMP_PYCALLBACK__(wxPyWizardPage, wxWizardPage, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyWizardPage, wxWizardPage, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyWizardPage, wxWizardPage, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyWizardPage, wxWizardPage, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyWizardPage, wxWizardPage, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyWizardPage, wxWizardPage, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyWizardPage, wxWizardPage, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyWizardPage, wxWizardPage, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyWizardPage, wxWizardPage, RemoveChild);

%}



class wxPyWizardPage : public wxWizardPage {
public:
    // ctor accepts an optional bitmap which will be used for this page instead
    // of the default one for this wizard (should be of the same size). Notice
    // that no other parameters are needed because the wizard will resize and
    // reposition the page anyhow
    %addmethods {
        wxPyWizardPage(wxWizard *parent,
                       const wxBitmap* bitmap = &wxNullBitmap,
                       const wxString* resource = &wxPyEmptyString) {
            wxChar* res = NULL;
            if (resource->Length())
                res = (wxChar*)resource->c_str();
            return new wxPyWizardPage(parent, *bitmap, res);
        }
    }

    %name(wxPrePyWizardPage)wxPyWizardPage();

    %addmethods {
        bool Create(wxWizard *parent,
                    const wxBitmap& bitmap = wxNullBitmap,
                    const wxString& resource = wxPyEmptyString) {
            wxChar* res = NULL;
            if (resource.Length())
                res = (wxChar*)resource.c_str();
            return self->Create(parent, bitmap, res);
        }
    }

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyWizardPage)"

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPrePyWizardPage:val._setOORInfo(val)"

    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    void base_DoGetSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const;

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);
};

//----------------------------------------------------------------------


// wxWizardPageSimple just returns the pointers given to the ctor and is useful
// to create a simple wizard where the order of pages never changes.
//
// OTOH, it is also possible to dynamicly decide which page to return (i.e.
// depending on the user's choices) as the wizard sample shows - in order to do
// this, you must derive from wxWizardPage directly.
class wxWizardPageSimple : public wxWizardPage
{
public:
    // ctor takes the previous and next pages
    wxWizardPageSimple(wxWizard *parent,
                       wxWizardPage *prev = NULL,
                       wxWizardPage *next = NULL,
                       const wxBitmap& bitmap = wxNullBitmap,
                       const wxChar* resource = NULL);
    %name(wxPreWizardPageSimple)wxWizardPageSimple();

    bool Create(wxWizard *parent = NULL,
                wxWizardPage *prev = NULL,
                wxWizardPage *next = NULL,
                const wxBitmap& bitmap = wxNullBitmap,
                const wxChar* resource = NULL);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreWizardPageSimple:val._setOORInfo(val)"

    // the pointers may be also set later - but before starting the wizard
    void SetPrev(wxWizardPage *prev);
    void SetNext(wxWizardPage *next);

    // a convenience function to make the pages follow each other
    static void Chain(wxWizardPageSimple *first, wxWizardPageSimple *second);
};


//----------------------------------------------------------------------

class  wxWizard : public wxDialog
{
public:
    // ctor
    wxWizard(wxWindow *parent,
             int id = -1,
             const wxString& title = wxEmptyString,
             const wxBitmap& bitmap = wxNullBitmap,
             const wxPoint& pos = wxDefaultPosition);
    %name(wxPreWizard)wxWizard();

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreWizard:val._setOORInfo(val)"

    bool Create(wxWindow *parent,
             int id = -1,
             const wxString& title = wxEmptyString,
             const wxBitmap& bitmap = wxNullBitmap,
             const wxPoint& pos = wxDefaultPosition);

    void Init();


    // executes the wizard starting from the given page, returns TRUE if it was
    // successfully finished, FALSE if user cancelled it
    virtual bool RunWizard(wxWizardPage *firstPage);

    // get the current page (NULL if RunWizard() isn't running)
    virtual wxWizardPage *GetCurrentPage() const;

    // set the min size which should be available for the pages: a
    // wizard will take into account the size of the bitmap (if any)
    // itself and will never be less than some predefined fixed size
    virtual void SetPageSize(const wxSize& size);

    // get the size available for the page: the wizards are not resizeable, so
    // this size doesn't change
    virtual wxSize GetPageSize() const;

    // set the best size for the wizard, i.e. make it big enough to contain all
    // of the pages starting from the given one
    //
    // this function may be called several times and possible with different
    // pages in which case it will only increase the page size if needed (this
    // may be useful if not all pages are accessible from the first one by
    // default)
    virtual void FitToPage(const wxWizardPage *firstPage);

    // is the wizard running?
    bool IsRunning() const { return m_page != NULL; }

    // show the prev/next page, but call TransferDataFromWindow on the current
    // page first and return FALSE without changing the page if
    // TransferDataFromWindow() returns FALSE - otherwise, returns TRUE
    bool ShowPage(wxWizardPage *page, bool goingForward = TRUE);

    bool HasNextPage(wxWizardPage* page);
    bool HasPrevPage(wxWizardPage* page);
};


//----------------------------------------------------------------------

%init %{
    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();
%}

//----------------------------------------------------------------------
// This file gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_wizardextras.py";

//---------------------------------------------------------------------------
