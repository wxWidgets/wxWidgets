/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.cpp
// Purpose:     provide wxSizer class for layounting
// Author:      Robert Roebling and Robin Dunn
// Modified by:
// Created:     
// RCS-ID:      $Id$
// Copyright:   (c) Robin Dunn, Dirk Holtwick and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSIZERS_H__
#define __WXSIZERS_H__

#ifdef __GNUG__
#pragma interface "sizers.h"
#endif

#include "wx/sizer.h"


//---------------------------------------------------------------------------
// wxSizerItem
//---------------------------------------------------------------------------

wxSizerItem::wxSizerItem( int width, int height, int option )
{
    m_window = (wxWindow *) NULL;
    m_sizer = (wxSizer *) NULL;
    m_minSize.x = width;
    m_minSize.h = height;
    m_option = option;
}

wxSizerItem::wxSizerItem( wxWindow *window, int option )
{
    m_window = window;
    m_sizer = (wxSizer *) NULL;
    m_minSize = window->GetSize();
    m_option = option;
}

wxSizerItem::wxSizerItem( wxSizer *sizer, int option )
{
    m_window = (wxWindow *) NULL;
    m_sizer = sizer;
    m_minSize.x = -1;
    m_minSize.h = -1;
    m_option = option;
}

wxSize wxSizerItem::GetMinSize()
{
    if (IsSizer())
        return m_sizer->GetMinSize();
    else
        return m_minSize;
}

bool wxSizerItem::IsWindow()
{
    return (m_window != NULL);
}

bool wxSizerItem::IsSizer()
{
    return (m_sizer != NULL);
}

bool wxSizerItem::IsSpacer()
{
    return (m_window == NULL) && (m_sizer == NULL);
}

//---------------------------------------------------------------------------
// wxSizer
//---------------------------------------------------------------------------

wxSizer::wxSizer()
{
    m_children.DeleteContents( TRUE );
}

wxSizer::~wxSizer()
{
}
   
void wxSizer::Add( wxWindow *window, int option )
{
    m_children.Append( new wxSizerItem( window, option ) );
}

void wxSizer::Add( wxSizer *sizer, int option )
{
    m_children.Append( new wxSizerItem( sizer, option ) );
}

void wxSizer::Add( int width, int height, int option )
{
    m_children.Append( new wxSizerItem( width, height, option ) );
}

void wxSizer::Fit( wxWindow *window );
{
    window->SetSize( GetMinWindowSize( window ) );
}

void wxSizer::SetSizeHints( wxWindow *window );
{
    wxSize size( GetMinWindowSize( window ) );
    window->SetSizeHints( size.x, size.y );
}

wxSize wxSizer::GetMinWindowSize( wxWindow *window )
{
    wxSize min( GetMinSize() );
    wxSize size( window->GetSize() );
    wxSize client_size( window->GetClientSize() );
    return wxSize( min.x+size.x-client_size.x, min.y+size.y-client_size.y ); 
}

void wxSizer::DoSetDimension( int x, int y, int width, int height )
{
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    RecalcSizes();
}

#endif
  // __SIZERS_H__
