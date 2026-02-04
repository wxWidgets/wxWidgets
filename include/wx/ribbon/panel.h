///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/panel.h
// Purpose:     Ribbon-style container for a group of related tools / controls
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_PANEL_H_
#define _WX_RIBBON_PANEL_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/bitmap.h"
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
                  const wxBitmap& minimised_icon = wxNullBitmap,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxRIBBON_PANEL_DEFAULT_STYLE);

    virtual ~wxRibbonPanel();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxString& label = wxEmptyString,
                const wxBitmap& icon = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxRIBBON_PANEL_DEFAULT_STYLE);

    wxBitmap& GetMinimisedIcon() {return m_minimised_icon;}
    const wxBitmap& GetMinimisedIcon() const {return m_minimised_icon;}
    // Bricsys change
    void setMinimisedIcon(const wxBitmap& icon) { m_minimised_icon = icon; }
    // End Bricsys change
    bool IsMinimised() const;
    bool IsMinimised(wxSize at_size) const;
    bool IsHovered() const;
    bool IsExtButtonHovered() const;
    // Bricsys change
    bool IsLabelHovered() const;
    // End Bricsys change
    bool CanAutoMinimise() const;

    bool ShowExpanded();
    bool HideExpanded();

    void SetArtProvider(wxRibbonArtProvider* art) wxOVERRIDE;

    virtual bool Realize() wxOVERRIDE;
    virtual bool Layout() wxOVERRIDE;
    virtual wxSize GetMinSize() const wxOVERRIDE;
    // Bricsys change
    // Reset panel size in case children size has changed
    virtual void resetPanelSize() { m_smallest_unminimised_size = wxDefaultSize; }
    // End Bricsys change

    virtual bool IsSizingContinuous() const wxOVERRIDE;

    virtual void AddChild(wxWindowBase *child) wxOVERRIDE;
    virtual void RemoveChild(wxWindowBase *child) wxOVERRIDE;

    virtual bool HasExtButton() const;
    // Bricsys change
    virtual bool HasSlideOutPanel() const { return m_hasSlideOutPanel; }
    virtual void SetSlideOutPanel(bool hasSlideOut) { m_hasSlideOutPanel = hasSlideOut; }
    // End Bricsys change

    wxRibbonPanel* GetExpandedDummy();
    wxRibbonPanel* GetExpandedPanel();

    // Finds the best width and height given the parent's width and height
    virtual wxSize GetBestSizeForParentSize(const wxSize& parentSize) const wxOVERRIDE;

    long GetFlags() { return m_flags; }

    void HideIfExpanded();

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;
    virtual wxSize GetPanelSizerBestSize() const;
    wxSize  GetPanelSizerMinSize() const;
    wxBorder GetDefaultBorder() const wxOVERRIDE { return wxBORDER_NONE; }
    wxSize GetMinNotMinimisedSize() const;

    virtual wxSize DoGetNextSmallerSize(wxOrientation direction,
                                      wxSize relative_to) const wxOVERRIDE;
    virtual wxSize DoGetNextLargerSize(wxOrientation direction,
                                     wxSize relative_to) const wxOVERRIDE;

    void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
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
    // Bricsys change
    void OnMouseUp(wxMouseEvent& evt);
    // End Bricsys change

    void TestPositionForHover(const wxPoint& pos);
    bool ShouldSendEventToDummy(wxEvent& evt);
    virtual bool TryAfter(wxEvent& evt) wxOVERRIDE;

    void CommonInit(const wxString& label, const wxBitmap& icon, long style);
    static wxRect GetExpandedPosition(wxRect panel,
                                      wxSize expanded_size,
                                      wxDirection direction);

    wxBitmap m_minimised_icon;
    wxBitmap m_minimised_icon_resized;
    wxSize m_smallest_unminimised_size;
    wxSize m_minimised_size;
    wxDirection m_preferred_expand_direction;
    wxRibbonPanel* m_expanded_dummy;
    wxRibbonPanel* m_expanded_panel;
    wxWindow* m_child_with_focus;
    long m_flags;
    bool m_minimised;
    bool m_hovered;
    bool m_ext_button_hovered;
    wxRect m_ext_button_rect;
    // Bricsys change
    bool m_hasSlideOutPanel;
    wxRect m_panel_label_rect;
    bool m_panel_label_hovered;
    // End Bricsys change

#ifndef SWIG
    wxDECLARE_CLASS(wxRibbonPanel);
    wxDECLARE_EVENT_TABLE();
#endif
};


class WXDLLIMPEXP_RIBBON wxRibbonPanelEvent : public wxCommandEvent
{
public:
    wxRibbonPanelEvent(wxEventType command_type = wxEVT_NULL,
                       int win_id = 0,
                       wxRibbonPanel* panel = NULL)
        : wxCommandEvent(command_type, win_id)
        , m_panel(panel)
    {
    }
    wxEvent *Clone() const wxOVERRIDE { return new wxRibbonPanelEvent(*this); }

    wxRibbonPanel* GetPanel() {return m_panel;}
    void SetPanel(wxRibbonPanel* panel) {m_panel = panel;}

protected:
    wxRibbonPanel* m_panel;

#ifndef SWIG
private:
    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxRibbonPanelEvent);
#endif
};

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, wxRibbonPanelEvent);
// Bricsys change
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_RIBBONPANEL_LABEL_ACTIVATED, wxRibbonPanelEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_RIBBONPANEL_EXPANDED_SHOWN, wxRibbonPanelEvent);
// End Bricsys change

typedef void (wxEvtHandler::*wxRibbonPanelEventFunction)(wxRibbonPanelEvent&);

#define wxRibbonPanelEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxRibbonPanelEventFunction, func)

#define EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, winid, wxRibbonPanelEventHandler(fn))
// Bricsys change
#define EVT_RIBBONPANEL_LABEL_ACTIVATED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_RIBBONPANEL_LABEL_ACTIVATED, winid, wxRibbonPanelEventHandler(fn))
#define EVT_RIBBONPANEL_EXPANDED_SHOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_RIBBONPANEL_EXPANDED_SHOWN, winid, wxRibbonPanelEventHandler(fn))
// End Bricsys change
#else

// wxpython/swig event work
%constant wxEventType wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED;

%pythoncode {
    EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED = wx.PyEventBinder( wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED, 1 )
}

// Bricsys change
// wxpython/swig event work
%constant wxEventType wxEVT_RIBBONPANEL_LABEL_ACTIVATED;

%pythoncode{
    EVT_RIBBONPANEL_LABEL_ACTIVATED = wx.PyEventBinder(wxEVT_RIBBONPANEL_LABEL_ACTIVATED, 1)
}

%constant wxEventType wxEVT_RIBBONPANEL_EXPANDED_SHOWN;

%pythoncode{
    EVT_RIBBONPANEL_EXPANDED_SHOWN = wx.PyEventBinder(wxEVT_RIBBONPANEL_EXPANDED_SHOWN, 1)
}
// End Bricsys change
#endif

// old wxEVT_COMMAND_* constants
#define wxEVT_COMMAND_RIBBONPANEL_EXTBUTTON_ACTIVATED   wxEVT_RIBBONPANEL_EXTBUTTON_ACTIVATED

// Bricsys change
#define wxEVT_COMMAND_RIBBONPANEL_LABEL_ACTIVATED   wxEVT_RIBBONPANEL_LABEL_ACTIVATED
#define wxEVT_COMMAND_RIBBONPANEL_EXPANDED_SHOWN   wxEVT_RIBBONPANEL_EXPANDED_SHOWN
// End Bricsys change

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PANEL_H_
