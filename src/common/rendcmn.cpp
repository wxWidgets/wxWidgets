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
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/renderer.h"

#include "wx/ptr_scpd.h"

#if wxUSE_DYNLIB_CLASS
    #include "wx/dynlib.h"
#endif // wxUSE_DYNLIB_CLASS

// ----------------------------------------------------------------------------
// wxRendererPtr: auto pointer holding the global renderer
// ----------------------------------------------------------------------------

wxDECLARE_SCOPED_PTR(wxRendererNative, wxRendererPtrBase);
wxDEFINE_SCOPED_PTR(wxRendererNative, wxRendererPtrBase);

class wxRendererPtr : public wxRendererPtrBase
{
public:
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

    // return the global and unique wxRendererPtr
    static wxRendererPtr& Get()
    {
        static wxRendererPtr s_renderer;

        return s_renderer;
    }

private:
    wxRendererPtr() : wxRendererPtrBase(NULL) { m_initialized = false; }

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

#if wxUSE_DYNLIB_CLASS

// ----------------------------------------------------------------------------
// wxRendererFromDynLib: represents a renderer dynamically loaded from a DLL
// ----------------------------------------------------------------------------

class wxRendererFromDynLib : public wxDelegateRendererNative
{
public:
    // create the object wrapping the given renderer created from this DLL
    //
    // we take ownership of the pointer and will delete it (and also unload the
    // DLL) when we're deleted
    wxRendererFromDynLib(wxDynamicLibrary& dll, wxRendererNative *renderer)
        : wxDelegateRendererNative(*renderer),
          m_renderer(renderer),
          m_dllHandle(dll.Detach())
        {
        }

    virtual ~wxRendererFromDynLib()
    {
        delete m_renderer;
        wxDynamicLibrary::Unload(m_dllHandle);
    }

private:
    wxRendererNative *m_renderer;
    wxDllType m_dllHandle;
};

#endif // wxUSE_DYNLIB_CLASS

// ============================================================================
// wxRendererNative implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Managing the global renderer
// ----------------------------------------------------------------------------

/* static */
wxRendererNative& wxRendererNative::Get()
{
    wxRendererPtr& renderer = wxRendererPtr::Get();

    return renderer.IsOk() ? *renderer.get() : GetDefault();
}

/* static */
wxRendererNative *wxRendererNative::Set(wxRendererNative *rendererNew)
{
    wxRendererPtr& renderer = wxRendererPtr::Get();

    wxRendererNative *rendererOld = renderer.release();

    renderer.reset(rendererNew);

    return rendererOld;
}


// ----------------------------------------------------------------------------
// Dynamic renderers loading
// ----------------------------------------------------------------------------

#if wxUSE_DYNLIB_CLASS

/* static */
wxRendererNative *wxRendererNative::Load(const wxString& name)
{
    wxString fullname = wxDynamicLibrary::CanonicalizePluginName(name);

    wxDynamicLibrary dll(fullname);
    if ( !dll.IsLoaded() )
        return NULL;

    // each theme DLL must export a wxCreateRenderer() function with this
    // signature
    typedef wxRendererNative *(*wxCreateRenderer_t)();

    wxDYNLIB_FUNCTION(wxCreateRenderer_t, wxCreateRenderer, dll);
    if ( !pfnwxCreateRenderer )
        return NULL;

    wxRendererNative *renderer = (*pfnwxCreateRenderer)();
    if ( !renderer )
        return NULL;

    return new wxRendererFromDynLib(dll, renderer);
}

#endif // wxUSE_DYNLIB_CLASS

