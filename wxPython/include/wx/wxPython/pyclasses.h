////////////////////////////////////////////////////////////////////////////
// Name:        pyclasses.h
// Purpose:     wxPython python-aware classes for redirecting virtual method
//              calls for classes located in the core but that need to be
//              visible to multiple modules
//
// Author:      Robin Dunn
//
// Created:     11-Oct-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __pyclasses_h__
#define __pyclasses_h__



//---------------------------------------------------------------------------

class wxPySizer : public wxSizer {
    DECLARE_DYNAMIC_CLASS(wxPySizer);
public:
    wxPySizer() : wxSizer() {};

    DEC_PYCALLBACK___pure(RecalcSizes);
    DEC_PYCALLBACK_wxSize__pure(CalcMin);
    PYPRIVATE;
};


//---------------------------------------------------------------------------

class wxPyValidator : public wxValidator {
    DECLARE_DYNAMIC_CLASS(wxPyValidator);
public:
    wxPyValidator() {
    }

    ~wxPyValidator() {
    }

    wxObject* Clone() const {
        wxPyValidator* ptr = NULL;
        wxPyValidator* self = (wxPyValidator*)this;

        wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(self->m_myInst, "Clone")) {
            PyObject* ro;
            ro = wxPyCBH_callCallbackObj(self->m_myInst, Py_BuildValue("()"));
            if (ro) {
                wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxPyValidator"));
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads();

        // This is very dangerous!!! But is the only way I could find
        // to squash a memory leak.  Currently it is okay, but if the
        // validator architecture in wxWindows ever changes, problems
        // could arise.
        delete self;
        return ptr;
    }


    DEC_PYCALLBACK_BOOL_WXWIN(Validate);
    DEC_PYCALLBACK_BOOL_(TransferToWindow);
    DEC_PYCALLBACK_BOOL_(TransferFromWindow);

    PYPRIVATE;
};


//---------------------------------------------------------------------------


class wxPyPen : public wxPen {
public:
    wxPyPen(wxColour& colour, int width=1, int style=wxSOLID)
        : wxPen(colour, width, style)
        { m_dash = NULL; }
    ~wxPyPen();

    void SetDashes(int nb_dashes, const wxDash *dash);

private:
    wxDash* m_dash;
};


//---------------------------------------------------------------------------

class wxPyTimer : public wxTimer
{
public:
    wxPyTimer(wxEvtHandler *owner=NULL, int id = -1)
        : wxTimer(owner, id)
    {
        if (owner == NULL) SetOwner(this);
    }

    DEC_PYCALLBACK__(Notify);
    PYPRIVATE;
};


//---------------------------------------------------------------------------


class wxPyProcess : public wxProcess {
public:
    wxPyProcess(wxEvtHandler *parent = NULL, int id = -1)
        : wxProcess(parent, id)
        {}

    DEC_PYCALLBACK_VOID_INTINT(OnTerminate);

    PYPRIVATE;
};


//---------------------------------------------------------------------------

class wxPyVScrolledWindow;
class wxPyVListBox;
class wxPyHtmlListBox;
class wxPyPanel;
class wxPyPopupTransientWindow;
class wxPyPreviewFrame;
class wxPyWindow;
class wxPyPreviewControlBar;
class wxPyPrintPreview;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
