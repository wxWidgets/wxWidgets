/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/palette.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"

extern "C" 
{
	#include "xpm.h"
} ;

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)
#endif

#include <PictUtils.h>

CTabHandle wxMacCreateColorTable( int numColors )
{
	CTabHandle newColors; /* Handle to the new color table */
	short index; /* Index into the table of colors */
	/* Allocate memory for the color table */
	newColors = (CTabHandle)NewHandleClear( sizeof (ColorTable) +
	sizeof (ColorSpec) * (numColors - 1) );
	if (newColors != nil)
	{
		/* Initialize the fields */
		(**newColors).ctSeed = GetCTSeed();
		(**newColors).ctFlags = 0;
		(**newColors).ctSize = numColors - 1;
		/* Initialize the table of colors */
	}
	return newColors ;
}

void wxMacDestroyColorTable( CTabHandle colors ) 
{
	DisposeHandle( (Handle) colors ) ;
}

void wxMacSetColorTableEntry( CTabHandle newColors , int index , int red , int green ,  int blue )
{
	(**newColors).ctTable[index].value = index;
	(**newColors).ctTable[index].rgb.red = 0 ;// someRedValue;
	(**newColors).ctTable[index].rgb.green = 0 ; // someGreenValue;
	(**newColors).ctTable[index].rgb.blue = 0 ; // someBlueValue;
}

GWorldPtr wxMacCreateGWorld( int height , int width , int depth )
{
	OSErr err = noErr ;
	GWorldPtr port ;
	Rect rect = { 0 , 0 , width , height } ;
	
	if ( depth < 0 )
	{
		// get max pixel depth
		CGrafPtr port ;
		GetCWMgrPort( &port ) ; 
		GDHandle maxDevice ;
		
		maxDevice = GetMaxDevice( &port->portRect ) ;
		if ( maxDevice )
			depth = (**((**maxDevice).gdPMap)).pixelSize ;
		else
			depth = 8 ; 
	}
		
	err = NewGWorld( &port , depth , &rect , NULL , NULL , 0 ) ;
	if ( err == noErr )
	{
		return port ;
	}
	return NULL ;
} 

void wxMacDestroyGWorld( GWorldPtr gw )
{
	if ( gw )
		DisposeGWorld( gw ) ;
}

wxBitmapRefData::wxBitmapRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_bitmapMask = NULL;
		m_hBitmap = NULL ;
		m_hPict = NULL ;
		m_bitmapType = kMacBitmapTypeUnknownType ;
}

wxBitmapRefData::~wxBitmapRefData()
{
	switch (m_bitmapType)
	{
		case kMacBitmapTypePict :
			{
				if ( m_hPict )
				{
					KillPicture( m_hPict ) ;
					m_hPict = NULL ;
				}
			}
			break ;
		case kMacBitmapTypeGrafWorld :
			{
				if ( m_hBitmap )
				{
					wxMacDestroyGWorld( m_hBitmap ) ;
					m_hBitmap = NULL ;
				}
			}
			break ;
		default :
			// unkown type ?
			break ;
	} ;
	
  if (m_bitmapMask)
  {
    delete m_bitmapMask;
    m_bitmapMask = NULL;
  }
}

wxList wxBitmap::sm_handlers;

wxBitmap::wxBitmap()
{
    m_refData = NULL;

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
}

