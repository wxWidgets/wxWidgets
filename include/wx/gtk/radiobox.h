/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKRADIOBOXH__
#define __GTKRADIOBOXH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_RADIOBOX

#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxRadioBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxRadioBoxNameStr;

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

    ~wxRadioBox();
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

    int FindString( const wxString& s) const;
    void SetSelection( int n );
    int GetSelection() const;
    
    wxString GetString( int n ) const;
    
    wxString GetLabel( int item ) const;
    wxString GetLabel() const { return wxControl::GetLabel(); }
    void SetLabel( const wxString& label );
    void SetLabel( int item, const wxString& label );
    
    /* doesn't work */
    void SetLabel( int item, wxBitmap *bitmap );
    
    bool Show( bool show );
    void Show( int item, bool show );
    
    bool Enable( bool enable );
    void Enable( int item, bool enable );
    
    virtual wxString GetStringSelection() const;
    virtual bool SetStringSelection( const wxString& s );
    
    virtual int Number() const;
    int GetNumberOfRowsOrCols() const;
    void SetNumberOfRowsOrCols( int n );
    
    // implementation
    // --------------
    
    void SetFocus();
    void GtkDisableEvents();
    void GtkEnableEvents();
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
#if wxUSE_TOOLTIPS
    void ApplyToolTip( GtkTooltips *tips, const wxChar *tip );
#endif // wxUSE_TOOLTIPS
    wxSize LayoutItems();

    virtual void DoSetSize( int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO );
    virtual void OnInternalIdle();
    
    bool             m_alreadySent;
    bool             m_hasFocus,
                     m_lostFocus;
    int              m_majorDim;
    wxList           m_boxes;

protected:
    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxRadioBox)
};

#endif

#endif // __GTKRADIOBOXH__
