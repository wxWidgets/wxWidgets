/////////////////////////////////////////////////////////////////////////////
// Name:        toplevel.cpp
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"
#include "wx/toplevel.h"
#include "wx/app.h"
#include "wx/mgl/private.h"

// ----------------------------------------------------------------------------
// idle system
// ----------------------------------------------------------------------------

extern int g_openDialogs;

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

#ifndef __WXUNIVERSAL__
    IMPLEMENT_DYNAMIC_CLASS(wxTopLevelWindow, wxWindow)
#endif


// ============================================================================
// implementation
// ============================================================================

void wxTopLevelWindowMGL::Init()
{
    m_isShown = FALSE;
    m_isIconized = FALSE;
    m_isMaximized = FALSE;
    m_fsIsShowing = FALSE;
    m_sizeSet = FALSE;
}

bool wxTopLevelWindowMGL::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& sizeOrig,
                                 long style,
                                 const wxString &name)
{
    // always create a frame of some reasonable, even if arbitrary, size (at
    // least for MSW compatibility)
    wxSize size = sizeOrig;
    if ( size.x == -1 || size.y == -1 )
    {
        wxSize sizeDpy = wxGetDisplaySize();
        if ( size.x == -1 )
            size.x = sizeDpy.x / 3;
        if ( size.y == -1 )
            size.y = sizeDpy.y / 5;
    }
    
    wxWindow::Create(NULL, id, pos, sizeOrig, style, name);
    SetParent(parent);
    if ( parent )
        parent->AddChild(this);

    wxTopLevelWindows.Append(this);
    m_title = title;

    return TRUE;
}

wxTopLevelWindowMGL::~wxTopLevelWindowMGL()
{
    m_isBeingDeleted = TRUE;

    wxTopLevelWindows.DeleteObject(this);

    if (wxTheApp->GetTopWindow() == this)
        wxTheApp->SetTopWindow(NULL);

    if ((wxTopLevelWindows.Number() == 0) &&
        (wxTheApp->GetExitOnFrameDelete()))
    {
        wxTheApp->ExitMainLoop();
    }
}

bool wxTopLevelWindowMGL::ShowFullScreen(bool show, long style)
{
    if (show == m_fsIsShowing) return FALSE; // return what?

    m_fsIsShowing = show;

    if (show)
    {
        m_fsSaveStyle = m_windowStyle;
        m_fsSaveFlag = style;
        GetPosition(&m_fsSaveFrame.x, &m_fsSaveFrame.y);
        GetSize(&m_fsSaveFrame.width, &m_fsSaveFrame.height);

        if ( style & wxFULLSCREEN_NOCAPTION )
            m_windowStyle &= ~wxCAPTION;
        if ( style & wxFULLSCREEN_NOBORDER )
            m_windowStyle = wxSIMPLE_BORDER;

        int x, y;
        wxDisplaySize(&x, &y);
        SetSize(0, 0, x, y);
    }
    else
    {
        m_windowStyle = m_fsSaveStyle;
        SetSize(m_fsSaveFrame.x, m_fsSaveFrame.y, 
                m_fsSaveFrame.width, m_fsSaveFrame.height);
    }

    return TRUE;
}

bool wxTopLevelWindowMGL::Show(bool show)
{
    bool ret = wxTopLevelWindowBase::Show(show);

    // If this is the first time Show was called, send size event,
    // so that the frame can adjust itself (think auto layout or single child)
    if ( !m_sizeSet )
    {
        m_sizeSet = TRUE;
        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    if ( ret && show && AcceptsFocus() )
        SetFocus();
        // FIXME_MGL -- don't do this for popup windows?
    return ret;
}

void wxTopLevelWindowMGL::Maximize(bool maximize)
{
    int x, y, w, h;
    wxClientDisplayRect(&x, &y, &w, &h);

    rect_t screenRect = MGL_defRect(x, y, w, h);
    MGL_wmInvalidateRect(g_winMng, &screenRect);

    if ( maximize && !m_isMaximized )
    {
        m_isMaximized = TRUE;

        GetPosition(&m_savedFrame.x, &m_savedFrame.y);
        GetSize(&m_savedFrame.width, &m_savedFrame.height);

        SetSize(x, y, w, h);
    }
    else if ( !maximize && m_isMaximized )
    {
        m_isMaximized = FALSE;
        SetSize(m_savedFrame.x, m_savedFrame.y, 
                m_savedFrame.width, m_savedFrame.height);
    }
}

bool wxTopLevelWindowMGL::IsMaximized() const
{
    return m_isMaximized;
}

void wxTopLevelWindowMGL::Restore()
{
    if ( IsIconized() )
    {
        Iconize(FALSE);
    }
    if ( IsMaximized() )
    {
        Maximize(FALSE);
    }
}

void wxTopLevelWindowMGL::Iconize(bool WXUNUSED(iconize))
{
    wxFAIL_MSG(wxT("Iconize not supported under wxMGL"));
    // FIXME_MGL - Iconize is not supported in fullscreen mode.
    //             It will be supported in windowed mode (if ever implemented in MGL...)
}

bool wxTopLevelWindowMGL::IsIconized() const
{
    return m_isIconized;
}
