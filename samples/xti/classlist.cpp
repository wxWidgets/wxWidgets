/////////////////////////////////////////////////////////////////////////////
// Name:        classlist.cpp
// Purpose:     ClassListDialog implementation
// Author:      Francesco Montorsi
// Modified by: 
// Created:     03/06/2007 14:49:55
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWidgets license
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

// IMPLEMENT_DYNAMIC_CLASS( ClassListDialog, wxDialog )  -- see the header
BEGIN_EVENT_TABLE( ClassListDialog, wxDialog )
    EVT_LISTBOX( ID_LISTBOX, ClassListDialog::OnListboxSelected )
    EVT_TREE_SEL_CHANGED( ID_TREECTRL, ClassListDialog::OnTreectrlSelChanged )
END_EVENT_TABLE()


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
    ClassListDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, _("This is the list of wxWidgets classes registered in the XTI system.\nNote that not all wxWidgets classes are registered nor all registered classes are completely _described_ using XTI metadata."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT|wxALL, 5);

    m_pClassCountText = new wxStaticText( itemDialog1, wxID_STATIC, _("There are _RUNTIME_COMPUTED_ classes registered."), wxDefaultPosition, wxDefaultSize, 0 );
    m_pClassCountText->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));
    itemBoxSizer2->Add(m_pClassCountText, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer5, 1, wxGROW, 5);

    wxChoicebook* itemChoicebook6 = new wxChoicebook( itemDialog1, ID_LISTMODE, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );

    wxPanel* itemPanel7 = new wxPanel( itemChoicebook6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel7->SetSizer(itemBoxSizer8);

    wxArrayString m_pRawListBoxStrings;
    m_pRawListBox = new wxListBox( itemPanel7, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, m_pRawListBoxStrings, wxLB_SINGLE );
    itemBoxSizer8->Add(m_pRawListBox, 1, wxGROW, 5);

    itemChoicebook6->AddPage(itemPanel7, _("Raw list"));

    wxPanel* itemPanel10 = new wxPanel( itemChoicebook6, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    itemPanel10->SetSizer(itemBoxSizer11);

    m_pParentTreeCtrl = new wxTreeCtrl( itemPanel10, ID_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_HAS_BUTTONS |wxTR_SINGLE );
    itemBoxSizer11->Add(m_pParentTreeCtrl, 1, wxGROW, 5);

    itemChoicebook6->AddPage(itemPanel10, _("Classes by parent"));

    wxPanel* itemPanel13 = new wxPanel( itemChoicebook6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel13->SetSizer(itemBoxSizer14);

    wxArrayString m_pSizeListBoxStrings;
    m_pSizeListBox = new wxListBox( itemPanel13, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, m_pSizeListBoxStrings, wxLB_SINGLE );
    itemBoxSizer14->Add(m_pSizeListBox, 1, wxGROW, 5);

    itemChoicebook6->AddPage(itemPanel13, _("Classes by size"));

    itemBoxSizer5->Add(itemChoicebook6, 0, wxGROW|wxALL, 5);

    m_pTextCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(500, -1), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer5->Add(m_pTextCtrl, 3, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer17 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer17, 0, wxGROW|wxALL, 5);
    wxButton* itemButton18 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer17->AddButton(itemButton18);

    wxButton* itemButton19 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
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
                                     wxString::Format(wxT(" [%d]"), ret));
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
        m_pRawListBox->Append(arr[i]);

    // sort again using size as sortkey
    arr.Sort((wxArrayString::CompareFunction)CompareClassSizes);

    // now add it to the size-mode listbox
    for (unsigned int i=0; i<arr.GetCount(); i++)
        m_pSizeListBox->Append(arr[i]);

    // add root item to parent-mode treectrl
    wxTreeItemId id = m_pParentTreeCtrl->AddRoot(_T("wxObject"));

    // recursively add all leaves to the treectrl
    int count = AddClassesWithParent(CLASSINFO(wxObject), id);
    m_pParentTreeCtrl->SetItemText(id, m_pParentTreeCtrl->GetItemText(id) +
                                 wxString::Format(wxT(" [%d]"), count));

    // initially expand the root item
    m_pParentTreeCtrl->Expand(id);

    // last, tell the user how many registered classes are present
    m_pClassCountText->SetLabel(
        wxString::Format(wxT("There are %d classes registered in wxXTI."), arr.GetCount()));
}


// ----------------------------------------------------------------------------
// ClassListDialog - event handlers
// ----------------------------------------------------------------------------

// defined later
wxString DumpClassInfo(const wxClassInfo*);

void ClassListDialog::OnListboxSelected( wxCommandEvent& event )
{
    // get info about the selected class
    wxString classname = event.GetString().BeforeFirst(_T(' '));
    m_pTextCtrl->SetValue(DumpClassInfo(wxClassInfo::FindClass(classname)));
}

void ClassListDialog::OnTreectrlSelChanged( wxTreeEvent& event )
{
    // get info about the selected class
    wxString classname = m_pParentTreeCtrl->GetItemText(event.GetItem()).BeforeFirst(_T(' '));
    m_pTextCtrl->SetValue(DumpClassInfo(wxClassInfo::FindClass(classname)));
}



// ----------------------------------------------------------------------------
// dump functions
// ----------------------------------------------------------------------------

wxString DumpStr(const wxString &str)
{
    if (str.empty())
        return wxT("none");
    return str;
}

wxString DumpTypeInfo(const wxTypeInfo *ti)
{
    if (!ti)
        return _T("none");

    return DumpStr(ti->GetTypeName());
}

wxString DumpPropertyAccessor(const wxPropertyAccessor *acc, int indent)
{
    wxString ind = _T("\n") + wxString(indent, wxT(' '));
    wxString infostr;

    if (!acc)
        return ind + _T("no property accessors");
    
    if (acc->HasSetter())
        infostr << ind << _T("setter name: ") << acc->GetSetterName();
    if (acc->HasCollectionGetter())
        infostr << ind << _T("collection getter name: ") << acc->GetCollectionGetterName();
    if (acc->HasGetter())
        infostr << ind << _T("getter name: ") << acc->GetGetterName();
    if (acc->HasAdder())
        infostr << ind << _T("adder name: ") << acc->GetAdderName();

    return infostr;
}

wxString DumpPropertyInfo(const wxPropertyInfo *prop, int indent)
{
    wxString ind = _T("\n") + wxString(indent, wxT(' '));
    wxString infostr;

    if (!prop)
        return ind + _T("none");

    infostr << ind << _T("flags: ");
    if (prop->GetFlags() & wxPROP_DEPRECATED)
        infostr << _T("wxPROP_DEPRECATED,");
    if (prop->GetFlags() & wxPROP_OBJECT_GRAPH)
        infostr << _T("wxPROP_OBJECT_GRAPH,");
    if (prop->GetFlags() & wxPROP_ENUM_STORE_LONG)
        infostr << _T("wxPROP_ENUM_STORE_LONG,");
    if (prop->GetFlags() & wxPROP_DONT_STREAM)
        infostr << _T("wxPROP_DONT_STREAM,");

    if (prop->GetFlags() == 0)
        infostr << _T("none");
    else
        infostr.RemoveLast();       // remove last comma

    infostr << ind << _T("help string: ") << DumpStr(prop->GetHelpString());
    infostr << ind << _T("group string: ") << DumpStr(prop->GetGroupString());

    infostr << ind << _T("collection element type: ") << DumpTypeInfo(prop->GetCollectionElementTypeInfo());
    infostr << ind << _T("type: ") << DumpTypeInfo(prop->GetTypeInfo());

    infostr << ind << _T("default value: ") << DumpStr(prop->GetDefaultValue().GetAsString());
    infostr << DumpPropertyAccessor(prop->GetAccessor(), indent+1);

    return infostr;
}

wxString DumpHandlerInfo(const wxHandlerInfo *phdlr, int indent)
{
    wxString ind = _T("\n") + wxString(indent, wxT(' '));
    wxString infostr;

    if (!phdlr)
        return ind + _T("none");

    infostr << ind << _T("event class: ") << 
        (phdlr->GetEventClassInfo() ? phdlr->GetEventClassInfo()->GetClassName() : wxT("none"));

    return infostr;
}

wxString DumpClassInfo(const wxClassInfo *info)
{
    wxString infostr;

    if (!info)
        return wxEmptyString;

    // basic stuff:

    infostr << _T("\n BASIC RTTI INFO ABOUT ") << info->GetClassName();
    infostr << _T("\n ---------------------------------------------------------");
    infostr << _T("\n  Base class #1: ") << DumpStr(info->GetBaseClassName1());
    infostr << _T("\n  Base class #2: ") << DumpStr(info->GetBaseClassName2());
    infostr << _T("\n  Include file: ") << DumpStr(info->GetIncludeName());
    infostr << _T("\n  Size: ") << info->GetSize();
    infostr << _T("\n  Dynamic: ") << (info->IsDynamic() ? _T("true") : _T("false"));


    // advanced stuff:

    infostr << _T("\n\n\n ADVANCED RTTI INFO ABOUT ") << info->GetClassName();
    infostr << _T("\n ---------------------------------------------------------");

    const wxPropertyInfo *prop;
    int pcount;
    for (prop = info->GetFirstProperty(), pcount = 0;
         prop;
         prop = prop->GetNext(), pcount++)
    {
        infostr << _T("\n\n  [") << pcount+1 << _T("] Property: ") << prop->GetName();
        infostr << DumpPropertyInfo(prop, 4);
    }

    infostr << _T("\n");

    const wxHandlerInfo *h;
    int hcount;
    for (h = info->GetFirstHandler(), hcount = 0;
         h;
         h = h->GetNext(), hcount++)
    {
        infostr << _T("\n\n  [") << hcount+1 << _T("] Handler: ") << h->GetName();
        infostr << DumpHandlerInfo(h, 4);
    }

    if (pcount+hcount == 0)
        infostr << _T("\n no advanced info\n");

    return infostr;
}
