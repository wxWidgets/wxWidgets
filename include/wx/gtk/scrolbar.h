/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/scrolbar.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SCROLLBAR_H_
#define _WX_GTK_SCROLLBAR_H_

//-----------------------------------------------------------------------------
// wxScrollBar
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxScrollBar: public wxScrollBarBase
{
public:
    wxScrollBar();
    inline wxScrollBar( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxScrollBarNameStr) )
    {
        Create( parent, id, pos, size, style, validator, name );
    }
    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxScrollBarNameStr) );
    virtual ~wxScrollBar();
    int GetThumbPosition() const override;
    int GetThumbSize() const override;
    int GetPageSize() const override;
    int GetRange() const override;
    virtual void SetThumbPosition( int viewStart ) override;
    virtual void SetScrollbar( int position, int thumbSize, int range, int pageSize,
      bool refresh = true ) override;

    void SetThumbSize(int thumbSize);
    void SetPageSize( int pageLength );
    void SetRange(int range);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
};

#endif // _WX_GTK_SCROLLBAR_H_
