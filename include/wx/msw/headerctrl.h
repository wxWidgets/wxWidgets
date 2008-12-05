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
    virtual unsigned int DoGetCount() const;
    virtual void DoInsert(const wxHeaderColumn& col, unsigned int idx);
    virtual void DoDelete(unsigned int idx);
    virtual void DoShowColumn(unsigned int idx, bool show);
    virtual void DoShowSortIndicator(unsigned int idx, int sortOrder);

    // override wxWindow methods which must be implemented by a new control
    virtual wxSize DoGetBestSize() const;

    // override MSW-specific methods needed for new control
    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const;

    // common part of all ctors
    void Init();

    // the image list: initially NULL, created on demand
    wxImageList *m_imageList;

    DECLARE_NO_COPY_CLASS(wxHeaderCtrl)
};

#endif // _WX_MSW_HEADERCTRL_H_

