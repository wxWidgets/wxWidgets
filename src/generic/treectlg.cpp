/////////////////////////////////////////////////////////////////////////////
// Name:        treectlg.cpp
// Purpose:     generic tree control implementation
// Author:      Robert Roebling
// Created:     01/02/97
// Modified:    22/10/98 - almost total rewrite, simpler interface (VZ)
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "treectlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/generic/treectlg.h"
#include "wx/imaglist.h"
#include "wx/settings.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/arrimpl.cpp"
#include "wx/dcclient.h"
#include "wx/msgdlg.h"

// -----------------------------------------------------------------------------
// array types
// -----------------------------------------------------------------------------

class WXDLLEXPORT wxGenericTreeItem;

WX_DEFINE_EXPORTED_ARRAY(wxGenericTreeItem *, wxArrayGenericTreeItems);
//WX_DEFINE_OBJARRAY(wxArrayTreeItemIds);

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const int NO_IMAGE = -1;

#define PIXELS_PER_UNIT 10

// -----------------------------------------------------------------------------
// private classes
// -----------------------------------------------------------------------------

// timer used for enabling in-place edit
class WXDLLEXPORT wxTreeRenameTimer: public wxTimer
{
public:
    wxTreeRenameTimer( wxGenericTreeCtrl *owner );

    void Notify();

private:
    wxGenericTreeCtrl   *m_owner;
};

// control used for in-place edit
class WXDLLEXPORT wxTreeTextCtrl: public wxTextCtrl
{
public:
    wxTreeTextCtrl() { }
    wxTreeTextCtrl( wxWindow *parent,
                    const wxWindowID id,
                    bool *accept,
                    wxString *res,
                    wxGenericTreeCtrl *owner,
                    const wxString &value = wxEmptyString,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize,
                    int style = wxSIMPLE_BORDER,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString &name = wxTextCtrlNameStr );

    void OnChar( wxKeyEvent &event );
    void OnKeyUp( wxKeyEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    bool               *m_accept;
    wxString           *m_res;
    wxGenericTreeCtrl         *m_owner;
    wxString            m_startValue;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTreeTextCtrl);
};

// a tree item
class WXDLLEXPORT wxGenericTreeItem
{
public:
    // ctors & dtor
    wxGenericTreeItem() { m_data = NULL; }
    wxGenericTreeItem( wxGenericTreeItem *parent,
            const wxString& text,
            wxDC& dc,
            int image, int selImage,
            wxTreeItemData *data );

    ~wxGenericTreeItem();

    // trivial accessors
    wxArrayGenericTreeItems& GetChildren() { return m_children; }

    const wxString& GetText() const { return m_text; }
    int GetImage(wxTreeItemIcon which = wxTreeItemIcon_Normal) const
        { return m_images[which]; }
    wxTreeItemData *GetData() const { return m_data; }

    // returns the current image for the item (depending on its
    // selected/expanded/whatever state)
    int GetCurrentImage() const;

    void SetText( const wxString &text );
    void SetImage(int image, wxTreeItemIcon which) { m_images[which] = image; }
    void SetData(wxTreeItemData *data) { m_data = data; }

    void SetHasPlus(bool has = TRUE) { m_hasPlus = has; }

    void SetBold(bool bold) { m_isBold = bold; }

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }

    void SetX(int x) { m_x = x; }
    void SetY(int y) { m_y = y; }

    int  GetHeight() const { return m_height; }
    int  GetWidth()  const { return m_width; }

    void SetHeight(int h) { m_height = h; }
    void SetWidth(int w) { m_width = w; }


    wxGenericTreeItem *GetParent() const { return m_parent; }

    // operations
        // deletes all children notifying the treectrl about it if !NULL
        // pointer given
    void DeleteChildren(wxGenericTreeCtrl *tree = NULL);
        // FIXME don't know what is it for
    void Reset();

    // get count of all children (and grand children if 'recursively')
    size_t GetChildrenCount(bool recursively = TRUE) const;

    void Insert(wxGenericTreeItem *child, size_t index)
    { m_children.Insert(child, index); }

    void SetCross( int x, int y );
    void GetSize( int &x, int &y, const wxGenericTreeCtrl* );

        // return the item at given position (or NULL if no item), onButton is
        // TRUE if the point belongs to the item's button, otherwise it lies
        // on the button's label
    wxGenericTreeItem *HitTest( const wxPoint& point, const wxGenericTreeCtrl *, int &flags);

    void Expand() { m_isCollapsed = FALSE; }
    void Collapse() { m_isCollapsed = TRUE; }

    void SetHilight( bool set = TRUE ) { m_hasHilight = set; }

    // status inquiries
    bool HasChildren() const { return !m_children.IsEmpty(); }
    bool IsSelected()  const { return m_hasHilight != 0; }
    bool IsExpanded()  const { return !m_isCollapsed; }
    bool HasPlus()     const { return m_hasPlus || HasChildren(); }
    bool IsBold()      const { return m_isBold != 0; }

    // attributes
        // get them - may be NULL
    wxTreeItemAttr *GetAttributes() const { return m_attr; }
        // get them ensuring that the pointer is not NULL
    wxTreeItemAttr& Attr()
    {
        if ( !m_attr )
            m_attr = new wxTreeItemAttr;

        return *m_attr;
    }

private:
    wxString            m_text;

    // tree ctrl images for the normal, selected, expanded and
    // expanded+selected states
    int                 m_images[wxTreeItemIcon_Max];

    wxTreeItemData     *m_data;

    // use bitfields to save size
    int                 m_isCollapsed :1;
    int                 m_hasHilight  :1; // same as focused
    int                 m_hasPlus     :1; // used for item which doesn't have
                                          // children but has a [+] button
    int                 m_isBold      :1; // render the label in bold font

    wxCoord             m_x, m_y;
    wxCoord             m_height, m_width;
    int                 m_xCross, m_yCross;
    int                 m_level;

    wxArrayGenericTreeItems m_children;
    wxGenericTreeItem  *m_parent;

    wxTreeItemAttr     *m_attr;
};

// =============================================================================
// implementation
// =============================================================================

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// translate the key or mouse event flags to the type of selection we're
// dealing with
static void EventFlagsToSelType(long style,
                                bool shiftDown,
                                bool ctrlDown,
                                bool &is_multiple,
                                bool &extended_select,
                                bool &unselect_others)
{
    is_multiple = (style & wxTR_MULTIPLE) != 0;
    extended_select = shiftDown && is_multiple;
    unselect_others = !(extended_select || (ctrlDown && is_multiple));
}

// -----------------------------------------------------------------------------
// wxTreeRenameTimer (internal)
// -----------------------------------------------------------------------------

wxTreeRenameTimer::wxTreeRenameTimer( wxGenericTreeCtrl *owner )
{
    m_owner = owner;
}

void wxTreeRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxTreeTextCtrl (internal)
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeTextCtrl,wxTextCtrl);

BEGIN_EVENT_TABLE(wxTreeTextCtrl,wxTextCtrl)
    EVT_CHAR           (wxTreeTextCtrl::OnChar)
    EVT_KEY_UP         (wxTreeTextCtrl::OnKeyUp)
    EVT_KILL_FOCUS     (wxTreeTextCtrl::OnKillFocus)
END_EVENT_TABLE()

wxTreeTextCtrl::wxTreeTextCtrl( wxWindow *parent,
                                const wxWindowID id,
                                bool *accept,
                                wxString *res,
                                wxGenericTreeCtrl *owner,
                                const wxString &value,
                                const wxPoint &pos,
                                const wxSize &size,
                                int style,
                                const wxValidator& validator,
                                const wxString &name )
              : wxTextCtrl( parent, id, value, pos, size, style, validator, name )
{
    m_res = res;
    m_accept = accept;
    m_owner = owner;
    (*m_accept) = FALSE;
    (*m_res) = wxEmptyString;
    m_startValue = value;
}

void wxTreeTextCtrl::OnChar( wxKeyEvent &event )
{
    if (event.m_keyCode == WXK_RETURN)
    {
        (*m_accept) = TRUE;
        (*m_res) = GetValue();
        
        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);

        if ((*m_accept) && ((*m_res) != m_startValue))
            m_owner->OnRenameAccept();
            
        return;
    }
    if (event.m_keyCode == WXK_ESCAPE)
    {
        (*m_accept) = FALSE;
        (*m_res) = "";
        
        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);
            
        return;
    }
    event.Skip();
}

void wxTreeTextCtrl::OnKeyUp( wxKeyEvent &event )
{
    // auto-grow the textctrl:
    wxSize parentSize = m_owner->GetSize();
    wxPoint myPos = GetPosition();
    wxSize mySize = GetSize();
    int sx, sy;
    GetTextExtent(GetValue() + _T("MM"), &sx, &sy);
    if (myPos.x + sx > parentSize.x) sx = parentSize.x - myPos.x;
    if (mySize.x > sx) sx = mySize.x;
    SetSize(sx, -1);
    
    event.Skip();
}

