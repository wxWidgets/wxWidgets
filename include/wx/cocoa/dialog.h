/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/dialog.h
// Purpose:     wxDialog class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/15
// Copyright:   David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_DIALOG_H_
#define _WX_COCOA_DIALOG_H_

#include "wx/defs.h"
// NOTE: we don't need panel.h, but other things expect it to be included
#include "wx/panel.h"
#include "wx/cocoa/NSPanel.h"

// ========================================================================
// wxDialog
// ========================================================================
class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase, protected wxCocoaNSPanel
{
    DECLARE_DYNAMIC_CLASS(wxDialog)
    WX_DECLARE_COCOA_OWNER(NSPanel,NSWindow,NSWindow)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxDialog() { Init(); }

    // Constructor with no modal flag - the new convention.
    wxDialog(wxWindow *parent, wxWindowID winid,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        Init();
        Create(parent, winid, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxDialogNameStr);

    virtual ~wxDialog();
protected:
    void Init();

// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
protected:
    virtual void CocoaDelegate_windowWillClose(void);
    virtual bool Cocoa_canBecomeMainWindow(bool &canBecome)
    {   canBecome = true; return true; }

// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    virtual bool Show(bool show = true);

    void SetModal(bool flag);
    virtual bool IsModal() const { return m_isModal; }
    bool m_isModal;

    // For now, same as Show(true) but returns return code
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);
};

#endif // _WX_COCOA_DIALOG_H_
