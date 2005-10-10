///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/treebkg.cpp
// Purpose:     generic implementation of wxTreebook
// Author:      Evgeniy Tarassov, Vadim Zeitlin
// Modified by:
// Created:     2005-09-15
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

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

#if wxUSE_TREEBOOK

#include "wx/treebook.h"
#include "wx/imaglist.h"
#include "wx/settings.h"

// ----------------------------------------------------------------------------
// various wxWidgets macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < DoInternalGetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxTreebookEvent, wxNotifyEvent)

const wxEventType wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING = wxNewEventType();
const wxEventType wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED = wxNewEventType();
const wxEventType wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED = wxNewEventType();
const wxEventType wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED = wxNewEventType();
const int wxID_TREEBOOKTREEVIEW = wxNewId();

BEGIN_EVENT_TABLE(wxTreebook, wxBookCtrlBase)
    EVT_SIZE(wxTreebook::OnSize)
    EVT_TREE_SEL_CHANGED   (wxID_TREEBOOKTREEVIEW, wxTreebook::OnTreeSelectionChange)
    EVT_TREE_ITEM_EXPANDED (wxID_TREEBOOKTREEVIEW, wxTreebook::OnTreeNodeExpandedCollapsed)
    EVT_TREE_ITEM_COLLAPSED(wxID_TREEBOOKTREEVIEW, wxTreebook::OnTreeNodeExpandedCollapsed)
END_EVENT_TABLE()

// ============================================================================
// wxTreebook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxTreebook creation
// ----------------------------------------------------------------------------

void wxTreebook::Init()
{
    m_tree = NULL;
    m_selection =
    m_actualSelection = wxNOT_FOUND;
}

bool
wxTreebook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    // Check the style flag to have either wxTBK_RIGHT or wxTBK_LEFT
    if ( style & wxTBK_RIGHT )
    {
        wxASSERT_MSG( !(style & wxTBK_LEFT),
                            _T("RIGHT and LEFT can't be used together") );
    }
    else
    {
        style |= wxTBK_LEFT;
    }

    // no border for this control, it doesn't look nice together with the tree
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size,
                            style, wxDefaultValidator, name) )
        return false;

    m_tree = new wxTreeCtrl
                 (
                    this,
                    wxID_TREEBOOKTREEVIEW,
                    wxDefaultPosition,
                    wxDefaultSize,
                    wxBORDER_SIMPLE |
                    wxTR_DEFAULT_STYLE |
                    wxTR_HIDE_ROOT |
                    wxTR_SINGLE
                 );
    m_tree->AddRoot(wxEmptyString); // label doesn't matter, it's hidden

#ifdef __WXMSW__
    // see listbook.h for origins of that
    // On XP with themes enabled the GetViewRect used in GetListSize to
    // determine the space needed for the list view will incorrectly return
    // (0,0,0,0) the first time.  So send a pending event so OnSize will be
    // called again after the window is ready to go.  Technically we don't
    // need to do this on non-XP windows, but if things are already sized
    // correctly then nothing changes and so there is no harm.
    wxSizeEvent evt;
    GetEventHandler()->AddPendingEvent(evt);
#endif

    return true;
}


// insert a new page just before the pagePos
bool wxTreebook::InsertPage(size_t pagePos,
                            wxWindow *page,
                            const wxString& text,
                            bool bSelect,
                            int imageId)
{
    return DoInsertPage(pagePos, page, text, bSelect, imageId);
}

bool wxTreebook::AddSubPage(size_t pagePos,
                            wxWindow *page,
                            const wxString& text,
                            bool bSelect,
                            int imageId)
{
    return DoInsertSubPage(pagePos, page, text, bSelect, imageId);
}

bool wxTreebook::AddPage(wxWindow *page, const wxString& text, bool bSelect,
                         int imageId)
{
    return DoInsertPage(m_treeIds.GetCount(), page, text, bSelect, imageId);
}

// insertion time is linear to the number of top-pages
bool wxTreebook::AddSubPage(wxWindow *page, const wxString& text, bool bSelect, int imageId)
{
    return DoAddSubPage(page, text, bSelect, imageId);
}


