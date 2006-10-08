/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/bitmap.cpp
// Purpose:     wxBitmap
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif

#include "wx/xpmdecod.h"

#include "wx/rawbmp.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject )

#ifdef __DARWIN__
    #include <ApplicationServices/ApplicationServices.h>
#else
    #include <PictUtils.h>
#endif

#include "wx/mac/uma.h"

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
    (**newColors).ctTable[index].rgb.red = red ; // someRedValue;
    (**newColors).ctTable[index].rgb.green = green ; // someGreenValue;
    (**newColors).ctTable[index].rgb.blue = blue ; // someBlueValue;
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

#define kDefaultRes 0x00480000 /* Default resolution is 72 DPI; Fixed type */

OSErr SetupCIconHandlePixMap( CIconHandle icon , short depth , Rect  *bounds , CTabHandle colors )
{
    CTabHandle newColors;       /* Color table used for the off-screen PixMap */
    Ptr        offBaseAddr;     /* Pointer to the off-screen pixel image */
    OSErr      error;           /* Returns error code */
    short      bytesPerRow;     /* Number of bytes per row in the PixMap */


    error = noErr;
    newColors = nil;
    offBaseAddr = nil;

    bytesPerRow = ((depth * (bounds->right - bounds->left) + 31) / 32) * 4;

   /* Clone the clut if indexed color; allocate a dummy clut if direct color*/
    if (depth <= 8)
        {
        newColors = colors;
        error = HandToHand((Handle *) &newColors);
        }
    else
        {
        newColors = (CTabHandle) NewHandle(sizeof(ColorTable) -
                sizeof(CSpecArray));
        error = MemError();
        }
    if (error == noErr)
        {
        /* Allocate pixel image; long integer multiplication avoids overflow */
        (**icon).iconData = NewHandle((unsigned long) bytesPerRow * (bounds->bottom -
                bounds->top));
        if ((**icon).iconData != nil)
            {
            /* Initialize fields common to indexed and direct PixMaps */
            (**icon).iconPMap.baseAddr = 0;  /* Point to image */
            (**icon).iconPMap.rowBytes = bytesPerRow | /* MSB set for PixMap */
                    0x8000;
            (**icon).iconPMap.bounds = *bounds;        /* Use given bounds */
            (**icon).iconPMap.pmVersion = 0;           /* No special stuff */
            (**icon).iconPMap.packType = 0;            /* Default PICT pack */
            (**icon).iconPMap.packSize = 0;            /* Always zero in mem */
            (**icon).iconPMap.hRes = kDefaultRes;      /* 72 DPI default res */
            (**icon).iconPMap.vRes = kDefaultRes;      /* 72 DPI default res */
            (**icon).iconPMap.pixelSize = depth;       /* Set # bits/pixel */

            /* Initialize fields specific to indexed and direct PixMaps */
            if (depth <= 8)
                {
                /* PixMap is indexed */
                (**icon).iconPMap.pixelType = 0;       /* Indicates indexed */
                (**icon).iconPMap.cmpCount = 1;        /* Have 1 component */
                (**icon).iconPMap.cmpSize = depth;     /* Component size=depth */
                (**icon).iconPMap.pmTable = newColors; /* Handle to CLUT */
                }
            else
                {
                /* PixMap is direct */
                (**icon).iconPMap.pixelType = RGBDirect; /* Indicates direct */
                (**icon).iconPMap.cmpCount = 3;          /* Have 3 components */
                if (depth == 16)
                    (**icon).iconPMap.cmpSize = 5;       /* 5 bits/component */
                else
                    (**icon).iconPMap.cmpSize = 8;       /* 8 bits/component */
                (**newColors).ctSeed = 3 * (**icon).iconPMap.cmpSize;
                (**newColors).ctFlags = 0;
                (**newColors).ctSize = 0;
                (**icon).iconPMap.pmTable = newColors;
                }
            }
        else
            error = MemError();
        }
    else
        newColors = nil;

    /* If no errors occurred, return a handle to the new off-screen PixMap */
    if (error != noErr)
        {
        if (newColors != nil)
            DisposeCTable(newColors);
        }

    /* Return the error code */
    return error;
}

