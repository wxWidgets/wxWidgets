/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/datavgen.cpp
// Purpose:     wxDataViewCtrl generic implementation
// Author:      Robert Roebling
// Modified by: Francesco Montorsi, Guru Kathiresan, Bo Yang
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifdef wxUSE_GENERICDATAVIEWCTRL

#ifndef WX_PRECOMP
    #ifdef __WXMSW__
        #include "wx/msw/private.h"
        #include "wx/msw/wrapwin.h"
        #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #endif
    #include "wx/sizer.h"
    #include "wx/log.h"
    #include "wx/dcclient.h"
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/dcscreen.h"
#endif

#include "wx/stockitem.h"
#include "wx/calctrl.h"
#include "wx/popupwin.h"
#include "wx/renderer.h"
#include "wx/dcbuffer.h"
#include "wx/icon.h"
#include "wx/list.h"
#include "wx/listimpl.cpp"
#include "wx/imaglist.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDataViewCtrl;

static const int SCROLL_UNIT_X = 15;

// the cell padding on the left/right
static const int PADDING_RIGHTLEFT = 3;

// the expander space margin
static const int EXPANDER_MARGIN = 4;

#ifdef __WXMSW__
static const int EXPANDER_OFFSET = 4;
#else
static const int EXPANDER_OFFSET = 1;
#endif

//-----------------------------------------------------------------------------
// wxDataViewHeaderWindow
//-----------------------------------------------------------------------------

// on wxMSW the header window (only that part however) can be made native!
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #define USE_NATIVE_HEADER_WINDOW
#endif

//Below is the compare stuff
//For the generic implements, both the leaf nodes and the nodes are sorted for fast search when needed
static wxDataViewModel * g_model;
static int g_column = -2;
static bool g_asending = true;

// NB: for some reason, this class must be dllexport'ed or we get warnings from
//     MSVC in DLL build
class WXDLLIMPEXP_ADV wxDataViewHeaderWindowBase : public wxControl
{
public:
    wxDataViewHeaderWindowBase()
        { m_owner = NULL; }

    bool Create(wxDataViewCtrl *parent, wxWindowID id,
                const wxPoint &pos, const wxSize &size,
                const wxString &name)
    {
        return wxWindow::Create(parent, id, pos, size, wxNO_BORDER, name);
    }

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }

    // called on column addition/removal
    virtual void UpdateDisplay() { /* by default, do nothing */ }

    // returns the n-th column
    virtual wxDataViewColumn *GetColumn(unsigned int n)
    {
        wxASSERT(m_owner);
        wxDataViewColumn *ret = m_owner->GetColumn(n);
        wxASSERT(ret);

        return ret;
    }

protected:
    wxDataViewCtrl      *m_owner;

    // sends an event generated from the n-th wxDataViewColumn
    void SendEvent(wxEventType type, unsigned int n);
};

#ifdef USE_NATIVE_HEADER_WINDOW

#define COLUMN_WIDTH_OFFSET         2
#define wxDataViewHeaderWindowMSW   wxDataViewHeaderWindow

class wxDataViewHeaderWindowMSW : public wxDataViewHeaderWindowBase
{
public:

    wxDataViewHeaderWindowMSW( wxDataViewCtrl *parent,
                               wxWindowID id,
                               const wxPoint &pos = wxDefaultPosition,
                               const wxSize &size = wxDefaultSize,
                               const wxString &name = wxT("wxdataviewctrlheaderwindow") )
    {
        Create(parent, id, pos, size, name);
    }

    bool Create(wxDataViewCtrl *parent, wxWindowID id,
                const wxPoint &pos, const wxSize &size,
                const wxString &name);

    ~wxDataViewHeaderWindowMSW();

    // called when any column setting is changed and/or changed
    // the column count
    virtual void UpdateDisplay();

    virtual void OnInternalIdle();

    // called Refresh afterwards
    virtual void ScrollWindow(int dx, int dy, const wxRect *rect = NULL);

    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

protected:
    virtual bool MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result);

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags);

    wxSize DoGetBestSize() const;

    unsigned int GetColumnIdxFromHeader(NMHEADER *nmHDR);

    wxDataViewColumn *GetColumnFromHeader(NMHEADER *nmHDR)
        { return GetColumn(GetColumnIdxFromHeader(nmHDR)); }

    int          m_scrollOffsetX;
    int          m_buttonHeight;
    bool         m_vetoColumnDrag;
    bool         m_delayedUpdate;
    wxImageList *m_imageList;

private:
    DECLARE_DYNAMIC_CLASS(wxDataViewHeaderWindowMSW)
};

#else       // !defined(__WXMSW__)

#define HEADER_WINDOW_HEIGHT            25
#define HEADER_HORIZ_BORDER             5
#define HEADER_VERT_BORDER              3
#define wxGenericDataViewHeaderWindow   wxDataViewHeaderWindow

class wxGenericDataViewHeaderWindow : public wxDataViewHeaderWindowBase
{
public:
    wxGenericDataViewHeaderWindow( wxDataViewCtrl *parent,
                                    wxWindowID id,
                                    const wxPoint &pos = wxDefaultPosition,
                                    const wxSize &size = wxDefaultSize,
                                    const wxString &name = wxT("wxdataviewctrlheaderwindow") )
    {
        Init();
        Create(parent, id, pos, size, name);
    }

    bool Create(wxDataViewCtrl *parent, wxWindowID id,
                const wxPoint &pos, const wxSize &size,
                const wxString &name);

    ~wxGenericDataViewHeaderWindow()
    {
        delete m_resizeCursor;
    }

    virtual void UpdateDisplay() { Refresh(); }

    // event handlers:

    void OnPaint( wxPaintEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );


protected:

    // vars used for column resizing:

    wxCursor            *m_resizeCursor;
    const wxCursor      *m_currentCursor;
    bool                 m_isDragging;

    bool                 m_dirty;     // needs refresh?
    int                  m_hover;     // index of the column under the mouse
    int                  m_column;    // index of the column being resized
    int                  m_currentX;  // divider line position in logical (unscrolled) coords
    int                  m_minX;      // minimal position beyond which the divider line
                                      // can't be dragged in logical coords

    // the pen used to draw the current column width drag line
    // when resizing the columsn
    wxPen m_penCurrent;


    // internal utilities:

    void Init()
    {
        m_currentCursor = (wxCursor *) NULL;
        m_resizeCursor = new wxCursor( wxCURSOR_SIZEWE );

        m_isDragging = false;
        m_dirty = false;

        m_hover = wxNOT_FOUND;
        m_column = wxNOT_FOUND;
        m_currentX = 0;
        m_minX = 0;

        wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
        m_penCurrent = wxPen(col, 1, wxSOLID);
    }

    void AdjustDC(wxDC& dc);

private:
    DECLARE_DYNAMIC_CLASS(wxGenericDataViewHeaderWindow)
    DECLARE_EVENT_TABLE()
};

#endif      // defined(__WXMSW__)

//-----------------------------------------------------------------------------
// wxDataViewRenameTimer
//-----------------------------------------------------------------------------

class wxDataViewRenameTimer: public wxTimer
{
private:
    wxDataViewMainWindow *m_owner;

public:
    wxDataViewRenameTimer( wxDataViewMainWindow *owner );
    void Notify();
};

//-----------------------------------------------------------------------------
// wxDataViewTreeNode
//-----------------------------------------------------------------------------
class wxDataViewTreeNode;
WX_DEFINE_ARRAY( wxDataViewTreeNode *, wxDataViewTreeNodes );
WX_DEFINE_ARRAY( void* , wxDataViewTreeLeaves);

int LINKAGEMODE wxGenericTreeModelNodeCmp( wxDataViewTreeNode ** node1, wxDataViewTreeNode ** node2);
int LINKAGEMODE wxGenericTreeModelItemCmp( void ** id1, void ** id2);

class wxDataViewTreeNode
{
public:
    wxDataViewTreeNode( wxDataViewTreeNode * parent = NULL )
    {
        m_parent = parent;
        if (!parent)
            m_open = true;
        else
            m_open = false;
        m_hasChildren = false;
        m_subTreeCount  = 0;
    }

    ~wxDataViewTreeNode()
    {
    }

    wxDataViewTreeNode * GetParent() const { return m_parent; }
    void SetParent( wxDataViewTreeNode * parent ) { m_parent = parent; }
    wxDataViewTreeNodes &  GetNodes() { return m_nodes; }
    wxDataViewTreeLeaves & GetChildren() { return m_leaves; }

    void AddNode( wxDataViewTreeNode * node )
    {
        m_leaves.Add( node->GetItem().GetID() );
        if (g_column >= -1)
            m_leaves.Sort( &wxGenericTreeModelItemCmp );
        m_nodes.Add( node );
        if (g_column >= -1)
            m_nodes.Sort( &wxGenericTreeModelNodeCmp );
    }
    void AddLeaf( void * leaf )
    {
        m_leaves.Add( leaf );
        if (g_column >= -1)
            m_leaves.Sort( &wxGenericTreeModelItemCmp );
    }

    wxDataViewItem & GetItem() { return m_item; }
    const wxDataViewItem & GetItem() const { return m_item; }
    void SetItem( const wxDataViewItem & item ) { m_item = item; }

    unsigned int GetChildrenNumber() const { return m_leaves.GetCount(); }
    unsigned int GetNodeNumber() const { return m_nodes.GetCount(); }
    int GetIndentLevel() const
    {
        int ret = 0;
        const wxDataViewTreeNode * node = this;
        while( node->GetParent()->GetParent() != NULL )
        {
            node = node->GetParent();
            ret ++;
        }
        return ret;
    }

    bool IsOpen() const
    {
        return m_open;
    }

    void ToggleOpen()
    {
        int len = m_nodes.GetCount();
        int sum = 0;
        for ( int i = 0;i < len; i ++)
            sum += m_nodes[i]->GetSubTreeCount();

        sum += m_leaves.GetCount();
        if (m_open)
        {
            ChangeSubTreeCount(-sum);
            m_open = !m_open;
        }
        else
        {
            m_open = !m_open;
            ChangeSubTreeCount(sum);
        }
    }
    bool HasChildren() const { return m_hasChildren; }
    void SetHasChildren( bool has ){ m_hasChildren = has; }

    void SetSubTreeCount( int num ) { m_subTreeCount = num; }
    int GetSubTreeCount() const { return m_subTreeCount; }
    void ChangeSubTreeCount( int num )
    {
        if( !m_open )
            return;
        m_subTreeCount += num;
        if( m_parent )
            m_parent->ChangeSubTreeCount(num);
    }

    void Resort()
    {
        if (g_column >= -1)
        {
            m_nodes.Sort( &wxGenericTreeModelNodeCmp );
            int len = m_nodes.GetCount();
            for (int i = 0; i < len; i ++)
                m_nodes[i]->Resort();
            m_leaves.Sort( &wxGenericTreeModelItemCmp );
        }
    }

private:
    wxDataViewTreeNode  *m_parent;
    wxDataViewTreeNodes  m_nodes;
    wxDataViewTreeLeaves m_leaves;
    wxDataViewItem       m_item;
    bool                 m_open;
    bool                 m_hasChildren;
    int                  m_subTreeCount;
};

int LINKAGEMODE wxGenericTreeModelNodeCmp( wxDataViewTreeNode ** node1, wxDataViewTreeNode ** node2)
{
    return g_model->Compare( (*node1)->GetItem(), (*node2)->GetItem(), g_column, g_asending );
}

int LINKAGEMODE wxGenericTreeModelItemCmp( void ** id1, void ** id2)
{
    return g_model->Compare( *id1, *id2, g_column, g_asending );
}



//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SIZE_T(unsigned int, wxDataViewSelection,
                                            WXDLLIMPEXP_ADV);
WX_DECLARE_LIST(wxDataViewItem, ItemList);
WX_DEFINE_LIST(ItemList)

class wxDataViewMainWindow: public wxWindow
{
public:
    wxDataViewMainWindow( wxDataViewCtrl *parent,
                            wxWindowID id,
                            const wxPoint &pos = wxDefaultPosition,
                            const wxSize &size = wxDefaultSize,
                            const wxString &name = wxT("wxdataviewctrlmainwindow") );
    virtual ~wxDataViewMainWindow();

    bool IsVirtualList() const { return m_root == NULL; }

    // notifications from wxDataViewModel
    bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item );
    bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item );
    bool ItemChanged( const wxDataViewItem &item );
    bool ValueChanged( const wxDataViewItem &item, unsigned int col );
    bool Cleared();
    void Resort()
    {
        if (!IsVirtualList())
        {
            SortPrepare();
            m_root->Resort();
        }
        UpdateDisplay();
    }

    void SortPrepare()
    {
        g_model = GetOwner()->GetModel();
        wxDataViewColumn* col = GetOwner()->GetSortingColumn();
        if( !col )
        {
            if (g_model->HasDefaultCompare())
                g_column = -1;
            else
                g_column = -2;

            g_asending = true;
            return;
        }
        g_column = col->GetModelColumn();
        g_asending = col->IsSortOrderAscending();
    }

    void SetOwner( wxDataViewCtrl* owner ) { m_owner = owner; }
    wxDataViewCtrl *GetOwner() { return m_owner; }
    const wxDataViewCtrl *GetOwner() const { return m_owner; }

    void OnPaint( wxPaintEvent &event );
    void OnArrowChar(unsigned int newCurrent, const wxKeyEvent& event);
    void OnChar( wxKeyEvent &event );
    void OnMouse( wxMouseEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

    void UpdateDisplay();
    void RecalculateDisplay();
    void OnInternalIdle();

    void OnRenameTimer();

    void ScrollWindow( int dx, int dy, const wxRect *rect = NULL );
    void ScrollTo( int rows, int column );

    bool HasCurrentRow() { return m_currentRow != (unsigned int)-1; }
    void ChangeCurrentRow( unsigned int row );

    bool IsSingleSel() const { return !GetParent()->HasFlag(wxDV_MULTIPLE); }
    bool IsEmpty() { return GetRowCount() == 0; }

    int GetCountPerPage() const;
    int GetEndOfLastCol() const;
    unsigned int GetFirstVisibleRow() const;
    //I change this method to un const because in the tree view, the displaying number of the tree are changing along with the expanding/collapsing of the tree nodes
    unsigned int GetLastVisibleRow();
    unsigned int GetRowCount();

    wxDataViewItem GetSelection() const;
    wxDataViewSelection GetSelections(){ return m_selection; }
    void SetSelections( const wxDataViewSelection & sel ) { m_selection = sel; UpdateDisplay(); }
    void Select( const wxArrayInt& aSelections );
    void SelectAllRows( bool on );
    void SelectRow( unsigned int row, bool on );
    void SelectRows( unsigned int from, unsigned int to, bool on );
    void ReverseRowSelection( unsigned int row );
    bool IsRowSelected( unsigned int row );
    void SendSelectionChangedEvent( const wxDataViewItem& item);

    void RefreshRow( unsigned int row );
    void RefreshRows( unsigned int from, unsigned int to );
    void RefreshRowsAfter( unsigned int firstRow );

    // returns the colour to be used for drawing the rules
    wxColour GetRuleColour() const
    {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
    }

    wxRect GetLineRect( unsigned int row ) const;

    int GetLineStart( unsigned int row ) const;  // row * m_lineHeight in fixed mode
    int GetLineHeight( unsigned int row ) const; // m_lineHeight in fixed mode
    int GetLineAt( unsigned int y ) const;       // y / m_lineHeight in fixed mode

    //Some useful functions for row and item mapping
    wxDataViewItem GetItemByRow( unsigned int row ) const;
    int GetRowByItem( const wxDataViewItem & item ) const;

    //Methods for building the mapping tree
    void BuildTree( wxDataViewModel  * model );
    void DestroyTree();
    void HitTest( const wxPoint & point, wxDataViewItem & item, wxDataViewColumn* &column );
    wxRect GetItemRect( const wxDataViewItem & item, const wxDataViewColumn* column );

    void Expand( unsigned int row ) { OnExpanding( row ); }
    void Collapse( unsigned int row ) { OnCollapsing( row ); }
private:
    wxDataViewTreeNode * GetTreeNodeByRow( unsigned int row ) const;
    //We did not need this temporarily
    //wxDataViewTreeNode * GetTreeNodeByItem( const wxDataViewItem & item );

    int RecalculateCount();

    wxDataViewEvent SendExpanderEvent( wxEventType type, const wxDataViewItem & item );
    void OnExpanding( unsigned int row );
    void OnCollapsing( unsigned int row );

    wxDataViewTreeNode * FindNode( const wxDataViewItem & item );

private:
    wxDataViewCtrl             *m_owner;
    int                         m_lineHeight;
    bool                        m_dirty;

    wxDataViewColumn           *m_currentCol;
    unsigned int                m_currentRow;
    wxDataViewSelection         m_selection;

    wxDataViewRenameTimer      *m_renameTimer;
    bool                        m_lastOnSame;

    bool                        m_hasFocus;

    int                         m_dragCount;
    wxPoint                     m_dragStart;

    // for double click logic
    unsigned int m_lineLastClicked,
           m_lineBeforeLastClicked,
           m_lineSelectSingleOnUp;

    // the pen used to draw horiz/vertical rules
    wxPen m_penRule;

    // the pen used to draw the expander and the lines
    wxPen m_penExpander;

    //This is the tree structure of the model
    wxDataViewTreeNode * m_root;
    int m_count;
    //This is the tree node under the cursor
    wxDataViewTreeNode * m_underMouse;
private:
    DECLARE_DYNAMIC_CLASS(wxDataViewMainWindow)
    DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------
// wxGenericDataViewModelNotifier
// ---------------------------------------------------------

class wxGenericDataViewModelNotifier: public wxDataViewModelNotifier
{
public:
    wxGenericDataViewModelNotifier( wxDataViewMainWindow *mainWindow )
        { m_mainWindow = mainWindow; }

    virtual bool ItemAdded( const wxDataViewItem & parent, const wxDataViewItem & item )
        { return m_mainWindow->ItemAdded( parent , item ); }
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item )
        { return m_mainWindow->ItemDeleted( parent, item ); }
    virtual bool ItemChanged( const wxDataViewItem & item )
        { return m_mainWindow->ItemChanged(item);  }
    virtual bool ValueChanged( const wxDataViewItem & item , unsigned int col )
        { return m_mainWindow->ValueChanged( item, col ); }
    virtual bool Cleared()
        { return m_mainWindow->Cleared(); }
    virtual void Resort()
         { m_mainWindow->Resort(); }

    wxDataViewMainWindow    *m_mainWindow;
};

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewRenderer, wxDataViewRendererBase)

