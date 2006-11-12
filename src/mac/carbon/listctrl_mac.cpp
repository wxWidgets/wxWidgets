/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/listctrl_mac.cpp
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

#include "wx/listctrl.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/mac/uma.h"

#include "wx/imaglist.h"
#include "wx/sysopt.h"
#include "wx/timer.h"

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
WX_DEFINE_LIST(wxColumnList)

// so we can check for column clicks
static const EventTypeSpec eventList[] =
{
    { kEventClassControl, kEventControlHit },
    { kEventClassControl, kEventControlDraw }
};

static pascal OSStatus wxMacListCtrlEventHandler( EventHandlerCallRef handler , EventRef event , void *data )
{
    OSStatus result = eventNotHandledErr ;

    wxMacCarbonEvent cEvent( event ) ;

    ControlRef controlRef ;
    cEvent.GetParameter( kEventParamDirectObject , &controlRef ) ;

    wxListCtrl *window = (wxListCtrl*) data ;
    wxListEvent le( wxEVT_COMMAND_LIST_COL_CLICK, window->GetId() );
    le.SetEventObject( window );

    switch ( GetEventKind( event ) )
    {
        // check if the column was clicked on and fire an event if so
        case kEventControlHit :
            {
                ControlPartCode result = cEvent.GetParameter<ControlPartCode>(kEventParamControlPart, typeControlPartCode) ;
                if (result == kControlButtonPart){
                    DataBrowserPropertyID col;
                    GetDataBrowserSortProperty(controlRef, &col);
                    int column = col - kMinColumnId;
                    le.m_col = column;
                    window->GetEventHandler()->ProcessEvent( le );
                }
                result = CallNextEventHandler(handler, event);
                break;
            }
        case kEventControlDraw:
            {
                CGContextRef context = cEvent.GetParameter<CGContextRef>(kEventParamCGContextRef, typeCGContextRef) ;
                window->MacSetDrawingContext(context);
                result = CallNextEventHandler(handler, event);
                window->MacSetDrawingContext(NULL);
                break;
            }
        default :
            break ;
    }


    return result ;
}

DEFINE_ONE_SHOT_HANDLER_GETTER( wxMacListCtrlEventHandler )

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
    virtual wxString GetColumnTextValue( unsigned int column );

    virtual int GetColumnImageValue( unsigned int column );
    virtual void SetColumnImageValue( unsigned int column, int imageIndex );

    virtual ~wxMacListCtrlItem();
protected:
    wxListItemList m_rowItems;
};

DataBrowserDrawItemUPP gDataBrowserDrawItemUPP = NULL;
//DataBrowserEditItemUPP gDataBrowserEditItemUPP = NULL;
DataBrowserHitTestUPP gDataBrowserHitTestUPP = NULL;

// TODO: Make a better name!!
class wxMacDataBrowserListCtrlControl : public wxMacDataItemBrowserControl
{
public:
    wxMacDataBrowserListCtrlControl( wxWindow *peer, const wxPoint& pos, const wxSize& size, long style );
    virtual ~wxMacDataBrowserListCtrlControl();

    // create a list item (can be a subclass of wxMacListBoxItem)

    virtual wxMacDataItem* CreateItem();

    virtual void MacInsertItem( unsigned int n, wxListItem* item );
    virtual void MacSetColumnInfo( unsigned int row, unsigned int column, wxListItem* item );
    virtual void MacGetColumnInfo( unsigned int row, unsigned int column, wxListItem& item );
    virtual void UpdateState(wxMacDataItem* dataItem, wxListItem* item);

protected:
    // we need to override to provide specialized handling for virtual wxListCtrls
    virtual OSStatus GetSetItemData(DataBrowserItemID itemID,
                        DataBrowserPropertyID property,
                        DataBrowserItemDataRef itemData,
                        Boolean changeValue );

    virtual void    ItemNotification(
                        DataBrowserItemID itemID,
                        DataBrowserItemNotification message,
                        DataBrowserItemDataRef itemData);

    virtual Boolean CompareItems(DataBrowserItemID itemOneID,
                        DataBrowserItemID itemTwoID,
                        DataBrowserPropertyID sortProperty);

    static pascal void    DataBrowserDrawItemProc(ControlRef browser,
                        DataBrowserItemID item,
                        DataBrowserPropertyID property,
                        DataBrowserItemState itemState,
                        const Rect *theRect,
                        SInt16 gdDepth,
                        Boolean colorDevice);

    virtual void        DrawItem(DataBrowserItemID itemID,
                            DataBrowserPropertyID property,
                            DataBrowserItemState itemState,
                            const Rect *itemRect,
                            SInt16 gdDepth,
                            Boolean colorDevice);

    static pascal Boolean  DataBrowserEditTextProc(ControlRef browser,
                                    DataBrowserItemID item,
                                    DataBrowserPropertyID property,
                                    CFStringRef theString,
                                    Rect *maxEditTextRect,
                                    Boolean *shrinkToFit);

    static pascal Boolean  DataBrowserHitTestProc(ControlRef browser,
                                    DataBrowserItemID itemID,
                                    DataBrowserPropertyID property,
                                    const Rect *theRect,
                                    const Rect *mouseRect) { return true; }

    virtual bool        ConfirmEditText(DataBrowserItemID item,
                                    DataBrowserPropertyID property,
                                    CFStringRef theString,
                                    Rect *maxEditTextRect,
                                    Boolean *shrinkToFit);



    wxClientDataType m_clientDataItemsType;
    bool m_isVirtual;

};

class wxMacListCtrlEventDelegate : public wxEvtHandler
{
public:
    wxMacListCtrlEventDelegate( wxListCtrl* list, int id );
    virtual bool ProcessEvent( wxEvent& event );

private:
    wxListCtrl* m_list;
    int         m_id;
};

wxMacListCtrlEventDelegate::wxMacListCtrlEventDelegate( wxListCtrl* list, int id )
{
    m_list = list;
    m_id = id;
}