CIconHandle wxMacCreateCIcon(GWorldPtr image , GWorldPtr mask , short dstDepth , short iconSize  )
{
    GWorldPtr       saveWorld;
    GDHandle        saveHandle;

    GetGWorld(&saveWorld,&saveHandle);      // save Graphics env state
    SetGWorld(image,nil);

    Rect frame = { 0 , 0 , iconSize , iconSize } ;
    Rect imageBounds = frame ;
    GetPortBounds( image , &imageBounds ) ;

    int bwSize = iconSize / 8 * iconSize ;
    CIconHandle icon = (CIconHandle) NewHandleClear( sizeof ( CIcon ) + 2 * bwSize) ;
    HLock((Handle)icon) ;
    SetupCIconHandlePixMap( icon , dstDepth , &frame,GetCTable(dstDepth)) ;
    HLock( (**icon).iconData ) ;
    (**icon).iconPMap.baseAddr = *(**icon).iconData ;

    LockPixels(GetGWorldPixMap(image));

    CopyBits(GetPortBitMapForCopyBits(image),
                (BitMapPtr)&((**icon).iconPMap),
                &imageBounds,
                &imageBounds,
                srcCopy | ditherCopy, nil);


    UnlockPixels(GetGWorldPixMap(image));
    HUnlock( (**icon).iconData ) ;

    (**icon).iconMask.rowBytes = iconSize / 8 ;
    (**icon).iconMask.bounds = frame ;

    (**icon).iconBMap.rowBytes = iconSize / 8 ;
    (**icon).iconBMap.bounds = frame ;
    (**icon).iconMask.baseAddr = (char*) &(**icon).iconMaskData ;
    (**icon).iconBMap.baseAddr = (char*) &(**icon).iconMaskData + bwSize ;

    if ( mask )
    {
        Rect r ;
        GetPortBounds( image , &r ) ;
      LockPixels(GetGWorldPixMap(mask) ) ;
      CopyBits(GetPortBitMapForCopyBits(mask) ,
          &(**icon).iconBMap , &r , &r, srcCopy , nil ) ;
      CopyBits(GetPortBitMapForCopyBits(mask) ,
          &(**icon).iconMask , &r , &r, srcCopy , nil ) ;
      UnlockPixels(GetGWorldPixMap( mask ) ) ;
    }
    else
    {
        Rect r ;
        GetPortBounds( image , &r ) ;
        LockPixels(GetGWorldPixMap(image));
      CopyBits(GetPortBitMapForCopyBits(image) ,
          &(**icon).iconBMap , &r , &r, srcCopy , nil ) ;
      CopyBits(GetPortBitMapForCopyBits(image) ,
          &(**icon).iconMask , &r , &r, srcCopy , nil ) ;
        UnlockPixels(GetGWorldPixMap(image));
    }

    (**icon).iconMask.baseAddr = NULL ;
    (**icon).iconBMap.baseAddr = NULL ;
    (**icon).iconPMap.baseAddr = NULL ;
    HUnlock((Handle)icon) ;
    SetGWorld(saveWorld,saveHandle);

    return icon;
}

PicHandle wxMacCreatePict(GWorldPtr wp, GWorldPtr mask)
{
  CGrafPtr       origPort ;
  GDHandle       origDev ;

  PicHandle      pict;

  RGBColor       white = { 0xffff ,0xffff , 0xffff } ;
  RGBColor       black = { 0x0000 ,0x0000 , 0x0000 } ;

  GetGWorld( &origPort , &origDev ) ;

  RgnHandle clipRgn = NULL ;

  if ( mask )
  {
    clipRgn = NewRgn() ;
    LockPixels( GetGWorldPixMap( mask ) ) ;
    BitMapToRegion( clipRgn , (BitMap*) *GetGWorldPixMap( mask ) ) ;
    UnlockPixels( GetGWorldPixMap( mask ) ) ;
  }

  SetGWorld( wp , NULL ) ;
  Rect portRect ;
  if ( clipRgn )
    GetRegionBounds( clipRgn , &portRect ) ;
  else
      GetPortBounds( wp , &portRect ) ;
  pict = OpenPicture(&portRect);
  if(pict)
  {
    RGBForeColor( &black ) ;
    RGBBackColor( &white ) ;

    if ( clipRgn )
        SetClip( clipRgn ) ;

    LockPixels( GetGWorldPixMap( wp ) ) ;
    CopyBits(GetPortBitMapForCopyBits(wp),
            GetPortBitMapForCopyBits(wp),
            &portRect,
            &portRect,
            srcCopy,clipRgn);
    UnlockPixels( GetGWorldPixMap( wp ) ) ;
    ClosePicture();
  }
  SetGWorld( origPort , origDev ) ;
  if ( clipRgn )
      DisposeRgn( clipRgn ) ;
  return pict;
}

