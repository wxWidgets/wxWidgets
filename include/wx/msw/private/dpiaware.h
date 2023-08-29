/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/dpiaware.h
// Purpose:     AutoSystemDpiAware class
// Author:      Maarten Bent
// Created:     2016-10-06
// Copyright:   (c) Maarten Bent
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_DPI_AWARE_H_
#define _WX_MSW_DPI_AWARE_H_

#ifndef WX_PRECOMP
    #include "wx/msw/missing.h"
#endif

#include "wx/dynlib.h"

namespace wxMSWImpl
{

// ----------------------------------------------------------------------------
// Temporarily change the DPI Awareness context to GDIScaled or System
// ----------------------------------------------------------------------------

class AutoSystemDpiAware
{
    #define WXDPI_AWARENESS_CONTEXT_UNAWARE           ((WXDPI_AWARENESS_CONTEXT)-1)
    #define WXDPI_AWARENESS_CONTEXT_SYSTEM_AWARE      ((WXDPI_AWARENESS_CONTEXT)-2)
    #define WXDPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED ((WXDPI_AWARENESS_CONTEXT)-5)
    typedef WXDPI_AWARENESS_CONTEXT
            (WINAPI *SetThreadDpiAwarenessContext_t)(WXDPI_AWARENESS_CONTEXT);

public:
    AutoSystemDpiAware()
        : m_prevContext(WXDPI_AWARENESS_CONTEXT_UNAWARE)
    {
        if ( ms_pfnSetThreadDpiAwarenessContext == (SetThreadDpiAwarenessContext_t)-1)
        {
            wxLoadedDLL dllUser32("user32.dll");
            wxDL_INIT_FUNC(ms_pfn, SetThreadDpiAwarenessContext, dllUser32);
        }

        if ( ms_pfnSetThreadDpiAwarenessContext )
        {
            m_prevContext = ms_pfnSetThreadDpiAwarenessContext(
                                    WXDPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);
            if ( !m_prevContext )
            {
                m_prevContext = ms_pfnSetThreadDpiAwarenessContext(
                                    WXDPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
        }

    }

    ~AutoSystemDpiAware()
    {
        if ( ms_pfnSetThreadDpiAwarenessContext )
        {
            ms_pfnSetThreadDpiAwarenessContext(m_prevContext);
        }
    }

private:
    WXDPI_AWARENESS_CONTEXT m_prevContext;

    // This static member is defined in src/msw/window.cpp.
    static SetThreadDpiAwarenessContext_t ms_pfnSetThreadDpiAwarenessContext;
};

} // namespace wxMSWImpl

#endif // _WX_MSW_DPI_AWARE_H_
