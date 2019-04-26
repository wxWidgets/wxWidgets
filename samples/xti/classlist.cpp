/////////////////////////////////////////////////////////////////////////////
// Name:        classlist.cpp
// Purpose:     ClassListDialog implementation
// Author:      Francesco Montorsi
// Modified by:
// Created:     03/06/2007 14:49:55
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "classlist.h"

#if !wxUSE_EXTENDED_RTTI
    #error This sample requires XTI (eXtended RTTI) enabled
#endif

// wxIMPLEMENT_DYNAMIC_CLASS(ClassListDialog, wxDialog);  -- see the header
wxBEGIN_EVENT_TABLE( ClassListDialog, wxDialog )
    EVT_LISTBOX( ID_LISTBOX, ClassListDialog::OnListboxSelected )
    EVT_TREE_SEL_CHANGED( ID_TREECTRL, ClassListDialog::OnTreectrlSelChanged )
    EVT_CHOICEBOOK_PAGE_CHANGED( ID_LISTMODE, ClassListDialog::OnChoiceBookPageChange )

    EVT_CHECKBOX( ID_SHOW_ONLY_XTI, ClassListDialog::OnShowOnlyXTICheckbox )
    EVT_CHECKBOX( ID_SHOW_PROPERTIES_RECURSIVELY, ClassListDialog::OnShowRecursiveInfoCheckbox )
wxEND_EVENT_TABLE()

// defined later
wxString DumpClassInfo(const wxClassInfo*, bool recursive);


// ----------------------------------------------------------------------------
// ClassListDialog
// ----------------------------------------------------------------------------

ClassListDialog::ClassListDialog()
{
    Init();
}

ClassListDialog::ClassListDialog( wxWindow* parent, wxWindowID id,
                                      const wxString& caption, const wxPoint& pos,
                                      const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

bool ClassListDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
                                const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();

    return true;
}

ClassListDialog::~ClassListDialog()
{
}

void ClassListDialog::Init()
{
    m_pClassCountText = NULL;
    m_pRawListBox = NULL;
    m_pParentTreeCtrl = NULL;
    m_pSizeListBox = NULL;
    m_pTextCtrl = NULL;
}

