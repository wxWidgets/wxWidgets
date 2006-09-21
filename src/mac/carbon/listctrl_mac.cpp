/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/listctrl.cpp
// Purpose:     wxListCtrl
// Author:      Julian Smart
// Modified by: Agron Selimaj
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_LISTCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/textctrl.h"
#endif

#include "wx/mac/uma.h"

#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/sysopt.h"

#define wxMAC_ALWAYS_USE_GENERIC_LISTCTRL wxT("mac.listctrl.always_use_generic")

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxListCtrlStyle )

wxBEGIN_FLAGS( wxListCtrlStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxLC_LIST)
    wxFLAGS_MEMBER(wxLC_REPORT)
    wxFLAGS_MEMBER(wxLC_ICON)
    wxFLAGS_MEMBER(wxLC_SMALL_ICON)
    wxFLAGS_MEMBER(wxLC_ALIGN_TOP)
    wxFLAGS_MEMBER(wxLC_ALIGN_LEFT)
    wxFLAGS_MEMBER(wxLC_AUTOARRANGE)
    wxFLAGS_MEMBER(wxLC_USER_TEXT)
    wxFLAGS_MEMBER(wxLC_EDIT_LABELS)
    wxFLAGS_MEMBER(wxLC_NO_HEADER)
    wxFLAGS_MEMBER(wxLC_SINGLE_SEL)
    wxFLAGS_MEMBER(wxLC_SORT_ASCENDING)
    wxFLAGS_MEMBER(wxLC_SORT_DESCENDING)
    wxFLAGS_MEMBER(wxLC_VIRTUAL)

wxEND_FLAGS( wxListCtrlStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxListCtrl, wxControl,"wx/listctrl.h")

wxBEGIN_PROPERTIES_TABLE(wxListCtrl)
    wxEVENT_PROPERTY( TextUpdated , wxEVT_COMMAND_TEXT_UPDATED , wxCommandEvent )

    wxPROPERTY_FLAGS( WindowStyle , wxListCtrlStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxListCtrl)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxListCtrl , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size , long , WindowStyle )

/*
 TODO : Expose more information of a list's layout etc. via appropriate objects (à la NotebookPageInfo)
*/
#else
IMPLEMENT_DYNAMIC_CLASS(wxListCtrl, wxControl)
#endif

IMPLEMENT_DYNAMIC_CLASS(wxListView, wxListCtrl)
IMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxNotifyEvent)

WX_DECLARE_EXPORTED_LIST(wxListItem, wxListItemList);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxListItemList)

class wxMacListCtrlItem : public wxMacListBoxItem
{
public:
    wxMacListCtrlItem();
    
    virtual void Notification(wxMacDataItemBrowserControl *owner ,
        DataBrowserItemNotification message,
        DataBrowserItemDataRef itemData ) const;
        
    virtual void SetColumnInfo( unsigned int column, wxListItem* item );
    virtual wxListItem* GetColumnInfo( unsigned int column );
    virtual bool HasColumnInfo( unsigned int column );
    
    virtual void SetColumnTextValue( unsigned int column, const wxString& text );
    virtual const wxString& GetColumnTextValue( unsigned int column ); 
        
    virtual int GetColumnImageValue( unsigned int column );
    virtual void SetColumnImageValue( unsigned int column, int imageIndex );
    
    virtual OSStatus GetSetData( wxMacDataItemBrowserControl *owner ,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    bool changeValue );

        
    virtual ~wxMacListCtrlItem();
protected:
    wxListItemList m_rowItems;
};

// TODO: Make a better name!!
class wxMacDataBrowserListCtrlControl : public wxMacDataItemBrowserControl
{
public:
    wxMacDataBrowserListCtrlControl( wxWindow *peer, const wxPoint& pos, const wxSize& size, long style );
    virtual ~wxMacDataBrowserListCtrlControl();

    // create a list item (can be a subclass of wxMacListBoxItem)

    virtual wxMacListCtrlItem* CreateItem();
    
    virtual void MacInsertItem( unsigned int n, wxListItem* item );
    virtual void MacSetColumnInfo( unsigned int row, unsigned int column, wxListItem* item );
    virtual void MacGetColumnInfo( unsigned int row, unsigned int column, wxListItem& item );
    virtual void UpdateState(wxMacDataItem* dataItem, wxListItem* item);
    
protected:
    wxClientDataType m_clientDataItemsType;

};

// TODO: This gives errors, find out why. 
//BEGIN_EVENT_TABLE(wxListCtrl, wxControl)
//    EVT_PAINT(wxListCtrl::OnPaint)
//END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

wxMacListControl* wxListCtrl::GetPeer() const
{
    return dynamic_cast<wxMacListControl*>(m_peer);
}

// ----------------------------------------------------------------------------
// wxListCtrl construction
// ----------------------------------------------------------------------------

void wxListCtrl::Init()
{
    m_imageListNormal = NULL;
    m_imageListSmall = NULL;
    m_imageListState = NULL;
    
    // keep track of if we created our own image lists, or if they were assigned
    // to us.
    m_ownsImageListNormal = m_ownsImageListSmall = m_ownsImageListState = false;
    m_colCount = 0;
    m_count = 0;
    m_textCtrl = NULL;
    m_genericImpl = NULL;
    m_dbImpl = NULL;
}

