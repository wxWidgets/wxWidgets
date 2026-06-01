/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/scrolbar.h
// Purpose:     wxWinUI wxScrollBar declaration (WinUI ScrollBar)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_SCROLBAR_H_
#define _WX_WINUI_SCROLBAR_H_

#include <memory>

class wxWinUIScrollBarImpl;

class WXDLLIMPEXP_CORE wxScrollBar : public wxScrollBarBase
{
public:
    wxScrollBar();
    wxScrollBar(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxScrollBarNameStr));
    ~wxScrollBar() override;

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxScrollBarNameStr));

    int GetThumbPosition() const override { return m_position; }
    int GetThumbSize() const override { return m_thumbSize; }
    int GetPageSize() const override { return m_pageSize; }
    int GetRange() const override { return m_range; }

    void SetThumbPosition(int viewStart) override;
    void SetScrollbar(int position, int thumbSize,
                      int range, int pageSize,
                      bool refresh = true) override;

protected:
    wxSize DoGetBestSize() const override;

    void ApplyToPeer();
    void OnPeerScroll(int newValue, int eventType);

    std::unique_ptr<wxWinUIScrollBarImpl> m_winui;
    int m_position = 0;
    int m_thumbSize = 1;
    int m_range = 0;
    int m_pageSize = 1;
    bool m_updating = false;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxScrollBar);
};

#endif // _WX_WINUI_SCROLBAR_H_
