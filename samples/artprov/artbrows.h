/////////////////////////////////////////////////////////////////////////////
// Name:        artbrows.h
// Purpose:     wxArtProvider demo - art browser dialog
// Author:      Vaclav Slavik
// Modified by:
// Created:     2002/04/05
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __ARTBROWS_H__
#define __ARTBROWS_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "artbrows.h"
#endif

#include "wx/dialog.h"
#include "wx/artprov.h"

class WXDLLEXPORT wxListCtrl;
class WXDLLEXPORT wxListEvent;
class WXDLLEXPORT wxStaticBitmap;

class wxArtBrowserDialog : public wxDialog
{
public:
    wxArtBrowserDialog(wxWindow *parent);

    void SetArtClient(const wxArtClient& client);

private:
    void OnSelectItem(wxListEvent &event);
    void OnChooseClient(wxCommandEvent &event);
    
    wxListCtrl *m_list;
    wxStaticBitmap *m_canvas;
    wxString m_client;

    DECLARE_EVENT_TABLE()
};

#endif // __ARTBROWS_H__

