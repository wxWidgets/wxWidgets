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

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
        m_nSepCount = 0;
        m_index = 0 ;
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        m_nSepCount = 1;
        m_index = 0 ;
    }

    // set/get the number of separators which we use to cover the space used by
    // a control in the toolbar
    void SetSeparatorsCount(size_t count) { m_nSepCount = count; }
    size_t GetSeparatorsCount() const { return m_nSepCount; }

    int		m_index ;
private:
    size_t m_nSepCount;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         bool toggle,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, bitmap1, bitmap2, toggle,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

// ----------------------------------------------------------------------------
// wxToolBar construction
// ----------------------------------------------------------------------------

void wxToolBar::Init()
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

bool wxToolBar::Realize()
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
		wxBitmapRefData * bmap = (wxBitmapRefData*) ( tool->GetBitmap1().GetRefData()) ;
		
		if( !tool->IsSeparator() )
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
			OnLeftClick( ( (wxToolBarTool*) (m_tools.Nth( index )->Data() ) ) ->m_index , ( (wxToolBarTool*) (m_tools.Nth( index )->Data() ) ) ->IsToggled() ) ;
		}
	}
}

void wxToolBar::SetRows(int nRows)
{
    if ( nRows == m_maxRows )
    {
        // avoid resizing the frame uselessly
        return;
    }

    m_maxRows = nRows;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    MacClientToRootWindow( &x , &y ) ;
    Point pt = { x ,y } ;

	int index = 0 ;
	for ( index = 0 ; index < m_macToolHandles.Count() ; ++index )
	{
		if ( PtInRect( pt , &(**(ControlHandle)(m_macToolHandles[index])).contrlRect) )
		{
			return  (wxToolBarTool*) (m_tools.Nth( index )->Data() ) ;
		}
	}

    return (wxToolBarToolBase *)NULL;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *t, bool enable)
{
        wxToolBarTool *tool = (wxToolBarTool *)t;
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *t, bool toggle)
{
       wxToolBarTool *tool = (wxToolBarTool *)t;
       // TODO: set toggle state
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase *tool)
{
    // nothing special to do here - we really create the toolbar buttons in
    // Realize() later
    tool->Attach(this);

    return TRUE;
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *t, bool toggle)
{
       wxToolBarTool *tool = (wxToolBarTool *)t;
       // TODO: set toggle state
}

bool wxToolBar::DoDeleteTool(size_t pos, wxToolBarToolBase *tool)
{
		return TRUE ;
}
#endif // wxUSE_TOOLBAR

