///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/auibook.cpp
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Modified by: Jens Lody
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved
//                            2012, Jens Lody for the code related to left
//                                  and right positioning
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------


#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_AUI

#include "wx/aui/auibook.h"
#include "wx/aui/dockart.h"
#include "wx/aui/framemanager.h"
#include "wx/aui/tabart.h"
#include "wx/aui/tabmdi.h"

#ifndef WX_PRECOMP
#include "wx/settings.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#endif


#include "wx/arrimpl.cpp"

wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_BUTTON, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_CANCEL_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_AUINOTEBOOK_TAB_DCLICK, wxAuiNotebookEvent);

IMPLEMENT_CLASS(wxAuiNotebook, wxBookCtrlBase)
IMPLEMENT_DYNAMIC_CLASS(wxAuiNotebookEvent, wxBookCtrlEvent)

// extern functions from framemanager.cpp
extern void wxAuiDoInsertDockLayer(wxAuiPaneInfoArray&,int,int);
extern void wxAuiDoInsertDockRow(wxAuiPaneInfoArray&,int,int,int);
extern void wxAuiDoInsertPane(wxAuiPaneInfoArray&,int,int,int,int);
extern void wxAuiDoInsertPage(wxAuiPaneInfoArray&,int,int,int,int,int);


BEGIN_EVENT_TABLE(wxAuiNotebook, wxControl)
EVT_SIZE(wxAuiNotebook::OnSize)
EVT_AUI_ALLOW_DND(wxAuiNotebook::OnPaneDrop)
END_EVENT_TABLE()

// -- wxAuiNotebook class implementation --
bool wxAuiNotebook::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    if(!wxControl::Create(parent, id, pos, size, style))
        return false;

    Init(style);
    return true;
}

// Init contains common initialization
// code called by all constructors
void wxAuiNotebook::Init(long style)
{
    SetName(wxT("wxAuiNotebook"));
    SetArtProvider(new wxAuiDefaultTabArt);

    m_mgr.SetManagedWindow(this);
    m_mgr.SetDockSizeConstraint(1.0, 1.0); // no dock size constraint
    SetWindowStyleFlag(style);

}

wxAuiNotebook::~wxAuiNotebook()
{
    // Indicate we're deleting pages
    SendDestroyEvent();

    while ( GetPageCount() > 0 )
        DeletePage(0);

    m_mgr.UnInit();
}

void wxAuiNotebook::SetArtProvider(wxAuiTabArt* art)
{
    m_mgr.SetTabArtProvider(art);
    UpdateTabCtrlSize();
}

// SetTabCtrlHeight() is the highest-level override of the
// tab height.  A call to this function effectively enforces a
// specified tab ctrl height, overriding all other considerations,
// such as text or bitmap height.  It overrides any call to
// SetUniformBitmapSize().  Specifying a height of -1 reverts
// any previous call and returns to the default behaviour
void wxAuiNotebook::SetTabCtrlHeight(int height)
{
    GetArtProvider()->SetTabCtrlHeight(height);
}

void wxAuiNotebook::SetTabCtrlWidth(int width)
{
    GetArtProvider()->SetTabCtrlWidth(width);
}


// SetUniformBitmapSize() ensures that all tabs will have
// the same height, even if some tabs don't have bitmaps
// Passing wxDefaultSize to this function will instruct
// the control to use dynamic tab height-- so when a tab
// with a large bitmap is added, the tab ctrl's height will
// automatically increase to accommodate the bitmap

void wxAuiNotebook::SetUniformBitmapSize(const wxSize& size)
{
    GetArtProvider()->SetUniformBitmapSize(size);
}

// UpdateTabCtrlSize() does the actual tab resizing. It's meant
// to be used internally
bool wxAuiNotebook::UpdateTabCtrlSize()
{
    // get the tab ctrl height we will use
    wxSize size = CalculateTabCtrlSize();

    // if the tab control height needs to change, update
    // all of our tab controls with the new height
    if (m_mgr.GetTabArtProvider()->GetRequestedSize() == size)
        return false;


    // Are we supposed to update from the 'size' or what?

    return true;
}

