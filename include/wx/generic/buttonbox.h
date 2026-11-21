/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/buttonbox.h
// Purpose:     wxGenericButtonBox: wxToggleButton based segmented control
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_BUTTONBOX_H_
#define _WX_GENERIC_BUTTONBOX_H_

#include <vector>

class WXDLLIMPEXP_FWD_CORE wxToggleButton;
class WXDLLIMPEXP_FWD_CORE wxBoxSizer;

class WXDLLIMPEXP_CORE wxGenericButtonBox : public wxButtonBoxBase
{
public:
    wxGenericButtonBox() = default;
    wxGenericButtonBox(wxWindow* parent, wxWindowID id,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxBB_DEFAULT_STYLE,
                       const wxString& name = wxASCII_STR(wxButtonBoxNameStr))
    {
        Create(parent, id, pos, size, style, name);
    }

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxButtonBoxNameStr));

    virtual size_t GetCount() const override { return m_segments.size(); }
    virtual void Insert(size_t pos, const wxString& label,
                        const wxBitmapBundle& bitmap = wxBitmapBundle()) override;
    virtual void Delete(size_t pos) override;
    virtual void Clear() override;

    virtual int  GetSelection() const override;
    virtual void SetSelection(int n) override;
    virtual bool IsSelected(size_t n) const override;
    virtual void SetSelected(size_t n, bool selected = true) override;

    virtual void SetItemLabel(size_t n, const wxString& label) override;
    virtual wxString GetItemLabel(size_t n) const override;
    virtual void SetItemBitmap(size_t n, const wxBitmapBundle& bitmap) override;
    virtual wxBitmapBundle GetItemBitmap(size_t n) const override;
    virtual void SetItemWidth(size_t n, int width) override;
    virtual int  GetItemWidth(size_t n) const override;
    virtual void EnableItem(size_t n, bool enable = true) override;
    virtual bool IsItemEnabled(size_t n) const override;
    virtual void SetItemToolTip(size_t n, const wxString& tip) override;
    virtual wxString GetItemToolTip(size_t n) const override;

protected:
    virtual wxSize DoGetBestSize() const override;

private:
    struct Segment
    {
        wxToggleButton* button;
        wxBitmapBundle  bitmap;
        wxString        label;
        wxString        tooltip;
    };

    int  FindSegment(const wxObject* button) const;
    wxToggleButton* CreateButton(const Segment& seg);
    void ReplaceButton(size_t n);       // re-create after label/bitmap change
    void OnToggle(wxCommandEvent& event);
    void Relayout();

    std::vector<Segment> m_segments;
    wxBoxSizer* m_sizer = nullptr;
    int m_lastSelected = wxNOT_FOUND;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxGenericButtonBox);
};

#endif // _WX_GENERIC_BUTTONBOX_H_
