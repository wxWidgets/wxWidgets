/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/editlbox.cpp
// Purpose:     ListBox with editable items
// Author:      Vaclav Slavik
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_EDITABLELISTBOX

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/editlbox.h"
#include "wx/sizer.h"
#include "wx/listctrl.h"
#include "wx/artprov.h"
#include "wx/weakref.h"

namespace
{
bool IsEditableListCurrent(const wxWeakRef<wxEditableListBox>& owner,
                           const wxWeakRef<wxListCtrl>& list)
{
    return owner && list && !owner->IsBeingDeleted() && !list->IsBeingDeleted() &&
           owner->GetListCtrl() == list.get() && list->GetParent() == owner.get();
}
}

// ============================================================================
// implementation
// ============================================================================

const char wxEditableListBoxNameStr[] = "editableListBox";

// list control with auto-resizable column:
class CleverListCtrl : public wxListCtrl
{
public:
   CleverListCtrl(wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLC_ICON,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString &name = wxListCtrlNameStr)
         : wxListCtrl(parent, id, pos, size, style, validator, name)
    {
        CreateColumns();
    }

    void CreateColumns()
    {
        InsertColumn(0, wxT("item"));
        SizeColumns();
    }

    void SizeColumns()
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
         // The scrollbar is drawn over the contents and takes no room: the
         // only column is as wide as the rows, and so is the editor of an
         // item.
         int w = GetClientSize().x;
#elif defined(__WXMSW__)
         int w = GetSize().x;
         w -= GetScrollbarSize(wxVERTICAL) + 6;
#else
         int w = GetSize().x;
         w -= 2*GetScrollbarSize(wxVERTICAL);
#endif
         if (w < 0) w = 0;
         SetColumnWidth(0, w);
    }

private:
    wxDECLARE_EVENT_TABLE();
    void OnSize(wxSizeEvent& event)
    {
        SizeColumns();
        event.Skip();
    }
};

wxBEGIN_EVENT_TABLE(CleverListCtrl, wxListCtrl)
   EVT_SIZE(CleverListCtrl::OnSize)
wxEND_EVENT_TABLE()


// ----------------------------------------------------------------------------
// wxEditableListBox
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxEditableListBox, wxPanel);

// NB: generate the IDs at runtime to avoid conflict with XRCID values,
//     they could cause XRCCTRL() failures in XRC-based dialogs
const wxWindowIDRef wxID_ELB_DELETE = wxWindow::NewControlId();
const wxWindowIDRef wxID_ELB_EDIT = wxWindow::NewControlId();
const wxWindowIDRef wxID_ELB_NEW = wxWindow::NewControlId();
const wxWindowIDRef wxID_ELB_UP = wxWindow::NewControlId();
const wxWindowIDRef wxID_ELB_DOWN = wxWindow::NewControlId();
const wxWindowIDRef wxID_ELB_LISTCTRL = wxWindow::NewControlId();

#if defined(__WXWINUI__) && wxUSE_WINUI3

// The buttons show glyphs of the icon font of Windows instead of the stock
// bitmaps: they are drawn like the icons of every WinUI control, in the
// colour of the text of the theme, and dim with the button.
static wxBitmapButton *
wxELBMakeButton(wxWindow *parent, wxWindowID id, wxUniChar glyph)
{
    wxBitmapButton * const button = new wxBitmapButton(
        parent, id, wxBitmapBundle(), wxDefaultPosition,
        // Wide enough for the glyph between the margins a WinUI button
        // keeps on both sides of its content.
        parent->FromDIP(wxSize(40, 32)), wxBORDER_NONE);

    // A wxBitmapButton is created as a button without text: this one has
    // one, the glyph, and no bitmap.
    button->SetWindowStyleFlag(button->GetWindowStyleFlag() & ~wxBU_NOTEXT);

    wxFont font = button->GetFont();
    if ( !font.SetFaceName("Segoe Fluent Icons") )
        font.SetFaceName("Segoe MDL2 Assets");
    font.Scale(1.15f);
    button->SetFont(font);
    button->SetLabel(wxString(glyph));
    return button;
}

#endif // __WXWINUI__

wxBEGIN_EVENT_TABLE(wxEditableListBox, wxPanel)
    EVT_LIST_ITEM_SELECTED(wxID_ELB_LISTCTRL, wxEditableListBox::OnItemSelected)
    EVT_LIST_END_LABEL_EDIT(wxID_ELB_LISTCTRL, wxEditableListBox::OnEndLabelEdit)
    EVT_BUTTON(wxID_ELB_NEW, wxEditableListBox::OnNewItem)
    EVT_BUTTON(wxID_ELB_UP, wxEditableListBox::OnUpItem)
    EVT_BUTTON(wxID_ELB_DOWN, wxEditableListBox::OnDownItem)
    EVT_BUTTON(wxID_ELB_EDIT, wxEditableListBox::OnEditItem)
    EVT_BUTTON(wxID_ELB_DELETE, wxEditableListBox::OnDelItem)