wxBitmap::wxBitmap(const char bits[], int the_width, int the_height, int no_bits)
{
    m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = the_width ;
    M_BITMAPDATA->m_height = the_height ;
    M_BITMAPDATA->m_depth = no_bits ;
    M_BITMAPDATA->m_numColors = 0;
		if ( no_bits == 1 )
		{
	    M_BITMAPDATA->m_bitmapType = kMacBitmapTypeGrafWorld ;
	    M_BITMAPDATA->m_hBitmap = wxMacCreateGWorld( the_width , the_height , no_bits ) ;
			M_BITMAPDATA->m_ok = (M_BITMAPDATA->m_hBitmap != NULL ) ;
	
			CGrafPtr 	origPort ;
			GDHandle	origDevice ;
			
			GetGWorld( &origPort , &origDevice ) ;
			SetGWorld( M_BITMAPDATA->m_hBitmap , NULL ) ;
	
			// bits is a word aligned array
			
			unsigned char* linestart = (unsigned char*) bits ;
			int linesize = ( the_width / 16 ) * 2  ;
			if ( the_width % 16 )
			{
				linesize += 2 ;
			} ;
			
			RGBColor colors[2] = { 
				{ 0xFFFF , 0xFFFF , 0xFFFF } ,
				{ 0, 0 , 0 } 
				} ;
			
			for( int y = 0 ; y < the_height ; ++y , linestart += linesize )
			{
				for( int x = 0 ; x < the_width ; ++x )
				{
					int index = x / 8 ;
					int bit = x % 8 ;
					int mask = 1 << bit ;
					if ( linestart[index] & mask )
					{
						SetCPixel( x , y , &colors[1] ) ;
					}
					else
					{
						SetCPixel( x , y , &colors[0] ) ;
					}
				}
				
			}
	
	   	SetGWorld( origPort , origDevice ) ;
	   }
	   else
	   {
				//multicolor BITMAPs not yet implemented
	   }

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(void *data, long type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(const wxString& filename, long type)
{
    LoadFile(filename, (int)type);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(const char **data)
{
    (void) Create((void *)data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
    M_BITMAPDATA->m_height = h;
    M_BITMAPDATA->m_depth = d;

    M_BITMAPDATA->m_bitmapType = kMacBitmapTypeGrafWorld ;
    M_BITMAPDATA->m_hBitmap = wxMacCreateGWorld( w , h , d ) ;
		M_BITMAPDATA->m_ok = (M_BITMAPDATA->m_hBitmap != NULL ) ;
    return M_BITMAPDATA->m_ok;
}

void wxBitmap::SetHBITMAP(WXHBITMAP bmp)
{
    M_BITMAPDATA->m_bitmapType = kMacBitmapTypeGrafWorld ;
    M_BITMAPDATA->m_hBitmap = bmp ;
		M_BITMAPDATA->m_ok = (M_BITMAPDATA->m_hBitmap != NULL ) ;
}

bool wxBitmap::LoadFile(const wxString& filename, long type)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->LoadFile(this, filename, type, -1, -1);
}

bool wxBitmap::Create(void *data, long type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->Create(this, data, type, width, height, depth);
}

bool wxBitmap::SaveFile(const wxString& filename, int type, const wxPalette *palette)
{
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
  }

  return handler->SaveFile(this, filename, type, palette);
}

void wxBitmap::SetWidth(int w)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
}

void wxBitmap::SetHeight(int h)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_height = h;
}

void wxBitmap::SetDepth(int d)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_depth = d;
}

void wxBitmap::SetQuality(int q)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_quality = q;
}

void wxBitmap::SetOk(bool isOk)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_ok = isOk;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapPalette = palette ;
}

void wxBitmap::SetMask(wxMask *mask)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapMask = mask ;
}

void wxBitmap::AddHandler(wxBitmapHandler *handler)
{
    sm_handlers.Append(handler);
}

void wxBitmap::InsertHandler(wxBitmapHandler *handler)
{
    sm_handlers.Insert(handler);
}

bool wxBitmap::RemoveHandler(const wxString& name)
{
    wxBitmapHandler *handler = FindHandler(name);
    if ( handler )
    {
        sm_handlers.DeleteObject(handler);
        return TRUE;
    }
    else
        return FALSE;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& name)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetName() == name )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& extension, long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetExtension() == extension &&
                    (bitmapType == -1 || handler->GetType() == bitmapType) )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if (handler->GetType() == bitmapType)
            return handler;
        node = node->Next();
    }
    return NULL;
}

