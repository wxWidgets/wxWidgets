/////////////////////////////////////////////////////////////////////////////
// Name:        display.cpp
// Purpose:     MSW Implementation of wxDisplay class
// Author:      Royce Mitchell III
// Modified by:
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "display.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
   #include "wx/dynarray.h"
#endif

#include "wx/display.h"

// the following define is necessary to access the multi-monitor function
// declarations in a manner safe to use w/ Windows 95
// JACS: not used for now until we're clear about the legality
// of distributing multimon.h. Meanwhile you can download the file
// yourself from:
// http://www.microsoft.com/msj/0697/monitor/monitortextfigs.htm#fig4

#if 0
#define COMPILE_MULTIMON_STUBS
#include "wx/msw/multimon.h"
#endif

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

void wxmswInitDisplayRectArray();

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxmswDisplayInfo;

WX_DECLARE_OBJARRAY(wxmswDisplayInfo, wxmswDisplayInfoArray);

class wxmswDisplayInfo
{
public:
	HMONITOR m_hmon;
    DISPLAY_DEVICE m_dd;
    wxRect m_rect;
    int m_depth;
};

wxmswDisplayInfoArray* g_wxmswDisplayInfoArray = 0;

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxmswDisplayInfoArray);

// ===========================================================================
// implementation
// ===========================================================================

BOOL CALLBACK wxmswMonitorEnumProc (
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor-appropriate device context
  LPRECT lprcMonitor, // pointer to monitor intersection rectangle
  LPARAM dwData       // data passed from EnumDisplayMonitors
)
{
    wxmswDisplayInfo* info = new wxmswDisplayInfo();
    info->m_hmon = hMonitor;
    info->m_rect.SetX ( lprcMonitor->left );
    info->m_rect.SetY ( lprcMonitor->top );
    info->m_rect.SetWidth ( lprcMonitor->right - lprcMonitor->left );
    info->m_rect.SetHeight ( lprcMonitor->bottom - lprcMonitor->top );
    // now add this monitor to the array
    g_wxmswDisplayInfoArray->Add ( info );

    return TRUE; // continue the enumeration
}

class wxmswDisplayModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxmswDisplayModule)
public:
    wxmswDisplayModule() {}
    bool OnInit();
    void OnExit();
};

IMPLEMENT_DYNAMIC_CLASS(wxmswDisplayModule, wxModule)

bool wxmswDisplayModule::OnInit()
{
    g_wxmswDisplayInfoArray = new wxmswDisplayInfoArray();
    if ( !g_wxmswDisplayInfoArray )
    {
        wxFAIL_MSG(wxT("Couldn't allocate array for display information"));
        return FALSE;
    }

    // Royce3: I'm assuming that the monitor's are enumerated in the same
    // order as the calls to EnumDisplayDevices below. We shall soon see
    // if that assumption is correct.
    if ( !EnumDisplayMonitors ( NULL, NULL, wxmswMonitorEnumProc, 0 ) )
        wxLogLastError(wxT("EnumDisplayMonitors"));

    size_t iDevNum = 0, count = g_wxmswDisplayInfoArray->Count();
    while ( iDevNum < count )
    {
        wxmswDisplayInfo& info = (*g_wxmswDisplayInfoArray)[iDevNum];

        // MSDN: Before calling EnumDisplayDevices, you must initialize the cb
        // member of DISPLAY_DEVICE to the size, in bytes, of DISPLAY_DEVICE
        info.m_dd.cb = sizeof(info.m_dd);

        if ( !EnumDisplayDevices ( NULL, iDevNum, &info.m_dd, 0 ) )
            wxLogLastError(wxT("EnumDisplayDevices"));

        // get this display's Depth
        DEVMODE devmode;
        memset ( &devmode, 0, sizeof(devmode) );

        // MSDN: Before calling EnumDisplaySettings, set the dmSize member to
        // sizeof(DEVMODE), and set the dmDriverExtra member to indicate the size,
        // in bytes, of the additional space available to receive private
        // driver-data.
        devmode.dmSize = sizeof(devmode);
        devmode.dmDriverExtra = 0;

        if ( !EnumDisplaySettings ( info.m_dd.DeviceName, ENUM_CURRENT_SETTINGS, &devmode ) )
        {
            wxLogLastError(wxT("EnumDisplaySettings"));
            devmode.dmFields = 0;
        }

        if ( !(devmode.dmFields&DM_BITSPERPEL) )
            info.m_depth = -1;
        else
            info.m_depth = devmode.dmBitsPerPel;


        iDevNum++;
    }
    return TRUE;
}

void wxmswDisplayModule::OnExit()
{
    size_t count = g_wxmswDisplayInfoArray->Count();
    while ( count-- )
    {
        wxmswDisplayInfo* info = g_wxmswDisplayInfoArray->Detach ( count );
        delete info;
    }
    delete g_wxmswDisplayInfoArray;
    g_wxmswDisplayInfoArray = 0;
}

// ---------------------------------------------------------------------------
// wxDisplay
// ---------------------------------------------------------------------------

size_t wxDisplayBase::GetCount()
{
    return GetSystemMetrics ( SM_CMONITORS );
}

int wxDisplayBase::GetFromPoint ( const wxPoint& pt )
{
    POINT pt2;
    pt2.x = pt.x;
    pt2.y = pt.y;

    HMONITOR hmon = MonitorFromPoint ( pt2, 0 );
    if ( !hmon )
        return -1;
    size_t count = wxDisplayBase::GetCount(), index;

    for ( index = 0; index < count; index++ )
    {
        if ( hmon == (*g_wxmswDisplayInfoArray)[index].m_hmon )
            return index;
    }

    return -1;
}

wxDisplay::wxDisplay ( size_t index ) : wxDisplayBase ( index )
{
}

wxRect wxDisplay::GetGeometry() const
{
    return (*g_wxmswDisplayInfoArray)[m_index].m_rect;
}

int wxDisplay::GetDepth() const
{
    return (*g_wxmswDisplayInfoArray)[m_index].m_depth;
}

wxString wxDisplay::GetName() const
{
    return wxString ( (*g_wxmswDisplayInfoArray)[m_index].m_dd.DeviceName );
}

#endif//wxUSE_DISPLAY
