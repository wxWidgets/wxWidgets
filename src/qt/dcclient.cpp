/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcclient.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcclient.h"
#include "wx/qt/dcclient.h"

//##############################################################################

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner )
    : wxQTDCImpl( owner )
{
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *win )
    : wxQTDCImpl( owner )
{
}

//##############################################################################


wxClientDCImpl::wxClientDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner, win )
{
}

//##############################################################################

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
    : wxClientDCImpl( owner )
{
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win )
    : wxClientDCImpl( owner, win )
{
}

