/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/appprogress.h
// Purpose:     wxAppProgressIndicator interface.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-09-05
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_APPPROG_H_
#define _WX_MSW_APPPROG_H_

#include <memory>

struct wxMSWAppProgressState;

class WXDLLIMPEXP_CORE wxAppProgressIndicator
    : public wxAppProgressIndicatorBase
{
public:
    wxAppProgressIndicator(wxWindow* parent = nullptr, int maxValue = 100);
    virtual ~wxAppProgressIndicator();

    virtual bool IsAvailable() const override;

    virtual void SetValue(int value) override;
    virtual void SetRange(int range) override;
    virtual void Pulse() override;
    virtual void Reset() override;

private:
    // The implementation owns weak TLW identities and one exact-generation
    // taskbar controller per target. Keeping this out of the public header also
    // prevents a raw wxWindow/HWND from becoming accidental persistent state.
    // shared_ptr is intentional: a native taskbar seam can synchronously call
    // application code which destroys this wrapper. The operation already on
    // the stack must retain the detached sidecar long enough to observe its
    // retired generation without touching the deleted wrapper.
    mutable std::shared_ptr<wxMSWAppProgressState> m_state;

    wxDECLARE_NO_COPY_CLASS(wxAppProgressIndicator);
};

#endif  // _WX_MSW_APPPROG_H_