void wxTreeTextCtrl::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    if (!wxPendingDelete.Member(this))
        wxPendingDelete.Append(this);

    if ((*m_accept) && ((*m_res) != m_startValue))
        m_owner->OnRenameAccept();
}

#if 0
// -----------------------------------------------------------------------------
// wxTreeEvent
// -----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeEvent, wxNotifyEvent)

wxTreeEvent::wxTreeEvent( wxEventType commandType, int id )
           : wxNotifyEvent( commandType, id )
{
    m_code = 0;
    m_itemOld = (wxGenericTreeItem *)NULL;
}
#endif

// -----------------------------------------------------------------------------
// wxGenericTreeItem
// -----------------------------------------------------------------------------

wxGenericTreeItem::wxGenericTreeItem(wxGenericTreeItem *parent,
                                     const wxString& text,
                                     wxDC& WXUNUSED(dc),
                                     int image, int selImage,
                                     wxTreeItemData *data)
                 : m_text(text)
{
    m_images[wxTreeItemIcon_Normal] = image;
    m_images[wxTreeItemIcon_Selected] = selImage;
    m_images[wxTreeItemIcon_Expanded] = NO_IMAGE;
    m_images[wxTreeItemIcon_SelectedExpanded] = NO_IMAGE;

    m_data = data;
    m_x = m_y = 0;
    m_xCross = m_yCross = 0;

    m_level = 0;

    m_isCollapsed = TRUE;
    m_hasHilight = FALSE;
    m_hasPlus = FALSE;
    m_isBold = FALSE;

    m_parent = parent;

    m_attr = (wxTreeItemAttr *)NULL;

    // We don't know the height here yet.
    m_width = 0;
    m_height = 0;
}

wxGenericTreeItem::~wxGenericTreeItem()
{
    delete m_data;

    delete m_attr;

    wxASSERT_MSG( m_children.IsEmpty(),
                  wxT("please call DeleteChildren() before deleting the item") );
}

void wxGenericTreeItem::DeleteChildren(wxGenericTreeCtrl *tree)
{
    size_t count = m_children.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        wxGenericTreeItem *child = m_children[n];
        if (tree)
            tree->SendDeleteEvent(child);

        child->DeleteChildren(tree);
        delete child;
    }

    m_children.Empty();
}

void wxGenericTreeItem::SetText( const wxString &text )
{
    m_text = text;
}

void wxGenericTreeItem::Reset()
{
    m_text.Empty();
    for ( int i = 0; i < wxTreeItemIcon_Max; i++ )
    {
        m_images[i] = NO_IMAGE;
    }

    m_data = NULL;
    m_x = m_y =
    m_height = m_width = 0;
    m_xCross =
    m_yCross = 0;

    m_level = 0;

    DeleteChildren();
    m_isCollapsed = TRUE;

    m_parent = (wxGenericTreeItem *)NULL;
}

size_t wxGenericTreeItem::GetChildrenCount(bool recursively) const
{
    size_t count = m_children.Count();
    if ( !recursively )
        return count;

    size_t total = count;
    for (size_t n = 0; n < count; ++n)
    {
        total += m_children[n]->GetChildrenCount();
    }

    return total;
}

void wxGenericTreeItem::SetCross( int x, int y )
{
    m_xCross = x;
    m_yCross = y;
}

void wxGenericTreeItem::GetSize( int &x, int &y, const wxGenericTreeCtrl *theTree )
{
    int bottomY=m_y+theTree->GetLineHeight(this);
    if ( y < bottomY ) y = bottomY;
    int width = m_x +  m_width;
    if ( x < width ) x = width;

    if (IsExpanded())
    {
        size_t count = m_children.Count();
        for ( size_t n = 0; n < count; ++n )
        {
            m_children[n]->GetSize( x, y, theTree );
        }
    }
}

wxGenericTreeItem *wxGenericTreeItem::HitTest( const wxPoint& point,
                                               const wxGenericTreeCtrl *theTree,
                                               int &flags)
{
    if ((point.y > m_y) && (point.y < m_y + theTree->GetLineHeight(this)))
    {
        if (point.y < m_y+theTree->GetLineHeight(this)/2 )
            flags |= wxTREE_HITTEST_ONITEMUPPERPART;
        else
            flags |= wxTREE_HITTEST_ONITEMLOWERPART;

        // 5 is the size of the plus sign
        if ((point.x > m_xCross-5) && (point.x < m_xCross+5) &&
            (point.y > m_yCross-5) && (point.y < m_yCross+5) &&
            (IsExpanded() || HasPlus()))
        {
            flags|=wxTREE_HITTEST_ONITEMBUTTON;
            return this;
        }

        if ((point.x >= m_x) && (point.x <= m_x+m_width))
        {
            int image_w = -1;
            int image_h;

            // assuming every image (normal and selected ) has the same size !
            if ( (GetImage() != NO_IMAGE) && theTree->m_imageListNormal )
                theTree->m_imageListNormal->GetSize(GetImage(), image_w, image_h);

            if ((image_w != -1) && (point.x <= m_x + image_w + 1))
                flags |= wxTREE_HITTEST_ONITEMICON;
            else
                flags |= wxTREE_HITTEST_ONITEMLABEL;

            return this;
        }

        if (point.x < m_x)
            flags |= wxTREE_HITTEST_ONITEMINDENT;
        if (point.x > m_x+m_width)
            flags |= wxTREE_HITTEST_ONITEMRIGHT;

        return this;
    }
    else
    {
        if (!m_isCollapsed)
        {
            size_t count = m_children.Count();
            for ( size_t n = 0; n < count; n++ )
            {
                wxGenericTreeItem *res = m_children[n]->HitTest( point, theTree, flags );
                if ( res != NULL )
                    return res;
            }
        }
    }

    flags|=wxTREE_HITTEST_NOWHERE;

    return (wxGenericTreeItem*) NULL;
}

int wxGenericTreeItem::GetCurrentImage() const
{
    int image = NO_IMAGE;
    if ( IsExpanded() )
    {
        if ( IsSelected() )
        {
            image = GetImage(wxTreeItemIcon_SelectedExpanded);
        }

        if ( image == NO_IMAGE )
        {
            // we usually fall back to the normal item, but try just the
            // expanded one (and not selected) first in this case
            image = GetImage(wxTreeItemIcon_Expanded);
        }
    }
    else // not expanded
    {
        if ( IsSelected() )
            image = GetImage(wxTreeItemIcon_Selected);
    }

    // may be it doesn't have the specific image we want, try the default one
    // instead
    if ( image == NO_IMAGE )
    {
        image = GetImage();
    }

    return image;
}

// -----------------------------------------------------------------------------
// wxGenericTreeCtrl implementation
// -----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericTreeCtrl, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxGenericTreeCtrl,wxScrolledWindow)
    EVT_PAINT          (wxGenericTreeCtrl::OnPaint)
    EVT_MOUSE_EVENTS   (wxGenericTreeCtrl::OnMouse)
    EVT_CHAR           (wxGenericTreeCtrl::OnChar)
    EVT_SET_FOCUS      (wxGenericTreeCtrl::OnSetFocus)
    EVT_KILL_FOCUS     (wxGenericTreeCtrl::OnKillFocus)
    EVT_IDLE           (wxGenericTreeCtrl::OnIdle)
END_EVENT_TABLE()

#if !defined(__WXMSW__) || defined(__WIN16__)
/*
 * wxTreeCtrl has to be a real class or we have problems with
 * the run-time information.
 */

IMPLEMENT_DYNAMIC_CLASS(wxTreeCtrl, wxGenericTreeCtrl)
#endif

// -----------------------------------------------------------------------------
// construction/destruction
// -----------------------------------------------------------------------------

void wxGenericTreeCtrl::Init()
{
    m_current =
    m_key_current =
    m_anchor = (wxGenericTreeItem *) NULL;
    m_hasFocus = FALSE;
    m_dirty = FALSE;

    m_xScroll = 0;
    m_yScroll = 0;
    m_lineHeight = 10;
    m_indent = 15;
    m_spacing = 18;

    m_hilightBrush = new wxBrush
    (
      wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT),
      wxSOLID
    );

    m_imageListNormal =
    m_imageListState = (wxImageList *) NULL;
    m_ownsImageListNormal = 
    m_ownsImageListState = FALSE;

    m_dragCount = 0;
    m_isDragging = FALSE;
    m_dropTarget =
    m_oldSelection = (wxGenericTreeItem *)NULL;

    m_renameTimer = new wxTreeRenameTimer( this );
    m_lastOnSame = FALSE;

    m_normalFont = wxSystemSettings::GetSystemFont( wxSYS_DEFAULT_GUI_FONT );
    m_boldFont = wxFont( m_normalFont.GetPointSize(),
                            m_normalFont.GetFamily(),
                            m_normalFont.GetStyle(),
                            wxBOLD,
                            m_normalFont.GetUnderlined());
}

