/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/dpiaware.h
// Purpose:     AutoSystemDpiAware class
// Author:      Maarten Bent
// Modified by:
// Created:     10/6/2016
// Copyright:   (c) Maarten Bent
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DPI_AWARE_H_
#define _WX_MSW_DPI_AWARE_H_

#ifndef WX_PRECOMP
    #include "wx/msw/missing.h"
#endif

#include "wx/dynlib.h"

// ----------------------------------------------------------------------------
// Temporarily change the DPI Awareness context to System
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE AutoSystemDpiAware
{
    typedef DPI_AWARENESS_CONTEXT(WINAPI *SetThreadDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT);

public:
    AutoSystemDpiAware()
        : m_prevContext(DPI_AWARENESS_CONTEXT_UNAWARE)
        , m_pfnSetThreadDpiAwarenessContext((SetThreadDpiAwarenessContext_t)-1)
    {
#if wxUSE_DYNLIB_CLASS
        wxLoadedDLL dllUser32(wxT("User32.dll"));

        if (m_pfnSetThreadDpiAwarenessContext == (SetThreadDpiAwarenessContext_t)-1)
        {
            wxDL_INIT_FUNC(m_pfn, SetThreadDpiAwarenessContext, dllUser32);
        }

        if (!m_pfnSetThreadDpiAwarenessContext)
        {
            return;
        }

        m_prevContext = m_pfnSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
#endif
    }

    ~AutoSystemDpiAware()
    {
#if wxUSE_DYNLIB_CLASS
        if (m_pfnSetThreadDpiAwarenessContext)
        {
            m_pfnSetThreadDpiAwarenessContext(m_prevContext);
        }
#endif
    }

private:
    DPI_AWARENESS_CONTEXT m_prevContext;

    SetThreadDpiAwarenessContext_t m_pfnSetThreadDpiAwarenessContext;
};

#endif // _WX_MSW_DPI_AWARE_H_