wxDataViewRenderer::wxDataViewRenderer( const wxString &varianttype,
                                        wxDataViewCellMode mode,
                                        int align) :
    wxDataViewRendererBase( varianttype, mode, align )
{
    m_dc = NULL;
    m_align = align;
    m_mode = mode;
    m_wantsAttr = false;
    m_hasAttr = false;
}

wxDataViewRenderer::~wxDataViewRenderer()
{
    if (m_dc)
        delete m_dc;
}

wxDC *wxDataViewRenderer::GetDC()
{
    if (m_dc == NULL)
    {
        if (GetOwner() == NULL)
            return NULL;
        if (GetOwner()->GetOwner() == NULL)
            return NULL;
        m_dc = new wxClientDC( GetOwner()->GetOwner() );
    }

    return m_dc;
}

void wxDataViewRenderer::SetAlignment( int align )
{
    m_align=align;
}

int wxDataViewRenderer::GetAlignment() const
{
    return m_align;
}

int wxDataViewRenderer::CalculateAlignment() const
{
    if (m_align == wxDVR_DEFAULT_ALIGNMENT)
    {
        if (GetOwner() == NULL)
           return wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL;

        return GetOwner()->GetAlignment() | wxALIGN_CENTRE_VERTICAL;
    }

    return m_align;
}

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer)

wxDataViewCustomRenderer::wxDataViewCustomRenderer( const wxString &varianttype,
                          wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
}

void wxDataViewCustomRenderer::RenderText( const wxString &text, int xoffset, wxRect cell, wxDC *dc, int state )
{
    wxDataViewCtrl *view = GetOwner()->GetOwner();
    wxColour col = (state & wxDATAVIEW_CELL_SELECTED) ?
                        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) :
                        view->GetForegroundColour();
    dc->SetTextForeground(col);
    dc->DrawText( text, cell.x + xoffset, cell.y + ((cell.height - dc->GetCharHeight()) / 2));
}

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewTextRenderer, wxDataViewCustomRenderer)

wxDataViewTextRenderer::wxDataViewTextRenderer( const wxString &varianttype,
                                                wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
}

bool wxDataViewTextRenderer::SetValue( const wxVariant &value )
{
    m_text = value.GetString();

    return true;
}

bool wxDataViewTextRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewTextRenderer::HasEditorCtrl()
{
    return true;
}

wxControl* wxDataViewTextRenderer::CreateEditorCtrl( wxWindow *parent,
        wxRect labelRect, const wxVariant &value )
{
    return new wxTextCtrl( parent, wxID_ANY, value,
                           wxPoint(labelRect.x,labelRect.y),
                           wxSize(labelRect.width,labelRect.height) );
}

bool wxDataViewTextRenderer::GetValueFromEditorCtrl( wxControl *editor, wxVariant &value )
{
    wxTextCtrl *text = (wxTextCtrl*) editor;
    value = text->GetValue();
    return true;
}

bool wxDataViewTextRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    RenderText( m_text, 0, cell, dc, state );
    return true;
}

wxSize wxDataViewTextRenderer::GetSize() const
{
    const wxDataViewCtrl *view = GetView();
    if (!m_text.empty())
    {
        int x,y;
        view->GetTextExtent( m_text, &x, &y );
        return wxSize( x, y );
    }
    return wxSize(80,20);
}

// ---------------------------------------------------------
// wxDataViewTextRendererAttr
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewTextRendererAttr, wxDataViewTextRenderer)

wxDataViewTextRendererAttr::wxDataViewTextRendererAttr( const wxString &varianttype,
                            wxDataViewCellMode mode, int align ) :
    wxDataViewTextRenderer( varianttype, mode, align )
{
    m_wantsAttr = true;
}

bool wxDataViewTextRendererAttr::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    wxFont font;
    wxColour colour;

    if (m_hasAttr)
    {
        if (m_attr.HasColour())
        {
            colour = dc->GetTextForeground();
            dc->SetTextForeground( m_attr.GetColour() );
        }

        if (m_attr.GetBold() || m_attr.GetItalic())
        {
             font = dc->GetFont();
             wxFont myfont = font;
             if (m_attr.GetBold())
                 myfont.SetWeight( wxFONTWEIGHT_BOLD );
             if (m_attr.GetItalic())
                 myfont.SetStyle( wxFONTSTYLE_ITALIC );
             dc->SetFont( myfont );
        }
    }

    dc->DrawText( m_text, cell.x, cell.y + ((cell.height - dc->GetCharHeight()) / 2));

    // restore dc
    if (m_hasAttr)
    {
        if (m_attr.HasColour())
            dc->SetTextForeground( colour );

        if (m_attr.GetBold() || m_attr.GetItalic())
            dc->SetFont( font );
    }

    return true;
}


// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewBitmapRenderer, wxDataViewCustomRenderer)

wxDataViewBitmapRenderer::wxDataViewBitmapRenderer( const wxString &varianttype,
                                                    wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
}

bool wxDataViewBitmapRenderer::SetValue( const wxVariant &value )
{
    if (value.GetType() == wxT("wxBitmap"))
        m_bitmap << value;
    if (value.GetType() == wxT("wxIcon"))
        m_icon << value;

    return true;
}

bool wxDataViewBitmapRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewBitmapRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    if (m_bitmap.Ok())
        dc->DrawBitmap( m_bitmap, cell.x, cell.y );
    else if (m_icon.Ok())
        dc->DrawIcon( m_icon, cell.x, cell.y );

    return true;
}

wxSize wxDataViewBitmapRenderer::GetSize() const
{
    if (m_bitmap.Ok())
        return wxSize( m_bitmap.GetWidth(), m_bitmap.GetHeight() );
    else if (m_icon.Ok())
        return wxSize( m_icon.GetWidth(), m_icon.GetHeight() );

    return wxSize(16,16);
}

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleRenderer, wxDataViewCustomRenderer)

wxDataViewToggleRenderer::wxDataViewToggleRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
    m_toggle = false;
}

bool wxDataViewToggleRenderer::SetValue( const wxVariant &value )
{
    m_toggle = value.GetBool();

    return true;
}

bool wxDataViewToggleRenderer::GetValue( wxVariant &WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewToggleRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    // User wxRenderer here

    wxRect rect;
    rect.x = cell.x + cell.width/2 - 10;
    rect.width = 20;
    rect.y = cell.y + cell.height/2 - 10;
    rect.height = 20;

    int flags = 0;
    if (m_toggle)
        flags |= wxCONTROL_CHECKED;
    if (GetMode() != wxDATAVIEW_CELL_ACTIVATABLE)
        flags |= wxCONTROL_DISABLED;

    wxRendererNative::Get().DrawCheckBox(
            GetOwner()->GetOwner(),
            *dc,
            rect,
            flags );

    return true;
}

bool wxDataViewToggleRenderer::Activate( wxRect WXUNUSED(cell),
                                         wxDataViewModel *model,
                                         const wxDataViewItem & item, unsigned int col)
{
    bool value = !m_toggle;
    wxVariant variant = value;
    model->SetValue( variant, item, col);
    model->ValueChanged( item, col );
    return true;
}

wxSize wxDataViewToggleRenderer::GetSize() const
{
    return wxSize(20,20);
}

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressRenderer, wxDataViewCustomRenderer)

wxDataViewProgressRenderer::wxDataViewProgressRenderer( const wxString &label,
    const wxString &varianttype, wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
    m_label = label;
    m_value = 0;
}

wxDataViewProgressRenderer::~wxDataViewProgressRenderer()
{
}

bool wxDataViewProgressRenderer::SetValue( const wxVariant &value )
{
    m_value = (long) value;

    if (m_value < 0) m_value = 0;
    if (m_value > 100) m_value = 100;

    return true;
}

bool wxDataViewProgressRenderer::GetValue( wxVariant &value ) const
{
    value = (long) m_value;
    return true;
}

bool wxDataViewProgressRenderer::Render( wxRect cell, wxDC *dc, int WXUNUSED(state) )
{
    double pct = (double)m_value / 100.0;
    wxRect bar = cell;
    bar.width = (int)(cell.width * pct);
    dc->SetPen( *wxTRANSPARENT_PEN );
    dc->SetBrush( *wxBLUE_BRUSH );
    dc->DrawRectangle( bar );

    dc->SetBrush( *wxTRANSPARENT_BRUSH );
    dc->SetPen( *wxBLACK_PEN );
    dc->DrawRectangle( cell );

    return true;
}

wxSize wxDataViewProgressRenderer::GetSize() const
{
    return wxSize(40,12);
}

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

#define wxUSE_DATE_RENDERER_POPUP (wxUSE_CALENDARCTRL && wxUSE_POPUPWIN)

#if wxUSE_DATE_RENDERER_POPUP

class wxDataViewDateRendererPopupTransient: public wxPopupTransientWindow
{
public:
    wxDataViewDateRendererPopupTransient( wxWindow* parent, wxDateTime *value,
        wxDataViewModel *model, const wxDataViewItem & item, unsigned int col) :
        wxPopupTransientWindow( parent, wxBORDER_SIMPLE ),
        m_item( item )
    {
        m_model = model;
        m_col = col;
        m_cal = new wxCalendarCtrl( this, wxID_ANY, *value );
        wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add( m_cal, 1, wxGROW );
        SetSizer( sizer );
        sizer->Fit( this );
    }

    void OnCalendar( wxCalendarEvent &event );

    wxCalendarCtrl      *m_cal;
    wxDataViewModel *m_model;
    unsigned int               m_col;
    const wxDataViewItem &   m_item;

protected:
    virtual void OnDismiss()
    {
    }

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxDataViewDateRendererPopupTransient,wxPopupTransientWindow)
    EVT_CALENDAR( wxID_ANY, wxDataViewDateRendererPopupTransient::OnCalendar )
END_EVENT_TABLE()

void wxDataViewDateRendererPopupTransient::OnCalendar( wxCalendarEvent &event )
{
    wxDateTime date = event.GetDate();
    wxVariant value = date;
    m_model->SetValue( value, m_item, m_col );
    m_model->ValueChanged( m_item, m_col );
    DismissAndNotify();
}

#endif // wxUSE_DATE_RENDERER_POPUP

IMPLEMENT_ABSTRACT_CLASS(wxDataViewDateRenderer, wxDataViewCustomRenderer)

wxDataViewDateRenderer::wxDataViewDateRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
}

bool wxDataViewDateRenderer::SetValue( const wxVariant &value )
{
    m_date = value.GetDateTime();

    return true;
}

bool wxDataViewDateRenderer::GetValue( wxVariant &value ) const
{
    value = m_date;
    return true;
}

bool wxDataViewDateRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    wxString tmp = m_date.FormatDate();
    RenderText( tmp, 0, cell, dc, state );
    return true;
}

wxSize wxDataViewDateRenderer::GetSize() const
{
    const wxDataViewCtrl* view = GetView();
    wxString tmp = m_date.FormatDate();
    wxCoord x,y,d;
    view->GetTextExtent( tmp, &x, &y, &d );
    return wxSize(x,y+d);
}

bool wxDataViewDateRenderer::Activate( wxRect WXUNUSED(cell), wxDataViewModel *model,
                                       const wxDataViewItem & item, unsigned int col )
{
    wxVariant variant;
    model->GetValue( variant, item, col );
    wxDateTime value = variant.GetDateTime();

#if wxUSE_DATE_RENDERER_POPUP
    wxDataViewDateRendererPopupTransient *popup = new wxDataViewDateRendererPopupTransient(
        GetOwner()->GetOwner()->GetParent(), &value, model, item, col);
    wxPoint pos = wxGetMousePosition();
    popup->Move( pos );
    popup->Layout();
    popup->Popup( popup->m_cal );
#else // !wxUSE_DATE_RENDERER_POPUP
    wxMessageBox(value.Format());
#endif // wxUSE_DATE_RENDERER_POPUP/!wxUSE_DATE_RENDERER_POPUP
    return true;
}

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewIconTextRenderer, wxDataViewCustomRenderer)

wxDataViewIconTextRenderer::wxDataViewIconTextRenderer(
  const wxString &varianttype, wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
    SetMode(mode);
    SetAlignment(align);
}

wxDataViewIconTextRenderer::~wxDataViewIconTextRenderer()
{
}

bool wxDataViewIconTextRenderer::SetValue( const wxVariant &value )
{
    m_value << value;
    return true;
}

bool wxDataViewIconTextRenderer::GetValue( wxVariant& WXUNUSED(value) ) const
{
    return false;
}

bool wxDataViewIconTextRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    int xoffset = 0;
    const wxIcon &icon = m_value.GetIcon();
    if (icon.IsOk())
    {
        dc->DrawIcon( icon, cell.x, cell.y + ((cell.height - icon.GetHeight()) / 2));
        xoffset =  icon.GetWidth()+4;
    }

    RenderText( m_value.GetText(), xoffset, cell, dc, state );

    return true;
}