/*
 * wxMask
 */

wxMask::wxMask()
{
/* TODO
    m_maskBitmap = 0;
*/
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
/* TODO
    m_maskBitmap = 0;
*/
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
/* TODO
    m_maskBitmap = 0;
*/

    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
/* TODO
    m_maskBitmap = 0;
*/

    Create(bitmap);
}

wxMask::~wxMask()
{
// TODO: delete mask bitmap
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
// TODO
    return FALSE;
}

/*
 * wxBitmapHandler
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

bool wxBitmapHandler::Create(wxBitmap *bitmap, void *data, long type, int width, int height, int depth)
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long type,
        int desiredWidth, int desiredHeight)
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
{
    return FALSE;
}

/*
 * Standard handlers
 */

class WXDLLEXPORT wxPICTResourceHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxPICTResourceHandler)
public:
    inline wxPICTResourceHandler()
    {
        m_name = "Macintosh Pict resource";
        m_extension = "";
        m_type = wxBITMAP_TYPE_PICT_RESOURCE;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight);
};
IMPLEMENT_DYNAMIC_CLASS(wxPICTResourceHandler, wxBitmapHandler)

bool  wxPICTResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight)
{
	Str255 theName ;
	
	strcpy( (char*) theName , name ) ;
	c2pstr( (char*) theName ) ;
	
	PicHandle thePict = (PicHandle ) GetNamedResource( 'PICT' , theName ) ;
	if ( thePict )
	{
		PictInfo theInfo ;
		
		GetPictInfo( thePict , &theInfo , 0 , 0 , systemMethod , 0 ) ;
		DetachResource( (Handle) thePict ) ;
		M_BITMAPHANDLERDATA->m_bitmapType = kMacBitmapTypePict ;
		M_BITMAPHANDLERDATA->m_hPict = thePict ;
		M_BITMAPHANDLERDATA->m_width =  theInfo.sourceRect.right - theInfo.sourceRect.left ;
		M_BITMAPHANDLERDATA->m_height = theInfo.sourceRect.bottom - theInfo.sourceRect.top ;
		
		M_BITMAPHANDLERDATA->m_depth = theInfo.depth ;
		M_BITMAPHANDLERDATA->m_ok = true ;
		M_BITMAPHANDLERDATA->m_numColors = theInfo.uniqueColors ;
//		M_BITMAPHANDLERDATA->m_bitmapPalette;
//		M_BITMAPHANDLERDATA->m_quality;
		return TRUE ;
	}
	return FALSE ;
}

/* TODO: bitmap handlers, a bit like this:
class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxBMPResourceHandler)
public:
    inline wxBMPResourceHandler()
    {
        m_name = "Windows bitmap resource";
        m_extension = "";
        m_type = wxBITMAP_TYPE_BMP_RESOURCE;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight);
};
IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)
*/

class WXDLLEXPORT wxXPMFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxXPMFileHandler)
public:
  inline wxXPMFileHandler(void)
  {
  m_name = "XPM bitmap file";
  m_extension = "xpm";
  m_type = wxBITMAP_TYPE_XPM;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth = -1, int desiredHeight = -1);
  virtual bool SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};
IMPLEMENT_DYNAMIC_CLASS(wxXPMFileHandler, wxBitmapHandler)

