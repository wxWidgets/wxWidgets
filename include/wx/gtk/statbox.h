/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/stabox.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICBOXH__
#define __GTKSTATICBOXH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

class wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox();
    wxStaticBox( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr );
    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxStaticBoxNameStr );

    virtual void SetLabel( const wxString &label );

    // implementation

    virtual void ApplyWidgetStyle();

    virtual bool IsTransparentForMouse() const { return TRUE; }

private:
    DECLARE_DYNAMIC_CLASS(wxStaticBox)
};

#endif // __GTKSTATICBOXH__
