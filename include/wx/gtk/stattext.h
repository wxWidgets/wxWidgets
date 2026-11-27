/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/stattext.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_STATTEXT_H_
#define _WX_GTK_STATTEXT_H_

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxASCII_STR(wxStaticTextNameStr) );

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxASCII_STR(wxStaticTextNameStr) );

    void SetLabel( const wxString &label ) override;

    bool SetFont( const wxFont &font ) override;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    // --------------

protected:
    virtual bool GTKWidgetNeedsMnemonic() const override;
    virtual void GTKWidgetDoSetMnemonic(GtkWidget* w) override;

    virtual wxSize DoGetBestSize() const override;

    virtual wxString WXGetVisibleLabel() const override;
    virtual void WXSetVisibleLabel(const wxString& str) override;
#if wxUSE_MARKUP
    virtual bool DoSetLabelMarkup(const wxString& markup) override;
#endif // wxUSE_MARKUP

    virtual wxSize GetMinSizeFromKnownDirection(int direction, int size,
                                                int availableOtherDir) override;
protected:
    virtual void DoSetSize(int x, int y, int width, int height,
                           int sizeFlags = wxSIZE_AUTO) override;
private:
    wxSize GTKGetTextSize(int width) const;
    wxSize GTKGetWrappedSize(int width) const;
    // reset cache
    void GTKResetWrapCache() { m_wrapCache[0].width = m_wrapCache[1].width = -1; }
    // sizes for the last two widths asked for, as sizers alternate between
    // two widths in one layout pass (row and column)
    struct WrapCacheEntry { int width = -1; wxSize size; };
    mutable WrapCacheEntry m_wrapCache[2];
    mutable int m_wrapCacheNext = 0;
    // width set by DoSetSize(), -1 before, used for the best size with wxST_WRAP
    int m_wrapWidth = -1;
    
private:
    // Common part of SetLabel() and DoSetLabelMarkup().
    typedef void (wxStaticText::*GTKLabelSetter)(GtkLabel *, const wxString&);

    void GTKDoSetLabel(GTKLabelSetter setter, const wxString& label);

    // If our font has been changed, we compute the best size ourselves because
    // GTK doesn't always do it correctly, see DoGetBestSize().
    bool m_computeOurOwnBestSize = false;

    wxDECLARE_DYNAMIC_CLASS(wxStaticText);
};

#endif
    // _WX_GTK_STATTEXT_H_