bool wxListCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{

    // for now, we'll always use the generic list control for ICON and LIST views,
    // because they dynamically change the number of columns on resize.
    // Also, allow the user to set it to use the list ctrl as well.
    if ( (wxSystemOptions::HasOption( wxMAC_ALWAYS_USE_GENERIC_LISTCTRL ) 
            && (wxSystemOptions::GetOptionInt( wxMAC_ALWAYS_USE_GENERIC_LISTCTRL ) == 1)) ||
            (style & wxLC_ICON) || (style & wxLC_SMALL_ICON) || (style & wxLC_LIST))
    {
        m_macIsUserPane = true;
    
        if ( !wxWindow::Create(parent, id, pos, size, style, name) )
            return false;
        m_genericImpl = new wxGenericListCtrl(this, id, pos, size, style, validator, name);
        return true;
    }
    
    else
    {
         m_macIsUserPane = false;
        
        if ( !wxWindow::Create(parent, id, pos, size, style, name) )
            return false;
        m_dbImpl = new wxMacDataBrowserListCtrlControl( this, pos, size, style );
        m_peer = m_dbImpl;

        MacPostControlCreate( pos, size );
    }
    
    return true;
}

wxListCtrl::~wxListCtrl()
{        
    if (m_ownsImageListNormal)
        delete m_imageListNormal;
    if (m_ownsImageListSmall)
        delete m_imageListSmall;
    if (m_ownsImageListState)
        delete m_imageListState;
}

// ----------------------------------------------------------------------------
// set/get/change style
// ----------------------------------------------------------------------------

// Add or remove a single window style
void wxListCtrl::SetSingleStyle(long style, bool add)
{
    long flag = GetWindowStyleFlag();

    // Get rid of conflicting styles
    if ( add )
    {
        if ( style & wxLC_MASK_TYPE)
            flag = flag & ~wxLC_MASK_TYPE;
        if ( style & wxLC_MASK_ALIGN )
            flag = flag & ~wxLC_MASK_ALIGN;
        if ( style & wxLC_MASK_SORT )
            flag = flag & ~wxLC_MASK_SORT;
    }

    if ( add )
        flag |= style;
    else
        flag &= ~style;

    SetWindowStyleFlag(flag);
}

// Set the whole window style
void wxListCtrl::SetWindowStyleFlag(long flag)
{
    if ( flag != m_windowStyle )
    {
        m_windowStyle = flag;

        if (m_genericImpl){
            m_genericImpl->SetWindowStyleFlag(flag);
        }

        Refresh();
    }
}

void wxListCtrl::DoSetSize( int x, int y, int width, int height, int sizeFlags ){
    wxControl::DoSetSize(x, y, width, height, sizeFlags);
            
    if (m_genericImpl)
        m_genericImpl->SetSize(x, y, width, height, sizeFlags);
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

// Gets information about this column
bool wxListCtrl::GetColumn(int col, wxListItem& item) const
{
    if (m_genericImpl)
        return m_genericImpl->GetColumn(col, item);

    bool success = true;
    
    if ( item.m_mask & wxLIST_MASK_TEXT )
    {
    }

    if ( item.m_mask & wxLIST_MASK_FORMAT )
    {
    }

    if ( item.m_mask & wxLIST_MASK_IMAGE )
    {
    }

    if ( (item.m_mask & wxLIST_MASK_TEXT) )
    {
    }


    return success;
}

// Sets information about this column
bool wxListCtrl::SetColumn(int col, wxListItem& item)
{
    if (m_genericImpl)
        return m_genericImpl->SetColumn(col, item);
        
    return false;
}

int wxListCtrl::GetColumnCount() const
{
    if (m_genericImpl)
        return m_genericImpl->GetColumnCount();
        
    if (m_dbImpl){
        UInt32 count;
        m_dbImpl->GetColumnCount(&count);
        return count;
    }

    return m_colCount;
}

// Gets the column width
int wxListCtrl::GetColumnWidth(int col) const
{
    if (m_genericImpl)
        return m_genericImpl->GetColumnWidth(col);
        
    if (m_dbImpl){
        return m_dbImpl->GetColumnWidth(col);
    }

    return 0;
}

// Sets the column width
bool wxListCtrl::SetColumnWidth(int col, int width)
{
    if (m_genericImpl)
        return m_genericImpl->SetColumnWidth(col, width);
        
    // TODO: This is setting the width of the first column
    //       to the entire window width; investigate why
    //       this is.
    
    //if (m_dbImpl){
    //    m_dbImpl->SetColumnWidth(col+1, width);
    //    return true;
    //}
    
    return false;
}

// Gets the number of items that can fit vertically in the
// visible area of the list control (list or report view)
// or the total number of items in the list control (icon
// or small icon view)
int wxListCtrl::GetCountPerPage() const
{
    if (m_genericImpl)
        return m_genericImpl->GetCountPerPage();
        
    if (m_dbImpl){
    }
        
    return 1;
}

// Gets the edit control for editing labels.
wxTextCtrl* wxListCtrl::GetEditControl() const
{
    if (m_genericImpl)
        return m_genericImpl->GetEditControl();
        
    return NULL;
}

// Gets information about the item
bool wxListCtrl::GetItem(wxListItem& info) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItem(info);
        
    if (m_dbImpl)
        m_dbImpl->MacGetColumnInfo(info.m_itemId, info.m_col, info);
    bool success = true;
    return success;
}