bool wxGenericTreeCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style,
                        const wxValidator &validator,
                        const wxString& name )
{
    wxScrolledWindow::Create( parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, name );

#if wxUSE_VALIDATORS
    SetValidator( validator );
#endif

    SetBackgroundColour( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_LISTBOX ) );
//  m_dottedPen = wxPen( "grey", 0, wxDOT );  too slow under XFree86
    m_dottedPen = wxPen( "grey", 0, 0 );

    return TRUE;
}

wxGenericTreeCtrl::~wxGenericTreeCtrl()
{
    wxDELETE( m_hilightBrush );

    DeleteAllItems();

    delete m_renameTimer;
    if (m_ownsImageListNormal) delete m_imageListNormal;
    if (m_ownsImageListState) delete m_imageListState;
}

// -----------------------------------------------------------------------------
// accessors
// -----------------------------------------------------------------------------

size_t wxGenericTreeCtrl::GetCount() const
{
    return m_anchor == NULL ? 0u : m_anchor->GetChildrenCount();
}

void wxGenericTreeCtrl::SetIndent(unsigned int indent)
{
    m_indent = indent;
    m_dirty = TRUE;
}

void wxGenericTreeCtrl::SetSpacing(unsigned int spacing)
{
    m_spacing = spacing;
    m_dirty = TRUE;
}

size_t wxGenericTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively)
{
    wxCHECK_MSG( item.IsOk(), 0u, wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->GetChildrenCount(recursively);
}

// -----------------------------------------------------------------------------
// functions to work with tree items
// -----------------------------------------------------------------------------

wxString wxGenericTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), wxT(""), wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->GetText();
}

int wxGenericTreeCtrl::GetItemImage(const wxTreeItemId& item,
                             wxTreeItemIcon which) const
{
    wxCHECK_MSG( item.IsOk(), -1, wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->GetImage(which);
}

wxTreeItemData *wxGenericTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), NULL, wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->GetData();
}

void wxGenericTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxClientDC dc(this);
    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    pItem->SetText(text);
    CalculateSize(pItem, dc);
    RefreshLine(pItem);
}

void wxGenericTreeCtrl::SetItemImage(const wxTreeItemId& item,
                              int image,
                              wxTreeItemIcon which)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    pItem->SetImage(image, which);

    wxClientDC dc(this);
    CalculateSize(pItem, dc);
    RefreshLine(pItem);
}

void wxGenericTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    ((wxGenericTreeItem*) item.m_pItem)->SetData(data);
}

void wxGenericTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    pItem->SetHasPlus(has);
    RefreshLine(pItem);
}

void wxGenericTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    // avoid redrawing the tree if no real change
    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    if ( pItem->IsBold() != bold )
    {
        pItem->SetBold(bold);
        RefreshLine(pItem);
    }
}

void wxGenericTreeCtrl::SetItemTextColour(const wxTreeItemId& item,
                                   const wxColour& col)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    pItem->Attr().SetTextColour(col);
    RefreshLine(pItem);
}

void wxGenericTreeCtrl::SetItemBackgroundColour(const wxTreeItemId& item,
                                         const wxColour& col)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    pItem->Attr().SetBackgroundColour(col);
    RefreshLine(pItem);
}

void wxGenericTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxCHECK_RET( item.IsOk(), wxT("invalid tree item") );

    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
    pItem->Attr().SetFont(font);
    RefreshLine(pItem);
}

// -----------------------------------------------------------------------------
// item status inquiries
// -----------------------------------------------------------------------------

bool wxGenericTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

	// An item is only visible if it's not a descendant of a collapsed item
    wxGenericTreeItem *pItem = (wxGenericTreeItem*) item.m_pItem;
	wxGenericTreeItem* parent = pItem->GetParent();
	while (parent)
	{
		if (!parent->IsExpanded())
			return FALSE;
		parent = parent->GetParent();
	}

    int startX, startY;
    GetViewStart(& startX, & startY);

    wxSize clientSize = GetClientSize();

    wxRect rect;
    if (!GetBoundingRect(item, rect))
        return FALSE;
	if (rect.GetWidth() == 0 || rect.GetHeight() == 0)
		return FALSE;
    if (rect.GetBottom() < 0 || rect.GetTop() > clientSize.y)
        return FALSE;
    if (rect.GetRight() < 0 || rect.GetLeft() > clientSize.x)
        return FALSE;

    return TRUE;
}

bool wxGenericTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return !((wxGenericTreeItem*) item.m_pItem)->GetChildren().IsEmpty();
}

bool wxGenericTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->IsExpanded();
}

bool wxGenericTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->IsSelected();
}

bool wxGenericTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, wxT("invalid tree item") );

    return ((wxGenericTreeItem*) item.m_pItem)->IsBold();
}

// -----------------------------------------------------------------------------
// navigation
// -----------------------------------------------------------------------------

wxTreeItemId wxGenericTreeCtrl::GetParent(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  return ((wxGenericTreeItem*) item.m_pItem)->GetParent();
}

wxTreeItemId wxGenericTreeCtrl::GetFirstChild(const wxTreeItemId& item, long& cookie) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  cookie = 0;
  return GetNextChild(item, cookie);
}

wxTreeItemId wxGenericTreeCtrl::GetNextChild(const wxTreeItemId& item, long& cookie) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxArrayGenericTreeItems& children = ((wxGenericTreeItem*) item.m_pItem)->GetChildren();
  if ( (size_t)cookie < children.Count() )
  {
    return children.Item((size_t)cookie++);
  }
  else
  {
    // there are no more of them
    return wxTreeItemId();
  }
}

wxTreeItemId wxGenericTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxArrayGenericTreeItems& children = ((wxGenericTreeItem*) item.m_pItem)->GetChildren();
  return (children.IsEmpty() ? wxTreeItemId() : wxTreeItemId(children.Last()));
}

wxTreeItemId wxGenericTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxGenericTreeItem *i = (wxGenericTreeItem*) item.m_pItem;
  wxGenericTreeItem *parent = i->GetParent();
  if ( parent == NULL )
  {
    // root item doesn't have any siblings
    return wxTreeItemId();
  }

  wxArrayGenericTreeItems& siblings = parent->GetChildren();
  int index = siblings.Index(i);
  wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

  size_t n = (size_t)(index + 1);
  return n == siblings.Count() ? wxTreeItemId() : wxTreeItemId(siblings[n]);
}

wxTreeItemId wxGenericTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxGenericTreeItem *i = (wxGenericTreeItem*) item.m_pItem;
  wxGenericTreeItem *parent = i->GetParent();
  if ( parent == NULL )
  {
    // root item doesn't have any siblings
    return wxTreeItemId();
  }

  wxArrayGenericTreeItems& siblings = parent->GetChildren();
  int index = siblings.Index(i);
  wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

  return index == 0 ? wxTreeItemId()
                    : wxTreeItemId(siblings[(size_t)(index - 1)]);
}

// Only for internal use right now, but should probably be public
wxTreeItemId wxGenericTreeCtrl::GetNext(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxGenericTreeItem *i = (wxGenericTreeItem*) item.m_pItem;

  // First see if there are any children.
  wxArrayGenericTreeItems& children = i->GetChildren();
  if (children.GetCount() > 0)
  {
     return children.Item(0);
  }
  else
  {
     // Try a sibling of this or ancestor instead
     wxTreeItemId p = item;
     wxTreeItemId toFind;
     do
     {
        toFind = GetNextSibling(p);
        p = GetParent(p);
     } while (p.IsOk() && !toFind.IsOk());
     return toFind;
  }
}

wxTreeItemId wxGenericTreeCtrl::GetPrev(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxFAIL_MSG(wxT("not implemented"));

  return wxTreeItemId();
}

wxTreeItemId wxGenericTreeCtrl::GetFirstVisibleItem() const
{
  wxTreeItemId id = GetRootItem();
  if (!id.IsOk())
    return id;

  do
  {
    if (IsVisible(id))
        return id;
    id = GetNext(id);
  } while (id.IsOk());

  return wxTreeItemId();
}

wxTreeItemId wxGenericTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxTreeItemId id = item;
  while (id.IsOk())
  {
	  id = GetNext(id);

	  if (id.IsOk() && IsVisible(id))
		  return id;
  }
  return wxTreeItemId();
}

wxTreeItemId wxGenericTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
  wxCHECK_MSG( item.IsOk(), wxTreeItemId(), wxT("invalid tree item") );

  wxFAIL_MSG(wxT("not implemented"));

  return wxTreeItemId();
}

// -----------------------------------------------------------------------------
// operations
// -----------------------------------------------------------------------------

