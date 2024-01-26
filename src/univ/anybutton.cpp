/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/anybutton.cpp
// Purpose:     wxAnyButton
// Author:      Vadim Zeitlin
// Created:     2014-03-26 (extracted from button.cpp and tglbtn.cpp)
// Copyright:   (c) 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"
#include "wx/univ/colschem.h"

// ============================================================================
// implementation
// ============================================================================


void wxAnyButton::Press()
{
    if ( !m_isPressed )
    {
        m_isPressed = true;

        Refresh();
    }
}

void wxAnyButton::Release()
{
    if ( m_isPressed )
    {
        m_isPressed = false;

        Refresh();
    }
}

void wxAnyButton::Toggle()
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
    Refresh();
}

bool wxAnyButton::PerformAction(const wxControlAction& action,
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

    return true;
}

/* static */
wxInputHandler *wxAnyButton::GetStdInputHandler(wxInputHandler *handlerDef)
{
    static wxStdAnyButtonInputHandler s_handlerBtn(handlerDef);

    return &s_handlerBtn;
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

wxSize wxAnyButton::DoGetBestClientSize() const
{
    wxClientDC dc(wxConstCast(this, wxAnyButton));
    wxCoord width, height;
    dc.GetMultiLineTextExtent(GetLabel(), &width, &height);

    if ( m_bitmap.IsOk() )
    {
        // allocate extra space for the bitmap
        wxCoord heightBmp = m_bitmap.GetHeight() + 2*m_marginBmpY;
        if ( height < heightBmp )
            height = heightBmp;

        width += m_bitmap.GetWidth() + 2*m_marginBmpX;
    }

    // The default size should not be adjusted, so the code is moved into the
    // renderer. This is conceptual wrong but currently the only solution.
    // (Otto Wyss, Patch 664399)

/*
    // for compatibility with other ports, the buttons default size is never
    // less than the standard one, but not when display not PDAs.
    if (wxSystemSettings::GetScreenType() > wxSYS_SCREEN_PDA)
    {
        if ( !(GetWindowStyle() & wxBU_EXACTFIT) )
        {
            wxSize szDef = GetDefaultSize();
            if ( width < szDef.x )
                width = szDef.x;
        }
    }
*/
    return wxSize(width, height);
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxAnyButton::DoDraw(wxControlRenderer *renderer)
{
    if ( !(GetWindowStyle() & wxBORDER_NONE) )
    {
        renderer->DrawButtonBorder();
    }

    renderer->DrawButtonLabel(m_bitmap, m_marginBmpX, m_marginBmpY);
}

bool wxAnyButton::DoDrawBackground(wxDC& dc)
{
    wxRect rect;
    wxSize size = GetSize();
    rect.width = size.x;
    rect.height = size.y;

    if ( GetBackgroundBitmap().IsOk() )
    {
        // get the bitmap and the flags
        int alignment;
        wxStretch stretch;
        wxBitmap bmp = GetBackgroundBitmap(&alignment, &stretch);
        wxControlRenderer::DrawBitmap(dc, bmp, rect, alignment, stretch);
    }
    else
    {
        m_renderer->DrawButtonSurface(dc, wxTHEME_BG_COLOUR(this),
                                      rect, GetStateFlags());
    }

    return true;
}

// ============================================================================
// wxStdAnyButtonInputHandler
// ============================================================================

wxStdAnyButtonInputHandler::wxStdAnyButtonInputHandler(wxInputHandler *handler)
                       : wxStdInputHandler(handler)
{
    m_winCapture = nullptr;
    m_winHasMouse = false;
}

bool wxStdAnyButtonInputHandler::HandleKey(wxInputConsumer *consumer,
                                        const wxKeyEvent& event,
                                        bool pressed)
{
    int keycode = event.GetKeyCode();
    if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
    {
        consumer->PerformAction(wxACTION_BUTTON_TOGGLE);

        return true;
    }

    return wxStdInputHandler::HandleKey(consumer, event, pressed);
}

bool wxStdAnyButtonInputHandler::HandleMouse(wxInputConsumer *consumer,
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
        if ( event.LeftDown() || event.LeftDClick() )
        {
            m_winCapture = consumer->GetInputWindow();
            m_winCapture->CaptureMouse();
            m_winHasMouse = true;

            consumer->PerformAction(wxACTION_BUTTON_PRESS);
        }
        else if ( event.LeftUp() )
        {
            if ( m_winCapture )
            {
                m_winCapture->ReleaseMouse();
                m_winCapture = nullptr;
            }

            if ( m_winHasMouse )
            {
                // this will generate a click event
                consumer->PerformAction(wxACTION_BUTTON_TOGGLE);

                return true;
            }
            //else: the mouse was released outside the window, this doesn't
            //      count as a click
        }
        //else: don't do anything special about the double click
    }

    return wxStdInputHandler::HandleMouse(consumer, event);
}

bool wxStdAnyButtonInputHandler::HandleMouseMove(wxInputConsumer *consumer,
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
            m_winHasMouse = false;

            // we do have a pressed button, so release it
            consumer->GetInputWindow()->SetCurrent(false);
            consumer->PerformAction(wxACTION_BUTTON_RELEASE);

            return true;
        }
        // and entering it back should make it pressed again if it had been
        // pressed
        else if ( event.Entering() )
        {
            // the mouse is (back) inside the button
            m_winHasMouse = true;

            // we did have a pressed button which we released when leaving the
            // window, press it again
            consumer->GetInputWindow()->SetCurrent(true);
            consumer->PerformAction(wxACTION_BUTTON_PRESS);

            return true;
        }
    }

    return wxStdInputHandler::HandleMouseMove(consumer, event);
}

bool wxStdAnyButtonInputHandler::HandleFocus(wxInputConsumer * WXUNUSED(consumer),
                                          const wxFocusEvent& WXUNUSED(event))
{
    // buttons change appearance when they get/lose focus, so return true to
    // refresh
    return true;
}

bool wxStdAnyButtonInputHandler::HandleActivation(wxInputConsumer *consumer,
                                               bool WXUNUSED(activated))
{
    // the default button changes appearance when the app is [de]activated, so
    // return true to refresh
    return wxStaticCast(consumer->GetInputWindow(), wxAnyButton)->IsDefault();
}
