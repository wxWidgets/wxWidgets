///////////////////////////////////////////////////////////////////////////////
// Name:        msw/gdiimage.cpp
// Purpose:     wxGDIImage implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "gdiimage.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#include "wx/app.h"

#include "wx/msw/dib.h"
#include "wx/msw/gdiimage.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// all image handlers are declared/defined in this file because the outside
// world doesn't have to know about them (but only about wxBITMAP_TYPE_XXX ids)

class WXDLLEXPORT wxBMPFileHandler : public wxBitmapHandler
{
public:
    wxBMPFileHandler() : wxBitmapHandler(_T("Windows bitmap file"), _T("bmp"),
                                         wxBITMAP_TYPE_BMP)
    {
    }

    virtual bool LoadFile(wxBitmap *bitmap,
                          const wxString& name, long flags,
                          int desiredWidth, int desiredHeight);
    virtual bool SaveFile(wxBitmap *bitmap,
                          const wxString& name, int type,
                          const wxPalette *palette = NULL);

private:
    DECLARE_DYNAMIC_CLASS(wxBMPFileHandler)
};

class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
public:
    wxBMPResourceHandler() : wxBitmapHandler(_T("Windows bitmap resource"),
                                             wxEmptyString,
                                             wxBITMAP_TYPE_BMP_RESOURCE)
    {
    }

    virtual bool LoadFile(wxBitmap *bitmap,
                          const wxString& name, long flags,
                          int desiredWidth, int desiredHeight);

private:
    DECLARE_DYNAMIC_CLASS(wxBMPResourceHandler)
};

class WXDLLEXPORT wxIconHandler : public wxGDIImageHandler
{
public:
    wxIconHandler(const wxString& name, const wxString& ext, long type)
        : wxGDIImageHandler(name, ext, type)
    {
    }

    // creating and saving icons is not supported
    virtual bool Create(wxGDIImage *WXUNUSED(image),
                        void *WXUNUSED(data),
                        long WXUNUSED(flags),
                        int WXUNUSED(width),
                        int WXUNUSED(height),
                        int WXUNUSED(depth) = 1)
    {
        return FALSE;
    }

    virtual bool Save(wxGDIImage *WXUNUSED(image),
                      const wxString& WXUNUSED(name),
                      int WXUNUSED(type))
    {
        return FALSE;
    }

    virtual bool Load(wxGDIImage *image,
                      const wxString& name,
                      long flags,
                      int desiredWidth, int desiredHeight)
    {
        wxIcon *icon = wxDynamicCast(image, wxIcon);
        wxCHECK_MSG( icon, FALSE, _T("wxIconHandler only works with icons") );

        return LoadIcon(icon, name, flags, desiredWidth, desiredHeight);
    }

protected:
    virtual bool LoadIcon(wxIcon *icon,
                          const wxString& name, long flags,
                          int desiredWidth = -1, int desiredHeight = -1) = 0;
};

class WXDLLEXPORT wxICOFileHandler : public wxIconHandler
{
public:
    wxICOFileHandler() : wxIconHandler(_T("ICO icon file"),
                                       _T("ico"),
                                       wxBITMAP_TYPE_ICO)
    {
    }

    virtual bool LoadIcon(wxIcon *icon,
                          const wxString& name, long flags,
                          int desiredWidth = -1, int desiredHeight = -1);

private:
    DECLARE_DYNAMIC_CLASS(wxICOFileHandler)
};

class WXDLLEXPORT wxICOResourceHandler: public wxIconHandler
{
public:
    wxICOResourceHandler() : wxIconHandler(_T("ICO resource"),
                                           _T("ico"),
                                           wxBITMAP_TYPE_ICO_RESOURCE)
    {
    }

    virtual bool LoadIcon(wxIcon *icon,
                          const wxString& name, long flags,
                          int desiredWidth = -1, int desiredHeight = -1);

private:
    DECLARE_DYNAMIC_CLASS(wxICOResourceHandler)
};

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxBMPFileHandler, wxBitmapHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxICOFileHandler, wxGDIImageHandler)
    IMPLEMENT_DYNAMIC_CLASS(wxICOResourceHandler, wxGDIImageHandler)

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxSize GetHiconSize(HICON hicon);

// ============================================================================
// implementation
// ============================================================================

wxList wxGDIImage::ms_handlers;