wxTreeItemId wxGenericTreeCtrl::DoInsertItem(const wxTreeItemId& parentId,
                                      size_t previous,
                                      const wxString& text,
                                      int image, int selImage,
                                      wxTreeItemData *data)
{
    wxGenericTreeItem *parent = (wxGenericTreeItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    wxClientDC dc(this);
    wxGenericTreeItem *item =
        new wxGenericTreeItem( parent, text, dc, image, selImage, data );

    if ( data != NULL )
    {
        data->m_pItem = (long) item;
    }

    parent->Insert( item, previous );

    m_dirty = TRUE;

    return item;
}

wxTreeItemId wxGenericTreeCtrl::AddRoot(const wxString& text,
                                 int image, int selImage,
                                 wxTreeItemData *data)
{
    wxCHECK_MSG( !m_anchor, wxTreeItemId(), wxT("tree can have only one root") );

    wxClientDC dc(this);
    m_anchor = new wxGenericTreeItem((wxGenericTreeItem *)NULL, text, dc,
                                   image, selImage, data);
    if ( data != NULL )
    {
        data->m_pItem = (long) m_anchor;
    }

    if (!HasFlag(wxTR_MULTIPLE))
    {
        m_current = m_key_current = m_anchor;
        m_current->SetHilight( TRUE );
    }

    m_dirty = TRUE;

    return m_anchor;
}

wxTreeItemId wxGenericTreeCtrl::PrependItem(const wxTreeItemId& parent,
                                     const wxString& text,
                                     int image, int selImage,
                                     wxTreeItemData *data)
{
    return DoInsertItem(parent, 0u, text, image, selImage, data);
}

wxTreeItemId wxGenericTreeCtrl::InsertItem(const wxTreeItemId& parentId,
                                    const wxTreeItemId& idPrevious,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
    wxGenericTreeItem *parent = (wxGenericTreeItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    int index = parent->GetChildren().Index((wxGenericTreeItem*) idPrevious.m_pItem);
    wxASSERT_MSG( index != wxNOT_FOUND,
                  wxT("previous item in wxGenericTreeCtrl::InsertItem() is not a sibling") );

    return DoInsertItem(parentId, (size_t)++index, text, image, selImage, data);
}

wxTreeItemId wxGenericTreeCtrl::InsertItem(const wxTreeItemId& parentId,
                                    size_t before,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
    wxGenericTreeItem *parent = (wxGenericTreeItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    return DoInsertItem(parentId, before, text, image, selImage, data);
}

wxTreeItemId wxGenericTreeCtrl::AppendItem(const wxTreeItemId& parentId,
                                    const wxString& text,
                                    int image, int selImage,
                                    wxTreeItemData *data)
{
    wxGenericTreeItem *parent = (wxGenericTreeItem*) parentId.m_pItem;
    if ( !parent )
    {
        // should we give a warning here?
        return AddRoot(text, image, selImage, data);
    }

    return DoInsertItem( parent, parent->GetChildren().Count(), text,
                         image, selImage, data);
}

void wxGenericTreeCtrl::SendDeleteEvent(wxGenericTreeItem *item)
{
    wxTreeEvent event( wxEVT_COMMAND_TREE_DELETE_ITEM, GetId() );
    event.m_item = (long) item;
    event.SetEventObject( this );
    ProcessEvent( event );
}

void wxGenericTreeCtrl::DeleteChildren(const wxTreeItemId& itemId)
{
    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;
    item->DeleteChildren(this);

    m_dirty = TRUE;
}

void wxGenericTreeCtrl::Delete(const wxTreeItemId& itemId)
{
    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;

    // don't stay with invalid m_key_current or we will crash in the next call
    // to OnChar()
    bool changeKeyCurrent = FALSE;
    wxGenericTreeItem *itemKey = m_key_current;
    while ( itemKey && !changeKeyCurrent )
    {
        if ( itemKey == item )
        {
            // m_key_current is a descendant of the item being deleted
            changeKeyCurrent = TRUE;
        }
        else
        {
            itemKey = itemKey->GetParent();
        }
    }

    wxGenericTreeItem *parent = item->GetParent();
    if ( parent )
    {
        parent->GetChildren().Remove( item );  // remove by value
    }

    if ( changeKeyCurrent )
    {
        // may be NULL or not
        m_key_current = parent;
    }

    item->DeleteChildren(this);
    SendDeleteEvent(item);
    delete item;

    m_dirty = TRUE;
}

void wxGenericTreeCtrl::DeleteAllItems()
{
    if ( m_anchor )
    {
        m_anchor->DeleteChildren(this);
        delete m_anchor;

        m_anchor = NULL;

        m_dirty = TRUE;
    }
}

void wxGenericTreeCtrl::Expand(const wxTreeItemId& itemId)
{
    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;

    wxCHECK_RET( item, _T("invalid item in wxGenericTreeCtrl::Expand") );

    if ( !item->HasPlus() )
        return;

    if ( item->IsExpanded() )
        return;

    wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_EXPANDING, GetId() );
    event.m_item = (long) item;
    event.SetEventObject( this );

    if ( ProcessEvent( event ) && !event.IsAllowed() )
    {
        // cancelled by program
        return;
    }

    item->Expand();
    CalculatePositions();

    RefreshSubtree(item);

    event.SetEventType(wxEVT_COMMAND_TREE_ITEM_EXPANDED);
    ProcessEvent( event );
}

void wxGenericTreeCtrl::ExpandAll(const wxTreeItemId& item)
{
    Expand(item);
    if ( IsExpanded(item) )
    {
        long cookie;
        wxTreeItemId child = GetFirstChild(item, cookie);
        while ( child.IsOk() )
        {
            ExpandAll(child);

            child = GetNextChild(item, cookie);
        }
    }
}

void wxGenericTreeCtrl::Collapse(const wxTreeItemId& itemId)
{
    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;

    if ( !item->IsExpanded() )
        return;

    wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_COLLAPSING, GetId() );
    event.m_item = (long) item;
    event.SetEventObject( this );
    if ( ProcessEvent( event ) && !event.IsAllowed() )
    {
        // cancelled by program
        return;
    }

    item->Collapse();

    wxArrayGenericTreeItems& children = item->GetChildren();
    size_t count = children.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        Collapse(children[n]);
    }

    CalculatePositions();

    RefreshSubtree(item);

    event.SetEventType(wxEVT_COMMAND_TREE_ITEM_COLLAPSED);
    ProcessEvent( event );
}

void wxGenericTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
    Collapse(item);
    DeleteChildren(item);
}

void wxGenericTreeCtrl::Toggle(const wxTreeItemId& itemId)
{
    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;

    if (item->IsExpanded())
        Collapse(itemId);
    else
        Expand(itemId);
}

void wxGenericTreeCtrl::Unselect()
{
    if (m_current)
    {
        m_current->SetHilight( FALSE );
        RefreshLine( m_current );
    }
}

void wxGenericTreeCtrl::UnselectAllChildren(wxGenericTreeItem *item)
{
    if (item->IsSelected())
    {
        item->SetHilight(FALSE);
        RefreshLine(item);
    }

    if (item->HasChildren())
    {
        wxArrayGenericTreeItems& children = item->GetChildren();
        size_t count = children.Count();
        for ( size_t n = 0; n < count; ++n )
        {
            UnselectAllChildren(children[n]);
        }
    }
}

void wxGenericTreeCtrl::UnselectAll()
{
    UnselectAllChildren((wxGenericTreeItem*) GetRootItem().m_pItem);
}

// Recursive function !
// To stop we must have crt_item<last_item
// Algorithm :
// Tag all next children, when no more children,
// Move to parent (not to tag)
// Keep going... if we found last_item, we stop.
bool wxGenericTreeCtrl::TagNextChildren(wxGenericTreeItem *crt_item, wxGenericTreeItem *last_item, bool select)
{
    wxGenericTreeItem *parent = crt_item->GetParent();

    if (parent == NULL) // This is root item
        return TagAllChildrenUntilLast(crt_item, last_item, select);

    wxArrayGenericTreeItems& children = parent->GetChildren();
    int index = children.Index(crt_item);
    wxASSERT( index != wxNOT_FOUND ); // I'm not a child of my parent?

    size_t count = children.Count();
    for (size_t n=(size_t)(index+1); n<count; ++n)
    {
        if (TagAllChildrenUntilLast(children[n], last_item, select)) return TRUE;
    }

    return TagNextChildren(parent, last_item, select);
}

bool wxGenericTreeCtrl::TagAllChildrenUntilLast(wxGenericTreeItem *crt_item, wxGenericTreeItem *last_item, bool select)
{
    crt_item->SetHilight(select);
    RefreshLine(crt_item);

    if (crt_item==last_item)
        return TRUE;

    if (crt_item->HasChildren())
    {
        wxArrayGenericTreeItems& children = crt_item->GetChildren();
        size_t count = children.Count();
        for ( size_t n = 0; n < count; ++n )
        {
            if (TagAllChildrenUntilLast(children[n], last_item, select))
                return TRUE;
        }
    }

  return FALSE;
}