bool wxTreebook::DoInsertPage(size_t pagePos,
                              wxWindow *page,
                              const wxString& text,
                              bool bSelect,
                              int imageId)
{
    wxCHECK_MSG( pagePos <= DoInternalGetPageCount(), false,
                        wxT("Invalid treebook page position") );

    if ( !wxBookCtrlBase::InsertPage(pagePos, page, text, bSelect, imageId) )
        return false;

    wxTreeItemId newId;
    if ( pagePos == DoInternalGetPageCount() )
    {
        // append the page to the end
        wxTreeItemId rootId = m_tree->GetRootItem();

        newId = m_tree->AppendItem(rootId, text, imageId);
    }
    else // insert the new page before the given one
    {
        wxTreeItemId nodeId = m_treeIds[pagePos];

        wxTreeItemId previousId = m_tree->GetPrevSibling(nodeId);
        wxTreeItemId parentId = m_tree->GetItemParent(nodeId);

        if ( previousId.IsOk() )
        {
            // insert before the sibling - previousId
            newId = m_tree->InsertItem(parentId, previousId, text, imageId);
        }
        else // no prev siblings -- insert as a first child
        {
            wxASSERT_MSG( parentId.IsOk(), wxT( "Tree has no root node?" ) );

            newId = m_tree->PrependItem(parentId, text, imageId);
        }
    }

    if ( !newId.IsOk() )
    {
        //something wrong -> cleaning and returning with false
        (void)wxBookCtrlBase::DoRemovePage(pagePos);

        wxFAIL_MSG( wxT("Failed to insert treebook page") );
        return false;
    }

    DoInternalAddPage(pagePos, page, newId);

    DoUpdateSelection(bSelect, pagePos);

    m_tree->InvalidateBestSize();

    return true;
}

bool wxTreebook::DoAddSubPage(wxWindow *page, const wxString& text, bool bSelect, int imageId)
{
    wxTreeItemId rootId = m_tree->GetRootItem();

    wxTreeItemId lastNodeId = m_tree->GetLastChild(rootId);

    wxCHECK_MSG( lastNodeId.IsOk(), false,
                        _T("Can't insert sub page when there are no pages") );

    // now calculate its position (should we save/update it too?)
    size_t newPos = m_tree->GetCount() -
                        (m_tree->GetChildrenCount(lastNodeId, true) + 1);

    return DoInsertSubPage(newPos, page, text, bSelect, imageId);
}

bool wxTreebook::DoInsertSubPage(size_t pagePos,
                                 wxTreebookPage *page,
                                 const wxString& text,
                                 bool bSelect,
                                 int imageId)
{
    wxTreeItemId parentId = DoInternalGetPage(pagePos);
    wxCHECK_MSG( parentId.IsOk(), false, wxT("invalid tree item") );

    size_t newPos = pagePos + m_tree->GetChildrenCount(parentId, true) + 1;
    wxASSERT_MSG( newPos <= DoInternalGetPageCount(),
                    wxT("Internal error in tree insert point calculation") );

    if ( !wxBookCtrlBase::InsertPage(newPos, page, text, bSelect, imageId) )
        return false;

    wxTreeItemId newId = m_tree->AppendItem(parentId, text, imageId);

    if ( !newId.IsOk() )
    {
        (void)wxBookCtrlBase::DoRemovePage(newPos);

        wxFAIL_MSG( wxT("Failed to insert treebook page") );
        return false;
    }

    DoInternalAddPage(newPos, page, newId);

    DoUpdateSelection(bSelect, newPos);

    m_tree->InvalidateBestSize();

    return true;
}

bool wxTreebook::DeletePage(size_t pagePos)
{
    wxCHECK_MSG( IS_VALID_PAGE(pagePos), false, wxT("Invalid tree index") );

    wxTreebookPage *oldPage = DoRemovePage(pagePos);
    if ( !oldPage )
        return false;

    delete oldPage;

    return true;
}

wxTreebookPage *wxTreebook::DoRemovePage(size_t pagePos)
{
    wxTreeItemId pageId = DoInternalGetPage(pagePos);
    wxCHECK_MSG( pageId.IsOk(), NULL, wxT("Invalid tree index") );

    wxTreebookPage * oldPage = GetPage(pagePos);

    size_t subCount = m_tree->GetChildrenCount(pageId, true);
    wxASSERT_MSG ( IS_VALID_PAGE(pagePos + subCount),
                        wxT("Internal error in wxTreebook::DoRemovePage") );

    // here we are going to delete ALL the pages in the range
    // [pagePos, pagePos + subCount] -- the page and its children

    // deleting all the pages from the base class
    for ( size_t i = 0; i <= subCount; ++i )
    {
        wxTreebookPage *page = wxBookCtrlBase::DoRemovePage(pagePos);

        // don't delete the page itself though -- it will be deleted in
        // DeletePage() when we return
        if ( i )
        {
            delete page;
        }
    }

    DoInternalRemovePageRange(pagePos, subCount);

    m_tree->DeleteChildren( pageId );
    m_tree->Delete( pageId );
    m_tree->InvalidateBestSize();

    return oldPage;
}

