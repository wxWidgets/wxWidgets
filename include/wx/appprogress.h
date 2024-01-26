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
    wxAppProgressIndicatorBase() = default;
    virtual ~wxAppProgressIndicatorBase() = default;

    virtual bool IsAvailable() const = 0;

    virtual void SetValue(int value) = 0;
    virtual void SetRange(int range) = 0;
    virtual void Pulse() = 0;
    virtual void Reset() = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxAppProgressIndicatorBase);
};

#if defined(__WXMSW__) && wxUSE_TASKBARBUTTON
    #include "wx/msw/appprogress.h"
#elif defined(__WXOSX_COCOA__)
    #include "wx/osx/appprogress.h"
#else
    class wxAppProgressIndicator : public wxAppProgressIndicatorBase
    {
    public:
        wxAppProgressIndicator(wxWindow* WXUNUSED(parent) = nullptr,
                               int WXUNUSED(maxValue) = 100)
        {
        }

        virtual bool IsAvailable() const override { return false; }

        virtual void SetValue(int WXUNUSED(value)) override { }
        virtual void SetRange(int WXUNUSED(range)) override { }
        virtual void Pulse() override { }
        virtual void Reset() override { }
    };
#endif

#endif  // _WX_APPPROG_H_
