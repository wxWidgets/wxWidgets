///////////////////////////////////////////////////////////////////////////////
// Name:        wx/scopeguard.h
// Purpose:     declares wxwxScopeGuard and related macros
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
    Acknowledgements: this header is heavily based on (well, almost the exact
    copy of) wxScopeGuard.h by Andrei Alexandrescu and Petru Marginean published
    in December 2000 issue of C/C++ Users Journal.
 */

#ifndef _WX_SCOPEGUARD_H_
#define _WX_SCOPEGUARD_H_

#include "wx/defs.h"

#include "wx/except.h"

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

namespace wxPrivate
{
    // in the original implementation this was a member template function of
    // ScopeGuardImplBase but gcc 2.8 which is still used for OS/2 doesn't
    // support member templates and so we must make it global
    template <typename ScopeGuardImpl>
    void OnScopeExit(ScopeGuardImpl& guard)
    {
        if ( !guard.WasDismissed() )
        {
            // we're called from ScopeGuardImpl dtor and so we must not throw
            wxTRY
            {
                guard.Execute();
            }
            wxCATCH_ALL(;) // do nothing, just eat the exception
        }
    }

    // just to avoid the warning about unused variables
    template <typename T>
    void Use(const T& WXUNUSED(t))
    {
    }
} // namespace wxPrivate

// ============================================================================
// wxScopeGuard for functions and functors
// ============================================================================

// ----------------------------------------------------------------------------
// wxScopeGuardImplBase: used by wxScopeGuardImpl[0..N] below
// ----------------------------------------------------------------------------

class wxScopeGuardImplBase
{
public:
    wxScopeGuardImplBase() : m_wasDismissed(false) { }

    void Dismiss() const { m_wasDismissed = true; }

    // for OnScopeExit() only we can't make it friend, unfortunately)!
    bool WasDismissed() const { return m_wasDismissed; }

protected:
    ~wxScopeGuardImplBase() { }

    wxScopeGuardImplBase(const wxScopeGuardImplBase& other)
        : m_wasDismissed(other.m_wasDismissed)
    {
        other.Dismiss();
    }

    // must be mutable for copy ctor to work
    mutable bool m_wasDismissed;

private:
    wxScopeGuardImplBase& operator=(const wxScopeGuardImplBase&);
};

// ----------------------------------------------------------------------------
// wxScopeGuardImpl0: scope guard for actions without parameters
// ----------------------------------------------------------------------------

template <typename F>
class wxScopeGuardImpl0 : public wxScopeGuardImplBase
{
public:
    static wxScopeGuardImpl0<F> MakeGuard(F fun)
    {
        return wxScopeGuardImpl0<F>(fun);
    }

    ~wxScopeGuardImpl0() { wxPrivate::OnScopeExit(*this); }

    void Execute() { m_fun(); }

protected:
    wxScopeGuardImpl0(F fun) : m_fun(fun) { }

    F m_fun;

    wxScopeGuardImpl0& operator=(const wxScopeGuardImpl0&);
};

template <typename F>
inline wxScopeGuardImpl0<F> wxMakeGuard(F fun)
{
    return wxScopeGuardImpl0<F>::MakeGuard(fun);
}

// ----------------------------------------------------------------------------
// wxScopeGuardImpl1: scope guard for actions with 1 parameter
// ----------------------------------------------------------------------------

template <typename F, typename P1>
class wxScopeGuardImpl1 : public wxScopeGuardImplBase
{
public:
    static wxScopeGuardImpl1<F, P1> MakeGuard(F fun, P1 p1)
    {
        return wxScopeGuardImpl1<F, P1>(fun, p1);
    }

    ~wxScopeGuardImpl1() { wxPrivate::OnScopeExit(*this); }

    void Execute() { m_fun(m_p1); }

protected:
    wxScopeGuardImpl1(F fun, P1 p1) : m_fun(fun), m_p1(p1) { }