bool wxXPMFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
#if USE_XPM_IN_MSW
    XImage *ximage;
    XpmAttributes xpmAttr;
    HDC     dc;

    M_BITMAPHANDLERDATA->m_ok = FALSE;
    dc = CreateCompatibleDC(NULL);
    if (dc)
    {
      xpmAttr.valuemask = XpmReturnPixels;
      int errorStatus = XpmReadFileToImage(&dc, WXSTRINGCAST name, &ximage, (XImage **) NULL, &xpmAttr);
      DeleteDC(dc);
      if (errorStatus == XpmSuccess)
      {
        M_BITMAPHANDLERDATA->m_hBitmap = (WXHBITMAP) ximage->bitmap;

        BITMAP  bm;
        GetObject((HBITMAP)M_BITMAPHANDLERDATA->m_hBitmap, sizeof(bm), (LPSTR) & bm);

        M_BITMAPHANDLERDATA->m_width = (bm.bmWidth);
        M_BITMAPHANDLERDATA->m_height = (bm.bmHeight);
        M_BITMAPHANDLERDATA->m_depth = (bm.bmPlanes * bm.bmBitsPixel);
        M_BITMAPHANDLERDATA->m_numColors = xpmAttr.npixels;
        XpmFreeAttributes(&xpmAttr);
        XImageFree(ximage);

        M_BITMAPHANDLERDATA->m_ok = TRUE;
        return TRUE;
      }
      else
      {
        M_BITMAPHANDLERDATA->m_ok = FALSE;
        return FALSE;
      }
    }
#endif

    return FALSE;
}

bool wxXPMFileHandler::SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
{
#if USE_XPM_IN_MSW
      HDC     dc = NULL;

      Visual *visual = NULL;
      XImage  ximage;

      dc = CreateCompatibleDC(NULL);
      if (dc)
      {
        if (SelectObject(dc, (HBITMAP) M_BITMAPHANDLERDATA->m_hBitmap))
        { /* for following SetPixel */
          /* fill the XImage struct 'by hand' */
    ximage.width = M_BITMAPHANDLERDATA->m_width; 
     ximage.height = M_BITMAPHANDLERDATA->m_height;
    ximage.depth = M_BITMAPHANDLERDATA->m_depth; 
     ximage.bitmap = (void *)M_BITMAPHANDLERDATA->m_hBitmap;
    int errorStatus = XpmWriteFileFromImage(&dc, WXSTRINGCAST name,
              &ximage, (XImage *) NULL, (XpmAttributes *) NULL);

          if (dc)
      DeleteDC(dc);

    if (errorStatus == XpmSuccess)
      return TRUE;    /* no error */
    else
      return FALSE;
        } else return FALSE;
      } else return FALSE;
#else
  return FALSE;
#endif
}


class WXDLLEXPORT wxXPMDataHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxXPMDataHandler)
public:
  inline wxXPMDataHandler(void)
  {
  m_name = "XPM bitmap data";
  m_extension = "xpm";
  m_type = wxBITMAP_TYPE_XPM_DATA;
  };

  virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
};
IMPLEMENT_DYNAMIC_CLASS(wxXPMDataHandler, wxBitmapHandler)

bool wxXPMDataHandler::Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth)
{
 		XImage *		ximage;
  	int     		ErrorStatus;
  	XpmAttributes 	xpmAttr;

    xpmAttr.valuemask = XpmReturnInfos; // get infos back
    ErrorStatus = XpmCreateImageFromData( GetMainDevice() , (char **)data,
         &ximage, (XImage **) NULL, &xpmAttr);

    if (ErrorStatus == XpmSuccess)
    {
			M_BITMAPHANDLERDATA->m_ok = FALSE;
			M_BITMAPHANDLERDATA->m_numColors = 0;
			M_BITMAPHANDLERDATA->m_hBitmap = ximage->gworldptr ;
	  
			M_BITMAPHANDLERDATA->m_width = ximage->width;
			M_BITMAPHANDLERDATA->m_height = ximage->height;
			M_BITMAPHANDLERDATA->m_depth = ximage->depth;
			M_BITMAPHANDLERDATA->m_numColors = xpmAttr.npixels;
    	XpmFreeAttributes(&xpmAttr);
	    M_BITMAPHANDLERDATA->m_ok = TRUE;
			ximage->gworldptr = NULL ;
			XImageFree(ximage); // releases the malloc, but does not detroy
		                  // the bitmap
			M_BITMAPHANDLERDATA->m_bitmapType = kMacBitmapTypeGrafWorld ;
		
			return TRUE;
    } 
    else
    {
      M_BITMAPHANDLERDATA->m_ok = FALSE;
      return FALSE;
    }
  return FALSE;
}