wxSize wxDataViewIconTextRenderer::GetSize() const
{
    const wxDataViewCtrl *view = GetView();
    if (!m_value.GetText().empty())
    {
        int x,y;
        view->GetTextExtent( m_value.GetText(), &x, &y );

        if (m_value.GetIcon().IsOk())
            x += m_value.GetIcon().GetWidth() + 4;
        return wxSize( x, y );
    }
    return wxSize(80,20);
}

wxControl *
wxDataViewIconTextRenderer::CreateEditorCtrl(wxWindow * WXUNUSED(parent),
                                             wxRect WXUNUSED(labelRect),
                                             const wxVariant& WXUNUSED(value))
{
    return NULL;
}

bool
wxDataViewIconTextRenderer::GetValueFromEditorCtrl(wxControl* WXUNUSED(editor),
                                                   wxVariant& WXUNUSED(value))
{
    return false;
}

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumn, wxDataViewColumnBase)

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewRenderer *cell,
                                    unsigned int model_column,
                                    int width, wxAlignment align, int flags ) :
    wxDataViewColumnBase( title, cell, model_column, width, align, flags )
{
    SetAlignment(align);
    SetTitle(title);
    SetFlags(flags);

    m_autosize = width < 0;  // TODO

    Init(width < 0 ? wxDVC_DEFAULT_WIDTH : width);
}

wxDataViewColumn::wxDataViewColumn( const wxBitmap &bitmap, wxDataViewRenderer *cell,
                                    unsigned int model_column,
                                    int width, wxAlignment align, int flags ) :
    wxDataViewColumnBase( bitmap, cell, model_column, width, align, flags )
{
    SetAlignment(align);
    SetFlags(flags);

    Init(width < 0 ? wxDVC_DEFAULT_WIDTH : width);
}

wxDataViewColumn::~wxDataViewColumn()
{
}

void wxDataViewColumn::Init( int width )
{
    m_width = width;
    m_minWidth = wxDVC_DEFAULT_MINWIDTH;
    m_ascending = true;
}

void wxDataViewColumn::SetResizeable( bool resizeable )
{
    if (resizeable)
        m_flags |= wxDATAVIEW_COL_RESIZABLE;
    else
        m_flags &= ~wxDATAVIEW_COL_RESIZABLE;
}

void wxDataViewColumn::SetHidden( bool hidden )
{
    if (hidden)
        m_flags |= wxDATAVIEW_COL_HIDDEN;
    else
        m_flags &= ~wxDATAVIEW_COL_HIDDEN;

    // tell our owner to e.g. update its scrollbars:
    if (GetOwner())
        GetOwner()->OnColumnChange();
}

void wxDataViewColumn::SetSortable( bool sortable )
{
    if (sortable)
        m_flags |= wxDATAVIEW_COL_SORTABLE;
    else
        m_flags &= ~wxDATAVIEW_COL_SORTABLE;

    // Update header button
    if (GetOwner())
        GetOwner()->OnColumnChange();
}

void wxDataViewColumn::SetReorderable( bool reorderable )
{
    if (reorderable)
        m_flags |= wxDATAVIEW_COL_REORDERABLE;
    else
        m_flags &= ~wxDATAVIEW_COL_REORDERABLE;
}

void wxDataViewColumn::SetSortOrder( bool ascending )
{
    m_ascending = ascending;

    // Update header button
    if (GetOwner())
        GetOwner()->OnColumnChange();
}

bool wxDataViewColumn::IsSortOrderAscending() const
{
    return m_ascending;
}

void wxDataViewColumn::SetInternalWidth( int width )
{
    m_width = width;

    // the scrollbars of the wxDataViewCtrl needs to be recalculated!
    if (m_owner && m_owner->m_clientArea)
        m_owner->m_clientArea->RecalculateDisplay();
}

void wxDataViewColumn::SetWidth( int width )
{
    if (m_owner->m_headerArea) m_owner->m_headerArea->UpdateDisplay();

    SetInternalWidth(width);
}


//-----------------------------------------------------------------------------
// wxDataViewHeaderWindowBase
//-----------------------------------------------------------------------------

void wxDataViewHeaderWindowBase::SendEvent(wxEventType type, unsigned int n)
{
    wxWindow *parent = GetParent();
    wxDataViewEvent le(type, parent->GetId());

    le.SetEventObject(parent);
    le.SetColumn(n);
    le.SetDataViewColumn(GetColumn(n));
    le.SetModel(GetOwner()->GetModel());

    // for events created by wxDataViewHeaderWindow the
    // row / value fields are not valid

    parent->GetEventHandler()->ProcessEvent(le);
}

#ifdef USE_NATIVE_HEADER_WINDOW

#ifndef HDS_DRAGDROP
    #define HDS_DRAGDROP 0x0040
#endif
#ifndef HDS_FULLDRAG
    #define HDS_FULLDRAG 0x0080
#endif

// implemented in msw/listctrl.cpp:
int WXDLLIMPEXP_CORE wxMSWGetColumnClicked(NMHDR *nmhdr, POINT *ptClick);

IMPLEMENT_ABSTRACT_CLASS(wxDataViewHeaderWindowMSW, wxWindow)

bool wxDataViewHeaderWindowMSW::Create( wxDataViewCtrl *parent, wxWindowID id,
                                        const wxPoint &pos, const wxSize &size,
                                        const wxString &name )
{
    m_owner = parent;

    m_scrollOffsetX = 0;
    m_delayedUpdate = false;
    m_vetoColumnDrag = false;
    m_buttonHeight = wxRendererNative::Get().GetHeaderButtonHeight( this );

    int x = pos.x == wxDefaultCoord ? 0 : pos.x,
        y = pos.y == wxDefaultCoord ? 0 : pos.y,
        w = size.x == wxDefaultCoord ? 1 : size.x,
        h = m_buttonHeight;

    wxSize new_size(w,h);

    if ( !CreateControl(parent, id, pos, new_size, 0, wxDefaultValidator, name) )
        return false;

    // create the native WC_HEADER window:
    WXHWND hwndParent = (HWND)parent->GetHandle();
    WXDWORD msStyle = WS_CHILD | HDS_DRAGDROP | HDS_BUTTONS | HDS_HORZ | HDS_HOTTRACK | HDS_FULLDRAG;

    if ( m_isShown )
        msStyle |= WS_VISIBLE;

    m_hWnd = CreateWindowEx(0,
                            WC_HEADER,
                            (LPCTSTR) NULL,
                            msStyle,
                            x, y, w, h,
                            (HWND)hwndParent,
                            (HMENU)-1,
                            wxGetInstance(),
                            (LPVOID) NULL);
    if (m_hWnd == NULL)
    {
        wxLogLastError(_T("CreateWindowEx"));
        return false;
    }

    m_imageList = new wxImageList( 16, 16 );
    (void)Header_SetImageList((HWND) m_hWnd, m_imageList->GetHIMAGELIST());

    // we need to subclass the m_hWnd to force wxWindow::HandleNotify
    // to call wxDataViewHeaderWindow::MSWOnNotify
    SubclassWin(m_hWnd);

    // the following is required to get the default win's font for
    // header windows and must be done befor sending the HDM_LAYOUT msg
    SetFont(GetFont());

    return true;
}

wxDataViewHeaderWindowMSW::~wxDataViewHeaderWindow()
{
    delete m_imageList;
    UnsubclassWin();
}

wxSize wxDataViewHeaderWindowMSW::DoGetBestSize() const
{
    return wxSize( 80, m_buttonHeight+2 );
}

void wxDataViewHeaderWindowMSW::OnInternalIdle()
{
    if (m_delayedUpdate)
    {
        m_delayedUpdate = false;
        UpdateDisplay();
    }
}

void wxDataViewHeaderWindowMSW::UpdateDisplay()
{
    // remove old columns
    for (int j=0, max=Header_GetItemCount((HWND)m_hWnd); j < max; j++)
        Header_DeleteItem((HWND)m_hWnd, 0);

    m_imageList->RemoveAll();

    // add the updated array of columns to the header control
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int added = 0;
    for (unsigned int i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetColumn( i );
        if (col->IsHidden())
            continue;      // don't add it!

        wxString title( col->GetTitle() );
        HDITEM hdi;
        hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
        if (col->GetBitmap().IsOk())
        {
           m_imageList->Add( col->GetBitmap() );
           hdi.mask |= HDI_IMAGE;
           hdi.iImage = m_imageList->GetImageCount()-1;
        }
        hdi.pszText = (wxChar *) title.wx_str();
        hdi.cxy = col->GetWidth();
        hdi.cchTextMax = sizeof(hdi.pszText)/sizeof(hdi.pszText[0]);
        hdi.fmt = HDF_LEFT | HDF_STRING;
        if (col->GetBitmap().IsOk())
            hdi.fmt |= HDF_IMAGE;

        //hdi.fmt &= ~(HDF_SORTDOWN|HDF_SORTUP);

        if (col->IsSortable() && GetOwner()->GetSortingColumn() == col)
        {
            //The Microsoft Comctrl32.dll 6.0 support SORTUP/SORTDOWN, but they are not default
            //see http://msdn2.microsoft.com/en-us/library/ms649534.aspx for more detail
            // VZ: works with 5.81
            hdi.fmt |= col->IsSortOrderAscending() ? HDF_SORTUP : HDF_SORTDOWN;
        }

        // lParam is reserved for application's use:
        // we store there the column index to use it later in MSWOnNotify
        // (since columns may have been hidden)
        hdi.lParam = (LPARAM)i;

        // the native wxMSW implementation of the header window
        // draws the column separator COLUMN_WIDTH_OFFSET pixels
        // on the right: to correct this effect we make the column
        // exactly COLUMN_WIDTH_OFFSET wider (for the first column):
        if (i == 0)
            hdi.cxy += COLUMN_WIDTH_OFFSET;

        switch (col->GetAlignment())
        {
        case wxALIGN_LEFT:
            hdi.fmt |= HDF_LEFT;
            break;
        case wxALIGN_CENTER:
        case wxALIGN_CENTER_HORIZONTAL:
            hdi.fmt |= HDF_CENTER;
            break;
        case wxALIGN_RIGHT:
            hdi.fmt |= HDF_RIGHT;
            break;

        default:
            // such alignment is not allowed for the column header!
            break; // wxFAIL;
        }

        SendMessage((HWND)m_hWnd, HDM_INSERTITEM,
                    (WPARAM)added, (LPARAM)&hdi);
        added++;
    }
}

unsigned int wxDataViewHeaderWindowMSW::GetColumnIdxFromHeader(NMHEADER *nmHDR)
{
    unsigned int idx;

    // NOTE: we don't just return nmHDR->iItem because when there are
    //       hidden columns, nmHDR->iItem may be different from
    //       nmHDR->pitem->lParam

    if (nmHDR->pitem && nmHDR->pitem->mask & HDI_LPARAM)
    {
        idx = (unsigned int)nmHDR->pitem->lParam;
        return idx;
    }

    HDITEM item;
    item.mask = HDI_LPARAM;
    Header_GetItem((HWND)m_hWnd, nmHDR->iItem, &item);

    return (unsigned int)item.lParam;
}

bool wxDataViewHeaderWindowMSW::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR *nmhdr = (NMHDR *)lParam;

    // is it a message from the header?
    if ( nmhdr->hwndFrom != (HWND)m_hWnd )
        return wxWindow::MSWOnNotify(idCtrl, lParam, result);

    NMHEADER *nmHDR = (NMHEADER *)nmhdr;
    switch ( nmhdr->code )
    {
        case NM_RELEASEDCAPTURE:
            {
                // user has released the mouse
                m_vetoColumnDrag = false;
            }
            break;

        case HDN_BEGINTRACK:
            // user has started to resize a column:
            // do we need to veto it?
            if (!GetColumn(nmHDR->iItem)->IsResizeable())
            {
                // veto it!
                *result = TRUE;
            }
            break;

        case HDN_BEGINDRAG:
            // valid column
            if (nmHDR->iItem != -1)
            {
                // user has started to reorder a valid column
                if ((m_vetoColumnDrag == true) || (!GetColumn(nmHDR->iItem)->IsReorderable()))
                {
                    // veto it!
                    *result = TRUE;
                    m_vetoColumnDrag = true;
                }
            }
            else
            {
                // veto it!
                m_vetoColumnDrag = true;
            }
            break;

        case HDN_ENDDRAG:       // user has finished reordering a column
            {
               wxDataViewColumn *col = GetColumn(nmHDR->iItem);
               unsigned int new_pos = nmHDR->pitem->iOrder;
               m_owner->ColumnMoved( col, new_pos );
               m_delayedUpdate = true;
            }
            break;

        case HDN_ITEMCHANGING:
            if (nmHDR->pitem != NULL &&
                (nmHDR->pitem->mask & HDI_WIDTH) != 0)
            {
                int minWidth = GetColumnFromHeader(nmHDR)->GetMinWidth();
                if (nmHDR->pitem->cxy < minWidth)
                {
                    // do not allow the user to resize this column under
                    // its minimal width:
                    *result = TRUE;
                }
            }
            break;

        case HDN_ITEMCHANGED:   // user is resizing a column
        case HDN_ENDTRACK:      // user has finished resizing a column

            // update the width of the modified column:
            if (nmHDR->pitem != NULL &&
                (nmHDR->pitem->mask & HDI_WIDTH) != 0)
            {
                unsigned int idx = GetColumnIdxFromHeader(nmHDR);
                unsigned int w = nmHDR->pitem->cxy;
                wxDataViewColumn *col = GetColumn(idx);

                // see UpdateDisplay() for more info about COLUMN_WIDTH_OFFSET
                if (idx == 0 && w > COLUMN_WIDTH_OFFSET)
                    w -= COLUMN_WIDTH_OFFSET;

                if (w >= (unsigned)col->GetMinWidth())
                    col->SetInternalWidth(w);
            }
            break;

        case HDN_ITEMCLICK:
            {
                unsigned int idx = GetColumnIdxFromHeader(nmHDR);
                wxDataViewModel * model = GetOwner()->GetModel();

                if(nmHDR->iButton == 0)
                {
                    wxDataViewColumn *col = GetColumn(idx);
                    if(col->IsSortable())
                    {
                        if(model && m_owner->GetSortingColumn() == col)
                        {
                            bool order = col->IsSortOrderAscending();
                            col->SetSortOrder(!order);
                        }
                        else if(model)
                        {
                            m_owner->SetSortingColumn(col);
                        }
                    }
                    UpdateDisplay();
                    if(model)
                        model->Resort();
                }

                wxEventType evt = nmHDR->iButton == 0 ?
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK :
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK;
                SendEvent(evt, idx);
            }
            break;

        case NM_RCLICK:
            {
                // NOTE: for some reason (i.e. for a bug in Windows)
                //       the HDN_ITEMCLICK notification is not sent on
                //       right clicks, so we need to handle NM_RCLICK

                POINT ptClick;
                int column = wxMSWGetColumnClicked(nmhdr, &ptClick);
                if (column != wxNOT_FOUND)
                {
                    HDITEM item;
                    item.mask = HDI_LPARAM;
                    Header_GetItem((HWND)m_hWnd, column, &item);

                    // 'idx' may be different from 'column' if there are
                    // hidden columns...
                    unsigned int idx = (unsigned int)item.lParam;
                    SendEvent(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK,
                              idx);
                }
            }
            break;

        case HDN_GETDISPINFOW:
            // see wxListCtrl::MSWOnNotify for more info!
            break;

        case HDN_ITEMDBLCLICK:
            {
                unsigned int idx = GetColumnIdxFromHeader(nmHDR);
                int w = GetOwner()->GetBestColumnWidth(idx);

                // update the native control:
                HDITEM hd;
                ZeroMemory(&hd, sizeof(hd));
                hd.mask = HDI_WIDTH;
                hd.cxy = w;
                Header_SetItem(GetHwnd(),
                               nmHDR->iItem,  // NOTE: we don't want 'idx' here!
                               &hd);

                // update the wxDataViewColumn class:
                GetColumn(idx)->SetInternalWidth(w);
            }
            break;

        default:
            return wxWindow::MSWOnNotify(idCtrl, lParam, result);
    }

    return true;
}

