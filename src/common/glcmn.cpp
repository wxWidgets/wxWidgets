///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/glcmn.cpp
// Purpose:     wxGLCanvasBase implementation
// Author:      Vadim Zeitlin
// Created:     2007-04-09
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_GLCANVAS

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/glcanvas.h"

// DLL options compatibility check:
#include "wx/build.h"
WX_CHECK_BUILD_OPTIONS("wxGL")

IMPLEMENT_CLASS(wxGLApp, wxApp)

// ============================================================================
// implementation
// ============================================================================

wxGLCanvasBase::wxGLCanvasBase()
{
#if WXWIN_COMPATIBILITY_2_8
    m_glContext = NULL;
#endif

    // we always paint background entirely ourselves so prevent wx from erasing
    // it to avoid flicker
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

bool wxGLCanvasBase::SetCurrent(const wxGLContext& context) const
{
    // although on MSW it works even if the window is still hidden, it doesn't
    // work in other ports (notably X11-based ones) and documentation mentions
    // that SetCurrent() can only be called for a shown window, so check for it
    wxASSERT_MSG( IsShownOnScreen(), _T("can't make hidden GL canvas current") );

    return context.SetCurrent(*static_cast<const wxGLCanvas *>(this));
}

bool wxGLCanvasBase::SetColour(const wxString& colour)
{
    wxColour col = wxTheColourDatabase->Find(colour);
    if ( !col.Ok() )
        return false;

    GLboolean isRGBA;
    glGetBooleanv(GL_RGBA_MODE, &isRGBA);
    if ( isRGBA )
    {
        glColor3f(col.Red() / 256., col.Green() / 256., col.Blue() / 256.);
    }
    else // indexed colour
    {
        GLint pix = GetColourIndex(col);
        if ( pix == -1 )
        {
            wxLogError(_("Failed to allocate colour for OpenGL"));
            return false;
        }

        glIndexi(pix);
    }

    return true;
}

wxGLCanvasBase::~wxGLCanvasBase()
{
#if WXWIN_COMPATIBILITY_2_8
    delete m_glContext;
#endif // WXWIN_COMPATIBILITY_2_8
}

#if WXWIN_COMPATIBILITY_2_8

wxGLContext *wxGLCanvasBase::GetContext() const
{
    return m_glContext;
}

void wxGLCanvasBase::SetCurrent()
{
    if ( m_glContext )
        SetCurrent(*m_glContext);
}

void wxGLCanvasBase::OnSize(wxSizeEvent& WXUNUSED(event))
{
}

#endif // WXWIN_COMPATIBILITY_2_8

/* static */
bool wxGLCanvasBase::IsExtensionInList(const char *list, const char *extension)
{
    if ( !list )
        return false;

    for ( const char *p = list; *p; p++ )
    {
        // advance up to the next possible match
        p = wxStrstr(p, extension);
        if ( !p )
            break;

        // check that the extension appears at the beginning/ending of the list
        // or is preceded/followed by a space to avoid mistakenly finding
        // "glExtension" in a list containing some "glFunkyglExtension"
        if ( (p == list || p[-1] == ' ') )
        {
            char c = p[strlen(extension)];
            if ( c == '\0' || c == ' ' )
                return true;
        }
    }

    return false;
}

#endif // wxUSE_GLCANVAS