// Sets information about the item
bool wxListCtrl::SetItem(wxListItem& info)
{
    if (m_genericImpl)
        return m_genericImpl->SetItem(info);

    if (m_dbImpl)
        m_dbImpl->MacSetColumnInfo( info.m_itemId, info.m_col, &info );

    return true;
}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{
    if (m_genericImpl)
        return m_genericImpl->SetItem(index, col, label, imageId);

    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    info.m_col = col;
    if ( imageId > -1 )
    {
        info.m_image = imageId;
        info.m_mask |= wxLIST_MASK_IMAGE;
    }
    return SetItem(info);
}


// Gets the item state
int wxListCtrl::GetItemState(long item, long stateMask) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemState(item, stateMask);
        
    wxListItem info;

    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_itemId = item;

    if (!GetItem(info))
        return 0;

    return info.m_state;
}

// Sets the item state
bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{
    if (m_genericImpl)
        return m_genericImpl->SetItemState(item, state, stateMask);

    wxListItem info;
    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_state = state;
    info.m_itemId = item;
    return SetItem(info);
}

// Sets the item image
bool wxListCtrl::SetItemImage(long item, int image, int WXUNUSED(selImage))
{
    return SetItemColumnImage(item, 0, image);
}

// Sets the item image
bool wxListCtrl::SetItemColumnImage(long item, long column, int image)
{
    if (m_genericImpl)
        return m_genericImpl->SetItemColumnImage(item, column, image);

    wxListItem info;

    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_image = image;
    info.m_itemId = item;
    info.m_col = column;

    return SetItem(info);
}

// Gets the item text
wxString wxListCtrl::GetItemText(long item) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemText(item);
        
    wxListItem info;

    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = item;

    if (!GetItem(info))
        return wxEmptyString;
    return info.m_text;
}

// Sets the item text
void wxListCtrl::SetItemText(long item, const wxString& str)
{
    if (m_genericImpl)
        return m_genericImpl->SetItemText(item, str);
        
    wxListItem info;

    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = item;
    info.m_text = str;

    SetItem(info);
}

// Gets the item data
long wxListCtrl::GetItemData(long item) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemData(item);

    wxListItem info;

    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;

    if (!GetItem(info))
        return 0;
    return info.m_data;
}

// Sets the item data
bool wxListCtrl::SetItemData(long item, long data)
{
    if (m_genericImpl)
        return m_genericImpl->SetItemData(item, data);
        
    wxListItem info;

    info.m_mask = wxLIST_MASK_DATA;
    info.m_itemId = item;
    info.m_data = data;

    return SetItem(info);
}

wxRect wxListCtrl::GetViewRect() const
{
    wxASSERT_MSG( !HasFlag(wxLC_REPORT | wxLC_LIST),
                    _T("wxListCtrl::GetViewRect() only works in icon mode") );
                    
    if (m_genericImpl)
        return m_genericImpl->GetViewRect();
        
    wxRect rect;
    return rect;
}

// Gets the item rectangle
bool wxListCtrl::GetItemRect(long item, wxRect& rect, int code) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemRect(item, rect, code);

    return true; 
}

// Gets the item position
bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemPosition(item, pos);

    bool success = false;

    return success;
}

// Sets the item position.
bool wxListCtrl::SetItemPosition(long item, const wxPoint& pos)
{
    if (m_genericImpl)
        return m_genericImpl->SetItemPosition(item, pos);

    return false;
}

// Gets the number of items in the list control
int wxListCtrl::GetItemCount() const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemCount();

    if (m_dbImpl)
        return m_dbImpl->MacGetCount();
    
    return m_count;
}

void wxListCtrl::SetItemSpacing( int spacing, bool isSmall )
{
    if (m_genericImpl)
        m_genericImpl->SetItemSpacing(spacing, isSmall);
}

wxSize wxListCtrl::GetItemSpacing() const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemSpacing();

    return wxSize(0, 0);
}

void wxListCtrl::SetItemTextColour( long item, const wxColour &col )
{
    if (m_genericImpl){
        m_genericImpl->SetItemTextColour(item, col);
    }
    
    wxListItem info;
    info.m_itemId = item;
    info.SetTextColour( col );
    SetItem( info );
}

wxColour wxListCtrl::GetItemTextColour( long item ) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemTextColour(item);
        
    if (m_dbImpl){
        wxListItem info;
        if (GetItem(info))
            return info.GetTextColour();
    }
    return wxNullColour;
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col )
{
    if (m_genericImpl){
        m_genericImpl->SetItemBackgroundColour(item, col);
        return;
    }
    
    wxListItem info;
    info.m_itemId = item;
    info.SetBackgroundColour( col );
    SetItem( info );
}

wxColour wxListCtrl::GetItemBackgroundColour( long item ) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemBackgroundColour(item);
        
    if (m_dbImpl){
        wxListItem info;
        if (GetItem(info))
            return info.GetBackgroundColour();
    }
    return wxNullColour;
}

void wxListCtrl::SetItemFont( long item, const wxFont &f )
{
    if (m_genericImpl){
        m_genericImpl->SetItemFont(item, f);
        return;
    }

    wxListItem info;
    info.m_itemId = item;
    info.SetFont( f );
    SetItem( info );
}

wxFont wxListCtrl::GetItemFont( long item ) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemFont(item);
        
    if (m_dbImpl){
        wxListItem info;
        if (GetItem(info))
            return info.GetFont();
    }

    return wxNullFont;
}

