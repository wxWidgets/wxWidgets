/////////////////////////////////////////////////////////////////////////////
// Name:        classlist.cpp
// Purpose:     wxClassListDialog implementation
// Author:      Francesco Montorsi
// Modified by: 
// Created:     03/06/2007 14:49:55
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWidgets license
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// includes

#include "classlist.h"


IMPLEMENT_DYNAMIC_CLASS( wxClassListDialog, wxDialog )
BEGIN_EVENT_TABLE( wxClassListDialog, wxDialog )

////@begin wxClassListDialog event table entries
    EVT_LISTBOX( ID_LISTBOX, wxClassListDialog::OnListboxSelected )

    EVT_TREE_SEL_CHANGED( ID_TREECTRL, wxClassListDialog::OnTreectrlSelChanged )

////@end wxClassListDialog event table entries
END_EVENT_TABLE()


// ----------------------------------------------------------------------------
// wxClassListDialog
// ----------------------------------------------------------------------------

wxClassListDialog::wxClassListDialog()
{
    Init();
}

wxClassListDialog::wxClassListDialog( wxWindow* parent, wxWindowID id, 
                                      const wxString& caption, const wxPoint& pos, 
                                      const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

bool wxClassListDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, 
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

wxClassListDialog::~wxClassListDialog()
{
}

void wxClassListDialog::Init()
{
////@begin wxClassListDialog member initialisation
    m_pListBox = NULL;
    m_pTreeCtrl = NULL;
    m_pTextCtrl = NULL;
////@end wxClassListDialog member initialisation
}

void wxClassListDialog::CreateControls()
{    
////@begin wxClassListDialog content construction
    wxClassListDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( itemDialog1, wxID_STATIC, _("This is the list of wxWidgets classes registered in the XTI system:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 1, wxGROW, 5);

    wxChoicebook* itemChoicebook5 = new wxChoicebook( itemDialog1, ID_LISTMODE, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );

    wxPanel* itemPanel6 = new wxPanel( itemChoicebook5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel6->SetSizer(itemBoxSizer7);

    wxArrayString m_pListBoxStrings;
    m_pListBox = new wxListBox( itemPanel6, ID_LISTBOX, wxDefaultPosition, wxDefaultSize, m_pListBoxStrings, wxLB_SINGLE );
    itemBoxSizer7->Add(m_pListBox, 1, wxGROW, 5);

    itemChoicebook5->AddPage(itemPanel6, _("Raw list"));

    wxPanel* itemPanel9 = new wxPanel( itemChoicebook5, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer10 = new wxBoxSizer(wxVERTICAL);
    itemPanel9->SetSizer(itemBoxSizer10);

    m_pTreeCtrl = new wxTreeCtrl( itemPanel9, ID_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_HAS_BUTTONS |wxTR_SINGLE );
    itemBoxSizer10->Add(m_pTreeCtrl, 1, wxGROW, 5);

    itemChoicebook5->AddPage(itemPanel9, _("Classes by parent"));

    itemBoxSizer4->Add(itemChoicebook5, 0, wxGROW|wxALL, 5);

    m_pTextCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(500, -1), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer4->Add(m_pTextCtrl, 3, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer13 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer13, 0, wxGROW|wxALL, 5);
    wxButton* itemButton14 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer13->AddButton(itemButton14);

    wxButton* itemButton15 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer13->AddButton(itemButton15);

    itemStdDialogButtonSizer13->Realize();

////@end wxClassListDialog content construction

    InitControls();
}

int wxClassListDialog::AddClassesWithParent(const wxClassInfo *parent, const wxTreeItemId &id)
{
    const wxClassInfo *ci = wxClassInfo::GetFirst();
    int count = 0;
    while (ci)
    {
        // is this class derived from the given parent?
        if (wxString(ci->GetBaseClassName1()) == parent->GetClassName() ||
            wxString(ci->GetBaseClassName2()) == parent->GetClassName())
        {
            wxTreeItemId child = m_pTreeCtrl->AppendItem(id, ci->GetClassName());
            
            // update the name of this child with the count of the children classes
            int ret = AddClassesWithParent(ci, child);
            m_pTreeCtrl->SetItemText(child,
                                     m_pTreeCtrl->GetItemText(child) +
                                     wxString::Format(wxT(" [%d]"), ret));
            count += ret+1;
        }
        
        ci = ci->GetNext();
    }

    // reorder all the children we've just added


    return count;
}

void wxClassListDialog::InitControls()
{
    const wxClassInfo *ci = wxClassInfo::GetFirst();
    while (ci)
    {
        // init the listbox
        m_pListBox->Append(
            wxString::Format(wxT("[%d] %s"), m_pListBox->GetCount()+1, ci->GetClassName()));
        
        ci = ci->GetNext();
    }

    wxTreeItemId id = m_pTreeCtrl->AddRoot(_T("wxObject"));
    int count = AddClassesWithParent(CLASSINFO(wxObject), id);
    m_pTreeCtrl->SetItemText(id, m_pTreeCtrl->GetItemText(id) +
                                 wxString::Format(wxT(" [%d]"), count));

    // initially expand the root item
    m_pTreeCtrl->Expand(id);
    m_pTreeCtrl->SortChildren(id);
}

wxBitmap wxClassListDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxClassListDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxClassListDialog bitmap retrieval
}

wxIcon wxClassListDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxClassListDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxClassListDialog icon retrieval
}


// ----------------------------------------------------------------------------
// wxClassListDialog - event handlers
// ----------------------------------------------------------------------------

wxString DumpClassInfo(const wxClassInfo*);

void wxClassListDialog::OnListboxSelected( wxCommandEvent& WXUNUSED(event) )
{
    // get info about the selected class
    wxString classname = m_pListBox->GetStringSelection().AfterFirst(_T(' '));
    m_pTextCtrl->SetValue(DumpClassInfo(wxClassInfo::FindClass(classname)));
}

void wxClassListDialog::OnTreectrlSelChanged( wxTreeEvent& event )
{
    // get info about the selected class
    wxString classname = m_pTreeCtrl->GetItemText(event.GetItem()).BeforeFirst(_T(' '));
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
/*
    // return the accessor for this property
    wxPropertyAccessor* GetAccessor() const { return m_accessor; }
*/
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