bool wxMacListCtrlEventDelegate::ProcessEvent( wxEvent& event )
{
    // even though we use a generic list ctrl underneath, make sure
    // we present ourselves as wxListCtrl.
    event.SetEventObject( m_list );
    event.SetId( m_id );

    if ( !event.IsKindOf( CLASSINFO( wxCommandEvent ) ) )
    {
        if (m_list->GetEventHandler()->ProcessEvent( event ))
            return true;
    }
    return wxEvtHandler::ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// wxListCtrlRenameTimer (internal)
//-----------------------------------------------------------------------------

class wxListCtrlRenameTimer: public wxTimer
{
private:
    wxListCtrl *m_owner;

public:
    wxListCtrlRenameTimer( wxListCtrl *owner );
    void Notify();
};

//-----------------------------------------------------------------------------
// wxListCtrlTextCtrlWrapper: wraps a wxTextCtrl to make it work for inline editing
//-----------------------------------------------------------------------------

class wxListCtrlTextCtrlWrapper : public wxEvtHandler
{
public:
    // NB: text must be a valid object but not Create()d yet
    wxListCtrlTextCtrlWrapper(wxListCtrl *owner,
                          wxTextCtrl *text,
                          long itemEdit);

    wxTextCtrl *GetText() const { return m_text; }

    void AcceptChangesAndFinish();

protected:
    void OnChar( wxKeyEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    bool AcceptChanges();
    void Finish();

private:
    wxListCtrl         *m_owner;
    wxTextCtrl         *m_text;
    wxString            m_startValue;
    long              m_itemEdited;
    bool                m_finished;
    bool                m_aboutToFinish;

    DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxListCtrlRenameTimer (internal)
//-----------------------------------------------------------------------------

wxListCtrlRenameTimer::wxListCtrlRenameTimer( wxListCtrl *owner )
{
    m_owner = owner;
}

void wxListCtrlRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxListCtrlTextCtrlWrapper (internal)
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxListCtrlTextCtrlWrapper, wxEvtHandler)
    EVT_CHAR           (wxListCtrlTextCtrlWrapper::OnChar)
    EVT_KEY_UP         (wxListCtrlTextCtrlWrapper::OnKeyUp)
    EVT_KILL_FOCUS     (wxListCtrlTextCtrlWrapper::OnKillFocus)
END_EVENT_TABLE()

wxListCtrlTextCtrlWrapper::wxListCtrlTextCtrlWrapper(wxListCtrl *owner,
                                             wxTextCtrl *text,
                                             long itemEdit)
              : m_startValue(owner->GetItemText(itemEdit)),
                m_itemEdited(itemEdit)
{
    m_owner = owner;
    m_text = text;
    m_finished = false;
    m_aboutToFinish = false;

    wxRect rectLabel;
    int offset = 8;
    owner->GetItemRect(itemEdit, rectLabel);

    m_text->Create(owner, wxID_ANY, m_startValue,
                   wxPoint(rectLabel.x+offset,rectLabel.y),
                   wxSize(rectLabel.width-offset,rectLabel.height));
    m_text->SetFocus();

    m_text->PushEventHandler(this);
}

void wxListCtrlTextCtrlWrapper::Finish()
{
    if ( !m_finished )
    {
        m_finished = true;

        m_text->RemoveEventHandler(this);
        m_owner->FinishEditing(m_text);

        wxPendingDelete.Append( this );
    }
}

bool wxListCtrlTextCtrlWrapper::AcceptChanges()
{
    const wxString value = m_text->GetValue();

    if ( value == m_startValue )
        // nothing changed, always accept
        return true;

    if ( !m_owner->OnRenameAccept(m_itemEdited, value) )
        // vetoed by the user
        return false;

    // accepted, do rename the item
    m_owner->SetItemText(m_itemEdited, value);

    return true;
}

void wxListCtrlTextCtrlWrapper::AcceptChangesAndFinish()
{
    m_aboutToFinish = true;

    // Notify the owner about the changes
    AcceptChanges();

    // Even if vetoed, close the control (consistent with MSW)
    Finish();
}

void wxListCtrlTextCtrlWrapper::OnChar( wxKeyEvent &event )
{
    switch ( event.m_keyCode )
    {
        case WXK_RETURN:
            AcceptChangesAndFinish();
            break;

        case WXK_ESCAPE:
            m_owner->OnRenameCancelled( m_itemEdited );
            Finish();
            break;

        default:
            event.Skip();
    }
}

void wxListCtrlTextCtrlWrapper::OnKeyUp( wxKeyEvent &event )
{
    if (m_finished)
    {
        event.Skip();
        return;
    }

    // auto-grow the textctrl:
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = m_text->GetPosition();
    wxSize mySize = m_text->GetSize();
    int sx, sy;
    m_text->GetTextExtent(m_text->GetValue() + _T("MM"), &sx, &sy);
    if (myPos.x + sx > parentSize.x)
        sx = parentSize.x - myPos.x;
    if (mySize.x > sx)
        sx = mySize.x;
    m_text->SetSize(sx, wxDefaultCoord);

    event.Skip();
}

void wxListCtrlTextCtrlWrapper::OnKillFocus( wxFocusEvent &event )
{
    if ( !m_finished && !m_aboutToFinish )
    {
        if ( !AcceptChanges() )
            m_owner->OnRenameCancelled( m_itemEdited );

        Finish();
    }

    // We must let the native text control handle focus
    event.Skip();
}

BEGIN_EVENT_TABLE(wxListCtrl, wxControl)
    EVT_LEFT_DOWN(wxListCtrl::OnLeftDown)
    EVT_LEFT_DCLICK(wxListCtrl::OnDblClick)
END_EVENT_TABLE()

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
    m_compareFunc = NULL;
    m_compareFuncData = 0;
    m_colsInfo = wxColumnList();
    m_textColor = wxNullColour;
    m_bgColor = wxNullColour;
    m_textctrlWrapper = NULL;
    m_current = -1;
    m_renameTimer = new wxListCtrlRenameTimer( this );
}

class wxGenericListCtrlHook : public wxGenericListCtrl
{
public:
    wxGenericListCtrlHook(wxListCtrl* parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxValidator& validator,
                          const wxString& name)
        : wxGenericListCtrl(parent, id, pos, size, style, validator, name),
          m_nativeListCtrl(parent)
    {
    }

protected:
    virtual wxListItemAttr * OnGetItemAttr(long item) const
    {
        return m_nativeListCtrl->OnGetItemAttr(item);
    }

    virtual int OnGetItemImage(long item) const
    {
        return m_nativeListCtrl->OnGetItemImage(item);
    }

    virtual int OnGetItemColumnImage(long item, long column) const
    {
        return m_nativeListCtrl->OnGetItemColumnImage(item, column);
    }

    virtual wxString OnGetItemText(long item, long column) const
    {
        return m_nativeListCtrl->OnGetItemText(item, column);
    }

    wxListCtrl* m_nativeListCtrl;

};

void wxListCtrl::OnLeftDown(wxMouseEvent& event)
{
    if ( m_textctrlWrapper )
    {
        m_current = -1;
        m_textctrlWrapper->AcceptChangesAndFinish();
    }

    int hitResult;
    long current = HitTest(event.GetPosition(), hitResult);
    if ((current == m_current) &&
        (hitResult == wxLIST_HITTEST_ONITEM) &&
        HasFlag(wxLC_EDIT_LABELS) )
    {
        m_renameTimer->Start( 100, true );
    }
    else
    {
        m_current = current;
    }
    event.Skip();
}

void wxListCtrl::OnDblClick(wxMouseEvent& event)
{
    m_current = -1;
    event.Skip();
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
            (style & wxLC_ICON) || (style & wxLC_SMALL_ICON) || (style & wxLC_LIST) )
    {
        m_macIsUserPane = true;

        long paneStyle = style;
        paneStyle &= ~wxSIMPLE_BORDER;
        paneStyle &= ~wxDOUBLE_BORDER;
        paneStyle &= ~wxSUNKEN_BORDER;
        paneStyle &= ~wxRAISED_BORDER;
        paneStyle &= ~wxSTATIC_BORDER;
        if ( !wxWindow::Create(parent, id, pos, size, paneStyle | wxNO_BORDER, name) )
            return false;

        // since the generic control is a child, make sure we position it at 0, 0
        m_genericImpl = new wxGenericListCtrlHook(this, id, wxPoint(0, 0), size, style, validator, name);
        m_genericImpl->PushEventHandler( new wxMacListCtrlEventDelegate( this, GetId() ) );
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

        InstallControlEventHandler( m_peer->GetControlRef() , GetwxMacListCtrlEventHandlerUPP(),
            GetEventTypeCount(eventList), eventList, this,
            (EventHandlerRef *)&m_macListCtrlEventHandler);
    }

    return true;
}

