/////////////////////////////////////////////////////////////////////////////
// Name:        topluniv.cpp
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univtoplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif

#include "wx/toplevel.h"
#include "wx/univ/renderer.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/cshelp.h"


// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTopLevelWindow, wxTopLevelWindowNative)
    WX_EVENT_TABLE_INPUT_CONSUMER(wxTopLevelWindow)
    EVT_NC_PAINT(wxTopLevelWindow::OnNcPaint)
END_EVENT_TABLE()

WX_FORWARD_TO_INPUT_CONSUMER(wxTopLevelWindow)

// ============================================================================
// implementation
// ============================================================================

int wxTopLevelWindow::ms_drawDecorations = -1;

void wxTopLevelWindow::Init()
{
    m_isActive = FALSE;
    m_windowStyle = 0;
    m_pressedButton = 0;
}

bool wxTopLevelWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString &name)
{
    // init them to avoid compiler warnings
    long styleOrig = 0,
         exstyleOrig = 0;

    if ( ms_drawDecorations == -1 )
        ms_drawDecorations = !wxSystemSettings::HasFrameDecorations() ||
                             wxGetEnv(wxT("WXDECOR"), NULL);
        // FIXME -- wxUniv should provide a way to force non-native decorations!
        //          $WXDECOR is just a hack in absence of better wxUniv solution

    if ( ms_drawDecorations )
    {
        CreateInputHandler(wxINP_HANDLER_TOPLEVEL);

        styleOrig = style;
        exstyleOrig = GetExtraStyle();
        style &= ~(wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
                   wxSYSTEM_MENU | wxRESIZE_BORDER | wxFRAME_TOOL_WINDOW |
                   wxTHICK_FRAME);
        style = wxSIMPLE_BORDER;
        SetExtraStyle(exstyleOrig &
                      ~(wxFRAME_EX_CONTEXTHELP | wxDIALOG_EX_CONTEXTHELP));
    }

    if ( !wxTopLevelWindowNative::Create(parent, id, title, pos,
                                         size, style, name) )
        return FALSE;

    if ( ms_drawDecorations )
    {
        m_windowStyle = styleOrig;
        m_exStyle = exstyleOrig;
    }

    return TRUE;
}

bool wxTopLevelWindow::ShowFullScreen(bool show, long style)
{
    if ( show == IsFullScreen() ) return FALSE;

    if ( ms_drawDecorations )
    {
        if ( show )
        {
            m_fsSavedStyle = m_windowStyle;
            if ( style & wxFULLSCREEN_NOBORDER )
                m_windowStyle |= wxSIMPLE_BORDER;
            if ( style & wxFULLSCREEN_NOCAPTION )
                m_windowStyle &= ~wxCAPTION;
        }
        else
        {
            m_windowStyle = m_fsSavedStyle;
        }
    }

    return wxTopLevelWindowNative::ShowFullScreen(show, style);
}

long wxTopLevelWindow::GetDecorationsStyle() const
{
    long style = 0;

    if ( m_windowStyle & wxCAPTION )
    {
        style |= wxTOPLEVEL_TITLEBAR | wxTOPLEVEL_BUTTON_CLOSE;
        if ( m_windowStyle & wxMINIMIZE_BOX )
            style |= wxTOPLEVEL_BUTTON_ICONIZE;
        if ( m_windowStyle & wxMAXIMIZE_BOX )
        {
            if ( IsMaximized() )
                style |= wxTOPLEVEL_BUTTON_RESTORE;
            else
                style |= wxTOPLEVEL_BUTTON_MAXIMIZE;
        }
#if wxUSE_HELP
        if ( m_exStyle & (wxFRAME_EX_CONTEXTHELP | wxDIALOG_EX_CONTEXTHELP))
            style |= wxTOPLEVEL_BUTTON_HELP;
#endif
    }
    if ( (m_windowStyle & (wxSIMPLE_BORDER | wxNO_BORDER)) == 0 )
        style |= wxTOPLEVEL_BORDER;
    if ( m_windowStyle & (wxRESIZE_BORDER | wxTHICK_FRAME) )
        style |= wxTOPLEVEL_RESIZEABLE;

    if ( IsMaximized() )
        style |= wxTOPLEVEL_MAXIMIZED;
    if ( GetIcon().Ok() )
        style |= wxTOPLEVEL_ICON;
    if ( m_isActive )
        style |= wxTOPLEVEL_ACTIVE;

    return style;
}

