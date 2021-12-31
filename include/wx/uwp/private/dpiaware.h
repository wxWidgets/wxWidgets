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

#if wxUSE_DYNLIB_CLASS

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
        : m_prevContext(WXDPI_AWARENESS_CONTEXT_UNAWARE),
          m_pfnSetThreadDpiAwarenessContext((SetThreadDpiAwarenessContext_t)-1)
    {
        if ( m_pfnSetThreadDpiAwarenessContext == (SetThreadDpiAwarenessContext_t)-1)
        {
            wxLoadedDLL dllUser32("user32.dll");
            wxDL_INIT_FUNC(m_pfn, SetThreadDpiAwarenessContext, dllUser32);
        }

        if ( m_pfnSetThreadDpiAwarenessContext )
        {
            m_prevContext = m_pfnSetThreadDpiAwarenessContext(
                                    WXDPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);
            if ( !m_prevContext )
            {
                m_prevContext = m_pfnSetThreadDpiAwarenessContext(
                                    WXDPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
            }
        }

    }

    ~AutoSystemDpiAware()
    {
        if ( m_pfnSetThreadDpiAwarenessContext )
        {
            m_pfnSetThreadDpiAwarenessContext(m_prevContext);
        }
    }

private:
    WXDPI_AWARENESS_CONTEXT m_prevContext;

    SetThreadDpiAwarenessContext_t m_pfnSetThreadDpiAwarenessContext;
};

#else // !wxUSE_DYNLIB_CLASS

// Just a stub to avoid littering the code with wxUSE_DYNLIB_CLASS checks.
class AutoSystemDpiAware { };

#endif // wxUSE_DYNLIB_CLASS/!wxUSE_DYNLIB_CLASS

} // namespace wxMSWImpl

#endif // _WX_MSW_DPI_AWARE_H_
