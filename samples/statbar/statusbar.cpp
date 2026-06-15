/////////////////////////////////////////////////////////////////////////////
// Name:        Statusbar.cpp
// Purpose:     Minimal wxStatusBar sample
// Author:      Georg Tschojer
// Created:     15.06.26
// Copyright:   (c) Georg Tschojer
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// DECLARATIONS
// ============================================================================
#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "Main.cpp"
#include <wx/wx.h>

// Standard panel class that uses the frame's status bar
class MyPanel : public wxPanel {
public:
    MyPanel(wxFrame *parent);

private:
    wxStatusBar *m_statusBar; // Pointer to the parent frame's status bar
};

#endif  // STATUSBAR_PANEL_H

// ============================================================================
// IMPLEMENTATION
// ============================================================================
#include "statusbar.h"

// Constructor for the panel
MyPanel::MyPanel(wxFrame *parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE) {

    // Retrieve the pointer to the status bar from the parent frame
    m_statusBar = parent->GetStatusBar();

    // Example: Update the status bar text from within the panel
    if (m_statusBar) {
        m_statusBar->SetStatusText("Panel initialized");
    }
};
