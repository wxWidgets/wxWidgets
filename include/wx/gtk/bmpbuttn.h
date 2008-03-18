/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_BMPBUTTON_H_
#define _WX_GTK_BMPBUTTON_H_

// ----------------------------------------------------------------------------
// wxBitmapButton
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapButton: public wxBitmapButtonBase
{
public:
    wxBitmapButton() { Init(); }

    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxButtonNameStr)
    {
        Init();

        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBU_AUTODRAW,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

    void SetLabel( const wxString &label );
    virtual void SetLabel( const wxBitmap& bitmap ) { SetBitmapLabel(bitmap); }

    virtual bool Enable(bool enable = true);

    // implementation
    // --------------

    void GTKMouseEnters();
    void GTKMouseLeaves();
    void GTKPressed();
    void GTKReleased();

protected:
    virtual void OnSetBitmap();
    virtual wxSize DoGetBestSize() const;
    void DoApplyWidgetStyle(GtkRcStyle *style);

    void Init();

private:
    void OnFocusChange(wxFocusEvent& event);

    // true iff mouse hovers over the button
    bool         m_mouseHovers;
    // true iff the button is in pressed state
    bool         m_isPressed;

    DECLARE_DYNAMIC_CLASS(wxBitmapButton)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GTK_BMPBUTTON_H_
