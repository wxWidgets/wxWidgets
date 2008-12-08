///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/headerctrl.h
// Purpose:     wxMSW native wxHeaderCtrl
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_HEADERCTRL_H_
#define _WX_MSW_HEADERCTRL_H_

class WXDLLIMPEXP_FWD_CORE wxImageList;

// ----------------------------------------------------------------------------
// wxHeaderCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxHeaderCtrl : public wxHeaderCtrlBase
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

    virtual ~wxHeaderCtrl();


private:
    // implement base class pure virtuals
    virtual void DoSetCount(unsigned int count);
    virtual unsigned int DoGetCount() const;
    virtual void DoUpdate(unsigned int idx);

    virtual void DoScrollHorz(int dx);

    // override wxWindow methods which must be implemented by a new control
    virtual wxSize DoGetBestSize() const;

    // override MSW-specific methods needed for new control
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    // common part of all ctors
    void Init();

    // wrapper around Header_{Set,Insert}Item(): either appends the item to the
    // end or modifies the existing item by copying information from
    // GetColumn(idx) to it
    enum Operation { Set, Insert };
    void DoSetOrInsertItem(Operation oper, unsigned int idx);

    // get the event type corresponding to a click or double click event
    // (depending on dblclk value) with the specified (using MSW convention)
    // mouse button
    wxEventType GetClickEventType(bool dblclk, int button);


    // the image list: initially NULL, created on demand
    wxImageList *m_imageList;

    DECLARE_NO_COPY_CLASS(wxHeaderCtrl)
};

#endif // _WX_MSW_HEADERCTRL_H_

