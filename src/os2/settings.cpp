/////////////////////////////////////////////////////////////////////////////
// Name:        settings.cpp
// Purpose:     wxSettings
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/gdicmn.h"
#endif

#include "wx/module.h"
#include "wx/settings.h"
#include "wx/window.h"
#include "wx/os2/private.h"

// the module which is used to clean up wxSystemSettings data (this is a
// singleton class so it can't be done in the dtor)
class wxSystemSettingsModule : public wxModule
{
    friend class wxSystemSettings;
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxSystemSettingsModule)

    static wxArrayString   sm_optionNames;
    static wxArrayString   sm_optionValues;
};

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

static wxFont *gs_fontDefault = NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxSystemSettingsModule
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSystemSettingsModule, wxModule)

wxArrayString wxSystemSettingsModule::sm_optionNames;
wxArrayString wxSystemSettingsModule::sm_optionValues;

bool wxSystemSettingsModule::OnInit()
{
    return TRUE;
}

void wxSystemSettingsModule::OnExit()
{
    sm_optionNames.Clear();
    sm_optionValues.Clear();
    delete gs_fontDefault;
}

wxColour wxSystemSettingsNative::GetColour(
  wxSystemColour                    nIndex
)
{
    COLORREF                        vRef;
    wxColour                        vCol;
    switch (nIndex)
    {
        //
        // PM actually has values for these
        //
        case wxSYS_COLOUR_WINDOW:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_WINDOW
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        case wxSYS_COLOUR_WINDOWFRAME:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_WINDOWFRAME
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        case wxSYS_COLOUR_MENUTEXT:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_MENUTEXT
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        case wxSYS_COLOUR_BTNFACE:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_BUTTONDEFAULT
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        case wxSYS_COLOUR_BTNSHADOW:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_BUTTONMIDDLE
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        case wxSYS_COLOUR_BTNHIGHLIGHT:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_BUTTONLIGHT
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        case wxSYS_COLOUR_MENUBAR:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_MENU
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;

        //
        // We'll have to just give values to these
        //
        case wxSYS_COLOUR_LISTBOX:
        case wxSYS_COLOUR_CAPTIONTEXT:
            return(*wxWHITE);
            break;

        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_INFOTEXT:
            vCol = (*wxBLACK);
            break;

        //
        // We should customize these to look like other ports
        //

        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_HIGHLIGHT:
            vCol = (*wxBLUE);
            break;

        case wxSYS_COLOUR_SCROLLBAR:
        case wxSYS_COLOUR_BACKGROUND:
        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_APPWORKSPACE:
        case wxSYS_COLOUR_HIGHLIGHTTEXT:
        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_3DDKSHADOW:
        case wxSYS_COLOUR_3DLIGHT:
        case wxSYS_COLOUR_INFOBK:
            vCol = (*wxLIGHT_GREY);
            break;

        default:
            vRef = (ULONG)::WinQuerySysColor( HWND_DESKTOP
                                             ,SYSCLR_WINDOW
                                             ,0L
                                            );
            vCol.Set( GetRValue(vRef)
                     ,GetGValue(vRef)
                     ,GetBValue(vRef)
                    );
            break;
    }
    return(vCol);
} // end of wxSystemSettingsNative::GetColour

wxFont wxSystemSettingsNative::GetFont(
  wxSystemFont                      index
)
{
    // TODO
    switch (index)
    {
        case wxSYS_DEVICE_DEFAULT_FONT:
        {
            break;
        }
        case wxSYS_DEFAULT_PALETTE:
        {
            break;
        }
        case wxSYS_SYSTEM_FIXED_FONT:
        {
            break;
        }
        case wxSYS_SYSTEM_FONT:
        {
            break;
        }
        default:
        case wxSYS_DEFAULT_GUI_FONT:
        {
            break;
        }
    }
    if(wxSWISS_FONT)
         return *wxSWISS_FONT;

    return wxNullFont;
}

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(
  wxSystemMetric                    index
)
{
    switch ( index)
    {
        // TODO case wxSYS_MOUSE_BUTTONS:
        // TODO case wxSYS_BORDER_X:
        // TODO case wxSYS_BORDER_Y:
        // TODO case wxSYS_CURSOR_X:
        // TODO case wxSYS_CURSOR_Y:
        // TODO case wxSYS_DCLICK_X:
        // TODO case wxSYS_DCLICK_Y:
        // TODO case wxSYS_DRAG_X:
        // TODO case wxSYS_DRAG_Y:
        // TODO case wxSYS_EDGE_X:
        // TODO case wxSYS_EDGE_Y:
        // TODO case wxSYS_HSCROLL_ARROW_X:
        // TODO case wxSYS_HSCROLL_ARROW_Y:
        // TODO case wxSYS_HTHUMB_X:
        // TODO case wxSYS_ICON_X:
        // TODO case wxSYS_ICON_Y:
        // TODO case wxSYS_ICONSPACING_X:
        // TODO case wxSYS_ICONSPACING_Y:
        // TODO case wxSYS_WINDOWMIN_X:
        // TODO case wxSYS_WINDOWMIN_Y:
        
        case wxSYS_SCREEN_X:
            return ::WinQuerySysValue(HWND_DESKTOP,SV_CXSCREEN);
        case wxSYS_SCREEN_Y:
            return ::WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN);
            
        // TODO case wxSYS_FRAMESIZE_X:
        // TODO case wxSYS_FRAMESIZE_Y:
        // TODO case wxSYS_SMALLICON_X:
        // TODO case wxSYS_SMALLICON_Y:
        // TODO case wxSYS_HSCROLL_Y:
        // TODO case wxSYS_VSCROLL_X:
        // TODO case wxSYS_VSCROLL_ARROW_X:
        // TODO case wxSYS_VSCROLL_ARROW_Y:
        // TODO case wxSYS_VTHUMB_Y:
        // TODO case wxSYS_CAPTION_Y:
        // TODO case wxSYS_MENU_Y:
        // TODO case wxSYS_NETWORK_PRESENT:
        // TODO case wxSYS_PENWINDOWS_PRESENT:
        // TODO case wxSYS_SHOW_SOUNDS:
        // TODO case wxSYS_SWAP_BUTTONS:
            
        default:
            return -1;  // unsupported metric
    }
    return 0;
}

bool wxSystemSettingsNative::HasFeature(
  wxSystemFeature                   index
)
{
    switch (index)
    {
        case wxSYS_CAN_ICONIZE_FRAME:
            return TRUE;

        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return FALSE;

        default:
            return FALSE;
    }
    return FALSE;
}