bool wxTreebook::DeleteAllPages()
{
    wxBookCtrlBase::DeleteAllPages();
    m_treeIds.Clear();
    m_selection =
    m_actualSelection = wxNOT_FOUND;

    m_tree->DeleteChildren(m_tree->GetRootItem());

    return true;
}

void wxTreebook::DoInternalAddPage(size_t newPos,
                                   wxTreebookPage *page,
                                   wxTreeItemId pageId)
{
    wxASSERT_MSG( newPos <= m_treeIds.GetCount(), wxT("Ivalid index passed to wxTreebook::DoInternalAddPage") );

    // hide newly inserted page initially (it will be shown when selected)
    if ( page )
        page->Hide();

    if ( newPos == m_treeIds.GetCount() )
    {
        // append
        m_treeIds.Add(pageId);
    }
    else // insert
    {
        m_treeIds.Insert(pageId, newPos);

        if ( m_selection != wxNOT_FOUND && newPos <= (size_t)m_selection )
        {
            // selection has been moved one unit toward the end
            ++m_selection;
            if ( m_actualSelection != wxNOT_FOUND )
                ++m_actualSelection;
        }
        else if ( m_actualSelection != wxNOT_FOUND &&
                    newPos <= (size_t)m_actualSelection )
        {
            DoSetSelection(m_selection);
        }
    }
}

void wxTreebook::DoInternalRemovePageRange(size_t pagePos, size_t subCount)
{
    // Attention: this function is only for a situation when we delete a node
    // with all its children so pagePos is the node's index and subCount is the
    // node children count
    wxASSERT_MSG( pagePos + subCount < m_treeIds.GetCount(),
                    wxT("Ivalid page index") );

    wxTreeItemId pageId = m_treeIds[pagePos];

    m_treeIds.RemoveAt(pagePos, subCount + 1);

    if ( m_selection != wxNOT_FOUND )
    {
        if ( (size_t)m_selection > pagePos + subCount)
        {
            // selection is far after the deleted page, so just update the index and move on
            m_selection -= 1 + subCount;
            if ( m_actualSelection != wxNOT_FOUND)
            {
                m_actualSelection -= subCount + 1;
            }
        }
        else if ( (size_t)m_selection >= pagePos )
        {
            // as selected page is going to be deleted, try to select the next
            // sibling if exists, if not then the parent
            wxTreeItemId nodeId = m_tree->GetNextSibling(pageId);

            m_selection = wxNOT_FOUND;
            m_actualSelection = wxNOT_FOUND;

            if ( nodeId.IsOk() )
            {
                // selecting next siblings
                m_tree->SelectItem(nodeId);
            }
            else // no next sibling, select the parent
            {
                wxTreeItemId parentId = m_tree->GetItemParent(pageId);

                if ( parentId.IsOk() && parentId != m_tree->GetRootItem() )
                {
                    m_tree->SelectItem(parentId);
                }
                else // parent is root
                {
                    // we can't select it as it's hidden
                    DoUpdateSelection(false, wxNOT_FOUND);
                }
            }
        }
        else if ( m_actualSelection != wxNOT_FOUND &&
                    (size_t)m_actualSelection >= pagePos )
        {
            // nothing to do -- selection is before the deleted node, but
            // actually shown page (the first (sub)child with page != NULL) is
            // already deleted
            m_actualSelection = m_selection;
            DoSetSelection(m_selection);
        }
        //else: nothing to do -- selection is before the deleted node
    }
    else
    {
        DoUpdateSelection(false, wxNOT_FOUND);
    }
}


void wxTreebook::DoUpdateSelection(bool bSelect, int newPos)
{
    int newSelPos;
    if ( bSelect )
    {
        newSelPos = newPos;
    }
    else if ( m_selection == wxNOT_FOUND && DoInternalGetPageCount() > 0 )
    {
        newSelPos = 0;
    }
    else
    {
        newSelPos = wxNOT_FOUND;
    }

    if ( newSelPos != wxNOT_FOUND )
    {
        SetSelection((size_t)newSelPos);
    }
}

wxTreeItemId wxTreebook::DoInternalGetPage(size_t pagePos) const
{
    if ( pagePos >= m_treeIds.GetCount() )
    {
        // invalid position but ok here, in this internal function, don't assert
        // (the caller will do it)
        return wxTreeItemId();
    }

    return m_treeIds[pagePos];
}

