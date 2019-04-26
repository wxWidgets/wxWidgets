/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/stackwalk.cpp
// Purpose:     wxStackWalker implementation for Win32
// Author:      Vadim Zeitlin
// Modified by: Artur Bac 2010-10-01 AMD64 Port
// Created:     2005-01-08
// Copyright:   (c) 2003-2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#if wxUSE_STACKWALKER

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif

#include "wx/stackwalk.h"

#include "wx/msw/debughlp.h"
#include "wx/msw/seh.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStackFrame
// ----------------------------------------------------------------------------

void wxStackFrame::OnGetName()
{
    if ( m_hasName )
        return;

    m_hasName = true;

    // get the name of the function for this stack frame entry
    if ( !wxDbgHelpDLL::CallSymFromAddr
                        (
                            ::GetCurrentProcess(),
                            GetSymAddr(),
                            &m_offset,
                            &m_name
                        ) )
    {
        wxDbgHelpDLL::LogError(wxT("SymFromAddr"));
    }
}

void wxStackFrame::OnGetLocation()
{
    if ( m_hasLocation )
        return;

    m_hasLocation = true;

    // get the source line for this stack frame entry ignoring possible errors
    // (it's normal that we don't have source info for some symbols, e.g. all
    // those from the system DLLs)
    wxDbgHelpDLL::CallSymGetLineFromAddr(::GetCurrentProcess(), GetSymAddr(),
                                         &m_filename, &m_line);
}

bool
wxStackFrame::GetParam(size_t n,
                       wxString *type,
                       wxString *name,
                       wxString *value) const
{
    if ( !DoGetParamCount() )
        ConstCast()->OnGetParam();

    if ( n >= DoGetParamCount() )
        return false;

    if ( type )
        *type = m_paramTypes[n];
    if ( name )
        *name = m_paramNames[n];
    if ( value )
        *value = m_paramValues[n];

    return true;
}

void wxStackFrame::OnParam(wxSYMBOL_INFO *pSymInfo)
{
    m_paramTypes.Add(wxEmptyString);
    m_paramNames.Add(pSymInfo->Name);

    wxString value;

    // if symbol information is corrupted and we crash, the exception is going
    // to be ignored when we're called from WalkFromException() because of the
    // exception handler there returning EXCEPTION_CONTINUE_EXECUTION, but we'd
    // be left in an inconsistent state, so deal with it explicitly here (even
    // if normally we should never crash, of course...)
#ifdef _CPPUNWIND
    try
#else
    wxSEH_TRY
#endif
    {
        // as it is a parameter (and not a global var), it is always offset by
        // the frame address
        DWORD_PTR pValue = m_addrFrame + pSymInfo->Address;
        value = wxDbgHelpDLL::DumpSymbol(pSymInfo, (void *)pValue);
    }
#ifdef _CPPUNWIND
    catch ( ... )
    {
    }
#else
    wxSEH_IGNORE
#endif

    m_paramValues.Add(value);
}