void wxDataViewHeaderWindowMSW::ScrollWindow(int dx, int WXUNUSED(dy),
                                             const wxRect * WXUNUSED(rect))
{
    m_scrollOffsetX += dx;

    GetParent()->Layout();
}

void wxDataViewHeaderWindowMSW::DoSetSize(int x, int y,
                                          int w, int h,
                                          int f)
{
    // TODO: why is there a border + 2px around it?
    wxControl::DoSetSize( x+m_scrollOffsetX+1, y+1, w-m_scrollOffsetX-2, h-2, f );
}

#else       // !defined(__WXMSW__)

IMPLEMENT_ABSTRACT_CLASS(wxGenericDataViewHeaderWindow, wxWindow)
BEGIN_EVENT_TABLE(wxGenericDataViewHeaderWindow, wxWindow)
    EVT_PAINT         (wxGenericDataViewHeaderWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxGenericDataViewHeaderWindow::OnMouse)
    EVT_SET_FOCUS     (wxGenericDataViewHeaderWindow::OnSetFocus)
END_EVENT_TABLE()

bool wxGenericDataViewHeaderWindow::Create(wxDataViewCtrl *parent, wxWindowID id,
                                           const wxPoint &pos, const wxSize &size,
                                           const wxString &name )
{
    m_owner = parent;

    if (!wxDataViewHeaderWindowBase::Create(parent, id, pos, size, name))
        return false;

    wxVisualAttributes attr = wxPanel::GetClassDefaultAttributes();
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );
    SetOwnForegroundColour( attr.colFg );
    SetOwnBackgroundColour( attr.colBg );
    if (!m_hasFont)
        SetOwnFont( attr.font );

    // set our size hints: wxDataViewCtrl will put this wxWindow inside
    // a wxBoxSizer and in order to avoid super-big header windows,
    // we need to set our height as fixed
    SetMinSize(wxSize(-1, HEADER_WINDOW_HEIGHT));
    SetMaxSize(wxSize(-1, HEADER_WINDOW_HEIGHT));

    return true;
}

void wxGenericDataViewHeaderWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    int w, h;
    GetClientSize( &w, &h );

    wxAutoBufferedPaintDC dc( this );

    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    int xpix;
    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );

    int x;
    m_owner->GetViewStart( &x, NULL );

    // account for the horz scrollbar offset
    dc.SetDeviceOrigin( -x * xpix, 0 );

    dc.SetFont( GetFont() );

    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    int xpos = 0;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetColumn( i );
        if (col->IsHidden())
            continue;      // skip it!

        int cw = col->GetWidth();
        int ch = h;

        wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE;
        if (col->IsSortable() && GetOwner()->GetSortingColumn() == col)
        {
            if (col->IsSortOrderAscending())
                sortArrow = wxHDR_SORT_ICON_UP;
            else
                sortArrow = wxHDR_SORT_ICON_DOWN;
        }

        int state = 0;
        if (m_parent->IsEnabled())
        {
            if ((int) i == m_hover)
                state = wxCONTROL_CURRENT;
        }
        else
        {
            state = (int) wxCONTROL_DISABLED;
        }

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(xpos, 0, cw, ch-1),
                                    state,
                                    sortArrow
                                );

        // align as required the column title:
        int x = xpos;
        wxSize titleSz = dc.GetTextExtent(col->GetTitle());
        switch (col->GetAlignment())
        {
        case wxALIGN_LEFT:
            x += HEADER_HORIZ_BORDER;
            break;
        case wxALIGN_RIGHT:
            x += cw - titleSz.GetWidth() - HEADER_HORIZ_BORDER;
            break;
        default:
        case wxALIGN_CENTER:
        case wxALIGN_CENTER_HORIZONTAL:
            x += (cw - titleSz.GetWidth() - 2 * HEADER_HORIZ_BORDER)/2;
            break;
        }

        // always center the title vertically:
        int y = wxMax((ch - titleSz.GetHeight()) / 2, HEADER_VERT_BORDER);

        dc.SetClippingRegion( xpos+HEADER_HORIZ_BORDER,
                              HEADER_VERT_BORDER,
                              wxMax(cw - 2 * HEADER_HORIZ_BORDER, 1),  // width
                              wxMax(ch - 2 * HEADER_VERT_BORDER, 1));  // height
        dc.DrawText( col->GetTitle(), x, y );
        dc.DestroyClippingRegion();

        xpos += cw;
    }
}

void wxGenericDataViewHeaderWindow::OnSetFocus( wxFocusEvent &event )
{
    GetParent()->SetFocus();
    event.Skip();
}

void wxGenericDataViewHeaderWindow::OnMouse( wxMouseEvent &event )
{
    // we want to work with logical coords
    int x;
    m_owner->CalcUnscrolledPosition(event.GetX(), 0, &x, NULL);
    int y = event.GetY();

    if (m_isDragging)
    {
        // we don't draw the line beyond our window,
        // but we allow dragging it there
        int w = 0;
        GetClientSize( &w, NULL );
        m_owner->CalcUnscrolledPosition(w, 0, &w, NULL);
        w -= 6;

        if (event.ButtonUp())
        {
            m_isDragging = false;
            if (HasCapture())
                ReleaseMouse();

            m_dirty = true;
        }
        m_currentX = wxMax(m_minX + 7, x);

        if (m_currentX < w)
        {
            GetColumn(m_column)->SetWidth(m_currentX - m_minX);
            Refresh();
            GetOwner()->Refresh();
        }

    }
    else     // not dragging
    {
        m_minX = 0;
        m_column = wxNOT_FOUND;

        bool hit_border = false;

        // end of the current column
        int xpos = 0;

        // find the column where this event occured
        int countCol = m_owner->GetColumnCount();
        for (int column = 0; column < countCol; column++)
        {
            wxDataViewColumn *p = GetColumn(column);

            if (p->IsHidden())
                continue;   // skip if not shown

            xpos += p->GetWidth();
            m_column = column;
            if ((abs(x-xpos) < 3) && (y < 22))
            {
                hit_border = true;
                break;
            }

            if (x < xpos)
            {
                // inside the column
                break;
            }

            m_minX = xpos;
        }

        int old_hover = m_hover;
        m_hover = m_column;
        if (event.Leaving())
            m_hover = wxNOT_FOUND;
        if (old_hover != m_hover)
            Refresh();

        if (m_column == wxNOT_FOUND)
            return;

        bool resizeable = GetColumn(m_column)->IsResizeable();
        if (event.LeftDClick() && resizeable)
        {
            GetColumn(m_column)->SetWidth(GetOwner()->GetBestColumnWidth(m_column));
            Refresh();
        }
        else if (event.LeftDown() || event.RightUp())
        {
            if (hit_border && event.LeftDown() && resizeable)
            {
                m_isDragging = true;
                CaptureMouse();
                m_currentX = x;
            }
            else    // click on a column
            {
                wxDataViewModel * model = GetOwner()->GetModel();
                wxEventType evt = event.LeftDown() ?
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK :
                        wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK;
                SendEvent(evt, m_column);

                //Left click the header
                if(event.LeftDown())
                {
                    wxDataViewColumn *col = GetColumn(m_column);
                    if(col->IsSortable())
                    {
                        wxDataViewColumn* sortCol = m_owner->GetSortingColumn();
                        if(model && sortCol == col)
                        {
                            bool order = col->IsSortOrderAscending();
                            col->SetSortOrder(!order);
                        }
                        else if(model)
                        {
                            m_owner->SetSortingColumn(col);
                        }
                    }
                    UpdateDisplay();
                    if(model)
                        model->Resort();
                    //Send the column sorted event
                    SendEvent(wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED, m_column);
                }
            }
        }
        else if (event.Moving())
        {
            if (hit_border && resizeable)
                m_currentCursor = m_resizeCursor;
            else
                m_currentCursor = wxSTANDARD_CURSOR;

            SetCursor(*m_currentCursor);
        }
    }
}

void wxGenericDataViewHeaderWindow::AdjustDC(wxDC& dc)
{
    int xpix, x;

    m_owner->GetScrollPixelsPerUnit( &xpix, NULL );
    m_owner->GetViewStart( &x, NULL );

    // shift the DC origin to match the position of the main window horizontal
    // scrollbar: this allows us to always use logical coords
    dc.SetDeviceOrigin( -x * xpix, 0 );
}

#endif      // defined(__WXMSW__)

//-----------------------------------------------------------------------------
// wxDataViewRenameTimer
//-----------------------------------------------------------------------------

wxDataViewRenameTimer::wxDataViewRenameTimer( wxDataViewMainWindow *owner )
{
    m_owner = owner;
}

void wxDataViewRenameTimer::Notify()
{
    m_owner->OnRenameTimer();
}

//-----------------------------------------------------------------------------
// wxDataViewMainWindow
//-----------------------------------------------------------------------------

//The tree building helper, declared firstly
static void BuildTreeHelper( wxDataViewModel * model,  wxDataViewItem & item, wxDataViewTreeNode * node);

int LINKAGEMODE wxDataViewSelectionCmp( unsigned int row1, unsigned int row2 )
{
    if (row1 > row2) return 1;
    if (row1 == row2) return 0;
    return -1;
}


IMPLEMENT_ABSTRACT_CLASS(wxDataViewMainWindow, wxWindow)

BEGIN_EVENT_TABLE(wxDataViewMainWindow,wxWindow)
    EVT_PAINT         (wxDataViewMainWindow::OnPaint)
    EVT_MOUSE_EVENTS  (wxDataViewMainWindow::OnMouse)
    EVT_SET_FOCUS     (wxDataViewMainWindow::OnSetFocus)
    EVT_KILL_FOCUS    (wxDataViewMainWindow::OnKillFocus)
    EVT_CHAR          (wxDataViewMainWindow::OnChar)
END_EVENT_TABLE()

wxDataViewMainWindow::wxDataViewMainWindow( wxDataViewCtrl *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size, const wxString &name ) :
    wxWindow( parent, id, pos, size, wxWANTS_CHARS|wxBORDER_NONE, name ),
    m_selection( wxDataViewSelectionCmp )

{
    SetOwner( parent );

    m_lastOnSame = false;
    m_renameTimer = new wxDataViewRenameTimer( this );

    // TODO: user better initial values/nothing selected
    m_currentCol = NULL;
    m_currentRow = 0;

    m_lineHeight = wxMax( 17, GetCharHeight() + 2 ); // 17 = mini icon height + 1

    m_dragCount = 0;
    m_dragStart = wxPoint(0,0);
    m_lineLastClicked = (unsigned int) -1;
    m_lineBeforeLastClicked = (unsigned int) -1;
    m_lineSelectSingleOnUp = (unsigned int) -1;

    m_hasFocus = false;

    SetBackgroundColour( *wxWHITE );

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    m_penRule = wxPen(GetRuleColour());

    //Here I compose a pen can draw black lines, maybe there are something system colour to use
    m_penExpander = wxPen(wxColour(0,0,0));
    //Some new added code to deal with the tree structure
    m_root = new wxDataViewTreeNode( NULL );
    m_root->SetHasChildren(true);

    //Make m_count = -1 will cause the class recaculate the real displaying number of rows.
    m_count = -1;
    m_underMouse = NULL;
    UpdateDisplay();
}

wxDataViewMainWindow::~wxDataViewMainWindow()
{
    DestroyTree();
    delete m_renameTimer;
}

void wxDataViewMainWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxDataViewModel *model = GetOwner()->GetModel();
    wxAutoBufferedPaintDC dc( this );

#ifdef __WXMSW__
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.SetBrush( wxBrush( GetBackgroundColour()) );
    dc.SetBrush( *wxWHITE_BRUSH );
    wxSize size( GetClientSize() );
    dc.DrawRectangle( 0,0,size.x,size.y );
