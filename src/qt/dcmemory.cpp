/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcmemory.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/qt/dcmemory.h"

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
    : wxWindowDCImpl( owner )
{
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc )
    : wxWindowDCImpl( owner )
{
}

