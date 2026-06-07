///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/panel.h
// Purpose:     Ribbon-style container for a group of related tools / controls
// Author:      Peter Cawley
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_PANEL_H_
#define _WX_RIBBON_PANEL_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/bmpbndl.h"
#include "wx/ribbon/control.h"

enum wxRibbonPanelOption
{
    wxRIBBON_PANEL_NO_AUTO_MINIMISE = 1 << 0,
    wxRIBBON_PANEL_EXT_BUTTON       = 1 << 3,
    wxRIBBON_PANEL_MINIMISE_BUTTON  = 1 << 4,
    wxRIBBON_PANEL_STRETCH          = 1 << 5,
    wxRIBBON_PANEL_FLEXIBLE         = 1 << 6,

    wxRIBBON_PANEL_DEFAULT_STYLE    = 0
};

class WXDLLIMPEXP_RIBBON wxRibbonPanel : public wxRibbonControl
{
public:
    wxRibbonPanel();

    wxRibbonPanel(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxString& label = wxEmptyString,
                  const wxBitmapBundle& minimisedIcon = wxBitmapBundle(),
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxRIBBON_PANEL_DEFAULT_STYLE);

    virtual ~wxRibbonPanel();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxString& label = wxEmptyString,
                const wxBitmapBundle& icon = wxBitmapBundle(),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxRIBBON_PANEL_DEFAULT_STYLE);

    wxBitmap GetMinimisedIcon() { return m_minimisedIcon.GetBitmapFor(this); }
    const wxBitmapBundle& GetMinimisedIconBundle() const { return m_minimisedIcon; }
    bool IsMinimised() const;
    bool IsMinimised(wxSize atSize) const;
    bool IsHovered() const;
    bool IsExtButtonHovered() const;
    bool CanAutoMinimise() const;

    bool ShowExpanded();
    bool HideExpanded();

    void SetArtProvider(wxRibbonArtProvider* art) override;

    virtual bool Realize() override;
    virtual bool Layout() override;
    virtual wxSize GetMinSize() const override;

    virtual bool IsSizingContinuous() const override;

    virtual void AddChild(wxWindowBase *child) override;
    virtual void RemoveChild(wxWindowBase *child) override;

    virtual bool HasExtButton() const;

    wxRibbonPanel* GetExpandedDummy();
    wxRibbonPanel* GetExpandedPanel();

    // Finds the best width and height given the parent's width and height
    virtual wxSize GetBestSizeForParentSize(const wxSize& parentSize) const override;

    long GetFlags() { return m_flags; }

    void HideIfExpanded();

protected:
    virtual wxSize DoGetBestSize() const override;
    virtual wxSize GetPanelSizerBestSize() const;
    wxSize  GetPanelSizerMinSize() const;
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }
    wxSize GetMinNotMinimisedSize() const;

    virtual wxSize DoGetNextSmallerSize(wxOrientation direction,
                                      wxSize relativeTo) const override;
    virtual wxSize DoGetNextLargerSize(wxOrientation direction,
                                     wxSize relativeTo) const override;

    void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) override;
    void OnSize(wxSizeEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnMouseEnter(wxMouseEvent& evt);
    void OnMouseEnterChild(wxMouseEvent& evt);
    void OnMouseLeave(wxMouseEvent& evt);
    void OnMouseLeaveChild(wxMouseEvent& evt);
    void OnMouseClick(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnKillFocus(wxFocusEvent& evt);
    void OnChildKillFocus(wxFocusEvent& evt);
    void OnDPIChanged(wxDPIChangedEvent& evt);

    void TestPositionForHover(const wxPoint& pos);
    bool ShouldSendEventToDummy(wxEvent& evt);
    virtual bool TryAfter(wxEvent& evt) override;

    void CommonInit(const wxString& label, const wxBitmapBundle& icon, long style);
    static wxRect GetExpandedPosition(wxRect panel,
                                      wxSize expandedSize,
                                      wxDirection direction);

    wxBitmapBundle m_minimisedIcon;
    wxBitmap m_minimisedIconResized;
    wxSize m_smallestUnminimisedSize;
    wxSize m_minimisedSize;
    wxDirection m_preferredExpandDirection;
    wxRibbonPanel* m_expandedDummy;
    wxRibbonPanel* m_expandedPanel;
    wxWindow* m_childWithFocus;
    long m_flags;
    bool m_minimised;
    bool m_hovered;
    bool m_extButtonHovered;
    wxRect m_extButtonRect;

#ifndef SWIG
    wxDECLARE_CLASS(wxRibbonPanel);
    wxDECLARE_EVENT_TABLE();
#endif
};


class WXDLLIMPEXP_RIBBON wxRibbonPanelEvent : public wxCommandEvent
{
public:
    wxRibbonPanelEvent(wxEventType commandType = wxEVT_NULL,
                       int winId = 0,
                       wxRibbonPanel* panel = nullptr)
        : wxCommandEvent(commandType, winId)
        , m_panel(panel)
    {
    }

    wxRibbonPanelEvent(const wxRibbonPanelEvent& e) = default;
    wxNODISCARD wxEvent *Clone() const override { return new wxRibbonPanelEvent(*this); }

    wxRibbonPanel* GetPanel() { return m_panel; }
    void SetPanel(wxRibbonPanel* panel) { m_panel = panel; }

protected:
    wxRibbonPanel* m_panel;

#ifndef SWIG
private:
    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxRibbonPanelEvent);
#endif
};

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, wxRibbonPanelEvent);

typedef void (wxEvtHandler::*wxRibbonPanelEventFunction)(wxRibbonPanelEvent&);

#define wxRibbonPanelEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxRibbonPanelEventFunction, func)

#define EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, winid, wxRibbonPanelEventHandler(fn))
#else

// wxpython/swig event work
%constant wxEventType wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED;

%pythoncode {
    EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED = wx.PyEventBinder( wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, 1 )
}
#endif

// old wxEVT_COMMAND_* constants
#define wxEVT_COMMAND_RIBBONPANEL_EXTBUTTON_ACTIVATED   wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PANEL_H_