#endif

    // prepare the DC
    GetOwner()->PrepareDC( dc );
    dc.SetFont( GetFont() );

    wxRect update = GetUpdateRegion().GetBox();
    m_owner->CalcUnscrolledPosition( update.x, update.y, &update.x, &update.y );

    // compute which items needs to be redrawn
    unsigned int item_start = GetLineAt( wxMax(0,update.y) );
    unsigned int item_count =
        wxMin( (int)(  GetLineAt( wxMax(0,update.y+update.height) ) - item_start + 1),
               (int)(GetRowCount( ) - item_start));
    unsigned int item_last = item_start + item_count;

    // compute which columns needs to be redrawn
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int col_start = 0;
    unsigned int x_start = 0;
    for (x_start = 0; col_start < cols; col_start++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn(col_start);
        if (col->IsHidden())
            continue;      // skip it!

        unsigned int w = col->GetWidth();
        if (x_start+w >= (unsigned int)update.x)
            break;

        x_start += w;
    }

    unsigned int col_last = col_start;
    unsigned int x_last = x_start;
    for (; col_last < cols; col_last++)
    {
        wxDataViewColumn *col = GetOwner()->GetColumn(col_last);
        if (col->IsHidden())
            continue;      // skip it!

        if (x_last > (unsigned int)update.GetRight())
            break;

        x_last += col->GetWidth();
    }

    // Draw horizontal rules if required
    if ( m_owner->HasFlag(wxDV_HORIZ_RULES) )
    {
        dc.SetPen(m_penRule);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        for (unsigned int i = item_start; i <= item_last+1; i++)
        {
            int y = GetLineStart( i );
            dc.DrawLine(x_start, y, x_last, y);
        }
    }

    // Draw vertical rules if required
    if ( m_owner->HasFlag(wxDV_VERT_RULES) )
    {
        dc.SetPen(m_penRule);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        int x = x_start;
        for (unsigned int i = col_start; i < col_last; i++)
        {
            wxDataViewColumn *col = GetOwner()->GetColumn(i);
            if (col->IsHidden())
                continue;       // skip it

            dc.DrawLine(x, GetLineStart( item_start ),
                        x, GetLineStart( item_last ) );

            x += col->GetWidth();
        }

        // Draw last vertical rule
        dc.DrawLine(x, GetLineStart( item_start ),
                    x, GetLineStart( item_last ) );
    }

    // redraw the background for the items which are selected/current
    for (unsigned int item = item_start; item < item_last; item++)
    {
        bool selected = m_selection.Index( item ) != wxNOT_FOUND;
        if (selected || item == m_currentRow)
        {
            int flags = selected ? (int)wxCONTROL_SELECTED : 0;
            if (item == m_currentRow)
                flags |= wxCONTROL_CURRENT;
            if (m_hasFocus)
                flags |= wxCONTROL_FOCUSED;

            wxRect rect( x_start, GetLineStart( item ), x_last, GetLineHeight( item ) );
            wxRendererNative::Get().DrawItemSelectionRect
                                (
                                    this,
                                    dc,
                                    rect,
                                    flags
                                );
        }
    }

    wxDataViewColumn *expander = GetOwner()->GetExpanderColumn();
    if (!expander)
    {
        // TODO: last column for RTL support
        expander = GetOwner()->GetColumn( 0 );
        GetOwner()->SetExpanderColumn(expander);
    }

    // redraw all cells for all rows which must be repainted and all columns
    wxRect cell_rect;
    cell_rect.x = x_start;
    for (unsigned int i = col_start; i < col_last; i++)
    {

        wxDataViewColumn *col = GetOwner()->GetColumn( i );
        wxDataViewRenderer *cell = col->GetRenderer();
        cell_rect.width = col->GetWidth();

        if (col->IsHidden())
            continue;       // skip it!

        for (unsigned int item = item_start; item < item_last; item++)
        {
            // get the cell value and set it into the renderer
            wxVariant value;
            wxDataViewTreeNode *node = NULL;
            wxDataViewItem dataitem;

            if (!IsVirtualList())
            {
                node = GetTreeNodeByRow(item);
                if( node == NULL )
                    continue;

                dataitem = node->GetItem();

                if ((i > 0) && model->IsContainer(dataitem) && !model->HasContainerColumns(dataitem))
                    continue;
            }
            else
            {
                dataitem = wxDataViewItem( wxUIntToPtr(item) );
            }

            model->GetValue( value, dataitem, col->GetModelColumn());
            cell->SetValue( value );

            if (cell->GetWantsAttr())
            {
                wxDataViewItemAttr attr;
                bool ret = model->GetAttr( dataitem, col->GetModelColumn(), attr );
                if (ret)
                    cell->SetAttr( attr );
                cell->SetHasAttr( ret );
            }

            // update cell_rect
            cell_rect.y = GetLineStart( item );
            cell_rect.height = GetLineHeight( item );     // -1 is for the horizontal rules (?)

            //Draw the expander here.
            int indent = 0;
            if ((!IsVirtualList()) && (col == expander))
            {
                indent = node->GetIndentLevel();

                //Calculate the indent first
                indent = cell_rect.x + GetOwner()->GetIndent() * indent;

                int expander_width = m_lineHeight - 2*EXPANDER_MARGIN;
                // change the cell_rect.x to the appropriate pos
                int  expander_x = indent + EXPANDER_MARGIN;
                int expander_y = cell_rect.y + EXPANDER_MARGIN + (GetLineHeight(item) / 2) - (expander_width/2) - EXPANDER_OFFSET;
                indent = indent + m_lineHeight;  //try to use the m_lineHeight as the expander space
                dc.SetPen( m_penExpander );
                dc.SetBrush( wxNullBrush );
                if( node->HasChildren() )
                {
                    wxRect rect( expander_x , expander_y, expander_width, expander_width);
                    int flag = 0;
                    if (m_underMouse == node)
                    {
                        flag |= wxCONTROL_CURRENT;
                    }
                    if( node->IsOpen() )
                        wxRendererNative::Get().DrawTreeItemButton( this, dc, rect, flag|wxCONTROL_EXPANDED );
                    else
                        wxRendererNative::Get().DrawTreeItemButton( this, dc, rect, flag);
                }
                 //force the expander column to left-center align
                 cell->SetAlignment( wxALIGN_CENTER_VERTICAL );
            }
            if (node && !node->HasChildren())
            {
                // Yes, if the node does not have any child, it must be a leaf which
                // mean that it is a temporarily created by GetTreeNodeByRow
                wxDELETE(node);
            }

            // cannot be bigger than allocated space
            wxSize size = cell->GetSize();
            // Because of the tree structure indent, here we should minus the width of the cell for drawing
            size.x = wxMin( size.x + 2*PADDING_RIGHTLEFT, cell_rect.width - indent );
            // size.y = wxMin( size.y, cell_rect.height );
            size.y = cell_rect.height;

            wxRect item_rect(cell_rect.GetTopLeft(), size);
            int align = cell->CalculateAlignment();

            // horizontal alignment:
            item_rect.x = cell_rect.x;
            if (align & wxALIGN_CENTER_HORIZONTAL)
                item_rect.x = cell_rect.x + (cell_rect.width / 2) - (size.x / 2);
            else if (align & wxALIGN_RIGHT)
                item_rect.x = cell_rect.x + cell_rect.width - size.x;
            //else: wxALIGN_LEFT is the default

            // vertical alignment:
            item_rect.y = cell_rect.y;
            if (align & wxALIGN_CENTER_VERTICAL)
                item_rect.y = cell_rect.y + (cell_rect.height / 2) - (size.y / 2);
            else if (align & wxALIGN_BOTTOM)
                item_rect.y = cell_rect.y + cell_rect.height - size.y;
            //else: wxALIGN_TOP is the default

            // add padding
            item_rect.x += PADDING_RIGHTLEFT;
            item_rect.width = size.x - 2 * PADDING_RIGHTLEFT;

            //Here we add the tree indent
            item_rect.x += indent;

            int state = 0;
            if (m_hasFocus && (m_selection.Index(item) != wxNOT_FOUND))
                state |= wxDATAVIEW_CELL_SELECTED;

            // TODO: it would be much more efficient to create a clipping
            //       region for the entire column being rendered (in the OnPaint
            //       of wxDataViewMainWindow) instead of a single clip region for
            //       each cell. However it would mean that each renderer should
            //       respect the given wxRect's top & bottom coords, eventually
            //       violating only the left & right coords - however the user can
            //       make its own renderer and thus we cannot be sure of that.
            dc.SetClippingRegion( item_rect );
            cell->Render( item_rect, &dc, state );
            dc.DestroyClippingRegion();
        }

        cell_rect.x += cell_rect.width;
    }
}

void wxDataViewMainWindow::OnRenameTimer()
{
    // We have to call this here because changes may just have
    // been made and no screen update taken place.
    if ( m_dirty )
        wxSafeYield();

    int xpos = 0;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        if (c->IsHidden())
            continue;      // skip it!

        if (c == m_currentCol)
            break;
        xpos += c->GetWidth();
    }
    wxRect labelRect( xpos,
                      GetLineStart( m_currentRow ),
                      m_currentCol->GetWidth(),
                      GetLineHeight( m_currentRow ) );

    GetOwner()->CalcScrolledPosition( labelRect.x, labelRect.y,
                                     &labelRect.x, &labelRect.y);

    wxDataViewItem item = GetItemByRow( m_currentRow );
    m_currentCol->GetRenderer()->StartEditing( item, labelRect );

}

//------------------------------------------------------------------
// Helper class for do operation on the tree node
//------------------------------------------------------------------
class DoJob
{
public:
    DoJob() { }
    virtual ~DoJob() { }

    //The return value control how the tree-walker tranverse the tree
    // 0: Job done, stop tranverse and return
    // 1: Ignore the current node's subtree and continue
    // 2: Job not done, continue
    enum  { OK = 0 , IGR = 1, CONT = 2 };
    virtual int operator() ( wxDataViewTreeNode * node ) = 0;
    virtual int operator() ( void * n ) = 0;
};

bool Walker( wxDataViewTreeNode * node, DoJob & func )
{
    if( node==NULL )
        return false;

    switch( func( node ) )
    {
        case DoJob::OK :
            return true;
        case DoJob::IGR:
            return false;
        case DoJob::CONT:
        default:
            ;
    }

    wxDataViewTreeNodes nodes = node->GetNodes();
    wxDataViewTreeLeaves leaves = node->GetChildren();

    int len_nodes = nodes.GetCount();
    int len = leaves.GetCount();
    int i = 0, nodes_i = 0;

    for(; i < len; i ++ )
    {
        void * n = leaves[i];
        if( nodes_i < len_nodes && n == nodes[nodes_i]->GetItem().GetID() )
        {
            wxDataViewTreeNode * nd = nodes[nodes_i];
            nodes_i++;

            if( Walker( nd , func ) )
                return true;

        }
        else
            switch( func( n ) )
            {
                case DoJob::OK :
                    return true;
                case DoJob::IGR:
                    continue;
                case DoJob::CONT:
                default:
                   ;
            }
    }
    return false;
}

bool wxDataViewMainWindow::ItemAdded(const wxDataViewItem & parent, const wxDataViewItem & item)
{
    if (!m_root)
    {
        m_count++;
        UpdateDisplay();
        return true;
    }

    SortPrepare();

    wxDataViewTreeNode * node;
    node = FindNode(parent);

    if( node == NULL )
        return false;

    node->SetHasChildren( true );

    if( g_model->IsContainer( item ) )
    {
        wxDataViewTreeNode * newnode = new wxDataViewTreeNode( node );
        newnode->SetItem(item);
        newnode->SetHasChildren( true );
        node->AddNode( newnode);
    }
    else
        node->AddLeaf( item.GetID() );

    node->ChangeSubTreeCount(1);

    m_count = -1;
    UpdateDisplay();

    return true;
}

static void DestroyTreeHelper( wxDataViewTreeNode * node);

bool wxDataViewMainWindow::ItemDeleted(const wxDataViewItem& parent,
                                       const wxDataViewItem& item)
{
    if (!m_root)
    {
        m_count--;
        if( m_currentRow > GetRowCount() )
            m_currentRow = m_count - 1;

        m_selection.Empty();

        UpdateDisplay();

        return true;
    }

    wxDataViewTreeNode * node = FindNode(parent);

    wxCHECK_MSG( node != NULL, false, "item not found" );
    wxCHECK_MSG( node->GetChildren().Index( item.GetID() ) != wxNOT_FOUND, false, "item not found" );

    int sub = -1;
    node->GetChildren().Remove( item.GetID() );
    //Manuplate selection
    if( m_selection.GetCount() > 1 )
    {
        m_selection.Empty();
    }
    bool isContainer = false;
    wxDataViewTreeNodes nds = node->GetNodes();
    for (size_t i = 0; i < nds.GetCount(); i ++)
    {
        if (nds[i]->GetItem() == item)
        {
            isContainer = true;
            break;
        }
    }
    if( isContainer )
    {
        wxDataViewTreeNode * n = NULL;
        wxDataViewTreeNodes nodes = node->GetNodes();
        int len = nodes.GetCount();
        for( int i = 0; i < len; i ++)
        {
            if( nodes[i]->GetItem() == item )
            {
                n = nodes[i];
                break;
            }
        }

        wxCHECK_MSG( n != NULL, false, "item not found" );

        node->GetNodes().Remove( n );
        sub -= n->GetSubTreeCount();
        ::DestroyTreeHelper(n);
    }
    //Make the row number invalid and get a new valid one when user call GetRowCount
    m_count = -1;
    node->ChangeSubTreeCount(sub);

    //Change the current row to the last row if the current exceed the max row number
    if( m_currentRow > GetRowCount() )
        m_currentRow = m_count - 1;

    UpdateDisplay();

    return true;
}

bool wxDataViewMainWindow::ItemChanged(const wxDataViewItem & item)
{
    SortPrepare();
    g_model->Resort();

    //Send event
    wxWindow *parent = GetParent();
    wxDataViewEvent le(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED, parent->GetId());
    le.SetEventObject(parent);
    le.SetModel(GetOwner()->GetModel());
    le.SetItem(item);
    parent->GetEventHandler()->ProcessEvent(le);

    return true;
}

bool wxDataViewMainWindow::ValueChanged( const wxDataViewItem & item, unsigned int col )
{
    // NOTE: to be valid, we cannot use e.g. INT_MAX - 1
/*#define MAX_VIRTUAL_WIDTH       100000

    wxRect rect( 0, row*m_lineHeight, MAX_VIRTUAL_WIDTH, m_lineHeight );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );
    Refresh( true, &rect );

    return true;
*/
    SortPrepare();
    g_model->Resort();

    //Send event
    wxWindow *parent = GetParent();
    wxDataViewEvent le(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED, parent->GetId());
    le.SetEventObject(parent);
    le.SetModel(GetOwner()->GetModel());
    le.SetItem(item);
    le.SetColumn(col);
    le.SetDataViewColumn(GetOwner()->GetColumn(col));
    parent->GetEventHandler()->ProcessEvent(le);

    return true;
}

bool wxDataViewMainWindow::Cleared()
{
    DestroyTree();

    SortPrepare();
    BuildTree( GetOwner()->GetModel() );

    UpdateDisplay();

    return true;
}

void wxDataViewMainWindow::UpdateDisplay()
{
    m_dirty = true;
}

void wxDataViewMainWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    if (m_dirty)
    {
        RecalculateDisplay();
        m_dirty = false;
    }
}

void wxDataViewMainWindow::RecalculateDisplay()
{
    wxDataViewModel *model = GetOwner()->GetModel();
    if (!model)
    {
        Refresh();
        return;
    }

    int width = GetEndOfLastCol();
    int height = GetLineStart( GetRowCount() );

    SetVirtualSize( width, height );
    GetOwner()->SetScrollRate( 10, m_lineHeight );

    Refresh();
}

void wxDataViewMainWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxWindow::ScrollWindow( dx, dy, rect );

    if (GetOwner()->m_headerArea)
        GetOwner()->m_headerArea->ScrollWindow( dx, 0 );
}

void wxDataViewMainWindow::ScrollTo( int rows, int column )
{
    int x, y;
    m_owner->GetScrollPixelsPerUnit( &x, &y );
    int sy = GetLineStart( rows )/y;
    int sx = 0;
    if( column != -1 )
    {
        wxRect rect = GetClientRect();
        int colnum = 0;
        int x_start = 0, x_end = 0, w = 0;
        int xx, yy, xe;
        m_owner->CalcUnscrolledPosition( rect.x, rect.y, &xx, &yy );
        for (x_start = 0; colnum < column; colnum++)
        {
            wxDataViewColumn *col = GetOwner()->GetColumn(colnum);
            if (col->IsHidden())
                continue;      // skip it!

            w = col->GetWidth();
            x_start += w;
        }

        x_end = x_start + w;
        xe = xx + rect.width;
        if( x_end > xe )
        {
            sx = ( xx + x_end - xe )/x;
        }
        if( x_start < xx )
        {
            sx = x_start/x;
        }
    }
    m_owner->Scroll( sx, sy );
}

int wxDataViewMainWindow::GetCountPerPage() const
{
    wxSize size = GetClientSize();
    return size.y / m_lineHeight;
}

int wxDataViewMainWindow::GetEndOfLastCol() const
{
    int width = 0;
    unsigned int i;
    for (i = 0; i < GetOwner()->GetColumnCount(); i++)
    {
        const wxDataViewColumn *c =
            wx_const_cast(wxDataViewCtrl*, GetOwner())->GetColumn( i );

        if (!c->IsHidden())
            width += c->GetWidth();
    }
    return width;
}

unsigned int wxDataViewMainWindow::GetFirstVisibleRow() const
{
    int x = 0;
    int y = 0;
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );

    return GetLineAt( y );
}

unsigned int wxDataViewMainWindow::GetLastVisibleRow()
{
    wxSize client_size = GetClientSize();
    m_owner->CalcUnscrolledPosition( client_size.x, client_size.y,
                                     &client_size.x, &client_size.y );

    //we should deal with the pixel here
    unsigned int row = GetLineAt(client_size.y) - 1;

    return wxMin( GetRowCount()-1, row );
}

unsigned int wxDataViewMainWindow::GetRowCount()
{
    if ( m_count == -1 )
    {
        m_count = RecalculateCount();
        UpdateDisplay();
    }
    return m_count;
}

void wxDataViewMainWindow::ChangeCurrentRow( unsigned int row )
{
    m_currentRow = row;

    // send event
}

void wxDataViewMainWindow::SelectAllRows( bool on )
{
    if (IsEmpty())
        return;

    if (on)
    {
        m_selection.Clear();
        for (unsigned int i = 0; i < GetRowCount(); i++)
            m_selection.Add( i );
        Refresh();
    }
    else
    {
        unsigned int first_visible = GetFirstVisibleRow();
        unsigned int last_visible = GetLastVisibleRow();
        unsigned int i;
        for (i = 0; i < m_selection.GetCount(); i++)
        {
            unsigned int row = m_selection[i];
            if ((row >= first_visible) && (row <= last_visible))
                RefreshRow( row );
        }
        m_selection.Clear();
    }
}

