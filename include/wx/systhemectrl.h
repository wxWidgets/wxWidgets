/////////////////////////////////////////////////////////////////////////////
// Name:        wx/systhemectrl.h
// Purpose:     Class to make controls appear in the systems theme
// Author:      Tobias Taschner
// Created:     2014-08-14
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYSTHEMECTRL_H
#define _WX_SYSTHEMECTRL_H

#include "wx/defs.h"

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #define wxHAS_SYSTEM_THEMED_CONTROL
#endif

class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_CORE wxSystemThemedControlBase
{
public:
    wxSystemThemedControlBase() { }

    virtual ~wxSystemThemedControlBase() { }

protected:
    // This method is virtual and can be overridden, e.g. composite controls do
    // it to enable the system theme for all of their parts.
    virtual void DoEnableSystemTheme
#ifdef wxHAS_SYSTEM_THEMED_CONTROL
    // Only __WXMSW__ has a non-trivial implementation currently.
    (bool enable, wxWindow* window);
#else
    (bool WXUNUSED(enable), wxWindow* WXUNUSED(window)) { }
#endif // wxHAS_SYSTEM_THEMED_CONTROL

    wxDECLARE_NO_COPY_CLASS(wxSystemThemedControlBase);
};

// This class used CRTP, i.e. it should be instantiated for the real base class
// and inherited from.
template <class C>
class wxSystemThemedControl : public C,
                              public wxSystemThemedControlBase
{
public:
    wxSystemThemedControl() { }

    void EnableSystemTheme(bool enable = true)
    {
        DoEnableSystemTheme(enable, this);
    }

protected:
    wxDECLARE_NO_COPY_TEMPLATE_CLASS(wxSystemThemedControl, C);
};

#endif // _WX_SYSTHEMECTRL_H
