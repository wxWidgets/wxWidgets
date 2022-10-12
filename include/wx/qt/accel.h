/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_ACCEL_H_
#define _WX_QT_ACCEL_H_

/* wxQt accelerators implementation:
 *
 * Storing:
 * QShortcuts are stored in wxWindow (m_qtShortcuts) to allow to delete them
 * when the accelerator table is changed, and also because we need to specify
 * a not-null parent from them, which is unknown at the moment of creating the
 * accelerator table. So, the accelerator table only contains a list of
 * wxAcceleratorEntries, which are converted to a list of QShortcuts when
 * the table is fixed to a wxWindow.
 *
 * Passing keypresses to accelerators:
 * The accelerators are implemented using QShortcut's. As there is no easy way
 * to call them, we must pass all keypress events through the QApplication
 * notify() function (which is the one that checks if the keypress match any
 * shortcut.
 *
 * Executing commands when a QShortcut is triggered:
 * Each QShortcut has a property ("wxQt_Command") set with the number of the
 * wx command it is associated to. Then, its activated() signal is connected to
 * a small handler (wxQtShortcutHandler in window_qt.h) which calls the main
 * handler (wxWindow::QtHandleShortcut) passing the command extracted from the
 * QShortcut. This handler will finally create and send the appropriate wx
 * event to the window. */

#include "wx/vector.h"

class QShortcut;
template < class T > class QList;

class WXDLLIMPEXP_CORE wxAcceleratorTable : public wxObject
{
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

    // Implementation
    wxVector<QShortcut*> ConvertShortcutTable( QWidget *parent ) const;

    bool Ok() const { return IsOk(); }
    bool IsOk() const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const override;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxAcceleratorTable);
};

#endif // _WX_QT_ACCEL_H_
