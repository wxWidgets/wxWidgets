/////////////////////////////////////////////////////////////////////////////
// Name:        toplevel.cpp
// Purpose:
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
    
    wxWindow::Create(parent, id, pos, sizeOrig, style, name);

    wxTopLevelWindows.Append(this);

    m_title = title;
    
    // FIXME_MGL -- should activate itself when shown!

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
    if ( ret && show )
        SetFocus();
    return ret;
}

void wxTopLevelWindowMGL::Maximize(bool maximize)
{
    if ( maximize && !m_isMaximized )
    {
        int x, y, w, h;
        
        m_isMaximized = TRUE;

        GetPosition(&m_savedFrame.x, &m_savedFrame.y);
        GetSize(&m_savedFrame.width, &m_savedFrame.height);

        wxClientDisplayRect(&x, &y, &w, &h);
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

void wxTopLevelWindowMGL::Iconize(bool iconize)
{
    // FIXME_MGL - use wxDesktop for this
}

bool wxTopLevelWindowMGL::IsIconized() const
{
    return m_isIconized;
}