void wxGenericTreeCtrl::SelectItemRange(wxGenericTreeItem *item1, wxGenericTreeItem *item2)
{
    // item2 is not necessary after item1
    wxGenericTreeItem *first=NULL, *last=NULL;

    // choice first' and 'last' between item1 and item2
    if (item1->GetY()<item2->GetY())
    {
        first=item1;
        last=item2;
    }
    else
    {
        first=item2;
        last=item1;
    }

    bool select = m_current->IsSelected();

    if ( TagAllChildrenUntilLast(first,last,select) )
        return;

    TagNextChildren(first,last,select);
}

void wxGenericTreeCtrl::SelectItem(const wxTreeItemId& itemId,
                            bool unselect_others,
                            bool extended_select)
{
    wxCHECK_RET( itemId.IsOk(), wxT("invalid tree item") );

    bool is_single=!(GetWindowStyleFlag() & wxTR_MULTIPLE);
    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;

    //wxCHECK_RET( ( (!unselect_others) && is_single),
    //           wxT("this is a single selection tree") );

    // to keep going anyhow !!!
    if (is_single)
    {
        if (item->IsSelected())
            return; // nothing to do
        unselect_others = TRUE;
        extended_select = FALSE;
    }
    else if ( unselect_others && item->IsSelected() )
    {
        // selection change if there is more than one item currently selected
        wxArrayTreeItemIds selected_items;
        if ( GetSelections(selected_items) == 1 )
            return;
    }

    wxTreeEvent event( wxEVT_COMMAND_TREE_SEL_CHANGING, GetId() );
    event.m_item = (long) item;
    event.m_itemOld = (long) m_current;
    event.SetEventObject( this );
    // TODO : Here we don't send any selection mode yet !

    if ( GetEventHandler()->ProcessEvent( event ) && !event.IsAllowed() )
      return;

    // ctrl press
    if (unselect_others)
    {
        if (is_single) Unselect(); // to speed up thing
        else UnselectAll();
    }

    // shift press
    if (extended_select)
    {
        if ( !m_current )
        {
            m_current =
            m_key_current = (wxGenericTreeItem*) GetRootItem().m_pItem;
        }

        // don't change the mark (m_current)
        SelectItemRange(m_current, item);
    }
    else
    {
        bool select=TRUE; // the default

        // Check if we need to toggle hilight (ctrl mode)
        if (!unselect_others)
          select=!item->IsSelected();

        m_current = m_key_current = item;
        m_current->SetHilight(select);
        RefreshLine( m_current );
    }

    event.SetEventType(wxEVT_COMMAND_TREE_SEL_CHANGED);
    GetEventHandler()->ProcessEvent( event );
}

void wxGenericTreeCtrl::FillArray(wxGenericTreeItem *item,
                           wxArrayTreeItemIds &array) const
{
    if ( item->IsSelected() )
        array.Add(wxTreeItemId(item));

    if ( item->HasChildren() )
    {
        wxArrayGenericTreeItems& children = item->GetChildren();
        size_t count = children.GetCount();
        for ( size_t n = 0; n < count; ++n )
            FillArray(children[n], array);
    }
}

size_t wxGenericTreeCtrl::GetSelections(wxArrayTreeItemIds &array) const
{
  array.Empty();
  wxTreeItemId idRoot = GetRootItem();
  if ( idRoot.IsOk() )
  {
      FillArray((wxGenericTreeItem*) idRoot.m_pItem, array);
  }
  //else: the tree is empty, so no selections

  return array.Count();
}

void wxGenericTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    if (!item.IsOk()) return;

    wxGenericTreeItem *gitem = (wxGenericTreeItem*) item.m_pItem;

    // first expand all parent branches
    wxGenericTreeItem *parent = gitem->GetParent();
    while ( parent )
    {
        Expand(parent);
        parent = parent->GetParent();
    }

    //if (parent) CalculatePositions();

    ScrollTo(item);
}

void wxGenericTreeCtrl::ScrollTo(const wxTreeItemId &item)
{
    if (!item.IsOk()) return;

    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) wxYield();

    wxGenericTreeItem *gitem = (wxGenericTreeItem*) item.m_pItem;

    // now scroll to the item
    int item_y = gitem->GetY();

    int start_x = 0;
    int start_y = 0;
    ViewStart( &start_x, &start_y );
    start_y *= PIXELS_PER_UNIT;

    int client_h = 0;
    int client_w = 0;
    GetClientSize( &client_w, &client_h );

    if (item_y < start_y+3)
    {
        // going down
        int x = 0;
        int y = 0;
        m_anchor->GetSize( x, y, this );
        y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        x += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        int x_pos = GetScrollPos( wxHORIZONTAL );
        // Item should appear at top
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, item_y/PIXELS_PER_UNIT );
    }
    else if (item_y+GetLineHeight(gitem) > start_y+client_h)
    {
        // going up
        int x = 0;
        int y = 0;
        m_anchor->GetSize( x, y, this );
        y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        x += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        item_y += PIXELS_PER_UNIT+2;
        int x_pos = GetScrollPos( wxHORIZONTAL );
        // Item should appear at bottom
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, (item_y+GetLineHeight(gitem)-client_h)/PIXELS_PER_UNIT );
    }
}

// FIXME: tree sorting functions are not reentrant and not MT-safe!
static wxGenericTreeCtrl *s_treeBeingSorted = NULL;

static int LINKAGEMODE tree_ctrl_compare_func(wxGenericTreeItem **item1,
                                  wxGenericTreeItem **item2)
{
    wxCHECK_MSG( s_treeBeingSorted, 0, wxT("bug in wxGenericTreeCtrl::SortChildren()") );

    return s_treeBeingSorted->OnCompareItems(*item1, *item2);
}

int wxGenericTreeCtrl::OnCompareItems(const wxTreeItemId& item1,
                               const wxTreeItemId& item2)
{
    return wxStrcmp(GetItemText(item1), GetItemText(item2));
}

void wxGenericTreeCtrl::SortChildren(const wxTreeItemId& itemId)
{
    wxCHECK_RET( itemId.IsOk(), wxT("invalid tree item") );

    wxGenericTreeItem *item = (wxGenericTreeItem*) itemId.m_pItem;

    wxCHECK_RET( !s_treeBeingSorted,
                 wxT("wxGenericTreeCtrl::SortChildren is not reentrant") );

    wxArrayGenericTreeItems& children = item->GetChildren();
    if ( children.Count() > 1 )
    {
        s_treeBeingSorted = this;
        children.Sort(tree_ctrl_compare_func);
        s_treeBeingSorted = NULL;

        m_dirty = TRUE;
    }
    //else: don't make the tree dirty as nothing changed
}

wxImageList *wxGenericTreeCtrl::GetImageList() const
{
    return m_imageListNormal;
}

wxImageList *wxGenericTreeCtrl::GetStateImageList() const
{
    return m_imageListState;
}

void wxGenericTreeCtrl::SetImageList(wxImageList *imageList)
{
    if (m_ownsImageListNormal) delete m_imageListNormal;

    m_imageListNormal = imageList;
    m_ownsImageListNormal = FALSE;

    if ( !m_imageListNormal )
        return;

    // Calculate a m_lineHeight value from the image sizes.
    // May be toggle off. Then wxGenericTreeCtrl will spread when
    // necessary (which might look ugly).
    wxClientDC dc(this);
    m_lineHeight = (int)(dc.GetCharHeight() + 4);
    int width = 0, height = 0,
        n = m_imageListNormal->GetImageCount();

    for (int i = 0; i < n ; i++)
    {
        m_imageListNormal->GetSize(i, width, height);
        if (height > m_lineHeight) m_lineHeight = height;
    }

    if (m_lineHeight < 40)
        m_lineHeight += 2;                 // at least 2 pixels
    else
        m_lineHeight += m_lineHeight/10;   // otherwise 10% extra spacing
}

void wxGenericTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    if (m_ownsImageListState) delete m_imageListState;
    m_imageListState = imageList;
    m_ownsImageListState = FALSE;
}

void wxGenericTreeCtrl::AssignImageList(wxImageList *imageList)
{
    SetImageList(imageList);
    m_ownsImageListNormal = TRUE;
}

void wxGenericTreeCtrl::AssignStateImageList(wxImageList *imageList)
{
    SetStateImageList(imageList);
    m_ownsImageListState = TRUE;
}

// -----------------------------------------------------------------------------
// helpers
// -----------------------------------------------------------------------------

void wxGenericTreeCtrl::AdjustMyScrollbars()
{
    if (m_anchor)
    {
        int x = 0;
        int y = 0;
        m_anchor->GetSize( x, y, this );
        y += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        x += PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
        int x_pos = GetScrollPos( wxHORIZONTAL );
        int y_pos = GetScrollPos( wxVERTICAL );
        SetScrollbars( PIXELS_PER_UNIT, PIXELS_PER_UNIT, x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT, x_pos, y_pos );
    }
    else
    {
        SetScrollbars( 0, 0, 0, 0 );
    }
}