// Gets the number of selected items in the list control
int wxListCtrl::GetSelectedItemCount() const
{
    if (m_genericImpl)
        return m_genericImpl->GetSelectedItemCount();

    if (m_dbImpl)
        return m_dbImpl->GetSelectedItemCount(NULL, true);

    return 0;
}

// Gets the text colour of the listview
wxColour wxListCtrl::GetTextColour() const
{
    if (m_genericImpl)
        return m_genericImpl->GetTextColour();

    // TODO: we need owner drawn list items to customize text color.
    if (m_dbImpl)
        return *wxBLACK;
}

// Sets the text colour of the listview
void wxListCtrl::SetTextColour(const wxColour& col)
{
    if (m_genericImpl){
        m_genericImpl->SetTextColour(col);
        return;
    }
    
    // TODO: if we add owner-drawn item support for DataBrowser,
    // consider supporting this property
}

// Gets the index of the topmost visible item when in
// list or report view
long wxListCtrl::GetTopItem() const
{
    if (m_genericImpl)
        return m_genericImpl->GetTopItem();
        
    return 0;
}

// Searches for an item, starting from 'item'.
// 'geometry' is one of
// wxLIST_NEXT_ABOVE/ALL/BELOW/LEFT/RIGHT.
// 'state' is a state bit flag, one or more of
// wxLIST_STATE_DROPHILITED/FOCUSED/SELECTED/CUT.
// item can be -1 to find the first item that matches the
// specified flags.
// Returns the item or -1 if unsuccessful.
long wxListCtrl::GetNextItem(long item, int geom, int state) const
{
    if (m_genericImpl)
        return m_genericImpl->GetNextItem(item, geom, state);

    return 0;
}


wxImageList *wxListCtrl::GetImageList(int which) const
{
    if (m_genericImpl)
        return m_genericImpl->GetImageList(which);

    if ( which == wxIMAGE_LIST_NORMAL )
    {
        return m_imageListNormal;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        return m_imageListSmall;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        return m_imageListState;
    }
    return NULL;
}

void wxListCtrl::SetImageList(wxImageList *imageList, int which)
{
    if (m_genericImpl){
        m_genericImpl->SetImageList(imageList, which);
        return;
    }

    int flags = 0;
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        if (m_ownsImageListNormal) delete m_imageListNormal;
        m_imageListNormal = imageList;
        m_ownsImageListNormal = false;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        if (m_ownsImageListSmall) delete m_imageListSmall;
        m_imageListSmall = imageList;
        m_ownsImageListSmall = false;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        if (m_ownsImageListState) delete m_imageListState;
        m_imageListState = imageList;
        m_ownsImageListState = false;
    }
}

void wxListCtrl::AssignImageList(wxImageList *imageList, int which)
{
    if (m_genericImpl){
        m_genericImpl->AssignImageList(imageList, which);
        return;
    }

    SetImageList(imageList, which);
    if ( which == wxIMAGE_LIST_NORMAL )
        m_ownsImageListNormal = true;
    else if ( which == wxIMAGE_LIST_SMALL )
        m_ownsImageListSmall = true;
    else if ( which == wxIMAGE_LIST_STATE )
        m_ownsImageListState = true;
}

// ----------------------------------------------------------------------------
// Operations
// ----------------------------------------------------------------------------

// Arranges the items
bool wxListCtrl::Arrange(int flag)
{
    if (m_genericImpl)
        return m_genericImpl->Arrange(flag);
    return false;
}

// Deletes an item
bool wxListCtrl::DeleteItem(long item)
{
    if (m_genericImpl)
        return m_genericImpl->DeleteItem(item);
        
    if (m_dbImpl){
        m_dbImpl->MacDelete(item);
        wxListEvent event( wxEVT_COMMAND_LIST_DELETE_ITEM, GetId() );
        event.SetEventObject( this );
        event.m_itemIndex = item;
        GetEventHandler()->ProcessEvent( event );   
    
    }
    return true;
}

// Deletes all items
bool wxListCtrl::DeleteAllItems()
{
    if (m_genericImpl)
        return m_genericImpl->DeleteAllItems();
        
    if (m_dbImpl){
        m_dbImpl->MacClear();
        wxListEvent event( wxEVT_COMMAND_LIST_DELETE_ALL_ITEMS, GetId() );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );   
    }
    return true;
}

// Deletes all items
bool wxListCtrl::DeleteAllColumns()
{
    if (m_genericImpl)
        return m_genericImpl->DeleteAllColumns();
        
    if (m_dbImpl){
        UInt32 cols;
        m_dbImpl->GetColumnCount(&cols);
        for (UInt32 col = 0; col < cols; col++){
                DeleteColumn(col);
        } 
    }
        
    return true;
}

// Deletes a column
bool wxListCtrl::DeleteColumn(int col)
{
    if (m_genericImpl)
        return m_genericImpl->DeleteColumn(col);
        
    if (m_dbImpl){
        OSStatus err = m_dbImpl->RemoveColumn(col);
        return err == noErr;
    }
    
    return true;
}

// Clears items, and columns if there are any.
void wxListCtrl::ClearAll()
{
    if (m_genericImpl){
        m_genericImpl->ClearAll();
        return;
    }
    
    DeleteAllItems();
    if ( m_colCount > 0 )
        DeleteAllColumns();
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    if (m_genericImpl)
        return m_genericImpl->EditLabel(item, textControlClass);
        
    return NULL;
}

