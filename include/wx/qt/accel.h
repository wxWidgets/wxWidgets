/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Peter Most
// Modified by:
// Created:     09/08/09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_ACCEL_H_
#define _WX_QT_ACCEL_H_

class WXDLLIMPEXP_CORE wxAcceleratorTable : public wxObject
{
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

private:
    DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
};

#endif // _WX_QT_ACCEL_H_