BOOL CALLBACK
EnumSymbolsProc(wxPSYMBOL_INFO pSymInfo, ULONG WXUNUSED(SymSize), PVOID data)
{
    wxStackFrame *frame = static_cast<wxStackFrame *>(data);

    // we're only interested in parameters
    if ( pSymInfo->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
    {
        frame->OnParam(pSymInfo);
    }

    // return true to continue enumeration, false would have stopped it
    return TRUE;
}

void wxStackFrame::OnGetParam()
{
    // use SymSetContext to get just the locals/params for this frame
    IMAGEHLP_STACK_FRAME imagehlpStackFrame;
    wxZeroMemory(imagehlpStackFrame);
    imagehlpStackFrame.InstructionOffset = GetSymAddr();
    if ( !wxDbgHelpDLL::SymSetContext
                        (
                            ::GetCurrentProcess(),
                            &imagehlpStackFrame,
                            0           // unused
                        ) )
    {
        // for symbols from kernel DLL we might not have access to their
        // address, this is not a real error
        if ( ::GetLastError() != ERROR_INVALID_ADDRESS )
        {
            wxDbgHelpDLL::LogError(wxT("SymSetContext"));
        }

        return;
    }

    if ( !wxDbgHelpDLL::CallSymEnumSymbols
                        (
                            ::GetCurrentProcess(),
                            0,                  // DLL base: use current context
                            EnumSymbolsProc,    // callback
                            this                // data to pass to it
                        ) )
    {
        wxDbgHelpDLL::LogError(wxT("SymEnumSymbols"));
    }
}


// ----------------------------------------------------------------------------
// wxStackWalker
// ----------------------------------------------------------------------------

void wxStackWalker::WalkFrom(const CONTEXT *pCtx, size_t skip, size_t maxDepth)
{
    if ( !wxDbgHelpDLL::Init() )
    {
        // don't log a user-visible error message here because the stack trace
        // is only needed for debugging/diagnostics anyhow and we shouldn't
        // confuse the user by complaining that we couldn't generate it
        wxLogDebug(wxT("Failed to get stack backtrace: %s"),
                   wxDbgHelpDLL::GetErrorMessage().c_str());
        return;
    }

    // according to MSDN, the first parameter should be just a unique value and
    // not process handle (although the parameter is prototyped as "HANDLE
    // hProcess") and actually it advises to use the process id and not handle,
    // but then we need to use the same value in StackWalk() call
    // below which should be a real handle... so this is what we use
    const HANDLE hProcess = ::GetCurrentProcess();

    if ( !wxDbgHelpDLL::CallSymInitialize
                        (
                            hProcess,
                            TRUE    // load symbols for all loaded modules
                        ) )
    {
        wxDbgHelpDLL::LogError(wxT("SymInitialize"));

        return;
    }

    CONTEXT ctx = *pCtx; // will be modified by StackWalk()

    DWORD dwMachineType;

    // initialize the initial frame: currently we can do it for x86 only
    STACKFRAME sf;
    wxZeroMemory(sf);

#if defined(_M_AMD64)
    sf.AddrPC.Offset       = ctx.Rip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = ctx.Rsp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = ctx.Rbp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_AMD64;
#elif defined(_M_ARM64)
    // TODO: Verify this code once Windows 10 for ARM64 is commercially available
    sf.AddrPC.Offset       = ctx.Pc;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = ctx.Sp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = ctx.Fp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_ARM64;
#elif  defined(_M_IX86)
    sf.AddrPC.Offset       = ctx.Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = ctx.Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = ctx.Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;
#else
    #error "Need to initialize STACKFRAME on non x86"
#endif // _M_IX86

    // iterate over all stack frames
    for ( size_t nLevel = 0; nLevel < maxDepth; nLevel++ )
    {
        // get the next stack frame
        if ( !wxDbgHelpDLL::StackWalk
                            (
                                dwMachineType,
                                hProcess,
                                ::GetCurrentThread(),
                                &sf,
                                &ctx,
                                NULL,       // read memory function (default)
                                wxDbgHelpDLL::SymFunctionTableAccess,
                                wxDbgHelpDLL::SymGetModuleBase,
                                NULL        // address translator for 16 bit
                            ) )
        {
            if ( ::GetLastError() )
                wxDbgHelpDLL::LogError(wxT("StackWalk"));

            break;
        }

        // don't show this frame itself in the output
        if ( nLevel >= skip )
        {
            wxStackFrame frame(nLevel - skip,
                               wxUIntToPtr(sf.AddrPC.Offset),
                               sf.AddrFrame.Offset);

            OnStackFrame(frame);
        }
    }

    if ( !wxDbgHelpDLL::SymCleanup(hProcess) )
    {
        wxDbgHelpDLL::LogError(wxT("SymCleanup"));
    }
}

void wxStackWalker::WalkFrom(const _EXCEPTION_POINTERS *ep, size_t skip, size_t maxDepth)
{
    WalkFrom(ep->ContextRecord, skip, maxDepth);
}

#if wxUSE_ON_FATAL_EXCEPTION

void wxStackWalker::WalkFromException(size_t maxDepth)
{
    extern EXCEPTION_POINTERS *wxGlobalSEInformation;

    wxCHECK_RET( wxGlobalSEInformation,
                 wxT("wxStackWalker::WalkFromException() can only be called from wxApp::OnFatalException()") );

    // don't skip any frames, the first one is where we crashed
    WalkFrom(wxGlobalSEInformation, 0, maxDepth);
}

#endif // wxUSE_ON_FATAL_EXCEPTION

#ifdef __VISUALC__
    #pragma warning(push)

    // "warning C4740: flow in or out of inline asm code suppresses global
    //  optimization"
    #pragma warning(disable: 4740)

    // "warning C4748: /GS can not protect parameters and local variables from
    //  local buffer overrun because optimizations are disabled in function"
    #pragma warning(disable: 4748)
#endif

void wxStackWalker::Walk(size_t skip, size_t maxDepth)
{
    // This code is based on frames.cpp from Edd Dawson's dbg library
    // (https://bitbucket.org/edd/dbg/) which is distributed under Boost
    // Software License.

    CONTEXT ctx;
#ifdef __WIN64__
    RtlCaptureContext(&ctx);
#else // Win32
    // RtlCaptureContext() is not implemented correctly for x86 and can even
    // crash when frame pointer is omitted, don't use it.
    wxZeroMemory(ctx);
    ctx.ContextFlags = CONTEXT_CONTROL;

    #ifdef __GNUC__
        DWORD regEip, regEsp, regEbp;

        asm volatile ("call 1f\n\t" "1: pop %0" : "=g"(regEip));
        asm volatile ("movl %%esp, %0" : "=g"(regEsp));
        asm volatile ("movl %%ebp, %0" : "=g"(regEbp));

        ctx.Eip = regEip;
        ctx.Esp = regEsp;
        ctx.Ebp = regEbp;
    #elif defined(__VISUALC__)
        __asm
        {
        Here:
          mov [ctx.Ebp], ebp
          mov [ctx.Esp], esp
          mov eax, [Here]
          mov [ctx.Eip], eax
        }
    #else
        #error Missing implementation of RtlCaptureContext()
    #endif
#endif // Win64/32

    WalkFrom(&ctx, skip, maxDepth);
}

#ifdef __VISUALC__
    #pragma warning(pop)
#endif

#endif // wxUSE_STACKWALKER

