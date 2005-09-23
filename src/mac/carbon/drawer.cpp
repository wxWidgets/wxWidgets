/////////////////////////////////////////////////////////////////////////////
// Name:        drawer.cpp
// Purpose:     Drawer child window classes.
//              Drawer windows appear under their parent window and
//              behave like a drawer, opening and closing to reveal
//              content that does not need to be visible at all times.
// Author:      Jason Bagley 
// Modified by: Ryan Norton (To make it work :), plus bug fixes)
// Created:     2004-30-01
// RCS-ID:      $Id$
// Copyright:   (c) Jason Bagley; Art & Logic, Inc.
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/mac/private.h"

#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )

#include "wx/mac/carbon/drawer.h"

IMPLEMENT_DYNAMIC_CLASS(wxDrawerWindow, wxWindow)

// Use constants for now.
// These can be made into member variables and set dynamically.
const int kLeadingOffset = 20;
const int kTrailingOffset = 20;


// Converts Mac window edge constants to wxDirections, wxLEFT, wxRIGHT, etc.
static wxDirection WindowEdgeToDirection(OptionBits edge);

// Convert wxDirections to MAc window edge constants.
static OptionBits DirectionToWindowEdge(wxDirection direction);


wxDrawerWindow::wxDrawerWindow()
{
}

wxDrawerWindow::~wxDrawerWindow()
{ 
    m_isBeingDeleted = TRUE;
    this->Show(FALSE);
}
    
bool wxDrawerWindow::Create(wxWindow *parent,
 wxWindowID id, const wxString& title,
 wxSize size, wxDirection edge, const wxString& name)
{
    wxASSERT_MSG(NULL != parent, wxT("wxDrawerWindows must be attached to a parent window."));
       
    // Constrain the drawer size to the parent window.
    const wxSize parentSize(parent->GetClientSize());
    if (wxLEFT == edge || wxRIGHT == edge)
    {
        if (size.GetHeight() > parentSize.GetHeight())
            size.SetHeight(parentSize.GetHeight() - (kLeadingOffset + kTrailingOffset));
    }
    else
    {
        if (size.GetWidth() > parentSize.GetWidth())
            size.SetWidth(parentSize.GetWidth() - (kLeadingOffset + kTrailingOffset));
    }
    
    // Create the drawer window. 
    const wxPoint pos(0, 0);
    const wxSize dummySize(0,0);
    const long style = wxFRAME_DRAWER;
    
    bool success  = wxWindow::Create(parent, id, pos, dummySize, style, name);
    if (success)
    {
        this->MacCreateRealWindow(title, pos, size, style, name);
        success = (m_macWindow != NULL);
    }
    
    if (success)
    {
        // Use drawer brush.
        m_macBackgroundBrush.MacSetTheme(kThemeBrushDrawerBackground);
        ::SetThemeWindowBackground((WindowRef)m_macWindow,
         m_macBackgroundBrush.MacGetTheme(), false);
         
        // Leading and trailing offset are gaps from parent window edges
        // to where the drawer starts.
        ::SetDrawerOffsets((WindowRef)m_macWindow, kLeadingOffset, kTrailingOffset);

        // Set the drawers parent.
        // Is there a better way to get the parent's WindowRef?
        wxTopLevelWindow* tlwParent = wxDynamicCast(parent, wxTopLevelWindow);
        if (NULL != tlwParent)
        { 
            OSStatus status = ::SetDrawerParent((WindowRef)m_macWindow,
            (WindowRef)tlwParent->MacGetWindowRef());
            success = (noErr == status);
        }
        else
            success = false;
    }
    
    return success && SetPreferredEdge(edge);
}

wxDirection wxDrawerWindow::GetCurrentEdge() const
{
    const OptionBits edge = ::GetDrawerCurrentEdge((WindowRef)m_macWindow);
    return WindowEdgeToDirection(edge);
}

wxDirection wxDrawerWindow::GetPreferredEdge() const
{
    const OptionBits edge = ::GetDrawerPreferredEdge((WindowRef)m_macWindow);
    return WindowEdgeToDirection(edge);
}

bool wxDrawerWindow::IsOpen() const
{
    WindowDrawerState state = ::GetDrawerState((WindowRef)m_macWindow);
    return (state == kWindowDrawerOpen || state == kWindowDrawerOpening);
}

bool wxDrawerWindow::Open(bool show)
{
    static const Boolean kAsynchronous = true;
    OSStatus status = noErr;

    if (show)
    {
        const OptionBits preferredEdge = ::GetDrawerPreferredEdge((WindowRef)m_macWindow);
        status = ::OpenDrawer((WindowRef)m_macWindow, preferredEdge, kAsynchronous);
    }
    else
        status = ::CloseDrawer((WindowRef)m_macWindow, kAsynchronous);

    return (noErr == status);
}

bool wxDrawerWindow::SetPreferredEdge(wxDirection edge)
{
    const OSStatus status = ::SetDrawerPreferredEdge((WindowRef)m_macWindow,
     DirectionToWindowEdge(edge));
	return (noErr == status);
}


OptionBits DirectionToWindowEdge(wxDirection direction)
{
    OptionBits edge;
    switch (direction)
    {
        case wxTOP:
        edge = kWindowEdgeTop;
        break;
        
        case wxBOTTOM:
        edge = kWindowEdgeBottom;
        break;
        
        case wxRIGHT:
        edge = kWindowEdgeRight;
        break;
        
        case wxLEFT:
        default:
        edge = kWindowEdgeLeft;
        break;
    }
    return edge;
}

wxDirection WindowEdgeToDirection(OptionBits edge)
{
    wxDirection direction;
    switch (edge)
    {
        case kWindowEdgeTop:
        direction = wxTOP;
        break;
        
        case kWindowEdgeBottom:
        direction = wxBOTTOM;
        break;
        
        case kWindowEdgeRight:
        direction = wxRIGHT;
        break;
        
        case kWindowEdgeDefault: // store current preferred and return that here?
        case kWindowEdgeLeft:
        default:
        direction = wxLEFT;
        break;
    }
    
    return direction;
}

#endif // defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )

