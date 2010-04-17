/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/checklst.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checklst.h"

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
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
}


bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              int n, const wxString choices[],
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return false;
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return false;
}


bool wxCheckListBox::IsChecked(unsigned int item) const
{
    return false;
}

void wxCheckListBox::Check(unsigned int item, bool check )
{
}

