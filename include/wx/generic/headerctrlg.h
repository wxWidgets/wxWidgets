///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/headerctrlg.h
// Purpose:     Generic wxHeaderCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2008-12-01
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_HEADERCTRLG_H_
#define _WX_GENERIC_HEADERCTRLG_H_

#include "wx/event.h"
#include "wx/vector.h"

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

    // common part of all ctors
    void Init();

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouse(wxMouseEvent& event);

    // return the horizontal start position of the given column
    int GetColStart(unsigned int idx) const;

    // refresh the given column [only]
    void RefreshCol(unsigned int idx);

    // refresh all the controls starting from (and including) the given one
    void RefreshColsAfter(unsigned int idx);

    // return the column at the given position or -1 if it is beyond the
    // rightmost column and put true into onSeparator output parameter if the
    // position is near the divider at the right end of this column (notice
    // that this means that we return column 0 even if the position is over
    // column 1 but close enough to the divider separating it from column 0)
    int FindColumnAtPos(int x, bool& onSeparator) const;


    // number of columns in the control currently
    unsigned int m_numColumns;

    // index of the column under mouse or -1 if none
    unsigned int m_hover;

    // the horizontal scroll offset
    int m_scrollOffset;


    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxHeaderCtrl)
};

#endif // _WX_GENERIC_HEADERCTRLG_H_

