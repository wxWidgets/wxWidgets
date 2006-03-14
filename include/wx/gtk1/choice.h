/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/choice.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKCHOICEH__
#define __GTKCHOICEH__

class WXDLLIMPEXP_BASE wxSortedArrayString;
class WXDLLIMPEXP_BASE wxArrayString;

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxChoice : public wxChoiceBase
{
public:
    wxChoice();
    wxChoice( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = (const wxString *) NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr )
    {
        m_strings = (wxSortedArrayString *)NULL;

        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    wxChoice( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr )
    {
        m_strings = (wxSortedArrayString *)NULL;

        Create(parent, id, pos, size, choices, style, validator, name);
    }
    ~wxChoice();
    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = (wxString *) NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );
    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );

    // implement base class pure virtuals
    void Delete(int n);
    void Clear();

    int GetSelection() const;
    int GetCurrentSelection() const { return GetSelection(); }
    void SetSelection( int n );

    virtual size_t GetCount() const;
    virtual int FindString(const wxString& s, bool bCase = false) const;
    wxString GetString( int n ) const;
    void SetString( int n, const wxString& string );

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

protected:
    wxList m_clientList;    // contains the client data for the items

    void DoApplyWidgetStyle(GtkRcStyle *style);
    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, int pos);

    virtual void DoSetItemClientData( int n, void* clientData );
    virtual void* DoGetItemClientData( int n ) const;
    virtual void DoSetItemClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetItemClientObject( int n ) const;

    virtual wxSize DoGetBestSize() const;

    virtual bool IsOwnGtkWindow( GdkWindow *window );

private:
    // common part of Create() and DoAppend()
    int GtkAddHelper(GtkWidget *menu, size_t pos, const wxString& item);

    // this array is only used for controls with wxCB_SORT style, so only
    // allocate it if it's needed (hence using pointer)
    wxSortedArrayString *m_strings;

public:
    // this circumvents a GTK+ 2.0 bug so that the selection is
    // invalidated properly
    int m_selection_hack;

private:
    DECLARE_DYNAMIC_CLASS(wxChoice)
};


#endif // __GTKCHOICEH__
