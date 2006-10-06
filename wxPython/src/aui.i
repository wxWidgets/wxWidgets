/////////////////////////////////////////////////////////////////////////////
// Name:        aui.i
// Purpose:     Wrappers for the wxAUI classes.
//
// Author:      Robin Dunn
//
// Created:     5-July-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"The wx.aui moduleis an Advanced User Interface library that aims to
implement \"cutting-edge\" interface usability and design features so
developers can quickly and easily create beautiful and usable
application interfaces.

**Vision and Design Principles**

wx.aui attempts to encapsulate the following aspects of the user
interface:

  * Frame Management: Frame management provides the means to open,
    move and hide common controls that are needed to interact with the
    document, and allow these configurations to be saved into
    different perspectives and loaded at a later time.

  * Toolbars: Toolbars are a specialized subset of the frame
    management system and should behave similarly to other docked
    components. However, they also require additional functionality,
    such as \"spring-loaded\" rebar support, \"chevron\" buttons and
    end-user customizability.

  * Modeless Controls: Modeless controls expose a tool palette or set
    of options that float above the application content while allowing
    it to be accessed. Usually accessed by the toolbar, these controls
    disappear when an option is selected, but may also be \"torn off\"
    the toolbar into a floating frame of their own.

  * Look and Feel: Look and feel encompasses the way controls are
    drawn, both when shown statically as well as when they are being
    moved. This aspect of user interface design incorporates \"special
    effects\" such as transparent window dragging as well as frame
    animation.

**PyAUI adheres to the following principles**

  - Use native floating frames to obtain a native look and feel for
    all platforms;

  - Use existing wxPython code where possible, such as sizer
    implementation for frame management;

  - Use standard wxPython coding conventions.


**Usage**

The following example shows a simple implementation that utilizes
`wx.aui.FrameManager` to manage three text controls in a frame window::

    import wx
    import wx.aui

    class MyFrame(wx.Frame):

        def __init__(self, parent, id=-1, title='wx.aui Test',
                     size=(800, 600), style=wx.DEFAULT_FRAME_STYLE):
            wx.Frame.__init__(self, parent, id, title, pos, size, style)

            self._mgr = wx.aui.FrameManager(self)

            # create several text controls
            text1 = wx.TextCtrl(self, -1, 'Pane 1 - sample text',
                                wx.DefaultPosition, wx.Size(200,150),
                                wx.NO_BORDER | wx.TE_MULTILINE)

            text2 = wx.TextCtrl(self, -1, 'Pane 2 - sample text',
                                wx.DefaultPosition, wx.Size(200,150),
                                wx.NO_BORDER | wx.TE_MULTILINE)

            text3 = wx.TextCtrl(self, -1, 'Main content window',
                                wx.DefaultPosition, wx.Size(200,150),
                                wx.NO_BORDER | wx.TE_MULTILINE)

            # add the panes to the manager
            self._mgr.AddPane(text1, wx.LEFT, 'Pane Number One')
            self._mgr.AddPane(text2, wx.BOTTOM, 'Pane Number Two')
            self._mgr.AddPane(text3, wx.CENTER)

            # tell the manager to 'commit' all the changes just made
            self._mgr.Update()

            self.Bind(wx.EVT_CLOSE, self.OnClose)


        def OnClose(self, event):
            # deinitialize the frame manager
            self._mgr.UnInit()
            # delete the frame
            self.Destroy()


    app = wx.App()
    frame = MyFrame(None)
    frame.Show()
    app.MainLoop()
"
%enddef



%module(package="wx", docstring=DOCSTRING) aui

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include <wx/aui/aui.h>
%}

//---------------------------------------------------------------------------

%import core.i
%import windows.i

%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }


%include _aui_docstrings.i

//---------------------------------------------------------------------------


#define wxUSE_AUI 1
#define WXDLLIMPEXP_AUI
#define unsigned
#define wxDEPRECATED(decl)


