/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.cpp
// Purpose:     provide new wxSizer class for layout
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
#include "wx/statbox.h"
#include "wx/notebook.h"

//---------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxSizerItem, wxObject);
IMPLEMENT_ABSTRACT_CLASS(wxSizer, wxObject);
IMPLEMENT_ABSTRACT_CLASS(wxBoxSizer, wxSizer);
IMPLEMENT_ABSTRACT_CLASS(wxStaticBoxSizer, wxBoxSizer);
#if wxUSE_NOTEBOOK
IMPLEMENT_ABSTRACT_CLASS(wxNotebookSizer, wxSizer);
#endif

//---------------------------------------------------------------------------
// wxSizerItem
//---------------------------------------------------------------------------

wxSizerItem::wxSizerItem( int width, int height, int option, int flag, int border, wxObject* userData )
{
    m_window = (wxWindow *) NULL;
    m_sizer = (wxSizer *) NULL;
    m_option = option;
    m_border = border;
    m_flag = flag;
    m_userData = userData;

    // minimal size is the initial size
    m_minSize.x = width;
    m_minSize.y = height;

    SetRatio(width, height);

    // size is set directly
    m_size = m_minSize;
}

wxSizerItem::wxSizerItem( wxWindow *window, int option, int flag, int border, wxObject* userData )
{
    m_window = window;
    m_sizer = (wxSizer *) NULL;
    m_option = option;
    m_border = border;
    m_flag = flag;
    m_userData = userData;

    // minimal size is the initial size
    m_minSize = window->GetSize();

    // aspect ratio calculated from initial size
    SetRatio(m_minSize);

    // size is calculated later
    // m_size = ...
}

wxSizerItem::wxSizerItem( wxSizer *sizer, int option, int flag, int border, wxObject* userData )
{
    m_window = (wxWindow *) NULL;
    m_sizer = sizer;
    m_option = option;
    m_border = border;
    m_flag = flag;
    m_userData = userData;

    // minimal size is calculated later
    // m_minSize = ...
    m_ratio = 0;

    // size is calculated later
    // m_size = ...
}

wxSizerItem::~wxSizerItem()
{
    if (m_userData)
        delete m_userData;
    if (m_sizer)
        delete m_sizer;
}


