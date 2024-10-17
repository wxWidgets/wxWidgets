/////////////////////////////////////////////////////////////////////////////
// Name:        artbrows.h
// Purpose:     wxArtProvider demo - art browser dialog
// Author:      Vaclav Slavik
// Created:     2002/04/05
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ARTBROWS_H__
#define __ARTBROWS_H__

#include "wx/dialog.h"
#include "wx/artprov.h"

class WXDLLIMPEXP_FWD_CORE wxListCtrl;
class WXDLLIMPEXP_FWD_CORE wxListEvent;
class WXDLLIMPEXP_FWD_CORE wxStaticBitmap;

class wxArtBrowserDialog : public wxDialog
{
public:
    wxArtBrowserDialog(wxWindow *parent);
    ~wxArtBrowserDialog();

    void SetArtClient(const wxArtClient& client);
    void SetArtBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size = wxDefaultSize);

private:
    void OnSelectItem(wxListEvent &event);
    void OnChangeSize(wxCommandEvent &event);
    void OnChooseClient(wxCommandEvent &event);

    wxSize GetSelectedBitmapSize() const;

    void DeleteListItemData();

    wxListCtrl *m_list;
    wxStaticBitmap *m_canvas;
    wxStaticText *m_text;
    wxString m_client;
    wxChoice *m_sizes;
    wxString m_currentArtId;

    wxDECLARE_EVENT_TABLE();
};

#endif // __ARTBROWS_H__

