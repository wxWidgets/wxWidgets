/////////////////////////////////////////////////////////////////////////////
// Name:        _display.i
// Purpose:     SWIG interface for wxVideoMode and wxDisplay
//
// Author:      Robin Dunn
//
// Created:     9-Mar-2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/display.h>
%}


//---------------------------------------------------------------------------
%newgroup


DocStr(wxVideoMode,
       "A simple struct containing video mode parameters for a display", "");

struct wxVideoMode
{
    wxVideoMode(int width = 0, int height = 0, int depth = 0, int freq = 0);
    ~wxVideoMode();
    
    DocDeclStr(
        bool , Matches(const wxVideoMode& other) const,
        "Returns true if this mode matches the other one in the sense that all
non zero fields of the other mode have the same value in this
one (except for refresh which is allowed to have a greater value)", "");

    DocDeclStr(
        int , GetWidth() const,
        "Returns the screen width in pixels (e.g. 640*480), 0 means unspecified", "");
    
    DocDeclStr(
        int , GetHeight() const,
        "Returns the screen width in pixels (e.g. 640*480), 0 means
unspecified", "");
    
    DocDeclStr(
        int , GetDepth() const,
        "Returns the screen's bits per pixel (e.g. 32), 1 is monochrome and 0
means unspecified/known", "");
    

    DocDeclStr(
        bool , IsOk() const,
        "returns true if the object has been initialized", "");
    


    %pythoncode { def __nonzero__(self): return self.IsOk() }
    %extend {
        bool __eq__(const wxVideoMode* other) { return other ? (*self == *other) : false; }
        bool __ne__(const wxVideoMode* other) { return other ? (*self != *other) : true;  }
    }

    
    // the screen size in pixels (e.g. 640*480), 0 means unspecified
    int w, h;

    // bits per pixel (e.g. 32), 1 is monochrome and 0 means unspecified/known
    int bpp;

    // refresh frequency in Hz, 0 means unspecified/unknown
    int refresh;
};


%immutable;
const wxVideoMode     wxDefaultVideoMode;
%mutable;


//---------------------------------------------------------------------------


%{
// dummy version of wxDisplay for when it is not enabled in the wxWidgets build
#if !wxUSE_DISPLAY
#include <wx/dynarray.h>
#include <wx/vidmode.h>

WX_DECLARE_OBJARRAY(wxVideoMode, wxArrayVideoModes);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayVideoModes);
const wxVideoMode wxDefaultVideoMode;

class wxDisplay 
{
public:
    wxDisplay(size_t index = 0) { wxPyRaiseNotImplemented(); }
    ~wxDisplay() {}

    static size_t  GetCount()
        { wxPyRaiseNotImplemented(); return 0; }
    
    static int GetFromPoint(const wxPoint& pt)
        { wxPyRaiseNotImplemented(); return wxNOT_FOUND; }
    static int GetFromWindow(wxWindow *window) 
        { wxPyRaiseNotImplemented(); return wxNOT_FOUND; }
        
    virtual bool IsOk() const { return false; }
    virtual wxRect GetGeometry() const { wxRect r; return r; }
    virtual wxString GetName() const { return wxEmptyString; }
    bool IsPrimary() const { return false; }
    
    wxArrayVideoModes GetModes(const wxVideoMode& mode = wxDefaultVideoMode)
        { wxArrayVideoModes a; return a; }

    virtual wxVideoMode GetCurrentMode() const
        { return wxDefaultVideoMode; }
    
    virtual bool ChangeMode(const wxVideoMode& mode = wxDefaultVideoMode)
       { return false; }
    
    void  ResetMode() {}
};
#endif
%}



DocStr(wxDisplay,
       "Represents a display/monitor attached to the system", "");
class wxDisplay
{
public:
    // 
    DocCtorStr(
        wxDisplay(size_t index = 0),
        "Set up a Display instance with the specified display.  The displays
are numbered from 0 to GetCount() - 1, 0 is always the primary display
and the only one which is always supported", "");

    virtual ~wxDisplay();
    
    DocDeclStr(
        static size_t , GetCount(),
        "Return the number of available displays.", "");
    

    DocDeclStr(
        static int , GetFromPoint(const wxPoint& pt),
        "Find the display where the given point lies, return wx.NOT_FOUND if it
doesn't belong to any display", "");
    

    DocStr(GetFromWindow,
           "Find the display where the given window lies, return wx.NOT_FOUND if
it is not shown at all.", "");
#ifdef __WXMSW__
    static int GetFromWindow(wxWindow *window);
#else
    %extend {
        static int GetFromWindow(wxWindow *window) 
            { wxPyRaiseNotImplemented(); return wxNOT_FOUND; }
    }
#endif
    
    DocDeclStr(
        virtual bool , IsOk() const,
        "Return true if the object was initialized successfully", "");
    %pythoncode { def __nonzero__(self): return self.IsOk() }
 

    DocDeclStr(
        virtual wxRect , GetGeometry() const,
        "Returns the bounding rectangle of the display whose index was passed
to the constructor.", "");
    

    DocDeclStr(
        virtual wxString , GetName() const,
        "Returns the display's name. A name is not available on all platforms.", "");
    

    DocDeclStr(
        bool , IsPrimary() const,
        "Returns true if the display is the primary display. The primary
display is the one whose index is 0.", "");
    


    %extend {
        DocAStr(GetModes,
                "GetModes(VideoMode mode=DefaultVideoMode) -> [videoMode...]",
                "Enumerate all video modes supported by this display matching the given
one (in the sense of VideoMode.Match()).

As any mode matches the default value of the argument and there is
always at least one video mode supported by display, the returned
array is only empty for the default value of the argument if this
function is not supported at all on this platform.", "");
        
        PyObject* GetModes(const wxVideoMode& mode = wxDefaultVideoMode) {
            PyObject* pyList = NULL;
            wxArrayVideoModes arr = self->GetModes(mode);
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            pyList = PyList_New(0);
            for (int i=0; i < arr.GetCount(); i++) {
                wxVideoMode* m = new wxVideoMode(arr.Item(i));
                PyObject* pyObj = wxPyConstructObject(m, wxT("wxVideoMode"), true);
                PyList_Append(pyList, pyObj);
                Py_DECREF(pyObj);
            }
            wxPyEndBlockThreads(blocked);
            return pyList;
        }
    }
    

    DocDeclStr(
        virtual wxVideoMode , GetCurrentMode() const,
        "Get the current video mode.", "");
    

    DocDeclStr(
        virtual bool , ChangeMode(const wxVideoMode& mode = wxDefaultVideoMode),
        "Changes the video mode of this display to the mode specified in the
mode parameter.

If wx.DefaultVideoMode is passed in as the mode parameter, the defined
behaviour is that wx.Display will reset the video mode to the default
mode used by the display.  On Windows, the behavior is normal.
However, there are differences on other platforms. On Unix variations
using X11 extensions it should behave as defined, but some
irregularities may occur.

On wxMac passing in wx.DefaultVideoMode as the mode parameter does
nothing.  This happens because Carbon no longer has access to
DMUseScreenPrefs, an undocumented function that changed the video mode
to the system default by using the system's 'scrn' resource.

Returns True if succeeded, False otherwise", "");
    

    DocDeclStr(
        void , ResetMode(),
        "Restore the default video mode (just a more readable synonym)", "");
    

};

//---------------------------------------------------------------------------
