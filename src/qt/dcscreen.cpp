/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcscreen.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcscreen.h"
#include "wx/qt/dcscreen.h"

wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner )
    : wxWindowDCImpl( owner )
{
}

