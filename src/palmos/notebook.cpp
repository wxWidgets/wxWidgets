///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_NOTEBOOK

// wxWidgets
#ifndef WX_PRECOMP
  #include  "wx/string.h"
#endif  // WX_PRECOMP

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxNotebook, wxBookCtrlBase)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, wxNotebook::OnSelChange)

    EVT_SIZE(wxNotebook::OnSize)

    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

const wxNotebookPageInfoList& wxNotebook::GetPageInfos() const
{
    wxNotebookPageInfoList* list;

    return m_pageInfos ;
}

// common part of all ctors
void wxNotebook::Init()
{
}

// default for dynamic class
wxNotebook::wxNotebook()
{
}

// the same arguments as for wxControl
wxNotebook::wxNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
}

// Create() function
bool wxNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    return false;
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------

size_t wxNotebook::GetPageCount() const
{
    return 0;
}

int wxNotebook::GetRowCount() const
{
    return 0;
}

int wxNotebook::SetSelection(size_t nPage)
{
    return 0;
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    return 0;
}

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
    return false;
}

wxString wxNotebook::GetPageText(size_t nPage) const
{
    return wxEmptyString;
}

int wxNotebook::GetPageImage(size_t nPage) const
{
    return -1;
}

bool wxNotebook::SetPageImage(size_t nPage, int nImage)
{
    return false;
}

void wxNotebook::SetImageList(wxImageList* imageList)
{
}

// ----------------------------------------------------------------------------
// wxNotebook size settings
// ----------------------------------------------------------------------------

void wxNotebook::SetPageSize(const wxSize& size)
{
}

void wxNotebook::SetPadding(const wxSize& padding)
{
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    return wxSize(0,0);
}

void wxNotebook::AdjustPageSize(wxNotebookPage *page)
{
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook, without deleting
wxNotebookPage *wxNotebook::DoRemovePage(size_t nPage)
{
    return NULL;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    return true;
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    return false;
}

int wxNotebook::HitTest(const wxPoint& pt, long *flags) const
{
    return 0;
}


// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

void wxNotebook::OnSize(wxSizeEvent& event)
{
}

void wxNotebook::OnSelChange(wxBookCtrlEvent& event)
{
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
}

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

#if wxUSE_CONSTRAINTS

// override these 2 functions to do nothing: everything is done in OnSize

void wxNotebook::SetConstraintSizes(bool WXUNUSED(recurse))
{
}

bool wxNotebook::DoPhase(int WXUNUSED(nPhase))
{
  return true;
}

#endif // wxUSE_CONSTRAINTS

#endif // wxUSE_NOTEBOOK
