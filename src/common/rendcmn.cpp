///////////////////////////////////////////////////////////////////////////////
// Name:        common/rendcmn.cpp
// Purpose:     wxRendererNative common functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.07.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

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

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/renderer.h"

#include "wx/ptr_scpd.h"

// ----------------------------------------------------------------------------
// wxRendererPtr: auto pointer holding the global renderer
// ----------------------------------------------------------------------------

wxDECLARE_SCOPED_PTR(wxRendererNative, wxRendererPtrBase);
wxDEFINE_SCOPED_PTR(wxRendererNative, wxRendererPtrBase);

class wxRendererPtr : public wxRendererPtrBase
{
public:
    wxRendererPtr() : wxRendererPtrBase(NULL) { m_initialized = false; }

    // return true if we have a renderer, false otherwise
    bool IsOk()
    {
        if ( !m_initialized )
        {
            // only try to create the renderer once
            m_initialized = true;

            DoInit();
        }

        return get() != NULL;
    }

private:
    void DoInit()
    {
        wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
        if ( traits )
        {
            // ask the traits object to create a renderer for us
            reset(traits->CreateRenderer());
        }
    }

    bool m_initialized;

    DECLARE_NO_COPY_CLASS(wxRendererPtr)
};

// ============================================================================
// wxRendererNative implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::Get()
{
    static wxRendererPtr s_renderer;

    return s_renderer.IsOk() ? *s_renderer.get() : GetDefault();
}