wxSize wxSizerItem::GetSize()
{
    wxSize ret;
    if (IsSizer())
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

wxSize wxSizerItem::CalcMin()
{
    wxSize ret;
    if (IsSizer())
    {
        ret = m_sizer->CalcMin();
        // if we have to preserve aspect ratio _AND_ this is
        // the first-time calculation, consider ret to be initial size
        if ((m_flag & wxSHAPED) && !m_ratio) SetRatio(ret);
    }

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

void wxSizerItem::SetDimension( wxPoint pos, wxSize size )
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
    if (m_flag & wxSHAPED) {
        // adjust aspect ratio
        int rwidth = (int) (size.y * m_ratio);
        if (rwidth > size.x) {
            // fit horizontally
            int rheight = (int) (size.x / m_ratio);
            // add vertical space
            if (m_flag & wxALIGN_CENTER_VERTICAL)
                pos.y += (size.y - rheight) / 2;
            else if (m_flag & wxALIGN_BOTTOM)
                pos.y += (size.y - rheight);
            // use reduced dimensions
            size.y =rheight;
        } else if (rwidth < size.x) {
            // add horizontal space
            if (m_flag & wxALIGN_CENTER_HORIZONTAL)
                pos.x += (size.x - rwidth) / 2;
            else if (m_flag & wxALIGN_RIGHT)
                pos.x += (size.x - rwidth);
            size.x = rwidth;
        }
    }

    if (IsSizer())
        m_sizer->SetDimension( pos.x, pos.y, size.x, size.y );

    if (IsWindow())
        m_window->SetSize( pos.x, pos.y, size.x, size.y, wxSIZE_ALLOW_MINUS_ONE );

    m_size = size;
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

void wxSizer::Add( wxWindow *window, int option, int flag, int border, wxObject* userData )
{
    m_children.Append( new wxSizerItem( window, option, flag, border, userData ) );
}

void wxSizer::Add( wxSizer *sizer, int option, int flag, int border, wxObject* userData )
{
    m_children.Append( new wxSizerItem( sizer, option, flag, border, userData ) );
}

void wxSizer::Add( int width, int height, int option, int flag, int border, wxObject* userData )
{
    m_children.Append( new wxSizerItem( width, height, option, flag, border, userData ) );
}

void wxSizer::Prepend( wxWindow *window, int option, int flag, int border, wxObject* userData )
{
    m_children.Insert( new wxSizerItem( window, option, flag, border, userData ) );
}

void wxSizer::Prepend( wxSizer *sizer, int option, int flag, int border, wxObject* userData )
{
    m_children.Insert( new wxSizerItem( sizer, option, flag, border, userData ) );
}

void wxSizer::Prepend( int width, int height, int option, int flag, int border, wxObject* userData )
{
    m_children.Insert( new wxSizerItem( width, height, option, flag, border, userData ) );
}

void wxSizer::Insert( int before, wxWindow *window, int option, int flag, int border, wxObject* userData )
{
    m_children.Insert( before, new wxSizerItem( window, option, flag, border, userData ) );
}

void wxSizer::Insert( int before, wxSizer *sizer, int option, int flag, int border, wxObject* userData )
{
    m_children.Insert( before, new wxSizerItem( sizer, option, flag, border, userData ) );
}

void wxSizer::Insert( int before, int width, int height, int option, int flag, int border, wxObject* userData )
{
    m_children.Insert( before, new wxSizerItem( width, height, option, flag, border, userData ) );
}

bool wxSizer::Remove( wxWindow *window )
{
    wxASSERT( window );

    wxNode *node = m_children.First();
    while (node)
    {
        wxSizerItem *item = (wxSizerItem*)node->Data();
	    if (item->GetWindow() == window)
	    {
            m_children.DeleteNode( node );
	       return TRUE;
	    }
        node = node->Next();
    }

    return FALSE;
}

bool wxSizer::Remove( wxSizer *sizer )
{
    wxASSERT( sizer );

    wxNode *node = m_children.First();
    while (node)
    {
        wxSizerItem *item = (wxSizerItem*)node->Data();
	    if (item->GetSizer() == sizer)
	    {
            m_children.DeleteNode( node );
	        return TRUE;
	    }
        node = node->Next();
    }

    return FALSE;
}

bool wxSizer::Remove( int pos )
{
    wxNode *node = m_children.Nth( pos );
    if (!node) return FALSE;

    m_children.DeleteNode( node );

    return TRUE;
}

void wxSizer::Fit( wxWindow *window )
{
    window->SetSize( GetMinWindowSize( window ) );
}

void wxSizer::Layout()
{
    CalcMin();
    RecalcSizes();
}

void wxSizer::SetSizeHints( wxWindow *window )
{
    wxSize size( GetMinWindowSize( window ) );
    window->SetSizeHints( size.x, size.y );
}

wxSize wxSizer::GetMinWindowSize( wxWindow *window )
{
    wxSize minSize( GetMinSize() );
    wxSize size( window->GetSize() );
    wxSize client_size( window->GetClientSize() );
    return wxSize( minSize.x+size.x-client_size.x,
                   minSize.y+size.y-client_size.y );
}

void wxSizer::SetDimension( int x, int y, int width, int height )
{
    m_position.x = x;
    m_position.y = y;
    m_size.x = width;
    m_size.y = height;
    CalcMin();
    RecalcSizes();
}

//---------------------------------------------------------------------------
// wxBoxSizer
//---------------------------------------------------------------------------

wxBoxSizer::wxBoxSizer( int orient )
{
    m_orient = orient;
}

void wxBoxSizer::RecalcSizes()
{
    if (m_children.GetCount() == 0)
        return;

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
        wxSizerItem *item = (wxSizerItem*) node->Data();

	    int weight = 1;
	    if (item->GetOption())
	        weight = item->GetOption();

	    wxSize size( item->CalcMin() );

	    if (m_orient == wxVERTICAL)
	    {
	        wxCoord height = size.y;
	        if (item->GetOption())
	        {
	            height = (delta * weight) + extra;
		        extra = 0; // only the first item will get the remainder as extra size
	        }

	        wxPoint child_pos( pt );
	        wxSize  child_size( wxSize( size.x, height) );

	        if (item->GetFlag() & (wxEXPAND | wxSHAPED))
	            child_size.x = m_size.x;
	        else if (item->GetFlag() & wxALIGN_RIGHT)
	            child_pos.x += m_size.x - size.x;
	        else if (item->GetFlag() & (wxCENTER | wxALIGN_CENTER_HORIZONTAL))
	        // XXX wxCENTER is added for backward compatibility;
	        //     wxALIGN_CENTER should be used in new code
	            child_pos.x += (m_size.x - size.x) / 2;

	        item->SetDimension( child_pos, child_size );

	        pt.y += height;
	    }
	    else
	    {
	        wxCoord width = size.x;
	        if (item->GetOption())
	        {  
	            width = (delta * weight) + extra;
		        extra = 0; // only the first item will get the remainder as extra size
	        }

	        wxPoint child_pos( pt );
	        wxSize  child_size( wxSize(width, size.y) );

	        if (item->GetFlag() & (wxEXPAND | wxSHAPED))
	            child_size.y = m_size.y;
	        else if (item->GetFlag() & wxALIGN_BOTTOM)
	            child_pos.y += m_size.y - size.y;
	        else if (item->GetFlag() & (wxCENTER | wxALIGN_CENTER_VERTICAL))
	        // XXX wxCENTER is added for backward compatibility;
	        //     wxALIGN_CENTER should be used in new code
	            child_pos.y += (m_size.y - size.y) / 2;

	        item->SetDimension( child_pos, child_size );

	        pt.x += width;
	    }

	    node = node->Next();
    }
}

wxSize wxBoxSizer::CalcMin()
{
    if (m_children.GetCount() == 0)
        return wxSize(10,10);

    m_stretchable = 0;
    m_minWidth = 0;
    m_minHeight = 0;
    m_fixedWidth = 0;
    m_fixedHeight = 0;

    wxNode *node = m_children.GetFirst();
    while (node)
    {
        wxSizerItem *item = (wxSizerItem*) node->Data();

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

//---------------------------------------------------------------------------
// wxStaticBoxSizer
//---------------------------------------------------------------------------

wxStaticBoxSizer::wxStaticBoxSizer( wxStaticBox *box, int orient )
  : wxBoxSizer( orient )
{
    wxASSERT_MSG( box, wxT("wxStaticBoxSizer needs a static box") );

    m_staticBox = box;
}

void wxStaticBoxSizer::RecalcSizes()
{
    // this will have to be done platform by platform
    // as there is no way to guess the thickness of
    // a wxStaticBox border
    int top_border = 15;
    if (m_staticBox->GetLabel().IsEmpty()) top_border = 5;
    int other_border = 5;

    m_staticBox->SetSize( m_position.x, m_position.y, m_size.x, m_size.y );

    wxPoint old_pos( m_position );
    m_position.x += other_border;
    m_position.y += top_border;
    wxSize old_size( m_size );
    m_size.x -= 2*other_border;
    m_size.y -= top_border + other_border;

    wxBoxSizer::RecalcSizes();

    m_position = old_pos;
    m_size = old_size;
}

wxSize wxStaticBoxSizer::CalcMin()
{
    // This will have to be done platform by platform
    // as there is no way to guess the thickness of
    // a wxStaticBox border.
    
    int top_border = 15;
    if (m_staticBox->GetLabel().IsEmpty()) top_border = 5;
    int other_border = 5;

    wxSize ret( wxBoxSizer::CalcMin() );
    ret.x += 2*other_border;
    ret.y += other_border + top_border;

    return ret;
}

//---------------------------------------------------------------------------
// wxNotebookSizer
//---------------------------------------------------------------------------

#if wxUSE_NOTEBOOK

wxNotebookSizer::wxNotebookSizer( wxNotebook *nb )
{
    wxASSERT_MSG( nb, wxT("wxNotebookSizer needs a notebook") );
    
    m_notebook = nb;
}

void wxNotebookSizer::RecalcSizes()
{
    m_notebook->SetSize( m_position.x, m_position.y, m_size.x, m_size.y );
}

wxSize wxNotebookSizer::CalcMin()
{
    // This will have to be done platform by platform
    // as there is no way to guess the thickness of
    // the wxNotebook tabs and border.
    
    int borderX = 5;
    int borderY = 5;
    if ((m_notebook->HasFlag(wxNB_RIGHT)) ||
        (m_notebook->HasFlag(wxNB_LEFT)))
    {
        borderX += 70; // improvements later..
    }
    else
    {
        borderY += 35; // improvements later..
    }
    
    if (m_notebook->GetChildren().GetCount() == 0)
        return wxSize(borderX + 10, borderY + 10);

    int maxX = 0;
    int maxY = 0;

    wxWindowList::Node *node = m_notebook->GetChildren().GetFirst();
    while (node)
    {
        wxWindow *item = node->GetData();
	    wxSizer *itemsizer = item->GetSizer();
	
	    if (itemsizer)
	    {
            wxSize subsize( itemsizer->CalcMin() );
	
	        if (subsize.x > maxX) maxX = subsize.x;
	        if (subsize.y > maxY) maxY = subsize.y;
	    }

	    node = node->GetNext();
    }

    return wxSize( borderX + maxX, borderY + maxY );
}

#endif // wxUSE_NOTEBOOK
