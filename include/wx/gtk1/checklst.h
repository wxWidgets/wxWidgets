///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.h
// Purpose:     wxCheckListBox class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __GTKCHECKLISTH__
#define __GTKCHECKLISTH__

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// wxCheckListBox
//-----------------------------------------------------------------------------

class wxCheckListBox : public wxListBox
{
public:
    wxCheckListBox();
    wxCheckListBox(wxWindow *parent, wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int nStrings = 0,
            const wxString *choices = (const wxString *)NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr);

    bool IsChecked( int index ) const;
    void Check( int index, bool check = TRUE );

    int GetItemHeight() const;
    
private:
    DECLARE_DYNAMIC_CLASS(wxCheckListBox)
};

#endif   //__GTKCHECKLISTH__