void ClassListDialog::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC, _("This is the list of wxWidgets classes registered in the XTI system.\nNote that not all wxWidgets classes are registered nor all registered classes are completely _described_ using XTI metadata."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT|wxALL, 5);

    // filters
    wxBoxSizer* filters = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(filters, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    filters->Add(new wxCheckBox(this, ID_SHOW_ONLY_XTI,
                                "Show only classes with eXtended infos"));
    filters->AddSpacer(10);
    filters->Add(new wxCheckBox(this, ID_SHOW_PROPERTIES_RECURSIVELY,
                                "Show properties of parent classes"));

    // show how many have we filtered out
    m_pClassCountText = new wxStaticText( this, wxID_STATIC,
                "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                wxDefaultPosition, wxDefaultSize, 0 );
    m_pClassCountText->SetFont(wxFontInfo(8).Family(wxFONTFAMILY_SWISS).Bold());
    itemBoxSizer2->Add(m_pClassCountText, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 1, wxGROW, 5);

    m_pChoiceBook = new wxChoicebook( this, ID_LISTMODE, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );

    // raw-list page
    wxPanel* itemPanel7 = new wxPanel( m_pChoiceBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel7->SetSizer(itemBoxSizer8);

    wxArrayString m_pRawListBoxStrings;
    m_pRawListBox = new wxListBox( itemPanel7, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, m_pRawListBoxStrings, wxLB_SINGLE );
    itemBoxSizer8->Add(m_pRawListBox, 1, wxGROW, 5);

    m_pChoiceBook->AddPage(itemPanel7, _("Raw list"));

    // by-size page
    wxPanel* itemPanel13 = new wxPanel( m_pChoiceBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel13->SetSizer(itemBoxSizer14);

    wxArrayString m_pSizeListBoxStrings;
    m_pSizeListBox = new wxListBox( itemPanel13, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, m_pSizeListBoxStrings, wxLB_SINGLE );
    itemBoxSizer14->Add(m_pSizeListBox, 1, wxGROW, 5);

    m_pChoiceBook->AddPage(itemPanel13, _("Classes by size"));

    // tree page
    wxPanel* itemPanel10 = new wxPanel( m_pChoiceBook, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemPanel10->SetSizer(itemBoxSizer11);

    m_pParentTreeCtrl = new wxTreeCtrl( itemPanel10, ID_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_HAS_BUTTONS |wxTR_SINGLE );
    itemBoxSizer11->Add(m_pParentTreeCtrl, 1, wxGROW, 5);

    m_pChoiceBook->AddPage(itemPanel10, _("Classes by parent"));


    itemBoxSizer5->Add(m_pChoiceBook, 0, wxGROW|wxALL, 5);

    m_pTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, "", wxDefaultPosition, wxSize(500, -1), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer5->Add(m_pTextCtrl, 3, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer17 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer17, 0, wxGROW|wxALL, 5);
    wxButton* itemButton18 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer17->AddButton(itemButton18);

    wxButton* itemButton19 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer17->AddButton(itemButton19);

    itemStdDialogButtonSizer17->Realize();

    InitControls();
}

int ClassListDialog::AddClassesWithParent(const wxClassInfo *parent, const wxTreeItemId &id)
{
    const wxClassInfo *ci = wxClassInfo::GetFirst();
    int count = 0;
    while (ci)
    {
        // is this class derived from the given parent?
        if (wxString(ci->GetBaseClassName1()) == parent->GetClassName() ||
            wxString(ci->GetBaseClassName2()) == parent->GetClassName())
        {
            wxTreeItemId child = m_pParentTreeCtrl->AppendItem(id, ci->GetClassName());

            // update the name of this child with the count of the children classes
            int ret = AddClassesWithParent(ci, child);
            m_pParentTreeCtrl->SetItemText(child,
                                     m_pParentTreeCtrl->GetItemText(child) +
                                     wxString::Format(" [%d]", ret));
            count += ret+1;
        }

        ci = ci->GetNext();
    }

    // reorder all the children we've just added
    m_pParentTreeCtrl->SortChildren(id);

    return count;
}

int GetSizeOfClass(const wxString &cn)
{
    const wxClassInfo *ci = wxClassInfo::FindClass(cn);
    if (ci)
        return ci->GetSize();
    return 0;
}

int CompareClassSizes(const wxString &class1, const wxString &class2)
{
    return GetSizeOfClass(class1) - GetSizeOfClass(class2);
}

void ClassListDialog::InitControls()
{
    // create a wxArrayString with the names of all classes:
    const wxClassInfo *ci = wxClassInfo::GetFirst();
    wxArrayString arr;
    while (ci)
    {
        arr.Add(ci->GetClassName());
        ci = ci->GetNext();
    }

    arr.Sort();     // sort alphabetically

    // now add it to the raw-mode listbox
    for (unsigned int i=0; i<arr.GetCount(); i++)
        if (!IsToDiscard(arr[i]))
            m_pRawListBox->Append(arr[i]);
    m_nCount = m_pRawListBox->GetCount();

    // sort again using size as sortkey
    arr.Sort((wxArrayString::CompareFunction)CompareClassSizes);

    // now add it to the size-mode listbox
    for (unsigned int i=0; i<arr.GetCount(); i++)
        if (!IsToDiscard(arr[i]))
            m_pSizeListBox->Append(arr[i]);

    // add root item to parent-mode treectrl
    wxTreeItemId id = m_pParentTreeCtrl->AddRoot("wxObject");

    // recursively add all leaves to the treectrl
    int count = AddClassesWithParent(CLASSINFO(wxObject), id);
    m_pParentTreeCtrl->SetItemText(id, m_pParentTreeCtrl->GetItemText(id) +
                                 wxString::Format(" [%d]", count));

    // initially expand the root item
    m_pParentTreeCtrl->Expand(id);

    m_nTotalCount = arr.GetCount();
    UpdateFilterText();

    // don't leave blank the XTI info display
    m_pChoiceBook->ChangeSelection(0);
    m_pRawListBox->Select(0);
    UpdateClassInfo(m_pRawListBox->GetStringSelection());
}

bool ClassListDialog::IsToDiscard(const wxString &classname) const
{
    wxCheckBox *cb = static_cast<wxCheckBox*>(FindWindow(ID_SHOW_ONLY_XTI));
    if (!cb || !cb->IsChecked())
        return false;

    // check if this class has XTI infos
    wxClassInfo *info = wxClassInfo::FindClass(classname);
    if (!info)
        return false;
    if (info->GetFirstProperty() != NULL || info->GetFirstHandler() != NULL)
        return false;       // has XTI info
    return true;            // no XTI info
}

void ClassListDialog::UpdateFilterText()
{
    // tell the user how many registered classes are present and
    // how many are we showing
    m_pClassCountText->SetLabel(
        wxString::Format(
            "Showing %d classes on a total of %d registered classes in wxXTI.",
            m_nCount, m_nTotalCount));
}

void ClassListDialog::UpdateClassInfo(const wxString &itemName)
{
    wxString classname = itemName.BeforeFirst(' ');
    wxCheckBox *cb = static_cast<wxCheckBox*>(FindWindow(ID_SHOW_PROPERTIES_RECURSIVELY));

    m_pTextCtrl->SetValue(
        DumpClassInfo(wxClassInfo::FindClass(classname), cb->IsChecked()));
}


// ----------------------------------------------------------------------------
// ClassListDialog - event handlers
// ----------------------------------------------------------------------------

void ClassListDialog::OnShowOnlyXTICheckbox( wxCommandEvent& WXUNUSED(event) )
{
    m_pRawListBox->Clear();
    m_pParentTreeCtrl->DeleteAllItems();
    m_pSizeListBox->Clear();

    InitControls();
}

void ClassListDialog::OnShowRecursiveInfoCheckbox( wxCommandEvent& WXUNUSED(event) )
{
    m_pRawListBox->Clear();
    m_pParentTreeCtrl->DeleteAllItems();
    m_pSizeListBox->Clear();

    InitControls();
}

void ClassListDialog::OnListboxSelected( wxCommandEvent& event )
{
    UpdateClassInfo(event.GetString());
}

void ClassListDialog::OnTreectrlSelChanged( wxTreeEvent& event )
{
    UpdateClassInfo(m_pParentTreeCtrl->GetItemText(event.GetItem()));
}

void ClassListDialog::OnChoiceBookPageChange( wxChoicebookEvent& event )
{
    switch (event.GetSelection())
    {
    case 0:
        if (m_pRawListBox->GetCount())
        {
            m_pRawListBox->Select(0);
            UpdateClassInfo(m_pRawListBox->GetStringSelection());
        }
        break;
    case 1:
        if (m_pSizeListBox->GetCount())
        {
            m_pSizeListBox->Select(0);
            UpdateClassInfo(m_pSizeListBox->GetStringSelection());
        }
        break;
    case 2:
        {
            wxTreeItemId root = m_pParentTreeCtrl->GetRootItem();
            if (root.IsOk())
            {
                m_pParentTreeCtrl->SelectItem(root);
                UpdateClassInfo(m_pParentTreeCtrl->GetItemText(root));
            }
        }
        break;
    }
}



// ----------------------------------------------------------------------------
// dump functions
// ----------------------------------------------------------------------------

wxString DumpStr(const wxString &str)
{
    if (str.empty())
        return "none";
    return str;
}

wxString DumpTypeInfo(const wxTypeInfo *ti)
{
    if (!ti)
        return "none";

    return DumpStr(ti->GetTypeName());
}

wxString DumpPropertyAccessor(const wxPropertyAccessor *acc, int indent)
{
    wxString ind = "\n" + wxString(indent, ' ');
    wxString infostr;

    if (!acc)
        return ind + "no property accessors";

    if (acc->HasSetter())
        infostr << ind << "setter name: " << acc->GetSetterName();
    if (acc->HasCollectionGetter())
        infostr << ind << "collection getter name: " << acc->GetCollectionGetterName();
    if (acc->HasGetter())
        infostr << ind << "getter name: " << acc->GetGetterName();
    if (acc->HasAdder())
        infostr << ind << "adder name: " << acc->GetAdderName();

    return infostr;
}

wxString DumpPropertyInfo(const wxPropertyInfo *prop, int indent)
{
    wxString ind = "\n" + wxString(indent, ' ');
    wxString infostr;

    if (!prop)
        return ind + "none";

    infostr << ind << "flags: ";
    if (prop->GetFlags() & wxPROP_DEPRECATED)
        infostr << "wxPROP_DEPRECATED,";
    if (prop->GetFlags() & wxPROP_OBJECT_GRAPH)
        infostr << "wxPROP_OBJECT_GRAPH,";
    if (prop->GetFlags() & wxPROP_ENUM_STORE_LONG)
        infostr << "wxPROP_ENUM_STORE_LONG,";
    if (prop->GetFlags() & wxPROP_DONT_STREAM)
        infostr << "wxPROP_DONT_STREAM,";

    if (prop->GetFlags() == 0)
        infostr << "none";
    else
        infostr.RemoveLast();       // remove last comma

    infostr << ind << "help string: " << DumpStr(prop->GetHelpString());
    infostr << ind << "group string: " << DumpStr(prop->GetGroupString());

    infostr << ind << "collection element type: " << DumpTypeInfo(prop->GetCollectionElementTypeInfo());
    infostr << ind << "type: " << DumpTypeInfo(prop->GetTypeInfo());

    infostr << ind << "default value: " << DumpStr(wxAnyGetAsString(prop->GetDefaultValue()));
    infostr << DumpPropertyAccessor(prop->GetAccessor(), indent+1);

    return infostr;
}

wxString DumpHandlerInfo(const wxHandlerInfo *phdlr, int indent)
{
    wxString ind = "\n" + wxString(indent, ' ');
    wxString infostr;

    if (!phdlr)
        return ind + "none";

    infostr << ind << "event class: " <<
        (phdlr->GetEventClassInfo() ? phdlr->GetEventClassInfo()->GetClassName() : "none");

    return infostr;
}

int DumpProperties(const wxClassInfo *info, wxString& infostr, bool recursive)
{
    const wxPropertyInfo *prop;
    int pcount;
    for (prop = info->GetFirstProperty(), pcount = 0;
         prop;
         prop = prop->GetNext(), pcount++)
    {
        infostr << "\n\n  [" << pcount+1 << "] Property: " << prop->GetName();
        infostr << DumpPropertyInfo(prop, 4);
    }

    if (pcount == 0)
        infostr << "\n None";

    if (recursive)
    {
        const wxClassInfo **parent = info->GetParents();
        wxString str;

        for (int i=0; parent[i] != NULL; i++)
        {
            int ppcount = DumpProperties(parent[i], str, recursive);
            if (ppcount)
            {
                pcount += ppcount;
                infostr << "\n\n  " << parent[i]->GetClassName() << " PARENT'S PROPERTIES:";
                infostr << str;
            }
        }
    }

    return pcount;
}

int DumpHandlers(const wxClassInfo *info, wxString& infostr, bool recursive)
{
    const wxHandlerInfo *h;
    int hcount;
    for (h = info->GetFirstHandler(), hcount = 0;
         h;
         h = h->GetNext(), hcount++)
    {
        infostr << "\n\n  [" << hcount+1 << "] Handler: " << h->GetName();
        infostr << DumpHandlerInfo(h, 4);
    }

    if (hcount == 0)
        infostr << "\n None";

    if (recursive)
    {
        const wxClassInfo **parent = info->GetParents();
        wxString str;

        for (int i=0; parent[i] != NULL; i++)
        {
            int hhcount = DumpHandlers(parent[i], str, recursive);
            if (hhcount)
            {
                hcount += hhcount;
                infostr << "\n\n  " << parent[i]->GetClassName() << " PARENT'S HANDLERS:";
                infostr << str;
            }
        }
    }

    return hcount;
}

wxString DumpClassInfo(const wxClassInfo *info, bool recursive)
{
    wxString infostr;

    if (!info)
        return wxEmptyString;

    // basic stuff:

    infostr << "\n BASIC RTTI INFO ABOUT " << info->GetClassName();
    infostr << "\n =================================================";
    infostr << "\n  Base class #1: " << DumpStr(info->GetBaseClassName1());
    infostr << "\n  Base class #2: " << DumpStr(info->GetBaseClassName2());
    infostr << "\n  Include file: " << DumpStr(info->GetIncludeName());
    infostr << "\n  Size: " << info->GetSize();
    infostr << "\n  Dynamic: " << (info->IsDynamic() ? "true" : "false");


    // advanced stuff:

    infostr << "\n\n\n ADVANCED RTTI INFO ABOUT " << info->GetClassName();
    infostr << "\n =================================================\n";
    infostr << "\n PROPERTIES";
    infostr << "\n -----------------------------------------";
    int pcount = DumpProperties(info, infostr, recursive);
    infostr << "\n\n HANDLERS";
    infostr << "\n -----------------------------------------";
    int hcount = DumpHandlers(info, infostr, recursive);

    if (pcount+hcount == 0)
        infostr << "\n\n no advanced info\n";
    else
    {
        infostr << "\n\n Total count of properties: " << pcount;
        infostr << "\n Total count of handlers: " << hcount << "\n";
    }

    return infostr;
}
