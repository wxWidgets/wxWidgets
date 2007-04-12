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
    DECLARE_DYNAMIC_CLASS(wxPySizer)
public:
    wxPySizer() : wxSizer() {};

    DEC_PYCALLBACK___pure(RecalcSizes);
    DEC_PYCALLBACK_wxSize__pure(CalcMin);
    PYPRIVATE;
};


//---------------------------------------------------------------------------

class wxPyValidator : public wxValidator {
    DECLARE_DYNAMIC_CLASS(wxPyValidator)
public:
    wxPyValidator() {
    }

    ~wxPyValidator() {
    }

    wxObject* Clone() const {
        wxPyValidator* ptr = NULL;
        wxPyValidator* self = (wxPyValidator*)this;

        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if (wxPyCBH_findCallback(self->m_myInst, "Clone")) {
            PyObject* ro;
            ro = wxPyCBH_callCallbackObj(self->m_myInst, Py_BuildValue("()"));
            if (ro) {
                wxPyConvertSwigPtr(ro, (void **)&ptr, wxT("wxPyValidator"));
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(blocked);

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


class wxPyTimer : public wxTimer
{
public:
    wxPyTimer(wxEvtHandler *owner=NULL, int id = -1);
    ~wxPyTimer();
    
    DEC_PYCALLBACK__(Notify);
    PYPRIVATE;
    DECLARE_ABSTRACT_CLASS(wxPyTimer)
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

#ifndef __WXX11__
class wxPyDropSource : public wxDropSource {
public:
#ifndef __WXGTK__
     wxPyDropSource(wxWindow *win = NULL,
                    const wxCursor &copy = wxNullCursor,
                    const wxCursor &move = wxNullCursor,
                    const wxCursor &none = wxNullCursor)
         : wxDropSource(win, copy, move, none) {}
#else
    wxPyDropSource(wxWindow *win = NULL,
                   const wxIcon& copy = wxNullIcon,
                   const wxIcon& move = wxNullIcon,
                   const wxIcon& none = wxNullIcon)
        : wxDropSource(win, copy, move, none) {}
#endif
    ~wxPyDropSource() { }

    DEC_PYCALLBACK_BOOL_DR(GiveFeedback);
    PYPRIVATE;
};


class wxPyDropTarget : public wxDropTarget {
public:
    wxPyDropTarget(wxDataObject *dataObject = NULL)
        : wxDropTarget(dataObject) {}

    // called when mouse leaves the window: might be used to remove the
    // feedback which was given in OnEnter()
    DEC_PYCALLBACK__(OnLeave);

    // called when the mouse enters the window (only once until OnLeave())
    DEC_PYCALLBACK_DR_2WXCDR(OnEnter);

    // called when the mouse moves in the window - shouldn't take long to
    // execute or otherwise mouse movement would be too slow
    DEC_PYCALLBACK_DR_2WXCDR(OnDragOver);
    
    // called after OnDrop() returns True: you will usually just call
    // GetData() from here and, probably, also refresh something to update the
    // new data and, finally, return the code indicating how did the operation
    // complete (returning default value in case of success and wxDragError on
    // failure is usually ok)
    DEC_PYCALLBACK_DR_2WXCDR_pure(OnData);
    
    // this function is called when data is dropped at position (x, y) - if it
    // returns True, OnData() will be called immediately afterwards which will
    // allow to retrieve the data dropped.
    DEC_PYCALLBACK_BOOL_INTINT(OnDrop);

    PYPRIVATE;
};

#endif


//---------------------------------------------------------------------------

class wxPyHScrolledWindow;
class wxPyVScrolledWindow;
class wxPyHVScrolledWindow;
class wxPyVListBox;
class wxPyHtmlListBox;
class wxPyPanel;
class wxPyScrolledWindow;
class wxPyPopupTransientWindow;
class wxPyPreviewFrame;
class wxPyWindow;
class wxPyPreviewControlBar;
class wxPyPrintPreview;
class wxPyListCtrl;
class wxPyControl;
class wxPyPrintout;
class wxGenericDragImage;
class wxPyTaskBarIcon;


#ifdef __WXMAC__
class wxPopupWindow;
class wxTaskBarIconEvent;
class wxTaskBarIcon;
class wxToggleButton;
#endif

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