    F m_fun;
    const P1 m_p1;

    wxScopeGuardImpl1& operator=(const wxScopeGuardImpl1&);
};

template <typename F, typename P1>
inline wxScopeGuardImpl1<F, P1> wxMakeGuard(F fun, P1 p1)
{
    return wxScopeGuardImpl1<F, P1>::MakeGuard(fun, p1);
}

// ----------------------------------------------------------------------------
// wxScopeGuardImpl2: scope guard for actions with 2 parameters
// ----------------------------------------------------------------------------

template <typename F, typename P1, typename P2>
class wxScopeGuardImpl2 : public wxScopeGuardImplBase
{
public:
    static wxScopeGuardImpl2<F, P1, P2> MakeGuard(F fun, P1 p1, P2 p2)
    {
        return wxScopeGuardImpl2<F, P1, P2>(fun, p1, p2);
    }

    ~wxScopeGuardImpl2() { wxPrivate::OnScopeExit(*this); }

    void Execute() { m_fun(m_p1, m_p2); }

protected:
    wxScopeGuardImpl2(F fun, P1 p1, P2 p2) : m_fun(fun), m_p1(p1), m_p2(p2) { }

    F m_fun;
    const P1 m_p1;
    const P2 m_p2;

    wxScopeGuardImpl2& operator=(const wxScopeGuardImpl2&);
};

template <typename F, typename P1, typename P2>
inline wxScopeGuardImpl2<F, P1, P2> wxMakeGuard(F fun, P1 p1, P2 p2)
{
    return wxScopeGuardImpl2<F, P1, P2>::MakeGuard(fun, p1, p2);
}

// ============================================================================
// wxScopeGuards for object methods
// ============================================================================

// ----------------------------------------------------------------------------
// wxObjScopeGuardImpl0
// ----------------------------------------------------------------------------

template <class Obj, typename MemFun>
class wxObjScopeGuardImpl0 : public wxScopeGuardImplBase
{
public:
    static wxObjScopeGuardImpl0<Obj, MemFun>
        MakeObjGuard(Obj& obj, MemFun memFun)
    {
        return wxObjScopeGuardImpl0<Obj, MemFun>(obj, memFun);
    }

    ~wxObjScopeGuardImpl0() { wxPrivate::OnScopeExit(*this); }

    void Execute() { (m_obj.*m_memfun)(); }

protected:
    wxObjScopeGuardImpl0(Obj& obj, MemFun memFun)
        : m_obj(obj), m_memfun(memFun) { }

    Obj& m_obj;
    MemFun m_memfun;
};

template <class Obj, typename MemFun>
inline wxObjScopeGuardImpl0<Obj, MemFun> wxMakeObjGuard(Obj& obj, MemFun memFun)
{
    return wxObjScopeGuardImpl0<Obj, MemFun>::MakeObjGuard(obj, memFun);
}

template <class Obj, typename MemFun, typename P1>
class wxObjScopeGuardImpl1 : public wxScopeGuardImplBase
{
public:
    static wxObjScopeGuardImpl1<Obj, MemFun, P1>
        MakeObjGuard(Obj& obj, MemFun memFun, P1 p1)
    {
        return wxObjScopeGuardImpl1<Obj, MemFun, P1>(obj, memFun, p1);
    }

    ~wxObjScopeGuardImpl1() { wxPrivate::OnScopeExit(*this); }

    void Execute() { (m_obj.*m_memfun)(m_p1); }

protected:
    wxObjScopeGuardImpl1(Obj& obj, MemFun memFun, P1 p1)
        : m_obj(obj), m_memfun(memFun), m_p1(p1) { }

    Obj& m_obj;
    MemFun m_memfun;
    const P1 m_p1;
};

