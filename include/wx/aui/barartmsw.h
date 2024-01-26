/////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/barartmsw.h
// Purpose:     Interface of wxAuiMSWToolBarArt
// Author:      Tobias Taschner
// Created:     2015-09-22
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUI_BARART_MSW_H_
#define _WX_AUI_BARART_MSW_H_

class WXDLLIMPEXP_AUI wxAuiMSWToolBarArt : public wxAuiGenericToolBarArt
{
public:
    wxAuiMSWToolBarArt();

    virtual wxAuiToolBarArt* Clone() override;

    virtual void DrawBackground(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& rect) override;

    virtual void DrawLabel(
        wxDC& dc,
        wxWindow* wnd,
        const wxAuiToolBarItem& item,
        const wxRect& rect) override;

    virtual void DrawButton(
        wxDC& dc,
        wxWindow* wnd,
        const wxAuiToolBarItem& item,
        const wxRect& rect) override;

    virtual void DrawDropDownButton(
        wxDC& dc,
        wxWindow* wnd,
        const wxAuiToolBarItem& item,
        const wxRect& rect) override;

    virtual void DrawControlLabel(
        wxDC& dc,
        wxWindow* wnd,
        const wxAuiToolBarItem& item,
        const wxRect& rect) override;

    virtual void DrawSeparator(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& rect) override;

    virtual void DrawGripper(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& rect) override;

    virtual void DrawOverflowButton(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& rect,
        int state) override;

    virtual wxSize GetLabelSize(
        wxDC& dc,
        wxWindow* wnd,
        const wxAuiToolBarItem& item) override;

    virtual wxSize GetToolSize(
        wxDC& dc,
        wxWindow* wnd,
        const wxAuiToolBarItem& item) override;

    virtual int GetElementSize(int element) override;
    virtual void SetElementSize(int elementId, int size) override;

    virtual int ShowDropDown(wxWindow* wnd,
        const wxAuiToolBarItemArray& items) override;

private:
    bool m_themed;
    wxSize m_buttonSize;
};

#endif // _WX_AUI_BARART_MSW_H_
