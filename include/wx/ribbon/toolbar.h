///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/toolbar.h
// Purpose:     Ribbon-style tool bar
// Author:      Peter Cawley
// Created:     2009-07-06
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_TOOLBAR_H_
#define _WX_RIBBON_TOOLBAR_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/control.h"
#include "wx/ribbon/art.h"
#include "wx/bmpbndl.h"

class wxRibbonToolBarToolBase;
class wxRibbonToolBarToolGroup;
WX_DEFINE_USER_EXPORTED_ARRAY_PTR(wxRibbonToolBarToolGroup*, wxArrayRibbonToolBarToolGroup, class WXDLLIMPEXP_RIBBON);

enum wxRibbonToolBarToolState
{
    wxRIBBON_TOOLBAR_TOOL_FIRST             = 1 << 0,
    wxRIBBON_TOOLBAR_TOOL_LAST              = 1 << 1,
    wxRIBBON_TOOLBAR_TOOL_POSITION_MASK     = wxRIBBON_TOOLBAR_TOOL_FIRST | wxRIBBON_TOOLBAR_TOOL_LAST,

    wxRIBBON_TOOLBAR_TOOL_NORMAL_HOVERED    = 1 << 3,
    wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED  = 1 << 4,
    wxRIBBON_TOOLBAR_TOOL_HOVER_MASK        = wxRIBBON_TOOLBAR_TOOL_NORMAL_HOVERED | wxRIBBON_TOOLBAR_TOOL_DROPDOWN_HOVERED,
    wxRIBBON_TOOLBAR_TOOL_NORMAL_ACTIVE     = 1 << 5,
    wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE   = 1 << 6,
    wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK       = wxRIBBON_TOOLBAR_TOOL_NORMAL_ACTIVE | wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE,
    wxRIBBON_TOOLBAR_TOOL_DISABLED          = 1 << 7,
    wxRIBBON_TOOLBAR_TOOL_TOGGLED           = 1 << 8,
    wxRIBBON_TOOLBAR_TOOL_STATE_MASK        = 0x1F8
};


class WXDLLIMPEXP_RIBBON wxRibbonToolBar : public wxRibbonControl
{
public:
    wxRibbonToolBar();

    wxRibbonToolBar(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0);

    virtual ~wxRibbonToolBar();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    virtual wxRibbonToolBarToolBase* AddTool(
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString,
                wxRibbonButtonKind kind = wxRIBBON_BUTTON_NORMAL);

    virtual wxRibbonToolBarToolBase* AddDropdownTool(
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString = wxEmptyString);

    virtual wxRibbonToolBarToolBase* AddHybridTool(
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString = wxEmptyString);

    virtual wxRibbonToolBarToolBase* AddToggleTool(
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString = wxEmptyString);

    virtual wxRibbonToolBarToolBase* AddTool(
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxBitmapBundle& bitmapDisabled = wxBitmapBundle(),
                const wxString& helpString = wxEmptyString,
                wxRibbonButtonKind kind = wxRIBBON_BUTTON_NORMAL,
                wxObject* clientData = nullptr);

    virtual wxRibbonToolBarToolBase* AddSeparator();

    virtual wxRibbonToolBarToolBase* InsertTool(
                size_t pos,
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString,
                wxRibbonButtonKind kind = wxRIBBON_BUTTON_NORMAL);

    virtual wxRibbonToolBarToolBase* InsertDropdownTool(
                size_t pos,
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString = wxEmptyString);

    virtual wxRibbonToolBarToolBase* InsertHybridTool(
                size_t pos,
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString = wxEmptyString);

    virtual wxRibbonToolBarToolBase* InsertToggleTool(
                size_t pos,
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxString& helpString = wxEmptyString);

    virtual wxRibbonToolBarToolBase* InsertTool(
                size_t pos,
                int toolId,
                const wxBitmapBundle& bitmap,
                const wxBitmapBundle& bitmapDisabled = wxBitmapBundle(),
                const wxString& helpString = wxEmptyString,
                wxRibbonButtonKind kind = wxRIBBON_BUTTON_NORMAL,
                wxObject* clientData = nullptr);

    virtual wxRibbonToolBarToolBase* InsertSeparator(size_t pos);

    virtual void ClearTools();
    virtual bool DeleteTool(int toolId);
    virtual bool DeleteToolByPos(size_t pos);

    virtual wxRibbonToolBarToolBase* FindById(int toolId) const;
    virtual wxRibbonToolBarToolBase* GetToolByPos(size_t pos) const;
    virtual wxRibbonToolBarToolBase* GetToolByPos(wxCoord x, wxCoord y) const;
    virtual size_t GetToolCount() const;
    virtual int GetToolId(const wxRibbonToolBarToolBase* tool) const;
    virtual wxRibbonToolBarToolBase* GetActiveTool() const;


