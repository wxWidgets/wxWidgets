/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/button.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_BUTTON_H_
#define _WX_GTK_BUTTON_H_

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxButton: public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    virtual ~wxButton();

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);

    virtual void SetDefault();
    virtual void SetLabel( const wxString &label );
    virtual bool Enable( bool enable = TRUE );

    // implementation
    // --------------

    // Since this wxButton doesn't derive from wxButtonBase (why?) we need
    // to override this here too...
    virtual bool ShouldInheritColours() const { return false; }
    
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // helper to allow access to protected member from GTK callback
    void MoveWindow(int x, int y, int width, int height) { DoMoveWindow(x, y, width, height); }

protected:
    virtual wxSize DoGetBestSize() const;
    virtual void DoApplyWidgetStyle(GtkRcStyle *style);

    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

private:
    DECLARE_DYNAMIC_CLASS(wxButton)
};

#endif // _WX_GTK_BUTTON_H_
