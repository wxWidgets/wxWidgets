/////////////////////////////////////////////////////////////////////////////
// Name:        _tipdlg.i
// Purpose:     SWIG defs for wxTip classes and such
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/tipdlg.h>
%}

//---------------------------------------------------------------------------


// wxTipProvider - a class which is used by wxTipDialog to get the text of the
// tips
class wxTipProvider
{
public:
    // wxTipProvider(size_t currentTip);  **** Abstract base class
    ~wxTipProvider();

    // get the current tip and update the internal state to return the next tip
    // when called for the next time
    virtual wxString GetTip();

    // get the current tip "index" (or whatever allows the tip provider to know
    // from where to start the next time)
    size_t GetCurrentTip();

    // Allows any user-derived class to optionally override this function to 
    // modify the tip as soon as it is read. If return wxEmptyString, then 
    // the tip is skipped, and the next one is read.
    virtual wxString PreprocessTip(const wxString& tip);

    %property(CurrentTip, GetCurrentTip, doc="See `GetCurrentTip`");
    %property(Tip, GetTip, doc="See `GetTip`");
};


// The C++ version of wxPyTipProvider
%{
class wxPyTipProvider : public wxTipProvider {
public:
    wxPyTipProvider(size_t currentTip)
        : wxTipProvider(currentTip) {}

    DEC_PYCALLBACK_STRING__pure(GetTip);
    DEC_PYCALLBACK_STRING_STRING(PreprocessTip);
    PYPRIVATE;
};

IMP_PYCALLBACK_STRING__pure( wxPyTipProvider, wxTipProvider, GetTip);
IMP_PYCALLBACK_STRING_STRING(wxPyTipProvider, wxTipProvider, PreprocessTip);
%}


// Now let SWIG know about it
class wxPyTipProvider : public wxTipProvider {
public:
    %pythonAppend wxPyTipProvider "self._setCallbackInfo(self, PyTipProvider)"
    wxPyTipProvider(size_t currentTip);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
};



// A dialog which shows a "tip" - a short and helpful messages describing to
// the user some program characteristic. Many programs show the tips at
// startup, so the dialog has "Show tips on startup" checkbox which allows to
// the user to disable this (however, it's the program which should show, or
// not, the dialog on startup depending on its value, not this class).
//
// The function returns True if this checkbox is checked, False otherwise.
MustHaveApp(wxShowTip);
bool wxShowTip(wxWindow *parent, wxTipProvider *tipProvider, bool showAtStartup = true);

// a function which returns an implementation of wxTipProvider using the
// specified text file as the source of tips (each line is a tip).
%newobject wxCreateFileTipProvider;
MustHaveApp(wxCreateFileTipProvider);
wxTipProvider* wxCreateFileTipProvider(const wxString& filename, size_t currentTip);




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