wxSize wxAuiNotebook::CalculateTabCtrlSize()
{
    // find out new best tab size
    wxAuiPaneInfoPtrArray allPanes;
    unsigned int i;
    for(i=0;i<m_mgr.GetPaneCount();i++)
    {
        allPanes.Add(&m_mgr.GetPane(i));
    }

    wxSize tab_size = m_mgr.GetTabArtProvider()->GetBestTabSize((wxWindow*)this, allPanes,
        m_mgr.GetTabArtProvider()->GetRequiredBitmapSize());
    // if a fixed tab ctrl height is specified,
    // just use that instead of calculating a
    // tab height
    if (m_mgr.GetTabArtProvider()->GetRequestedSize().y != -1)
        tab_size.y = m_mgr.GetTabArtProvider()->GetRequestedSize().y;

    return tab_size;
}


wxAuiTabArt* wxAuiNotebook::GetArtProvider() const
{
    return m_mgr.GetTabArtProvider();
}

void wxAuiNotebook::SetWindowStyleFlag(long style)
{
    //Necessary for backwards compatibility with old wxAuiNotebook control - if we don't strip these flags then we end up with an ugly border around notebooks (where there was none before) in old code.
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;
    style |= wxWANTS_CHARS;
    style |= wxAUI_MGR_NB_ALLOW_NOTEBOOKS;

    wxControl::SetWindowStyleFlag(style);

    // copy notebook dedicated style flags to the onboard manager flags
    m_mgr.SetFlags( style & 0xFFFF );

    // split is done by allowing the pane normal docking
    // if the flag changes, we have to redo the layout
    bool allow_docking = m_mgr.HasFlag(wxAUI_MGR_NB_TAB_SPLIT);
    for(size_t i=0; i<m_mgr.GetAllPanes().size() ;i++)
        m_mgr.GetAllPanes()[i].Dockable(allow_docking);

    m_mgr.Update();
}


bool wxAuiNotebook::AddPage(wxWindow* page, const wxString& caption, bool select, const wxBitmap& bitmap)
{
    wxASSERT_MSG(page, wxT("page pointer must be non-NULL"));
    if (!page)
        return false;

    return InsertPage(m_mgr.GetAllPanes().size(),page,caption,select,bitmap);
}

bool wxAuiNotebook::InsertPage(size_t pageIndex, wxWindow* page, const wxString& caption, bool select, const wxBitmap& bitmap)
{
    wxASSERT_MSG(page, wxT("page pointer must be non-NULL"));
    if (!page)
        return false;

    // Shift other panes so that this one can go in between them if necessary
    wxAuiDoInsertPage(m_mgr.GetAllPanes(),1,0,1,0,pageIndex);

    m_mgr.AddPane(page, wxAuiPaneInfo().Centre().Layer(0).Position(0).Caption(caption).Floatable(false).Movable().Page(pageIndex).Icon(bitmap).Dockable(m_mgr.HasFlag(wxAUI_NB_TAB_SPLIT)).CloseButton(false).AlwaysDockInNotebook());

    // Change the selection if explicitly requested, or if the page is the first one; for the later case, it ensures that there is a current page in the notebook
    if (select || GetPageCount() == 1)
        SetSelection(pageIndex);

    m_mgr.Update();

    return true;
}


// DeletePage() removes a tab from the multi-notebook,
// and destroys the window as well
bool wxAuiNotebook::DeletePage(size_t pageIndex)
{
    if(pageIndex>=m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::DeletePage"));
        return false;
    }

    wxWindow* wnd = m_mgr.GetPane(pageIndex).GetWindow();
    m_mgr.DetachPane(wnd);
#if wxUSE_MDI
    // actually destroy the window now
    if (wxDynamicCast(wnd,wxAuiMDIChildFrame))
    {
        // delete the child frame with pending delete, as is
        // customary with frame windows
        if (!wxPendingDelete.Member(wnd))
            wxPendingDelete.Append(wnd);
    }
    else
#endif
    {
        wnd->Destroy();
    }
    m_mgr.Update();
    return true;
}