wxEND_EVENT_TABLE()

bool wxEditableListBox::Create(wxWindow *parent, wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos, const wxSize& size,
                          long style,
                          const wxString& name)
{
    if (!wxPanel::Create(parent, id, pos, size, wxTAB_TRAVERSAL, name))
        return false;

    m_style = style;

    // wxEL_ALLOW_NEW requires ability to edit labels so implicitly add wxEL_ALLOW_EDIT
    if ( m_style & wxEL_ALLOW_NEW )
      m_style |= wxEL_ALLOW_EDIT;

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // No sunken edges around the parts of a WinUI control.
    const long borderStyle = wxBORDER_NONE;
#else
    const long borderStyle = wxSUNKEN_BORDER;
#endif

    wxPanel *subp = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                borderStyle | wxTAB_TRAVERSAL);
    wxSizer *subsizer = new wxBoxSizer(wxHORIZONTAL);

    subsizer->Add(new wxStaticText(subp, wxID_ANY, label),
                  wxSizerFlags(1).Center().Border(wxLEFT));

    const wxSizerFlags flagsCentered = wxSizerFlags().Center();

    if ( m_style & wxEL_ALLOW_EDIT )
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        m_bEdit = wxELBMakeButton(subp, wxID_ELB_EDIT, wxUniChar(0xE70F));
#else
        m_bEdit = new wxBitmapButton(subp, wxID_ELB_EDIT,
                                     wxArtProvider::GetBitmap(wxART_EDIT, wxART_BUTTON));
#endif
        m_bEdit->SetToolTip(_("Edit item"));
        subsizer->Add(m_bEdit, flagsCentered);
    }

    if ( m_style & wxEL_ALLOW_NEW )
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        m_bNew = wxELBMakeButton(subp, wxID_ELB_NEW, wxUniChar(0xE710));
#else
        m_bNew = new wxBitmapButton(subp, wxID_ELB_NEW,
                                    wxArtProvider::GetBitmap(wxART_NEW, wxART_BUTTON));
#endif
        m_bNew->SetToolTip(_("New item"));
        subsizer->Add(m_bNew, flagsCentered);
    }

    if ( m_style & wxEL_ALLOW_DELETE )
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        m_bDel = wxELBMakeButton(subp, wxID_ELB_DELETE, wxUniChar(0xE74D));
#else
        m_bDel = new wxBitmapButton(subp, wxID_ELB_DELETE,
                                    wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON));
#endif
        m_bDel->SetToolTip(_("Delete item"));
        subsizer->Add(m_bDel, flagsCentered);
    }

    if (!(m_style & wxEL_NO_REORDER))
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        m_bUp = wxELBMakeButton(subp, wxID_ELB_UP, wxUniChar(0xE74A));
#else
        m_bUp = new wxBitmapButton(subp, wxID_ELB_UP,
                                   wxArtProvider::GetBitmap(wxART_GO_UP, wxART_BUTTON));
#endif
        m_bUp->SetToolTip(_("Move up"));
        subsizer->Add(m_bUp, flagsCentered);

#if defined(__WXWINUI__) && wxUSE_WINUI3
        m_bDown = wxELBMakeButton(subp, wxID_ELB_DOWN, wxUniChar(0xE74B));
#else
        m_bDown = new wxBitmapButton(subp, wxID_ELB_DOWN,
                                     wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_BUTTON));
#endif
        m_bDown->SetToolTip(_("Move down"));
        subsizer->Add(m_bDown, flagsCentered);
    }

    subp->SetSizer(subsizer);
    subsizer->Fit(subp);

    sizer->Add(subp, wxSizerFlags().Expand());

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The list has the frame of a control of WinUI, a little under the row
    // of its commands, which are quiet buttons as those of a command bar.
    sizer->AddSpacer(FromDIP(4));
    long st = wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxBORDER_THEME;
#else
    long st = wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | borderStyle;
#endif
    if ( m_style & wxEL_ALLOW_EDIT )
         st |= wxLC_EDIT_LABELS;
    m_listCtrl = new CleverListCtrl(this, wxID_ELB_LISTCTRL,
                                    wxDefaultPosition, wxDefaultSize, st);
    wxArrayString empty_ar;
    SetStrings(empty_ar);

    sizer->Add(m_listCtrl, wxSizerFlags(1).Expand());

    SetSizer(sizer);
    Layout();

    return true;
}