void wxMacCreateBitmapButton( ControlButtonContentInfo*info , const wxBitmap& bitmap , int forceType )
{
    memset( info , 0 , sizeof(ControlButtonContentInfo) ) ;
    if ( bitmap.Ok() )
    {
        wxBitmapRefData * bmap = (wxBitmapRefData*) ( bitmap.GetRefData()) ;
        if ( bmap == NULL )
            return ;

        if ( bmap->m_bitmapType == kMacBitmapTypePict )
        {
            info->contentType = kControlContentPictHandle ;
            info->u.picture = MAC_WXHMETAFILE(bmap->m_hPict) ;
        }
        else if ( bmap->m_bitmapType == kMacBitmapTypeGrafWorld )
        {
            if ( (forceType == kControlContentCIconHandle || ( bmap->m_width == bmap->m_height && forceType != kControlContentPictHandle ) ) && ((bmap->m_width & 0x3) == 0) )
            {
                info->contentType = kControlContentCIconHandle ;
                if ( bitmap.GetMask() )
                {
                    info->u.cIconHandle = wxMacCreateCIcon( MAC_WXHBITMAP(bmap->m_hBitmap) , MAC_WXHBITMAP(bitmap.GetMask()->GetMaskBitmap()) ,
                                                           8 , bmap->m_width ) ;
                }
                else
                {
                    info->u.cIconHandle = wxMacCreateCIcon( MAC_WXHBITMAP(bmap->m_hBitmap) , NULL ,
                                                           8 , bmap->m_width ) ;
                }
            }
            else
            {
                info->contentType = kControlContentPictHandle ;
                if ( bitmap.GetMask() )
                {
                    info->u.picture = wxMacCreatePict( MAC_WXHBITMAP(bmap->m_hBitmap) , MAC_WXHBITMAP(bitmap.GetMask()->GetMaskBitmap() ) ) ;
               }
                else
                {
                    info->u.picture = wxMacCreatePict( MAC_WXHBITMAP(bmap->m_hBitmap) , NULL ) ;
                }
            }
        }
        else if ( bmap->m_bitmapType == kMacBitmapTypeIcon )
        {
            info->contentType = kControlContentCIconHandle ;
            info->u.cIconHandle = MAC_WXHICON(bmap->m_hIcon) ;
        }
    }
}

wxBitmapRefData::wxBitmapRefData()
    : m_width(0)
    , m_height(0)
    , m_depth(0)
    , m_ok(false)
    , m_numColors(0)
    , m_quality(0)
{
    m_bitmapMask = NULL;
    m_hBitmap = NULL ;
    m_hPict = NULL ;
    m_hIcon = NULL ;
    m_bitmapType = kMacBitmapTypeUnknownType ;
    m_hasAlpha = false;
}

// TODO move this to a public function of Bitmap Ref
static void DisposeBitmapRefData(wxBitmapRefData *data)
{
    if ( !data )
        return ;

    switch (data->m_bitmapType)
    {
        case kMacBitmapTypePict :
            {
                if ( data->m_hPict )
                {
                    KillPicture( MAC_WXHMETAFILE( data->m_hPict ) ) ;
                    data->m_hPict = NULL ;
                }
            }
            break ;
        case kMacBitmapTypeGrafWorld :
            {
                if ( data->m_hBitmap )
                {
                    wxMacDestroyGWorld( MAC_WXHBITMAP(data->m_hBitmap) ) ;
                    data->m_hBitmap = NULL ;
                }
            }
            break ;
        case kMacBitmapTypeIcon :
            if ( data->m_hIcon )
            {
                DisposeCIcon( MAC_WXHICON(data->m_hIcon) ) ;
                data->m_hIcon = NULL ;
            }

        default :
            // unkown type ?
            break ;
    }

    if (data->m_bitmapMask)
    {
        delete data->m_bitmapMask;
        data->m_bitmapMask = NULL;
    }
}