// We'll skip making wrappers for these, they have overloads that take a
// wxSize or wxPoint
%ignore wxPaneInfo::MaxSize(int x, int y);
%ignore wxPaneInfo::MinSize(int x, int y);
%ignore wxPaneInfo::BestSize(int x, int y);
%ignore wxPaneInfo::FloatingPosition(int x, int y);
%ignore wxPaneInfo::FloatingSize(int x, int y);

// But for these we will do the overloading (see %pythoncode below) so let's
// rename the C++ versions
%rename(_GetPaneByWidget) wxFrameManager::GetPane(wxWindow* window);
%rename(_GetPaneByName)   wxFrameManager::GetPane(const wxString& name);

%rename(_AddPane1) wxFrameManager::AddPane(wxWindow* window, const wxPaneInfo& pane_info);
%rename(_AddPane2) wxFrameManager::AddPane(wxWindow* window, int direction = wxLEFT,
                                           const wxString& caption = wxEmptyString);

%rename(AddPaneAtPos) wxFrameManager::AddPane(wxWindow* window,
                                              const wxPaneInfo& pane_info,
                                              const wxPoint& drop_pos);

// A typemap for the return value of wxFrameManager::GetAllPanes
%typemap(out) wxPaneInfoArray& {
    $result = PyList_New(0);
    for (size_t i=0; i < $1->GetCount(); i++) {
        PyObject* pane_obj = SWIG_NewPointerObj((void*)(&$1->Item(i)), SWIGTYPE_p_wxPaneInfo, 0);
        PyList_Append($result, pane_obj);
    }
}


%nokwargs wxAuiTabContainer::SetActivePage;

%pythonAppend wxAuiTabCtrl::wxAuiTabCtrl "self._setOORInfo(self)";

%pythonAppend wxAuiMultiNotebook::wxAuiMultiNotebook    "self._setOORInfo(self)";
%pythonAppend wxAuiMultiNotebook::wxAuiMultiNotebook()  "self._setOORInfo(self)";
%ignore wxAuiMultiNotebook::~wxAuiMultiNotebook;
%rename(PreAuiMultiNotebook) wxAuiMultiNotebook::wxAuiMultiNotebook();

//---------------------------------------------------------------------------
// Get all our defs from the REAL header files.
%include framemanager.h
%include dockart.h
%include floatpane.h
%include auibook.h

//---------------------------------------------------------------------------
// Methods to inject into the FrameManager class that will sort out calls to
// the overloaded versions of GetPane and AddPane

%extend wxFrameManager {
    %pythoncode {
    def GetPane(self, item):
        """
        GetPane(self, window_or_info item) -> PaneInfo

        GetPane is used to search for a `PaneInfo` object either by
        widget reference or by pane name, which acts as a unique id
        for a window pane. The returned `PaneInfo` object may then be
        modified to change a pane's look, state or position. After one
        or more modifications to the `PaneInfo`, `FrameManager.Update`
        should be called to realize the changes to the user interface.

        If the lookup failed (meaning the pane could not be found in
        the manager) GetPane returns an empty `PaneInfo`, a condition
        which can be checked by calling `PaneInfo.IsOk`.
        """
        if isinstance(item, wx.Window):
            return self._GetPaneByWidget(item)
        else:
            return self._GetPaneByName(item)

    def AddPane(self, window, info=None, caption=None):
        """
        AddPane(self, window, info=None, caption=None) -> bool

        AddPane tells the frame manager to start managing a child
        window. There are two versions of this function. The first
        verison accepts a `PaneInfo` object for the ``info`` parameter
        and allows the full spectrum of pane parameter
        possibilities. (Say that 3 times fast!)

        The second version is used for simpler user interfaces which
        do not require as much configuration.  In this case the
        ``info`` parameter specifies the direction property of the
        pane info, and defaults to ``wx.LEFT``.  The pane caption may
        also be specified as an extra parameter in this form.
        """
        if type(info) == PaneInfo:
            return self._AddPane1(window, info)
        else:
            # This Is AddPane2
            if info is None:
                info = wx.LEFT
            if caption is None:
                caption = ""
            return self._AddPane2(window, info, caption)
    }

    // For backwards compatibility
    %pythoncode {
         SetFrame = wx._deprecated(SetManagedWindow,
                                   "SetFrame is deprecated, use `SetManagedWindow` instead.")
         GetFrame = wx._deprecated(GetManagedWindow,
                                   "GetFrame is deprecated, use `GetManagedWindow` instead.")
    }
}

