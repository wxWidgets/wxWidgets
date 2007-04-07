/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/iconbndl.cpp
// Purpose:     wxIconBundle
// Author:      Mattia Barbon, Vadim Zeitlin
// Created:     23.03.2002
// RCS-ID:      $Id$
// Copyright:   (c) Mattia barbon
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/iconbndl.h"

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/icon.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/bitmap.h"
    #include "wx/image.h"
#endif

#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxIconArray)

IMPLEMENT_DYNAMIC_CLASS(wxIconBundle, wxGDIObject)

#define M_ICONBUNDLEDATA ((wxIconBundleRefData *)m_refData)

// ----------------------------------------------------------------------------
// wxIconBundleRefData
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxIconBundleRefData : public wxGDIRefData
{
public:
    // default and copy ctors and assignment operators are ok

protected:
    wxIconArray m_icons;

    friend class WXDLLEXPORT wxIconBundle;
};

// ============================================================================
// wxIconBundle implementation
// ============================================================================

wxIconBundle::wxIconBundle()
            : wxGDIObject()
{
}

wxIconBundle::wxIconBundle(const wxString& file, long type)
            : wxGDIObject()
{
    AddIcon(file, type);
}

wxIconBundle::wxIconBundle(const wxIconBundle& icon)
            : wxGDIObject()
{
    Ref(icon);
}

wxIconBundle::wxIconBundle(const wxIcon& icon)
            : wxGDIObject()
{
    AddIcon(icon);
}

wxObjectRefData *wxIconBundle::CreateRefData() const
{
    return new wxIconBundleRefData;
}

wxObjectRefData *wxIconBundle::CloneRefData(const wxObjectRefData *data) const
{
    return new wxIconBundleRefData(*wx_static_cast(const wxIconBundleRefData *, data));
}

void wxIconBundle::DeleteIcons()
{
    UnRef();
}

bool wxIconBundle::IsOk() const
{
    return M_ICONBUNDLEDATA && !M_ICONBUNDLEDATA->m_icons.IsEmpty();
}

void wxIconBundle::AddIcon(const wxString& file, long type)
{
#ifdef __WXMAC__
    // Deal with standard icons
    if ( type == wxBITMAP_TYPE_ICON_RESOURCE )
    {
        wxIcon tmp(file, type);
        if (tmp.Ok())
        {
            AddIcon(tmp);
            return;
        }
    }
#endif // __WXMAC__

#if wxUSE_IMAGE && (!defined(__WXMSW__) || wxUSE_WXDIB)
    wxImage image;

    const size_t count = wxImage::GetImageCount( file, type );
    for ( size_t i = 0; i < count; ++i )
    {
        if ( !image.LoadFile( file, type, i ) )
        {
            wxLogError( _("Failed to load image %d from file '%s'."),
                        i, file.c_str() );
            continue;
        }

        wxIcon tmp;
        tmp.CopyFromBitmap(wxBitmap(image));
        AddIcon(tmp);
    }
#else // !wxUSE_IMAGE
    wxUnusedVar(file);
    wxUnusedVar(type);
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

wxIcon wxIconBundle::GetIcon(const wxSize& size) const
{
    const size_t count = GetIconCount();

    // optimize for the common case of icon bundles containing one icon only
    wxIcon iconBest;
    switch ( count )
    {
        case 0:
            // nothing to do, iconBest is already invalid
            break;

        case 1:
            iconBest = M_ICONBUNDLEDATA->m_icons[0];
            break;

        default:
            // there is more than one icon, find the best match:
            wxCoord sysX = wxSystemSettings::GetMetric( wxSYS_ICON_X ),
                    sysY = wxSystemSettings::GetMetric( wxSYS_ICON_Y );

            const wxIconArray& iconArray = M_ICONBUNDLEDATA->m_icons;
            for ( size_t i = 0; i < count; i++ )
            {
                const wxIcon& icon = iconArray[i];
                wxCoord sx = icon.GetWidth(),
                        sy = icon.GetHeight();

                // if we got an icon of exactly the requested size, we're done
                if ( sx == size.x && sy == size.y )
                {
                    iconBest = icon;
                    break;
                }

                // the best icon is by default (arbitrarily) the first one but
                // if we find a system-sized icon, take it instead
                if ( sx == sysX && sy == sysY || !iconBest.IsOk() )
                    iconBest = icon;
            }
    }

#ifdef __WXMAC__
    return wxIcon(iconBest.GetHICON(), size);
#else
    return iconBest;
#endif
}

void wxIconBundle::AddIcon(const wxIcon& icon)
{
    wxCHECK_RET( icon.IsOk(), _T("invalid icon") );

    AllocExclusive();

    wxIconArray& iconArray = M_ICONBUNDLEDATA->m_icons;

    // replace existing icon with the same size if we already have it
    const size_t count = iconArray.size();
    for ( size_t i = 0; i < count; ++i )
    {
        wxIcon& tmp = iconArray[i];
        if ( tmp.Ok() &&
                tmp.GetWidth() == icon.GetWidth() &&
                tmp.GetHeight() == icon.GetHeight() )
        {
            tmp = icon;
            return;
        }
    }

    // if we don't, add an icon with new size
    iconArray.Add(icon);
}

size_t wxIconBundle::GetIconCount() const
{
    return IsOk() ? M_ICONBUNDLEDATA->m_icons.size() : 0;
}

wxIcon wxIconBundle::GetIconByIndex(size_t n) const
{
    wxCHECK_MSG( n < GetIconCount(), wxNullIcon, _T("invalid index") );

    return M_ICONBUNDLEDATA->m_icons[n];
}

