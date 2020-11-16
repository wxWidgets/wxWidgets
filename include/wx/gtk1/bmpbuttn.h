/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __BMPBUTTONH__
#define __BMPBUTTONH__

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
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxButtonNameStr))
    {
        Init();

        Create(parent, id, bitmap, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool CreateCloseButton(wxWindow* parent,
                           wxWindowID winid,
                           const wxString& name = wxString());

    void SetLabel( const wxString &label );
    virtual void SetLabel( const wxBitmap& bitmap ) { SetBitmapLabel(bitmap); }

    virtual bool Enable(bool enable = TRUE);

    // implementation
    // --------------

    void GTKSetHasFocus();
    void GTKSetNotFocus();
    void StartSelect();
    void EndSelect();
    void DoApplyWidgetStyle(GtkRcStyle *style);

    bool         m_hasFocus:1;
    bool         m_isSelected:1;

protected:
    virtual void OnSetBitmap();
    virtual wxSize DoGetBestSize() const;

    void Init();

private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapButton);
};

#endif // __BMPBUTTONH__