// RemovePage() removes a tab from the multi-notebook,
// but does not destroy the window
bool wxAuiNotebook::RemovePage(size_t pageIndex)
{
    if(pageIndex>=m_mgr.GetPaneCount())
    {
        // set new active pane unless we're being destroyed anyhow
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::RemovePage"));
        return false;
    }

    m_mgr.DetachPane(m_mgr.GetPane(pageIndex).GetWindow());
    m_mgr.Update();
    return true;
}

// GetPageIndex() returns the index of the page, or wxNOT_FOUND if the
// page could not be located in the notebook
int wxAuiNotebook::GetPageIndex(wxWindow* pageWindow)
{
    return m_mgr.GetAllPanes().Index(m_mgr.GetPane(pageWindow));
}



// SetPageText() changes the tab caption of the specified page
bool wxAuiNotebook::SetPageText(size_t pageIndex, const wxString& text)
{
    if(pageIndex>=m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::SetPageText"));
        return false;
    }

    m_mgr.GetPane(pageIndex).Caption(text);
    m_mgr.Update();
    return true;
}

// returns the page caption
wxString wxAuiNotebook::GetPageText(size_t pageIndex) const
{
    if(pageIndex>=m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::GetPageText"));
        return wxEmptyString;
    }

    return m_mgr.GetPane(pageIndex).GetCaption();
}

bool wxAuiNotebook::SetPageToolTip(size_t page_idx, const wxString& text)
{
    if (page_idx >= m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::SetPageToolTip"));
        return false;
    }

    // update tooltip info for pane.
    m_mgr.GetPane(page_idx).ToolTip(text);
    m_mgr.Update();

    // NB: we don't update the tooltip if it is already being displayed, it
    //     typically never happens, no need to code that
    return true;
}

wxString wxAuiNotebook::GetPageToolTip(size_t pageIndex) const
{
    if (pageIndex >= m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::GetPageToolTip"));
        return wxT("");
    }

    return m_mgr.GetPane(pageIndex).GetToolTip();;
}


bool wxAuiNotebook::SetPageBitmap(size_t pageIndex, const wxBitmap& bitmap)
{
    if(pageIndex>=m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::SetPageBitmap"));
        return false;
    }

    m_mgr.GetPane(pageIndex).Icon(bitmap);
    m_mgr.Update();
    return true;
}

// returns the page bitmap
wxBitmap wxAuiNotebook::GetPageBitmap(size_t pageIndex) const
{
    if(pageIndex>=m_mgr.GetPaneCount())
    {
        wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::GetPageBitmap"));
        return wxNullBitmap;
    }

    return m_mgr.GetPane(pageIndex).GetIcon();
}

// GetSelection() returns the index of the currently active page
int wxAuiNotebook::GetSelection() const
{
    return m_mgr.GetActivePane(FindFocus());
}

// SetSelection() sets the currently active page and emit changed event
int wxAuiNotebook::SetSelection(size_t new_page)
{
    return DoModifySelection(new_page, true);
}


// GetPageCount() returns the total number of
// pages managed by the multi-notebook
size_t wxAuiNotebook::GetPageCount() const
{
    return m_mgr.GetPaneCount();
}

// GetPage() returns the wxWindow pointer of the
// specified page
wxWindow* wxAuiNotebook::GetPage(size_t pageIndex) const
{
    return m_mgr.GetPane(pageIndex).GetWindow();
}

// GetActiveTabCtrl() returns the active tab control.  It is
// called to determine which control gets new windows being added
wxAuiTabContainer* wxAuiNotebook::GetActiveTabCtrl()
{
    unsigned int sel_pane_index = GetSelection();
    if (sel_pane_index >= 0 && sel_pane_index < m_mgr.GetPaneCount())
    {
        wxAuiTabContainer* ctrl;
        int idx;

        // find the tab ctrl with the current page
        if (m_mgr.FindTab(m_mgr.GetPane(sel_pane_index).GetWindow(), &ctrl, &idx))
        {
            return ctrl;
        }
    }

    // no current page, just find the first tab ctrl
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    size_t i, pane_count = all_panes.GetCount();
    for (i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.GetName() == wxT("dummy"))
            continue;

        wxAuiTabContainer* ctrl;
        int idx;

        // Return the tab ctrl for this pane, if one exists.
        if (m_mgr.FindTab(pane.GetWindow(), &ctrl, &idx))
        {
            return ctrl;
        }
    }

    return NULL;
}

