/////////////////////////////////////////////////////////////////////////////
// Name:        edlist.h
// Purpose:     Resource editor list of controls
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _EDLIST_H_
#define _EDLIST_H_

#include <wx/listctrl.h>
#include <wx/imaglist.h>

class wxResourceEditorControlList: public wxListCtrl
{
public:
    wxResourceEditorControlList(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        long style = wxLC_SMALL_ICON|wxLC_AUTOARRANGE|wxLC_SINGLE_SEL);
    ~wxResourceEditorControlList();

    // Load icons and add to the list
    void Initialize();

    // Get selection, or -1
    long GetSelection() const;

DECLARE_EVENT_TABLE()

protected:
    wxImageList m_imageList;
};

#endif

