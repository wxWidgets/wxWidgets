////////////////////////////////////////////////////////////////////////////////
// Name:        src/common/listctrlcmn.cpp
// Purpose:     Common defines for wxListCtrl and wxListCtrl-based classes.
// Author:      Kevin Ollivier
// Created:     09/15/06
// Copyright:   (c) Kevin Ollivier
// Licence:     wxWindows licence
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_LISTCTRL

#include "wx/listctrl.h"
#include "wx/imaglist.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif

const char wxListCtrlNameStr[] = "listCtrl";

// ListCtrl events
wxDEFINE_EVENT( wxEVT_LIST_BEGIN_DRAG, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_BEGIN_RDRAG, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_BEGIN_LABEL_EDIT, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_END_LABEL_EDIT, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_DELETE_ITEM, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_DELETE_ALL_ITEMS, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_SELECTED, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_DESELECTED, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_KEY_DOWN, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_INSERT_ITEM, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_COL_CLICK, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_COL_RIGHT_CLICK, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_COL_BEGIN_DRAG, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_COL_DRAGGING, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_COL_END_DRAG, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_RIGHT_CLICK, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_MIDDLE_CLICK, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_ACTIVATED, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_FOCUSED, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_CHECKED, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_ITEM_UNCHECKED, wxListEvent );
wxDEFINE_EVENT( wxEVT_LIST_CACHE_HINT, wxListEvent );

// -----------------------------------------------------------------------------
// XTI
// -----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxListCtrlStyle )
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

#if ((!defined(__WXMSW__) && !defined(__WXQT__) && !(defined(__WXMAC__) && wxOSX_USE_CARBON)) || defined(__WXUNIVERSAL__))
wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxListCtrl, wxGenericListCtrl, "wx/listctrl.h");
#else
wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxListCtrl, wxControl, "wx/listctrl.h");
#endif

wxBEGIN_PROPERTIES_TABLE(wxListCtrl)
wxEVENT_PROPERTY( TextUpdated, wxEVT_TEXT, wxCommandEvent )

wxPROPERTY_FLAGS( WindowStyle, wxListCtrlStyle, long, SetWindowStyleFlag, \
                 GetWindowStyleFlag, wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, \
                 wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxListCtrl)

wxCONSTRUCTOR_5( wxListCtrl, wxWindow*, Parent, wxWindowID, Id, \
                wxPoint, Position, wxSize, Size, long, WindowStyle )

/*
 TODO : Expose more information of a list's layout etc. via appropriate objects
 (see NotebookPageInfo)
 */

wxIMPLEMENT_DYNAMIC_CLASS(wxListView, wxListCtrl);
wxIMPLEMENT_DYNAMIC_CLASS(wxListItem, wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxListEvent, wxNotifyEvent);

// ----------------------------------------------------------------------------
// wxListCtrlBase implementation
// ----------------------------------------------------------------------------

long
wxListCtrlBase::AppendColumn(const wxString& heading,
                             wxListColumnFormat format,
                             int width)
{
    return InsertColumn(GetColumnCount(), heading, format, width);
}

long
wxListCtrlBase::InsertColumn(long col,
                             const wxString& heading,
                             int format,
                             int width)
{
    wxListItem item;
    item.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_FORMAT;
    item.m_text = heading;
    if ( width >= 0
            || width == wxLIST_AUTOSIZE
                || width == wxLIST_AUTOSIZE_USEHEADER )
    {
        item.m_mask |= wxLIST_MASK_WIDTH;
        item.m_width = width;
    }
    item.m_format = format;

    return InsertColumn(col, item);
}

long wxListCtrlBase::InsertColumn(long col, const wxListItem& info)
{
    long rc = DoInsertColumn(col, info);
    if ( rc != -1 )
    {
        // As our best size calculation depends on the column headers,
        // invalidate the previously cached best size when a column is added.
        InvalidateBestSize();
    }

    return rc;
}

wxSize wxListCtrlBase::DoGetBestClientSize() const
{
    // There is no obvious way to determine the best size in icon and list
    // modes so just don't do it for now.
    if ( !InReportView() )
        return wxControl::DoGetBestClientSize();

    int totalWidth;
    wxClientDC dc(const_cast<wxListCtrlBase*>(this));

    // In report mode, we use only the column headers, not items, to determine
    // the best width. The reason for this is that it's easier (we can't just
    // iterate over all items, especially not in a virtual control, so we'd
    // have to do something relatively complicated such as checking the size of
    // some items in the beginning and the end only) and also because the
    // columns are usually static while the list contents is dynamic so it
    // usually doesn't make much sense to adjust the control size to it anyhow.
    // And finally, scrollbars can always be used with the items while the
    // headers are just truncated if there is not enough place for them.
    const int columns = GetColumnCount();
    if ( HasFlag(wxLC_NO_HEADER) || !columns )
    {
        // Use some arbitrary width.
        totalWidth = 50*dc.GetCharWidth();
    }
    else // We do have columns, use them to determine the best width.
    {
        totalWidth = 0;
        for ( int col = 0; col < columns; col++ )
        {
            totalWidth += GetColumnWidth(col);
        }
    }

    // Use some arbitrary height, there is no good way to determine it.
    return wxSize(totalWidth, 10*dc.GetCharHeight());
}

