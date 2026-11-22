/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/buttonbox.h
// Purpose:     wxButtonBox for macOS, backed by NSSegmentedControl
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_BUTTONBOX_H_
#define _WX_OSX_BUTTONBOX_H_

#include <vector>

class wxButtonBoxImpl;

class WXDLLIMPEXP_CORE wxButtonBox : public wxButtonBoxBase
{
public:
    wxButtonBox() { }
    wxButtonBox(wxWindow* parent, wxWindowID id,
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

    virtual size_t GetCount() const override;
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

    // called by the Cocoa action handler
    virtual bool OSXHandleClicked(double timestampsec) override;

private:
    wxButtonBoxImpl* GetButtonBoxPeer() const;

    // NSSegmentedControl cannot hand back NSImage as wxBitmapBundle
    std::vector<wxBitmapBundle> m_bitmaps;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxButtonBox);
};

#endif // _WX_OSX_BUTTONBOX_H_
