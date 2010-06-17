/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcmemory.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcmemory.h"
#include "wx/qt/dcmemory.h"

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner )
    : wxWindowDCImpl( owner )
{
    m_ok = false;
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap )
    : wxWindowDCImpl( owner )
{
    m_ok = false;
    DoSelect(bitmap);
}

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc )
    : wxWindowDCImpl( owner )
{
    m_ok = false;
}

void wxMemoryDCImpl::DoSelect( const wxBitmap& bitmap )
{
    if (IsOk())
        m_qtPainter.end();
    
    m_ok = false;
    
    if (bitmap.IsOk())
        m_ok = m_qtPainter.begin(bitmap.GetHandle());
}