int wxTreebook::DoInternalFindPageById(wxTreeItemId pageId) const
{
    const size_t count = m_treeIds.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        if ( m_treeIds[i] == pageId )
            return i;
    }

    return wxNOT_FOUND;
}

bool wxTreebook::IsNodeExpanded(size_t pagePos) const
{
    wxTreeItemId pageId = DoInternalGetPage(pagePos);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    return m_tree->IsExpanded(pageId);
}

bool wxTreebook::ExpandNode(size_t pagePos, bool expand)
{
    wxTreeItemId pageId = DoInternalGetPage(pagePos);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    if ( expand )
    {
        m_tree->Expand( pageId );
    }
    else // collapse
    {
        m_tree->Collapse( pageId );

        // rely on the events generated by wxTreeCtrl to update selection
    }

    return true;
}

int wxTreebook::GetPageParent(size_t pagePos) const
{
    wxTreeItemId nodeId = DoInternalGetPage( pagePos );
    wxCHECK_MSG( nodeId.IsOk(), wxNOT_FOUND, wxT("Invalid page index spacified!") );

    const wxTreeItemId parent = m_tree->GetItemParent( nodeId );

    return parent.IsOk() ? DoInternalFindPageById(parent) : wxNOT_FOUND;
}

bool wxTreebook::SetPageText(size_t n, const wxString& strText)
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    m_tree->SetItemText(pageId, strText);

    return true;
}

wxString wxTreebook::GetPageText(size_t n) const
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), wxString(), wxT("invalid tree item") );

    return m_tree->GetItemText(pageId);
}

int wxTreebook::GetPageImage(size_t n) const
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), wxNOT_FOUND, wxT("invalid tree item") );

    return m_tree->GetItemImage(pageId);
}

bool wxTreebook::SetPageImage(size_t n, int imageId)
{
    wxTreeItemId pageId = DoInternalGetPage(n);

    wxCHECK_MSG( pageId.IsOk(), false, wxT("invalid tree item") );

    m_tree->SetItemImage(pageId, imageId);

    return true;
}

wxSize wxTreebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    const wxSize sizeTree = GetTreeSize();

    wxSize size = sizePage;
    size.x += sizeTree.x;

    return size;
}

int wxTreebook::GetSelection() const
{
   return m_selection;
}

int wxTreebook::SetSelection(size_t pagePos)
{
   if ( (size_t)m_selection != pagePos )
       return DoSetSelection(pagePos);

   return m_selection;
}

int wxTreebook::DoSetSelection(size_t pagePos)
{
    wxCHECK_MSG( IS_VALID_PAGE(pagePos), wxNOT_FOUND,
                 wxT("invalid page index in wxListbook::SetSelection()") );
    wxASSERT_MSG( GetPageCount() == DoInternalGetPageCount(),
                  wxT("wxTreebook logic error: m_treeIds and m_pages not in sync!"));

    const int oldSel = m_selection;

    wxTreebookEvent event(wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, m_windowId);
    event.SetEventObject(this);
    event.SetSelection(pagePos);
    event.SetOldSelection(m_selection);

    // don't send the event if the old and new pages are the same; do send it
    // otherwise and be prepared for it to be vetoed
    if ( (int)pagePos == m_selection ||
            !GetEventHandler()->ProcessEvent(event) ||
                event.IsAllowed() )
    {
        // hide the previously shown page
        wxTreebookPage * const oldPage = DoGetCurrentPage();
        if ( oldPage )
            oldPage->Hide();

        // then show the new one
        m_selection = pagePos;
        wxTreebookPage *page = wxBookCtrlBase::GetPage(m_selection);
        if ( !page )
        {
            // find the next page suitable to be shown: the first (grand)child
            // of this one with a non-NULL associated page
            wxTreeItemId childId = m_treeIds[pagePos];
            m_actualSelection = pagePos;
            while ( !page && childId.IsOk() )
            {
                wxTreeItemIdValue cookie;
                childId = m_tree->GetFirstChild( childId, cookie );
                if ( childId.IsOk() )
                {
                    page = wxBookCtrlBase::GetPage(++m_actualSelection);
                }
            }

            wxASSERT_MSG( page, wxT("no page to show found!") );
        }

        if ( page )
        {
            page->SetSize(GetPageRect());
            page->Show();
        }

        m_tree->SelectItem(DoInternalGetPage(pagePos));

        // notify about the (now completed) page change
        event.SetEventType(wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED);
        (void)GetEventHandler()->ProcessEvent(event);
    }
    else // page change vetoed
    {
        // tree selection might have already had changed
        m_tree->SelectItem(DoInternalGetPage(oldSel));
    }

    return oldSel;
}

