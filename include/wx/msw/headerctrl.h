///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/headerctrl.h
// Purpose:     wxMSW native wxHeaderCtrl
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_HEADERCTRL_H_
#define _WX_MSW_HEADERCTRL_H_

#include "wx/compositewin.h"

class wxMSWHeaderCtrl;

// ----------------------------------------------------------------------------
// wxHeaderCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderCtrl : public wxCompositeWindow<wxHeaderCtrlBase>
{
public:
    wxHeaderCtrl()
    {
        Init();
    }

    wxHeaderCtrl(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxHD_DEFAULT_STYLE,
                 const wxString& name = wxHeaderCtrlNameStr)
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHD_DEFAULT_STYLE,
                const wxString& name = wxHeaderCtrlNameStr);

    // Window style handling.
    virtual void SetWindowStyleFlag(long style) wxOVERRIDE;

protected:
    // Override wxWindow methods which must be implemented by a new control.
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

private:
    // Implement base class pure virtuals.
    virtual void DoSetCount(unsigned int count) wxOVERRIDE;
    virtual unsigned int DoGetCount() const wxOVERRIDE;
    virtual void DoUpdate(unsigned int idx) wxOVERRIDE;

    virtual void DoScrollHorz(int dx) wxOVERRIDE;

    virtual void DoSetColumnsOrder(const wxArrayInt& order) wxOVERRIDE;
    virtual wxArrayInt DoGetColumnsOrder() const wxOVERRIDE;

    // Pure virtual method inherited from wxCompositeWindow.
    virtual wxWindowList GetCompositeWindowParts() const wxOVERRIDE;

    // Common part of all ctors.
    void Init();

    // Events.
    void OnSize(wxSizeEvent& event);

    // Style flag helper function.
    long ApplyHeaderReorderFlagToStyle(long style);


    // The native header control.
    wxMSWHeaderCtrl* m_nativeControl;
    friend class wxMSWHeaderCtrl;

    wxDECLARE_NO_COPY_CLASS(wxHeaderCtrl);
};

#endif // _WX_MSW_HEADERCTRL_H_

