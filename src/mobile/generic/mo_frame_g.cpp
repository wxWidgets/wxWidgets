/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_frame_g.cpp
// Purpose:     Special frame class for use on iPhone or
//              iPhone 'emulator'
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:      $Id$
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#include "wx/mobile/generic/frame.h"
#include "wx/mobile/generic/simulator.h"
#include "wx/mobile/generic/keyboard.h"

IMPLEMENT_DYNAMIC_CLASS( wxMoFrame, wxWindow )

BEGIN_EVENT_TABLE( wxMoFrame, wxWindow )
    EVT_SIZE(wxMoFrame::OnSize)
    EVT_IDLE(wxMoFrame::OnIdle)
END_EVENT_TABLE()

wxMoFrame::wxMoFrame(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

wxMoFrame::~wxMoFrame()
{
}

void wxMoFrame::Init()
{
    m_simulatorScreenWindow = NULL;
}

bool wxMoFrame::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    wxMoSimulatorScreenWindow* actualParent = wxDynamicCast(parent, wxMoSimulatorScreenWindow);
    if (!actualParent)
        actualParent = GetSimulatorScreenWindow();

    if (!actualParent)
        actualParent = wxMoSimulatorScreenWindow::GetCurrentScreenWindow();

    wxASSERT(actualParent != NULL);

    style = style & ~(wxICONIZE|wxCAPTION|wxMINIMIZE|wxMINIMIZE_BOX|wxMAXIMIZE|wxMINIMIZE_BOX|
        wxCLOSE_BOX|wxSTAY_ON_TOP|wxSYSTEM_MENU|wxRESIZE_BORDER|wxFRAME_TOOL_WINDOW|
        wxFRAME_NO_TASKBAR|wxFRAME_FLOAT_ON_PARENT);

    m_title = caption;
    wxWindow::Create(actualParent, id, pos, size, style);

    if (actualParent)
        actualParent->SetApplicationWindow(this);

    SetBackgroundColour(wxColour(255, 255, 255));

    return true;    
}

void wxMoFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    DoLayout();
}

// Do layout
void wxMoFrame::DoLayout()
{
    // if we're using constraints or sizers - do use them
    if ( GetAutoLayout() )
    {
        Layout();
    }
    else
    {
        // do we have _exactly_ one child?
        wxWindow *child = (wxWindow *)NULL;
        for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = node->GetData();

            // exclude top level and managed windows (status bar isn't
            // currently in the children list except under wxMac anyhow, but
            // it makes no harm to test for it)
            if ( !win->IsTopLevel() && !IsOneOfBars(win) )
            {
                if ( child )
                {
                    return;     // it's our second subwindow - nothing to do
                }

                child = win;
            }
        }

        // do we have any children at all?
        if ( child && child->IsShown() )
        {
            // exactly one child - set it's size to fill the whole frame
            int clientW, clientH;
            DoGetClientSize(&clientW, &clientH);

            child->SetSize(0, 0, clientW, clientH);
        }
    }
}

// test whether this window makes part of the frame
// (menubar, toolbar and statusbar are excluded from automatic layout)
bool wxMoFrame::IsOneOfBars(const wxWindow *WXUNUSED(win)) const
{
    // TODO
    return false;
}

void wxMoFrame::SendSizeEvent()
{
    const wxSize size = GetSize();
    wxSizeEvent event( size, GetId() );
    event.SetEventObject( this );
    GetEventHandler()->AddPendingEvent( event );

#ifdef __WXGTK__
    // SendSizeEvent is typically called when a toolbar is shown
    // or hidden, but sending the size event alone is not enough
    // to trigger a full layout.
    ((wxFrame*)this)->GtkOnSize(
#ifndef __WXGTK20__
        0, 0, size.x, size.y
#endif // __WXGTK20__
    );
#endif // __WXGTK__
}

void wxMoFrame::OnIdle(wxIdleEvent& event)
{
    wxWindow* focusWin = wxFindFocusDescendant(this);
    if (focusWin)
    {
        wxTextCtrl* textCtrl = wxDynamicCast(focusWin, wxTextCtrl);
        if (textCtrl)
        {
            if (!wxMoKeyboard::IsKeyboardShowing())
            {
                wxMoKeyboard::ShowKeyboard(wxMOKEYBOARD_ALPHABETIC);
            }
        }
        else
        {
            if (wxMoKeyboard::IsKeyboardShowing())
            {
                wxWindow* focusWinBelowKeyboard = wxFindFocusDescendant(wxMoKeyboard::GetKeyboard());
                if (focusWinBelowKeyboard != focusWin)
                {
                    wxMoKeyboard::HideKeyboard();
                }
            }
        }
    }
    
    event.Skip();
}
