/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/listctrl.h
// Purpose:     Generic list control
// Author:      Robert Roebling
// Created:     01/02/97
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_LISTCTRL_H_
#define _WX_GENERIC_LISTCTRL_H_

#include "wx/containr.h"
#include "wx/scrolwin.h"
#include "wx/textctrl.h"

#if wxUSE_DRAG_AND_DROP
class WXDLLIMPEXP_FWD_CORE wxDropTarget;
#endif

//-----------------------------------------------------------------------------
// internal classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxListHeaderWindow;
class WXDLLIMPEXP_FWD_CORE wxListMainWindow;

//-----------------------------------------------------------------------------
// wxListCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGenericListCtrl: public wxNavigationEnabled<wxListCtrlBase>,
                                          public wxScrollHelper
{
    typedef wxNavigationEnabled<wxListCtrlBase> BaseType;

public:
    wxGenericListCtrl() : wxScrollHelper(this)
    {
        Init();
    }

    wxGenericListCtrl( wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = wxLC_ICON,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxASCII_STR(wxListCtrlNameStr))
            : wxScrollHelper(this)
    {
        Create(parent, winid, pos, size, style, validator, name);
    }

    virtual ~wxGenericListCtrl();

    void Init();

    bool Create( wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = wxLC_ICON,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString &name = wxASCII_STR(wxListCtrlNameStr));

    bool GetColumn( int col, wxListItem& item ) const override;
    bool SetColumn( int col, const wxListItem& item ) override;
    int GetColumnWidth( int col ) const override;
    bool SetColumnWidth( int col, int width) override;

    // Column ordering functions
    int GetColumnOrder(int col) const override;
    int GetColumnIndexFromOrder(int order) const override;

    wxArrayInt GetColumnsOrder() const override;
    bool SetColumnsOrder(const wxArrayInt& orders) override;

    int GetCountPerPage() const; // not the same in wxGLC as in Windows, I think
    wxRect GetViewRect() const;

    bool GetItem( wxListItem& info ) const;
    bool SetItem( wxListItem& info ) ;
    bool SetItem( long index, int col, const wxString& label, int imageId = -1 );
    int  GetItemState( long item, long stateMask ) const;
    bool SetItemState( long item, long state, long stateMask);
    bool SetItemImage( long item, int image, int selImage = -1 );
    bool SetItemColumnImage( long item, long column, int image );
    wxString GetItemText( long item, int col = 0 ) const;
    void SetItemText( long item, const wxString& str );
    wxUIntPtr GetItemData( long item ) const;
    bool SetItemPtrData(long item, wxUIntPtr data);
    bool SetItemData(long item, long data) { return SetItemPtrData(item, data); }
    bool GetItemRect( long item, wxRect& rect, int code = wxLIST_RECT_BOUNDS ) const;
    bool GetSubItemRect( long item, long subItem, wxRect& rect, int code = wxLIST_RECT_BOUNDS ) const;
    bool GetItemPosition( long item, wxPoint& pos ) const;
    bool SetItemPosition( long item, const wxPoint& pos ); // not supported in wxGLC
    int GetItemCount() const override;
    int GetColumnCount() const override;
    void SetItemSpacing( int spacing, bool isSmall = false );
    wxSize GetItemSpacing() const;
    void SetItemTextColour( long item, const wxColour& col);
    wxColour GetItemTextColour( long item ) const;
    void SetItemBackgroundColour( long item, const wxColour &col);
    wxColour GetItemBackgroundColour( long item ) const;
    void SetItemFont( long item, const wxFont &f);
    wxFont GetItemFont( long item ) const;
    int GetSelectedItemCount() const;
    wxColour GetTextColour() const;
    void SetTextColour(const wxColour& col);
    long GetTopItem() const;

    bool HasCheckBoxes() const override;
    bool EnableCheckBoxes(bool enable = true) override;
    bool IsItemChecked(long item) const override;
    void CheckItem(long item, bool check) override;

    void ShowSortIndicator(int idx, bool ascending = true) override;
    int GetSortIndicator() const override;
    bool IsAscendingSortIndicator() const override;

    void SetSingleStyle( long style, bool add = true ) ;
    void SetWindowStyleFlag( long style ) override;
    void RecreateWindow() {}
    long GetNextItem( long item, int geometry = wxLIST_NEXT_ALL, int state = wxLIST_STATE_DONTCARE ) const;
    bool Arrange( int flag = wxLIST_ALIGN_DEFAULT ); // always wxLIST_ALIGN_LEFT in wxGLC

    void ClearAll();
    bool DeleteItem( long item );
    bool DeleteAllItems();
    bool DeleteAllColumns() override;
    bool DeleteColumn( int col ) override;

    void SetItemCount(long count);

    wxTextCtrl *EditLabel(long item,
                          wxClassInfo* textControlClass = wxCLASSINFO(wxTextCtrl));

    // End label editing, optionally cancelling the edit
    bool EndEditLabel(bool cancel);

    wxTextCtrl* GetEditControl() const;
    bool IsVisible(long item) const override;
    void Edit( long item ) { EditLabel(item); }

    bool EnsureVisible( long item );
    long FindItem( long start, const wxString& str, bool partial = false );
    long FindItem( long start, wxUIntPtr data );
    long FindItem( long start, const wxPoint& pt, int direction ); // not supported in wxGLC
    long HitTest( const wxPoint& point, int& flags, long *pSubItem = nullptr ) const;
    long InsertItem(wxListItem& info);
    long InsertItem( long index, const wxString& label );
    long InsertItem( long index, int imageIndex );
    long InsertItem( long index, const wxString& label, int imageIndex );
    bool ScrollList( int dx, int dy );
    bool SortItems( wxListCtrlCompare fn, wxIntPtr data );

    // do we have a header window?
    bool HasHeader() const
        { return InReportView() && !HasFlag(wxLC_NO_HEADER); }

    // refresh items selectively (only useful for virtual list controls)
    void RefreshItem(long item);
    void RefreshItems(long itemFrom, long itemTo);

    virtual void EnableBellOnNoMatch(bool on = true) override;

    // overridden base class virtuals
    // ------------------------------

    virtual wxVisualAttributes GetDefaultAttributes() const override
    {
        return GetClassDefaultAttributes(GetWindowVariant());
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    virtual void Update() override;


    // implementation only from now on
    // -------------------------------

    // generic version extension, don't use in portable code
    bool Update( long item );

    void OnInternalIdle( ) override;

    // We have to hand down a few functions
    virtual void Refresh(bool eraseBackground = true,
                         const wxRect *rect = nullptr) override;

    virtual bool SetBackgroundColour( const wxColour &colour ) override;
    virtual bool SetForegroundColour( const wxColour &colour ) override;
    virtual wxColour GetBackgroundColour() const;
    virtual wxColour GetForegroundColour() const;
    virtual bool SetFont( const wxFont &font ) override;
    virtual bool SetCursor( const wxCursor &cursor ) override;

    virtual void ExtendRulesAndAlternateColour(bool extend = true) override;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) override;
    virtual wxDropTarget *GetDropTarget() const override;
#endif

    virtual bool ShouldInheritColours() const override { return false; }

    // implementation
    // --------------

    wxListHeaderWindow  *m_headerWin;
    wxListMainWindow    *m_mainWin;

protected:
    // Implement base class pure virtual methods.
    long DoInsertColumn(long col, const wxListItem& info) override;
    void DoUpdateImages(int which) override;

    virtual wxSize DoGetBestClientSize() const override;

    // it calls our OnGetXXX() functions
    friend class WXDLLIMPEXP_FWD_CORE wxListMainWindow;

    virtual wxBorder GetDefaultBorder() const override;

    virtual wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override;

private:
    void CreateOrDestroyHeaderWindowAsNeeded();
    void OnScroll( wxScrollWinEvent& event );
    void OnSize( wxSizeEvent &event );

    // we need to return a special WM_GETDLGCODE value to process just the
    // arrows but let the other navigation characters through
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    virtual WXLRESULT
    MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;
#endif // __WXMSW__

    WX_FORWARD_TO_SCROLL_HELPER()

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxGenericListCtrl);
};

#if (!defined(__WXMSW__) || defined(__WXUNIVERSAL__)) && (!(defined(__WXMAC__) && wxOSX_USE_CARBON) || defined(__WXUNIVERSAL__ ))
/*
 * wxListCtrl has to be a real class or we have problems with
 * the run-time information.
 */

class WXDLLIMPEXP_CORE wxListCtrl: public wxGenericListCtrl
{
    wxDECLARE_DYNAMIC_CLASS(wxListCtrl);

public:
    wxListCtrl() {}

    wxListCtrl(wxWindow *parent, wxWindowID winid = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxLC_ICON,
               const wxValidator &validator = wxDefaultValidator,
               const wxString &name = wxASCII_STR(wxListCtrlNameStr))
    : wxGenericListCtrl(parent, winid, pos, size, style, validator, name)
    {
    }

};
#endif // !__WXMSW__ || __WXUNIVERSAL__

#endif // _WX_GENERIC_LISTCTRL_H_