wxBitmapRefData::~wxBitmapRefData()
{
  DisposeBitmapRefData( this ) ;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    Ref(icon) ;
    return true;
}

wxBitmap::wxBitmap()
{
    m_refData = NULL;
}

wxBitmap::~wxBitmap()
{
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
        M_BITMAPDATA->m_ok = (MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap) != NULL ) ;

        CGrafPtr    origPort ;
        GDHandle    origDevice ;

        GetGWorld( &origPort , &origDevice ) ;
        SetGWorld( MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap) , NULL ) ;
        LockPixels( GetGWorldPixMap( MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap) ) ) ;

        // bits is a char array

        unsigned char* linestart = (unsigned char*) bits ;
        int linesize = ( the_width / (sizeof(unsigned char) * 8)) ;
        if ( the_width % (sizeof(unsigned char) * 8) ) {
            linesize += sizeof(unsigned char);
        }

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
        UnlockPixels( GetGWorldPixMap( MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap) ) ) ;

        SetGWorld( origPort , origDevice ) ;
    }
    else
    {
        wxFAIL_MSG(wxT("multicolor BITMAPs not yet implemented"));
    }
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);
}

wxBitmap::wxBitmap(void *data, wxBitmapType type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxString& filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

bool wxBitmap::CreateFromXpm(const char **bits)
{
    wxCHECK_MSG( bits != NULL, false, wxT("invalid bitmap data") );
    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(bits);
    wxCHECK_MSG( img.Ok(), false, wxT("invalid bitmap data") );
    *this = wxBitmap(img);
    return true;
}

wxBitmap::wxBitmap(const char **bits)
{
    (void) CreateFromXpm(bits);
}

wxBitmap::wxBitmap(char **bits)
{
    (void) CreateFromXpm((const char **)bits);
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

   GWorldPtr origPort;
   GDHandle  origDevice;

   GetGWorld( &origPort, &origDevice );

   // Update the subbitmaps reference data
   wxBitmapRefData *ref = (wxBitmapRefData *)ret.GetRefData();

   ref->m_numColors     = M_BITMAPDATA->m_numColors;
#if wxUSE_PALETTE
    ref->m_bitmapPalette = M_BITMAPDATA->m_bitmapPalette;
#endif // wxUSE_PALETTE
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
           GWorldPtr submask, mask;
           RGBColor  color;

           mask = (GWorldPtr) GetMask()->GetMaskBitmap();
           submask = wxMacCreateGWorld(rect.width, rect.height, GetMask()->GetDepth() );
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
           GWorldPtr subbitmap, bitmap;
           RGBColor  color;

           bitmap = (GWorldPtr) GetHBITMAP();
           subbitmap = (GWorldPtr) ref->m_hBitmap ;
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
    M_BITMAPDATA->m_ok = ( M_BITMAPDATA->m_hBitmap != NULL ) ;
    return M_BITMAPDATA->m_ok;
}

int wxBitmap::GetBitmapType() const
{
   wxCHECK_MSG( Ok(), kMacBitmapTypeUnknownType, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_bitmapType;
}

void wxBitmap::SetHBITMAP(WXHBITMAP bmp)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;
    else
        DisposeBitmapRefData( M_BITMAPDATA ) ;

    M_BITMAPDATA->m_bitmapType = kMacBitmapTypeGrafWorld ;
    M_BITMAPDATA->m_hBitmap = bmp ;
    M_BITMAPDATA->m_ok = ( M_BITMAPDATA->m_hBitmap != NULL ) ;
}

void wxBitmap::SetHICON(WXHICON ico)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;
    else
        DisposeBitmapRefData( M_BITMAPDATA ) ;

    M_BITMAPDATA->m_bitmapType = kMacBitmapTypeIcon ;
    M_BITMAPDATA->m_hIcon = ico ;
    M_BITMAPDATA->m_ok = ( M_BITMAPDATA->m_hIcon != NULL ) ;
}

void wxBitmap::SetPict(WXHMETAFILE pict)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;
    else
        DisposeBitmapRefData( M_BITMAPDATA ) ;

    M_BITMAPDATA->m_bitmapType = kMacBitmapTypePict ;
    M_BITMAPDATA->m_hPict = pict ;
    M_BITMAPDATA->m_ok = ( M_BITMAPDATA->m_hPict != NULL ) ;
}

