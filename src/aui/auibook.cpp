///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/auibook.cpp
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved
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

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/image.h"
    #include "wx/menu.h"
#endif


#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxAuiNotebookPageArray)

wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_BUTTON, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEvent);

IMPLEMENT_CLASS(wxAuiNotebook, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxAuiNotebookEvent, wxEvent)


// -- wxAuiNotebook class implementation --
wxAuiNotebook::wxAuiNotebook()
{
    m_tab_ctrl_height = 20;
    m_requested_bmp_size = wxDefaultSize;
    m_requested_tabctrl_height = -1;
}

wxAuiNotebook::wxAuiNotebook(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style) : wxPanel(parent , id, pos, size, style)
{
    m_requested_bmp_size = wxDefaultSize;
    m_requested_tabctrl_height = -1;
    InitNotebook(style);
}

bool wxAuiNotebook::Create(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style)
{
    //temp: (MJM)
    //if (!wxControl::Create(parent, id, pos, size, style))
        //return false;

    InitNotebook(style);

    return true;
}

// InitNotebook() contains common initialization
// code called by all constructors
void wxAuiNotebook::InitNotebook(long style)
{
    SetName(wxT("wxAuiNotebook"));
    m_flags = (unsigned int)style;
    m_tab_ctrl_height = 20;

    m_normal_font = *wxNORMAL_FONT;
    m_selected_font = *wxNORMAL_FONT;
    m_selected_font.SetWeight(wxBOLD);

    SetArtProvider(new wxAuiDefaultTabArt);

    m_mgr.SetManagedWindow(this);
    m_mgr.SetFlags(wxAUI_MGR_DEFAULT);
    m_mgr.SetDockSizeConstraint(1.0, 1.0); // no dock size constraint
    m_mgr.Update();
}

wxAuiNotebook::~wxAuiNotebook()
{
    // Indicate we're deleting pages
    //temp: (MJM)
    //SendDestroyEvent();

    while ( GetPageCount() > 0 )
        DeletePage(0);

    //temp: (MJM)
    //m_mgr.UnInit();
}

void wxAuiNotebook::SetArtProvider(wxAuiTabArt* art)
{
    m_mgr.SetTabArtProvider(art);

    //temp: (MJM)
    //UpdateTabCtrlHeight();
}

// SetTabCtrlHeight() is the highest-level override of the
// tab height.  A call to this function effectively enforces a
// specified tab ctrl height, overriding all other considerations,
// such as text or bitmap height.  It overrides any call to
// SetUniformBitmapSize().  Specifying a height of -1 reverts
// any previous call and returns to the default behavior

void wxAuiNotebook::SetTabCtrlHeight(int height)
{
    m_requested_tabctrl_height = height;
}


// SetUniformBitmapSize() ensures that all tabs will have
// the same height, even if some tabs don't have bitmaps
// Passing wxDefaultSize to this function will instruct
// the control to use dynamic tab height-- so when a tab
// with a large bitmap is added, the tab ctrl's height will
// automatically increase to accommodate the bitmap

void wxAuiNotebook::SetUniformBitmapSize(const wxSize& size)
{
    m_requested_bmp_size = size;
}

// UpdateTabCtrlHeight() does the actual tab resizing. It's meant
// to be used interally
void wxAuiNotebook::UpdateTabCtrlHeight()
{
    // get the tab ctrl height we will use
    int height = CalculateTabCtrlHeight();

    // if the tab control height needs to change, update
    // all of our tab controls with the new height
    if (m_tab_ctrl_height != height)
    {
        //temp: (MJM)
    }
}



int wxAuiNotebook::CalculateTabCtrlHeight()
{
    // if a fixed tab ctrl height is specified,
    // just return that instead of calculating a
    // tab height
    if (m_requested_tabctrl_height != -1)
        return m_requested_tabctrl_height;

    // find out new best tab height
    //temp: (MJM)
    //wxAuiTabArt* art = GetArtProvider();
    //return art->GetBestTabCtrlSize(this,
    //                             m_tabs.GetPages(),
    //                             m_requested_bmp_size);
}


wxAuiTabArt* wxAuiNotebook::GetArtProvider() const
{
    //temp: (MJM)
    return NULL;
}

void wxAuiNotebook::SetWindowStyleFlag(long style)
{
    //temp: (MJM)
    //wxControl::SetWindowStyleFlag(style);

    m_flags = (unsigned int)style;

    m_mgr.Update();
}


bool wxAuiNotebook::AddPage(wxWindow* page,
                            const wxString& caption,
                            bool select,
                            const wxBitmap& bitmap)
{
    //temp: (MJM)
    m_mgr.AddPane(page, wxAuiPaneInfo().SetDirectionCentre().SetLayer(1).SetPosition(1).SetCaption(caption).SetFloatable(false).SetMovable(true).SetPage(m_mgr.GetAllPanes().size()));
    m_mgr.Update();
}

bool wxAuiNotebook::InsertPage(size_t page_idx,
                               wxWindow* page,
                               const wxString& caption,
                               bool select,
                               const wxBitmap& bitmap)
{
    wxASSERT_MSG(page, wxT("page pointer must be non-NULL"));
    if (!page)
        return false;

    //temp: (MJM)

    return true;
}


// DeletePage() removes a tab from the multi-notebook,
// and destroys the window as well
bool wxAuiNotebook::DeletePage(size_t page_idx)
{
    //temp: (MJM)
    /*
    if (page_idx >= m_tabs.GetPageCount())
        return false;

    wxWindow* wnd = m_tabs.GetWindowFromIdx(page_idx);

    // hide the window in advance, as this will
    // prevent flicker
    ShowWnd(wnd, false);

    if (!RemovePage(page_idx))
        return false;

#if wxUSE_MDI
    // actually destroy the window now
    if (wnd->IsKindOf(CLASSINFO(wxAuiMDIChildFrame)))
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
    */

    return true;
}