wxListCtrl::~wxListCtrl()
{
    if (m_genericImpl)
    {
        m_genericImpl->PopEventHandler(/* deleteHandler = */ true);
    }

    if (m_ownsImageListNormal)
        delete m_imageListNormal;
    if (m_ownsImageListSmall)
        delete m_imageListSmall;
    if (m_ownsImageListState)
        delete m_imageListState;

    delete m_renameTimer;
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

        if (m_genericImpl)
        {
            m_genericImpl->SetWindowStyleFlag(flag);
        }

        Refresh();
    }
}

void wxListCtrl::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);

    if (m_genericImpl)
        m_genericImpl->SetSize(0, 0, width, height, sizeFlags);

    // determine if we need a horizontal scrollbar, and add it if so
    if (m_dbImpl)
    {
        int totalWidth = 0;
        for (int column = 0; column < GetColumnCount(); column++)
        {
              totalWidth += m_dbImpl->GetColumnWidth( column );
        }

        Boolean vertScrollBar;
        GetDataBrowserHasScrollBars( m_dbImpl->GetControlRef(), NULL, &vertScrollBar );
        if (totalWidth > width)
            SetDataBrowserHasScrollBars( m_dbImpl->GetControlRef(), true, vertScrollBar );
        else
            SetDataBrowserHasScrollBars( m_dbImpl->GetControlRef(), false, vertScrollBar );
    }
}

wxSize wxListCtrl::DoGetBestSize() const
{
    return wxWindow::DoGetBestSize();
}

bool wxListCtrl::SetFont(const wxFont& font)
{
    bool rv = true;
    rv = wxControl::SetFont(font);
    if (m_genericImpl)
        rv = m_genericImpl->SetFont(font);
    return rv;
}

bool wxListCtrl::SetForegroundColour(const wxColour& colour)
{
    bool rv = true;
    if (m_genericImpl)
        rv = m_genericImpl->SetForegroundColour(colour);
    if (m_dbImpl)
        SetTextColour(colour);
    return rv;
}

bool wxListCtrl::SetBackgroundColour(const wxColour& colour)
{
    bool rv = true;
    if (m_genericImpl)
        rv = m_genericImpl->SetBackgroundColour(colour);
    if (m_dbImpl)
        m_bgColor = colour;
    return rv;
}

wxColour wxListCtrl::GetBackgroundColour()
{
    if (m_genericImpl)
        return m_genericImpl->GetBackgroundColour();
    if (m_dbImpl)
        return m_bgColor;

    return wxNullColour;
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

    if (m_dbImpl)
    {

        wxColumnList::compatibility_iterator node = m_colsInfo.Item( col );
        wxASSERT_MSG( node, _T("invalid column index in wxMacListCtrlItem") );
        wxListItem* column = node->GetData();

        long mask = column->GetMask();
        if (mask & wxLIST_MASK_TEXT)
            item.SetText(column->GetText());
        if (mask & wxLIST_MASK_DATA)
            item.SetData(column->GetData());
        if (mask & wxLIST_MASK_IMAGE)
            item.SetImage(column->GetImage());
        if (mask & wxLIST_MASK_STATE)
            item.SetState(column->GetState());
        if (mask & wxLIST_MASK_FORMAT)
            item.SetAlign(column->GetAlign());
        if (mask & wxLIST_MASK_WIDTH)
            item.SetWidth(column->GetWidth());
    }

    return success;
}

// Sets information about this column
bool wxListCtrl::SetColumn(int col, wxListItem& item)
{
    if (m_genericImpl)
        return m_genericImpl->SetColumn(col, item);

    if (m_dbImpl)
    {
        long mask = item.GetMask();
        if ( col >= (int)m_colsInfo.GetCount() )
        {
            wxListItem* listItem = new wxListItem(item);
            m_colsInfo.Append( listItem );
        }
        else
        {
            wxListItem listItem;
            GetColumn( col, listItem );

            if (mask & wxLIST_MASK_TEXT)
                listItem.SetText(item.GetText());
            if (mask & wxLIST_MASK_DATA)
                listItem.SetData(item.GetData());
            if (mask & wxLIST_MASK_IMAGE)
                listItem.SetImage(item.GetImage());
            if (mask & wxLIST_MASK_STATE)
                listItem.SetState(item.GetState());
            if (mask & wxLIST_MASK_FORMAT)
                listItem.SetAlign(item.GetAlign());
            if (mask & wxLIST_MASK_WIDTH)
                listItem.SetWidth(item.GetWidth());
        }

        // change the appearance in the databrowser.
        DataBrowserListViewHeaderDesc columnDesc;
        columnDesc.version=kDataBrowserListViewLatestHeaderDesc;
        verify_noerr( m_dbImpl->GetHeaderDesc( kMinColumnId + col, &columnDesc ) );

        /*
        if (item.GetMask() & wxLIST_MASK_TEXT)
        {
            wxFontEncoding enc;
            if ( m_font.Ok() )
                enc = m_font.GetEncoding();
            else
                enc = wxLocale::GetSystemEncoding();
            wxMacCFStringHolder cfTitle;
            cfTitle.Assign( item.GetText() , enc );
            if(columnDesc.titleString)
                CFRelease(columnDesc.titleString);
            columnDesc.titleString = cfTitle;
        }
        */

        if (item.GetMask() & wxLIST_MASK_IMAGE && item.GetImage() != -1 )
        {
            columnDesc.btnContentInfo.contentType = kControlContentIconRef;
            wxImageList* imageList = GetImageList(wxIMAGE_LIST_SMALL);
            if (imageList && imageList->GetImageCount() > 0 )
            {
                wxBitmap bmp = imageList->GetBitmap( item.GetImage() );
                IconRef icon = bmp.GetBitmapData()->GetIconRef();
                columnDesc.btnContentInfo.u.iconRef = icon;
            }
        }

        verify_noerr( m_dbImpl->SetHeaderDesc( kMinColumnId + col, &columnDesc ) );

    }
    return true;
}