bool wxBitmap::LoadFile(const wxString& filename, wxBitmapType type)
{
    UnRef();

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler )
    {
        m_refData = new wxBitmapRefData;

        return handler->LoadFile(this, filename, type, -1, -1);
    }
    else
    {
        wxImage loadimage(filename, type);
        if (loadimage.Ok()) {
            *this = loadimage;
            return true;
        }
    }
    wxLogWarning(wxT("no bitmap handler for type %d defined."), type);
    return false;
}

bool wxBitmap::Create(void *data, wxBitmapType type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning(wxT("no bitmap handler for type %d defined."), type);

        return false;
    }

    return handler->Create(this, data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") );
    wxCHECK_RET( depth == -1, wxT("invalid bitmap depth") );

    m_refData = new wxBitmapRefData();

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int height = image.GetHeight();

    // Create picture

    Create( width , height , 32 ) ;

    CGrafPtr origPort ;
    GDHandle origDevice ;

    PixMapHandle pixMap = GetGWorldPixMap((GWorldPtr)GetHBITMAP()) ;
    LockPixels( pixMap );

    GetGWorld( &origPort , &origDevice ) ;
    SetGWorld( (GWorldPtr) GetHBITMAP() , NULL ) ;

    // Render image
    register unsigned char* data = image.GetData();
    char* destinationBase = GetPixBaseAddr( pixMap );
    register unsigned char* destination = (unsigned char*) destinationBase ;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            *destination++ = 0 ;
            *destination++ = *data++ ;
            *destination++ = *data++ ;
            *destination++ = *data++ ;
        }
        destinationBase += ((**pixMap).rowBytes & 0x7fff);
        destination = (unsigned char*) destinationBase ;
    }
    if ( image.HasAlpha() )
    {
      unsigned char *alpha = image.GetAlpha();

      wxColour maskcolor(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
      RGBColor color ;
      wxBitmap maskBitmap ;

      maskBitmap.Create( width, height, 24);
      LockPixels( GetGWorldPixMap( (GWorldPtr) maskBitmap.GetHBITMAP()) );
      SetGWorld( (GWorldPtr) maskBitmap.GetHBITMAP(), NULL);

      for (int y = 0; y < height; y++)
      {
          for (int x = 0; x < width; x++)
          {
              memset( &color , 255 - *alpha , sizeof( color ) );
              SetCPixel(x,y, &color);

              alpha += 1 ;
          }
      }  // for height
      SetGWorld( (GWorldPtr) GetHBITMAP(), NULL);
      SetMask(new wxMask( maskBitmap ));
      UnlockPixels( GetGWorldPixMap( (GWorldPtr) maskBitmap.GetHBITMAP()) );
    }
    else if ( image.HasMask() )
    {
      data = image.GetData();

      wxColour maskcolor(image.GetMaskRed(), image.GetMaskGreen(), image.GetMaskBlue());
      RGBColor white = { 0xffff, 0xffff, 0xffff };
      RGBColor black = { 0     , 0     , 0      };
      wxBitmap maskBitmap ;

      maskBitmap.Create( width, height, 1);
      LockPixels( GetGWorldPixMap( (GWorldPtr) maskBitmap.GetHBITMAP()) );
      SetGWorld( (GWorldPtr) maskBitmap.GetHBITMAP(), NULL);

      for (int y = 0; y < height; y++)
      {
          for (int x = 0; x < width; x++)
          {
              if ( data[0] == image.GetMaskRed() && data[1] == image.GetMaskGreen() && data[2] == image.GetMaskBlue() )
              {
                SetCPixel(x,y, &white);
              }
              else {
                SetCPixel(x,y, &black);
              }
              data += 3 ;
          }
      }  // for height
      SetGWorld( (GWorldPtr) GetHBITMAP(), NULL);
      SetMask(new wxMask( maskBitmap ));
      UnlockPixels( GetGWorldPixMap( (GWorldPtr) maskBitmap.GetHBITMAP()) );
    }

    UnlockPixels( GetGWorldPixMap( (GWorldPtr) GetHBITMAP()) );
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

    GWorldPtr origPort;
    GDHandle  origDevice;
    RgnHandle maskRgn = NULL ;
    GWorldPtr tempPort = NULL ;
    int      index;
    RGBColor color;
    // background color set to RGB(16,16,16) in consistent with wxGTK
    unsigned char mask_r=16, mask_g=16, mask_b=16;
    SInt16   r,g,b;
    wxMask  *mask = GetMask();

    GetGWorld( &origPort, &origDevice );
    if ( GetBitmapType() != kMacBitmapTypeGrafWorld )
    {
        tempPort = wxMacCreateGWorld( width , height , -1) ;
    }
    else
    {
        tempPort =  (GWorldPtr) GetHBITMAP() ;
    }
    LockPixels(GetGWorldPixMap(tempPort));
    SetGWorld( tempPort, NULL);
    if ( GetBitmapType() == kMacBitmapTypePict || GetBitmapType() == kMacBitmapTypeIcon )
    {
        Rect bitmaprect = { 0 , 0 , height, width };
        if ( GetBitmapType() == kMacBitmapTypeIcon )
        {
            ::PlotCIconHandle( &bitmaprect , atNone , ttNone , MAC_WXHICON(GetHICON()) ) ;
            maskRgn = NewRgn() ;
            BitMapToRegion( maskRgn , &(**(MAC_WXHICON(GetHICON()))).iconMask ) ;
        }
         else
             ::DrawPicture( (PicHandle) GetPict(), &bitmaprect ) ;
    }
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
            if ( maskRgn )
            {
                Point pt ;
                pt.h = xx ;
                pt.v = yy ;
                if ( !PtInRgn( pt , maskRgn ) )
                {
                    data[index    ] = mask_r;
                    data[index + 1] = mask_g;
                    data[index + 2] = mask_b;
                }
            }
            else
            {
                if (mask)
                {
                    if (mask->PointMasked(xx,yy))
                    {
                        data[index    ] = mask_r;
                        data[index + 1] = mask_g;
                        data[index + 2] = mask_b;
                    }
                }
            }
            index += 3;
        }
    }
    if (mask || maskRgn )
    {
        image.SetMaskColour( mask_r, mask_g, mask_b );
        image.SetMask( true );
    }

    // Free resources
    UnlockPixels(GetGWorldPixMap( tempPort ));
    SetGWorld(origPort, origDevice);
    if ( GetBitmapType() != kMacBitmapTypeGrafWorld )
    {
        wxMacDestroyGWorld( tempPort ) ;
    }
    if ( maskRgn )
    {
        DisposeRgn( maskRgn ) ;
    }

    return image;
}