    virtual wxObject* GetToolClientData(int toolId) const;
    virtual bool GetToolEnabled(int toolId) const;
    virtual wxString GetToolHelpString(int toolId) const;
    virtual wxRibbonButtonKind GetToolKind(int toolId) const;
    virtual int GetToolPos(int toolId) const;
    virtual wxRect GetToolRect(int toolId) const;
    virtual bool GetToolState(int toolId) const;

    virtual bool Realize() override;
    virtual void SetRows(int nMin, int nMax = -1);

    virtual void SetToolClientData(int toolId, wxObject* clientData);
    virtual void SetToolDisabledBitmap(int toolId, const wxBitmapBundle &bitmap);
    virtual void SetToolHelpString(int toolId, const wxString& helpString);
    virtual void SetToolNormalBitmap(int toolId, const wxBitmapBundle &bitmap);

    virtual bool IsSizingContinuous() const override;

    virtual void EnableTool(int toolId, bool enable = true);
    virtual void ToggleTool(int toolId, bool checked);

    // Finds the best width and height given the parent's width and height
    virtual wxSize GetBestSizeForParentSize(const wxSize& parentSize) const override;

protected:
    friend class wxRibbonToolBarEvent;
    virtual wxSize DoGetBestSize() const override;
    wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    void OnEraseBackground(wxEraseEvent& evt);
    void OnMouseDown(wxMouseEvent& evt);
    void OnMouseEnter(wxMouseEvent& evt);
    void OnMouseLeave(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    void OnMouseUp(wxMouseEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnDPIChanged(wxDPIChangedEvent& evt);

    virtual wxSize DoGetNextSmallerSize(wxOrientation direction,
                                      wxSize relativeTo) const override;
    virtual wxSize DoGetNextLargerSize(wxOrientation direction,
                                     wxSize relativeTo) const override;

    void CommonInit(long style);
    void AppendGroup();
    wxRibbonToolBarToolGroup* InsertGroup(size_t pos);
    virtual void UpdateWindowUI(long flags) override;

    static wxBitmap MakeDisabledBitmap(const wxBitmap& original);

    wxArrayRibbonToolBarToolGroup m_groups;
    wxRibbonToolBarToolBase* m_hoverTool;
    wxRibbonToolBarToolBase* m_activeTool;
    wxSize* m_sizes;
    int m_nrowsMin;
    int m_nrowsMax;

#ifndef SWIG
    wxDECLARE_CLASS(wxRibbonToolBar);
    wxDECLARE_EVENT_TABLE();
#endif
};


class WXDLLIMPEXP_RIBBON wxRibbonToolBarEvent : public wxCommandEvent
{
public:
    wxRibbonToolBarEvent(wxEventType commandType = wxEVT_NULL,
                       int winId = 0,
                       wxRibbonToolBar* bar = nullptr)
        : wxCommandEvent(commandType, winId)
        , m_bar(bar)
    {
    }

    wxRibbonToolBarEvent(const wxRibbonToolBarEvent& e) = default;
    wxNODISCARD wxEvent *Clone() const override { return new wxRibbonToolBarEvent(*this); }

    wxRibbonToolBar* GetBar() { return m_bar; }
    void SetBar(wxRibbonToolBar* bar) { m_bar = bar; }
    bool PopupMenu(wxMenu* menu);

protected:
    wxRibbonToolBar* m_bar;

#ifndef SWIG
private:
    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxRibbonToolBarEvent);
#endif
};

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_RIBBONTOOLBAR_CLICKED, wxRibbonToolBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, wxRibbonToolBarEvent);

typedef void (wxEvtHandler::*wxRibbonToolBarEventFunction)(wxRibbonToolBarEvent&);

#define wxRibbonToolBarEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxRibbonToolBarEventFunction, func)

#define EVT_RIBBONTOOLBAR_CLICKED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_RIBBONTOOLBAR_CLICKED, winid, wxRibbonToolBarEventHandler(fn))
#define EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, winid, wxRibbonToolBarEventHandler(fn))
#else

// wxpython/swig event work
%constant wxEventType wxEVT_RIBBONTOOLBAR_CLICKED;
%constant wxEventType wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED;

%pythoncode {
    EVT_RIBBONTOOLBAR_CLICKED = wx.PyEventBinder( wxEVT_RIBBONTOOLBAR_CLICKED, 1 )
    EVT_RIBBONTOOLBAR_DROPDOWN_CLICKED = wx.PyEventBinder( wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED, 1 )
}
#endif

// old wxEVT_COMMAND_* constants
#define wxEVT_COMMAND_RIBBONTOOL_CLICKED            wxEVT_RIBBONTOOLBAR_CLICKED
#define wxEVT_COMMAND_RIBBONTOOL_DROPDOWN_CLICKED   wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_TOOLBAR_H_
