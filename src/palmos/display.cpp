/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     Palm OS Implementation of wxDisplay class
// Author:      William Osborne
// Modified by:
// Created:     10.13.04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
   #include "wx/app.h"
   #include "wx/dynarray.h"
   #include "wx/frame.h"
#endif

#include "wx/dynload.h"

#include "wx/display.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#ifdef _UNICODE
    #define WINFUNC(x)  _T(#x) L"W"
#else
    #define WINFUNC(x) #x "A"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxDisplayInfo
{
public:
    // handle of this monitor used by MonitorXXX() functions, never NULL
    HMONITOR m_hmon;

    // IDirectDraw object used to control this display, may be NULL
    IDirectDraw2 *m_pDD2;

    // DirectDraw GUID for this display, only valid when using DirectDraw
    GUID m_guid;

    // the entire area of this monitor in virtual screen coordinates
    wxRect m_rect;

    // the display device name for this monitor, empty initially and retrieved
    // on demand by DoGetName()
    wxString m_devName;

    wxDisplayInfo() { m_hmon = NULL; m_pDD2 = NULL; }
    ~wxDisplayInfo() { if ( m_pDD2 ) m_pDD2->Release(); }
};

WX_DECLARE_OBJARRAY(wxDisplayInfo, wxDisplayInfoArray);
#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxDisplayInfoArray);

// this module is used to cleanup gs_displays array
class wxDisplayModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit();

    DECLARE_DYNAMIC_CLASS(wxDisplayModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDisplayModule, wxModule)

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// this is not really MT-unsafe as wxDisplay is only going to be used from the
// main thread, i.e. we consider that it's a GUI class and so don't protect it
static wxDisplayInfoArray *gs_displays = NULL;

// ===========================================================================
// implementation
// ===========================================================================

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

// initialize gs_displays using DirectX functions
static bool DoInitDirectX()
{
    return false;
}

// initialize gs_displays using the standard Windows functions
static void DoInitStdWindows()
{
}

// this function must be called before accessing gs_displays array as it
// creates and initializes it
static void InitDisplays()
{
}

// convert a DEVMODE to our wxVideoMode
wxVideoMode ConvertToVideoMode(const DEVMODE& dm)
{
    return wxVideoMode(160,
                       160,
                       16,
                       0);
}

// ----------------------------------------------------------------------------
// wxDisplayModule
// ----------------------------------------------------------------------------

void wxDisplayModule::OnExit()
{
}

// ---------------------------------------------------------------------------
// wxDisplay
// ---------------------------------------------------------------------------

/* static */
void wxDisplay::UseDirectX(bool useDX)
{
}

// helper of GetFromPoint() and GetFromWindow()
static int DisplayFromHMONITOR(HMONITOR hmon)
{
    return wxNOT_FOUND;
}

/* static */
size_t wxDisplayBase::GetCount()
{
    return 0;
}

/* static */
int wxDisplayBase::GetFromPoint ( const wxPoint& pt )
{
    return 0;
}

/* static */
int wxDisplayBase::GetFromWindow(wxWindow *window)
{
    return 0;
}

// ----------------------------------------------------------------------------
// wxDisplay ctor/dtor
// ----------------------------------------------------------------------------

wxDisplay::wxDisplay ( size_t n )
         : wxDisplayBase ( n )
{
}

wxDisplay::~wxDisplay()
{
}

// ----------------------------------------------------------------------------
// wxDisplay simple accessors
// ----------------------------------------------------------------------------

bool wxDisplay::IsOk() const
{
    return false;
}

wxRect wxDisplay::GetGeometry() const
{
    wxRect rect;

    return rect;
}

wxString wxDisplay::GetName() const
{
    wxString ret;
    
    return ret;
}

wxString wxDisplay::GetNameForEnumSettings() const
{
    wxString ret;
    
    return ret;
}

// ----------------------------------------------------------------------------
// video modes enumeration
// ----------------------------------------------------------------------------

wxArrayVideoModes
wxDisplay::DoGetModesDirectX(const wxVideoMode& WXUNUSED(modeMatch)) const
{
    wxArrayVideoModes modes;

    return modes;
}

wxArrayVideoModes
wxDisplay::DoGetModesWindows(const wxVideoMode& modeMatch) const
{
    wxArrayVideoModes modes;

    return modes;
}

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& modeMatch) const
{
    return gs_useDirectX ? DoGetModesDirectX(modeMatch)
                         : DoGetModesWindows(modeMatch);
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxVideoMode mode;

    return mode;
}

// ----------------------------------------------------------------------------
// video mode switching
// ----------------------------------------------------------------------------

bool wxDisplay::DoChangeModeDirectX(const wxVideoMode& mode)
{
    return false;
}

bool wxDisplay::DoChangeModeWindows(const wxVideoMode& mode)
{
    return false;
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    return gs_useDirectX ? DoChangeModeDirectX(mode)
                         : DoChangeModeWindows(mode);
}

#endif // wxUSE_DISPLAY