%extend wxDockInfo {
    ~wxDockInfo() {}
}

%extend wxDockUIPart {
    ~wxDockUIPart() {}
}

%extend wxPaneButton {
    ~wxPaneButton() {}
}

//---------------------------------------------------------------------------

%{
// A wxDocArt class that knows how to forward virtuals to Python methods  
class wxPyDockArt :  public wxDefaultDockArt
{
    wxPyDockArt() : wxDefaultDockArt() {}

    DEC_PYCALLBACK_INT_INT(GetMetric);
    DEC_PYCALLBACK_VOID_INTINT(SetMetric);
    DEC_PYCALLBACK__INTFONT(SetFont);
    DEC_PYCALLBACK_FONT_INT(GetFont);
    DEC_PYCALLBACK_COLOUR_INT(GetColour);
    DEC_PYCALLBACK__INTCOLOUR(SetColour);

    virtual void DrawSash(wxDC& dc,
                          wxWindow* window,
                          int orientation,
                          const wxRect& rect)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawSash"))) {
            PyObject* odc = wxPyMake_wxObject(&dc, false);
            PyObject* owin = wxPyMake_wxObject(window, false);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOiO)",
                                                         odc, owin, orientation, orect));
            Py_DECREF(odc);
            Py_DECREF(orect);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxDefaultDockArt::DrawSash(dc, window, orientation, rect);
    }

    virtual void DrawBackground(wxDC& dc,
                          wxWindow* window,
                          int orientation,
                          const wxRect& rect)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawBackground"))) {
            PyObject* odc = wxPyMake_wxObject(&dc, false);
            PyObject* owin = wxPyMake_wxObject(window, false);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOiO)",
                                                         odc, owin, orientation, orect));
            Py_DECREF(odc);
            Py_DECREF(orect);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxDefaultDockArt::DrawBackground(dc, window, orientation, rect);
    }

    virtual void DrawCaption(wxDC& dc,
                          wxWindow* window,
                          const wxString& text,
                          const wxRect& rect,
                          wxPaneInfo& pane)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawCaption"))) {
            PyObject* odc = wxPyMake_wxObject(&dc, false);
            PyObject* owin = wxPyMake_wxObject(window, false);
            PyObject* otext = wx2PyString(text);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            PyObject* opane = wxPyConstructObject((void*)&pane, wxT("wxPaneInfo"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOOOO)",
                                                         odc, owin, otext, orect, opane));
            Py_DECREF(odc);
            Py_DECREF(otext);
            Py_DECREF(orect);
            Py_DECREF(opane);
       }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxDefaultDockArt::DrawCaption(dc, window, text, rect, pane);
    }

    virtual void DrawGripper(wxDC& dc,
                          wxWindow* window,
                          const wxRect& rect,
                          wxPaneInfo& pane)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawGripper"))) {
            PyObject* odc = wxPyMake_wxObject(&dc, false);
            PyObject* owin = wxPyMake_wxObject(window, false);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            PyObject* opane = wxPyConstructObject((void*)&pane, wxT("wxPaneInfo"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOOO)", odc, owin, orect, opane));
            Py_DECREF(odc);
            Py_DECREF(orect);
            Py_DECREF(opane);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxDefaultDockArt::DrawGripper(dc, window, rect, pane);
    }

    virtual void DrawBorder(wxDC& dc,
                          wxWindow* window,
                          const wxRect& rect,
                          wxPaneInfo& pane)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawBorder"))) {
            PyObject* odc = wxPyMake_wxObject(&dc, false);
            PyObject* owin = wxPyMake_wxObject(window, false);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            PyObject* opane = wxPyConstructObject((void*)&pane, wxT("wxPaneInfo"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOO)", odc, orect, opane));
            Py_DECREF(odc);
            Py_DECREF(orect);
            Py_DECREF(opane);
       }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxDefaultDockArt::DrawBorder(dc, window, rect, pane);
    }

    virtual void DrawPaneButton(wxDC& dc,
                          wxWindow* window,
                          int button,
                          int button_state,
                          const wxRect& rect,
                          wxPaneInfo& pane)
    {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawPaneButton"))) {
            PyObject* odc = wxPyMake_wxObject(&dc, false);
            PyObject* owin = wxPyMake_wxObject(window, false);
            PyObject* orect = wxPyConstructObject((void*)&rect, wxT("wxRect"), 0);
            PyObject* opane = wxPyConstructObject((void*)&pane, wxT("wxPaneInfo"), 0);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOiIOO)",
                                                         odc, owin, button, button_state,
                                                         orect, opane));
            Py_DECREF(odc);
            Py_DECREF(orect);
            Py_DECREF(opane);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxDefaultDockArt::DrawPaneButton(dc, window, button, button_state, rect, pane);
    }

    PYPRIVATE;

};

