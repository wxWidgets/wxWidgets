/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     wxListBox class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

#ifdef __GNUG__
    #pragma interface "listbox.h"
#endif

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN
  class WXDLLEXPORT wxOwnerDrawn;

  // define the array of list box items
  #include  <wx/dynarray.h>

  WX_DEFINE_EXPORTED_ARRAY(wxOwnerDrawn *, wxListBoxItemsArray);
#endif // wxUSE_OWNER_DRAWN

// forward decl for GetSelections()
class wxArrayInt;

// List box item

WX_DEFINE_ARRAY( char * , wxListDataArray ) ;

// ----------------------------------------------------------------------------
// List box control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListBox : public wxListBoxBase
{
public:
    // ctors and such
    wxListBox();
    wxListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxListBoxNameStr);

    virtual ~wxListBox();

    // implement base class pure virtuals
    virtual void Clear();
    virtual void Delete(int n);

    virtual int GetCount() const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const;

    virtual bool IsSelected(int n) const;
    virtual void SetSelection(int n, bool select = TRUE);
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);

    virtual void DoSetFirstItem(int n);

    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;

    // wxCheckListBox support
#if wxUSE_OWNER_DRAWN
    // plug-in for derived classes
    virtual wxOwnerDrawn *CreateItem(size_t n);

    // allows to get the item and use SetXXX functions to set it's appearance
    wxOwnerDrawn *GetItem(size_t n) const { return m_aItems[n]; }

    // get the index of the given item
    int GetItemIndex(wxOwnerDrawn *item) const { return m_aItems.Index(item); }
#endif // wxUSE_OWNER_DRAWN

    // Windows callbacks

    virtual void SetupColours();
    virtual void	MacHandleControlClick( ControlHandle control , SInt16 controlpart ) ;

  	ListHandle		m_macList ;
	wxArrayString 	m_stringArray ;
	wxListDataArray m_dataArray ;
	void 			MacSetRedraw( bool doDraw ) ;
protected:
	void			MacDestroy() ;			
	void			MacDelete( int n ) ;
	void			MacInsert( int n , const char * text) ;
	void			MacAppend( const char * text) ;
	void			MacSet( int n , const char *text ) ;
	void			MacClear() ;
	void			MacSetSelection( int n , bool select ) ;
	int 			MacGetSelection() const ;
	int				MacGetSelections(wxArrayInt& aSelections) const ;
	bool			MacIsSelected( int n ) const ;
	void			MacScrollTo( int n ) ;
	void			OnSize( const wxSizeEvent &size ) ;
	void			MacDoClick() ;
	void			MacDoDoubleClick() ;

    // do we have multiple selections?
    bool HasMultipleSelection() const;

    // free memory (common part of Clear() and dtor)
    void Free();

    int m_noItems;
    int m_selected;

    virtual wxSize DoGetBestSize();

#if wxUSE_OWNER_DRAWN
    // control items
    wxListBoxItemsArray m_aItems;
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxListBox)
	DECLARE_EVENT_TABLE()
};

#endif
    // _WX_LISTBOX_H_