void wxListCtrlBase::SetAlternateRowColour(const wxColour& colour)
{
    wxASSERT(HasFlag(wxLC_VIRTUAL));
    m_alternateRowColour.SetBackgroundColour(colour);
}

void wxListCtrlBase::EnableAlternateRowColours(bool enable)
{
    if ( enable )
    {
        // This code is copied from wxDataViewMainWindow::OnPaint()

        // Determine the alternate rows colour automatically from the
        // background colour.
        const wxColour bgColour = GetBackgroundColour();

        // Depending on the background, alternate row color
        // will be 3% more dark or 50% brighter.
        int alpha = bgColour.GetRGB() > 0x808080 ? 97 : 150;
        SetAlternateRowColour(bgColour.ChangeLightness(alpha));
    }
    else // Disable striping by setting invalid alternative colour.
    {
        SetAlternateRowColour(wxColour());
    }
}

wxItemAttr *wxListCtrlBase::OnGetItemAttr(long item) const
{
    return (m_alternateRowColour.GetBackgroundColour().IsOk() && (item % 2))
        ? wxConstCast(&m_alternateRowColour, wxItemAttr)
        : nullptr; // no attributes by default
}

wxString wxListCtrlBase::OnGetItemText(long WXUNUSED(item), long WXUNUSED(col)) const
{
    // this is a pure virtual function, in fact - which is not really pure
    // because the controls which are not virtual don't need to implement it
    wxFAIL_MSG("wxListCtrl::OnGetItemText not supposed to be called");

    return wxEmptyString;
}

bool wxListCtrlBase::OnGetItemIsChecked(long WXUNUSED(item)) const
{
    // this is a pure virtual function, in fact - which is not really pure
    // because the controls which are not virtual don't need to implement it
    wxFAIL_MSG("wxListCtrl::OnGetItemIsChecked not supposed to be called");

    return false;
}

int wxListCtrlBase::OnGetItemImage(long WXUNUSED(item)) const
{
    wxCHECK_MSG(!GetImageList(wxIMAGE_LIST_SMALL),
                -1,
                "List control has an image list, OnGetItemImage or OnGetItemColumnImage should be overridden.");
    return -1;
}

int wxListCtrlBase::OnGetItemColumnImage(long item, long column) const
{
    if ( !column )
        return OnGetItemImage(item);

    return -1;
}

wxItemAttr* wxListCtrlBase::OnGetItemColumnAttr(long item, long WXUNUSED(column)) const
{
    return OnGetItemAttr(item);
}

// ----------------------------------------------------------------------------
// Images support
// ----------------------------------------------------------------------------

void wxListCtrlBase::SetNormalImages(const wxVector<wxBitmapBundle>& images)
{
    m_imagesNormal.SetImages(images);

    DoUpdateImages(wxIMAGE_LIST_NORMAL);
}

void wxListCtrlBase::SetSmallImages(const wxVector<wxBitmapBundle>& images)
{
    m_imagesSmall.SetImages(images);

    DoUpdateImages(wxIMAGE_LIST_SMALL);
}

wxWithImages* wxListCtrlBase::GetImages(int which)
{
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        return &m_imagesNormal;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        return &m_imagesSmall;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        return &m_imagesState;
    }
    return nullptr;
}

const wxWithImages* wxListCtrlBase::GetImages(int which) const
{
    return const_cast<wxListCtrlBase*>(this)->GetImages(which);
}

wxImageList* wxListCtrlBase::GetUpdatedImageList(int which)
{
    wxWithImages* const images = GetImages(which);
    return images ? images->GetUpdatedImageListFor(this) : nullptr;
}

wxImageList *wxListCtrlBase::GetImageList(int which) const
{
    const wxWithImages* const images = GetImages(which);
    return images ? images->GetImageList() : nullptr;
}

void wxListCtrlBase::SetImageList(wxImageList *imageList, int which)
{
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        m_imagesNormal.SetImageList(imageList);
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        m_imagesSmall.SetImageList(imageList);
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        m_imagesState.SetImageList(imageList);
    }
    else
    {
        wxFAIL_MSG("unknown image list");
        return;
    }

    // Actually update the images shown in the control.
    DoUpdateImages(which);
}

void wxListCtrlBase::AssignImageList(wxImageList *imageList, int which)
{
    SetImageList(imageList, which);
    if ( which == wxIMAGE_LIST_NORMAL )
        m_imagesNormal.TakeOwnership();
    else if ( which == wxIMAGE_LIST_SMALL )
        m_imagesSmall.TakeOwnership();
    else if ( which == wxIMAGE_LIST_STATE )
        m_imagesState.TakeOwnership();
}

#endif // wxUSE_LISTCTRL