void wxDataViewMainWindow::SelectRow( unsigned int row, bool on )
{
    if (m_selection.Index( row ) == wxNOT_FOUND)
    {
        if (on)
        {
            m_selection.Add( row );
            RefreshRow( row );
        }
    }
    else
    {
        if (!on)
        {
            m_selection.Remove( row );
            RefreshRow( row );
        }
    }
}

void wxDataViewMainWindow::SelectRows( unsigned int from, unsigned int to, bool on )
{
    if (from > to)
    {
        unsigned int tmp = from;
        from = to;
        to = tmp;
    }

    unsigned int i;
    for (i = from; i <= to; i++)
    {
        if (m_selection.Index( i ) == wxNOT_FOUND)
        {
            if (on)
                m_selection.Add( i );
        }
        else
        {
            if (!on)
                m_selection.Remove( i );
        }
    }
    RefreshRows( from, to );
}

void wxDataViewMainWindow::Select( const wxArrayInt& aSelections )
{
    for (size_t i=0; i < aSelections.GetCount(); i++)
    {
        int n = aSelections[i];

        m_selection.Add( n );
        RefreshRow( n );
    }
}

void wxDataViewMainWindow::ReverseRowSelection( unsigned int row )
{
    if (m_selection.Index( row ) == wxNOT_FOUND)
        m_selection.Add( row );
    else
        m_selection.Remove( row );
    RefreshRow( row );
}

bool wxDataViewMainWindow::IsRowSelected( unsigned int row )
{
    return (m_selection.Index( row ) != wxNOT_FOUND);
}

void wxDataViewMainWindow::SendSelectionChangedEvent( const wxDataViewItem& item)
{
    wxWindow *parent = GetParent();
    wxDataViewEvent le(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, parent->GetId());

    le.SetEventObject(parent);
    le.SetModel(GetOwner()->GetModel());
    le.SetItem( item );

    parent->GetEventHandler()->ProcessEvent(le);
}

