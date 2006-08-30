/////////////////////////////////////////////////////////////////////////////
// Name:        spinctrl.h
// Purpose:     wxSpinCtrl class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SPINCTRL_H_
#define _WX_GTK_SPINCTRL_H_

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinCtrl : public wxControl
{
public:
    wxSpinCtrl();
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = _T("wxSpinCtrl"))
    {
        Create(parent, id, value, pos, size, style, min, max, initial, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = _T("wxSpinCtrl"));

    void SetValue(const wxString& text);
    void SetSelection(long from, long to);

    virtual int GetValue() const;
    virtual void SetValue( int value );
    virtual void SetRange( int minVal, int maxVal );
    virtual int GetMin() const;
    virtual int GetMax() const;

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
    // implementation
    void OnChar( wxKeyEvent &event );
    
    void GtkDisableEvents();
    void GtkEnableEvents();

    int m_pos;

protected:
    virtual wxSize DoGetBestSize() const;
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

    // Widgets that use the style->base colour for the BG colour should
    // override this and return true.
    virtual bool UseGTKStyleBase() const { return true; }

private:
    DECLARE_DYNAMIC_CLASS(wxSpinCtrl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GTK_SPINCTRL_H_
