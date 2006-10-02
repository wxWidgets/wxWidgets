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
        "Returns True if this mode matches the other one in the sense that all
non-zero fields of the other mode have the same value in this
one (except for refresh which is allowed to have a greater value)", "");

    DocDeclStr(
        int , GetWidth() const,
        "Returns the screen width in pixels (e.g. 640*480), 0 means unspecified", "");

    DocDeclStr(
        int , GetHeight() const,
        "Returns the screen height in pixels (e.g. 640*480), 0 means unspecified", "");

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

    %property(Depth, GetDepth, doc="See `GetDepth`");
    %property(Height, GetHeight, doc="See `GetHeight`");
    %property(Width, GetWidth, doc="See `GetWidth`");
};


%{
#if !wxUSE_DISPLAY
const wxVideoMode     wxDefaultVideoMode;
#endif
%}

%immutable;
const wxVideoMode     wxDefaultVideoMode;
%mutable;


//---------------------------------------------------------------------------



DocStr(wxDisplay,
       "Represents a display/monitor attached to the system", "");
class wxDisplay
{
public:
    DocCtorStr(
        wxDisplay(unsigned index = 0),
        "Set up a Display instance with the specified display.  The displays
are numbered from 0 to GetCount() - 1, 0 is always the primary display
and the only one which is always supported", "");

    ~wxDisplay();

    DocDeclStr(
        static unsigned , GetCount(),
        "Return the number of available displays.", "");

    DocDeclStr(
        static int , GetFromPoint(const wxPoint& pt),
        "Find the display where the given point lies, return wx.NOT_FOUND if it
doesn't belong to any display", "");

    DocStr(GetFromWindow,
           "Find the display where the given window lies, return wx.NOT_FOUND if
it is not shown at all.", "");

    static int GetFromWindow(wxWindow *window);


    DocDeclStr(
        bool , IsOk() const,
        "Return true if the object was initialized successfully", "");
    %pythoncode { def __nonzero__(self): return self.IsOk() }

    DocDeclStr(
        wxRect , GetGeometry() const,
        "Returns the bounding rectangle of the display whose index was passed
to the constructor.", "");

    DocDeclStr(
        wxRect , GetClientArea() const,
        "Returns the bounding rectangle the client area of the display,
i.e., without taskbars and such.", "");

    DocDeclStr(
        wxString , GetName() const,
        "Returns the display's name. A name is not available on all platforms.", "");

    DocDeclStr(
        bool , IsPrimary() const,
        "Returns True if the display is the primary display. The primary
display is the one whose index is 0.", "");


    %extend
    {
        DocAStr(GetModes,
                "GetModes(VideoMode mode=DefaultVideoMode) -> [videoMode...]",
                "Enumerate all video modes supported by this display matching the given
one (in the sense of VideoMode.Match()).

As any mode matches the default value of the argument and there is
always at least one video mode supported by display, the returned
array is only empty for the default value of the argument if this
function is not supported at all on this platform.", "");

        PyObject* GetModes(const wxVideoMode& mode = wxDefaultVideoMode)
        {
%#if wxUSE_DISPLAY
            PyObject* pyList = NULL;
            wxArrayVideoModes arr = self->GetModes(mode);
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            pyList = PyList_New(0);
            for (size_t i=0; i < arr.GetCount(); i++)
            {
                wxVideoMode* m = new wxVideoMode(arr.Item(i));
                PyObject* pyObj = wxPyConstructObject(m, wxT("wxVideoMode"), true);
                PyList_Append(pyList, pyObj);
                Py_DECREF(pyObj);
            }
            wxPyEndBlockThreads(blocked);
            return pyList;
%#else
        wxPyRaiseNotImplemented();
        return NULL;
%#endif
        }

        DocStr(GetCurrentMode,
               "Get the current video mode.", "");
        wxVideoMode GetCurrentMode() const
        {
%#if wxUSE_DISPLAY
            return self->GetCurrentMode();
%#else
            wxPyRaiseNotImplemented();
            return wxDefaultVideoMode;
%#endif
        }


        DocStr(
            ChangeMode,
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

        bool ChangeMode(const wxVideoMode& mode = wxDefaultVideoMode)
        {
%#if wxUSE_DISPLAY
            return self->ChangeMode(mode);
%#else
            wxPyRaiseNotImplemented();
            return false;
%#endif
        }


        DocStr(
            ResetMode,
            "Restore the default video mode (just a more readable synonym)", "");
        void ResetMode()
        {
%#if wxUSE_DISPLAY
            self->ResetMode();
%#else
            wxPyRaiseNotImplemented();
%#endif
        }

    } // end of %extend

    %property(ClientArea, GetClientArea, doc="See `GetClientArea`");
    %property(CurrentMode, GetCurrentMode, doc="See `GetCurrentMode`");
    %property(Geometry, GetGeometry, doc="See `GetGeometry`");
    %property(Modes, GetModes, doc="See `GetModes`");
    %property(Name, GetName, doc="See `GetName`");
    
};

//---------------------------------------------------------------------------