void wxTopLevelWindow::RefreshTitleBar()
{
    wxNcPaintEvent event(GetId());
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// client area handling
// ----------------------------------------------------------------------------

wxPoint wxTopLevelWindow::GetClientAreaOrigin() const
{
    if ( ms_drawDecorations )
    {
        int w, h;
        wxTopLevelWindowNative::DoGetClientSize(&w, &h);
        wxRect rect = wxRect(wxTopLevelWindowNative::GetClientAreaOrigin(),
                             wxSize(w, h));
        rect = m_renderer->GetFrameClientArea(rect,
                                              GetDecorationsStyle());
        return rect.GetPosition();
    }
    else
    {
        return wxTopLevelWindowNative::GetClientAreaOrigin();
    }
}

void wxTopLevelWindow::DoGetClientSize(int *width, int *height) const
{
    if ( ms_drawDecorations )
    {
        int w, h;
        wxTopLevelWindowNative::DoGetClientSize(&w, &h);
        wxRect rect = wxRect(wxTopLevelWindowNative::GetClientAreaOrigin(),
                             wxSize(w, h));
        rect = m_renderer->GetFrameClientArea(rect,
                                              GetDecorationsStyle());
        if ( width )
            *width = rect.width;
        if ( height )
            *height = rect.height;
    }
    else
        wxTopLevelWindowNative::DoGetClientSize(width, height);
}

void wxTopLevelWindow::DoSetClientSize(int width, int height)
{
    if ( ms_drawDecorations )
    {
        wxSize size = m_renderer->GetFrameTotalSize(wxSize(width, height),
                                               GetDecorationsStyle());
        wxTopLevelWindowNative::DoSetClientSize(size.x, size.y);
    }
    else
        wxTopLevelWindowNative::DoSetClientSize(width, height);
}

void wxTopLevelWindow::OnNcPaint(wxPaintEvent& event)
{
    if ( !ms_drawDecorations || !m_renderer )
        event.Skip();
    else
    {
        // get the window rect
        wxRect rect;
        wxSize size = GetSize();
        rect.x =
        rect.y = 0;
        rect.width = size.x;
        rect.height = size.y;

        wxWindowDC dc(this);
        m_renderer->DrawFrameTitleBar(dc, rect,
                                      GetTitle(), m_titlebarIcon,
                                      GetDecorationsStyle(),
                                      m_pressedButton,
                                      wxCONTROL_PRESSED);
    }
}

long wxTopLevelWindow::HitTest(const wxPoint& pt) const
{
    int w, h;
    wxTopLevelWindowNative::DoGetClientSize(&w, &h);
    wxRect rect(wxTopLevelWindowNative::GetClientAreaOrigin(), wxSize(w, h));

    return m_renderer->HitTestFrame(rect, pt, GetDecorationsStyle());
}

// ----------------------------------------------------------------------------
// icons
// ----------------------------------------------------------------------------

void wxTopLevelWindow::SetIcon(const wxIcon& icon)
{
    wxTopLevelWindowNative::SetIcon(icon);

    if ( ms_drawDecorations && m_renderer )
    {
        wxSize size = m_renderer->GetFrameIconSize();

        if ( !icon.Ok() || size.x == -1  )
            m_titlebarIcon = icon;
        else
        {
            wxBitmap bmp1;
            bmp1.CopyFromIcon(icon);
            if ( !bmp1.Ok() )
                m_titlebarIcon = wxNullIcon;
            else if ( bmp1.GetWidth() == size.x && bmp1.GetHeight() == size.y )
                m_titlebarIcon = icon;
            else
            {
                wxImage img = bmp1.ConvertToImage();
                img.Rescale(size.x, size.y);
                m_titlebarIcon.CopyFromBitmap(wxBitmap(img));
            }
        }
    }
}

// ----------------------------------------------------------------------------
// actions
// ----------------------------------------------------------------------------

void wxTopLevelWindow::ClickTitleBarButton(long button)
{
    switch ( button )
    {
        case wxTOPLEVEL_BUTTON_CLOSE:
            Close();
            break;

        case wxTOPLEVEL_BUTTON_ICONIZE:
            Iconize();
            break;

        case wxTOPLEVEL_BUTTON_MAXIMIZE:
            Maximize();
            break;

        case wxTOPLEVEL_BUTTON_RESTORE:
            Restore();
            break;

        case wxTOPLEVEL_BUTTON_HELP:
#if wxUSE_HELP
            {
            wxContextHelp contextHelp(this);
            }
#endif
            break;

        default:
            wxFAIL_MSG(wxT("incorrect button specification"));
    }
}

bool wxTopLevelWindow::PerformAction(const wxControlAction& action,
                                     long numArg,
                                     const wxString& strArg)
{
    bool isActive = numArg != 0;

    if ( action == wxACTION_TOPLEVEL_ACTIVATE )
    {
        if ( m_isActive != isActive )
        {
            m_isActive = isActive;
            RefreshTitleBar();
        }
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_BUTTON_PRESS )
    {
        m_pressedButton = numArg;
        RefreshTitleBar();
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_BUTTON_RELEASE )
    {
        m_pressedButton = 0;
        RefreshTitleBar();
        return TRUE;
    }

    else if ( action == wxACTION_TOPLEVEL_BUTTON_CLICK )
    {
        m_pressedButton = 0;
        RefreshTitleBar();
        ClickTitleBarButton(numArg);
        return TRUE;
    }
    
    else if ( action == wxACTION_TOPLEVEL_MOVE )
    {
        InteractiveMove(wxINTERACTIVE_MOVE);
        return TRUE;
    }
    
    else if ( action == wxACTION_TOPLEVEL_RESIZE )
    {
        int flags = wxINTERACTIVE_RESIZE;
        if ( numArg & wxHT_TOPLEVEL_BORDER_N )
            flags |= wxINTERACTIVE_RESIZE_N;
        if ( numArg & wxHT_TOPLEVEL_BORDER_S )
            flags |= wxINTERACTIVE_RESIZE_S;
        if ( numArg & wxHT_TOPLEVEL_BORDER_W )
            flags |= wxINTERACTIVE_RESIZE_W;
        if ( numArg & wxHT_TOPLEVEL_BORDER_E )
            flags |= wxINTERACTIVE_RESIZE_E;
        InteractiveMove(flags);
        return TRUE;
    }
    
    else
        return FALSE;
}


// ============================================================================
// wxStdFrameInputHandler: handles focus, resizing and titlebar buttons clicks
// ============================================================================

wxStdFrameInputHandler::wxStdFrameInputHandler(wxInputHandler *inphand)
            : wxStdInputHandler(inphand)
{
    m_winCapture = NULL;
    m_winHitTest = 0;
    m_winPressed = 0;
    m_borderCursorOn = FALSE;
}

bool wxStdFrameInputHandler::HandleMouse(wxInputConsumer *consumer,
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
            wxTopLevelWindow *w = wxStaticCast(consumer->GetInputWindow(), wxTopLevelWindow);
            long hit = w->HitTest(event.GetPosition());

            if ( hit & wxHT_TOPLEVEL_ANY_BUTTON )
            {
                m_winCapture = w;
                m_winCapture->CaptureMouse();
                m_winHitTest = hit;
                m_winPressed = hit;
                consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_PRESS, m_winPressed);
                return TRUE;
            }
            else if ( hit & wxHT_TOPLEVEL_TITLEBAR )
            {
                consumer->PerformAction(wxACTION_TOPLEVEL_MOVE);
                return TRUE;
            }
            else if ( (consumer->GetInputWindow()->GetWindowStyle() & wxRESIZE_BORDER)
                      && (hit & wxHT_TOPLEVEL_ANY_BORDER) )
            {
                consumer->PerformAction(wxACTION_TOPLEVEL_RESIZE, hit);
                return TRUE;
            }
        }

        else // up
        {
            if ( m_winCapture )
            {
                m_winCapture->ReleaseMouse();
                m_winCapture = NULL;

                if ( m_winHitTest == m_winPressed )
                {
                    consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_CLICK, m_winPressed);
                    return TRUE;
                }
            }
            //else: the mouse was released outside the window, this doesn't
            //      count as a click
        }
    }

    return wxStdInputHandler::HandleMouse(consumer, event);
}