bool wxBitmap::SaveFile(const wxString& filename, wxBitmapType type,
                        const wxPalette *palette) const
{
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler )
    {
        return handler->SaveFile(this, filename, type, palette);
    }
    else
    {
        wxImage image = ConvertToImage();

        return image.SaveFile(filename, type);
    }

    wxLogWarning(wxT("no bitmap handler for type %d defined."), type);
    return false;
}

bool wxBitmap::IsOk() const
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

#if wxUSE_PALETTE
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
#endif // wxUSE_PALETTE

void wxBitmap::SetMask(wxMask *mask)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    // Remove existing mask if there is one.
    delete M_BITMAPDATA->m_bitmapMask;

    M_BITMAPDATA->m_bitmapMask = mask ;
}

WXHBITMAP wxBitmap::GetHBITMAP() const
{
   wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

   return MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap);
}

WXHMETAFILE wxBitmap::GetPict( bool *created ) const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    PicHandle picture = NULL ;       // This is the returned picture
    if ( created )
        (*created) = false ;
    // If bitmap already in Pict format return pointer
    if(M_BITMAPDATA->m_bitmapType == kMacBitmapTypePict) {
       return M_BITMAPDATA->m_hPict;
    }
    else if(M_BITMAPDATA->m_bitmapType != kMacBitmapTypeGrafWorld) {
       // Invalid bitmap
       return NULL;
    }
    else
    {
        if ( GetMask() )
        {
            picture = wxMacCreatePict( MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap) , MAC_WXHBITMAP(GetMask()->GetMaskBitmap() ) ) ;
        }
        else
        {
            picture = wxMacCreatePict( MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap) , NULL ) ;
        }
        if ( created && picture )
            (*created) = true ;
    }
     return picture ;
}

/*
 * wxMask
 */

