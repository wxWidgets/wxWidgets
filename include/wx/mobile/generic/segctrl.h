/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/segctrl.h
// Purpose:     wxMoSegmentedCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_SEGCTRL_H_
#define _WX_MOBILE_GENERIC_SEGCTRL_H_

class WXDLLIMPEXP_CORE wxImageList;

#include "wx/mobile/tabctrl.h"

/**
    @class wxMoSegmentedCtrl

    A class with the same API as wxMoTabCtrl, but displaying a space-efficient row of buttons.

    @category{wxMobile}
 */

class WXDLLEXPORT wxMoSegmentedCtrl: public wxMoTabCtrl
{
  DECLARE_DYNAMIC_CLASS(wxMoSegmentedCtrl)
 public:
    /// Default constructor.
    wxMoSegmentedCtrl();

    /// Constructor.
    wxMoSegmentedCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxTAB_TEXT, const wxString& name = wxT("tabCtrl"))
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT, const wxString& name = wxT("segmentedCtrl"));

    void Init();

// Implementation

    virtual void OnInsertItem(wxMoBarButton* button);

    void OnPaint(wxPaintEvent& event);

    bool SendCommand(wxEventType eventType, int selection);

protected:

    virtual wxSize DoGetBestSize() const;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxMoSegmentedCtrl)
};

#endif
    // _WX_MOBILE_GENERIC_SEGCTRL_H_