int wxGenericTreeCtrl::GetLineHeight(wxGenericTreeItem *item) const
{
    if (GetWindowStyleFlag() & wxTR_HAS_VARIABLE_ROW_HEIGHT)
        return item->GetHeight();
    else
        return m_lineHeight;
}

void wxGenericTreeCtrl::PaintItem(wxGenericTreeItem *item, wxDC& dc)
{
    wxTreeItemAttr *attr = item->GetAttributes();
    if ( attr && attr->HasFont() )
        dc.SetFont(attr->GetFont());
    else if (item->IsBold())
        dc.SetFont(m_boldFont);

    long text_w = 0;
    long text_h = 0;
    dc.GetTextExtent( item->GetText(), &text_w, &text_h );

    int image_h = 0;
    int image_w = 0;
    int image = item->GetCurrentImage();
    if ( image != NO_IMAGE )
    {
        if ( m_imageListNormal )
        {
            m_imageListNormal->GetSize( image, image_w, image_h );
            image_w += 4;
        }
        else
        {
            image = NO_IMAGE;
        }
    }

    int total_h = GetLineHeight(item);

    if (item->IsSelected())
        dc.SetBrush(*m_hilightBrush);
    else
    {
        wxColour colBg;
        if ( attr && attr->HasBackgroundColour() )
            colBg = attr->GetBackgroundColour();
        else
            colBg = m_backgroundColour;
        dc.SetBrush(wxBrush(colBg, wxSOLID));
    }

    dc.DrawRectangle( item->GetX()-2, item->GetY(), item->GetWidth()+2, total_h );

    if ( image != NO_IMAGE )
    {
        dc.SetClippingRegion( item->GetX(), item->GetY(), image_w-2, total_h );
        m_imageListNormal->Draw( image, dc,
                                 item->GetX(),
                                 item->GetY() +((total_h > image_h)?((total_h-image_h)/2):0),
                                 wxIMAGELIST_DRAW_TRANSPARENT );
        dc.DestroyClippingRegion();
    }

    dc.SetBackgroundMode(wxTRANSPARENT);
    int extraH = (total_h > text_h) ? (total_h - text_h)/2 : 0;
    dc.DrawText( item->GetText(),
                 (wxCoord)(image_w + item->GetX()),
                 (wxCoord)(item->GetY() + extraH));

    // restore normal font
    dc.SetFont( m_normalFont );
}

// Now y stands for the top of the item, whereas it used to stand for middle !
void wxGenericTreeCtrl::PaintLevel( wxGenericTreeItem *item, wxDC &dc, int level, int &y )
{
    int horizX = level*m_indent;

    item->SetX( horizX+m_indent+m_spacing );
    item->SetY( y );

    int oldY = y;
    y+=GetLineHeight(item)/2;

    item->SetCross( horizX+m_indent, y );

    int exposed_x = dc.LogicalToDeviceX( 0 );
    int exposed_y = dc.LogicalToDeviceY( item->GetY() );

    bool drawLines = ((GetWindowStyle() & wxTR_NO_LINES) == 0);

    if (IsExposed( exposed_x, exposed_y, 10000, GetLineHeight(item) ))  // 10000 = very much
    {
        int startX = horizX;
        int endX = horizX + (m_indent-5);

//        if (!item->HasChildren()) endX += (m_indent+5);
        if (!item->HasChildren()) endX += 20;

        if (drawLines)
            dc.DrawLine( startX, y, endX, y );

        if (item->HasPlus())
        {
            if (drawLines)
                dc.DrawLine( horizX+(m_indent+5), y, horizX+(m_indent+15), y );
            dc.SetPen( *wxGREY_PEN );
            dc.SetBrush( *wxWHITE_BRUSH );
            dc.DrawRectangle( horizX+(m_indent-5), y-4, 11, 9 );

            dc.SetPen( *wxBLACK_PEN );
            dc.DrawLine( horizX+(m_indent-2), y, horizX+(m_indent+3), y );
            if (!item->IsExpanded())
                dc.DrawLine( horizX+m_indent, y-2, horizX+m_indent, y+3 );

            dc.SetPen( m_dottedPen );
        }

        wxPen *pen = wxTRANSPARENT_PEN;
        wxColour colText;

        if ( item->IsSelected() )
        {
            colText = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_HIGHLIGHTTEXT );

            if ( m_hasFocus )
               pen = wxBLACK_PEN;

        }
        else
        {
            wxTreeItemAttr *attr = item->GetAttributes();
            if ( attr && attr->HasTextColour() )
                colText = attr->GetTextColour();
            else
                colText = wxSystemSettings::GetSystemColour( wxSYS_COLOUR_WINDOWTEXT );
        }

        // prepare to draw
        dc.SetTextForeground(colText);
        dc.SetPen(*pen);

        // draw
        PaintItem(item, dc);

        // restore DC objects
        dc.SetBrush( *wxWHITE_BRUSH );
        dc.SetPen( m_dottedPen );
        dc.SetTextForeground( *wxBLACK );
    }

    y = oldY+GetLineHeight(item);

    if (item->IsExpanded())
    {
        oldY+=GetLineHeight(item)/2;
        int semiOldY=0;

        wxArrayGenericTreeItems& children = item->GetChildren();
        size_t n, count = children.Count();
        for ( n = 0; n < count; ++n )
        {
            semiOldY=y;
            PaintLevel( children[n], dc, level+1, y );
        }

        // it may happen that the item is expanded but has no items (when you
        // delete all its children for example) - don't draw the vertical line
        // in this case
        if (count > 0)
        {
            semiOldY+=GetLineHeight(children[--n])/2;
            if (drawLines)
                dc.DrawLine( horizX+m_indent, oldY+5, horizX+m_indent, semiOldY );
        }
    }
}

void wxGenericTreeCtrl::DrawDropEffect(wxGenericTreeItem *item)
{
    if ( item )
    {
        if ( item->HasPlus() )
        {
            // it's a folder, indicate it by a border
            DrawBorder(item);
        }
        else
        {
            // draw a line under the drop target because the item will be
            // dropped there
            DrawLine(item, TRUE /* below */);
        }

        SetCursor(wxCURSOR_BULLSEYE);
    }
    else
    {
        // can't drop here
        SetCursor(wxCURSOR_NO_ENTRY);
    }
}

void wxGenericTreeCtrl::DrawBorder(const wxTreeItemId &item)
{
    wxCHECK_RET( item.IsOk(), _T("invalid item in wxGenericTreeCtrl::DrawLine") );

    wxGenericTreeItem *i = (wxGenericTreeItem*) item.m_pItem;

    wxClientDC dc(this);
    PrepareDC( dc );
    dc.SetLogicalFunction(wxINVERT);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    int w = i->GetWidth() + 2;
    int h = GetLineHeight(i) + 2;

    dc.DrawRectangle( i->GetX() - 1, i->GetY() - 1, w, h);
}

void wxGenericTreeCtrl::DrawLine(const wxTreeItemId &item, bool below)
{
    wxCHECK_RET( item.IsOk(), _T("invalid item in wxGenericTreeCtrl::DrawLine") );

    wxGenericTreeItem *i = (wxGenericTreeItem*) item.m_pItem;

    wxClientDC dc(this);
    PrepareDC( dc );
    dc.SetLogicalFunction(wxINVERT);

    int x = i->GetX(),
        y = i->GetY();
    if ( below )
    {
        y += GetLineHeight(i) - 1;
    }

    dc.DrawLine( x, y, x + i->GetWidth(), y);
}

// -----------------------------------------------------------------------------
// wxWindows callbacks
// -----------------------------------------------------------------------------

void wxGenericTreeCtrl::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC( dc );

    if ( !m_anchor)
        return;

    dc.SetFont( m_normalFont );
    dc.SetPen( m_dottedPen );

    // this is now done dynamically
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    int y = 2;
    PaintLevel( m_anchor, dc, 0, y );
}

void wxGenericTreeCtrl::OnSetFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_hasFocus = TRUE;

    if (m_current) RefreshLine( m_current );
}

void wxGenericTreeCtrl::OnKillFocus( wxFocusEvent &WXUNUSED(event) )
{
    m_hasFocus = FALSE;

    if (m_current) RefreshLine( m_current );
}

