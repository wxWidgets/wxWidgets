/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_STATTEXT_H_
#define _WX_GTK_STATTEXT_H_

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxStaticText : public wxControl
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize, 
                 long style = 0,
                 const wxString &name = wxStaticTextNameStr );

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize, 
                long style = 0,
                const wxString &name = wxStaticTextNameStr );

    wxString GetLabel() const;
    void SetLabel( const wxString &label );

    bool SetFont( const wxFont &font );
    bool SetForegroundColour( const wxColour& colour );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
    // see wx/stattext.h
    void Wrap(int width);

    // implementation
    // --------------

protected:
    virtual bool GTKWidgetNeedsMnemonic() const;
    virtual void GTKWidgetDoSetMnemonic(GtkWidget* w);

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
                           
    virtual wxSize DoGetBestSize() const;

    DECLARE_DYNAMIC_CLASS(wxStaticText)
};

#endif // _WX_GTK_STATTEXT_H_
