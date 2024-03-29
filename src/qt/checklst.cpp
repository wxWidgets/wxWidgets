/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/checklst.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QListWidgetItem>

wxCheckListBox::wxCheckListBox()
{
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int nStrings,
        const wxString *choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, nStrings, choices, style, validator, name );
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

wxCheckListBox::~wxCheckListBox()
{
    Clear();
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              int n, const wxString choices[],
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return wxCheckListBoxBase::Create( parent, id, pos, size, n, choices, style, validator, name );
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return wxCheckListBoxBase::Create( parent, id, pos, size, choices, style, validator, name );
}

void wxCheckListBox::Init()
{
    m_hasCheckBoxes = true;
}

bool wxCheckListBox::IsChecked(unsigned int n) const
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    wxCHECK_MSG(item != nullptr, false, wxT("wrong listbox index") );
    return item->checkState() == Qt::Checked;
}

void wxCheckListBox::Check(unsigned int n, bool check )
{
    // Prevent the emission of wxEVT_CHECKLISTBOX event by temporarily blocking all
    // signals on m_qtListWidget object.
    wxQtEnsureSignalsBlocked blocker(m_qtListWidget);
    QListWidgetItem* item = m_qtListWidget->item(n);
    wxCHECK_RET(item != nullptr, wxT("wrong listbox index") );
    item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
}

#endif // wxUSE_CHECKLISTBOX
