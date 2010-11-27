/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/groupbox_qt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/groupbox_qt.h"
#include "wx/window.h"
#include "wx/qt/converter.h"

wxQtGroupBox::wxQtGroupBox( wxWindow *parent, const wxString &label )
    : QGroupBox( wxQtConvertString( label ), parent->GetHandle() )
{
}
