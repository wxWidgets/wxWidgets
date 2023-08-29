/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/anybutton.h
// Purpose:     wxAnyButton class
// Author:      Julian Smart
// Created:     1997-02-01 (extracted from button.h)
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_ANYBUTTON_H_
#define _WX_MSW_ANYBUTTON_H_

// ----------------------------------------------------------------------------
// Common button functionality
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnyButton : public wxAnyButtonBase
{
public:
    wxAnyButton()
    {
        m_imageData = nullptr;
#if wxUSE_MARKUP
        m_markupText = nullptr;
#endif // wxUSE_MARKUP
    }

    virtual ~wxAnyButton();

    // overridden base class methods
    virtual void SetLabel(const wxString& label) override;
    virtual bool SetBackgroundColour(const wxColour &colour) override;
    virtual bool SetForegroundColour(const wxColour &colour) override;

    // implementation from now on
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;

    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item) override;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const override { return false; }

protected:
    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const override;

    virtual wxBitmap DoGetBitmap(State which) const override;
    virtual void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;
    virtual wxSize DoGetBitmapMargins() const override;
    virtual void DoSetBitmapMargins(wxCoord x, wxCoord y) override;
    virtual void DoSetBitmapPosition(wxDirection dir) override;

#if wxUSE_MARKUP
    virtual bool DoSetLabelMarkup(const wxString& markup) override;
#endif // wxUSE_MARKUP

    // Increases the passed in size to account for the button image.
    //
    // Should only be called if we do have a button, i.e. if m_imageData is
    // non-null.
    void AdjustForBitmapSize(wxSize& size) const;

    class wxButtonImageData *m_imageData;

#if wxUSE_MARKUP
    class wxMarkupText *m_markupText;
#endif // wxUSE_MARKUP

    // Switches button into owner-drawn mode: this is used if we need to draw
    // something not supported by the native control, such as using non default
    // colours or font.
    void MakeOwnerDrawn();
    bool IsOwnerDrawn() const;

    virtual bool MSWIsPushed() const;

private:
    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};

#endif // _WX_MSW_ANYBUTTON_H_
