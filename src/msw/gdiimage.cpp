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
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#include "wx/app.h"

#ifndef __WXMICROWIN__
#include "wx/msw/dib.h"
#endif

#include "wx/msw/bitmap.h"
#include "wx/msw/gdiimage.h"
#include "wx/bitmap.h"

#ifdef __WIN16__
#   include "wx/msw/curico.h"
#endif // __WIN16__

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#ifndef __WXMICROWIN__

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
IMPLEMENT_DYNAMIC_CLASS(wxICOFileHandler, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxICOResourceHandler, wxObject)

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

#endif
    // __MICROWIN__

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

WXHANDLE wxGDIImage::GetResourceHandle() const
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
#ifndef __WXMICROWIN__
    AddHandler(new wxBMPResourceHandler);
    AddHandler(new wxBMPFileHandler);
    AddHandler(new wxICOResourceHandler);
    AddHandler(new wxICOFileHandler);
#endif
}

#ifndef __WXMICROWIN__

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
            wxLogLastError(wxT("GetObject(HBITMAP)"));
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

#if wxUSE_PALETTE
    if ( success && palette )
    {
        bitmap->SetPalette(*palette);
    }

    // it was copied by the bitmap if it was loaded successfully
    delete palette;
#endif // wxUSE_PALETTE

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

#if wxUSE_PALETTE
    wxPalette *actualPalette = (wxPalette *)pal;
    if ( !actualPalette )
        actualPalette = bitmap->GetPalette();
#else
    wxPalette *actualPalette = NULL;
#endif // wxUSE_PALETTE

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
                                long WXUNUSED(flags),
                                int desiredWidth, int desiredHeight)
{
#if wxUSE_RESOURCE_LOADING_IN_MSW
    icon->UnRef();

    // actual size
    wxSize size;

#ifdef __WIN32__
    HICON hicon = NULL;

    // Parse the filename: it may be of the form "filename;n" in order to
    // specify the nth icon in the file.
    //
    // For the moment, ignore the issue of possible semicolons in the
    // filename.
    int iconIndex = 0;
    wxString nameReal(name);
    wxString strIconIndex = name.AfterLast(wxT(';'));
    if (strIconIndex != name)
    {
        iconIndex = wxAtoi(strIconIndex);
        nameReal = name.BeforeLast(wxT(';'));
    }

#if 0
    // If we don't know what size icon we're looking for,
    // try to find out what's there.
    // Unfortunately this doesn't work, because ExtractIconEx
    // will scale the icon to the 'desired' size, even if that
    // size of icon isn't explicitly stored. So we would have
    // to parse the icon file outselves.
    if ( desiredWidth == -1 &&
         desiredHeight == -1)
    {
        // Try loading a large icon first
        if ( ::ExtractIconEx(nameReal, iconIndex, &hicon, NULL, 1) == 1)
        {
        }
        // Then try loading a small icon
        else if ( ::ExtractIconEx(nameReal, iconIndex, NULL, &hicon, 1) == 1)
        {
        }
    }
    else
#endif
    // were we asked for a large icon?
    if ( desiredWidth == ::GetSystemMetrics(SM_CXICON) &&
         desiredHeight == ::GetSystemMetrics(SM_CYICON) )
    {
        // get the specified large icon from file
        if ( !::ExtractIconEx(nameReal, iconIndex, &hicon, NULL, 1) )
        {
            // it is not an error, but it might still be useful to be informed
            // about it optionally
            wxLogTrace(_T("iconload"),
                       _T("No large icons found in the file '%s'."),
                       name.c_str());
        }
    }
    else if ( desiredWidth == ::GetSystemMetrics(SM_CXSMICON) &&
              desiredHeight == ::GetSystemMetrics(SM_CYSMICON) )
    {
        // get the specified small icon from file
        if ( !::ExtractIconEx(nameReal, iconIndex, NULL, &hicon, 1) )
        {
            wxLogTrace(_T("iconload"),
                       _T("No small icons found in the file '%s'."),
                       name.c_str());
        }
    }
    //else: not standard size, load below

    if ( !hicon )
    {
        // take any size icon from the file by index
        hicon = ::ExtractIcon(wxGetInstance(), nameReal, iconIndex);
    }

    if ( !hicon )
    {
        wxLogSysError(_T("Failed to load icon from the file '%s'"),
                      name.c_str());

        return FALSE;
    }

    size = wxGetHiconSize(hicon);
#else // Win16
    HICON hicon = ReadIconFile((wxChar *)name.c_str(),
                               wxGetInstance(),
                               &size.x, &size.y);
#endif // Win32/Win16

    if ( (desiredWidth != -1 && desiredWidth != size.x) ||
         (desiredHeight != -1 && desiredHeight != size.y) )
    {
        wxLogTrace(_T("iconload"),
                   _T("Returning FALSE from wxICOFileHandler::Load because of the size mismatch: actual (%d, %d), requested (%d, %d)"),
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
                                    long WXUNUSED(flags),
                                    int desiredWidth, int desiredHeight)
{
    HICON hicon;

    // do we need the icon of the specific size or would any icon do?
    bool hasSize = desiredWidth != -1 || desiredHeight != -1;

    wxASSERT_MSG( !hasSize || (desiredWidth != -1 && desiredHeight != -1),
                  _T("width and height should be either both -1 or not") );

    // try to load the icon from this program first to allow overriding the
    // standard icons (although why one would want to do it considering that
    // we already have wxApp::GetStdIcon() is unclear)

    // note that we can't just always call LoadImage() because it seems to do
    // some icon rescaling internally which results in very ugly 16x16 icons
#if defined(__WIN32__) && !defined(__SC__)
    if ( hasSize )
    {
        hicon = (HICON)::LoadImage(wxGetInstance(), name, IMAGE_ICON,
                                    desiredWidth, desiredHeight,
                                    LR_DEFAULTCOLOR);
    }
    else
#endif // Win32/!Win32
    {
        hicon = ::LoadIcon(wxGetInstance(), name);
    }

    // next check if it's not a standard icon
    if ( !hicon && !hasSize )
    {
        static const struct
        {
            const wxChar *name;
            LPTSTR id;
        } stdIcons[] =
        {
            { wxT("wxICON_QUESTION"),   IDI_QUESTION    },
            { wxT("wxICON_WARNING"),    IDI_EXCLAMATION },
            { wxT("wxICON_ERROR"),      IDI_HAND        },
            { wxT("wxICON_INFORMATION"),       IDI_ASTERISK    },
        };

        for ( size_t nIcon = 0; !hicon && nIcon < WXSIZEOF(stdIcons); nIcon++ )
        {
            if ( name == stdIcons[nIcon].name )
            {
                hicon = ::LoadIcon((HINSTANCE)NULL, stdIcons[nIcon].id);
            }
        }
    }

    wxSize size = wxGetHiconSize(hicon);
    icon->SetSize(size.x, size.y);

    icon->SetHICON((WXHICON)hicon);

    return icon->Ok();
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

wxSize wxGetHiconSize(HICON hicon)
{
    wxSize size(32, 32);    // default

#ifdef __WIN32__
    // Win32s doesn't have GetIconInfo function...
    if ( hicon && wxGetOsVersion() != wxWIN32S )
    {
        ICONINFO info;
        if ( !::GetIconInfo(hicon, &info) )
        {
            wxLogLastError(wxT("GetIconInfo"));
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
#endif
    // __WXMICROWIN__