template <class Obj, typename MemFun, typename P1>
inline wxObjScopeGuardImpl1<Obj, MemFun, P1>
wxMakeObjGuard(Obj& obj, MemFun memFun, P1 p1)
{
    return wxObjScopeGuardImpl1<Obj, MemFun, P1>::MakeObjGuard(obj, memFun, p1);
}

template <class Obj, typename MemFun, typename P1, typename P2>
class wxObjScopeGuardImpl2 : public wxScopeGuardImplBase
{
public:
    static wxObjScopeGuardImpl2<Obj, MemFun, P1, P2>
        MakeObjGuard(Obj& obj, MemFun memFun, P1 p1, P2 p2)
    {
        return wxObjScopeGuardImpl2<Obj, MemFun, P1, P2>(obj, memFun, p1, p2);
    }

    ~wxObjScopeGuardImpl2() { wxPrivate::OnScopeExit(*this); }

    void Execute() { (m_obj.*m_memfun)(m_p1, m_p2); }

protected:
    wxObjScopeGuardImpl2(Obj& obj, MemFun memFun, P1 p1, P2 p2)
        : m_obj(obj), m_memfun(memFun), m_p1(p1), m_p2(p2) { }

    Obj& m_obj;
    MemFun m_memfun;
    const P1 m_p1;
    const P2 m_p2;
};

template <class Obj, typename MemFun, typename P1, typename P2>
inline wxObjScopeGuardImpl2<Obj, MemFun, P1, P2>
wxMakeObjGuard(Obj& obj, MemFun memFun, P1 p1, P2 p2)
{
    return wxObjScopeGuardImpl2<Obj, MemFun, P1, P2>::
                                            MakeObjGuard(obj, memFun, p1, p2);
}

// ============================================================================
// public stuff
// ============================================================================

// wxScopeGuard is just a reference, see the explanation in CUJ article
typedef const wxScopeGuardImplBase& wxScopeGuard;

// when an unnamed scope guard is needed, the macros below may be used
//
// NB: the original code has a single (and much nicer) ON_BLOCK_EXIT macro
//     but this results in compiler warnings about unused variables and I
//     didn't find a way to work around this other than by having different
//     macros with different names
#define wxON_BLOCK_EXIT0(f) \
    wxScopeGuard wxMAKE_UNIQUE_NAME(scopeGuard) = wxMakeGuard(f); \
    wxPrivate::Use(wxMAKE_UNIQUE_NAME(scopeGuard))

#define wxON_BLOCK_EXIT_OBJ0(o, m) \
    wxScopeGuard wxMAKE_UNIQUE_NAME(scopeGuard) = wxMakeObjGuard(o, m); \
    wxPrivate::Use(wxMAKE_UNIQUE_NAME(scopeGuard))

#define wxON_BLOCK_EXIT1(f, p1) \
    wxScopeGuard wxMAKE_UNIQUE_NAME(scopeGuard) = wxMakeGuard(f, p1); \
    wxPrivate::Use(wxMAKE_UNIQUE_NAME(scopeGuard))

#define wxON_BLOCK_EXIT_OBJ1(o, m, p1) \
    wxScopeGuard wxMAKE_UNIQUE_NAME(scopeGuard) = wxMakeObjGuard(o, m, p1); \
    wxPrivate::Use(wxMAKE_UNIQUE_NAME(scopeGuard))

#define wxON_BLOCK_EXIT2(f, p1, p2) \
    wxScopeGuard wxMAKE_UNIQUE_NAME(scopeGuard) = wxMakeGuard(f, p1, p2); \
    wxPrivate::Use(wxMAKE_UNIQUE_NAME(scopeGuard))

#define wxON_BLOCK_EXIT_OBJ2(o, m, p1, p2) \
    wxScopeGuard wxMAKE_UNIQUE_NAME(scopeGuard) = wxMakeObjGuard(o, m, p1, p2); \
    wxPrivate::Use(wxMAKE_UNIQUE_NAME(scopeGuard))

#endif // _WX_SCOPEGUARD_H_