// RemovePage() removes a tab from the multi-notebook,
// but does not destroy the window
bool wxAuiNotebook::RemovePage(size_t page_idx)
{
    //temp: (MJM)
    /*
    // save active window pointer
    wxWindow* active_wnd = NULL;

    // save pointer of window being deleted
    wxWindow* wnd = m_tabs.GetWindowFromIdx(page_idx);
    wxWindow* new_active = NULL;

    // make sure we found the page
    if (!wnd)
        return false;

    // find out which onscreen tab ctrl owns this tab
    wxAuiTabCtrl* ctrl;
    int ctrl_idx;
    if (!FindTab(wnd, &ctrl, &ctrl_idx))
        return false;

    bool is_active_in_split = ctrl->GetPage(ctrl_idx).active;


    // remove the tab from main catalog
    if (!m_tabs.RemovePage(wnd))
        return false;

    // remove the tab from the onscreen tab ctrl
    ctrl->RemovePage(wnd);

    if (is_active_in_split)
    {
        int ctrl_new_page_count = (int)ctrl->GetPageCount();

        if (ctrl_idx >= ctrl_new_page_count)
            ctrl_idx = ctrl_new_page_count-1;

        if (ctrl_idx >= 0 && ctrl_idx < (int)ctrl->GetPageCount())
        {
            // set new page as active in the tab split
            ctrl->SetActivePage(ctrl_idx);

            // if the page deleted was the current page for the
            // entire tab control, then record the window
            // pointer of the new active page for activation
            if (is_curpage)
            {
                new_active = ctrl->GetWindowFromIdx(ctrl_idx);
            }
        }
    }
    else
    {
        // we are not deleting the active page, so keep it the same
        new_active = active_wnd;
    }


    if (!new_active)
    {
        // we haven't yet found a new page to active,
        // so select the next page from the main tab
        // catalogue

        if (page_idx < m_tabs.GetPageCount())
        {
            new_active = m_tabs.GetPage(page_idx).window;
        }

        if (!new_active && m_tabs.GetPageCount() > 0)
        {
            new_active = m_tabs.GetPage(0).window;
        }
    }


    RemoveEmptyTabFrames();

    // set new active pane
    if (new_active && !m_isBeingDeleted)
    {
        SetSelectionToWindow(new_active);
    }
    */

    return true;
}

// GetPageIndex() returns the index of the page, or -1 if the
// page could not be located in the notebook
int wxAuiNotebook::GetPageIndex(wxWindow* page_wnd) const
{
    //temp: (MJM)
    return -1;
}



// SetPageText() changes the tab caption of the specified page
bool wxAuiNotebook::SetPageText(size_t page_idx, const wxString& text)
{
    //temp: (MJM)
    return true;
}

// returns the page caption
wxString wxAuiNotebook::GetPageText(size_t page_idx) const
{
    //temp: (MJM)
    return wxT("");
}

bool wxAuiNotebook::SetPageBitmap(size_t page_idx, const wxBitmap& bitmap)
{
    //temp: (MJM)
    return true;
}

// returns the page bitmap
wxBitmap wxAuiNotebook::GetPageBitmap(size_t page_idx) const
{
    //temp: (MJM)
    return wxBitmap();
}

// GetSelection() returns the index of the currently active page
int wxAuiNotebook::GetSelection() const
{
    //temp: (MJM)
    return -1;
}

// SetSelection() sets the currently active page
size_t wxAuiNotebook::SetSelection(size_t new_page)
{
    //temp: (MJM)
    return -1;
}


// GetPageCount() returns the total number of
// pages managed by the multi-notebook
size_t wxAuiNotebook::GetPageCount() const
{
    //temp: (MJM)
    return -1;
}

// GetPage() returns the wxWindow pointer of the
// specified page
wxWindow* wxAuiNotebook::GetPage(size_t page_idx) const
{
    //temp: (MJM)
    return NULL;
}

// GetActiveTabCtrl() returns the active tab control.  It is
// called to determine which control gets new windows being added
wxAuiTabCtrl* wxAuiNotebook::GetActiveTabCtrl()
{
    //temp: (MJM)
    return NULL;
}

void wxAuiNotebook::Split(size_t page, int direction)
{
    //temp: (MJM) - we should be able to implement by setting a few simple pane properties now
}

// Sets the normal font
void wxAuiNotebook::SetNormalFont(const wxFont& font)
{
    m_normal_font = font;
    GetArtProvider()->SetNormalFont(font);
}

// Sets the selected tab font
void wxAuiNotebook::SetSelectedFont(const wxFont& font)
{
    m_selected_font = font;
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
    //temp: (MJM)
    //wxControl::SetFont(font);

    wxFont normalFont(font);
    wxFont selectedFont(normalFont);
    selectedFont.SetWeight(wxBOLD);

    SetNormalFont(normalFont);
    SetSelectedFont(selectedFont);
    SetMeasuringFont(selectedFont);

    return true;
}

// Gets the tab control height
int wxAuiNotebook::GetTabCtrlHeight() const
{
    return m_tab_ctrl_height;
}

// Gets the height of the notebook for a given page height
int wxAuiNotebook::GetHeightForPageHeight(int pageHeight)
{
    UpdateTabCtrlHeight();

    int tabCtrlHeight = GetTabCtrlHeight();
    int decorHeight = 2;
    return tabCtrlHeight + pageHeight + decorHeight;
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
   //temp: (MJM)
}

#endif // wxUSE_AUI