void wxTreebook::SetImageList(wxImageList *imageList)
{
    wxBookCtrlBase::SetImageList(imageList);
    m_tree->SetImageList(imageList);
}

void wxTreebook::AssignImageList(wxImageList *imageList)
{
    wxBookCtrlBase::AssignImageList(imageList);
    m_tree->SetImageList(imageList);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxTreebook::OnTreeSelectionChange(wxTreeEvent& event)
{
    wxTreeItemId newId = event.GetItem();

    if ( (m_selection == wxNOT_FOUND &&
                (!newId.IsOk() || newId == m_tree->GetRootItem())) ||
            (m_selection != wxNOT_FOUND && newId == m_treeIds[m_selection]) )
    {
        // this event can only come when we modify the tree selection ourselves
        // so we should simply ignore it
        return;
    }

    int newPos = DoInternalFindPageById(newId);

    if ( newPos != wxNOT_FOUND )
        SetSelection( newPos );
}

void wxTreebook::OnTreeNodeExpandedCollapsed(wxTreeEvent & event)
{
    wxTreeItemId nodeId = event.GetItem();
    if ( !nodeId.IsOk() || nodeId == m_tree->GetRootItem() )
        return;
    int pagePos = DoInternalFindPageById(nodeId);
    wxCHECK_RET( pagePos != wxNOT_FOUND, wxT("Internal problem in wxTreebook!..") );

    wxTreebookEvent ev(m_tree->IsExpanded(nodeId)
            ? wxEVT_COMMAND_TREEBOOK_NODE_EXPANDED
            : wxEVT_COMMAND_TREEBOOK_NODE_COLLAPSED,
        m_windowId);

    ev.SetSelection(pagePos);
    ev.SetOldSelection(pagePos);
    ev.SetEventObject(this);

    GetEventHandler()->ProcessEvent(ev);
}

// ----------------------------------------------------------------------------
// wxTreebook geometry management
// ----------------------------------------------------------------------------

wxSize wxTreebook::GetTreeSize() const
{
    const wxSize sizeClient = GetClientSize(),
                 sizeBorder = m_tree->GetSize() - m_tree->GetClientSize(),
                 sizeTree = m_tree->GetBestSize() + sizeBorder;

    wxSize size;

    size.x = sizeTree.x;
    size.y = sizeClient.y;

    return size;
}

wxRect wxTreebook::GetPageRect() const
{
    const wxSize sizeTree = m_tree->GetSize();

    wxPoint pt;
    wxRect rectPage(pt, GetClientSize());
    switch ( GetWindowStyle() & wxTBK_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected wxTreebook alignment") );
            // fall through

        case wxTBK_LEFT:
            rectPage.x = sizeTree.x; // + MARGIN;
            // fall through

        case wxTBK_RIGHT:
            rectPage.width -= sizeTree.x; // + MARGIN;
            break;
    }

    return rectPage;
}

void wxTreebook::OnSize(wxSizeEvent& event)
{
    event.Skip();

    if ( !m_tree )
    {
        // we're not fully created yet
        return;
    }

    // resize the list control and the page area to fit inside our new size
    const wxSize sizeClient = GetClientSize(),
                 sizeBorder = m_tree->GetSize() - m_tree->GetClientSize(),
                 sizeTree = GetTreeSize();

    m_tree->SetClientSize( sizeTree.x - sizeBorder.x, sizeTree.y - sizeBorder.y );

    const wxSize sizeNew = m_tree->GetSize();
    wxPoint posTree;
    switch ( GetWindowStyle() & wxTBK_ALIGN_MASK )
    {
        default:
            wxFAIL_MSG( _T("unexpected wxTreebook alignment") );
            // fall through

        case wxTBK_LEFT:
            // posTree is already ok
            break;

        case wxTBK_RIGHT:
            posTree.x = sizeClient.x - sizeNew.x;
            break;
    }

    if ( m_tree->GetPosition() != posTree )
        m_tree->Move(posTree);

    // resize the currently shown page
    wxTreebookPage *page = DoGetCurrentPage();
    if ( page )
    {
        wxRect rectPage = GetPageRect();
        page->SetSize(rectPage);
    }
}

wxTreebookPage * wxTreebook::DoGetCurrentPage() const
{
    if ( m_selection == wxNOT_FOUND )
        return NULL;

    wxTreebookPage *page = wxBookCtrlBase::GetPage(m_selection);
    if ( !page && m_actualSelection != wxNOT_FOUND )
    {
        page = wxBookCtrlBase::GetPage(m_actualSelection);
    }

    return page;
}

#endif // wxUSE_TREEBOOK