// End label editing, optionally cancelling the edit
bool wxListCtrl::EndEditLabel(bool cancel)
{
    // TODO: generic impl. doesn't have this method - is it needed for us?
    if (m_genericImpl)
        return false; // m_genericImpl->EndEditLabel(cancel);
        
    bool b = true;
    return b;
}

// Ensures this item is visible
bool wxListCtrl::EnsureVisible(long item)
{
    if (m_genericImpl)
        return m_genericImpl->EnsureVisible(item);
        
    if (m_dbImpl){
        wxMacDataItem* dataItem = m_dbImpl->GetItemFromLine(item);
        m_dbImpl->RevealItem(dataItem, kDataBrowserRevealWithoutSelecting);
    }
}

// Find an item whose label matches this string, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    if (m_genericImpl)
        return m_genericImpl->FindItem(start, str, partial);

    return -1;
}

// Find an item whose data matches this data, starting from the item after 'start'
// or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, long data)
{
    if (m_genericImpl)
        return m_genericImpl->FindItem(start, data);
        
    long  idx = start + 1;
    long count = GetItemCount();

    while (idx < count)
    {
        if (GetItemData(idx) == data)
            return idx;
        idx++;
    };

    return -1;
}

// Find an item nearest this position in the specified direction, starting from
// the item after 'start' or the beginning if 'start' is -1.
long wxListCtrl::FindItem(long start, const wxPoint& pt, int direction)
{
    if (m_genericImpl)
        return m_genericImpl->FindItem(start, pt, direction);
    return -1;
}

// Determines which item (if any) is at the specified point,
// giving details in 'flags' (see wxLIST_HITTEST_... flags above)
long
wxListCtrl::HitTest(const wxPoint& point, int& flags, long *ptrSubItem) const
{
    if (m_genericImpl)
        return m_genericImpl->HitTest(point, flags, ptrSubItem);

    return -1;
}


// Inserts an item, returning the index of the new item if successful,
// -1 otherwise.
long wxListCtrl::InsertItem(wxListItem& info)
{
    wxASSERT_MSG( !IsVirtual(), _T("can't be used with virtual controls") );

    if (m_genericImpl)
        return m_genericImpl->InsertItem(info);
        
    if (m_dbImpl){
        int count = GetItemCount();

        if (info.m_itemId > count)
            info.m_itemId = count;
    
        m_dbImpl->MacInsertItem(info.m_itemId, &info );
        wxListEvent event( wxEVT_COMMAND_LIST_INSERT_ITEM, GetId() );
        event.SetEventObject( this );
        event.m_itemIndex = info.m_itemId;
        GetEventHandler()->ProcessEvent( event );   
    }

    return info.m_itemId;
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
    if (m_genericImpl)
        return m_genericImpl->InsertItem(index, label);

    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem(info);
}

// Inserts an image item
long wxListCtrl::InsertItem(long index, int imageIndex)
{
    if (m_genericImpl)
        return m_genericImpl->InsertItem(index, imageIndex);
        
    wxListItem info;
    info.m_image = imageIndex;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_itemId = index;
    return InsertItem(info);
}

// Inserts an image/string item
long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
    if (m_genericImpl)
        return m_genericImpl->InsertItem(index, label, imageIndex);
        
    wxListItem info;
    info.m_image = imageIndex;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_IMAGE | wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem(info);
}

// For list view mode (only), inserts a column.
long wxListCtrl::InsertColumn(long col, wxListItem& item)
{
    if (m_genericImpl)
        return m_genericImpl->InsertColumn(col, item);
    
    if (m_dbImpl){
        if ( !(item.GetMask() & wxLIST_MASK_WIDTH) ) 
            item.SetWidth(150);
        
        DataBrowserPropertyType type = kDataBrowserTextType;
        wxImageList* imageList = GetImageList(wxIMAGE_LIST_SMALL);
        if (imageList && imageList->GetImageCount() > 0){
            wxBitmap bmp = imageList->GetBitmap(0);
            if (bmp.Ok())
                type = kDataBrowserIconAndTextType;
        }
        
        fprintf(stderr, "Flush is %d\n", item.GetAlign());
        SInt16 just = teFlushDefault;
        if (item.GetMask() & wxLIST_MASK_FORMAT){
            if (item.GetAlign() == wxLIST_FORMAT_LEFT)
                just = teFlushLeft;
            else if (item.GetAlign() == wxLIST_FORMAT_CENTER)
                just = teCenter;
            else if (item.GetAlign() == wxLIST_FORMAT_RIGHT)
                just = teFlushRight;
        }
        m_dbImpl->InsertColumn(col, type, item.GetText(), just, item.GetWidth());
        
        if (GetWindowStyleFlag() & wxLC_EDIT_LABELS)
        {
            DataBrowserTableViewColumnID id;
            m_dbImpl->GetColumnIDFromIndex(col, &id);
            DataBrowserPropertyFlags flags;
            verify_noerr(m_dbImpl->GetPropertyFlags(id, &flags));
            flags |= kDataBrowserPropertyIsEditable;
            verify_noerr(m_dbImpl->SetPropertyFlags(id, flags));
        }
    }
    
    return col;
}

long wxListCtrl::InsertColumn(long col,
                              const wxString& heading,
                              int format,
                              int width)
{
    if (m_genericImpl)
        return m_genericImpl->InsertColumn(col, heading, format, width);

    wxListItem item;
    item.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT;
    item.m_text = heading;
    if ( width > -1 )
    {
        item.m_mask |= wxLIST_MASK_WIDTH;
        item.m_width = width;
    }
    item.m_format = format;

    return InsertColumn(col, item);
}

