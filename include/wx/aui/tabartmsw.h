/////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/tabartmsw.h
// Purpose:     wxAuiMSWTabArt declaration
// Author:      Tobias Taschner
// Created:     2015-09-26
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUI_TABARTMSW_H_
#define _WX_AUI_TABARTMSW_H_

#include "wx/aui/tabart.h"

#if wxUSE_AUI

class WXDLLIMPEXP_AUI wxAuiMSWTabArt : public wxAuiGenericTabArt
{

public:

    wxAuiMSWTabArt();
    virtual ~wxAuiMSWTabArt();

    wxNODISCARD wxAuiTabArt* Clone() override;

    void DrawBorder(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& rect) override;

    void DrawBackground(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& rect) override;

    int DrawPageTab(wxDC& dc,
        wxWindow* wnd,
        wxAuiNotebookPage& page,
        const wxRect& rect) override;

    void DrawButton(
        wxDC& dc,
        wxWindow* wnd,
        const wxRect& inRect,
        int bitmapId,
        int buttonState,
        int orientation,
        wxRect* outRect) override;

    int GetIndentSize() override;

    int GetBorderWidth(
        wxWindow* wnd) override;

    int GetAdditionalBorderSpace(
        wxWindow* wnd) override;

    wxSize GetPageTabSize(
        wxReadOnlyDC& dc,
        wxWindow* wnd,
        const wxAuiNotebookPage& page,
        int* xExtent) override;

    int ShowDropDown(
        wxWindow* wnd,
        const wxAuiNotebookPageArray& items,
        int activeIdx) override;

    int GetBestTabCtrlSize(wxWindow* wnd,
        const wxAuiNotebookPageArray& pages,
        const wxSize& requiredBmpSize) override;

    void UpdateDpi() override;

private:
    bool m_themed;
    wxSize m_closeBtnSize;
    wxSize m_tabSize;
    int m_maxTabHeight;

    void InitSizes(wxWindow* wnd, wxReadOnlyDC& dc);

    bool IsThemed() const;
};

#endif // wxUSE_AUI

#endif // _WX_AUI_TABARTMSW_H_
