/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/dcmemory.cpp
// Purpose:     wxMemoryDCImpl implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif

#include "wx/retro68/dcmemory.h"
#include "wx/retro68/private/qd.h"

extern GWorldPtr wxRetro68GetScratchPort();

//-----------------------------------------------------------------------------
// wxMemoryDCImpl
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxMemoryDCImpl, wxRetro68DCImpl);

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC *owner)
    : wxRetro68DCImpl(owner)
{
    Init();
}

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC *owner, wxBitmap& bitmap)
    : wxRetro68DCImpl(owner)
{
    Init();
    DoSelect(bitmap);
}

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC *owner, wxDC *WXUNUSED(dc))
    : wxRetro68DCImpl(owner)
{
    Init();
}

wxMemoryDCImpl::~wxMemoryDCImpl()
{
    Deselect();
}

void wxMemoryDCImpl::Init()
{
    // Use the scratch port when no bitmap is selected: this allows using
    // the DC for measuring the text, but nothing is drawn on it.
    GrafPtr port = reinterpret_cast<GrafPtr>(wxRetro68GetScratchPort());
    m_device = port;
    InitForPort(port, wxPoint(), wxRect());
    m_size = wxSize();
}

void wxMemoryDCImpl::Deselect()
{
    if ( m_bmp.IsOk() )
    {
        // Don't leave the GWorld we were drawing on as the current port.
        CGrafPtr port;
        GDHandle device;
        GetGWorld(&port, &device);
        if ( reinterpret_cast<GrafPtr>(port) ==
                reinterpret_cast<GrafPtr>(m_bmp.GetGWorld()) )
            wxMacRestoreScreenPort();

        m_bmp.UnlockPixels();
        m_bmp = wxNullBitmap;
    }

    InvalidateCachedPortState();
}

void wxMemoryDCImpl::DoSelect(const wxBitmap& bitmap)
{
    Deselect();

    if ( !bitmap.IsOk() )
    {
        Init();
        return;
    }

    m_bmp = bitmap;

    GWorldPtr gw = m_bmp.GetGWorld();
    if ( !gw )
    {
        m_bmp = wxNullBitmap;
        Init();
        return;
    }

    // Keep the pixels locked as long as the bitmap is selected.
    int rowBytes;
    m_bmp.LockPixels(&rowBytes);

    m_size = m_bmp.GetSize();

    GrafPtr port = reinterpret_cast<GrafPtr>(gw);
    m_device = port;
    InitForPort(port, wxPoint(), wxRect(m_size));
}