void wxAuiNotebook::OnTabCancelDrag(wxAuiNotebookEvent& /* evt */)
{
    //fixme: (MJM) merge - this has been broken in the merge and needs to be re-implemented
}

void wxAuiNotebook::OnPaneDrop(wxAuiManagerEvent& evt)
{
    // Fire our own notebook specific event in place of normal manager one.
    // This is for backwards compatibility with older notebook code, before the aui manager handled this.
    wxAuiNotebookEvent e(wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, m_mgr.GetManagedWindow()->GetId());
    int selIndex=m_mgr.GetAllPanes().Index(*evt.GetPane());
    e.SetSelection(selIndex);
    e.SetOldSelection(selIndex);
    e.SetEventObject(this);
    e.SetDragSource(this);
    e.Veto(); // dropping must be explicitly approved by control owner

    ProcessEvent(e);

    evt.Veto(!e.IsAllowed());
}

void wxAuiNotebook::Split(size_t pageIndex, int direction)
{
    // If splitting is not allowed then simply return
    if(!m_mgr.HasFlag(wxAUI_NB_TAB_SPLIT))
    {
        return;
    }

    wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();
    unsigned int pageCount = panes.size();
    if(pageCount>pageIndex)
    {
        switch(direction)
        {
            case wxTOP: direction = wxAUI_DOCK_TOP; break;
            case wxBOTTOM: direction = wxAUI_DOCK_BOTTOM; break;
            case wxLEFT: direction = wxAUI_DOCK_LEFT; break;
            case wxRIGHT: direction = wxAUI_DOCK_RIGHT; break;
            default: wxASSERT_MSG(0, wxT("Invalid direction passed to wxAuiNotebook::Split")); return;
        }

        panes[pageIndex].Direction(direction);

        wxAuiDoInsertDockLayer(panes,direction,panes[pageIndex].GetLayer());

        m_mgr.SetActivePane(panes[pageIndex].GetWindow());

        m_mgr.Update();
        return;
    }
    wxASSERT_MSG(0, wxT("Invalid page index passed to wxAuiNotebook::Split"));
}

// Sets the normal font
void wxAuiNotebook::SetNormalFont(const wxFont& font)
{
    GetArtProvider()->SetNormalFont(font);
}

// Sets the selected tab font
void wxAuiNotebook::SetSelectedFont(const wxFont& font)
{
    GetArtProvider()->SetSelectedFont(font);
}

// Sets the measuring font
void wxAuiNotebook::SetMeasuringFont(const wxFont& font)
{
    GetArtProvider()->SetMeasuringFont(font);
}

// Sets the tab font
bool wxAuiNotebook::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    wxFont normalFont(font);
    wxFont selectedFont(normalFont);
    selectedFont.SetWeight(wxFONTWEIGHT_BOLD);

    SetNormalFont(normalFont);
    SetSelectedFont(selectedFont);
    SetMeasuringFont(selectedFont);

    return true;
}

// Gets the tab control height
int wxAuiNotebook::GetTabCtrlHeight() const
{
    return GetArtProvider()->m_tabCtrlHeight;
}

// Gets the tab control width
int wxAuiNotebook::GetTabCtrlWidth() const
{
    return GetArtProvider()->m_tabCtrlWidth;
}

// Gets the height of the notebook for a given page height
int wxAuiNotebook::GetHeightForPageHeight(int pageHeight)
{
    UpdateTabCtrlSize();

    int tabCtrlHeight = GetTabCtrlHeight();
    int decorHeight = 2;
    return tabCtrlHeight + pageHeight + decorHeight;
}

// Gets the width of the notebook for a given page width
int wxAuiNotebook::GetWidthForPageWidth(int pageWidth)
{
    UpdateTabCtrlSize();

    int tabCtrlWidth = GetTabCtrlWidth();
    int decorWidth = 2;
    return tabCtrlWidth + pageWidth + decorWidth;
}