wxMask::wxMask()
    : m_maskBitmap(NULL)
{
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
    : m_maskBitmap(NULL)
{
    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
    : m_maskBitmap(NULL)
{
    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
    : m_maskBitmap(NULL)
{
    Create(bitmap);
}

wxMask::~wxMask()
{
    if ( m_maskBitmap )
    {
        wxMacDestroyGWorld(  (GWorldPtr) m_maskBitmap ) ;
        m_maskBitmap = NULL ;
    }
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
   if ( m_maskBitmap )
   {
       wxMacDestroyGWorld(  (GWorldPtr) m_maskBitmap ) ;
       m_maskBitmap = NULL ;
   }
   wxCHECK_MSG( bitmap.GetBitmapType() == kMacBitmapTypeGrafWorld, false,
                wxT("Cannot create mask from this bitmap type (TODO)"));
   // other types would require a temporary bitmap. not yet implemented

   wxCHECK_MSG( bitmap.Ok(), false, wxT("Invalid bitmap"));

   m_depth = bitmap.GetDepth() ;
   m_maskBitmap = wxMacCreateGWorld(bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetDepth() );
   Rect rect = { 0,0, bitmap.GetHeight(), bitmap.GetWidth() };

   LockPixels( GetGWorldPixMap( (GWorldPtr) m_maskBitmap) );
   LockPixels( GetGWorldPixMap( (GWorldPtr) bitmap.GetHBITMAP()) );
   CopyBits(GetPortBitMapForCopyBits( (GWorldPtr) bitmap.GetHBITMAP()),
            GetPortBitMapForCopyBits( (GWorldPtr) m_maskBitmap),
            &rect, &rect, srcCopy, 0);
   UnlockPixels( GetGWorldPixMap( (GWorldPtr) m_maskBitmap) );
   UnlockPixels( GetGWorldPixMap( (GWorldPtr) bitmap.GetHBITMAP()) );

   return false;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
    // TODO
    wxCHECK_MSG( 0, false, wxT("wxMask::Create not yet implemented"));
    return false;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    if ( m_maskBitmap )
    {
        wxMacDestroyGWorld(  (GWorldPtr) m_maskBitmap ) ;
        m_maskBitmap = NULL ;
    }
    wxCHECK_MSG( bitmap.GetBitmapType() == kMacBitmapTypeGrafWorld, false,
                 wxT("Cannot create mask from this bitmap type (TODO)"));
    // other types would require a temporary bitmap. not yet implemented

    wxCHECK_MSG( bitmap.Ok(), false, wxT("Illigal bitmap"));

    m_maskBitmap = wxMacCreateGWorld( bitmap.GetWidth() , bitmap.GetHeight() , 1 );
    m_depth = 1 ;
    LockPixels( GetGWorldPixMap(  (GWorldPtr) m_maskBitmap ) );
    LockPixels( GetGWorldPixMap(  (GWorldPtr) bitmap.GetHBITMAP() ) );
    RGBColor maskColor = MAC_WXCOLORREF(colour.GetPixel());

    // this is not very efficient, but I can't think
    // of a better way of doing it
    CGrafPtr    origPort ;
    GDHandle    origDevice ;
    RGBColor  col;
    RGBColor  colors[2] = {
        { 0xFFFF, 0xFFFF, 0xFFFF },
        { 0,      0,      0 }};

    GetGWorld( &origPort , &origDevice ) ;
    for (int w = 0; w < bitmap.GetWidth(); w++)
    {
        for (int h = 0; h < bitmap.GetHeight(); h++)
        {
            SetGWorld(  (GWorldPtr) bitmap.GetHBITMAP(), NULL ) ;
            GetCPixel( w , h , &col ) ;
            SetGWorld(  (GWorldPtr) m_maskBitmap , NULL ) ;
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
    UnlockPixels( GetGWorldPixMap(  (GWorldPtr) bitmap.GetHBITMAP() ) ) ;
    SetGWorld( origPort , origDevice ) ;

    return true;
}

bool wxMask::PointMasked(int x, int y)
{
   GWorldPtr origPort;
   GDHandle  origDevice;
   RGBColor  color;
   bool masked = true;

   GetGWorld( &origPort, &origDevice);

   //Set port to mask and see if it masked (1) or not ( 0 )
   SetGWorld( (GWorldPtr) m_maskBitmap, NULL);
   LockPixels(GetGWorldPixMap( (GWorldPtr) m_maskBitmap));
   GetCPixel(x,y, &color);
   masked = !(color.red == 0 && color.green == 0 && color.blue == 0);
   UnlockPixels(GetGWorldPixMap( (GWorldPtr) m_maskBitmap));

   SetGWorld( origPort, origDevice);

   return masked;
}

/*
 * wxBitmapHandler
 */

wxBitmapHandler::~wxBitmapHandler()
{
}

bool wxBitmapHandler::Create(wxBitmap *bitmap, void *data, long type, int width, int height, int depth)
{
    return false;
}

bool wxBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
        int desiredWidth, int desiredHeight)
{
    return false;
}

bool wxBitmapHandler::SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
{
    return false;
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
        m_name = wxT("Macintosh Pict resource");
        m_extension = wxEmptyString;
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
    wxMacStringToPascal( name , theName ) ;

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
//      M_BITMAPHANDLERDATA->m_bitmapPalette;
//      M_BITMAPHANDLERDATA->m_quality;
        return true ;
    }
    return false ;
}

void wxBitmap::InitStandardHandlers()
{
    AddHandler(new wxPICTResourceHandler) ;
    AddHandler(new wxICONResourceHandler) ;
}

// ----------------------------------------------------------------------------
// raw bitmap access support
// ----------------------------------------------------------------------------

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if ( !Ok() )
    {
        // no bitmap, no data (raw or otherwise)
        return NULL;
    }

   if ( M_BITMAPDATA->m_bitmapType != kMacBitmapTypeGrafWorld )
   {
       wxFAIL_MSG( _T("GetRawData() only supported for GWorlds") );

       return NULL;
   }

   GWorldPtr gworld = MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap);
   PixMapHandle hPixMap = GetGWorldPixMap(gworld);
   wxCHECK_MSG( hPixMap && *hPixMap, NULL,
                    _T("GetRawData(): failed to get PixMap from GWorld?") );

   wxCHECK_MSG( (*hPixMap)->pixelSize == bpp, NULL,
                    _T("GetRawData(): pixel format mismatch") );

   if ( !LockPixels(hPixMap) )
   {
       wxFAIL_MSG( _T("failed to lock PixMap in GetRawData()") );

       return NULL;
   }

   data.m_width = GetWidth();
   data.m_height = GetHeight();
   data.m_stride = (*hPixMap)->rowBytes & 0x7fff;

   M_BITMAPDATA->m_hasAlpha = false;

   return GetPixBaseAddr(hPixMap);
}

void wxBitmap::UngetRawData(wxPixelDataBase& dataBase)
{
    if ( !Ok() )
        return;

    if ( M_BITMAPDATA->m_hasAlpha )
    {
        wxAlphaPixelData& data = (wxAlphaPixelData&)dataBase;

        int w = data.GetWidth(),
            h = data.GetHeight();

        wxBitmap bmpMask(GetWidth(), GetHeight(), 32);
        wxAlphaPixelData dataMask(bmpMask, data.GetOrigin(), wxSize(w, h));
        wxAlphaPixelData::Iterator pMask(dataMask),
                                   p(data);
        for ( int y = 0; y < h; y++ )
        {
            wxAlphaPixelData::Iterator rowStartMask = pMask,
                                       rowStart = p;

            for ( int x = 0; x < w; x++ )
            {
                const wxAlphaPixelData::Iterator::ChannelType
                    alpha = p.Alpha();

                pMask.Red() = alpha;
                pMask.Green() = alpha;
                pMask.Blue() = alpha;

                ++p;
                ++pMask;
            }

            p = rowStart;
            p.OffsetY(data, 1);

            pMask = rowStartMask;
            pMask.OffsetY(dataMask, 1);
        }

        SetMask(new wxMask(bmpMask));
    }

    GWorldPtr gworld = MAC_WXHBITMAP(M_BITMAPDATA->m_hBitmap);
    PixMapHandle hPixMap = GetGWorldPixMap(gworld);
    if ( hPixMap )
    {
        UnlockPixels(hPixMap);
    }
}

void wxBitmap::UseAlpha()
{
    // remember that we are using alpha channel, we'll need to create a proper
    // mask in UngetRawData()
    M_BITMAPDATA->m_hasAlpha = true;
}