void wxGenericTreeCtrl::OnChar( wxKeyEvent &event )
{
    wxTreeEvent te( wxEVT_COMMAND_TREE_KEY_DOWN, GetId() );
    te.m_code = (int)event.KeyCode();
    te.SetEventObject( this );
    GetEventHandler()->ProcessEvent( te );

    if ( (m_current == 0) || (m_key_current == 0) )
    {
        event.Skip();
        return;
    }

    // how should the selection work for this event?
    bool is_multiple, extended_select, unselect_others;
    EventFlagsToSelType(GetWindowStyleFlag(),
                        event.ShiftDown(),
                        event.ControlDown(),
                        is_multiple, extended_select, unselect_others);

    // + : Expand
    // - : Collaspe
    // * : Expand all/Collapse all
    // ' ' | return : activate
    // up    : go up (not last children!)
    // down  : go down
    // left  : go to parent
    // right : open if parent and go next
    // home  : go to root
    // end   : go to last item without opening parents
    switch (event.KeyCode())
    {
        case '+':
        case WXK_ADD:
            if (m_current->HasPlus() && !IsExpanded(m_current))
            {
                Expand(m_current);
            }
            break;

        case '*':
        case WXK_MULTIPLY:
            if ( !IsExpanded(m_current) )
            {
                // expand all
                ExpandAll(m_current);
                break;
            }
            //else: fall through to Collapse() it

        case '-':
        case WXK_SUBTRACT:
            if (IsExpanded(m_current))
            {
                Collapse(m_current);
            }
            break;

        case ' ':
        case WXK_RETURN:
            {
                wxTreeEvent event( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, GetId() );
                event.m_item = (long) m_current;
                event.m_code = 0;
                event.SetEventObject( this );
                GetEventHandler()->ProcessEvent( event );
            }
            break;

            // up goes to the previous sibling or to the last of its children if
            // it's expanded
        case WXK_UP:
            {
                wxTreeItemId prev = GetPrevSibling( m_key_current );
                if (!prev)
                {
                    prev = GetParent( m_key_current );
                    if (prev)
                    {
                        long cockie = 0;
                        wxTreeItemId current = m_key_current;
                        if (current == GetFirstChild( prev, cockie ))
                        {
                            // otherwise we return to where we came from
                            SelectItem( prev, unselect_others, extended_select );
                            m_key_current= (wxGenericTreeItem*) prev.m_pItem;
                            EnsureVisible( prev );
                            break;
                        }
                    }
                }
                if (prev)
                {
                    while ( IsExpanded(prev) && HasChildren(prev) )
                    {
                        wxTreeItemId child = GetLastChild(prev);
                        if ( child )
                        {
                            prev = child;
                        }
                    }

                    SelectItem( prev, unselect_others, extended_select );
                    m_key_current=(wxGenericTreeItem*) prev.m_pItem;
                    EnsureVisible( prev );
                }
            }
            break;

            // left arrow goes to the parent
        case WXK_LEFT:
            {
                wxTreeItemId prev = GetParent( m_current );
                if (prev)
                {
                    EnsureVisible( prev );
                    SelectItem( prev, unselect_others, extended_select );
                }
            }
            break;

        case WXK_RIGHT:
            // this works the same as the down arrow except that we also expand the
            // item if it wasn't expanded yet
            Expand(m_current);
            // fall through

        case WXK_DOWN:
            {
                if (IsExpanded(m_key_current) && HasChildren(m_key_current))
                {
                    long cookie = 0;
                    wxTreeItemId child = GetFirstChild( m_key_current, cookie );
                    SelectItem( child, unselect_others, extended_select );
                    m_key_current=(wxGenericTreeItem*) child.m_pItem;
                    EnsureVisible( child );
                }
                else
                {
                    wxTreeItemId next = GetNextSibling( m_key_current );
                    if (!next)
                    {
                        wxTreeItemId current = m_key_current;
                        while (current && !next)
                        {
                            current = GetParent( current );
                            if (current) next = GetNextSibling( current );
                        }
                    }
                    if (next)
                    {
                        SelectItem( next, unselect_others, extended_select );
                        m_key_current=(wxGenericTreeItem*) next.m_pItem;
                        EnsureVisible( next );
                    }
                }
            }
            break;

            // <End> selects the last visible tree item
        case WXK_END:
            {
                wxTreeItemId last = GetRootItem();

                while ( last.IsOk() && IsExpanded(last) )
                {
                    wxTreeItemId lastChild = GetLastChild(last);

                    // it may happen if the item was expanded but then all of
                    // its children have been deleted - so IsExpanded() returned
                    // TRUE, but GetLastChild() returned invalid item
                    if ( !lastChild )
                        break;

                    last = lastChild;
                }

                if ( last.IsOk() )
                {
                    EnsureVisible( last );
                    SelectItem( last, unselect_others, extended_select );
                }
            }
            break;

            // <Home> selects the root item
        case WXK_HOME:
            {
                wxTreeItemId prev = GetRootItem();
                if (prev)
                {
                    EnsureVisible( prev );
                    SelectItem( prev, unselect_others, extended_select );
                }
            }
            break;

        default:
            event.Skip();
    }
}

wxTreeItemId wxGenericTreeCtrl::HitTest(const wxPoint& point, int& flags)
{
    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) wxYield();

    wxClientDC dc(this);
    PrepareDC(dc);
    wxCoord x = dc.DeviceToLogicalX( point.x );
    wxCoord y = dc.DeviceToLogicalY( point.y );
    int w, h;
    GetSize(&w, &h);

    flags=0;
    if (point.x<0) flags|=wxTREE_HITTEST_TOLEFT;
    if (point.x>w) flags|=wxTREE_HITTEST_TORIGHT;
    if (point.y<0) flags|=wxTREE_HITTEST_ABOVE;
    if (point.y>h) flags|=wxTREE_HITTEST_BELOW;

    if (m_anchor)
        return m_anchor->HitTest( wxPoint(x, y), this, flags);
    else
        return wxTreeItemId();
}

// get the bounding rectangle of the item (or of its label only)
bool wxGenericTreeCtrl::GetBoundingRect(const wxTreeItemId& item,
                         wxRect& rect,
                         bool textOnly) const
{
    wxCHECK_MSG( item.IsOk(), FALSE, _T("invalid item in wxGenericTreeCtrl::GetBoundingRect") );

    wxGenericTreeItem *i = (wxGenericTreeItem*) item.m_pItem;

    int startX, startY;
    GetViewStart(& startX, & startY);

    rect.x = i->GetX() - startX*PIXELS_PER_UNIT;
	rect.y = i->GetY() - startY*PIXELS_PER_UNIT;
    rect.width = i->GetWidth();
	//rect.height = i->GetHeight();
	rect.height = GetLineHeight(i);

    return TRUE;
}

/* **** */

void wxGenericTreeCtrl::Edit( const wxTreeItemId& item )
{
    if (!item.IsOk()) return;

    m_currentEdit = (wxGenericTreeItem*) item.m_pItem;

    wxTreeEvent te( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, GetId() );
    te.m_item = (long) m_currentEdit;
    te.SetEventObject( this );
    GetEventHandler()->ProcessEvent( te );

    if (!te.IsAllowed()) return;

    // We have to call this here because the label in
    // question might just have been added and no screen
    // update taken place.
    if (m_dirty) wxYield();

    wxString s = m_currentEdit->GetText();
    int x = m_currentEdit->GetX();
    int y = m_currentEdit->GetY();
    int w = m_currentEdit->GetWidth();
    int h = m_currentEdit->GetHeight();

    int image_h = 0;
    int image_w = 0;

    int image = m_currentEdit->GetCurrentImage();
    if ( image != NO_IMAGE )
    {
        if ( m_imageListNormal )
        {
            m_imageListNormal->GetSize( image, image_w, image_h );
            image_w += 4;
        }
        else
        {
            wxFAIL_MSG(_T("you must create an image list to use images!"));
        }
    }
    x += image_w;
    w -= image_w + 4; // I don't know why +4 is needed

    wxClientDC dc(this);
    PrepareDC( dc );
    x = dc.LogicalToDeviceX( x );
    y = dc.LogicalToDeviceY( y );

    wxTreeTextCtrl *text = new wxTreeTextCtrl(
      this, -1, &m_renameAccept, &m_renameRes, this, s, wxPoint(x-4,y-4), wxSize(w+11,h+8) );
    text->SetFocus();
}

void wxGenericTreeCtrl::OnRenameTimer()
{
    Edit( m_current );
}

void wxGenericTreeCtrl::OnRenameAccept()
{
    wxTreeEvent le( wxEVT_COMMAND_TREE_END_LABEL_EDIT, GetId() );
    le.m_item = (long) m_currentEdit;
    le.SetEventObject( this );
    le.m_label = m_renameRes;
    GetEventHandler()->ProcessEvent( le );

    if (!le.IsAllowed()) return;

    SetItemText( m_currentEdit, m_renameRes );
}

