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

#include "wx/wx.h"
#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/palette.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/image.h"

extern "C" 
{
#ifdef __UNIX__
    #include "xpm/xpm.h"
#else
	#include "xpm.h"
#endif
} ;

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)
#endif

#ifdef __UNIX__
    #include <ApplicationServices/ApplicationServices.h>
#else
    #include <PictUtils.h>
#endif

CTabHandle wxMacCreateColorTable( int numColors )
{
	CTabHandle newColors; /* Handle to the new color table */
	
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

GWorldPtr wxMacCreateGWorld( int width , int height , int depth )
{
	OSErr err = noErr ;
	GWorldPtr port ;
	Rect rect = { 0 , 0 , height , width } ;
	
	if ( depth < 0 )
	{
		depth = wxDisplayDepth() ;     
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

PicHandle wxMacCreatePict(GWorldPtr wp, GWorldPtr mask)
{
   CGrafPtr    origPort ;
   GDHandle    origDev ;

   PicHandle      pict;          // this is the Picture we give back

   RGBColor    gray = { 0xCCCC ,0xCCCC , 0xCCCC } ;
   RGBColor    white = { 0xffff ,0xffff , 0xffff } ;
   RGBColor    black = { 0x0000 ,0x0000 , 0x0000 } ;

   unsigned char *maskimage = NULL ;
   Rect portRect ;
   GetPortBounds( wp , &portRect ) ;
   int width = portRect.right - portRect.left ;
   int height = portRect.bottom - portRect.top ;

   LockPixels( GetGWorldPixMap( wp ) ) ;
   GetGWorld( &origPort , &origDev ) ;

   if ( mask )
   {
      maskimage = (unsigned char*) malloc( width * height ) ;
      SetGWorld( mask , NULL ) ;
      LockPixels( GetGWorldPixMap( mask ) ) ;
      for ( int y = 0 ; y < height ; y++ )
      {
         for( int x = 0 ; x < width ; x++ )
         {
            RGBColor col ;

            GetCPixel( x + portRect.left , y + portRect.top , &col ) ;
            maskimage[y*width + x] = ( col.red == 0 ) ; // for monochrome masks
         }
      }
      UnlockPixels( GetGWorldPixMap( mask ) ) ;
   }

   SetGWorld( wp , NULL ) ;

   pict = OpenPicture(&portRect);   // open a picture, this disables drawing
   if(!pict)
      return NULL;

   if ( maskimage )
   {
      RGBForeColor( &black ) ;
      RGBBackColor( &white ) ;
      PenMode(transparent);

      for ( int y = 0 ; y < height ; ++y )
      {
         for( int x = 0 ; x < width ; ++x )
         {
            if ( maskimage[y*width + x] )
            {
               RGBColor col ;

               GetCPixel( x + portRect.left , y + portRect.top , &col ) ;
               SetCPixel( x + portRect.left , y + portRect.top , &col ) ;
            }
            else {
                // With transparency set this sets a blank pixel not a white one
                SetCPixel( x + portRect.left , y + portRect.top , &white);
            }
         }
      }
      free( maskimage ) ;
      maskimage = NULL ;
   }
   else
   {
      RGBBackColor( &gray ) ;
      EraseRect(&portRect);
      RGBForeColor( &black ) ;
      RGBBackColor( &white ) ;

      CopyBits(GetPortBitMapForCopyBits(wp), /* src PixMap - we copy image over
                                              * itself - */
               GetPortBitMapForCopyBits(wp), //  dst PixMap - no drawing occurs
               &portRect,    // srcRect - it will be recorded and compressed -
               &portRect,    // dstRect - into the picture that is open -
               srcCopy,NULL); // copyMode and no clip region
   }
   ClosePicture();                  // We are done recording the picture
   UnlockPixels( GetGWorldPixMap( wp ) ) ;
   SetGWorld( origPort , origDev ) ;

   return pict;                  // return our groovy pict handle
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
	}
	
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
			LockPixels( GetGWorldPixMap( M_BITMAPDATA->m_hBitmap ) ) ;

#ifdef __UNIX__
           // bits is a word aligned array?? Don't think so
           // bits is a char array on MAC OS X however using the benefit of the
           // doubt I replaced references to 16 with sizeof(unsigned char)*8
           unsigned char* linestart = (unsigned char*) bits ;
           int linesize = ( the_width / (sizeof(unsigned char) * 8)) ;
           if ( the_width % (sizeof(unsigned char) * 8) ) {
               linesize += sizeof(unsigned char);
           }
#else
			// bits is a word aligned array
			
			unsigned char* linestart = (unsigned char*) bits ;
			int linesize = ( the_width / 16 ) * 2  ;
			if ( the_width % 16 )
			{
				linesize += 2 ;
			}
#endif
			
			RGBColor colors[2] = { 
				{ 0xFFFF , 0xFFFF , 0xFFFF } ,
				{ 0, 0 , 0 } 
				} ;
			
			for ( int y = 0 ; y < the_height ; ++y , linestart += linesize )
			{
				for ( int x = 0 ; x < the_width ; ++x )
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
		UnlockPixels( GetGWorldPixMap( M_BITMAPDATA->m_hBitmap ) ) ;
	
	   	SetGWorld( origPort , origDevice ) ;
	   }
	   else
	   {
         wxFAIL_MSG(wxT("multicolor BITMAPs not yet implemented"));
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

wxBitmap::wxBitmap(char **data)
{
    (void) Create((void *)data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect &rect) const
{
   wxCHECK_MSG( Ok() &&
                (rect.x >= 0) && (rect.y >= 0) &&
                (rect.x+rect.width <= GetWidth()) &&
                (rect.y+rect.height <= GetHeight()),
                wxNullBitmap, wxT("invalid bitmap or bitmap region") );

   
   wxBitmap ret( rect.width, rect.height, GetDepth() );
   wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

   WXHBITMAP origPort;
   GDHandle  origDevice;

   GetGWorld( &origPort, &origDevice );

   // Update the subbitmaps reference data
   wxBitmapRefData *ref = (wxBitmapRefData *)ret.GetRefData();

   ref->m_numColors     = M_BITMAPDATA->m_numColors;
   ref->m_bitmapPalette = M_BITMAPDATA->m_bitmapPalette;
   ref->m_bitmapType    = M_BITMAPDATA->m_bitmapType;

   // Copy sub region of this bitmap
   if(M_BITMAPDATA->m_bitmapType == kMacBitmapTypePict)
   {
       printf("GetSubBitmap:  Copy a region of a Pict structure - TODO\n");
   }
   else if(M_BITMAPDATA->m_bitmapType == kMacBitmapTypeGrafWorld)
   {
       // Copy mask
       if(GetMask())
       {
           WXHBITMAP submask, mask;
           RGBColor  color;

           mask = GetMask()->GetMaskBitmap();
           submask = wxMacCreateGWorld(rect.width, rect.height, 1);
           LockPixels(GetGWorldPixMap(mask));
           LockPixels(GetGWorldPixMap(submask));

           for(int yy = 0; yy < rect.height; yy++)
           {
               for(int xx = 0; xx < rect.width; xx++)
               {
                   SetGWorld(mask, NULL);
                   GetCPixel(rect.x + xx, rect.y + yy, &color);
                   SetGWorld(submask, NULL);
                   SetCPixel(xx,yy, &color);
               }
           }
           UnlockPixels(GetGWorldPixMap(mask));
           UnlockPixels(GetGWorldPixMap(submask));
           ref->m_bitmapMask = new wxMask;
           ref->m_bitmapMask->SetMaskBitmap(submask);
       }

       // Copy bitmap
       if(GetHBITMAP())
       {
           WXHBITMAP subbitmap, bitmap;
           RGBColor  color;

           bitmap = GetHBITMAP();
           subbitmap = wxMacCreateGWorld(rect.width, rect.height, GetDepth());
           LockPixels(GetGWorldPixMap(bitmap));
           LockPixels(GetGWorldPixMap(subbitmap));

           for(int yy = 0; yy < rect.height; yy++)
           {
               for(int xx = 0; xx < rect.width; xx++)
               {
                   SetGWorld(bitmap, NULL);
                   GetCPixel(rect.x + xx, rect.y + yy, &color);
                   SetGWorld(subbitmap, NULL);
                   SetCPixel(xx, yy, &color);
               }
           }
           UnlockPixels(GetGWorldPixMap(bitmap));
           UnlockPixels(GetGWorldPixMap(subbitmap));
           ret.SetHBITMAP(subbitmap);
       }
   }
   SetGWorld( origPort, origDevice );

   return ret;
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

int wxBitmap::GetBitmapType() const
{
   wxCHECK_MSG( Ok(), kMacBitmapTypeUnknownType, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_bitmapType;
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

wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") )
    wxCHECK_RET( depth == -1, wxT("invalid bitmap depth") )

    m_refData = new wxBitmapRefData();
      
    if (wxTheBitmapList) wxTheBitmapList->AddBitmap(this);

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int height = image.GetHeight();

    // Create picture

    Create( width , height , wxDisplayDepth() ) ;
    wxBitmap maskBitmap( width, height, 1);
    
    CGrafPtr origPort ;
    GDHandle origDevice ;

    LockPixels( GetGWorldPixMap(GetHBITMAP()) );
    LockPixels( GetGWorldPixMap(maskBitmap.GetHBITMAP()) );

    GetGWorld( &origPort , &origDevice ) ;
    SetGWorld( GetHBITMAP() , NULL ) ;
    
    // Render image
    wxColour rgb, maskcolor(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
    RGBColor color;
    RGBColor white = { 0xffff, 0xffff, 0xffff };
    RGBColor black = { 0     , 0     , 0      };

    register unsigned char* data = image.GetData();

    int index = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            rgb.Set(data[index++], data[index++], data[index++]);
            color = rgb.GetPixel();
            SetCPixel( x , y , &color ) ;
            if (image.HasMask())
            {
                SetGWorld(maskBitmap.GetHBITMAP(), NULL);
                if (rgb == maskcolor) {
                    SetCPixel(x,y, &white);
                }
                else {
                    SetCPixel(x,y, &black);
                }
                SetGWorld(GetHBITMAP(), NULL);
            }
        }
    }  // for height

    // Create mask
    if ( image.HasMask() ) {
        wxMask *mask = new wxMask( maskBitmap );
    }
    
    UnlockPixels( GetGWorldPixMap(GetHBITMAP()) );
    UnlockPixels( GetGWorldPixMap(maskBitmap.GetHBITMAP()) );
    SetGWorld( origPort, origDevice );
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;
    
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    // create an wxImage object
    int width = GetWidth();
    int height = GetHeight();
    image.Create( width, height );

    unsigned char *data = image.GetData();

    wxCHECK_MSG( data, wxNullImage, wxT("Could not allocate data for image") );

    WXHBITMAP origPort;
    GDHandle  origDevice;
    int      index;
    RGBColor color;
    // background color set to RGB(16,16,16) in consistent with wxGTK
    unsigned char mask_r=16, mask_g=16, mask_b=16;
    SInt16   r,g,b;
    wxMask  *mask = GetMask();

    GetGWorld( &origPort, &origDevice );
    LockPixels(GetGWorldPixMap(GetHBITMAP()));
    SetGWorld( GetHBITMAP(), NULL);

    // Copy data into image
    index = 0;
    for (int yy = 0; yy < height; yy++)
    {
        for (int xx = 0; xx < width; xx++)
        {
            GetCPixel(xx,yy, &color);
            r = ((color.red ) >> 8);
            g = ((color.green ) >> 8);
            b = ((color.blue ) >> 8);
            data[index    ] = r;
            data[index + 1] = g;
            data[index + 2] = b;
            if (mask)
            {
                if (mask->PointMasked(xx,yy))
                {
                    data[index    ] = mask_r;
                    data[index + 1] = mask_g;
                    data[index + 2] = mask_b;
                }
            }
            index += 3;
        }
    }
    if (mask)
    {
        image.SetMaskColour( mask_r, mask_g, mask_b );
        image.SetMask( true );
    }

    // Free resources
    UnlockPixels(GetGWorldPixMap(GetHBITMAP()));
    SetGWorld(origPort, origDevice);

    return image;
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

bool wxBitmap::Ok() const
{
   return (M_BITMAPDATA && M_BITMAPDATA->m_ok);
}

int wxBitmap::GetHeight() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_depth;
}

int wxBitmap::GetQuality() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_quality;
}

wxMask *wxBitmap::GetMask() const
{
   wxCHECK_MSG( Ok(), (wxMask *) NULL, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_bitmapMask;
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

wxPalette *wxBitmap::GetPalette() const
{
   wxCHECK_MSG( Ok(), NULL, wxT("Invalid bitmap  GetPalette()") );

   return &M_BITMAPDATA->m_bitmapPalette;
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

WXHBITMAP wxBitmap::GetHBITMAP() const
{
   wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_hBitmap;
}

PicHandle wxBitmap::GetPict() const
{
   wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );
   
   PicHandle picture;       // This is the returned picture

   // If bitmap already in Pict format return pointer
   if(M_BITMAPDATA->m_bitmapType == kMacBitmapTypePict) {
       return M_BITMAPDATA->m_hPict;
   }
   else if(M_BITMAPDATA->m_bitmapType != kMacBitmapTypeGrafWorld) {
       // Invalid bitmap
       return NULL;
   }

   RGBColor  gray = { 0xCCCC ,0xCCCC , 0xCCCC } ;
   RGBColor  white = { 0xffff ,0xffff , 0xffff } ;
   RGBColor  black = { 0x0000 ,0x0000 , 0x0000 } ;
   CGrafPtr  origPort;
   GDHandle  origDev ;
   wxMask   *mask;
   Rect      portRect ;

   GetPortBounds( GetHBITMAP() , &portRect ) ;
   int width = portRect.right - portRect.left ;
   int height = portRect.bottom - portRect.top ;

   LockPixels( GetGWorldPixMap( GetHBITMAP() ) ) ;
   GetGWorld( &origPort , &origDev ) ;

   mask = GetMask();

   SetGWorld( GetHBITMAP() , NULL ) ;

   picture = OpenPicture(&portRect);   // open a picture, this disables drawing
   if(!picture) {
       return NULL;
   }

   if( mask )
   {
#ifdef __UNIX__
       RGBColor trans = white;
#else
       RGBBackColor( &gray );
       EraseRect( &portRect );
       RGBColor trans = gray;
#endif
       RGBForeColor( &black ) ;
       RGBBackColor( &white ) ;
       PenMode(transparent);

       for ( int y = 0 ; y < height ; ++y )
       {
           for( int x = 0 ; x < width ; ++x )
           {
               if ( !mask->PointMasked(x,y) )
               {
                   RGBColor col ;

                   GetCPixel( x + portRect.left , y + portRect.top , &col ) ;
                   SetCPixel( x + portRect.left , y + portRect.top , &col ) ;
               }
               else {
                   // With transparency this sets a blank pixel
                   SetCPixel( x + portRect.left , y + portRect.top , &trans);
               }
           }
       }
   }
   else
   {
       RGBBackColor( &gray ) ;
       EraseRect(&portRect);
       RGBForeColor( &black ) ;
       RGBBackColor( &white ) ;

       CopyBits(GetPortBitMapForCopyBits(GetHBITMAP()), 
                // src PixMap - we copy image over itself -
                GetPortBitMapForCopyBits(GetHBITMAP()),
                //  dst PixMap - no drawing occurs
                &portRect,    // srcRect - it will be recorded and compressed -
                &portRect,    // dstRect - into the picture that is open -
                srcCopy,NULL); // copyMode and no clip region
   }
   ClosePicture();                  // We are done recording the picture
   UnlockPixels( GetGWorldPixMap( GetHBITMAP() ) ) ;
   SetGWorld( origPort , origDev ) ;

   return picture;                  // return our groovy pict handle
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
    m_maskBitmap = 0;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_maskBitmap = 0;
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_maskBitmap = 0;
    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    m_maskBitmap = 0;
    Create(bitmap);
}

wxMask::~wxMask()
{
	if ( m_maskBitmap )
	{
		wxMacDestroyGWorld( m_maskBitmap ) ;
		m_maskBitmap = NULL ;
	}
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
   if ( m_maskBitmap )
   {
       wxMacDestroyGWorld( m_maskBitmap ) ;
       m_maskBitmap = NULL ;
   }
   wxCHECK_MSG( bitmap.GetBitmapType() == kMacBitmapTypeGrafWorld, false,
                wxT("Cannot create mask from this bitmap type (TODO)"));
   // other types would require a temporary bitmap. not yet implemented

   wxCHECK_MSG( bitmap.Ok(), false, wxT("Invalid bitmap"));

   wxCHECK_MSG(bitmap.GetDepth() == 1, false,
               wxT("Cannot create mask from colour bitmap"));

   m_maskBitmap = wxMacCreateGWorld(bitmap.GetWidth(), bitmap.GetHeight(), 1);
   Rect rect = { 0,0, bitmap.GetHeight(), bitmap.GetWidth() };

   LockPixels( GetGWorldPixMap(m_maskBitmap) );
   LockPixels( GetGWorldPixMap(bitmap.GetHBITMAP()) );
   CopyBits(GetPortBitMapForCopyBits(bitmap.GetHBITMAP()),
            GetPortBitMapForCopyBits(m_maskBitmap),
            &rect, &rect, srcCopy, 0);
   UnlockPixels( GetGWorldPixMap(m_maskBitmap) );
   UnlockPixels( GetGWorldPixMap(bitmap.GetHBITMAP()) );

   return FALSE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
// TODO
    wxCHECK_MSG( 0, false, wxT("Not implemented"));
    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
	if ( m_maskBitmap )
	{
		wxMacDestroyGWorld( m_maskBitmap ) ;
		m_maskBitmap = NULL ;
	}
	wxCHECK_MSG( bitmap.GetBitmapType() == kMacBitmapTypeGrafWorld, false,
                 wxT("Cannot create mask from this bitmap type (TODO)"));
	// other types would require a temporary bitmap. not yet implemented 
	
    wxCHECK_MSG( bitmap.Ok(), false, wxT("Illigal bitmap"));

	m_maskBitmap = wxMacCreateGWorld( bitmap.GetWidth() , bitmap.GetHeight() , 1 );	
	LockPixels( GetGWorldPixMap( m_maskBitmap ) );
	LockPixels( GetGWorldPixMap( bitmap.GetHBITMAP() ) );
	RGBColor maskColor = colour.GetPixel();

    // this is not very efficient, but I can't think
    // of a better way of doing it
	CGrafPtr 	origPort ;
	GDHandle	origDevice ;
    RGBColor  col;
    RGBColor  colors[2] = {
        { 0xFFFF, 0xFFFF, 0xFFFF },
        { 0,      0,      0 }};
			
	GetGWorld( &origPort , &origDevice ) ;
	for (int w = 0; w < bitmap.GetWidth(); w++)
    {
        for (int h = 0; h < bitmap.GetHeight(); h++)
        {		
			SetGWorld( bitmap.GetHBITMAP(), NULL ) ;
			GetCPixel( w , h , &col ) ;
			SetGWorld( m_maskBitmap , NULL ) ;
            if (col.red == maskColor.red && col.green == maskColor.green && col.blue == maskColor.blue)
            {
				SetCPixel( w , h , &colors[0] ) ;
            }
            else
            {
				SetCPixel( w , h , &colors[1] ) ;
            }
        }
    }
	UnlockPixels( GetGWorldPixMap( (CGrafPtr) m_maskBitmap ) ) ;
	UnlockPixels( GetGWorldPixMap( bitmap.GetHBITMAP() ) ) ;
	SetGWorld( origPort , origDevice ) ;

    return TRUE;
}

bool wxMask::PointMasked(int x, int y)
{
   WXHBITMAP origPort;
   GDHandle  origDevice;
   RGBColor  color;
   bool masked = true;

   GetGWorld( &origPort, &origDevice);

   //Set port to mask and see if it masked (1) or not ( 0 )
   SetGWorld(m_maskBitmap, NULL);
   LockPixels(GetGWorldPixMap(m_maskBitmap));
   GetCPixel(x,y, &color);
   masked = !(color.red == 0 && color.green == 0 && color.blue == 0);
   UnlockPixels(GetGWorldPixMap(m_maskBitmap));

   SetGWorld( origPort, origDevice);

   return masked;
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
	
#if TARGET_CARBON
	c2pstrcpy( (StringPtr) theName , name ) ;
#else
	strcpy( (char *) theName , name ) ;
	c2pstr( (char *)theName ) ;
#endif
	
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
        { 
          
    ximage.width = M_BITMAPHANDLERDATA->m_width; 
     ximage.height = M_BITMAPHANDLERDATA->m_height;
    ximage.depth = M_BITMAPHANDLERDATA->m_depth; 
     ximage.bitmap = (void *)M_BITMAPHANDLERDATA->m_hBitmap;
    int errorStatus = XpmWriteFileFromImage(&dc, WXSTRINGCAST name,
              &ximage, (XImage *) NULL, (XpmAttributes *) NULL);

          if (dc)
      DeleteDC(dc);

    if (errorStatus == XpmSuccess)
      return TRUE;    
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
 	XImage *		ximage = NULL ;
 	XImage *		xshapeimage = NULL ;
  	int     		ErrorStatus;
  	XpmAttributes 	xpmAttr;

    xpmAttr.valuemask = XpmReturnInfos; // get infos back
    ErrorStatus = XpmCreateImageFromData( GetMainDevice() , (char **)data,
         &ximage, &xshapeimage, &xpmAttr);

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
			if ( xshapeimage != NULL )
			{
				wxMask* m = new wxMask() ;
				m->SetMaskBitmap( xshapeimage->gworldptr ) ;
				M_BITMAPHANDLERDATA->m_bitmapMask = m ;
			}
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
