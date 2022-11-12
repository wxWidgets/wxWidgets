/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/statbox.h
// Purpose:     wxStaticBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STATBOX_H_
#define _WX_MSW_STATBOX_H_

#include "wx/compositewin.h"

// Group box
class WXDLLIMPEXP_CORE wxStaticBox : public wxCompositeWindowSettersOnly<wxStaticBoxBase>
{
public:
    wxStaticBox()
        : wxCompositeWindowSettersOnly<wxStaticBoxBase>()
    {
    }

    wxStaticBox(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr))
        : wxCompositeWindowSettersOnly<wxStaticBoxBase>()
    {
        Create(parent, id, label, pos, size, style, name);
    }

    wxStaticBox(wxWindow* parent, wxWindowID id,
                wxWindow* label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxASCII_STR(wxStaticBoxNameStr))
        : wxCompositeWindowSettersOnly<wxStaticBoxBase>()
    {
        Create(parent, id, label, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
                wxWindow* label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    /// Implementation only
    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const override;

    virtual bool SetBackgroundColour(const wxColour& colour) override;
    virtual bool SetForegroundColour(const wxColour& colour) override;
    virtual bool SetFont(const wxFont& font) override;

    virtual void SetLabel(const wxString& label) override;

    virtual WXDWORD MSWGetStyle(long style, WXDWORD *exstyle) const override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

protected:
    virtual wxSize DoGetBestSize() const override;

public:
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;

protected:
    virtual wxWindowList GetCompositeWindowParts() const override;

private:
    void PositionLabelWindow();

    using base_type = wxCompositeWindowSettersOnly<wxStaticBoxBase>;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticBox);
};

// Indicate that we have the ctor overload taking wxWindow as label.
#define wxHAS_WINDOW_LABEL_IN_STATIC_BOX

#endif // _WX_MSW_STATBOX_H_