// ----------------------------------------------------------------------------
// wxGDIImage functions forwarded to wxGDIImageRefData
// ----------------------------------------------------------------------------

bool wxGDIImage::FreeResource(bool WXUNUSED(force))
{
    if ( !IsNull() )
    {
        GetGDIImageData()->Free();
        GetGDIImageData()->m_handle = 0;
    }

    return TRUE;
}

WXHANDLE wxGDIImage::GetResourceHandle()
{
    return GetHandle();
}

// ----------------------------------------------------------------------------
// wxGDIImage handler stuff
// ----------------------------------------------------------------------------

void wxGDIImage::AddHandler(wxGDIImageHandler *handler)
{
    ms_handlers.Append(handler);
}

void wxGDIImage::InsertHandler(wxGDIImageHandler *handler)
{
    ms_handlers.Insert(handler);
}

bool wxGDIImage::RemoveHandler(const wxString& name)
{
    wxGDIImageHandler *handler = FindHandler(name);
    if ( handler )
    {
        ms_handlers.DeleteObject(handler);
        return TRUE;
    }
    else
        return FALSE;
}

wxGDIImageHandler *wxGDIImage::FindHandler(const wxString& name)
{
    wxNode *node = ms_handlers.First();
    while ( node )
    {
        wxGDIImageHandler *handler = (wxGDIImageHandler *)node->Data();
        if ( handler->GetName() == name )
            return handler;
        node = node->Next();
    }

    return NULL;
}

wxGDIImageHandler *wxGDIImage::FindHandler(const wxString& extension,
                                           long type)
{
    wxNode *node = ms_handlers.First();
    while ( node )
    {
        wxGDIImageHandler *handler = (wxGDIImageHandler *)node->Data();
        if ( (handler->GetExtension() = extension) &&
             (type == -1 || handler->GetType() == type) )
        {
            return handler;
        }

        node = node->Next();
    }
    return NULL;
}

wxGDIImageHandler *wxGDIImage::FindHandler(long type)
{
    wxNode *node = ms_handlers.First();
    while ( node )
    {
        wxGDIImageHandler *handler = (wxGDIImageHandler *)node->Data();
        if ( handler->GetType() == type )
            return handler;

        node = node->Next();
    }

    return NULL;
}

void wxGDIImage::CleanUpHandlers()
{
    wxNode *node = ms_handlers.First();
    while ( node )
    {
        wxGDIImageHandler *handler = (wxGDIImageHandler *)node->Data();
        wxNode *next = node->Next();
        delete handler;
        delete node;
        node = next;
    }
}

void wxGDIImage::InitStandardHandlers()
{
    AddHandler(new wxBMPResourceHandler);
    AddHandler(new wxBMPFileHandler);

    // Not added by default: include xpmhand.h in your app
    // and call these in your wxApp::OnInit.
    //  AddHandler(new wxXPMFileHandler);
    //  AddHandler(new wxXPMDataHandler);

    AddHandler(new wxICOResourceHandler);
    AddHandler(new wxICOFileHandler);
}

// ----------------------------------------------------------------------------
// wxBitmap handlers
// ----------------------------------------------------------------------------

bool wxBMPResourceHandler::LoadFile(wxBitmap *bitmap,
                                    const wxString& name, long WXUNUSED(flags),
                                    int WXUNUSED(desiredWidth),
                                    int WXUNUSED(desiredHeight))
{
    // TODO: load colourmap.
    bitmap->SetHBITMAP((WXHBITMAP)::LoadBitmap(wxGetInstance(), name));

    wxBitmapRefData *data = bitmap->GetBitmapData();
    if ( bitmap->Ok() )
    {
        BITMAP bm;
        if ( !::GetObject(GetHbitmapOf(*bitmap), sizeof(BITMAP), (LPSTR) &bm) )
        {
            wxLogLastError("GetObject(HBITMAP)");
        }

        data->m_width = bm.bmWidth;
        data->m_height = bm.bmHeight;
        data->m_depth = bm.bmBitsPixel;
    }
    else
    {
        // it's probably not found
        wxLogError(wxT("Can't load bitmap '%s' from resources! Check .rc file."),
                   name.c_str());
    }

    return bitmap->Ok();
}

