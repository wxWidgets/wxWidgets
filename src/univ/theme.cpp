///////////////////////////////////////////////////////////////////////////////
// Name:        univ/theme.cpp
// Purpose:     implementation of wxTheme
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "theme.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/univ/renderer.h"
    #include "wx/univ/theme.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// dummy theme class
// ----------------------------------------------------------------------------

class wxDummyRenderer : public wxRenderer
{
public:
    virtual void DrawLabel(wxDC& dc, wxWindow *window) { }
    virtual void DrawBorder(wxDC& dc, wxWindow *window) { }
};

class wxDummyTheme : public wxTheme
{
public:
    wxDummyTheme()
    {
        m_renderer = new wxDummyRenderer;
    }

    virtual ~wxDummyTheme()
    {
        delete m_renderer;
    }

    virtual wxRenderer *GetRenderer() { return m_renderer; }
    virtual wxInputHandler *GetInputHandler() { return NULL; }
    virtual wxColourScheme *GetColourScheme() { return NULL; }

private:
    wxDummyRenderer *m_renderer;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation of the default theme (called by wxApp::OnInitGui)
// ----------------------------------------------------------------------------

wxTheme *wxTheme::ms_theme = (wxTheme *)NULL;

/* static */ bool wxTheme::CreateDefault()
{
    ms_theme = new wxDummyTheme;

    return TRUE;
}

/* static */ wxTheme *wxTheme::Set(wxTheme *theme)
{
    wxTheme *themeOld = ms_theme;
    ms_theme = theme;
    return themeOld;
}

// ----------------------------------------------------------------------------
// wxTheme dtor
// ----------------------------------------------------------------------------

wxTheme::~wxTheme()
{
}
