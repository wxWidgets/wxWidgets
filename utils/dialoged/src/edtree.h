/////////////////////////////////////////////////////////////////////////////
// Name:        edtree.h
// Purpose:     Resource editor project management tree control
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _EDTREE_H_
#define _EDTREE_H_

#include <wx/treectrl.h>

class wxResourceEditorProjectTree: public wxTreeCtrl
{
public:
    wxResourceEditorProjectTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style = wxTR_HAS_BUTTONS);
    
    void LeftDClick(wxMouseEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    
    inline void SetInvalid(bool invalid) { m_invalid = invalid; }
    
protected:
    bool        m_invalid; // While TRUE, don't respond to selections etc.
    
    DECLARE_EVENT_TABLE()
        
};

#endif

