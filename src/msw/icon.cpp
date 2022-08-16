/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/icon.cpp
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by: 20.11.99 (VZ): don't derive from wxBitmap any more
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/icon.h"
    #include "wx/bitmap.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxIcon, wxGDIObject);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIconRefData
// ----------------------------------------------------------------------------

void wxIconRefData::Free()
{
    if ( m_hIcon )
    {
        ::DestroyIcon((HICON) m_hIcon);

        m_hIcon = 0;
    }
}

// ----------------------------------------------------------------------------
// wxIcon
// ----------------------------------------------------------------------------

wxIcon::wxIcon(const char bits[], int width, int height)
{
    wxBitmap bmp(bits, width, height);
    CopyFromBitmap(bmp);
}

wxIcon::wxIcon(const wxString& iconfile,
               wxBitmapType type,
               int desiredWidth,
               int desiredHeight)

{
    LoadFile(iconfile, type, desiredWidth, desiredHeight);
}

wxIcon::wxIcon(const wxIconLocation& loc)
{
    // wxICOFileHandler accepts names in the format "filename;index"
    wxString fullname = loc.GetFileName();
    if ( loc.GetIndex() )
    {
        fullname << wxT(';') << loc.GetIndex();
    }
    //else: 0 is default

    LoadFile(fullname, wxBITMAP_TYPE_ICO);
}

wxIcon::~wxIcon()
{
}

wxObjectRefData *wxIcon::CloneRefData(const wxObjectRefData *dataOrig) const
{
    const wxIconRefData *
        data = static_cast<const wxIconRefData *>(dataOrig);
    if ( !data )
        return NULL;

    // we don't have to copy m_hIcon because we're only called from SetHICON()
    // which overwrites m_hIcon anyhow currently
    //
    // and if we're called from SetWidth/Height/Depth(), it doesn't make sense
    // to copy it either as the handle would be inconsistent with the new size
    return new wxIconRefData(*data);
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    HICON hicon = wxBitmapToHICON(bmp);
    if ( !hicon )
    {
        wxLogLastError(wxT("CreateIconIndirect"));
    }
    else
    {
        InitFromHICON((WXHICON)hicon, bmp.GetWidth(), bmp.GetHeight(),
                      bmp.GetScaleFactor());
    }
}

void wxIcon::CreateIconFromXpm(const char* const* data)
{
    wxBitmap bmp(data);
    CopyFromBitmap(bmp);
}

bool wxIcon::LoadFile(const wxString& filename,
                      wxBitmapType type,
                      int desiredWidth, int desiredHeight)
{
    UnRef();

    wxGDIImageHandler *handler = FindHandler(type);

    if ( !handler )
    {
        // load via wxBitmap which, in turn, uses wxImage allowing us to
        // support more formats
        wxBitmap bmp;
        if ( !bmp.LoadFile(filename, type) )
            return false;

        CopyFromBitmap(bmp);
        return true;
    }

    return handler->Load(this, filename, type, desiredWidth, desiredHeight);
}

bool wxIcon::CreateFromHICON(WXHICON icon)
{
    wxSize size = wxGetHiconSize(icon);
    return InitFromHICON(icon, size.GetWidth(), size.GetHeight());
}

bool wxIcon::InitFromHICON(WXHICON icon, int width, int height, double scale)
{
#if wxDEBUG_LEVEL >= 2
    if ( icon != NULL )
    {
        wxSize size = wxGetHiconSize(icon);
        wxASSERT_MSG(size.GetWidth() == width && size.GetHeight() == height,
                     wxS("Inconsistent icon parameters"));
    }
#endif // wxDEBUG_LEVEL >= 2

    AllocExclusive();

    GetGDIImageData()->m_handle = (WXHANDLE)icon;
    GetGDIImageData()->m_width = width;
    GetGDIImageData()->m_height = height;
    GetGDIImageData()->m_scaleFactor = scale;

    return IsOk();
}