int wxListCtrl::GetColumnCount() const
{
    if (m_genericImpl)
        return m_genericImpl->GetColumnCount();

    if (m_dbImpl)
    {
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

    if (m_dbImpl)
    {
        return m_dbImpl->GetColumnWidth(col);
    }

    return 0;
}

// Sets the column width
bool wxListCtrl::SetColumnWidth(int col, int width)
{
    if (m_genericImpl)
        return m_genericImpl->SetColumnWidth(col, width);

    if (m_dbImpl)
    {
        int mywidth = width;
        if (width == wxLIST_AUTOSIZE || width == wxLIST_AUTOSIZE_USEHEADER)
            mywidth = 150;

        if (col == -1)
        {
            for (int column = 0; column < GetColumnCount(); column++)
            {
                wxListItem colInfo;
                GetColumn(col, colInfo);

                colInfo.SetWidth(width);
                SetColumn(col, colInfo);

                m_dbImpl->SetColumnWidth(col, mywidth);
            }
        }
        else
        {
            wxListItem colInfo;
            GetColumn(col, colInfo);

            colInfo.SetWidth(width);
            SetColumn(col, colInfo);
            m_dbImpl->SetColumnWidth(col, mywidth);
        }
        return true;
    }

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

    if (m_dbImpl)
    {
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
    {
        if (!IsVirtual())
            m_dbImpl->MacGetColumnInfo(info.m_itemId, info.m_col, info);
        else
        {
            info.SetText( OnGetItemText(info.m_itemId, info.m_col) );
            info.SetImage( OnGetItemColumnImage(info.m_itemId, info.m_col) );
            wxListItemAttr* attrs = OnGetItemAttr( info.m_itemId );
            if (attrs)
            {
                info.SetFont( attrs->GetFont() );
                info.SetBackgroundColour( attrs->GetBackgroundColour() );
                info.SetTextColour( attrs->GetTextColour() );
            }
        }
    }
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
    info.m_itemId = item;
    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_state = state;
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


    if (m_dbImpl)
    {
        DataBrowserItemID id;
        DataBrowserPropertyID col = kMinColumnId;
        Rect bounds;
        DataBrowserPropertyPart part = kDataBrowserPropertyEnclosingPart;
        if ( code == wxLIST_RECT_LABEL )
            part = kDataBrowserPropertyTextPart;
        else if ( code == wxLIST_RECT_ICON )
            part = kDataBrowserPropertyIconPart;

        if ( !(GetWindowStyleFlag() & wxLC_VIRTUAL) )
        {
            wxMacDataItem* thisItem = m_dbImpl->GetItemFromLine(item);
            id = (DataBrowserItemID) thisItem;
        }
        else
            id = item+1;

        GetDataBrowserItemPartBounds( m_dbImpl->GetControlRef(), id, col, part, &bounds );

        rect.x = bounds.left;
        rect.y = bounds.top;
        rect.width = bounds.right - bounds.left; //GetClientSize().x; // we need the width of the whole row, not just the item.
        rect.height = bounds.bottom - bounds.top;
        //fprintf("id = %d, bounds = %d, %d, %d, %d\n", id, rect.x, rect.y, rect.width, rect.height);
    }
    return true;
}

// Gets the item position
bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    if (m_genericImpl)
        return m_genericImpl->GetItemPosition(item, pos);

    bool success = false;

    if (m_dbImpl)
    {
        wxRect itemRect;
        GetItemRect(item, itemRect);
        pos = itemRect.GetPosition();
        success = true;
    }

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
    if (m_genericImpl)
    {
        m_genericImpl->SetItemTextColour(item, col);
        return;
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

    if (m_dbImpl)
    {
        wxListItem info;
        if (GetItem(info))
            return info.GetTextColour();
    }
    return wxNullColour;
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col )
{
    if (m_genericImpl)
    {
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

    if (m_dbImpl)
    {
        wxListItem info;
        if (GetItem(info))
            return info.GetBackgroundColour();
    }
    return wxNullColour;
}

void wxListCtrl::SetItemFont( long item, const wxFont &f )
{
    if (m_genericImpl)
    {
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

    if (m_dbImpl)
    {
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
        return m_textColor;

    return wxNullColour;
}

// Sets the text colour of the listview
void wxListCtrl::SetTextColour(const wxColour& col)
{
    if (m_genericImpl)
    {
        m_genericImpl->SetTextColour(col);
        return;
    }

    if (m_dbImpl)
        m_textColor = col;
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

    if (m_dbImpl && geom == wxLIST_NEXT_ALL && state == wxLIST_STATE_SELECTED )
    {
        long count = m_dbImpl->MacGetCount() ;
        for ( long line = item + 1 ; line < count; line++ )
        {
            wxMacDataItem* id = m_dbImpl->GetItemFromLine(line);
            if ( m_dbImpl->IsItemSelected(id ) )
                return line;
        }
        return -1;
    }

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
    if (m_genericImpl)
    {
        m_genericImpl->SetImageList(imageList, which);
        return;
    }

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
    if (m_genericImpl)
    {
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

    if (m_dbImpl)
    {
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

    if (m_dbImpl)
    {
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

    if (m_dbImpl)
    {
        UInt32 cols;
        m_dbImpl->GetColumnCount(&cols);
        for (UInt32 col = 0; col < cols; col++)
        {
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

    if (m_dbImpl)
    {
        OSStatus err = m_dbImpl->RemoveColumn(col);
        return err == noErr;
    }

    return true;
}

// Clears items, and columns if there are any.
void wxListCtrl::ClearAll()
{
    if (m_genericImpl)
    {
        m_genericImpl->ClearAll();
        return;
    }

    if (m_dbImpl)
    {
        DeleteAllItems();
        DeleteAllColumns();
    }
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    if (m_genericImpl)
        return m_genericImpl->EditLabel(item, textControlClass);

    if (m_dbImpl)
    {
        wxCHECK_MSG( (item >= 0) && ((long)item < GetItemCount()), NULL,
                     wxT("wrong index in wxListCtrl::EditLabel()") );

        wxASSERT_MSG( textControlClass->IsKindOf(CLASSINFO(wxTextCtrl)),
                     wxT("EditLabel() needs a text control") );

        wxListEvent le( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, GetParent()->GetId() );
        le.SetEventObject( this );
        le.m_itemIndex = item;
        le.m_col = 0;
        GetItem( le.m_item );

        if ( GetParent()->GetEventHandler()->ProcessEvent( le ) && !le.IsAllowed() )
        {
            // vetoed by user code
            return NULL;
        }

        wxTextCtrl * const text = (wxTextCtrl *)textControlClass->CreateObject();
        m_textctrlWrapper = new wxListCtrlTextCtrlWrapper(this, text, item);
        return m_textctrlWrapper->GetText();
    }
    return NULL;
}

// End label editing, optionally cancelling the edit
bool wxListCtrl::EndEditLabel(bool cancel)
{
    // TODO: generic impl. doesn't have this method - is it needed for us?
    if (m_genericImpl)
        return true; // m_genericImpl->EndEditLabel(cancel);

    if (m_dbImpl)
        verify_noerr( SetDataBrowserEditItem(m_dbImpl->GetControlRef(), kDataBrowserNoItem, kMinColumnId) );
    return true;
}

// Ensures this item is visible
bool wxListCtrl::EnsureVisible(long item)
{
    if (m_genericImpl)
        return m_genericImpl->EnsureVisible(item);

    if (m_dbImpl)
    {
        wxMacDataItem* dataItem = m_dbImpl->GetItemFromLine(item);
        m_dbImpl->RevealItem(dataItem, kDataBrowserRevealWithoutSelecting);
    }

    return true;
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

    flags = wxLIST_HITTEST_NOWHERE;
    if (m_dbImpl)
    {
        int colHeaderHeight = 22; // TODO: Find a way to get this value from the db control?
        UInt16 rowHeight = 0;
        m_dbImpl->GetDefaultRowHeight(&rowHeight);

        int y = point.y;
        // get the actual row by taking scroll position into account
        UInt32 offsetX, offsetY;
        m_dbImpl->GetScrollPosition( &offsetY, &offsetX );
        y += offsetY;

        if ( !(GetWindowStyleFlag() & wxLC_NO_HEADER) )
            y -= colHeaderHeight;

        if ( y < 0 )
            return -1;

        int row = y / rowHeight;
        DataBrowserItemID id;
        m_dbImpl->GetItemID( (DataBrowserTableViewRowIndex) row, &id );

        // TODO: Use GetDataBrowserItemPartBounds to return if we are in icon or label
        if ( !(GetWindowStyleFlag() & wxLC_VIRTUAL ) )
        {
            wxMacListCtrlItem* lcItem;
            lcItem = (wxMacListCtrlItem*) id;
            if (lcItem)
            {
                flags = wxLIST_HITTEST_ONITEM;
                return row;
            }
        }
        else
        {
            if (row < GetItemCount() )
            {
                flags = wxLIST_HITTEST_ONITEM;
                return row;
            }
        }

    }
    return -1;
}


// Inserts an item, returning the index of the new item if successful,
// -1 otherwise.
long wxListCtrl::InsertItem(wxListItem& info)
{
    wxASSERT_MSG( !IsVirtual(), _T("can't be used with virtual controls") );

    if (m_genericImpl)
        return m_genericImpl->InsertItem(info);

    if (m_dbImpl)
    {
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

    if (m_dbImpl)
    {
        int width = item.GetWidth();
        if ( !(item.GetMask() & wxLIST_MASK_WIDTH) )
            width = 150;

        DataBrowserPropertyType type = kDataBrowserCustomType; //kDataBrowserTextType;
        wxImageList* imageList = GetImageList(wxIMAGE_LIST_SMALL);
        if (imageList && imageList->GetImageCount() > 0)
        {
            wxBitmap bmp = imageList->GetBitmap(0);
            //if (bmp.Ok())
            //    type = kDataBrowserIconAndTextType;
        }

        SInt16 just = teFlushDefault;
        if (item.GetMask() & wxLIST_MASK_FORMAT)
        {
            if (item.GetAlign() == wxLIST_FORMAT_LEFT)
                just = teFlushLeft;
            else if (item.GetAlign() == wxLIST_FORMAT_CENTER)
                just = teCenter;
            else if (item.GetAlign() == wxLIST_FORMAT_RIGHT)
                just = teFlushRight;
        }
        m_dbImpl->InsertColumn(col, type, item.GetText(), just, width);
        SetColumn(col, item);

        // set/remove options based on the wxListCtrl type.
        DataBrowserTableViewColumnID id;
        m_dbImpl->GetColumnIDFromIndex(col, &id);
        DataBrowserPropertyFlags flags;
        verify_noerr(m_dbImpl->GetPropertyFlags(id, &flags));
        if (GetWindowStyleFlag() & wxLC_EDIT_LABELS)
            flags |= kDataBrowserPropertyIsEditable;

        if (GetWindowStyleFlag() & wxLC_VIRTUAL){
            flags &= ~kDataBrowserListViewSortableColumn;
        }
        verify_noerr(m_dbImpl->SetPropertyFlags(id, flags));
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

    if (m_dbImpl)
    {
        m_dbImpl->SetScrollPosition(dx, dy);
    }
    return true;
}


bool wxListCtrl::SortItems(wxListCtrlCompare fn, long data)
{
    if (m_genericImpl)
        return m_genericImpl->SortItems(fn, data);

    if (m_dbImpl)
    {
        m_compareFunc = fn;
        m_compareFuncData = data;
    }

    return true;
}

void wxListCtrl::OnRenameTimer()
{
    wxCHECK_RET( HasCurrent(), wxT("unexpected rename timer") );

    EditLabel( m_current );
}

bool wxListCtrl::OnRenameAccept(long itemEdit, const wxString& value)
{
    wxListEvent le( wxEVT_COMMAND_LIST_END_LABEL_EDIT, GetId() );
    le.SetEventObject( this );
    le.m_itemIndex = itemEdit;

    GetItem( le.m_item );
    le.m_item.m_text = value;
    return !GetEventHandler()->ProcessEvent( le ) ||
                le.IsAllowed();
}

void wxListCtrl::OnRenameCancelled(long itemEdit)
{
    // let owner know that the edit was cancelled
    wxListEvent le( wxEVT_COMMAND_LIST_END_LABEL_EDIT, GetParent()->GetId() );

    le.SetEditCanceled(true);

    le.SetEventObject( this );
    le.m_itemIndex = itemEdit;

    GetItem( le.m_item );
    GetEventHandler()->ProcessEvent( le );
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

    if (m_genericImpl)
    {
        m_genericImpl->SetItemCount(count);
        return;
    }

    if (m_dbImpl)
    {
        // we need to temporarily disable the new item creation notification
        // procedure to speed things up
        // FIXME: Even this doesn't seem to help much...
        DataBrowserCallbacks callbacks;
        DataBrowserItemNotificationUPP itemUPP;
        GetDataBrowserCallbacks(m_dbImpl->GetControlRef(), &callbacks);
        itemUPP = callbacks.u.v1.itemNotificationCallback;
        callbacks.u.v1.itemNotificationCallback = 0;
        m_dbImpl->SetCallbacks(&callbacks);
        ::AddDataBrowserItems(m_dbImpl->GetControlRef(), kDataBrowserNoItem,
            count, NULL, kDataBrowserItemNoProperty);
        callbacks.u.v1.itemNotificationCallback = itemUPP;
        m_dbImpl->SetCallbacks(&callbacks);
    }
    m_count = count;
}

void wxListCtrl::RefreshItem(long item)
{
    if (m_genericImpl)
    {
        m_genericImpl->RefreshItem(item);
        return;
    }

    wxRect rect;
    GetItemRect(item, rect);
    RefreshRect(rect);
}

void wxListCtrl::RefreshItems(long itemFrom, long itemTo)
{
    if (m_genericImpl)
    {
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
    if ( list && lb )
    {
        bool trigger = false;

        wxListEvent event( wxEVT_COMMAND_LIST_ITEM_SELECTED, list->GetId() );
        bool isSingle = (list->GetWindowStyle() & wxLC_SINGLE_SEL) != 0;

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
                event.SetEventType( wxEVT_COMMAND_LIST_ITEM_ACTIVATED );
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
    m_isVirtual = false;

    if ( style & wxLC_VIRTUAL )
        m_isVirtual = true;

    DataBrowserSelectionFlags  options = kDataBrowserDragSelect;
    if ( style & wxLC_SINGLE_SEL )
    {
        options |= kDataBrowserSelectOnlyOne;
    }
    else
    {
        options |= kDataBrowserCmdTogglesSelection;
    }

    err = SetSelectionFlags( options );
    verify_noerr( err );

    DataBrowserCustomCallbacks callbacks;
    InitializeDataBrowserCustomCallbacks( &callbacks, kDataBrowserLatestCustomCallbacks );

    if ( gDataBrowserDrawItemUPP == NULL )
        gDataBrowserDrawItemUPP = NewDataBrowserDrawItemUPP(DataBrowserDrawItemProc);

    if ( gDataBrowserHitTestUPP == NULL )
        gDataBrowserHitTestUPP = NewDataBrowserHitTestUPP(DataBrowserHitTestProc);

    callbacks.u.v1.drawItemCallback = gDataBrowserDrawItemUPP;
    callbacks.u.v1.hitTestCallback = gDataBrowserHitTestUPP;

    SetDataBrowserCustomCallbacks( GetControlRef(), &callbacks );

    if ( style & wxLC_LIST )
    {
        InsertColumn(0, kDataBrowserIconAndTextType, wxEmptyString, -1, -1);
        verify_noerr( AutoSizeColumns() );
    }

    if ( style & wxLC_LIST || style & wxLC_NO_HEADER )
        verify_noerr( SetHeaderButtonHeight( 0 ) );

    if ( m_isVirtual )
        SetSortProperty( kMinColumnId - 1 );
    else
        SetSortProperty( kMinColumnId );
    if ( style & wxLC_SORT_ASCENDING )
    {
        m_sortOrder = SortOrder_Text_Ascending;
        SetSortOrder( kDataBrowserOrderIncreasing );
    }
    else if ( style & wxLC_SORT_DESCENDING )
    {
        m_sortOrder = SortOrder_Text_Descending;
        SetSortOrder( kDataBrowserOrderDecreasing );
    }
    else
    {
        m_sortOrder = SortOrder_None;
    }

    if ( style & wxLC_VRULES )
    {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
        verify_noerr( DataBrowserChangeAttributes(m_controlRef, kDataBrowserAttributeListViewDrawColumnDividers, kDataBrowserAttributeNone) );
#endif
    }

    verify_noerr( SetHiliteStyle(kDataBrowserTableViewFillHilite ) );
    err = SetHasScrollBars( (style & wxHSCROLL) != 0 , true );
}

pascal Boolean wxMacDataBrowserListCtrlControl::DataBrowserEditTextProc(
        ControlRef browser,
        DataBrowserItemID itemID,
        DataBrowserPropertyID property,
        CFStringRef theString,
        Rect *maxEditTextRect,
        Boolean *shrinkToFit)
{
    Boolean result = false;
    wxMacDataBrowserListCtrlControl* ctl = dynamic_cast<wxMacDataBrowserListCtrlControl*>( wxMacControl::GetReferenceFromNativeControl( browser ) );
    if ( ctl != 0 )
    {
        result = ctl->ConfirmEditText(itemID, property, theString, maxEditTextRect, shrinkToFit);
        theString = CFSTR("Hello!");
    }
    return result;
}

bool wxMacDataBrowserListCtrlControl::ConfirmEditText(
        DataBrowserItemID itemID,
        DataBrowserPropertyID property,
        CFStringRef theString,
        Rect *maxEditTextRect,
        Boolean *shrinkToFit)
{
    return false;
}

pascal void wxMacDataBrowserListCtrlControl::DataBrowserDrawItemProc(
        ControlRef browser,
        DataBrowserItemID itemID,
        DataBrowserPropertyID property,
        DataBrowserItemState itemState,
        const Rect *itemRect,
        SInt16 gdDepth,
        Boolean colorDevice)
{
    wxMacDataBrowserListCtrlControl* ctl = dynamic_cast<wxMacDataBrowserListCtrlControl*>( wxMacControl::GetReferenceFromNativeControl( browser ) );
    if ( ctl != 0 )
    {
        ctl->DrawItem(itemID, property, itemState, itemRect, gdDepth, colorDevice);
    }
}

// routines needed for DrawItem
enum
{
  kIconWidth = 16,
  kIconHeight = 16,
  kTextBoxHeight = 14,
  kIconTextSpacingV = 2,
  kItemPadding = 4,
  kContentHeight = kIconHeight + kTextBoxHeight + kIconTextSpacingV
};

static void calculateCGDrawingBounds(CGRect inItemRect, CGRect *outIconRect, CGRect *outTextRect, bool hasIcon = false)
{
  float textBottom;
  float iconH, iconW = 0;
  float padding = kItemPadding;
  if (hasIcon)
  {
    iconH = kIconHeight;
    iconW = kIconWidth;
    padding = padding*2;
  }

  textBottom = inItemRect.origin.y;

  *outIconRect = CGRectMake(inItemRect.origin.x + kItemPadding,
                textBottom + kIconTextSpacingV, kIconWidth,
                kIconHeight);

  *outTextRect = CGRectMake(inItemRect.origin.x + padding + iconW,
                textBottom + kIconTextSpacingV, inItemRect.size.width - padding - iconW,
                inItemRect.size.height - kIconTextSpacingV);
}

void wxMacDataBrowserListCtrlControl::DrawItem(
        DataBrowserItemID itemID,
        DataBrowserPropertyID property,
        DataBrowserItemState itemState,
        const Rect *itemRect,
        SInt16 gdDepth,
        Boolean colorDevice)
{
    wxString text;
    wxFont font = wxNullFont;
    int imgIndex = -1;
    short listColumn = property - kMinColumnId;

    wxListCtrl* list = wxDynamicCast( GetPeer() , wxListCtrl );
    wxMacListCtrlItem* lcItem;
    wxColour color = *wxBLACK;
    wxColour bgColor = wxNullColour;

    if (listColumn >= 0)
    {
        if (!m_isVirtual)
        {
            lcItem = (wxMacListCtrlItem*) itemID;
            if (lcItem->HasColumnInfo(listColumn)){
                wxListItem* item = lcItem->GetColumnInfo(listColumn);

                // we always use the 0 column to get font and text/background colors.
                if (lcItem->HasColumnInfo(0))
                {
                    wxListItem* firstItem = lcItem->GetColumnInfo(0);
                    color = firstItem->GetTextColour();
                    bgColor = firstItem->GetBackgroundColour();
                    font = firstItem->GetFont();
                }

                if (item->GetMask() & wxLIST_MASK_TEXT)
                    text = item->GetText();
                if (item->GetMask() & wxLIST_MASK_IMAGE)
                    imgIndex = item->GetImage();
            }

        }
        else
        {
            text = list->OnGetItemText( (long)itemID-1, listColumn );
            imgIndex = list->OnGetItemColumnImage( (long)itemID-1, listColumn );
            wxListItemAttr* attrs = list->OnGetItemAttr( (long)itemID-1 );
            if (attrs)
            {
                if (attrs->HasBackgroundColour())
                    bgColor = attrs->GetBackgroundColour();
                if (attrs->HasTextColour())
                    color = attrs->GetTextColour();
                if (attrs->HasFont())
                    font = attrs->GetFont();
            }
        }
    }

    wxColour listBgColor = list->GetBackgroundColour();
    if (bgColor == wxNullColour)
        bgColor = listBgColor;

    wxFont listFont = list->GetFont();
    if (font == wxNullFont)
        font = listFont;

    wxMacCFStringHolder cfString;
    cfString.Assign( text, wxLocale::GetSystemEncoding() );

    Rect enclosingRect;
    CGRect enclosingCGRect, iconCGRect, textCGRect;
    Boolean active;
    ThemeDrawingState savedState = NULL;
    CGContextRef context = (CGContextRef)list->MacGetDrawingContext();
    RGBColor labelColor;

    GetDataBrowserItemPartBounds(GetControlRef(), itemID, property, kDataBrowserPropertyEnclosingPart,
              &enclosingRect);

    enclosingCGRect = CGRectMake(enclosingRect.left,
                      enclosingRect.top,
                      enclosingRect.right - enclosingRect.left,
                      enclosingRect.bottom - enclosingRect.top);

    active = IsControlActive(GetControlRef());

    // don't paint the background over the vertical rule line
    if ( list->GetWindowStyleFlag() & wxLC_VRULES )
    {
        enclosingCGRect.origin.x += 1;
        enclosingCGRect.size.width -= 1;
    }
    if (itemState == kDataBrowserItemIsSelected)
    {
        RGBColor foregroundColor;

        GetThemeDrawingState(&savedState);

        GetThemeBrushAsColor(kThemeBrushAlternatePrimaryHighlightColor, 32, true, &foregroundColor);
        GetThemeTextColor(kThemeTextColorWhite, gdDepth, colorDevice, &labelColor);

        CGContextSaveGState(context);

        CGContextSetRGBFillColor(context, (float)foregroundColor.red / (float)USHRT_MAX,
                      (float)foregroundColor.green / (float)USHRT_MAX,
                      (float)foregroundColor.blue / (float)USHRT_MAX, 1.0);
        CGContextFillRect(context, enclosingCGRect);

        CGContextRestoreGState(context);
    }
    else
    {

        if (color.Ok())
            labelColor = MAC_WXCOLORREF( color.GetPixel() );
        else if (list->GetTextColour().Ok())
            labelColor = MAC_WXCOLORREF( list->GetTextColour().GetPixel() );
        else
        {
            labelColor.red = 0;
            labelColor.green = 0;
            labelColor.blue = 0;
        }

        if (bgColor.Ok())
        {
            RGBColor foregroundColor = MAC_WXCOLORREF( bgColor.GetPixel() );
            CGContextSaveGState(context);

            CGContextSetRGBFillColor(context, (float)foregroundColor.red / (float)USHRT_MAX,
                          (float)foregroundColor.green / (float)USHRT_MAX,
                          (float)foregroundColor.blue / (float)USHRT_MAX, 1.0);
            CGContextFillRect(context, enclosingCGRect);

            CGContextRestoreGState(context);
        }
    }

    calculateCGDrawingBounds(enclosingCGRect, &iconCGRect, &textCGRect, (imgIndex != -1) );

    if (imgIndex != -1)
    {
        wxImageList* imageList = list->GetImageList(wxIMAGE_LIST_SMALL);
        if (imageList && imageList->GetImageCount() > 0){
            wxBitmap bmp = imageList->GetBitmap(imgIndex);
            IconRef icon = bmp.GetBitmapData()->GetIconRef();

            RGBColor iconLabel;
            iconLabel.red = 0;
            iconLabel.green = 0;
            iconLabel.blue = 0;

            CGContextSaveGState(context);
            CGContextTranslateCTM(context, 0,iconCGRect.origin.y + CGRectGetMaxY(iconCGRect));
            CGContextScaleCTM(context,1.0f,-1.0f);
            PlotIconRefInContext(context, &iconCGRect, kAlignNone,
              active ? kTransformNone : kTransformDisabled, &iconLabel,
              kPlotIconRefNormalFlags, icon);

            CGContextRestoreGState(context);
        }
    }

    HIThemeTextHorizontalFlush hFlush = kHIThemeTextHorizontalFlushLeft;
    UInt16 fontID = kThemeViewsFont;

    if (font.Ok())
    {
        if (font.GetFamily() != wxFONTFAMILY_DEFAULT)
            fontID = font.MacGetThemeFontID();

// FIXME: replace these with CG or ATSUI calls so we can remove this #ifndef.
#ifndef __LP64__
        ::TextSize( (short)(font.MacGetFontSize()) ) ;
        ::TextFace( font.MacGetFontStyle() ) ;
#endif
    }

    wxListItem item;
    list->GetColumn(listColumn, item);
    if (item.GetMask() & wxLIST_MASK_FORMAT)
    {
        if (item.GetAlign() == wxLIST_FORMAT_LEFT)
            hFlush = kHIThemeTextHorizontalFlushLeft;
        else if (item.GetAlign() == wxLIST_FORMAT_CENTER)
            hFlush = kHIThemeTextHorizontalFlushCenter;
        else if (item.GetAlign() == wxLIST_FORMAT_RIGHT)
        {
            hFlush = kHIThemeTextHorizontalFlushRight;
            textCGRect.origin.x -= kItemPadding; // give a little extra paddding
        }
    }

    HIThemeTextInfo info;
    info.version = kHIThemeTextInfoVersionZero;
    info.state = active ? kThemeStateActive : kThemeStateInactive;
    info.fontID = fontID;
    info.horizontalFlushness = hFlush;
    info.verticalFlushness = kHIThemeTextVerticalFlushCenter;
    info.options = kHIThemeTextBoxOptionNone;
    info.truncationPosition = kHIThemeTextTruncationEnd;
    info.truncationMaxLines = 1;

    CGContextSaveGState(context);
    CGContextSetRGBFillColor (context, (float)labelColor.red / (float)USHRT_MAX,
                      (float)labelColor.green / (float)USHRT_MAX,
                      (float)labelColor.blue / (float)USHRT_MAX, 1.0);

    HIThemeDrawTextBox(cfString, &textCGRect, &info, context, kHIThemeOrientationNormal);

    CGContextRestoreGState(context);

    if (savedState != NULL)
        SetThemeDrawingState(savedState, true);
}

OSStatus wxMacDataBrowserListCtrlControl::GetSetItemData(DataBrowserItemID itemID,
                        DataBrowserPropertyID property,
                        DataBrowserItemDataRef itemData,
                        Boolean changeValue )
{
    wxString text;
    int imgIndex = -1;
    short listColumn = property - kMinColumnId;

    OSStatus err = errDataBrowserPropertyNotSupported;
    wxListCtrl* list = wxDynamicCast( GetPeer() , wxListCtrl );
    wxMacListCtrlItem* lcItem;

    if (listColumn >= 0)
    {
        if (!m_isVirtual)
        {
            lcItem = (wxMacListCtrlItem*) itemID;
            if (lcItem && lcItem->HasColumnInfo(listColumn)){
                wxListItem* item = lcItem->GetColumnInfo(listColumn);
                if (item->GetMask() & wxLIST_MASK_TEXT)
                    text = item->GetText();
                if (item->GetMask() & wxLIST_MASK_IMAGE)
                    imgIndex = item->GetImage();
            }
        }
        else
        {
            text = list->OnGetItemText( (long)itemID-1, listColumn );
            imgIndex = list->OnGetItemColumnImage( (long)itemID-1, listColumn );
        }
    }

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
                if ( property >= kMinColumnId )
                {
                    wxMacCFStringHolder cfStr;

                    if (text){
                        cfStr.Assign( text, wxLocale::GetSystemEncoding() );
                        err = ::SetDataBrowserItemDataText( itemData, cfStr );
                        err = noErr;
                    }



                    if ( imgIndex != -1 )
                    {
                        wxImageList* imageList = list->GetImageList(wxIMAGE_LIST_SMALL);
                        if (imageList && imageList->GetImageCount() > 0){
                            wxBitmap bmp = imageList->GetBitmap(imgIndex);
                            IconRef icon = bmp.GetBitmapData()->GetIconRef();
                            ::SetDataBrowserItemDataIcon(itemData, icon);
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
                if ( property >= kMinColumnId )
                {
                    short listColumn = property - kMinColumnId;

                    // TODO probably send the 'end edit' from here, as we
                    // can then deal with the veto
                    CFStringRef sr ;
                    verify_noerr( GetDataBrowserItemDataText( itemData , &sr ) ) ;
                    wxMacCFStringHolder cfStr(sr) ;;
                    if (m_isVirtual)
                        list->SetItem( (long)itemData-1 , listColumn, cfStr.AsString() ) ;
                    else
                    {
                        if (lcItem)
                            lcItem->SetColumnTextValue( listColumn, cfStr.AsString() );
                    }
                    err = noErr ;
                }
                break;
        }
    }
    return err;
}

void wxMacDataBrowserListCtrlControl::ItemNotification(DataBrowserItemID itemID,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData )
{
    // we want to depend on as little as possible to make sure tear-down of controls is safe
    if ( message == kDataBrowserItemRemoved)
    {
        // make sure MacDelete does the proper teardown.
        return;
    }
    else if ( message == kDataBrowserItemAdded )
    {
        // we don't issue events on adding, the item is not really stored in the list yet, so we
        // avoid asserts by getting out now
        return  ;
    }

    wxListCtrl *list = wxDynamicCast( GetPeer() , wxListCtrl );
    if ( list )
    {
        bool trigger = false;

        wxListEvent event( wxEVT_COMMAND_LIST_ITEM_SELECTED, list->GetId() );
        bool isSingle = (list->GetWindowStyle() & wxLC_SINGLE_SEL) != 0;

        event.SetEventObject( list );
        if ( !list->IsVirtual() )
        {
            DataBrowserTableViewRowIndex result = 0;
            verify_noerr( GetItemRow( itemID, &result ) ) ;
            event.m_itemIndex = result;

            if (event.m_itemIndex >= 0)
                MacGetColumnInfo(event.m_itemIndex,0,event.m_item);
        }
        else
        {
            event.m_itemIndex = (long)itemID;
        }

        switch (message)
        {
            case kDataBrowserItemDeselected:
                event.SetEventType(wxEVT_COMMAND_LIST_ITEM_DESELECTED);
                if ( !isSingle )
                    trigger = !IsSelectionSuppressed();
                break;

            case kDataBrowserItemSelected:
                trigger = !IsSelectionSuppressed();

                break;

            case kDataBrowserItemDoubleClicked:
                event.SetEventType( wxEVT_COMMAND_LIST_ITEM_ACTIVATED );
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

Boolean wxMacDataBrowserListCtrlControl::CompareItems(DataBrowserItemID itemOneID,
                        DataBrowserItemID itemTwoID,
                        DataBrowserPropertyID sortProperty)
{

    bool retval = false;
    wxString itemText;
    wxString otherItemText;
    int colId = sortProperty - kMinColumnId;

    wxListCtrl* list = wxDynamicCast( GetPeer() , wxListCtrl );

    if (colId >= 0)
    {
        if (!m_isVirtual)
        {
            wxMacListCtrlItem* item = (wxMacListCtrlItem*)itemOneID;
            wxMacListCtrlItem* otherItem = (wxMacListCtrlItem*)itemTwoID;
            wxListCtrlCompare func = list->GetCompareFunc();
            long item1 = GetLineFromItem(item);
            long item2 = GetLineFromItem(otherItem);

            if (func != NULL && item->HasColumnInfo(colId) && otherItem->HasColumnInfo(colId))
                return func(item1, item2, list->GetCompareFuncData()) >= 0;

            if (item->HasColumnInfo(colId))
                itemText = item->GetColumnInfo(colId)->GetText();
            if (otherItem->HasColumnInfo(colId))
                otherItemText = otherItem->GetColumnInfo(colId)->GetText();
        }
        else
        {

            long itemNum = (long)itemOneID;
            long otherItemNum = (long)itemTwoID;
            itemText = list->OnGetItemText( itemNum-1, colId );
            otherItemText = list->OnGetItemText( otherItemNum-1, colId );

        }

        DataBrowserSortOrder sort;
        verify_noerr(GetSortOrder(&sort));

        if ( sort == kDataBrowserOrderIncreasing )
        {
            retval = itemText.CmpNoCase( otherItemText ) > 0;
        }
        else if ( sort == kDataBrowserOrderDecreasing )
        {
            retval = itemText.CmpNoCase( otherItemText ) < 0;
        }
    }
    else{
        // fallback for undefined cases
        retval = itemOneID < itemTwoID;
    }

    return retval;
}

wxMacDataBrowserListCtrlControl::~wxMacDataBrowserListCtrlControl()
{
}

void wxMacDataBrowserListCtrlControl::MacSetColumnInfo( unsigned int row, unsigned int column, wxListItem* item )
{
    wxMacDataItem* dataItem = GetItemFromLine(row);
    wxASSERT_MSG( dataItem, _T("could not obtain wxMacDataItem for row in MacSetColumnInfo. Is row a valid wxListCtrl row?") );
    if (item)
    {
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
    if ( isSelected != isSelectedState )
    {
        DataBrowserSetOption options = kDataBrowserItemsAdd;
        if (!isSelectedState)
            options = kDataBrowserItemsRemove;
        SetSelectedItem(dataItem, options);
    }
    // TODO: Set column width if item width > than current column width
}

void wxMacDataBrowserListCtrlControl::MacGetColumnInfo( unsigned int row, unsigned int column, wxListItem& item )
{
    wxMacDataItem* dataItem = GetItemFromLine(row);
    wxASSERT_MSG( dataItem, _T("could not obtain wxMacDataItem in MacGetColumnInfo. Is row a valid wxListCtrl row?") );
    // CS should this guard against dataItem = 0 ? , as item is not a pointer if (item) is not appropriate
    //if (item)
    {
        wxMacListCtrlItem* listItem = dynamic_cast<wxMacListCtrlItem*>(dataItem);

        if (!listItem->HasColumnInfo( column ))
            return;

        wxListItem* oldItem = listItem->GetColumnInfo( column );

        if (oldItem)
        {
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
}

void wxMacDataBrowserListCtrlControl::MacInsertItem( unsigned int n, wxListItem* item )
{
    wxMacDataItemBrowserControl::MacInsert(n, item->GetText());
    MacSetColumnInfo(n, 0, item);
}

wxMacDataItem* wxMacDataBrowserListCtrlControl::CreateItem()
{
    return new wxMacListCtrlItem();
}

wxMacListCtrlItem::wxMacListCtrlItem()
{
    m_rowItems = wxListItemList( wxKEY_INTEGER );
}

int wxMacListCtrlItem::GetColumnImageValue( unsigned int column )
{
    if ( HasColumnInfo(column) )
        return GetColumnInfo(column)->GetImage();

    return -1;
}

void wxMacListCtrlItem::SetColumnImageValue( unsigned int column, int imageIndex )
{
    if ( HasColumnInfo(column) )
        GetColumnInfo(column)->SetImage(imageIndex);
}

wxString wxMacListCtrlItem::GetColumnTextValue( unsigned int column )
{
    if ( column == 0 )
        return GetLabel();

    if ( HasColumnInfo(column) )
        return GetColumnInfo(column)->GetText();

    return wxEmptyString;
}

void wxMacListCtrlItem::SetColumnTextValue( unsigned int column, const wxString& text )
{
    if ( HasColumnInfo(column) )
        GetColumnInfo(column)->SetText(text);

    // for compatibility with superclass APIs
    if ( column == 0 )
        SetLabel(text);
}

wxListItem* wxMacListCtrlItem::GetColumnInfo( unsigned int column )
{
    wxListItemList::compatibility_iterator node = m_rowItems.Find( column );
    wxASSERT_MSG( node, _T("invalid column index in wxMacListCtrlItem") );

    return node->GetData();
}

bool wxMacListCtrlItem::HasColumnInfo( unsigned int column )
{
    return m_rowItems.Find( column ) != NULL;
}

void wxMacListCtrlItem::SetColumnInfo( unsigned int column, wxListItem* item )
{

    if ( !HasColumnInfo(column) )
    {
        wxListItem* listItem = new wxListItem(*item);
        m_rowItems.Append( column, listItem );
    }
    else
    {
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

        if ( item->HasAttributes() )
        {
            if ( listItem->HasAttributes() )
                listItem->GetAttributes()->AssignFrom(*item->GetAttributes());
            else
            {
                listItem->SetTextColour(item->GetTextColour());
                listItem->SetBackgroundColour(item->GetBackgroundColour());
                listItem->SetFont(item->GetFont());
            }
        }
    }
}

#endif // wxUSE_LISTCTRL