// scroll the control by the given number of pixels (exception: in list view,
// dx is interpreted as number of columns)
bool wxListCtrl::ScrollList(int dx, int dy)
{
    if (m_genericImpl)
        return m_genericImpl->ScrollList(dx, dy);
        
    if (m_dbImpl){
        m_dbImpl->SetScrollPosition(dx, dy);
    }
    return true;
}


bool wxListCtrl::SortItems(wxListCtrlCompare fn, long data)
{
    if (m_genericImpl)
        return m_genericImpl->SortItems(fn, data);
        
    return true;
}

// ----------------------------------------------------------------------------
// virtual list controls
// ----------------------------------------------------------------------------

wxString wxListCtrl::OnGetItemText(long WXUNUSED(item), long WXUNUSED(col)) const
{
    // this is a pure virtual function, in fact - which is not really pure
    // because the controls which are not virtual don't need to implement it
    wxFAIL_MSG( _T("wxListCtrl::OnGetItemText not supposed to be called") );

    return wxEmptyString;
}

int wxListCtrl::OnGetItemImage(long WXUNUSED(item)) const
{
    wxCHECK_MSG(!GetImageList(wxIMAGE_LIST_SMALL),
                -1,
                wxT("List control has an image list, OnGetItemImage or OnGetItemColumnImage should be overridden."));
    return -1;
}

int wxListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if (!column)
        return OnGetItemImage(item);

    return -1;
}

wxListItemAttr *wxListCtrl::OnGetItemAttr(long WXUNUSED_UNLESS_DEBUG(item)) const
{
    wxASSERT_MSG( item >= 0 && item < GetItemCount(),
                  _T("invalid item index in OnGetItemAttr()") );

    // no attributes by default
    return NULL;
}

void wxListCtrl::SetItemCount(long count)
{
    wxASSERT_MSG( IsVirtual(), _T("this is for virtual controls only") );

    if (m_genericImpl){
        m_genericImpl->SetItemCount(count);
        return;
    }
    
    m_count = count;
}

void wxListCtrl::RefreshItem(long item)
{
    if (m_genericImpl){
        m_genericImpl->RefreshItem(item);
        return;
    }
    
    wxRect rect;
    GetItemRect(item, rect);
    RefreshRect(rect);
}

void wxListCtrl::RefreshItems(long itemFrom, long itemTo)
{
    if (m_genericImpl){
        m_genericImpl->RefreshItems(itemFrom, itemTo);
        return;
    }

    wxRect rect1, rect2;
    GetItemRect(itemFrom, rect1);
    GetItemRect(itemTo, rect2);

    wxRect rect = rect1;
    rect.height = rect2.GetBottom() - rect1.GetTop();

    RefreshRect(rect);
}


// wxMac internal data structures 

wxMacListCtrlItem::~wxMacListCtrlItem()
{
}

void wxMacListCtrlItem::Notification(wxMacDataItemBrowserControl *owner ,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData ) const
{

    wxMacDataBrowserListCtrlControl *lb = dynamic_cast<wxMacDataBrowserListCtrlControl*>(owner);

    // we want to depend on as little as possible to make sure tear-down of controls is safe
    if ( message == kDataBrowserItemRemoved)
    {
        if ( lb != NULL && lb->GetClientDataType() == wxClientData_Object )
        {
            delete (wxClientData*) (m_data);
        }

        delete this;
        return;
    }
    else if ( message == kDataBrowserItemAdded )
    {
        // we don't issue events on adding, the item is not really stored in the list yet, so we
        // avoid asserts by gettting out now
        return  ;
    }

    wxListCtrl *list = wxDynamicCast( owner->GetPeer() , wxListCtrl );
    if ( list ){
        bool trigger = false;
            
        wxListEvent event( wxEVT_COMMAND_LIST_ITEM_SELECTED, list->GetId() );
        bool isSingle = list->GetWindowStyle() | wxLC_SINGLE_SEL;
        event.SetEventObject( list );
        event.m_itemIndex = owner->GetLineFromItem( this ) ;
        if ( !list->IsVirtual() )
        {
            lb->MacGetColumnInfo(event.m_itemIndex,0,event.m_item);
        }
        
        switch (message)
        {
            case kDataBrowserItemDeselected:
                event.SetEventType(wxEVT_COMMAND_LIST_ITEM_DESELECTED);
                if ( !isSingle )
                    trigger = !lb->IsSelectionSuppressed();
                break;

            case kDataBrowserItemSelected:
                trigger = !lb->IsSelectionSuppressed();
                break;

            case kDataBrowserItemDoubleClicked:
                event.SetEventType( wxEVT_LEFT_DCLICK );
                trigger = true;
                break;

            case kDataBrowserEditStarted :
                // TODO : how to veto ?
                event.SetEventType( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT ) ;
                trigger = true ;
                break ;
                
            case kDataBrowserEditStopped :
                // TODO probably trigger only upon the value store callback, because
                // here IIRC we cannot veto
                event.SetEventType( wxEVT_COMMAND_LIST_END_LABEL_EDIT ) ;
                trigger = true ;
                break ;

            default:
                break;
        }

        if ( trigger )
        {
            // direct notification is not always having the listbox GetSelection() having in synch with event
            wxPostEvent( list->GetEventHandler(), event );
        }
    }

}