bool wxBMPFileHandler::LoadFile(wxBitmap *bitmap,
                                const wxString& name, long WXUNUSED(flags),
                                int WXUNUSED(desiredWidth),
                                int WXUNUSED(desiredHeight))
{
#if wxUSE_IMAGE_LOADING_IN_MSW
    wxPalette *palette = NULL;
    bool success = wxLoadIntoBitmap(WXSTRINGCAST name, bitmap, &palette) != 0;
    if ( success && palette )
    {
        bitmap->SetPalette(*palette);
    }

    // it was copied by the bitmap if it was loaded successfully
    delete palette;

    return success;
#else
    return FALSE;
#endif
}

bool wxBMPFileHandler::SaveFile(wxBitmap *bitmap,
                                const wxString& name,
                                int WXUNUSED(type),
                                const wxPalette *pal)
{
#if wxUSE_IMAGE_LOADING_IN_MSW
    wxPalette *actualPalette = (wxPalette *)pal;
    if ( !actualPalette )
        actualPalette = bitmap->GetPalette();
    return wxSaveBitmap(WXSTRINGCAST name, bitmap, actualPalette) != 0;
#else
    return FALSE;
#endif
}

// ----------------------------------------------------------------------------
// wxIcon handlers
// ----------------------------------------------------------------------------

bool wxICOFileHandler::LoadIcon(wxIcon *icon,
                                const wxString& name,
                                long flags,
                                int desiredWidth, int desiredHeight)
{
#if wxUSE_RESOURCE_LOADING_IN_MSW
    icon->UnRef();

    // actual size
    wxSize size;

#ifdef __WIN32__
    HICON hicon = ::ExtractIcon(wxGetInstance(), name, 0 /* first */);
    if ( !hicon )
    {
        wxLogSysError(_T("Failed to load icon from the file '%s'"),
                      name.c_str());

        return FALSE;
    }

    size = GetHiconSize(hicon);
#else // Win16
    HICON hicon = ReadIconFile((wxChar *)name.c_str(),
                               wxGetInstance(),
                               &size.x, &size.y);
#endif // Win32/Win16

    if ( (desiredWidth != -1 && desiredWidth != size.x) ||
         (desiredHeight != -1 && desiredHeight != size.y) )
    {
        wxLogDebug(_T("Returning FALSE from wxICOFileHandler::Load because "
                      "of the size mismatch: actual (%d, %d), "
                      "requested (%d, %d)"),
                      size.x, size.y,
                      desiredWidth, desiredHeight);

        ::DestroyIcon(hicon);

        return FALSE;
    }

    icon->SetHICON((WXHICON)hicon);
    icon->SetSize(size.x, size.y);

    return icon->Ok();
#else
    return FALSE;
#endif
}

bool wxICOResourceHandler::LoadIcon(wxIcon *icon,
                                    const wxString& name,
                                    long flags,
                                    int desiredWidth, int desiredHeight)
{
    HICON hicon;

#if defined(__WIN32__) && !defined(__SC__)
    if ( desiredWidth != -1 && desiredHeight != -1 )
    {
        hicon = (HICON)::LoadImage(wxGetInstance(), name, IMAGE_ICON,
                                    desiredWidth, desiredHeight,
                                    LR_DEFAULTCOLOR);
    }
    else
#endif // Win32
    {
        hicon = ::LoadIcon(wxGetInstance(), name);
    }

    wxSize size = GetHiconSize(hicon);
    icon->SetSize(size.x, size.y);

    // Override the found values with desired values
    if ( desiredWidth > -1 && desiredHeight > -1 )
    {
        icon->SetSize(desiredWidth, desiredHeight);
    }

    icon->SetHICON((WXHICON)hicon);

    return icon->Ok();
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxSize GetHiconSize(HICON hicon)
{
    wxSize size(32, 32);    // default

#ifdef __WIN32__
    // Win32s doesn't have GetIconInfo function...
    if ( hicon && wxGetOsVersion() != wxWIN32S )
    {
        ICONINFO info;
        if ( !::GetIconInfo(hicon, &info) )
        {
            wxLogLastError("GetIconInfo");
        }
        else
        {
            HBITMAP hbmp = info.hbmMask;
            if ( hbmp )
            {
                BITMAP bm;
                if ( ::GetObject(hbmp, sizeof(BITMAP), (LPSTR) &bm) )
                {
                    size = wxSize(bm.bmWidth, bm.bmHeight);
                }

                ::DeleteObject(info.hbmMask);
            }
            if ( info.hbmColor )
                ::DeleteObject(info.hbmColor);
        }
    }
#endif

    return size;
}
