/////////////////////////////////////////////////////////////////////////////
// Name:        iconbndl.cpp
// Purpose:     wxIconBundle
// Author:      Mattia Barbon
// Created:     23.03.2002
// RCS-ID:      $Id$
// Copyright:   (c) Mattia barbon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "iconbndl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/image.h"
    #include "wx/icon.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include "wx/iconbndl.h"
#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY(wxIconArray)

const wxIconBundle& wxIconBundle::operator =( const wxIconBundle& ic )
{
    if( this == &ic ) return *this;

    size_t i, max = ic.m_icons.GetCount();

    DeleteIcons();
    for( i = 0; i < max; ++i )
        m_icons.Add( ic.m_icons[i] );

    return *this;
}

void wxIconBundle::DeleteIcons()
{
    m_icons.Empty();
}

void wxIconBundle::AddIcon( const wxString& file, long type )
{
    size_t count = wxImage::GetImageCount( file, type );
    size_t i;
    wxImage image;
    wxIcon tmp;

    for( i = 0; i < count; ++i )
    {
        if( !image.LoadFile( file, type, i ) )
        {
            wxLogError( _("Failed to load image %d from file '%s'."),
                        i, file.c_str() );
            continue;
        }

        tmp.CopyFromBitmap( wxBitmap( image ) );
        AddIcon( tmp );
    }
}

const wxIcon& wxIconBundle::GetIcon( const wxSize& size ) const
{
    size_t i, max = m_icons.GetCount();
    wxCoord sysX = wxSystemSettings::GetMetric( wxSYS_ICON_X ),
            sysY = wxSystemSettings::GetMetric( wxSYS_ICON_Y );
    wxIcon* sysIcon = 0;

    for( i = 0; i < max; ++i )
    {
        wxCoord sx = m_icons[i].GetWidth(), sy = m_icons[i].GetHeight();
        // requested size
        if( sx == size.x && sy == size.y )
            return m_icons[i];
        // keep track if there is a system-size icon
        if( sx == sysX && sy == sysY )
            sysIcon = &m_icons[i];
    }

    // return the system-sized icon if we've got one
    if( sysIcon ) return *sysIcon;
    // return the first icon, if we have one
    return max > 0 ? m_icons[0] : wxNullIcon;
}

void wxIconBundle::AddIcon( const wxIcon& icon )
{
    size_t i, max = m_icons.GetCount();

    for( i = 0; i < max; ++i )
    {
        wxIcon& tmp = m_icons[i];
        if( tmp.GetWidth() == icon.GetWidth() &&
            tmp.GetHeight() == icon.GetHeight() )
        {
            tmp = icon;
            return;
        }
    }

    m_icons.Add( icon );
}
