/////////////////////////////////////////////////////////////////////////////
// Name:        univ/button.cpp
// Purpose:     wxButton
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univbutton.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BUTTON

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/dcscreen.h"
    #include "wx/button.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// default margins around the image
static const wxCoord DEFAULT_BTN_MARGIN_X = 0;
static const wxCoord DEFAULT_BTN_MARGIN_Y = 0;

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxButton::Init()
{
    m_isPressed =
    m_isDefault = FALSE;
}

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap,
                      const wxString &label,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxValidator& validator,
                      const wxString &name)
{
    // center label by default
    if ( !(style & wxALIGN_MASK) )
    {
        style |= wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL;
    }

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetLabel(label);
    SetImageLabel(bitmap);
    SetBestSize(size);

    return TRUE;
}

wxButton::~wxButton()
{
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

/* static */
wxSize wxButtonBase::GetDefaultSize()
{
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;

        // this corresponds more or less to wxMSW standard
        s_sizeBtn.x = (50 * dc.GetCharWidth())/4;
        s_sizeBtn.y = (14 * dc.GetCharHeight())/8;
    }

    return s_sizeBtn;
}

wxSize wxButton::DoGetBestClientSize() const
{
    wxClientDC dc(wxConstCast(this, wxButton));
    wxCoord width, height;
    dc.GetMultiLineTextExtent(GetLabel(), &width, &height);

    if ( m_bitmap.Ok() )
    {
        // allocate extra space for the bitmap
        wxCoord heightBmp = m_bitmap.GetHeight() + 2*m_marginBmpY;
        if ( height < heightBmp )
            height = heightBmp;

        width += m_bitmap.GetWidth() + 2*m_marginBmpX;
    }

    return wxSize(width, height);
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxButton::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawButtonBorder();
    renderer->DrawLabel(m_bitmap, m_marginBmpX, m_marginBmpY);
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

wxString wxButton::GetInputHandlerType() const
{
    return wxINP_HANDLER_BUTTON;
}

void wxButton::Press()
{
    if ( !m_isPressed )
    {
        m_isPressed = TRUE;

        Refresh();
    }
}

void wxButton::Release()
{
    if ( m_isPressed )
    {
        m_isPressed = FALSE;

        Refresh();
    }
}

void wxButton::Toggle()
{
    if ( m_isPressed )
        Release();
    else
        Press();

    if ( !m_isPressed )
    {
        // releasing button after it had been pressed generates a click event
        Click();
    }
}

void wxButton::Click()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    InitCommandEvent(event);
    Command(event);
}

bool wxButton::PerformAction(const wxControlAction& action,
                             long numArg,
                             const wxString& strArg)
{
    if ( action == wxACTION_BUTTON_TOGGLE )
        Toggle();
    else if ( action == wxACTION_BUTTON_CLICK )
        Click();
    else if ( action == wxACTION_BUTTON_PRESS )
        Press();
    else if ( action == wxACTION_BUTTON_RELEASE )
        Release();
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxButton::SetImageLabel(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    m_marginBmpX = DEFAULT_BTN_MARGIN_X;
    m_marginBmpY = DEFAULT_BTN_MARGIN_Y;
}

void wxButton::SetImageMargins(wxCoord x, wxCoord y)
{
    m_marginBmpX = x;
    m_marginBmpY = y;
}

void wxButton::SetDefault()
{
    m_isDefault = TRUE;
}

// ============================================================================
// wxStdButtonInputHandler
// ============================================================================

wxStdButtonInputHandler::wxStdButtonInputHandler(wxInputHandler *handler)
                       : wxStdInputHandler(handler)
{
    m_winCapture = NULL;
    m_winHasMouse = FALSE;
}

bool wxStdButtonInputHandler::HandleKey(wxControl *control,
                                        const wxKeyEvent& event,
                                        bool pressed)
{
    int keycode = event.GetKeyCode();
    if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
    {
        control->PerformAction(wxACTION_BUTTON_TOGGLE);

        return TRUE;
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdButtonInputHandler::HandleMouse(wxControl *control,
                                          const wxMouseEvent& event)
{
    // the button has 2 states: pressed and normal with the following
    // transitions between them:
    //
    //      normal -> left down -> capture mouse and go to pressed state
    //      pressed -> left up inside -> generate click -> go to normal
    //                         outside ------------------>
    //
    // the other mouse buttons are ignored
    if ( event.Button(1) )
    {
        if ( event.ButtonDown(1) )
        {
            m_winCapture = control;
            m_winCapture->CaptureMouse();
            m_winHasMouse = TRUE;

            control->PerformAction(wxACTION_BUTTON_PRESS);

            return TRUE;
        }
        else // up
        {
            if ( m_winCapture )
            {
                m_winCapture->ReleaseMouse();
                m_winCapture = NULL;
            }

            if ( m_winHasMouse )
            {
                // this will generate a click event
                control->PerformAction(wxACTION_BUTTON_TOGGLE);

                return TRUE;
            }
            //else: the mouse was released outside the window, this doesn't
            //      count as a click
        }
    }

    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdButtonInputHandler::HandleMouseMove(wxControl *control,
                                              const wxMouseEvent& event)
{
    // we only have to do something when the mouse leaves/enters the pressed
    // button and don't care about the other ones
    if ( event.GetEventObject() == m_winCapture )
    {
        // leaving the button should remove its pressed state
        if ( event.Leaving() )
        {
            // remember that the mouse is now outside
            m_winHasMouse = FALSE;

            // we do have a pressed button, so release it
            control->SetCurrent(FALSE);
            control->PerformAction(wxACTION_BUTTON_RELEASE);

            return TRUE;
        }
        // and entering it back should make it pressed again if it had been
        // pressed
        else if ( event.Entering() )
        {
            // the mouse is (back) inside the button
            m_winHasMouse = TRUE;

            // we did have a pressed button which we released when leaving the
            // window, press it again
            control->SetCurrent(TRUE);
            control->PerformAction(wxACTION_BUTTON_PRESS);

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleMouseMove(control, event);
}

bool wxStdButtonInputHandler::HandleFocus(wxControl *control,
                                          const wxFocusEvent& event)
{
    // buttons change appearance when they get/lose focus, so return TRUE to
    // refresh
    return TRUE;
}

bool wxStdButtonInputHandler::HandleActivation(wxControl *control,
                                               bool activated)
{
    // the default button changes appearance when the app is [de]activated, so
    // return TRUE to refresh
    return TRUE;
}

#endif // wxUSE_BUTTON

