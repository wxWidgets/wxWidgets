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

#ifdef __GNUG__
#pragma implementation "sizer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/sizer.h"
#include "wx/utils.h"

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
    wxSize minSize( GetMinSize() );
    wxSize size( window->GetSize() );
    wxSize client_size( window->GetClientSize() );
    return wxSize( minSize.x+size.x-client_size.x,
                   minSize.y+size.y-client_size.y ); 
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

//---------------------------------------------------------------------------
// wxBoxNewSizer
//---------------------------------------------------------------------------

wxBoxNewSizer::wxBoxNewSizer( int orient )
{
    m_orient = orient;
}

void wxBoxNewSizer::RecalcSizes()
{
    if (m_children.GetCount() == 0)
    {
        SetDimension( m_position.x, m_position.y, 2, 2 );
        return;
    }
    
    int delta = 0;
    int extra = 0;
    if (m_stretchable)
    {
        if (m_orient == wxHORIZONTAL)
        {
            delta = (m_size.x - m_fixedWidth) / m_stretchable;
            extra = (m_size.x - m_fixedWidth) % m_stretchable;
	}
	else
	{
            delta = (m_size.y - m_fixedHeight) / m_stretchable;
            extra = (m_size.y - m_fixedHeight) % m_stretchable;
	}
    }
    
    wxPoint pt( m_position );
    
    wxNode *node = m_children.GetFirst();
    while (node)
    {
        wxNewSizerItem *item = (wxNewSizerItem*) node->Data();

	int weight = 1;
	if (item->GetOption())
	    weight = item->GetOption();
	
	wxSize size( item->CalcMin() );
	
	if (m_orient == wxVERTICAL)
	{
	    long height = size.y;
	    if (item->GetOption())
	    {
	        height = (delta * weight) + extra;
		extra = 0; // only the first item will get the remainder as extra size
	    }
	    item->SetDimension( pt, wxSize( size.x, height) );
	    pt.y += height;
	}
	else
	{
	    long width = size.x;
	    if (item->GetOption())
	    {
	        width = (delta * weight) + extra;
		extra = 0; // only the first item will get the remainder as extra size
	    }
	    item->SetDimension( pt, wxSize(width, size.y) );
	    pt.x += width;
	}

	node = node->Next();
    }
}

wxSize wxBoxNewSizer::CalcMin()
{
    if (m_children.GetCount() == 0)
        return wxSize(2,2);
    
    m_stretchable = 0;
    m_minWidth = 0;
    m_minHeight = 0;
    m_fixedWidth = 0;
    m_fixedHeight = 0;
    
    wxNode *node = m_children.GetFirst();
    while (node)
    {
        wxNewSizerItem *item = (wxNewSizerItem*) node->Data();
	
	int weight = 1;
	if (item->GetOption())
	    weight = item->GetOption();
	
	wxSize size( item->CalcMin() );
	
	if (m_orient == wxHORIZONTAL)
	{
	    m_minWidth += (size.x * weight);
	    m_minHeight = wxMax( m_minHeight, size.y );
	}
	else
	{
	    m_minHeight += (size.y * weight);
	    m_minWidth = wxMax( m_minWidth, size.x );
	}
	
	if (item->GetOption())
	{
	    m_stretchable += weight;
	}
	else
	{
	    if (m_orient == wxVERTICAL)
	    {
		m_fixedHeight += size.y;
		m_fixedWidth = wxMax( m_fixedWidth, size.x );
	    }
	    else
	    {
		m_fixedWidth += size.x;
		m_fixedHeight = wxMax( m_fixedHeight, size.y );
	    }
	}
	
	node = node->Next();
    }
	
    return wxSize( m_minWidth, m_minHeight );
}
