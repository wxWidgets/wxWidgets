/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.cpp
// Purpose:     provide wxNewSizer class for layounting
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
#pragma implementation "sizer.h"
#endif

#include "wx/sizer.h"


//---------------------------------------------------------------------------
// wxNewSizerItem
//---------------------------------------------------------------------------

wxNewSizerItem::wxNewSizerItem( int width, int height, int option )
{
    m_window = (wxWindow *) NULL;
    m_sizer = (wxNewSizer *) NULL;
    m_minSize.x = width;
    m_minSize.y = height;
    m_option = option;
}

wxNewSizerItem::wxNewSizerItem( wxWindow *window, int option )
{
    m_window = window;
    m_sizer = (wxNewSizer *) NULL;
    m_minSize = window->GetSize();
    m_option = option;
}

wxNewSizerItem::wxNewSizerItem( wxNewSizer *sizer, int option )
{
    m_window = (wxWindow *) NULL;
    m_sizer = sizer;
    m_minSize = sizer->GetSize();
    m_option = option;
}

wxSize wxNewSizerItem::GetSize()
{
    if (IsNewSizer())
        return m_sizer->GetSize();
	
    if (IsWindow())
        return m_window->GetSize();
	
    return m_minSize;
}

wxSize wxNewSizerItem::CalcMin()
{
    if (IsNewSizer())
        return m_sizer->CalcMin();
	
    if (IsWindow())
        return m_window->GetSize();
	
    return m_minSize;
}

void wxNewSizerItem::SetDimension( wxPoint pos, wxSize size )
{
    if (IsNewSizer())
        m_sizer->SetDimension( pos.x, pos.y, size.x, size.y );
	
    if (IsWindow())
        m_window->SetSize( pos.x, pos.y, size.x, size.y );
	
    m_minSize = size;
}

bool wxNewSizerItem::IsWindow()
{
    return (m_window != NULL);
}

bool wxNewSizerItem::IsNewSizer()
{
    return (m_sizer != NULL);
}

bool wxNewSizerItem::IsSpacer()
{
    return (m_window == NULL) && (m_sizer == NULL);
}

//---------------------------------------------------------------------------
// wxNewSizer
//---------------------------------------------------------------------------

wxNewSizer::wxNewSizer()
{
    m_children.DeleteContents( TRUE );
}

wxNewSizer::~wxNewSizer()
{
}
   
void wxNewSizer::Add( wxWindow *window, int option )
{
    m_children.Append( new wxNewSizerItem( window, option ) );
}

void wxNewSizer::Add( wxNewSizer *sizer, int option )
{
    m_children.Append( new wxNewSizerItem( sizer, option ) );
}

void wxNewSizer::Add( int width, int height, int option )
{
    m_children.Append( new wxNewSizerItem( width, height, option ) );
}

void wxNewSizer::Fit( wxWindow *window )
{
    window->SetSize( GetMinWindowSize( window ) );
}

void wxNewSizer::Layout()
{
    m_size = CalcMin();
    RecalcSizes();
}

void wxNewSizer::SetSizeHints( wxWindow *window )
{
    wxSize size( GetMinWindowSize( window ) );
    window->SetSizeHints( size.x, size.y );
}

wxSize wxNewSizer::GetMinWindowSize( wxWindow *window )
{
    wxSize min( GetMinSize() );
    wxSize size( window->GetSize() );
    wxSize client_size( window->GetClientSize() );
    return wxSize( min.x+size.x-client_size.x, min.y+size.y-client_size.y ); 
}

void wxNewSizer::SetDimension( int x, int y, int width, int height )
{
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    RecalcSizes();
}

//---------------------------------------------------------------------------
// wxBorderNewSizer
//---------------------------------------------------------------------------

wxBorderNewSizer::wxBorderNewSizer( int sides )
{
    m_sides = sides;
}

void wxBorderNewSizer::Add( wxWindow *window, int option )
{
   wxCHECK_RET( m_children.GetCount() == 0, _T("border sizer can only hold one child") );
   
   wxNewSizer::Add( window, option );
}

void wxBorderNewSizer::Add( wxNewSizer *sizer, int option )
{
   wxCHECK_RET( m_children.GetCount() == 0, _T("border sizer can only hold one child") );
   
   wxNewSizer::Add( sizer, option );
}

void wxBorderNewSizer::Add( int width, int height, int option )
{
   wxCHECK_RET( m_children.GetCount() == 0, _T("border sizer can only hold one child") );
   
   wxNewSizer::Add( width, height, option );
}

void wxBorderNewSizer::RecalcSizes()
{
    wxNode *node = m_children.GetFirst();
    
    if (!node)
    {
        SetDimension( m_position.x, m_position.y, 2, 2 );
        return;
    }
    
    wxNewSizerItem *item = (wxNewSizerItem*) node->Data();
    
    wxSize size( m_size );
    wxPoint pt( m_position );
    int borderSize = item->GetOption();
    
    if (m_sides & wxWEST)
    {
        size.x -= borderSize;
        pt.x += borderSize;
    }
    if (m_sides & wxEAST)
    {
        size.x -= borderSize;
    }
    if (m_sides & wxNORTH)
    {
        size.y -= borderSize;
	pt.y += borderSize;
    }
    if (m_sides & wxSOUTH)
    {
        size.y -= borderSize;
    }
    
    item->SetDimension( pt, size );
}

wxSize wxBorderNewSizer::CalcMin()
{
    wxNode *node = m_children.GetFirst();
    
    if (!node)
        return wxSize(2,2);
    
    wxNewSizerItem *item = (wxNewSizerItem*) node->Data();
    
    wxSize size( item->CalcMin() );
    
    int borderSize = item->GetOption();
    
    if (m_sides & wxWEST)
        size.x += borderSize;
    if (m_sides & wxEAST)
        size.x += borderSize;
    if (m_sides & wxNORTH)
        size.y += borderSize;
    if (m_sides & wxSOUTH)
        size.y += borderSize;
	
    return size;
}

#endif
  // __SIZERS_H__
