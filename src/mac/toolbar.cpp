/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "toolbar.h"
#endif

#include "wx/wx.h"

#if wxUSE_TOOLBAR

#include "wx/toolbar.h"

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
END_EVENT_TABLE()

#include <wx/mac/uma.h>

wxToolBar::wxToolBar()
{
  m_maxWidth = -1;
  m_maxHeight = -1;
  m_defaultWidth = 24;
  m_defaultHeight = 22;
  // TODO
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
  m_maxWidth = m_maxHeight = 0;

  m_defaultWidth = 24;
  m_defaultHeight = 22;
  
  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  if (width <= 0)
    width = 100;
  if (height <= 0)
    height = 30;
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , wxPoint( x , y ) , wxSize( width , height ) ,style, *((wxValidator*)NULL) , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , "\p" , true , 0 , 0 , 1, 
	  	kControlPlacardProc , (long) this ) ;
	MacPostControlCreate() ;

  return TRUE;
}

wxToolBar::~wxToolBar()
{
    // TODO
}

PicHandle MakePict(GWorldPtr wp)
{
	CGrafPtr		origPort ;
	GDHandle		origDev ;
	
	PicHandle		pict;				// this is the Picture we give back

	RGBColor		gray = { 0xCCCC ,0xCCCC , 0xCCCC } ;
	
	GetGWorld( &origPort , &origDev ) ;
	SetGWorld( wp , NULL ) ;
	
	pict = OpenPicture(&wp->portRect);	// open a picture, this disables drawing
	if(!pict)	
		return NULL;
	
	RGBBackColor( &gray ) ;
	EraseRect(&wp->portRect) ;
	CopyBits((BitMap*)*wp->portPixMap,			// src PixMap	- we copy image over itself -
				(BitMap*)*wp->portPixMap,		// dst PixMap	- no drawing occurs -
				&wp->portRect,			// srcRect		- it will be recorded and compressed -
				&wp->portRect,			// dstRect		- into the picture that is open -
				srcCopy,NULL);			// copyMode and no clip region

	ClosePicture();						// We are done recording the picture
	SetGWorld( origPort , origDev ) ;
	return pict;						// return our groovy pict handle
}

PicHandle MakePictWhite(GWorldPtr wp)
{
	CGrafPtr		origPort ;
	GDHandle		origDev ;
	
	PicHandle		pict;				// this is the Picture we give back

	RGBColor		white = { 0xFFFF ,0xFFFF  , 0xFFFF  } ;
	
	GetGWorld( &origPort , &origDev ) ;
	SetGWorld( wp , NULL ) ;
	
	pict = OpenPicture(&wp->portRect);	// open a picture, this disables drawing
	if(!pict)	
		return NULL;
	
	RGBBackColor( &white ) ;
	EraseRect(&wp->portRect) ;
	CopyBits((BitMap*)*wp->portPixMap,			// src PixMap	- we copy image over itself -
				(BitMap*)*wp->portPixMap,		// dst PixMap	- no drawing occurs -
				&wp->portRect,			// srcRect		- it will be recorded and compressed -
				&wp->portRect,			// dstRect		- into the picture that is open -
				srcCopy,NULL);			// copyMode and no clip region

	ClosePicture();						// We are done recording the picture
	SetGWorld( origPort , origDev ) ;
	return pict;						// return our groovy pict handle
}

const short kwxMacToolBarTopMargin = 2 ;
const short kwxMacToolBarLeftMargin = 2 ;