// Advances the selection, generation page selection events
void wxAuiNotebook::AdvanceSelection(bool forward)
{
    if (GetPageCount() <= 1)
        return;

    int currentSelection = GetSelection();

    if (forward)
    {
        if (currentSelection == (int) (GetPageCount() - 1))
            return;
        else if (currentSelection == -1)
            currentSelection = 0;
        else
            currentSelection ++;
    }
    else
    {
        if (currentSelection <= 0)
            return;
        else
            currentSelection --;
    }

    SetSelection(currentSelection);
}

// Shows the window menu
bool wxAuiNotebook::ShowWindowMenu()
{
    wxAuiTabContainer* tabCtrl = GetActiveTabCtrl();

    int idx = tabCtrl->GetArtProvider()->ShowDropDown(this, tabCtrl->GetPages(), tabCtrl->GetActivePage());

    if (idx != -1)
    {
        wxAuiNotebookEvent e(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, GetId());
        e.SetSelection(idx);
        e.SetOldSelection(tabCtrl->GetActivePage());
        e.SetEventObject(this);
        GetEventHandler()->ProcessEvent(e);

        return true;
    }
    else
        return false;
}

void wxAuiNotebook::DoThaw()
{
    //fixme: (MJM) This has been broken in the merge and needs to be re-implemented.
    //DoSizing();

    wxBookCtrlBase::DoThaw();
}

void wxAuiNotebook::SetPageSize (const wxSize& WXUNUSED(size))
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
}

int wxAuiNotebook::HitTest (const wxPoint& WXUNUSED(pt), long* WXUNUSED(flags)) const
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
    return wxNOT_FOUND;
}

int wxAuiNotebook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG("Not implemented for wxAuiNotebook");
    return -1;
}

bool wxAuiNotebook::SetPageImage(size_t n, int imageId)
{
    return SetPageBitmap(n, GetImageList()->GetBitmap(imageId));
}

wxWindow* wxAuiNotebook::GetCurrentPage () const
{
    const int sel = GetSelection();

    return sel == wxNOT_FOUND ? NULL : GetPage(sel);
}

int wxAuiNotebook::ChangeSelection(size_t n)
{
    return DoModifySelection(n, false);
}

bool wxAuiNotebook::AddPage(wxWindow* page, const wxString& text, bool select, int imageId)
{
    if(HasImageList())
    {
        return AddPage(page, text, select, GetImageList()->GetBitmap(imageId));
    }
    else
    {
        return AddPage(page, text, select, wxNullBitmap);
    }
}

bool wxAuiNotebook::DeleteAllPages()
{
    size_t count = GetPageCount();
    for(size_t i = 0; i < count; i++)
    {
        DeletePage(0);
    }
    return true;
}

bool wxAuiNotebook::InsertPage(size_t index, wxWindow* page, const wxString& text, bool select, int imageId)
{
    if(HasImageList())
    {
        return InsertPage(index, page, text, select, GetImageList()->GetBitmap(imageId));
    }
    else
    {
        return InsertPage(index, page, text, select, wxNullBitmap);
    }
}

int wxAuiNotebook::DoModifySelection(size_t n, bool events)
{
    // don't change the page unless necessary;
    // however, clicking again on a tab should give it the focus.
    if ((int)n == GetSelection())
    {
        if(!GetCurrentPage()->HasFocus())
            GetCurrentPage()->SetFocus();

        return n;
    }

    bool vetoed = false;

    wxAuiNotebookEvent evt(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, m_windowId);

    if(events)
    {
        evt.SetSelection(n);
        evt.SetOldSelection(GetSelection());
        evt.SetEventObject(this);
        GetEventHandler()->ProcessEvent(evt);
        vetoed = !evt.IsAllowed();
    }

    if (!vetoed)
    {
        wxWindow* wnd = m_mgr.GetPane(n).GetWindow();
        m_mgr.SetActivePane(wnd);
        m_mgr.Update();

        // program allows the page change
        if(events)
        {
            evt.SetEventType(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED);
            (void)GetEventHandler()->ProcessEvent(evt);
        }
    }

    return GetSelection();
}


#endif // wxUSE_AUI