void wxDataViewMainWindow::RefreshRow( unsigned int row )
{
    wxRect rect( 0, GetLineStart( row ), GetEndOfLastCol(), GetLineHeight( row ) );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::RefreshRows( unsigned int from, unsigned int to )
{
    if (from > to)
    {
        unsigned int tmp = to;
        to = from;
        from = tmp;
    }

    wxRect rect( 0, GetLineStart( from ), GetEndOfLastCol(), GetLineStart( (to-from+1) ) );
    m_owner->CalcScrolledPosition( rect.x, rect.y, &rect.x, &rect.y );

    wxSize client_size = GetClientSize();
    wxRect client_rect( 0, 0, client_size.x, client_size.y );
    wxRect intersect_rect = client_rect.Intersect( rect );
    if (intersect_rect.width > 0)
        Refresh( true, &intersect_rect );
}

void wxDataViewMainWindow::RefreshRowsAfter( unsigned int firstRow )
{
    wxSize client_size = GetClientSize();
    int start = GetLineStart( firstRow );
    m_owner->CalcScrolledPosition( start, 0, &start, NULL );
    if (start > client_size.y) return;

    wxRect rect( 0, start, client_size.x, client_size.y - start );

    Refresh( true, &rect );
}

void wxDataViewMainWindow::OnArrowChar(unsigned int newCurrent, const wxKeyEvent& event)
{
    wxCHECK_RET( newCurrent < GetRowCount(),
                 _T("invalid item index in OnArrowChar()") );

    // if there is no selection, we cannot move it anywhere
    if (!HasCurrentRow())
        return;

    unsigned int oldCurrent = m_currentRow;

    // in single selection we just ignore Shift as we can't select several
    // items anyhow
    if ( event.ShiftDown() && !IsSingleSel() )
    {
        RefreshRow( oldCurrent );

        ChangeCurrentRow( newCurrent );

        // select all the items between the old and the new one
        if ( oldCurrent > newCurrent )
        {
            newCurrent = oldCurrent;
            oldCurrent = m_currentRow;
        }

        SelectRows( oldCurrent, newCurrent, true );
        if (oldCurrent!=newCurrent)
            SendSelectionChangedEvent(GetItemByRow(m_selection[0]));
    }
    else // !shift
    {
        RefreshRow( oldCurrent );

        // all previously selected items are unselected unless ctrl is held
        if ( !event.ControlDown() )
            SelectAllRows(false);

        ChangeCurrentRow( newCurrent );

        if ( !event.ControlDown() )
        {
            SelectRow( m_currentRow, true );
            SendSelectionChangedEvent(GetItemByRow(m_currentRow));
        }
        else
            RefreshRow( m_currentRow );
    }

    GetOwner()->EnsureVisible( m_currentRow, -1 );
}

wxRect wxDataViewMainWindow::GetLineRect( unsigned int row ) const
{
    wxRect rect;
    rect.x = 0;
    rect.y = GetLineStart( row );
    rect.width = GetEndOfLastCol();
    rect.height = GetLineHeight( row );

    return rect;
}

int wxDataViewMainWindow::GetLineStart( unsigned int row ) const
{
    const wxDataViewModel *model = GetOwner()->GetModel();

    if (GetOwner()->GetWindowStyle() & wxDV_VARIABLE_LINE_HEIGHT)
    {
        // TODO make more efficient

        int start = 0;

        unsigned int r;
        for (r = 0; r < row; r++)
        {
            const wxDataViewTreeNode* node = GetTreeNodeByRow(r);
            if (!node) return start;

            wxDataViewItem item = node->GetItem();

            if (node && !node->HasChildren())
            {
                // Yes, if the node does not have any child, it must be a leaf which
                // mean that it is a temporarily created by GetTreeNodeByRow
                wxDELETE(node);
            }

            unsigned int cols = GetOwner()->GetColumnCount();
            unsigned int col;
            int height = m_lineHeight;
            for (col = 0; col < cols; col++)
            {
                const wxDataViewColumn *column = GetOwner()->GetColumn(col);
                if (column->IsHidden())
                    continue;      // skip it!

                if ((col != 0) && model->IsContainer(item) && !model->HasContainerColumns(item))
                    continue;      // skip it!

                const wxDataViewRenderer *renderer = column->GetRenderer();
                wxVariant value;
                model->GetValue( value, item, column->GetModelColumn() );
                wxDataViewRenderer *renderer2 = const_cast<wxDataViewRenderer*>(renderer);
                renderer2->SetValue( value );
                height = wxMax( height, renderer->GetSize().y );
            }


            start += height;
        }

        return start;
    }
    else
    {
        return row * m_lineHeight;
    }
}

int wxDataViewMainWindow::GetLineAt( unsigned int y ) const
{
    const wxDataViewModel *model = GetOwner()->GetModel();

    // check for the easy case first
    if ( !GetOwner()->HasFlag(wxDV_VARIABLE_LINE_HEIGHT) )
        return y / m_lineHeight;

    // TODO make more efficient
    unsigned int row = 0;
    unsigned int yy = 0;
    for (;;)
    {
       const wxDataViewTreeNode* node = GetTreeNodeByRow(row);
       if (!node)
       {
           // not really correct...
           return row + ((y-yy) / m_lineHeight);
       }

       wxDataViewItem item = node->GetItem();

        if (node && !node->HasChildren())
        {
            // Yes, if the node does not have any child, it must be a leaf which
            // mean that it is a temporarily created by GetTreeNodeByRow
            wxDELETE(node);
        }

       unsigned int cols = GetOwner()->GetColumnCount();
       unsigned int col;
       int height = m_lineHeight;
       for (col = 0; col < cols; col++)
       {
            const wxDataViewColumn *column = GetOwner()->GetColumn(col);
            if (column->IsHidden())
                continue;      // skip it!

            if ((col != 0) && model->IsContainer(item) && !model->HasContainerColumns(item))
                continue;      // skip it!

            const wxDataViewRenderer *renderer = column->GetRenderer();
            wxVariant value;
            model->GetValue( value, item, column->GetModelColumn() );
            wxDataViewRenderer *renderer2 = const_cast<wxDataViewRenderer*>(renderer);
            renderer2->SetValue( value );
            height = wxMax( height, renderer->GetSize().y );
       }

       yy += height;
       if (y < yy)
           return row;

       row++;
    }
}

int wxDataViewMainWindow::GetLineHeight( unsigned int row ) const
{
    const wxDataViewModel *model = GetOwner()->GetModel();

    if (GetOwner()->GetWindowStyle() & wxDV_VARIABLE_LINE_HEIGHT)
    {
        wxASSERT( !IsVirtualList() );

        const wxDataViewTreeNode* node = GetTreeNodeByRow(row);
        // wxASSERT( node );
        if (!node) return m_lineHeight;

        wxDataViewItem item = node->GetItem();

        if (node && !node->HasChildren())
        {
                // Yes, if the node does not have any child, it must be a leaf which
                // mean that it is a temporarily created by GetTreeNodeByRow
             wxDELETE(node);
        }

        int height = m_lineHeight;

        unsigned int cols = GetOwner()->GetColumnCount();
        unsigned int col;
        for (col = 0; col < cols; col++)
        {
            const wxDataViewColumn *column = GetOwner()->GetColumn(col);
            if (column->IsHidden())
                continue;      // skip it!

            if ((col != 0) && model->IsContainer(item) && !model->HasContainerColumns(item))
                continue;      // skip it!

            const wxDataViewRenderer *renderer = column->GetRenderer();
            wxVariant value;
            model->GetValue( value, item, column->GetModelColumn() );
            wxDataViewRenderer *renderer2 = const_cast<wxDataViewRenderer*>(renderer);
            renderer2->SetValue( value );
            height = wxMax( height, renderer->GetSize().y );
        }

        return height;
    }
    else
    {
        return m_lineHeight;
    }
}

class RowToItemJob: public DoJob
{
public:
    RowToItemJob( unsigned int row , int current ) { this->row = row; this->current = current;}
    virtual ~RowToItemJob() { }

    virtual int operator() ( wxDataViewTreeNode * node )
    {
        current ++;
        if( current == static_cast<int>(row))
         {
            ret = node->GetItem();
            return DoJob::OK;
        }

        if( node->GetSubTreeCount() + current < static_cast<int>(row) )
        {
            current += node->GetSubTreeCount();
            return  DoJob::IGR;
        }
        else
        {
            //If the current has no child node, we can find the desired item of the row number directly.
            //This if can speed up finding in some case, and will has a very good effect when it comes to list view
            if( node->GetNodes().GetCount() == 0)
            {
                int index = static_cast<int>(row) - current - 1;
                ret = node->GetChildren().Item( index );
                return DoJob::OK;
            }
            return DoJob::CONT;
        }
    }

    virtual int operator() ( void * n )
    {
        current ++;
        if( current == static_cast<int>(row))
         {
            ret = wxDataViewItem( n );
            return DoJob::OK;
        }
        return DoJob::CONT;
    }
    wxDataViewItem GetResult(){ return ret; }
private:
    unsigned int row;
    int current;
    wxDataViewItem ret;
};

wxDataViewItem wxDataViewMainWindow::GetItemByRow(unsigned int row) const
{
    if (!m_root)
    {
        return wxDataViewItem( wxUIntToPtr(row) );
    }
    else
    {
        RowToItemJob job( row, -2 );
        Walker( m_root , job );
        return job.GetResult();
    }
}

class RowToTreeNodeJob: public DoJob
{
public:
    RowToTreeNodeJob( unsigned int row , int current, wxDataViewTreeNode * node )
    {
        this->row = row;
        this->current = current;
        ret = NULL;
        parent = node;
    }
    virtual ~RowToTreeNodeJob(){ }

    virtual int operator() ( wxDataViewTreeNode * node )
    {
        current ++;
        if( current == static_cast<int>(row))
         {
            ret = node;
            return DoJob::OK;
        }

        if( node->GetSubTreeCount() + current < static_cast<int>(row) )
        {
            current += node->GetSubTreeCount();
            return  DoJob::IGR;
        }
        else
        {
            parent = node;
            //If the current has no child node, we can find the desired item of the row number directly.
            //This if can speed up finding in some case, and will has a very good effect when it comes to list view
            if( node->GetNodes().GetCount() == 0)
            {
                int index = static_cast<int>(row) - current - 1;
                void * n = node->GetChildren().Item( index );
                ret = new wxDataViewTreeNode( parent );
                ret->SetItem( wxDataViewItem( n ));
                ret->SetHasChildren(false);
                return DoJob::OK;
            }
            return DoJob::CONT;
        }


    }

    virtual int operator() ( void * n )
    {
        current ++;
        if( current == static_cast<int>(row))
         {
            ret = new wxDataViewTreeNode( parent );
            ret->SetItem( wxDataViewItem( n ));
            ret->SetHasChildren(false);
            return DoJob::OK;
        }

        return DoJob::CONT;
    }
    wxDataViewTreeNode * GetResult(){ return ret; }
private:
    unsigned int row;
    int current;
    wxDataViewTreeNode * ret;
    wxDataViewTreeNode * parent;
};


wxDataViewTreeNode * wxDataViewMainWindow::GetTreeNodeByRow(unsigned int row) const
{
    wxASSERT( !IsVirtualList() );

    RowToTreeNodeJob job( row , -2, m_root );
    Walker( m_root , job );
    return job.GetResult();
}

wxDataViewEvent wxDataViewMainWindow::SendExpanderEvent( wxEventType type, const wxDataViewItem & item )
{
    wxWindow *parent = GetParent();
    wxDataViewEvent le(type, parent->GetId());

    le.SetEventObject(parent);
    le.SetModel(GetOwner()->GetModel());
    le.SetItem( item );

    parent->GetEventHandler()->ProcessEvent(le);
    return le;
}

void wxDataViewMainWindow::OnExpanding( unsigned int row )
{
    if (IsVirtualList())
       return;

    wxDataViewTreeNode * node = GetTreeNodeByRow(row);
    if( node != NULL )
    {
        if( node->HasChildren())
        {
            if( !node->IsOpen())
            {
               wxDataViewEvent e = SendExpanderEvent(wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING,node->GetItem());
               //Check if the user prevent expanding
               if( e.GetSkipped() )
                    return;

               node->ToggleOpen();
               //Here I build the children of current node
               if( node->GetChildrenNumber() == 0 )
               {
                   SortPrepare();
                   ::BuildTreeHelper(GetOwner()->GetModel(), node->GetItem(), node);
               }
               m_count = -1;
               UpdateDisplay();
               //Send the expanded event
               SendExpanderEvent(wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED,node->GetItem());
            }
            else
            {
                SelectRow( row, false );
                SelectRow( row + 1, true );
                ChangeCurrentRow( row + 1 );
                SendSelectionChangedEvent( GetItemByRow(row+1));
            }
        }
        else
            delete node;
    }
}

void wxDataViewMainWindow::OnCollapsing(unsigned int row)
{
    if (IsVirtualList())
       return;
       
    wxDataViewTreeNode * node = GetTreeNodeByRow(row);
    if( node != NULL )
    {
        wxDataViewTreeNode * nd = node;

        if( node->HasChildren() && node->IsOpen() )
        {
            wxDataViewEvent e = SendExpanderEvent(wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSING,node->GetItem());
            if( e.GetSkipped() )
                return;
            node->ToggleOpen();
            m_count = -1;
            UpdateDisplay();
            SendExpanderEvent(wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED,nd->GetItem());
        }
        else
        {
            node = node->GetParent();
            if( node != NULL )
            {
                int  parent = GetRowByItem( node->GetItem() );
                if( parent >= 0 )
                {
                    SelectRow( row, false);
                    SelectRow(parent , true );
                    ChangeCurrentRow( parent );
                    SendSelectionChangedEvent( node->GetItem() );
                }
            }
        }
        if( !nd->HasChildren())
            delete nd;
    }
}

wxDataViewTreeNode * wxDataViewMainWindow::FindNode( const wxDataViewItem & item )
{
    wxDataViewModel * model = GetOwner()->GetModel();
    if( model == NULL )
        return NULL;

    //Compose the a parent-chain of the finding item
    ItemList list;
    list.DeleteContents( true );
    wxDataViewItem it( item );
    while( it.IsOk() )
    {
        wxDataViewItem * pItem = new wxDataViewItem( it );
        list.Insert( pItem );
        it = model->GetParent( it );
    }

    //Find the item along the parent-chain.
    //This algorithm is designed to speed up the node-finding method
    wxDataViewTreeNode * node = m_root;
    for( ItemList::const_iterator iter = list.begin(); iter !=list.end(); iter++ )
    {
        if( node->HasChildren() )
        {
            if( node->GetChildrenNumber() == 0 )
            {
                SortPrepare();
                ::BuildTreeHelper(model, node->GetItem(), node);
            }

            wxDataViewTreeNodes nodes = node->GetNodes();
            unsigned int i;
            bool found = false;

            for (i = 0; i < nodes.GetCount(); i ++)
            {
                if (nodes[i]->GetItem() == (**iter))
                {
                    if (nodes[i]->GetItem() == item)
                       return nodes[i];

                    node = nodes[i];
                    found = true;
                    break;
                }
            }
            if (!found)
                return NULL;
        }
        else
            return NULL;
    }
    return NULL;
}

void wxDataViewMainWindow::HitTest( const wxPoint & point, wxDataViewItem & item, wxDataViewColumn* &column )
{
    wxDataViewColumn *col = NULL;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int colnum = 0;
    unsigned int x_start = 0;
    int x, y;
    m_owner->CalcUnscrolledPosition( point.x, point.y, &x, &y );
    for (x_start = 0; colnum < cols; colnum++)
    {
        col = GetOwner()->GetColumn(colnum);
        if (col->IsHidden())
            continue;      // skip it!

        unsigned int w = col->GetWidth();
        if (x_start+w >= (unsigned int)x)
            break;

        x_start += w;
    }

    column = col;
    item = GetItemByRow( GetLineAt( y ) );
}

wxRect wxDataViewMainWindow::GetItemRect( const wxDataViewItem & item, const wxDataViewColumn* column )
{
    int row = GetRowByItem(item);
    int y = GetLineStart( row );
    int h = GetLineHeight( m_lineHeight );
    int x = 0;
    wxDataViewColumn *col = NULL;
    for( int i = 0, cols = GetOwner()->GetColumnCount(); i < cols; i ++ )
    {
       col = GetOwner()->GetColumn( i );
       x += col->GetWidth();
       if( GetOwner()->GetColumn(i+1) == column )
           break;
    }
    int w = col->GetWidth();
    m_owner->CalcScrolledPosition( x, y, &x, &y );
    return wxRect(x, y, w, h);
}

int wxDataViewMainWindow::RecalculateCount()
{
    if (!m_root)
    {
        wxDataViewIndexListModel *list_model = (wxDataViewIndexListModel*) GetOwner()->GetModel();
#ifndef __WXMAC__
        return list_model->GetLastIndex() + 1;
#else
        return list_model->GetLastIndex() - 1;
#endif
    }
    else
    {
        return m_root->GetSubTreeCount();
    }
}

class ItemToRowJob : public DoJob
{
public:
    ItemToRowJob(const wxDataViewItem& item_, ItemList::const_iterator iter)
        : m_iter(iter),
          item(item_)
    {
        ret = -1;
    }

    //Maybe binary search will help to speed up this process
    virtual int operator() ( wxDataViewTreeNode * node)
    {
         ret ++;
         if( node->GetItem() == item )
         {
             return DoJob::OK;
         }

         if( node->GetItem() == **m_iter )
         {
             m_iter++;
             return DoJob::CONT;
         }
         else
         {
             ret += node->GetSubTreeCount();
             return DoJob::IGR;
         }

    }

    virtual int operator() ( void * n )
    {
        ret ++;
        if( n == item.GetID() )
            return DoJob::OK;
        return DoJob::CONT;
    }
    //the row number is begin from zero
    int GetResult() { return ret -1; }

private:
    ItemList::const_iterator  m_iter;
    wxDataViewItem item;
    int ret;

};

int wxDataViewMainWindow::GetRowByItem(const wxDataViewItem & item) const
{
    const wxDataViewModel * model = GetOwner()->GetModel();
    if( model == NULL )
        return -1;

    if (!m_root)
    {
        return wxPtrToUInt( item.GetID() );
    }
    else
    {
        if( !item.IsOk() )
            return -1;

        //Compose the a parent-chain of the finding item
        ItemList list;
        wxDataViewItem * pItem = NULL;
        list.DeleteContents( true );
        wxDataViewItem it( item );
        while( it.IsOk() )
        {
            pItem = new wxDataViewItem( it );
            list.Insert( pItem );
            it = model->GetParent( it );
        }
        pItem = new wxDataViewItem( );
        list.Insert( pItem );

        ItemToRowJob job( item, list.begin() );
        Walker(m_root , job );
        return job.GetResult();
    }
}

static void BuildTreeHelper( wxDataViewModel * model,  wxDataViewItem & item, wxDataViewTreeNode * node)
{
    if( !model->IsContainer( item ) )
        return;

    wxDataViewItemArray children;
    unsigned int num = model->GetChildren( item, children);
    unsigned int index = 0;
    while( index < num )
    {
        if( model->IsContainer( children[index] ) )
        {
            wxDataViewTreeNode * n = new wxDataViewTreeNode( node );
            n->SetItem(children[index]);
            n->SetHasChildren( true );
            node->AddNode( n );
        }
        else
        {
            node->AddLeaf( children[index].GetID() );
        }
        index ++;
    }
    node->SetSubTreeCount( num );
    wxDataViewTreeNode * n = node->GetParent();
    if( n != NULL)
        n->ChangeSubTreeCount(num);

}

void wxDataViewMainWindow::BuildTree(wxDataViewModel * model)
{
    DestroyTree();

    if (GetOwner()->GetModel()->IsVirtualListModel())
    {
        m_count = -1;
        return;
    }

    m_root = new wxDataViewTreeNode( NULL );
    m_root->SetHasChildren(true);

    //First we define a invalid item to fetch the top-level elements
    wxDataViewItem item;
    SortPrepare();
    BuildTreeHelper( model, item, m_root);
    m_count = -1;
}

static void DestroyTreeHelper( wxDataViewTreeNode * node )
{
    if( node->GetNodeNumber() != 0 )
    {
        int len = node->GetNodeNumber();
        int i = 0;
        wxDataViewTreeNodes& nodes = node->GetNodes();
        for(; i < len; i ++ )
        {
            DestroyTreeHelper(nodes[i]);
        }
    }
    delete node;
}

void wxDataViewMainWindow::DestroyTree()
{
    if (!IsVirtualList())
    {
       ::DestroyTreeHelper(m_root);
        m_count = 0;
        m_root = NULL;
    }
}

void wxDataViewMainWindow::OnChar( wxKeyEvent &event )
{
    if ( HandleAsNavigationKey(event) )
        return;

    // no item -> nothing to do
    if (!HasCurrentRow())
    {
        event.Skip();
        return;
    }

    // don't use m_linesPerPage directly as it might not be computed yet
    const int pageSize = GetCountPerPage();
    wxCHECK_RET( pageSize, _T("should have non zero page size") );

    switch ( event.GetKeyCode() )
    {
        case WXK_RETURN:
        {
            if (m_currentRow > 0)
            {
                wxWindow *parent = GetParent();
                wxDataViewEvent le(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, parent->GetId());
                le.SetItem( GetItemByRow(m_currentRow) );
                le.SetEventObject(parent);
                le.SetModel(GetOwner()->GetModel());

                parent->GetEventHandler()->ProcessEvent(le);
            }
            break;
        }
        case WXK_UP:
            if ( m_currentRow > 0 )
                OnArrowChar( m_currentRow - 1, event );
            break;

        case WXK_DOWN:
            if ( m_currentRow < GetRowCount() - 1 )
                OnArrowChar( m_currentRow + 1, event );
            break;
        //Add the process for tree expanding/collapsing
        case WXK_LEFT:
         OnCollapsing(m_currentRow);
         break;
     case WXK_RIGHT:
         OnExpanding( m_currentRow);
         break;
        case WXK_END:
            if (!IsEmpty())
                OnArrowChar( GetRowCount() - 1, event );
            break;

        case WXK_HOME:
            if (!IsEmpty())
                OnArrowChar( 0, event );
            break;

        case WXK_PAGEUP:
            {
                int steps = pageSize - 1;
                int index = m_currentRow - steps;
                if (index < 0)
                    index = 0;

                OnArrowChar( index, event );
            }
            break;

        case WXK_PAGEDOWN:
            {
                int steps = pageSize - 1;
                unsigned int index = m_currentRow + steps;
                unsigned int count = GetRowCount();
                if ( index >= count )
                    index = count - 1;

                OnArrowChar( index, event );
            }
            break;

        default:
            event.Skip();
    }
}

void wxDataViewMainWindow::OnMouse( wxMouseEvent &event )
{
    if (event.GetEventType() == wxEVT_MOUSEWHEEL)
    {
        // let the base handle mouse wheel events.
        event.Skip();
        return;
    }

    int x = event.GetX();
    int y = event.GetY();
    m_owner->CalcUnscrolledPosition( x, y, &x, &y );
    wxDataViewColumn *col = NULL;

    int xpos = 0;
    unsigned int cols = GetOwner()->GetColumnCount();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *c = GetOwner()->GetColumn( i );
        if (c->IsHidden())
            continue;      // skip it!

        if (x < xpos + c->GetWidth())
        {
            col = c;
            break;
        }
        xpos += c->GetWidth();
    }
    if (!col)
        return;

    wxDataViewRenderer *cell = col->GetRenderer();
    unsigned int current = GetLineAt( y );
    if ((current > GetRowCount()) || (x > GetEndOfLastCol()))
    {
        // Unselect all if below the last row ?
        return;
    }

    //Test whether the mouse is hovered on the tree item button
    bool hover = false;
    if ((!IsVirtualList()) && (GetOwner()->GetExpanderColumn() == col))
    {
        wxDataViewTreeNode * node = GetTreeNodeByRow(current);
        if( node!=NULL && node->HasChildren() )
        {
            int indent = node->GetIndentLevel();
            indent = GetOwner()->GetIndent()*indent;
            wxRect rect( xpos + indent + EXPANDER_MARGIN,
                         GetLineStart( current ) + EXPANDER_MARGIN + (GetLineHeight(current)/2) - (m_lineHeight/2) - EXPANDER_OFFSET,
                         m_lineHeight-2*EXPANDER_MARGIN,
                         m_lineHeight-2*EXPANDER_MARGIN + EXPANDER_OFFSET);
            if( rect.Contains( x, y) )
            {
                //So the mouse is over the expander
                hover = true;
                if (m_underMouse && m_underMouse != node)
                {
                    //wxLogMessage("Undo the row: %d", GetRowByItem(m_underMouse->GetItem()));
                    RefreshRow(GetRowByItem(m_underMouse->GetItem()));
                }
                if (m_underMouse != node)
                {
                    //wxLogMessage("Do the row: %d", current);
                    RefreshRow(current);
                }
                m_underMouse = node;
            }
        }
        if (node!=NULL && !node->HasChildren())
            delete node;
    }
    if (!hover)
    {
        if (m_underMouse != NULL)
        {
            //wxLogMessage("Undo the row: %d", GetRowByItem(m_underMouse->GetItem()));
            RefreshRow(GetRowByItem(m_underMouse->GetItem()));
            m_underMouse = NULL;
        }
    }

    wxDataViewModel *model = GetOwner()->GetModel();

    if (event.Dragging())
    {
        if (m_dragCount == 0)
        {
            // we have to report the raw, physical coords as we want to be
            // able to call HitTest(event.m_pointDrag) from the user code to
            // get the item being dragged
            m_dragStart = event.GetPosition();
        }

        m_dragCount++;

        if (m_dragCount != 3)
            return;

        if (event.LeftIsDown())
        {
            // Notify cell about drag
        }
        return;
    }
    else
    {
        m_dragCount = 0;
    }

    bool forceClick = false;

    if (event.ButtonDClick())
    {
        m_renameTimer->Stop();
        m_lastOnSame = false;
    }

    wxDataViewItem item = GetItemByRow(current);
    bool ignore_other_columns =
        ((GetOwner()->GetExpanderColumn() != col) &&
         (model->IsContainer(item)) &&
         (!model->HasContainerColumns(item)));

    if (event.LeftDClick())
    {
        if ( current == m_lineLastClicked )
        {
            if ((!ignore_other_columns) && (cell->GetMode() == wxDATAVIEW_CELL_ACTIVATABLE))
            {
                wxVariant value;
                model->GetValue( value, item, col->GetModelColumn() );
                cell->SetValue( value );
                wxRect cell_rect( xpos, GetLineStart( current ),
                                  col->GetWidth(), GetLineHeight( current ) );
                cell->Activate( cell_rect, model, item, col->GetModelColumn() );

            }
            else
            {
                wxWindow *parent = GetParent();
                wxDataViewEvent le(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, parent->GetId());
                le.SetItem( item );
                le.SetEventObject(parent);
                le.SetModel(GetOwner()->GetModel());

                parent->GetEventHandler()->ProcessEvent(le);
            }
            return;
        }
        else
        {
            // The first click was on another item, so don't interpret this as
            // a double click, but as a simple click instead
            forceClick = true;
        }
    }

    if (event.LeftUp())
    {
        if (m_lineSelectSingleOnUp != (unsigned int)-1)
        {
            // select single line
            SelectAllRows( false );
            SelectRow( m_lineSelectSingleOnUp, true );
        }

        //Process the event of user clicking the expander
        bool expander = false;
        if ((!IsVirtualList()) && (GetOwner()->GetExpanderColumn() == col))
        {
            wxDataViewTreeNode * node = GetTreeNodeByRow(current);
            if( node!=NULL && node->HasChildren() )
            {
                int indent = node->GetIndentLevel();
                indent = GetOwner()->GetIndent()*indent;
                wxRect rect( xpos + indent + EXPANDER_MARGIN,
                         GetLineStart( current ) + EXPANDER_MARGIN + (GetLineHeight(current)/2) - (m_lineHeight/2) - EXPANDER_OFFSET,
                         m_lineHeight-2*EXPANDER_MARGIN,
                         m_lineHeight-2*EXPANDER_MARGIN + EXPANDER_OFFSET);

                if( rect.Contains( x, y) )
                {
                    expander = true;
                    if( node->IsOpen() )
                        OnCollapsing(current);
                    else
                        OnExpanding( current );
                }
            }
            if (node && !node->HasChildren())
               delete node;
        }
        //If the user click the expander, we do not do editing even if the column with expander are editable
        if (m_lastOnSame && !expander && !ignore_other_columns)
        {
            if ((col == m_currentCol) && (current == m_currentRow) &&
                (cell->GetMode() & wxDATAVIEW_CELL_EDITABLE) )
            {
                m_renameTimer->Start( 100, true );
            }
        }

        m_lastOnSame = false;
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }
    else
    {
        // This is necessary, because after a DnD operation in
        // from and to ourself, the up event is swallowed by the
        // DnD code. So on next non-up event (which means here and
        // now) m_lineSelectSingleOnUp should be reset.
        m_lineSelectSingleOnUp = (unsigned int)-1;
    }

    if (event.RightDown())
    {
        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        // If the item is already selected, do not update the selection.
        // Multi-selections should not be cleared if a selected item is clicked.
        if (!IsRowSelected(current))
        {
            SelectAllRows(false);
            ChangeCurrentRow(current);
            SelectRow(m_currentRow,true);
            SendSelectionChangedEvent(GetItemByRow( m_currentRow ) );
        }

        wxVariant value;
        model->GetValue( value, item, col->GetModelColumn() );
        wxWindow *parent = GetParent();
        wxDataViewEvent le(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, parent->GetId());
        le.SetItem( item );
        le.SetEventObject(parent);
        le.SetModel(GetOwner()->GetModel());
        le.SetValue(value);
        parent->GetEventHandler()->ProcessEvent(le);
    }
    else if (event.MiddleDown())
    {
    }
    if (event.LeftDown() || forceClick)
    {
        SetFocus();

        m_lineBeforeLastClicked = m_lineLastClicked;
        m_lineLastClicked = current;

        unsigned int oldCurrentRow = m_currentRow;
        bool oldWasSelected = IsRowSelected(m_currentRow);

        bool cmdModifierDown = event.CmdDown();
        if ( IsSingleSel() || !(cmdModifierDown || event.ShiftDown()) )
        {
            if ( IsSingleSel() || !IsRowSelected(current) )
            {
                SelectAllRows( false );
                ChangeCurrentRow(current);
                SelectRow(m_currentRow,true);
                SendSelectionChangedEvent(GetItemByRow( m_currentRow ) );
            }
            else // multi sel & current is highlighted & no mod keys
            {
                m_lineSelectSingleOnUp = current;
                ChangeCurrentRow(current); // change focus
            }
        }
        else // multi sel & either ctrl or shift is down
        {
            if (cmdModifierDown)
            {
                ChangeCurrentRow(current);
                ReverseRowSelection(m_currentRow);
                SendSelectionChangedEvent(GetItemByRow(m_selection[0]) );
            }
            else if (event.ShiftDown())
            {
                ChangeCurrentRow(current);

                unsigned int lineFrom = oldCurrentRow,
                       lineTo = current;

                if ( lineTo < lineFrom )
                {
                    lineTo = lineFrom;
                    lineFrom = m_currentRow;
                }

                SelectRows(lineFrom, lineTo, true);
                SendSelectionChangedEvent(GetItemByRow(m_selection[0]) );
            }
            else // !ctrl, !shift
            {
                // test in the enclosing if should make it impossible
                wxFAIL_MSG( _T("how did we get here?") );
            }
        }

        if (m_currentRow != oldCurrentRow)
            RefreshRow( oldCurrentRow );

        wxDataViewColumn *oldCurrentCol = m_currentCol;

        // Update selection here...
        m_currentCol = col;

        m_lastOnSame = !forceClick && ((col == oldCurrentCol) &&
                        (current == oldCurrentRow)) && oldWasSelected;

        // Call LeftClick after everything else as under GTK+
        if (cell->GetMode() & wxDATAVIEW_CELL_ACTIVATABLE)
        {
            // notify cell about right click
            wxVariant value;
            model->GetValue( value, item, col->GetModelColumn() );
            cell->SetValue( value );
            wxRect cell_rect( xpos, GetLineStart( current ),
                          col->GetWidth(), GetLineHeight( current ) );
            /* ignore ret */ cell->LeftClick( event.GetPosition(), cell_rect, model, item, col->GetModelColumn());
        }
    }
}

