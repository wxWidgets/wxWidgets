/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/radiobox.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKRADIOBOXH__
#define __GTKRADIOBOXH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// wxRadioBox
//-----------------------------------------------------------------------------

class wxRadioBox: public wxControl
{
public:
    wxRadioBox() { Init(); }
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString choices[] = (const wxString *) NULL,
               int majorDim = 1,
               long style = wxRA_HORIZONTAL,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr)
    {
        Init();

        Create( parent, id, title, pos, size, n, choices, majorDim, style, val, name );
    }
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 1,
               long style = wxRA_HORIZONTAL,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr)
    {
        Init();

        Create( parent, id, title, pos, size, choices, majorDim, style, val, name );
    }

    virtual ~wxRadioBox();
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = (const wxString *) NULL,
                int majorDim = 0,
                long style = wxRA_HORIZONTAL,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_HORIZONTAL,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);

    int FindString( const wxString& s) const;
    void SetSelection( int n );
    int GetSelection() const;

    wxString GetString( int n ) const;
    void SetString( int n, const wxString& label );

    void Show( int item, bool show );
    void Enable( int item, bool enable );

    virtual wxString GetStringSelection() const;
    virtual bool SetStringSelection( const wxString& s );

    int GetCount() const;

    // for compatibility only, don't use these methods in new code!
#if WXWIN_COMPATIBILITY_2_2
    wxDEPRECATED( int Number() const );
    wxDEPRECATED( wxString GetLabel(int n) const );
    wxDEPRECATED( void SetLabel( int item, const wxString& label ) );
#endif // WXWIN_COMPATIBILITY_2_2

    // we have to override those to avoid virtual function name hiding
    virtual wxString GetLabel() const { return wxControl::GetLabel(); }
    virtual void SetLabel( const wxString& label );
    virtual bool Show( bool show = true );
    virtual bool Enable( bool enable = true );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    // implementation
    // --------------

    void SetFocus();
    void GtkDisableEvents();
    void GtkEnableEvents();
    bool IsOwnGtkWindow( GdkWindow *window );
    void DoApplyWidgetStyle(GtkRcStyle *style);
#if wxUSE_TOOLTIPS
    void ApplyToolTip( GtkTooltips *tips, const wxChar *tip );
#endif // wxUSE_TOOLTIPS

    // reposition the radio buttons correctly unless justCalc == true and
    // return the total size needed to accommodate them
    wxSize LayoutItems(bool justCalc) const;

    virtual void DoSetSize( int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO );
    virtual void OnInternalIdle();

    bool             m_hasFocus,
                     m_lostFocus;
    int              m_majorDim;
    wxList           m_boxes;

protected:
    // implement some base class methods
    virtual wxSize DoGetBestSize() const;

    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxRadioBox)
};

#endif // __GTKRADIOBOXH__