void wxGenericTreeCtrl::OnMouse( wxMouseEvent &event )
{
    if ( !m_anchor ) return;

    // we process left mouse up event (enables in-place edit), right down
    // (pass to the user code), left dbl click (activate item) and
    // dragging/moving events for items drag-and-drop
    if ( !(event.LeftDown() ||
           event.LeftUp() ||
           event.RightDown() ||
           event.LeftDClick() ||
           event.Dragging() ||
           ((event.Moving() || event.RightUp()) && m_isDragging)) )
    {
        event.Skip();

        return;
    }

    wxClientDC dc(this);
    PrepareDC(dc);
    wxCoord x = dc.DeviceToLogicalX( event.GetX() );
    wxCoord y = dc.DeviceToLogicalY( event.GetY() );

    int flags = 0;
    wxGenericTreeItem *item = m_anchor->HitTest( wxPoint(x,y), this, flags);

    if ( event.Dragging() && !m_isDragging )
    {
        if (m_dragCount == 0)
            m_dragStart = wxPoint(x,y);

        m_dragCount++;

        if (m_dragCount != 3)
        {
            // wait until user drags a bit further...
            return;
        }

        wxEventType command = event.RightIsDown()
                              ? wxEVT_COMMAND_TREE_BEGIN_RDRAG
                              : wxEVT_COMMAND_TREE_BEGIN_DRAG;

        wxTreeEvent nevent( command, GetId() );
        nevent.m_item = (long) m_current;
        nevent.SetEventObject(this);

        // by default the dragging is not supported, the user code must
        // explicitly allow the event for it to take place
        nevent.Veto();

        if ( GetEventHandler()->ProcessEvent(nevent) && nevent.IsAllowed() )
        {
            // we're going to drag this item
            m_isDragging = TRUE;

            // remember the old cursor because we will change it while
            // dragging
            m_oldCursor = m_cursor;

            // in a single selection control, hide the selection temporarily
            if ( !(GetWindowStyleFlag() & wxTR_MULTIPLE) )
            {
                m_oldSelection = (wxGenericTreeItem*) GetSelection().m_pItem;

                if ( m_oldSelection )
                {
                    m_oldSelection->SetHilight(FALSE);
                    RefreshLine(m_oldSelection);
                }
            }

            CaptureMouse();
        }
    }
    else if ( event.Moving() )
    {
        if ( item != m_dropTarget )
        {
            // unhighlight the previous drop target
            DrawDropEffect(m_dropTarget);

            m_dropTarget = item;

            // highlight the current drop target if any
            DrawDropEffect(m_dropTarget);

            wxYield();
        }
    }
    else if ( (event.LeftUp() || event.RightUp()) && m_isDragging )
    {
        // erase the highlighting
        DrawDropEffect(m_dropTarget);

        // generate the drag end event
        wxTreeEvent event(wxEVT_COMMAND_TREE_END_DRAG, GetId());

        event.m_item = (long) item;
        event.m_pointDrag = wxPoint(x, y);
        event.SetEventObject(this);

        (void)GetEventHandler()->ProcessEvent(event);

        m_isDragging = FALSE;
        m_dropTarget = (wxGenericTreeItem *)NULL;

        if ( m_oldSelection )
        {
            m_oldSelection->SetHilight(TRUE);
            RefreshLine(m_oldSelection);
            m_oldSelection = (wxGenericTreeItem *)NULL;
        }

        ReleaseMouse();

        SetCursor(m_oldCursor);

        wxYield();
    }
    else
    {
        // here we process only the messages which happen on tree items

        m_dragCount = 0;

        if (item == NULL) return;  /* we hit the blank area */

        if ( event.RightDown() )
        {
            wxTreeEvent nevent(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, GetId());
            nevent.m_item = (long) item;
            nevent.m_code = 0;
            CalcScrolledPosition(x, y,
                                 &nevent.m_pointDrag.x,
                                 &nevent.m_pointDrag.y);
            nevent.SetEventObject(this);
            GetEventHandler()->ProcessEvent(nevent);
        }
        else if ( event.LeftUp() )
        {
            if ( m_lastOnSame )
            {
                if ( (item == m_current) &&
                     (flags & wxTREE_HITTEST_ONITEMLABEL) &&
                     HasFlag(wxTR_EDIT_LABELS) )
                {
                    if ( m_renameTimer->IsRunning() )
                        m_renameTimer->Stop();

                    m_renameTimer->Start( 100, TRUE );
                }

                m_lastOnSame = FALSE;
            }
        }
        else
        {
            if ( event.LeftDown() )
            {
                m_lastOnSame = item == m_current;
            }

            // how should the selection work for this event?
            bool is_multiple, extended_select, unselect_others;
            EventFlagsToSelType(GetWindowStyleFlag(),
                                event.ShiftDown(),
                                event.ControlDown(),
                                is_multiple, extended_select, unselect_others);

            if ( (flags & wxTREE_HITTEST_ONITEMBUTTON) && event.LeftDown() )
            {
                Toggle( item );
                if ( is_multiple )
                    return;
            }

            SelectItem(item, unselect_others, extended_select);

            if ( event.LeftDClick() )
            {
                // double clicking should not start editing the item label
                m_renameTimer->Stop();
                m_lastOnSame = FALSE;

                wxTreeEvent nevent( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, GetId() );
                nevent.m_item = (long) item;
                nevent.m_code = 0;
                CalcScrolledPosition(x, y,
                                     &nevent.m_pointDrag.x,
                                     &nevent.m_pointDrag.y);
                nevent.SetEventObject( this );
                GetEventHandler()->ProcessEvent( nevent );
            }
        }
    }
}

void wxGenericTreeCtrl::OnIdle( wxIdleEvent &WXUNUSED(event) )
{
    /* after all changes have been done to the tree control,
     * we actually redraw the tree when everything is over */

    if (!m_dirty)
        return;

    m_dirty = FALSE;

    CalculatePositions();
    Refresh();
    AdjustMyScrollbars();
}

void wxGenericTreeCtrl::CalculateSize( wxGenericTreeItem *item, wxDC &dc )
{
    wxCoord text_w = 0;
    wxCoord text_h = 0;

    if (item->IsBold())
        dc.SetFont(m_boldFont);

    dc.GetTextExtent( item->GetText(), &text_w, &text_h );
    text_h+=2;

    // restore normal font
    dc.SetFont( m_normalFont );

    int image_h = 0;
    int image_w = 0;
    int image = item->GetCurrentImage();
    if ( image != NO_IMAGE )
    {
        if ( m_imageListNormal )
        {
            m_imageListNormal->GetSize( image, image_w, image_h );
            image_w += 4;
        }
    }

    int total_h = (image_h > text_h) ? image_h : text_h;

    if (total_h < 40)
        total_h += 2;            // at least 2 pixels
    else
        total_h += total_h/10;   // otherwise 10% extra spacing

    item->SetHeight(total_h);
    if (total_h>m_lineHeight)
        m_lineHeight=total_h;

    item->SetWidth(image_w+text_w+2);
}

// -----------------------------------------------------------------------------
// for developper : y is now the top of the level
// not the middle of it !
void wxGenericTreeCtrl::CalculateLevel( wxGenericTreeItem *item, wxDC &dc, int level, int &y )
{
    int horizX = level*m_indent;

    CalculateSize( item, dc );

    // set its position
    item->SetX( horizX+m_indent+m_spacing );
    item->SetY( y );
    y+=GetLineHeight(item);

    if ( !item->IsExpanded() )
    {
        // we dont need to calculate collapsed branches
        return;
    }

    wxArrayGenericTreeItems& children = item->GetChildren();
    size_t n, count = children.Count();
    for (n = 0; n < count; ++n )
        CalculateLevel( children[n], dc, level+1, y );  // recurse
}

void wxGenericTreeCtrl::CalculatePositions()
{
    if ( !m_anchor ) return;

    wxClientDC dc(this);
    PrepareDC( dc );

    dc.SetFont( m_normalFont );

    dc.SetPen( m_dottedPen );
    //if(GetImageList() == NULL)
    // m_lineHeight = (int)(dc.GetCharHeight() + 4);

    int y = 2;
    CalculateLevel( m_anchor, dc, 0, y ); // start recursion
}

void wxGenericTreeCtrl::RefreshSubtree(wxGenericTreeItem *item)
{
    if (m_dirty) return;

    wxClientDC dc(this);
    PrepareDC(dc);

    int cw = 0;
    int ch = 0;
    GetClientSize( &cw, &ch );

    wxRect rect;
    rect.x = dc.LogicalToDeviceX( 0 );
    rect.width = cw;
    rect.y = dc.LogicalToDeviceY( item->GetY() );
    rect.height = ch;

    Refresh( TRUE, &rect );

    AdjustMyScrollbars();
}

void wxGenericTreeCtrl::RefreshLine( wxGenericTreeItem *item )
{
    if (m_dirty) return;

    wxClientDC dc(this);
    PrepareDC( dc );

    int cw = 0;
    int ch = 0;
    GetClientSize( &cw, &ch );

    wxRect rect;
    rect.x = dc.LogicalToDeviceX( 0 );
    rect.y = dc.LogicalToDeviceY( item->GetY() );
    rect.width = cw;
    rect.height = GetLineHeight(item); //dc.GetCharHeight() + 6;

    Refresh( TRUE, &rect );
}

