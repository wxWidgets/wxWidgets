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

wxNewSizerItem::wxNewSizerItem( int width, int height, int option, int flag, int border )
{
    m_window = (wxWindow *) NULL;
    m_sizer = (wxNewSizer *) NULL;
    m_option = option;
    m_border = border;
    m_flag = flag;
    
    // minimal size is the initial size
    m_minSize.x = width;
    m_minSize.y = height;
    
    // size is set directly
    m_size = m_minSize;
}

wxNewSizerItem::wxNewSizerItem( wxWindow *window, int option, int flag, int border )
{
    m_window = window;
    m_sizer = (wxNewSizer *) NULL;
    m_option = option;
    m_border = border;
    m_flag = flag;
    
    // minimal size is the initial size
    m_minSize = window->GetSize();
    
    // size is calculated later
    // m_size = ...
}

wxNewSizerItem::wxNewSizerItem( wxNewSizer *sizer, int option, int flag, int border )
{
    m_window = (wxWindow *) NULL;
    m_sizer = sizer;
    m_option = option;
    m_border = border;
    m_flag = flag;
    
    // minimal size is calculated later
    // m_minSize = ...
   
    // size is calculated later
    // m_size = ...
}

wxSize wxNewSizerItem::GetSize()
{
    wxSize ret;
    if (IsNewSizer())
        ret = m_sizer->GetSize();
    else
    if (IsWindow())
        ret = m_window->GetSize();
    else ret = m_size;
    
    if (m_flag & wxWEST)
        ret.x += m_border;
    if (m_flag & wxEAST)
        ret.x += m_border;
    if (m_flag & wxNORTH)
        ret.y += m_border;
    if (m_flag & wxSOUTH)
        ret.y += m_border;
    
    return ret;
}

wxSize wxNewSizerItem::CalcMin()
{
    wxSize ret;
    if (IsNewSizer())
        ret = m_sizer->CalcMin();
/*  
    The minimum size of a window should be the
    initial size, as saved in m_minSize, not the
    current size.
    
    else
    if (IsWindow())
        ret = m_window->GetSize();
*/
    else ret = m_minSize;
    
    if (m_flag & wxWEST)
        ret.x += m_border;
    if (m_flag & wxEAST)
        ret.x += m_border;
    if (m_flag & wxNORTH)
        ret.y += m_border;
    if (m_flag & wxSOUTH)
        ret.y += m_border;
    
    return ret;
}

void wxNewSizerItem::SetDimension( wxPoint pos, wxSize size )
{
    if (m_flag & wxWEST)
    {
        pos.x += m_border;
        size.x -= m_border;
    }
    if (m_flag & wxEAST)
    {
        size.x -= m_border;
    }
    if (m_flag & wxNORTH)
    {
        pos.y += m_border;
        size.y -= m_border;
    }
    if (m_flag & wxSOUTH)
    {
        size.y -= m_border;
    }
	
    if (IsNewSizer())
        m_sizer->SetDimension( pos.x, pos.y, size.x, size.y );
	
    if (IsWindow())
        m_window->SetSize( pos.x, pos.y, size.x, size.y );

    m_size = size;
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
   
void wxNewSizer::Add( wxWindow *window, int option, int flag, int border )
{
    m_children.Append( new wxNewSizerItem( window, option, flag, border ) );
}

void wxNewSizer::Add( wxNewSizer *sizer, int option, int flag, int border )
{
    m_children.Append( new wxNewSizerItem( sizer, option, flag, border ) );
}

void wxNewSizer::Add( int width, int height, int option, int flag, int border )
{
    m_children.Append( new wxNewSizerItem( width, height, option, flag, border ) );
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
// wxBox
//---------------------------------------------------------------------------

wxBox::wxBox( int orient )
{
    m_orient = orient;
}

void wxBox::RecalcSizes()
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
	    
	    wxPoint child_pos( pt );
	    wxSize  child_size( wxSize( size.x, height) );
	    
	    if (item->GetFlag() & wxALIGN_RIGHT)
	      child_pos.x += m_size.x - size.x;
	    else if (item->GetFlag() & wxCENTER)
	      child_pos.x += (m_size.x - size.x) / 2;
	    else if (item->GetFlag() & wxEXPAND)
	      child_size.x = m_size.x;
	      
	    item->SetDimension( child_pos, child_size );
	    
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
	    
	    wxPoint child_pos( pt );
	    wxSize  child_size( wxSize(width, size.y) );
	    
	    if (item->GetFlag() & wxALIGN_BOTTOM)
	      child_pos.y += m_size.y - size.y;
	    else if (item->GetFlag() & wxCENTER)
	      child_pos.y += (m_size.y - size.y) / 2;
	    else if (item->GetFlag() & wxEXPAND)
	      child_size.y = m_size.y;
	      
	    item->SetDimension( child_pos, child_size );
	    
	    pt.x += width;
	}

	node = node->Next();
    }
}

wxSize wxBox::CalcMin()
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
