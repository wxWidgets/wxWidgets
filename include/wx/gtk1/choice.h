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
    wxList   m_clientDataList;
    wxList   m_clientObjectList;

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

private:
    DECLARE_DYNAMIC_CLASS(wxChoice)
};


#endif // __GTKCHOICEH__