class WXDLLEXPORT wxBMPResourceHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxBMPResourceHandler)
public:
    inline wxBMPResourceHandler()
    {
        m_name = "Windows bitmap resource";
        m_extension = "";
        m_type = wxBITMAP_TYPE_BMP_RESOURCE;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight);
};

IMPLEMENT_DYNAMIC_CLASS(wxBMPResourceHandler, wxBitmapHandler)

bool wxBMPResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
    // TODO: load colourmap.
/*
    M_BITMAPHANDLERDATA->m_hBitmap = (WXHBITMAP) ::LoadBitmap(wxGetInstance(), name);
    if (M_BITMAPHANDLERDATA->m_hBitmap)
    {
      M_BITMAPHANDLERDATA->m_ok = TRUE;
      BITMAP bm;
      GetObject((HBITMAP) M_BITMAPHANDLERDATA->m_hBitmap, sizeof(BITMAP), (LPSTR) &bm);
      M_BITMAPHANDLERDATA->m_width = bm.bmWidth;
      M_BITMAPHANDLERDATA->m_height = bm.bmHeight;
      M_BITMAPHANDLERDATA->m_depth = bm.bmBitsPixel;
      return TRUE;
    }
*/
  // it's probably not found
  wxLogError("Can't load bitmap '%s' from resources! Check .rc file.", name.c_str());

  return FALSE;
}

class WXDLLEXPORT wxBMPFileHandler: public wxBitmapHandler
{
  DECLARE_DYNAMIC_CLASS(wxBMPFileHandler)
public:
  inline wxBMPFileHandler(void)
  {
  m_name = "Windows bitmap file";
  m_extension = "bmp";
  m_type = wxBITMAP_TYPE_BMP;
  };

  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
  virtual bool SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};

IMPLEMENT_DYNAMIC_CLASS(wxBMPFileHandler, wxBitmapHandler)

bool wxBMPFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
#if USE_IMAGE_LOADING_IN_MSW
    wxPalette *palette = NULL;
    bool success = FALSE;
/*
    if (type & wxBITMAP_DISCARD_COLOURMAP)
      success = wxLoadIntoBitmap(WXSTRINGCAST name, bitmap);
    else
*/
    success = (wxLoadIntoBitmap(WXSTRINGCAST name, bitmap, &palette) != 0);
    if (!success && palette)
    {
      delete palette;
      palette = NULL;
    }
    if (palette)
      M_BITMAPHANDLERDATA->m_bitmapPalette = *palette;
    return success;
#else
  return FALSE;
#endif
}

bool wxBMPFileHandler::SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *pal)
{
#if USE_IMAGE_LOADING_IN_MSW
    wxPalette *actualPalette = (wxPalette *)pal;
    if (!actualPalette && (!M_BITMAPHANDLERDATA->m_bitmapPalette.IsNull()))
      actualPalette = & (M_BITMAPHANDLERDATA->m_bitmapPalette);
    return (wxSaveBitmap(WXSTRINGCAST name, bitmap, actualPalette) != 0);
#else
  return FALSE;
#endif
}



void wxBitmap::CleanUpHandlers()
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        wxNode *next = node->Next();
        delete handler;
        delete node;
        node = next;
    }
}

void wxBitmap::InitStandardHandlers()
{
	AddHandler( new wxPICTResourceHandler ) ;
	AddHandler( new wxICONResourceHandler ) ;
	AddHandler(new wxXPMFileHandler);
  AddHandler(new wxXPMDataHandler);
	AddHandler(new wxBMPResourceHandler);
	AddHandler(new wxBMPFileHandler);
}
