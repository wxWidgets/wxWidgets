/////////////////////////////////////////////////////////////////////////////
// Name:        mainfrm.cpp
// Purpose:     Studio main frame
// Author:      Julian Smart
// Modified by:
// Created:     27/7/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wx/laywin.h"

#include "studio.h"
#include "view.h"
#include "doc.h"
#include "cspalette.h"
#include "mainfrm.h"
#include "dialogs.h"

BEGIN_EVENT_TABLE(csFrame, wxDocMDIParentFrame)
    EVT_MENU(ID_CS_ABOUT, csFrame::OnAbout)
    EVT_MENU(wxID_EXIT, csFrame::OnQuit)
    EVT_MENU(wxID_HELP, csFrame::OnHelp)
    EVT_MENU(ID_CS_SETTINGS, csFrame::OnSettings)
    EVT_SIZE(csFrame::OnSize)
    EVT_SASH_DRAGGED(ID_LAYOUT_WINDOW_PALETTE, csFrame::OnSashDragPaletteWindow)
    EVT_SASH_DRAGGED(ID_LAYOUT_WINDOW_PROJECT, csFrame::OnSashDragProjectWindow)
    EVT_IDLE(csFrame::OnIdle)
    EVT_UPDATE_UI(wxID_PRINT, csFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_PREVIEW, csFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_SAVE, csFrame::OnSaveUpdate)
    EVT_UPDATE_UI(wxID_SAVEAS, csFrame::OnSaveUpdate)
    EVT_UPDATE_UI(wxID_UNDO, csFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_REDO, csFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_CUT, csFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_COPY, csFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_PASTE, csFrame::OnUpdateDisable)
    EVT_CLOSE(csFrame::OnCloseWindow)
END_EVENT_TABLE()

// Define my frame constructor
csFrame::csFrame(wxDocManager* manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
	long style):
  wxDocMDIParentFrame(manager, parent, id, title, pos, size, style, "frame")
{
    CreateToolBar(wxNO_BORDER|wxTB_FLAT|wxTB_HORIZONTAL);
    wxGetApp().InitToolBar(GetToolBar());

    // Accelerators
    wxAcceleratorEntry entries[4];

    entries[0].Set(wxACCEL_NORMAL,  WXK_F1,        wxID_HELP);
    entries[1].Set(wxACCEL_CTRL,   'O',            wxID_OPEN);
    entries[2].Set(wxACCEL_CTRL,   'N',            wxID_NEW);
    entries[3].Set(wxACCEL_CTRL,   'P',            wxID_PRINT);

    wxAcceleratorTable accel(4, entries);
    SetAcceleratorTable(accel);
}

void csFrame::OnHelp(wxCommandEvent& event)
{
    wxGetApp().GetHelpController().DisplayContents();
}

void csFrame::OnSettings(wxCommandEvent& event)
{
    csSettingsDialog* dialog = new csSettingsDialog(this);
    int ret = dialog->ShowModal();
    dialog->Destroy();
}

void csFrame::OnQuit(wxCommandEvent& event)
{
      Close(TRUE);
}

void csFrame::OnAbout(wxCommandEvent& event)
{
      (void)wxMessageBox("OGL Studio\n(c) 1999, Julian Smart", "About OGL Studio", wxICON_INFORMATION);
}

void csFrame::OnSashDragPaletteWindow(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case ID_LAYOUT_WINDOW_PALETTE:
        {
            wxGetApp().GetDiagramPaletteSashWindow()->SetDefaultSize(wxSize(10000, event.GetDragRect().height));
            break;
        }
    }
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
}

void csFrame::OnSashDragProjectWindow(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId())
    {
        case ID_LAYOUT_WINDOW_PROJECT:
        {
            wxGetApp().GetProjectSashWindow()->SetDefaultSize(wxSize(event.GetDragRect().width, 10000));
            break;
        }
    }
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void csFrame::OnCloseWindow(wxCloseEvent& event)
{
    int x, y;
    GetPosition(& x, & y);
    wxGetApp().m_mainFramePos = wxPoint(x, y);

    GetSize(& x, & y);
    wxGetApp().m_mainFrameSize = wxSize(x, y);

    wxDocMDIParentFrame::OnCloseWindow(event);
}

void csFrame::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
}

