/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKCHOICEH__
#define __GTKCHOICEH__

#ifdef __GNUG__
    #pragma interface "choice.h"
#endif

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

class wxChoice : public wxChoiceBase
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
    ~wxChoice();
    bool Create( wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = (wxString *) NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxChoiceNameStr );

    // implement base class pure virtuals
    void Delete(int n);
    void Clear();

    int GetSelection() const;
    void SetSelection( int n );

    virtual int GetCount() const;
    int FindString( const wxString &string ) const;
    wxString GetString( int n ) const;

    // implementation
    wxList m_clientList;    // contains the client data for the items

    void DisableEvents();
    void EnableEvents();
    void AppendCommon( const wxString &item );
    void ApplyWidgetStyle();

protected:
    virtual int DoAppend(const wxString& item);

    virtual void DoSetClientData( int n, void* clientData );
    virtual void* DoGetClientData( int n ) const;
    virtual void DoSetClientObject( int n, wxClientData* clientData );
    virtual wxClientData* DoGetClientObject( int n ) const;

    // the above virtuals hide these virtuals in wxChoiceBase
    virtual void DoSetClientData(void* clientData )
        { wxWindowBase::DoSetClientData(clientData); };
    virtual void* DoGetClientData() const
        { return(wxWindowBase::DoGetClientData()); };
    virtual void DoSetClientObject( wxClientData* clientData )
        { wxWindowBase::DoSetClientObject(clientData); };
    virtual wxClientData* DoGetClientObject() const
        { return(wxWindowBase::DoGetClientObject()); };

private:
    // common part of Create() and DoAppend()
    size_t AppendHelper(GtkWidget *menu, const wxString& item);

    // this array is only used for controls with wxCB_SORT style, so only
    // allocate it if it's needed (hence using pointer)
    wxSortedArrayString *m_strings;

    DECLARE_DYNAMIC_CLASS(wxChoice)
};


#endif // __GTKCHOICEH__