wxMacDataBrowserListCtrlControl::wxMacDataBrowserListCtrlControl( wxWindow *peer, const wxPoint& pos, const wxSize& size, long style)
    : wxMacDataItemBrowserControl( peer, pos, size, style )
{
    OSStatus err = noErr;
    m_clientDataItemsType = wxClientData_None;

    DataBrowserSelectionFlags  options = kDataBrowserDragSelect;
    if ( style & wxLC_SINGLE_SEL )
    {
        options |= kDataBrowserSelectOnlyOne;
    }
    else
    {
        options |= kDataBrowserAlwaysExtendSelection | kDataBrowserCmdTogglesSelection;
    }

    err = SetSelectionFlags( options );
    verify_noerr( err );

    if ( style & wxLC_LIST ){
        InsertColumn(0, kDataBrowserIconAndTextType, wxEmptyString, -1, -1);
        verify_noerr( AutoSizeColumns() );
        verify_noerr( SetHeaderButtonHeight( 0 ) );
    }

    SetDataBrowserSortProperty( m_controlRef , kMinColumnId );
    if ( style & wxLC_SORT_ASCENDING )
    {
        m_sortOrder = SortOrder_Text_Ascending;
        SetDataBrowserSortProperty( m_controlRef , kMinColumnId );
        SetDataBrowserSortOrder( m_controlRef , kDataBrowserOrderIncreasing);
    }
    else if ( style & wxLC_SORT_DESCENDING )
    {
        m_sortOrder = SortOrder_Text_Descending;
        SetDataBrowserSortProperty( m_controlRef , kMinColumnId );
        SetDataBrowserSortOrder( m_controlRef , kDataBrowserOrderDecreasing);
    }
    else
    {
        m_sortOrder = SortOrder_None;
        SetDataBrowserSortProperty( m_controlRef , kMinColumnId);
        SetDataBrowserSortOrder( m_controlRef , kDataBrowserOrderIncreasing);
    }

    verify_noerr( AutoSizeColumns() );
    verify_noerr( SetHiliteStyle(kDataBrowserTableViewFillHilite ) );
    err = SetHasScrollBars( (style & wxHSCROLL) != 0 , true );
}

wxMacDataBrowserListCtrlControl::~wxMacDataBrowserListCtrlControl()
{
}

void wxMacDataBrowserListCtrlControl::MacSetColumnInfo( unsigned int row, unsigned int column, wxListItem* item )
{ 
    wxMacDataItem* dataItem = GetItemFromLine(row);
    if (item){
        wxMacListCtrlItem* listItem = dynamic_cast<wxMacListCtrlItem*>(dataItem);
        listItem->SetColumnInfo( column, item );
        UpdateState(dataItem, item);
    }
}

// apply changes that need to happen immediately, rather than when the 
// databrowser control fires a callback.
void wxMacDataBrowserListCtrlControl::UpdateState(wxMacDataItem* dataItem, wxListItem* listItem)
{
    bool isSelected = IsItemSelected( dataItem );
    bool isSelectedState = (listItem->GetState() == wxLIST_STATE_SELECTED);

    // toggle the selection state if wxListInfo state and actual state don't match.    
    if ( isSelected != isSelectedState ){
        DataBrowserSetOption options = kDataBrowserItemsAdd;
        if (!isSelectedState)
            options = kDataBrowserItemsRemove;
        fprintf(stderr, "state = %d, db options = %d\n", isSelectedState, options);
        SetSelectedItem(dataItem, options);
    }
    // TODO: Set column width if item width > than current column width
}

void wxMacDataBrowserListCtrlControl::MacGetColumnInfo( unsigned int row, unsigned int column, wxListItem& item )
{ 
    wxMacDataItem* dataItem = GetItemFromLine(row);
    // CS should this guard against dataItem = 0 ? , as item is not a pointer if (item) is not appropriate
    //if (item) 
    {
        wxMacListCtrlItem* listItem = dynamic_cast<wxMacListCtrlItem*>(dataItem);
        wxListItem* oldItem = listItem->GetColumnInfo( column );
        
        long mask = item.GetMask();
        if ( !mask )
            // by default, get everything for backwards compatibility
            mask = -1;

        if ( mask & wxLIST_MASK_TEXT )
            item.SetText(oldItem->GetText());
        if ( mask & wxLIST_MASK_IMAGE )
            item.SetImage(oldItem->GetImage());
        if ( mask & wxLIST_MASK_DATA )
            item.SetData(oldItem->GetData());
        if ( mask & wxLIST_MASK_STATE )
            item.SetState(oldItem->GetState());
        if ( mask & wxLIST_MASK_WIDTH )
            item.SetWidth(oldItem->GetWidth());
        if ( mask & wxLIST_MASK_FORMAT )
            item.SetAlign(oldItem->GetAlign());

        item.SetTextColour(oldItem->GetTextColour());
        item.SetBackgroundColour(oldItem->GetBackgroundColour());
        item.SetFont(oldItem->GetFont());   
    }
}

void wxMacDataBrowserListCtrlControl::MacInsertItem( unsigned int n, wxListItem* item )
{
    wxMacDataItemBrowserControl::MacInsert(n, item->GetText());
    MacSetColumnInfo(n, 0, item);
}

wxMacListCtrlItem* wxMacDataBrowserListCtrlControl::CreateItem()
{
    return new wxMacListCtrlItem();
}

