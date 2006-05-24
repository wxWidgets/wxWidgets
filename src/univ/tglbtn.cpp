/////////////////////////////////////////////////////////////////////////////
// Name:        univ/tglbtn.cpp
// Purpose:     wxToggleButton
// Author:      Vadim Zeitlin
// Modified by: David Bjorkevik
// Created:     16.05.06
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOGGLEBTN

#include "wx/tglbtn.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/dcscreen.h"
    #include "wx/button.h"
    #include "wx/validate.h"
    #include "wx/settings.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"
#include "wx/univ/colschem.h"
#include "wx/stockitem.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED)

// default margins around the image
static const wxCoord DEFAULT_BTN_MARGIN_X = 0;  // We should give space for the border, at least.
static const wxCoord DEFAULT_BTN_MARGIN_Y = 0;

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxToggleButton::Init()
{
    m_isPressed = false;
    m_value = false;
}

bool wxToggleButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap,
                      const wxString &lbl,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxValidator& validator,
                      const wxString &name)
{
    wxString label(lbl);
    if (label.empty() && wxIsStockID(id))
        label = wxGetStockLabel(id);

    long ctrl_style = style & ~wxBU_ALIGN_MASK;

    wxASSERT_MSG( (ctrl_style & wxALIGN_MASK) == 0,
                  _T("Some style conflicts with align flags") );

    if((style & wxBU_RIGHT) == wxBU_RIGHT)
        ctrl_style |= wxALIGN_RIGHT;
    else if((style & wxBU_LEFT) == wxBU_LEFT)
        ctrl_style |= wxALIGN_LEFT;
    else
        ctrl_style |= wxALIGN_CENTRE_HORIZONTAL;

    if((style & wxBU_TOP) == wxBU_TOP)
        ctrl_style |= wxALIGN_TOP;
    else if((style & wxBU_BOTTOM) == wxBU_BOTTOM)
        ctrl_style |= wxALIGN_BOTTOM;
    else
        ctrl_style |= wxALIGN_CENTRE_VERTICAL;

    if ( !wxControl::Create(parent, id, pos, size, ctrl_style, validator, name) )
        return false;

    SetLabel(label);
    SetImageLabel(bitmap);
    // SetBestSize(size); -- called by SetImageLabel()

    CreateInputHandler(wxINP_HANDLER_BUTTON);

    return true;
}

wxToggleButton::~wxToggleButton()
{
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

/* static */
wxSize wxToggleButton::GetDefaultSize()
{
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;

        // this corresponds more or less to wxMSW standard in Win32 theme (see
        // wxWin32Renderer::AdjustSize())
//        s_sizeBtn.x = 8*dc.GetCharWidth();
//        s_sizeBtn.y = (11*dc.GetCharHeight())/10 + 2;
        // Otto Wyss, Patch 664399
        s_sizeBtn.x = dc.GetCharWidth()*10 + 2;
        s_sizeBtn.y = dc.GetCharHeight()*11/10 + 2;
    }

    return s_sizeBtn;
}

wxSize wxToggleButton::DoGetBestClientSize() const
{
    wxClientDC dc(wxConstCast(this, wxToggleButton));
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

void wxToggleButton::DoDraw(wxControlRenderer *renderer)
{
    if ( !(GetWindowStyle() & wxBORDER_NONE) )
    {
        renderer->DrawButtonBorder();
    }

    renderer->DrawLabel(m_bitmap, m_marginBmpX, m_marginBmpY);
}

bool wxToggleButton::DoDrawBackground(wxDC& dc)
{
    wxRect rect;
    wxSize size = GetSize();
    rect.width = size.x;
    rect.height = size.y;

    if ( GetBackgroundBitmap().Ok() )
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

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

void wxToggleButton::Press()
{
    if ( !m_isPressed )
    {
        m_isPressed = true;

        Refresh();
    }
}

void wxToggleButton::Release()
{
    if ( m_isPressed )
    {
        m_isPressed = false;

        Refresh();
    }
}

void wxToggleButton::Toggle()
{
    if ( m_isPressed )
        Release();
    else
        Press();

    if ( !m_isPressed )
    {
        // releasing button after it had been pressed generates a click event
        // and toggles value
        m_value = !m_value;
        Click();
    }
}

void wxToggleButton::Click()
{
    wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, GetId());
    InitCommandEvent(event);
    event.SetInt(GetValue());
    Command(event);
}

bool wxToggleButton::PerformAction(const wxControlAction& action,
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

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

void wxToggleButton::SetImageLabel(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;

    SetImageMargins(DEFAULT_BTN_MARGIN_X, DEFAULT_BTN_MARGIN_Y);
}

void wxToggleButton::SetImageMargins(wxCoord x, wxCoord y)
{
    m_marginBmpX = x + 2;
    m_marginBmpY = y + 2;

    SetBestSize(wxDefaultSize);
}

// void SetValue(bool state)
// Set the value of the toggle button.
void wxToggleButton::SetValue(bool state)
{
    m_value = state;
    Refresh();
}

// ============================================================================
// wxStdButtonInputHandler
// ============================================================================

wxStdToggleButtonInputHandler::wxStdToggleButtonInputHandler(wxInputHandler *handler)
                       : wxStdInputHandler(handler)
{
    m_winCapture = NULL;
    m_winHasMouse = false;
}

bool wxStdToggleButtonInputHandler::HandleKey(wxInputConsumer *consumer,
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

bool wxStdToggleButtonInputHandler::HandleMouse(wxInputConsumer *consumer,
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

            return true;
        }
        else if ( event.LeftUp() )
        {
            if ( m_winCapture )
            {
                m_winCapture->ReleaseMouse();
                m_winCapture = NULL;
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

bool wxStdToggleButtonInputHandler::HandleMouseMove(wxInputConsumer *consumer,
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

bool wxStdToggleButtonInputHandler::HandleFocus(wxInputConsumer * WXUNUSED(consumer),
                                          const wxFocusEvent& WXUNUSED(event))
{
    // buttons change appearance when they get/lose focus, so return true to
    // refresh
    return true;
}

bool wxStdToggleButtonInputHandler::HandleActivation(wxInputConsumer *consumer,
                                               bool WXUNUSED(activated))
{
    // the default button changes appearance when the app is [de]activated, so
    // return true to refresh
    return wxStaticCast(consumer->GetInputWindow(), wxToggleButton)->IsDefault();
}

#endif // wxUSE_TOGGLEBTN

