///////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_notebook_n.mm
// Purpose:     wxMoNotebook class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/sizer.h"
    #include "wx/string.h"
    #include "wx/dc.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/control.h"
#endif  // WX_PRECOMP

#include "wx/imaglist.h"
#include "wx/sysopt.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(page) ((page) < GetPageCount())

// copied from src/common/nbkbase.cpp
extern WXDLLEXPORT_DATA(const char) wxNotebookNameStr[] = "notebook";

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

#if 0
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)
#endif

BEGIN_EVENT_TABLE(wxMoNotebook, wxBookCtrlBase)
    EVT_TAB_SEL_CHANGING(wxID_ANY, wxMoNotebook::OnTabChanging)
    EVT_TAB_SEL_CHANGED(wxID_ANY, wxMoNotebook::OnTabChanged)

#if USE_NOTEBOOK_ANTIFLICKER
    EVT_ERASE_BACKGROUND(wxMoNotebook::OnEraseBackground)
    EVT_PAINT(wxMoNotebook::OnPaint)
#endif // USE_NOTEBOOK_ANTIFLICKER
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxMoNotebook, wxBookCtrlBase)

// IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

// common part of all ctors
void wxMoNotebook::Init()
{
    // FIXME stub
}

wxMoNotebook::wxMoNotebook()
{
    // FIXME stub
}

wxMoNotebook::wxMoNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    // FIXME stub
}

bool wxMoNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoNotebook::~wxMoNotebook()
{
}

// ----------------------------------------------------------------------------
// wxMoNotebook accessors
// ----------------------------------------------------------------------------

size_t wxMoNotebook::GetPageCount() const
{
    // FIXME stub

    return 0;
}

int wxMoNotebook::GetRowCount() const
{
    // FIXME stub

    return 0;
}

// get the currently selected page
int wxMoNotebook::GetSelection() const
{
    // FIXME stub

    return 0;
}

int wxMoNotebook::SetSelection(size_t page)
{
    // FIXME stub

    return 0;
}

int wxMoNotebook::ChangeSelection(size_t page)
{
    // FIXME stub

    return 0;
}

bool wxMoNotebook::SetPageText(size_t page, const wxString& strText)
{
    // FIXME stub

    return true;
}

wxString wxMoNotebook::GetPageText(size_t page) const
{
    // FIXME stub

    return wxEmptyString;
}

int wxMoNotebook::GetPageImage(size_t page) const
{
    // FIXME stub

    return 0;
}

bool wxMoNotebook::SetPageImage(size_t page, int nImage)
{
    // FIXME stub

    return true;
}

void wxMoNotebook::SetImageList(wxImageList* imageList)
{
    // FIXME stub
}

void wxMoNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
}

wxSize wxMoNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

void wxMoNotebook::UpdateSelectedPage(size_t newsel)
{
    // FIXME stub
}

// get the size which the choice control should have
wxSize wxMoNotebook::GetControllerSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}


// remove one page from the notebook, without deleting
wxNotebookPage *wxMoNotebook::DoRemovePage(size_t page)
{
    // FIXME stub

    return NULL;
}

// remove all pages
bool wxMoNotebook::DeleteAllPages()
{
    // FIXME stub

    return true;
}

// same as AddPage() but does it at given position
bool wxMoNotebook::InsertPage(size_t page,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    // FIXME stub

    return true;
}

int wxMoNotebook::HitTest(const wxPoint& WXUNUSED(pt), long *WXUNUSED(flags)) const
{
    return -1;
}

void wxMoNotebook::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // do nothing here
}

void wxMoNotebook::OnTabChanging(wxTabEvent& event)
{
    // FIXME stub
}

void wxMoNotebook::OnTabChanged(wxTabEvent& event)
{
    // FIXME stub
}

wxBookCtrlBaseEvent* wxMoNotebook::CreatePageChangingEvent() const
{
    // FIXME stub

    return NULL;
}

void wxMoNotebook::MakeChangedEvent(wxBookCtrlBaseEvent &event)
{
    // FIXME stub
}

// Set a text badge for the given item
bool wxMoNotebook::SetBadge(int item, const wxString& badge)
{
    // FIXME stub

    return true;
}

// Get the text badge for the given item
wxString wxMoNotebook::GetBadge(int item) const
{
    // FIXME stub

    return wxEmptyString;
}


#pragma mark -
#pragma mark Definition of absent wxBookCtrlBase methods

IMPLEMENT_ABSTRACT_CLASS(wxBookCtrlBase, wxControl)

BEGIN_EVENT_TABLE(wxBookCtrlBase, wxControl)
    EVT_SIZE(wxBookCtrlBase::OnSize)
END_EVENT_TABLE()


wxBookCtrlBase::~wxBookCtrlBase() {
    // FIXME stub
}

void wxBookCtrlBase::Init() {
    // FIXME stub
}

void wxBookCtrlBase::DoInvalidateBestSize() {
    // FIXME stub
}

wxSize wxBookCtrlBase::DoGetBestSize() const {
    // FIXME stub
    
    wxSize empty(1, 1);
    return empty;
}

void wxBookCtrlBase::OnSize(wxSizeEvent& event) {
    // FIXME stub
}

void wxBookCtrlBase::SetPageSize(const wxSize& size) {
    // FIXME stub
}

bool wxBookCtrlBase::DeletePage(size_t n) {
    // FIXME stub
    return true;
}

int wxBookCtrlBase::DoSetSelection(size_t nPage, int flags) {
    // FIXME stub
    return 0;
}

bool wxBookCtrlBase::DoSetSelectionAfterInsertion(size_t n, bool bSelect) {
    // FIXME stub
    return true;
}

void wxBookCtrlBase::DoSize() {
    // FIXME stub
}

wxRect wxBookCtrlBase::GetPageRect() const {
    // FIXME stub
    wxRect empty;
    return empty;
}

void wxBookCtrlBase::SetImageList(wxImageList *imageList) {
    // FIXME stub
}

wxSize wxBookCtrlBase::CalcSizeFromPage(const wxSize& sizePage) const {
    // FIXME stub
    wxSize empty(1, 1);
    return empty;
}