wxMacListCtrlItem::wxMacListCtrlItem()
{
    m_rowItems = wxListItemList();
}

int wxMacListCtrlItem::GetColumnImageValue( unsigned int column )
{
    return GetColumnInfo(column)->GetImage();
}

void wxMacListCtrlItem::SetColumnImageValue( unsigned int column, int imageIndex )
{
    GetColumnInfo(column)->SetImage(imageIndex);
}

const wxString& wxMacListCtrlItem::GetColumnTextValue( unsigned int column )
{
    if ( column == 0 )
        return GetLabel();

    return GetColumnInfo(column)->GetText();
}

void wxMacListCtrlItem::SetColumnTextValue( unsigned int column, const wxString& text )
{
    GetColumnInfo(column)->SetText(text);
    
    // for compatibility with superclass APIs
    if ( column == 0 )
        SetLabel(text);
}

wxListItem* wxMacListCtrlItem::GetColumnInfo( unsigned int column )
{
    wxListItemList::compatibility_iterator node = m_rowItems.Item( column );
    wxASSERT_MSG( node, _T("invalid column index in wxMacListCtrlItem") );

    return node->GetData();
}

bool wxMacListCtrlItem::HasColumnInfo( unsigned int column )
{
    return m_rowItems.GetCount() > column;
}

void wxMacListCtrlItem::SetColumnInfo( unsigned int column, wxListItem* item )
{

    if ( column >= m_rowItems.GetCount() ){
        wxListItem* listItem = new wxListItem(*item);
        //listItem->SetAlign(item->GetAlign());
        //listItem->SetBackgroundColour(item->GetBackgroundColour());
        //listItem->SetColumn(item->GetColumn());
        //listItem->SetData(item->GetData());
        //listItem->SetFont(item->GetFont());
        //listItem->SetId(GetOrder());
        //listItem->SetImage(item->GetImage());
        //listItem->SetMask(item->GetMask());
        
        //listItem->SetText(item->GetText());      
        m_rowItems.Append( listItem );
    }
    else{
        wxListItem* listItem = GetColumnInfo( column );
        long mask = item->GetMask();
        if (mask & wxLIST_MASK_TEXT)
            listItem->SetText(item->GetText());
        if (mask & wxLIST_MASK_DATA)
            listItem->SetData(item->GetData());
        if (mask & wxLIST_MASK_IMAGE)
            listItem->SetImage(item->GetImage());
        if (mask & wxLIST_MASK_STATE)
            listItem->SetState(item->GetState());
        if (mask & wxLIST_MASK_FORMAT)
            listItem->SetAlign(item->GetAlign());
        if (mask & wxLIST_MASK_WIDTH)
            listItem->SetWidth(item->GetWidth());
    }
}

OSStatus wxMacListCtrlItem::GetSetData( wxMacDataItemBrowserControl *owner ,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    bool changeValue )
{
        
    OSStatus err = errDataBrowserPropertyNotSupported;
    wxListCtrl* list = wxDynamicCast( owner->GetPeer() , wxListCtrl );
    if ( !changeValue )
    {
        switch (property)
        {
            case kDataBrowserItemIsEditableProperty :
                if ( list && list->HasFlag( wxLC_EDIT_LABELS ) )
                {
                    verify_noerr(SetDataBrowserItemDataBooleanValue( itemData, true ));
                    err = noErr ;
                }
                break ;
            default :
                if ( property >= kMinColumnId ){
                    short listColumn = property - kMinColumnId;
                    
                    if (HasColumnInfo(listColumn)){
                        wxListItem* item = GetColumnInfo(listColumn);
                        wxMacCFStringHolder cfStr;
                        
                        if (item->GetText()){
                            cfStr.Assign( item->GetText(), wxLocale::GetSystemEncoding() );
                            err = ::SetDataBrowserItemDataText( itemData, cfStr );
                            err = noErr;
                        }
                        
                        int imgIndex = item->GetImage();
                        if ( (item->GetMask() & wxLIST_MASK_IMAGE) ){
                            wxImageList* imageList = list->GetImageList(wxIMAGE_LIST_SMALL);
                            if (imageList && imageList->GetImageCount() > 0){
                                wxBitmap bmp = imageList->GetBitmap(imgIndex);
                                IconRef icon = bmp.GetBitmapData()->GetIconRef();
                                ::SetDataBrowserItemDataIcon(itemData, icon);
                            }
                        }
                        
                    }
                }
                break ;
        }
    }
    else
    {
        switch (property)
        {
             default:
                if ( property >= kMinColumnId ){
                    short listColumn = property - kMinColumnId;
                    
                    if (HasColumnInfo(listColumn)){
                        // TODO probably send the 'end edit' from here, as we
                        // can then deal with the veto
                        CFStringRef sr ;
                        verify_noerr( GetDataBrowserItemDataText( itemData , &sr ) ) ;
                        wxMacCFStringHolder cfStr(sr) ;;
                        list->SetItem( owner->GetLineFromItem(this) , listColumn, cfStr.AsString() ) ;
                        err = noErr ;                        
                    }
                }
                break;
        }
    }
    
    // don't duplicate the numeric order column handling
    if (err == errDataBrowserPropertyNotSupported) 
        err = wxMacDataItem::GetSetData(owner, property, itemData, changeValue);
        
    return err;
}

#endif // wxUSE_LISTCTRL
