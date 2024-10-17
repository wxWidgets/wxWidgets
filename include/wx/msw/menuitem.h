///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Vadim Zeitlin
// Created:     11.11.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _MENUITEM_H
#define   _MENUITEM_H

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"

    struct tagRECT;
#endif

// ----------------------------------------------------------------------------
// wxMenuItem: an item in the menu, optionally implements owner-drawn behaviour
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
#if wxUSE_OWNER_DRAWN
                             , public wxOwnerDrawn
#endif
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *parentMenu = nullptr,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = nullptr);
    virtual ~wxMenuItem();

    // override base class virtuals
    virtual void SetItemLabel(const wxString& strName) override;

    virtual void Enable(bool bDoEnable = true) override;
    virtual void Check(bool bDoCheck = true) override;
    virtual bool IsChecked() const override;

    // unfortunately needed to resolve ambiguity between
    // wxMenuItemBase::IsCheckable() and wxOwnerDrawn::IsCheckable()
    bool IsCheckable() const { return wxMenuItemBase::IsCheckable(); }

    // the id for a popup menu is really its menu handle (as required by
    // ::AppendMenu() API), so this function will return either the id or the
    // menu handle depending on what we are
    //
    // notice that it also returns the id as an unsigned int, as required by
    // Win32 API
    WXWPARAM GetMSWId() const;

    void SetBitmaps(const wxBitmapBundle& bmpChecked,
                    const wxBitmapBundle& bmpUnchecked = wxNullBitmap)
    {
        DoSetBitmap(bmpChecked, true);
        DoSetBitmap(bmpUnchecked, false);
    }

    virtual void SetBitmap(const wxBitmapBundle& bmp) override
    {
        DoSetBitmap(bmp, true);
    }

    virtual wxBitmap GetBitmap() const override
    {
        return GetBitmap(true);
    }

    void SetupBitmaps();

    // The functions taking bChecked are wxMSW-specific.
    void SetBitmap(const wxBitmapBundle& bmp, bool bChecked)
    {
        DoSetBitmap(bmp, bChecked);
    }

    wxBitmap GetBitmap(bool bChecked) const;

#if wxUSE_OWNER_DRAWN
    void SetDisabledBitmap(const wxBitmapBundle& bmpDisabled)
    {
        m_bmpDisabled = bmpDisabled;
        SetOwnerDrawn(true);
    }

    wxBitmap GetDisabledBitmap() const;

    int MeasureAccelWidth() const;

    // override wxOwnerDrawn base class virtuals
    virtual wxString GetName() const override;
    virtual bool OnMeasureItem(size_t *pwidth, size_t *pheight) override;
    virtual bool OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat) override;

protected:
    virtual void GetFontToUse(wxFont& font) const override;
    virtual void GetColourToUse(wxODStatus stat, wxColour& colText, wxColour& colBack) const override;

private:
    // helper function for draw std menu check mark
    void DrawStdCheckMark(WXHDC hdc, const tagRECT* rc, wxODStatus stat);

    // helper function to determine if the item must be owner-drawn
    bool MSWMustUseOwnerDrawn();
#endif // wxUSE_OWNER_DRAWN

    enum BitmapKind
    {
        Normal,
        Checked,
        Unchecked
    };

    // helper function to get a handle for bitmap associated with item
    WXHBITMAP GetHBitmapForMenu(BitmapKind kind) const;

    // helper function to set/change the bitmap
    void DoSetBitmap(const wxBitmapBundle& bmp, bool bChecked);

private:
    // common part of all ctors
    void Init();

    // Return the item position in the menu containing it.
    //
    // Returns -1 if the item is not attached to a menu or if we can't find its
    // position (which is not really supposed to ever happen).
    int MSGetMenuItemPos() const;

    // Get the extent of the given text using the correct font.
    wxSize GetMenuTextExtent(const wxString& text) const;

    // item bitmaps
    wxBitmapBundle m_bmpUnchecked;   // (used only for checkable items)
#if wxUSE_OWNER_DRAWN
    wxBitmapBundle m_bmpDisabled;
#endif // wxUSE_OWNER_DRAWN

    // Bitmaps being currently used: we must store them separately from the
    // bundle itself because their HBITMAPs must remain valid as long as
    // they're used by Windows.
    wxBitmap m_bmpCheckedCurrent,
             m_bmpUncheckedCurrent;

    // Give wxMenu access to our MSWMustUseOwnerDrawn() and GetHBitmapForMenu().
    friend class wxMenu;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMenuItem);
};

#endif  //_MENUITEM_H