bool wxToolBar::CreateTools()
{
  if (m_tools.Number() == 0)
      return FALSE;

	Rect toolbarrect = { m_y , m_x , m_y + m_height , m_x + m_width } ;
	ControlFontStyleRec		controlstyle ;
	WindowPtr				window = GetMacRootWindow() ;
	controlstyle.flags = kControlUseFontMask ;
	controlstyle.font = kControlFontSmallSystemFont ;
	
	wxNode *node = m_tools.First();
	int noButtons = 0;
	int x = 0 ;
	wxSize toolSize = GetToolSize() ;
    int tw, th;
    GetSize(& tw, & th);
	while (node)
	{
		wxToolBarTool *tool = (wxToolBarTool *)node->Data();
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( tool->m_bitmap1.GetRefData()) ;
		
		if( tool->m_toolStyle != wxTOOL_STYLE_SEPARATOR )
		{
			Rect toolrect = { toolbarrect.top + kwxMacToolBarTopMargin , toolbarrect.left + x + kwxMacToolBarLeftMargin , 0 , 0 } ;
			toolrect.right = toolrect.left + toolSize.x ;
			toolrect.bottom = toolrect.top + toolSize.y ;
			
			PicHandle	icon = NULL ;
			if ( bmap )
			{
				if ( bmap->m_bitmapType == kMacBitmapTypePict )
					icon = bmap->m_hPict ;
				else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
				{
					icon = MakePict( bmap->m_hBitmap ) ;
				}
			}
			
			ControlHandle m_macToolHandle ;
				
			if ( icon )
			{
				m_macToolHandle = UMANewControl( window , &toolrect , "\p" , true , 0 , 
					kControlBehaviorOffsetContents + kControlContentPictHandle , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
				ControlButtonContentInfo info ;
				
				info.contentType = kControlContentPictHandle ;
				info.u.picture = icon ;
				
				UMASetControlData( m_macToolHandle , kControlButtonPart , kControlBevelButtonContentTag , sizeof(info) , (char*) &info ) ;
			}
			else
			{
						m_macToolHandle = UMANewControl( window , &toolrect , "\p" , true , 0 , 
						kControlBehaviorOffsetContents  , 0 , kControlBevelButtonNormalBevelProc , (long) this ) ;
			}
			m_macToolHandles.Add( m_macToolHandle ) ;
			UMASetControlFontStyle( m_macToolHandle , &controlstyle ) ;
			UMAEmbedControl( m_macToolHandle , m_macControl ) ;
			
			x += (int)toolSize.x;
			noButtons ++;
		}
		else
		{
			m_macToolHandles.Add( NULL ) ;
			x += (int)toolSize.x / 4;
		}
	    if ( toolbarrect.left + x + kwxMacToolBarLeftMargin > m_maxWidth)
    	  	m_maxWidth = toolbarrect.left + x + kwxMacToolBarLeftMargin;
    	if (toolbarrect.top + kwxMacToolBarTopMargin + toolSize.y > m_maxHeight)
      		m_maxHeight = toolbarrect.top + kwxMacToolBarTopMargin ;

		node = node->Next();
	}

  if ( GetWindowStyleFlag() & wxTB_HORIZONTAL )
  {
    m_maxWidth = tw ; // +=toolSize.x;
    m_maxHeight += toolSize.y;
  	m_maxHeight += m_yMargin;
  }
  else
  {
    m_maxHeight = th ;// += toolSize.y;
    m_maxWidth += toolSize.x;
  	m_maxWidth += m_xMargin;
  }

  SetSize(m_maxWidth, m_maxHeight);

  return TRUE;
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x; m_defaultHeight = size.y;
}

wxSize wxToolBar::GetMaxSize() const
{
    return wxSize(m_maxWidth, m_maxHeight);
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

void wxToolBar::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	int index = 0 ;
	for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
	{
		if ( m_macToolHandles[index] == (void*) control )
		{
			OnLeftClick( ( (wxToolBarTool*) (m_tools.Nth( index )->Data() ) ) ->m_index , ( (wxToolBarTool*) (m_tools.Nth( index )->Data() ) ) ->m_toggleState ) ;
		}
	}
}

void wxToolBar::EnableTool(int toolIndex, bool enable)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        tool->m_enabled = enable;
        // TODO enable button
    }
}

void wxToolBar::ToggleTool(int toolIndex, bool toggle)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if (tool->m_isToggle)
        {
            tool->m_toggleState = toggle;
            // TODO: set toggle state
        }
    }
}

void wxToolBar::ClearTools()
{
    // TODO
    wxToolBarBase::ClearTools();
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.

wxToolBarTool *wxToolBar::AddTool(int index, const wxBitmap& bitmap, const wxBitmap& pushedBitmap,
             bool toggle, long xPos, long yPos, wxObject *clientData, const wxString& helpString1, const wxString& helpString2)
{
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, wxNullBitmap, toggle, xPos, yPos, helpString1, helpString2);
  tool->m_clientData = clientData;

  if (xPos > -1)
    tool->m_x = xPos;
  else
    tool->m_x = m_xMargin;

  if (yPos > -1)
    tool->m_y = yPos;
  else
    tool->m_y = m_yMargin;

  tool->SetSize(GetToolSize().x, GetToolSize().y);

  if ((tool->m_x + bitmap.GetWidth() + m_xMargin) > m_maxWidth)
    m_maxWidth = (tool->m_x + tool->GetWidth() + m_xMargin);

  if ((tool->m_y + bitmap.GetHeight() + m_yMargin) > m_maxHeight)
    m_maxHeight = (tool->m_y + tool->GetHeight() + m_yMargin);

  m_tools.Append((long)index, tool);
  return tool;
}

#endif // wxUSE_TOOLBAR