void wxEditableListBox::SetStrings(const wxArrayString& strings)
{
    const wxWeakRef<wxEditableListBox> owner(this);
    const wxWeakRef<wxListCtrl> list(m_listCtrl);
    if ( !IsEditableListCurrent(owner, list) || !list->DeleteAllItems() ||
         !IsEditableListCurrent(owner, list) )
        return;
    size_t i;

    for (i = 0; i < strings.GetCount(); i++)
    {
        const long inserted = list->InsertItem(i, strings[i]);
        if ( !IsEditableListCurrent(owner, list) || inserted == -1 )
            return;
    }

    const long inserted = list->InsertItem(strings.GetCount(), wxEmptyString);
    if ( !IsEditableListCurrent(owner, list) || inserted == -1 )
        return;
    list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxEditableListBox::GetStrings(wxArrayString& strings) const
{
    strings.Clear();

    for (int i = 0; i < m_listCtrl->GetItemCount()-1; i++)
        strings.Add(m_listCtrl->GetItemText(i));
}

void wxEditableListBox::OnItemSelected(wxListEvent& event)
{
    m_selection = event.GetIndex();
    if (!(m_style & wxEL_NO_REORDER))
    {
        m_bUp->Enable(m_selection != 0 && m_selection < m_listCtrl->GetItemCount()-1);
        m_bDown->Enable(m_selection < m_listCtrl->GetItemCount()-2);
    }

    if (m_style & wxEL_ALLOW_EDIT)
        m_bEdit->Enable(m_selection < m_listCtrl->GetItemCount()-1);
    if (m_style & wxEL_ALLOW_DELETE)
        m_bDel->Enable(m_selection < m_listCtrl->GetItemCount()-1);
}

void wxEditableListBox::OnNewItem(wxCommandEvent& WXUNUSED(event))
{
    const wxWeakRef<wxEditableListBox> owner(this);
    const wxWeakRef<wxListCtrl> list(m_listCtrl);
    if ( !IsEditableListCurrent(owner, list) || list->GetItemCount() == 0 )
        return;
    list->SetItemState(list->GetItemCount()-1,
                       wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    // Selection handlers may destroy either the composite or its list.
    if ( !IsEditableListCurrent(owner, list) ||
         m_selection < 0 || m_selection >= list->GetItemCount() )
        return;
    list->EditLabel(m_selection);
}

void wxEditableListBox::OnEndLabelEdit(wxListEvent& event)
{
    if ( event.GetIndex() == m_listCtrl->GetItemCount()-1 &&
         !event.GetText().empty() )
    {
        const wxWeakRef<wxEditableListBox> owner(this);
        const wxWeakRef<wxListCtrl> list(m_listCtrl);
        // The user edited last (empty) line, i.e. added new entry. We have to
        // add new empty line here so that adding one more line is still
        // possible:
        const long inserted = list->InsertItem(list->GetItemCount(), wxEmptyString);
        if ( !IsEditableListCurrent(owner, list) || inserted == -1 )
            return;

        // Simulate a wxEVT_LIST_ITEM_SELECTED event for the new item,
        // so that the buttons are enabled/disabled properly
        wxListEvent selectionEvent(wxEVT_LIST_ITEM_SELECTED, list->GetId());
        selectionEvent.SetEventObject(list.get());
        selectionEvent.m_itemIndex = event.GetIndex();
        list->GetEventHandler()->ProcessEvent(selectionEvent);
    }
}

void wxEditableListBox::OnDelItem(wxCommandEvent& WXUNUSED(event))
{
    const wxWeakRef<wxEditableListBox> owner(this);
    const wxWeakRef<wxListCtrl> list(m_listCtrl);
    if ( !IsEditableListCurrent(owner, list) || m_selection < 0 ||
         m_selection >= list->GetItemCount()-1 )
        return;
    if ( !list->DeleteItem(m_selection) || !IsEditableListCurrent(owner, list) ||
         m_selection < 0 || list->GetItemCount() == 0 )
        return;
    list->SetItemState(wxMin(m_selection, list->GetItemCount()-1),
                       wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxEditableListBox::OnEditItem(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->EditLabel(m_selection);
}

void wxEditableListBox::SwapItems(long i1, long i2)
{
    // swap the text
    wxString t1 = m_listCtrl->GetItemText(i1);
    wxString t2 = m_listCtrl->GetItemText(i2);
    m_listCtrl->SetItemText(i1, t2);
    m_listCtrl->SetItemText(i2, t1);

    // swap the item data
    wxUIntPtr d1 = m_listCtrl->GetItemData(i1);
    wxUIntPtr d2 = m_listCtrl->GetItemData(i2);
    m_listCtrl->SetItemPtrData(i1, d2);
    m_listCtrl->SetItemPtrData(i2, d1);
}


void wxEditableListBox::OnUpItem(wxCommandEvent& WXUNUSED(event))
{
    SwapItems(m_selection - 1, m_selection);
    m_listCtrl->SetItemState(m_selection - 1,
                             wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxEditableListBox::OnDownItem(wxCommandEvent& WXUNUSED(event))
{
    SwapItems(m_selection + 1, m_selection);
    m_listCtrl->SetItemState(m_selection + 1,
                             wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

#endif // wxUSE_EDITABLELISTBOX
