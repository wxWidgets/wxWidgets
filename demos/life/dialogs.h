/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.h
// Purpose:     Life! dialogs
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIFE_DIALOGS_H_
#define _LIFE_DIALOGS_H_

#ifdef __GNUG__
    #pragma interface "dialogs.h"
#endif

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/spinctrl.h"

#include "life.h"
#include "game.h"


// --------------------------------------------------------------------------
// LifeSamplesDialog
// --------------------------------------------------------------------------

class LifeSamplesDialog : public wxDialog
{
public:
    // ctor and dtor
    LifeSamplesDialog(wxWindow *parent);
    ~LifeSamplesDialog();

    // members
    const LifeShape& GetShape();

    // event handlers
    void OnListBox(wxCommandEvent &event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

    int         m_value;
    wxListBox  *m_list;
    wxTextCtrl *m_text;
    LifeCanvas *m_canvas;
    Life       *m_life;
};

// --------------------------------------------------------------------------
// LifeAboutDialog
// --------------------------------------------------------------------------

class LifeAboutDialog : public wxDialog
{
public:
    // ctor
    LifeAboutDialog(wxWindow *parent);
};

#endif  // _LIFE_DIALOGS_H_