void wxDataViewMainWindow::OnSetFocus( wxFocusEvent &event )
{
    m_hasFocus = true;

    if (HasCurrentRow())
        Refresh();

    event.Skip();
}

void wxDataViewMainWindow::OnKillFocus( wxFocusEvent &event )
{
    m_hasFocus = false;

    if (HasCurrentRow())
        Refresh();

    event.Skip();
}

wxDataViewItem wxDataViewMainWindow::GetSelection() const
{
    if( m_selection.GetCount() != 1 )
        return wxDataViewItem();

    return GetItemByRow( m_selection.Item(0));
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------
WX_DEFINE_LIST(wxDataViewColumnList)

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase)

BEGIN_EVENT_TABLE(wxDataViewCtrl, wxDataViewCtrlBase)
    EVT_SIZE(wxDataViewCtrl::OnSize)
END_EVENT_TABLE()

wxDataViewCtrl::~wxDataViewCtrl()
{
    if (m_notifier)
        GetModel()->RemoveNotifier( m_notifier );

    m_cols.Clear();
}

void wxDataViewCtrl::Init()
{
    m_cols.DeleteContents(true);
    m_notifier = NULL;
    
    // No sorting column at start
    m_sortingColumn = NULL;
    m_headerArea = NULL;
}

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxValidator& validator )
{
    if ( (style & wxBORDER_MASK) == 0)
        style |= wxBORDER_SUNKEN;

    Init();

    if (!wxControl::Create( parent, id, pos, size,
                            style | wxScrolledWindowStyle, validator))
        return false;

    SetInitialSize(size);

#ifdef __WXMAC__
    MacSetClipChildren( true );
#endif

    m_clientArea = new wxDataViewMainWindow( this, wxID_ANY );

    if (HasFlag(wxDV_NO_HEADER))
        m_headerArea = NULL;
    else
        m_headerArea = new wxDataViewHeaderWindow( this, wxID_ANY );

    SetTargetWindow( m_clientArea );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    if (m_headerArea)
        sizer->Add( m_headerArea, 0, wxGROW );
    sizer->Add( m_clientArea, 1, wxGROW );
    SetSizer( sizer );

    return true;
}

#ifdef __WXMSW__
WXLRESULT wxDataViewCtrl::MSWWindowProc(WXUINT nMsg,
                                       WXWPARAM wParam,
                                       WXLPARAM lParam)
{
    WXLRESULT rc = wxDataViewCtrlBase::MSWWindowProc(nMsg, wParam, lParam);

#ifndef __WXWINCE__
    // we need to process arrows ourselves for scrolling
    if ( nMsg == WM_GETDLGCODE )
    {
        rc |= DLGC_WANTARROWS;
    }
#endif

    return rc;
}
#endif

wxSize wxDataViewCtrl::GetSizeAvailableForScrollTarget(const wxSize& size)
{
    wxSize newsize = size;
    if (!HasFlag(wxDV_NO_HEADER) && (m_headerArea))
       newsize.y -= m_headerArea->GetSize().y;
    
    return newsize;
}

void wxDataViewCtrl::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    // We need to override OnSize so that our scrolled
    // window a) does call Layout() to use sizers for
    // positioning the controls but b) does not query
    // the sizer for their size and use that for setting
    // the scrollable area as set that ourselves by
    // calling SetScrollbar() further down.

    Layout();

    AdjustScrollbars();
}

bool wxDataViewCtrl::AssociateModel( wxDataViewModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    m_notifier = new wxGenericDataViewModelNotifier( m_clientArea );

    model->AddNotifier( m_notifier );

    m_clientArea->DestroyTree();

    m_clientArea->BuildTree(model);

    m_clientArea->UpdateDisplay();

    return true;
}

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;

    m_cols.Append( col );
    OnColumnChange();
    return true;
}

bool wxDataViewCtrl::PrependColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::PrependColumn(col))
        return false;

    m_cols.Insert( col );
    OnColumnChange();
    return true;
}

bool wxDataViewCtrl::InsertColumn( unsigned int pos, wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::InsertColumn(pos,col))
        return false;

    m_cols.Insert( pos, col );
    OnColumnChange();
    return true;
}

void wxDataViewCtrl::OnColumnChange()
{
    if (m_headerArea)
        m_headerArea->UpdateDisplay();

    m_clientArea->UpdateDisplay();
}

void wxDataViewCtrl::DoSetExpanderColumn()
{
    m_clientArea->UpdateDisplay();
}

void wxDataViewCtrl::DoSetIndent()
{
    m_clientArea->UpdateDisplay();
}

unsigned int wxDataViewCtrl::GetColumnCount() const
{
    return m_cols.GetCount();
}

wxDataViewColumn* wxDataViewCtrl::GetColumn( unsigned int pos ) const
{
    wxDataViewColumnList::const_iterator iter;
    unsigned int i = 0;
    for (iter = m_cols.begin(); iter!=m_cols.end(); iter++)
    {
        if (i == pos)
            return *iter;

        if ((*iter)->IsHidden())
            continue;
        i ++;
    }
    return NULL;
}

void wxDataViewCtrl::ColumnMoved( wxDataViewColumn* col, unsigned int new_pos )
{
    if (new_pos > m_cols.GetCount()) return;

    // Exchange position
    m_cols.DeleteContents(false);
    m_cols.DeleteObject( col );
    m_cols.Insert( new_pos, col );
    m_cols.DeleteContents(true);

    m_clientArea->UpdateDisplay();
}

bool wxDataViewCtrl::DeleteColumn( wxDataViewColumn *column )
{
    wxDataViewColumnList::compatibility_iterator ret = m_cols.Find( column );
    if (!ret)
        return false;

    m_cols.Erase(ret);
    OnColumnChange();

    return true;
}

bool wxDataViewCtrl::ClearColumns()
{
    m_cols.Clear();
    OnColumnChange();
    return true;
}

int wxDataViewCtrl::GetColumnPosition( const wxDataViewColumn *column ) const
{
    int ret = 0, dead = 0;
    int len = GetColumnCount();
    for (int i=0; i<len; i++)
    {
        wxDataViewColumn * col = GetColumn(i);
        if (col->IsHidden())
            continue;
        ret += col->GetWidth();
        if (column==col)
        {
            CalcScrolledPosition( ret, dead, &ret, &dead );
            break;
        }
    }
    return ret;
}

wxDataViewColumn *wxDataViewCtrl::GetSortingColumn() const
{
    return NULL;
}

//Selection code with wxDataViewItem as parameters
wxDataViewItem wxDataViewCtrl::GetSelection() const
{
    return m_clientArea->GetSelection();
}

int wxDataViewCtrl::GetSelections( wxDataViewItemArray & sel ) const
{
    sel.Empty();
    wxDataViewSelection selection = m_clientArea->GetSelections();
    int len = selection.GetCount();
    for( int i = 0; i < len; i ++)
    {
        unsigned int row = selection[i];
        sel.Add( m_clientArea->GetItemByRow( row ) );
    }
    return len;
}

void wxDataViewCtrl::SetSelections( const wxDataViewItemArray & sel )
{
    wxDataViewSelection selection(wxDataViewSelectionCmp);
    int len = sel.GetCount();
    for( int i = 0; i < len; i ++ )
    {
        int row = m_clientArea->GetRowByItem( sel[i] );
        if( row >= 0 )
            selection.Add( static_cast<unsigned int>(row) );
    }
    m_clientArea->SetSelections( selection );
}

void wxDataViewCtrl::Select( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if( row >= 0 )
    {
        //Unselect all rows before select another in the single select mode
        if (m_clientArea->IsSingleSel())
            m_clientArea->SelectAllRows(false);
        m_clientArea->SelectRow(row, true);
    }
}

void wxDataViewCtrl::Unselect( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if( row >= 0 )
        m_clientArea->SelectRow(row, false);
}

bool wxDataViewCtrl::IsSelected( const wxDataViewItem & item ) const
{
    int row = m_clientArea->GetRowByItem( item );
    if( row >= 0 )
    {
        return m_clientArea->IsRowSelected(row);
    }
    return false;
}

//Selection code with row number as parameter
int wxDataViewCtrl::GetSelections( wxArrayInt & sel ) const
{
    sel.Empty();
    wxDataViewSelection selection = m_clientArea->GetSelections();
    int len = selection.GetCount();
    for( int i = 0; i < len; i ++)
    {
        unsigned int row = selection[i];
        sel.Add( row );
    }
    return len;
}

void wxDataViewCtrl::SetSelections( const wxArrayInt & sel )
{
    wxDataViewSelection selection(wxDataViewSelectionCmp);
    int len = sel.GetCount();
    for( int i = 0; i < len; i ++ )
    {
        int row = sel[i];
        if( row >= 0 )
            selection.Add( static_cast<unsigned int>(row) );
    }
    m_clientArea->SetSelections( selection );
}

void wxDataViewCtrl::Select( int row )
{
    if( row >= 0 )
    {
        if (m_clientArea->IsSingleSel())
            m_clientArea->SelectAllRows(false);
        m_clientArea->SelectRow( row, true );
    }
}

void wxDataViewCtrl::Unselect( int row )
{
    if( row >= 0 )
        m_clientArea->SelectRow(row, false);
}

bool wxDataViewCtrl::IsSelected( int row ) const
{
    if( row >= 0 )
        return m_clientArea->IsRowSelected(row);
    return false;
}

void wxDataViewCtrl::SelectRange( int from, int to )
{
    wxArrayInt sel;
    for( int i = from; i < to; i ++ )
        sel.Add( i );
    m_clientArea->Select(sel);
}

void wxDataViewCtrl::UnselectRange( int from, int to )
{
    wxDataViewSelection sel = m_clientArea->GetSelections();
    for( int i = from; i < to; i ++ )
        if( sel.Index( i ) != wxNOT_FOUND )
            sel.Remove( i );
    m_clientArea->SetSelections(sel);
}

void wxDataViewCtrl::SelectAll()
{
    m_clientArea->SelectAllRows(true);
}

void wxDataViewCtrl::UnselectAll()
{
    m_clientArea->SelectAllRows(false);
}

void wxDataViewCtrl::EnsureVisible( int row, int column )
{
    if( row < 0 )
        row = 0;
    if( row > (int) m_clientArea->GetRowCount() )
        row = m_clientArea->GetRowCount();

    int first = m_clientArea->GetFirstVisibleRow();
    int last = m_clientArea->GetLastVisibleRow();
    if( row < first )
        m_clientArea->ScrollTo( row, column );
    else if( row > last )
        m_clientArea->ScrollTo( row - last + first, column );
    else
        m_clientArea->ScrollTo( first, column );
}

void wxDataViewCtrl::EnsureVisible( const wxDataViewItem & item, const wxDataViewColumn * column )
{
    int row = m_clientArea->GetRowByItem(item);
    if( row >= 0 )
    {
        if( column == NULL )
            EnsureVisible(row, -1);
        else
        {
            int col = 0;
            int len = GetColumnCount();
            for( int i = 0; i < len; i ++ )
                if( GetColumn(i) == column )
                {
                    col = i;
                    break;
                }
            EnsureVisible( row, col );
        }
    }

}

void wxDataViewCtrl::HitTest( const wxPoint & point, wxDataViewItem & item, wxDataViewColumn* &column ) const
{
    m_clientArea->HitTest(point, item, column);
}

wxRect wxDataViewCtrl::GetItemRect( const wxDataViewItem & item, const wxDataViewColumn* column ) const
{
    return m_clientArea->GetItemRect(item, column);
}

wxDataViewItem wxDataViewCtrl::GetItemByRow( unsigned int row ) const
{
    return m_clientArea->GetItemByRow( row );
}

int wxDataViewCtrl::GetRowByItem( const wxDataViewItem & item ) const
{
    return m_clientArea->GetRowByItem( item );
}

void wxDataViewCtrl::Expand( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if (row != -1)
        m_clientArea->Expand(row);
}

void wxDataViewCtrl::Collapse( const wxDataViewItem & item )
{
    int row = m_clientArea->GetRowByItem( item );
    if (row != -1)
        m_clientArea->Collapse(row);
}

 #endif
    // !wxUSE_GENERICDATAVIEWCTRL

#endif
    // wxUSE_DATAVIEWCTRL