bool wxStdFrameInputHandler::HandleMouseMove(wxInputConsumer *consumer,
                                             const wxMouseEvent& event)
{
    if ( event.GetEventObject() == m_winCapture )
    {
        long hit = m_winCapture->HitTest(event.GetPosition());

        if ( hit != m_winHitTest )
        {
            if ( hit != m_winPressed )
                consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_RELEASE, m_winPressed);
            else
                consumer->PerformAction(wxACTION_TOPLEVEL_BUTTON_PRESS, m_winPressed);

            m_winHitTest = hit;
            return TRUE;
        }
    }
    else if ( consumer->GetInputWindow()->GetWindowStyle() & wxRESIZE_BORDER )
    {
        wxTopLevelWindow *win = wxStaticCast(consumer->GetInputWindow(), 
                                             wxTopLevelWindow);
        long hit = win->HitTest(event.GetPosition());
        
        if ( hit != m_winHitTest )
        {
            m_winHitTest = hit;
            
            if ( m_borderCursorOn )
            {
                m_borderCursorOn = FALSE;
                win->SetCursor(m_origCursor);
            }
            
            if ( hit & wxHT_TOPLEVEL_ANY_BORDER )
            {
                m_borderCursorOn = TRUE;
                wxCursor cur;
            
                switch (hit)
                {
                    case wxHT_TOPLEVEL_BORDER_N:
                    case wxHT_TOPLEVEL_BORDER_S:
                        cur = wxCursor(wxCURSOR_SIZENS);
                        break;
                    case wxHT_TOPLEVEL_BORDER_W:
                    case wxHT_TOPLEVEL_BORDER_E:
                        cur = wxCursor(wxCURSOR_SIZEWE);
                        break;
                    case wxHT_TOPLEVEL_BORDER_NE:
                    case wxHT_TOPLEVEL_BORDER_SW:
                        cur = wxCursor(wxCURSOR_SIZENESW);
                        break;
                    case wxHT_TOPLEVEL_BORDER_NW:
                    case wxHT_TOPLEVEL_BORDER_SE:
                        cur = wxCursor(wxCURSOR_SIZENWSE);
                        break;
                    default:
                        m_borderCursorOn = FALSE;
                        break;
                }
                if ( m_borderCursorOn )
                {
                    m_origCursor = win->GetCursor();
                    win->SetCursor(cur);
                }
            }
        }
    }

    return wxStdInputHandler::HandleMouseMove(consumer, event);
}

bool wxStdFrameInputHandler::HandleActivation(wxInputConsumer *consumer,
                                              bool activated)
{
    if ( m_borderCursorOn )
    {
        consumer->GetInputWindow()->SetCursor(m_origCursor);
        m_borderCursorOn = FALSE;
    }
    consumer->PerformAction(wxACTION_TOPLEVEL_ACTIVATE, activated);
    return FALSE;
}
