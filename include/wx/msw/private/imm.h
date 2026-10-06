///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/imm.h
// Purpose:     Helpers for using Windows Input Method Manager API.
// Author:      Vadim Zeitlin
// Created:     2026-10-05
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_IMM_H_
#define _WX_MSW_PRIVATE_IMM_H_

#include "wx/defs.h"
#include "wx/msw/wrapwin.h"

#include <imm.h>

// IMM functions loaded dynamically to avoid having to link with imm32.lib.
//
// This class is implemented in src/msw/window.cpp.
class WXDLLIMPEXP_CORE wxIMMFunctions
{
public:
    // Return the global object, IsOk() must be checked before using it.
    static const wxIMMFunctions& Get();

    bool IsOk() const { return m_ok; }

    typedef BOOL (WINAPI *ImmAssociateContextEx_t)(HWND, HIMC, DWORD);
    typedef HIMC (WINAPI *ImmGetContext_t)(HWND);
    typedef BOOL (WINAPI *ImmGetOpenStatus_t)(HIMC);
    typedef BOOL (WINAPI *ImmReleaseContext_t)(HWND, HIMC);
    typedef BOOL (WINAPI *ImmSetCompositionFontW_t)(HIMC, LPLOGFONTW);
    typedef BOOL (WINAPI *ImmSetCompositionWindow_t)(HIMC, LPCOMPOSITIONFORM);
    typedef BOOL (WINAPI *ImmSetCandidateWindow_t)(HIMC, LPCANDIDATEFORM);

    ImmAssociateContextEx_t AssociateContextEx = nullptr;
    ImmGetContext_t GetContext = nullptr;
    ImmGetOpenStatus_t GetOpenStatus = nullptr;
    ImmReleaseContext_t ReleaseContext = nullptr;
    ImmSetCompositionFontW_t SetCompositionFontW = nullptr;
    ImmSetCompositionWindow_t SetCompositionWindow = nullptr;
    ImmSetCandidateWindow_t SetCandidateWindow = nullptr;

private:
    wxIMMFunctions();

    bool m_ok = false;

    wxDECLARE_NO_COPY_CLASS(wxIMMFunctions);
};

// RAII helper acquiring and releasing the input method context.
//
// The context is null if IMM functions are not available, so checking that
// it is non-null is sufficient for using wxIMMFunctions with it.
class wxIMCContext
{
public:
    explicit wxIMCContext(HWND hwnd)
        : m_hwnd(hwnd),
          m_hIMC(wxIMMFunctions::Get().IsOk()
                    ? wxIMMFunctions::Get().GetContext(hwnd)
                    : nullptr)
    {
    }

    operator HIMC() const { return m_hIMC; }

    ~wxIMCContext()
    {
        if ( m_hIMC )
            wxIMMFunctions::Get().ReleaseContext(m_hwnd, m_hIMC);
    }

private:
    const HWND m_hwnd;
    const HIMC m_hIMC;

    wxDECLARE_NO_COPY_CLASS(wxIMCContext);
};

#endif // _WX_MSW_PRIVATE_IMM_H_
