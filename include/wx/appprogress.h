/////////////////////////////////////////////////////////////////////////////
// Name:        wx/appprogress.h
// Purpose:     wxAppProgressIndicator interface.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-09-05
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APPPROG_H_
#define _WX_APPPROG_H_

#include "wx/defs.h"

class WXDLLIMPEXP_CORE wxAppProgressIndicatorBase
{
public:
    wxAppProgressIndicatorBase() {}
    virtual ~wxAppProgressIndicatorBase() {}
    virtual void SetValue(int value) = 0;
    virtual void SetRange(int range) = 0;
    virtual void Pulse() = 0;
    virtual void Reset() = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxAppProgressIndicatorBase);
};

#if defined(__WXMSW__)
    #include "wx/msw/appprogress.h"
#endif

#endif  // _WX_APPPROG_H_
