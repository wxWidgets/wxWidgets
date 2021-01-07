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
        m_imageData = NULL;
#if wxUSE_MARKUP
        m_markupText = NULL;
#endif // wxUSE_MARKUP
    }

    virtual ~wxAnyButton();

    // overridden base class methods
    virtual void SetLabel(const wxString& label) wxOVERRIDE;
    virtual bool SetBackgroundColour(const wxColour &colour) wxOVERRIDE;
    virtual bool SetForegroundColour(const wxColour &colour) wxOVERRIDE;

    // implementation from now on
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;

    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item) wxOVERRIDE;

    // returns true if the platform should explicitly apply a theme border
    virtual bool CanApplyThemeBorder() const wxOVERRIDE { return false; }

protected:
    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    virtual wxBitmap DoGetBitmap(State which) const wxOVERRIDE;
    virtual void DoSetBitmap(const wxBitmap& bitmap, State which) wxOVERRIDE;
    virtual wxSize DoGetBitmapMargins() const wxOVERRIDE;
    virtual void DoSetBitmapMargins(wxCoord x, wxCoord y) wxOVERRIDE;
    virtual void DoSetBitmapPosition(wxDirection dir) wxOVERRIDE;

#if wxUSE_MARKUP
    virtual bool DoSetLabelMarkup(const wxString& markup) wxOVERRIDE;
#endif // wxUSE_MARKUP

    // Increases the passed in size to account for the button image.
    //
    // Should only be called if we do have a button, i.e. if m_imageData is
    // non-NULL.
    void AdjustForBitmapSize(wxSize& size) const;

    class wxButtonImageData *m_imageData;

#if wxUSE_MARKUP
    class wxMarkupText *m_markupText;
#endif // wxUSE_MARKUP

    // Switches button into owner-drawn mode: this is used if we need to draw
    // something not supported by the native control, such as using non default
    // colours or a bitmap on pre-XP systems.
    void MakeOwnerDrawn();
    bool IsOwnerDrawn() const;

    virtual bool MSWIsPushed() const;

private:
    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};

#endif // _WX_MSW_ANYBUTTON_H_