// Make sure the correct toolbars are showing for the active view
void csFrame::OnIdle(wxIdleEvent& event)
{
    wxDocMDIParentFrame::OnIdle(event);

    wxSashLayoutWindow* paletteWin = wxGetApp().GetDiagramPaletteSashWindow();
    wxSashLayoutWindow* diagramToolBarWin = wxGetApp().GetDiagramToolBarSashWindow();
    if (!paletteWin || !diagramToolBarWin)
        return;
    bool doLayout = FALSE;
    if (GetActiveChild())
    {
        if (!paletteWin->IsShown() || !diagramToolBarWin->IsShown())
        {
            paletteWin->Show(TRUE);
            diagramToolBarWin->Show(TRUE);

            doLayout = TRUE;
        }
    }
    else
    {
        if (paletteWin->IsShown() || diagramToolBarWin->IsShown())
        {
            paletteWin->Show(FALSE);
            diagramToolBarWin->Show(FALSE);
            doLayout = TRUE;
        }
    }
    if (doLayout)
    {
        wxLayoutAlgorithm layout;
        layout.LayoutMDIFrame(this);
    }
}

// General handler for disabling items
void csFrame::OnUpdateDisable(wxUpdateUIEvent& event)
{
    event.Enable(FALSE);
}

void csFrame::OnSaveUpdate(wxUpdateUIEvent& event)
{
    event.Enable( (GetActiveChild() != NULL) );
}

/*
 * Child frame
 */

BEGIN_EVENT_TABLE(csMDIChildFrame, wxDocMDIChildFrame)
  EVT_ACTIVATE(csMDIChildFrame::OnActivate)
END_EVENT_TABLE()

csMDIChildFrame::csMDIChildFrame(wxDocument* doc, wxView* view, wxMDIParentFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
  wxDocMDIChildFrame(doc, view, parent, id, title, pos, size, style)
{
    // Accelerators
    wxAcceleratorEntry entries[18];

    // Usual editing functions
    entries[0].Set(wxACCEL_NORMAL,  WXK_DELETE,     wxID_CLEAR);
    entries[1].Set(wxACCEL_CTRL,    'X',            wxID_CUT);
    entries[2].Set(wxACCEL_CTRL,    'C',            wxID_COPY);
    entries[3].Set(wxACCEL_SHIFT,   WXK_INSERT,     wxID_PASTE);
    entries[4].Set(wxACCEL_CTRL,    'V',            wxID_PASTE);
    entries[5].Set(wxACCEL_CTRL,    'A',            ID_CS_SELECT_ALL);

    // Undo/redo
    entries[6].Set(wxACCEL_CTRL,    'Z',            wxID_UNDO);
    entries[7].Set(wxACCEL_CTRL,    'Y',            wxID_REDO);

    // Other
    entries[8].Set(wxACCEL_NORMAL,  WXK_RETURN,     ID_CS_EDIT_PROPERTIES);
    entries[9].Set(wxACCEL_ALT,     WXK_RETURN,     ID_CS_EDIT_PROPERTIES);
    entries[10].Set(wxACCEL_CTRL,   'D',            wxID_DUPLICATE);
    entries[11].Set(wxACCEL_NORMAL,  WXK_F1,        wxID_HELP);

    // File handling
    entries[12].Set(wxACCEL_CTRL,   'S',            wxID_SAVE);
    entries[13].Set(wxACCEL_NORMAL,  WXK_F12,       wxID_SAVEAS);
    entries[14].Set(wxACCEL_CTRL,   'O',            wxID_OPEN);
    entries[15].Set(wxACCEL_CTRL,   'N',            wxID_NEW);
    entries[16].Set(wxACCEL_CTRL,   'P',            wxID_PRINT);
    entries[17].Set(wxACCEL_CTRL,   'W',            wxID_CLOSE);


    wxAcceleratorTable accel(18, entries);
    SetAcceleratorTable(accel);
}

void csMDIChildFrame::OnActivate(wxActivateEvent& event)
{
    wxDocMDIChildFrame::OnActivate(event);
/*
    wxSashLayoutWindow* win = wxGetApp().GetDiagramPaletteSashWindow();
    if (!win)
        return;

    win->Show(event.GetActive());

    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame((wxMDIParentFrame*) GetParent());
*/
}