IMP_PYCALLBACK_INT_INT(wxPyDockArt, wxDefaultDockArt, GetMetric);
IMP_PYCALLBACK_VOID_INTINT(wxPyDockArt, wxDefaultDockArt, SetMetric);
IMP_PYCALLBACK__INTFONT(wxPyDockArt, wxDefaultDockArt, SetFont);
IMP_PYCALLBACK_FONT_INT(wxPyDockArt, wxDefaultDockArt, GetFont);
IMP_PYCALLBACK_COLOUR_INT(wxPyDockArt, wxDefaultDockArt, GetColour);
IMP_PYCALLBACK__INTCOLOUR(wxPyDockArt, wxDefaultDockArt, SetColour);

%}


DocStr(wxPyDockArt,
"This version of the `DockArt` class has been instrumented to be
subclassable in Python and to reflect all calls to the C++ base class
methods to the Python methods implemented in the derived class.", "");

class wxPyDockArt :  public wxDefaultDockArt
{
    %pythonAppend wxPyDockArt     "self._setCallbackInfo(self, PyDockArt)"
    PyDocArt();

};


//---------------------------------------------------------------------------

%extend wxAuiMultiNotebook {
    %property(PageCount, GetPageCount, doc="See `GetPageCount`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
}


%extend wxAuiNotebookEvent {
    %property(OldSelection, GetOldSelection, SetOldSelection, doc="See `GetOldSelection` and `SetOldSelection`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
}


%extend wxAuiTabContainer {
    %property(ActivePage, GetActivePage, SetActivePage, doc="See `GetActivePage` and `SetActivePage`");
    %property(PageCount, GetPageCount, doc="See `GetPageCount`");
    %property(Pages, GetPages, doc="See `GetPages`");
}


%extend wxFrameManager {
    %property(AllPanes, GetAllPanes, doc="See `GetAllPanes`");
    %property(ArtProvider, GetArtProvider, SetArtProvider, doc="See `GetArtProvider` and `SetArtProvider`");
    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(ManagedWindow, GetManagedWindow, SetManagedWindow, doc="See `GetManagedWindow` and `SetManagedWindow`");
}


%extend wxFrameManagerEvent {
    %property(Button, GetButton, SetButton, doc="See `GetButton` and `SetButton`");
    %property(DC, GetDC, SetDC, doc="See `GetDC` and `SetDC`");
    %property(Pane, GetPane, SetPane, doc="See `GetPane` and `SetPane`");
}


//---------------------------------------------------------------------------

#undef wxUSE_AUI
#undef WXDLLIMPEXP_AUI

//---------------------------------------------------------------------------

