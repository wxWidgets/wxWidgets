/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "bitmap.h"
#endif

#include "wx/wxprec.h"

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/image.h"
#include "wx/metafile.h"
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

// Implementation Notes
// -------------------- 
//
// we are always working with a 32 bit deep pixel buffer 
// under QuickDraw its alpha parts are going to be ignored in the GWorld, 
// therefore we have a separate GWorld there for blitting the mask in

// under Quartz then content is transformed into a CGImageRef representing the same data
// which can be transferred to the GPU by the OS for fast rendering

// we don't dare premultiplied alpha yet
#define wxMAC_USE_PREMULTIPLIED_ALPHA 0

IconFamilyHandle wxMacCreateIconFamily(const wxBitmap& bmp)
{
    // setup the header properly
    
    IconFamilyHandle iconFamily = (IconFamilyHandle) NewHandle(8) ;
    (**iconFamily).resourceType = kIconFamilyType ;
    (**iconFamily).resourceSize = sizeof(OSType) + sizeof(Size);
    
    int w = bmp.GetWidth() ;
    int h = bmp.GetHeight() ;
    int sz = wxMax( w , h ) ;
    if ( sz > 128 )
    {
        wxFAIL_MSG( wxT("Currently only 128 pixels wide images are supported") ) ;
    }
    
    Handle data = NULL ; 
    Handle maskdata = NULL ;
    unsigned char * maskptr = NULL ;
    unsigned char * ptr = NULL ;
    size_t size ;
    size_t masksize ;
    OSType dataType ;
    OSType maskType ;

    bool hasAlpha = bmp.HasAlpha() ;
    wxMask *mask = bmp.GetMask() ;
    // thumbnail is 128 x 128 with 32 bits per pixel
    
    if ( sz > 48 )
    {
        sz = 128 ;
        dataType = kThumbnail32BitData ;
        maskType = kThumbnail8BitMask ;
    }
    else if ( sz > 32 )
    {
        sz = 48 ;
        dataType = kHuge32BitData ;
        maskType = kHuge8BitMask ;
    }
    else if ( sz > 16 )
    {
        sz = 32 ;
        dataType = kLarge32BitData ;
        maskType = kLarge8BitMask ;
    }
    else 
    {
        sz = 16 ;
        dataType = kSmall32BitData ;
        maskType = kSmall8BitMask ;
    }

    size = sz * sz * 4 ;
    data = NewHandle( size) ;    
    HLock( data ) ;
    ptr = (unsigned char*) *data ;
    memset( ptr , 0, size ) ;

    masksize = sz * sz ;
    maskdata = NewHandle( masksize ) ;    
    HLock( maskdata ) ;
    maskptr = (unsigned char*) *maskdata ;
    memset( maskptr , 0 , masksize ) ;

    unsigned char * source = (unsigned char*) bmp.GetRawAccess() ;
    unsigned char * masksource = mask ? (unsigned char*) mask->GetRawAccess() : NULL ;
    for ( int y = 0 ; y < h ; ++y )
    {
        unsigned char * dest = ptr + y * sz * 4 ;
        unsigned char * maskdest = maskptr + y * sz ;
        for ( int x = 0 ; x < w ; ++x )
        {
            unsigned char a = *source ++ ;
            unsigned char r = *source ++ ;
            unsigned char g = *source ++ ;
            unsigned char b = *source ++ ;
            
            *dest++ = 0 ;
            *dest++ = r ;
            *dest++ = g ;
            *dest++ = b ;
            
            if ( mask )
                *maskdest++ = *masksource++ ;
            else if ( hasAlpha )
                *maskdest++ = a ;
            else
                *maskdest++ = 0xFF ;
        }
    }
    
    OSStatus err = SetIconFamilyData( iconFamily, dataType , data ) ;
    wxASSERT_MSG( err == noErr , wxT("Error when adding bitmap") ) ;

    err = SetIconFamilyData( iconFamily, maskType , maskdata ) ;
    wxASSERT_MSG( err == noErr , wxT("Error when adding mask") ) ;
    HUnlock( data ) ;
    HUnlock( maskdata ) ;
    DisposeHandle( data ) ;
    DisposeHandle( maskdata ) ;
    return iconFamily ;
}

IconRef wxMacCreateIconRef(const wxBitmap& bmp)
{
    IconFamilyHandle iconFamily = wxMacCreateIconFamily( bmp ) ;
    IconRef iconRef ;
    static int iconCounter = 2 ;
    
    OSStatus err = RegisterIconRefFromIconFamily( 'WXNG' , (OSType) iconCounter, iconFamily, &iconRef ) ;
    UInt16 owners ;
    err = GetIconRefOwners(iconRef , &owners ) ;

    wxASSERT_MSG( err == noErr , wxT("Error when adding bitmap") ) ;
    // we have to retain a reference, as Unregister will decrement it 
    AcquireIconRef( iconRef ) ;
    UnregisterIconRef( 'WXNG' , (OSType) iconCounter ) ;
    DisposeHandle( (Handle) iconFamily ) ;
    ++iconCounter ;
    
    return iconRef ;
}

PicHandle wxMacCreatePicHandle( const wxBitmap &bmp )
{
    CGrafPtr        origPort = NULL ;
    GDHandle        origDev = NULL ;
    PicHandle       pict = NULL ;
    GWorldPtr       wp = NULL ;
    GWorldPtr       mask = NULL ;

    GetGWorld( &origPort , &origDev ) ;

    wp = (GWorldPtr) bmp.GetHBITMAP( (WXHBITMAP*) &mask ) ;

    SetGWorld( wp , NULL ) ;
    Rect portRect ;
    GetPortBounds( wp , &portRect ) ;
    pict = OpenPicture(&portRect);
    
    if(pict)
    {
        RGBColor       white = { 0xffff ,0xffff , 0xffff } ;
        RGBColor       black = { 0x0000 ,0x0000 , 0x0000 } ;
        RGBForeColor( &black ) ;
        RGBBackColor( &white ) ;

        LockPixels( GetGWorldPixMap( wp ) ) ;
        CopyBits(GetPortBitMapForCopyBits(wp),
                GetPortBitMapForCopyBits(wp),
                &portRect,
                &portRect,
                srcCopy,NULL);
        UnlockPixels( GetGWorldPixMap( wp ) ) ;
        ClosePicture();
    }
    SetGWorld( origPort , origDev ) ;

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
        info->contentType = kControlContentIconRef ;
        info->u.iconRef = wxMacCreateIconRef( bitmap ) ;  
         
#if wxMAC_USE_CORE_GRAPHICS              
        /*
            // only on 10.4 more controls will accept a CGImage
        
            info->contentType = kControlContentCGImageRef ;
            info->u.imageRef = (CGImageRef) bmap->CGImageCreate() ;
        */
#endif
    }
}

void wxMacReleaseBitmapButton( ControlButtonContentInfo*info )
{
    if ( info->contentType == kControlContentIconRef )
    {
        ReleaseIconRef(info->u.iconRef) ;
    }
#if wxMAC_USE_CORE_GRAPHICS
    else if ( info->contentType == kControlContentCGImageRef )
    {
        CGImageRelease( info->u.imageRef ) ;
    }
#endif
    else
    {
        wxFAIL_MSG(wxT("Unexpected bitmap type") ) ;
    }
}


void wxBitmapRefData::Init()
{
    m_width = 0 ;
    m_height = 0 ;
    m_depth = 0 ;
    m_ok = false ;
    m_bitmapMask = NULL ;
#if wxMAC_USE_CORE_GRAPHICS
    m_cgImageRef = NULL ;
#endif
    m_hBitmap = NULL ;
    m_hMaskBitmap = NULL;
    m_maskBytesPerRow = NULL ;

    m_rawAccessCount = 0 ;
    m_hasAlpha = false;
}

wxBitmapRefData::wxBitmapRefData()
{
    Init() ;
}

wxBitmapRefData::wxBitmapRefData( int w , int h , int d ) 
{
    Init() ;
    Create( w , h , d ) ;
}

bool wxBitmapRefData::Create( int w , int h , int d ) 
{
    m_width = w ;
    m_height = h ; 
    m_depth = d ;

    m_bytesPerRow = w * 4 ;
    size_t size = m_bytesPerRow * h ;
    void*  data = m_memBuf.GetWriteBuf(size) ;
    memset( data ,  0 , size) ;
    m_memBuf.UngetWriteBuf(size) ;

    m_hBitmap = NULL ;
    Rect rect = { 0 , 0 , m_height , m_width } ;
    verify_noerr( NewGWorldFromPtr( (GWorldPtr*) &m_hBitmap , k32ARGBPixelFormat , &rect , NULL , NULL , 0 ,
        (char*) data , m_bytesPerRow ) ) ; 
    wxASSERT_MSG( m_hBitmap , wxT("Unable to create GWorld context") ) ;
    m_ok = ( m_hBitmap != NULL ) ;

    return m_ok ;   
}

void wxBitmapRefData::UseAlpha( bool use )
{
    if ( m_hasAlpha == use )
        return ;
        
    m_hasAlpha = use ;
    if ( m_hasAlpha )
    {
        int width = GetWidth() ;
        int height = GetHeight() ;
        m_maskBytesPerRow = ( width + 3 ) & 0xFFFFFFC ;
        size_t size = height * m_maskBytesPerRow ;
        unsigned char * data = (unsigned char * ) m_maskMemBuf.GetWriteBuf( size ) ;
        memset( data , 0 , size ) ;
        wxASSERT( m_hMaskBitmap == NULL ) ;
        Rect rect = { 0 , 0 , height , width } ;
        verify_noerr( NewGWorldFromPtr( (GWorldPtr*) &m_hMaskBitmap , k8IndexedGrayPixelFormat , &rect , NULL , NULL , 0 ,
            (char*) data , m_maskBytesPerRow ) ) ; 
        wxASSERT_MSG( m_hMaskBitmap , wxT("Unable to create GWorld context for alpha mask") ) ;
        m_maskMemBuf.UngetWriteBuf(size) ;
#if !wxMAC_USE_CORE_GRAPHICS
        UpdateAlphaMask() ;
#endif
    }
    else
    {
        DisposeGWorld( m_hMaskBitmap ) ;
        m_hMaskBitmap = NULL ;
        m_maskBytesPerRow = 0 ;
    }
}

void *wxBitmapRefData::GetRawAccess() const
{
    wxCHECK_MSG( Ok(), NULL , wxT("invalid bitmap") ) ;
    return m_memBuf.GetData() ;
}

void *wxBitmapRefData::BeginRawAccess() 
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") ) ;
    wxASSERT( m_rawAccessCount == 0 ) ;
    ++m_rawAccessCount ;
#if wxMAC_USE_CORE_GRAPHICS
     // we must destroy an existing cached image, as
    // the bitmap data may change now
    if ( m_cgImageRef )
    {
        CGImageRelease( m_cgImageRef ) ;
        m_cgImageRef = NULL ;
    }
#endif
    return m_memBuf.GetData() ;
}

void wxBitmapRefData::EndRawAccess()
{
    wxCHECK_RET( Ok() , wxT("invalid bitmap") ) ;
    wxASSERT( m_rawAccessCount == 1 ) ;
    --m_rawAccessCount ;
#if !wxMAC_USE_CORE_GRAPHICS
    UpdateAlphaMask() ;
#endif
}


#if wxMAC_USE_CORE_GRAPHICS
void wxMacMemoryBufferReleaseProc(void *info, const void *data, size_t size)
{
    wxMemoryBuffer* membuf = (wxMemoryBuffer*) info ;
    wxASSERT( data == membuf->GetData() ) ;
    delete membuf ;
}

CGImageRef wxBitmapRefData::CGImageCreate() const
{
    wxASSERT( m_ok ) ;
    wxASSERT( m_rawAccessCount >= 0 ) ;
    CGImageRef image ;
    if ( m_rawAccessCount > 0 || m_cgImageRef == NULL )
    {
        size_t imageSize = m_width * m_height * 4 ;
        void * dataBuffer = m_memBuf.GetData() ;
        int w = m_width ;
        int h = m_height ;
        CGImageAlphaInfo alphaInfo = kCGImageAlphaNoneSkipFirst ;
        wxMemoryBuffer* membuf = NULL ;
       
        if ( m_bitmapMask )
        {
            membuf = new wxMemoryBuffer( imageSize ) ;
            memcpy( membuf->GetData() , dataBuffer , imageSize ) ;
            unsigned char *sourcemaskstart = (unsigned char *) m_bitmapMask->GetRawAccess() ;
            int maskrowbytes = m_bitmapMask->GetBytesPerRow() ;
            unsigned char *destalpha = (unsigned char *) membuf->GetData() ;
            alphaInfo = kCGImageAlphaFirst ;
            for ( int y = 0 ; y < h ; ++y , sourcemaskstart += maskrowbytes)
            {
                unsigned char *sourcemask = sourcemaskstart ;
                for( int x = 0 ; x < w ; ++x , sourcemask++ , destalpha += 4 )
                {
                    *destalpha = *sourcemask ;
                }
            }
        }
        else if ( m_hasAlpha )
        {
#if wxMAC_USE_PREMULTIPLIED_ALPHA
            alphaInfo = kCGImageAlphaPremultipliedFirst ;
#else
            alphaInfo = kCGImageAlphaFirst ;
#endif
            membuf = new wxMemoryBuffer( m_memBuf ) ;
        }
        else
        {
            membuf = new wxMemoryBuffer( m_memBuf ) ;
        }
        CGColorSpaceRef colorSpace = wxMacGetGenericRGBColorSpace();
    	CGDataProviderRef dataProvider = 
    	    CGDataProviderCreateWithData( membuf , (const void *)membuf->GetData() , imageSize, 
    	        wxMacMemoryBufferReleaseProc );
        image = 
    	    ::CGImageCreate( w, h, 8 , 32 , 4 * m_width , colorSpace, alphaInfo , 
    			dataProvider, NULL , false , kCGRenderingIntentDefault );
      	CGDataProviderRelease( dataProvider);
    }
    else
    {
        image = m_cgImageRef ;
        CGImageRetain( image ) ;
    }
    if ( m_rawAccessCount == 0 && m_cgImageRef == NULL)
    {
        // we keep it for later use
        m_cgImageRef = image ;
        CGImageRetain( image ) ;
    } 
    return image ;
}
#endif

GWorldPtr wxBitmapRefData::GetHBITMAP(GWorldPtr* mask) const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );
    if ( mask )
    {
        *mask = NULL ;
        if ( m_bitmapMask )
            *mask = (GWorldPtr) m_bitmapMask->GetHBITMAP() ;        
        else if ( m_hasAlpha )
        {
#if !wxMAC_USE_CORE_GRAPHICS
            if ( m_rawAccessCount > 0 )
                UpdateAlphaMask() ;
#else
            // this structure is not kept in synch when using CG, so if someone
            // is really accessing the Graphports, we have to sync it
            UpdateAlphaMask() ;
#endif
            *mask = m_hMaskBitmap ;
        }
    }
    return m_hBitmap ;
}

void wxBitmapRefData::UpdateAlphaMask() const 
{
    if ( m_hasAlpha )
    {
        unsigned char *sourcemask = (unsigned char *) GetRawAccess() ;
        unsigned char *destalphabase = (unsigned char *) m_maskMemBuf.GetData() ;
     
        int h = GetHeight() ;
        int w = GetWidth() ;
     
        for ( int y = 0 ; y < h ; ++y , destalphabase += m_maskBytesPerRow )
        {
            unsigned char* destalpha = destalphabase ;
            for( int x = 0 ; x < w ; ++x , sourcemask += 4, ++destalpha )
            {
                *destalpha = *sourcemask ;
            }
        }
    }
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( m_rawAccessCount == 0 , wxT("Bitmap still selected when destroyed") ) ;

#if wxMAC_USE_CORE_GRAPHICS
    if ( m_cgImageRef )
    {
        CGImageRelease( m_cgImageRef ) ;
        m_cgImageRef = NULL ;
    }
#endif
    if ( m_hBitmap )
    {
        DisposeGWorld( MAC_WXHBITMAP(m_hBitmap) ) ;
        m_hBitmap = NULL ;
    }
    if ( m_hMaskBitmap )
    {
        DisposeGWorld( MAC_WXHBITMAP(m_hMaskBitmap) ) ;
        m_hMaskBitmap = NULL ;
    }

    if (m_bitmapMask)
    {
        delete m_bitmapMask;
        m_bitmapMask = NULL;
    }
}

wxBitmapRefData::~wxBitmapRefData()
{
    Free() ;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    int w = icon.GetWidth() ;
    int h = icon.GetHeight() ;
    Create( icon.GetWidth() , icon.GetHeight() ) ;

    if ( w == h && ( w == 16 || w == 32 || w == 48 || w == 128 ) )
    {
        IconFamilyHandle iconFamily = NULL ;
        Handle imagehandle = NewHandle(0) ;
        Handle maskhandle = NewHandle(0) ;
        
        OSType maskType ;
        OSType dataType ;
        IconSelectorValue selector ;    
        if ( w == 128 )
        {
            dataType = kThumbnail32BitData ;
            maskType = kThumbnail8BitMask ;
            selector = kSelectorAllAvailableData ;
        }
        else if ( w == 48 )
        {
            dataType = kHuge32BitData ;
            maskType = kHuge8BitMask ;
            selector = kSelectorHuge32Bit | kSelectorHuge8BitMask ;
        }
        else if ( w == 32 )
        {
            dataType = kLarge32BitData ;
            maskType = kLarge8BitMask ;
            selector = kSelectorLarge32Bit | kSelectorLarge8BitMask ;
        }
        else if ( w == 16 )
        {
            dataType = kSmall32BitData ;
            maskType = kSmall8BitMask ;
            selector = kSelectorSmall32Bit | kSelectorSmall8BitMask ;
        }
        else
        {
            wxFAIL_MSG(wxT("Illegal icon size for conversion") ) ;
        }


        OSStatus err = ( IconRefToIconFamily( MAC_WXHICON(icon.GetHICON()) , selector , &iconFamily ) ) ;
        
        err =( GetIconFamilyData( iconFamily , dataType , imagehandle ) ) ;
        err =( GetIconFamilyData( iconFamily , maskType , maskhandle ) ) ;
        wxASSERT( GetHandleSize( imagehandle ) == w * 4 * h ) ;
        wxASSERT( GetHandleSize( maskhandle ) == w * h ) ;
        UseAlpha() ;
        unsigned char *source = (unsigned char *) *imagehandle ;
        unsigned char *sourcemask = (unsigned char *) *maskhandle ;
        
        unsigned char* destination = (unsigned char*) BeginRawAccess() ;
        for ( int y = 0 ; y < h ; ++y )
        {
            for ( int x = 0 ; x < w ; ++x )
            {
                *destination++ = *sourcemask++ ;
                source++ ;
                *destination++ = *source++ ;
                *destination++ = *source++ ;
                *destination++ = *source++ ;
            }
        }
        EndRawAccess() ;
        DisposeHandle( imagehandle ) ;
        DisposeHandle( maskhandle ) ;
    }
    else
    {            
        wxMemoryDC dc ;
        dc.SelectObject( *this ) ;
        dc.DrawIcon( icon , 0 , 0 ) ;
        dc.SelectObject( wxNullBitmap ) ;
    }
    return true;
}

wxBitmap::wxBitmap()
{
}

wxBitmap::~wxBitmap()
{
}

wxBitmap::wxBitmap(const char bits[], int the_width, int the_height, int no_bits)
{
    m_refData = new wxBitmapRefData( the_width , the_height , no_bits ) ;

    if ( no_bits == 1 )
    {
        int linesize = ( the_width / (sizeof(unsigned char) * 8)) ;
        if ( the_width % (sizeof(unsigned char) * 8) ) {
            linesize += sizeof(unsigned char);
        }
        unsigned char* linestart = (unsigned char*) bits ;
        unsigned char* destination = (unsigned char*) BeginRawAccess() ;
        for ( int y = 0 ; y < the_height ; ++y , linestart += linesize )
        {
            for ( int x = 0 ; x < the_width ; ++x )
            {
                int index = x / 8 ;
                int bit = x % 8 ;
                int mask = 1 << bit ;
                if ( linestart[index] & mask )
                {
                    *destination++ = 0xFF ;
                    *destination++ = 0 ;
                    *destination++ = 0 ;
                    *destination++ = 0 ;
                }
                else
                {
                    *destination++ = 0xFF ;
                    *destination++ = 0xFF ;
                    *destination++ = 0xFF ;
                    *destination++ = 0xFF ;
                }
            }
        }
        EndRawAccess() ;
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

wxBitmap::wxBitmap(const char **bits)
{
    (void) CreateFromXpm(bits);
}

wxBitmap::wxBitmap(char **bits)
{
    (void) CreateFromXpm((const char **)bits);
}

void* wxBitmap::GetRawAccess() const
{
    wxCHECK_MSG( Ok() , NULL , wxT("invalid bitmap") ) ;
    return M_BITMAPDATA->GetRawAccess() ;
}

void* wxBitmap::BeginRawAccess() 
{
    wxCHECK_MSG( Ok() , NULL , wxT("invalid bitmap") ) ;
    return M_BITMAPDATA->BeginRawAccess() ;
}

void wxBitmap::EndRawAccess()
{
    wxCHECK_RET( Ok() , wxT("invalid bitmap") ) ;
    M_BITMAPDATA->EndRawAccess() ;
}

bool wxBitmap::CreateFromXpm(const char **bits)
{
    wxCHECK_MSG( bits != NULL, FALSE, wxT("invalid bitmap data") )
    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(bits);
    wxCHECK_MSG( img.Ok(), FALSE, wxT("invalid bitmap data") )
    *this = wxBitmap(img);
    return TRUE;
}

#if wxMAC_USE_CORE_GRAPHICS
WXCGIMAGEREF wxBitmap::CGImageCreate() const
{
    wxCHECK_MSG( Ok(), NULL , wxT("invalid bitmap") ) ;
    return M_BITMAPDATA->CGImageCreate() ;
}
#endif

wxBitmap wxBitmap::GetSubBitmap(const wxRect &rect) const
{
    wxCHECK_MSG( Ok() &&
                (rect.x >= 0) && (rect.y >= 0) &&
                (rect.x+rect.width <= GetWidth()) &&
                (rect.y+rect.height <= GetHeight()),
                wxNullBitmap, wxT("invalid bitmap or bitmap region") );


    wxBitmap ret( rect.width, rect.height, GetDepth() );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );


    int sourcewidth = GetWidth() ;
    int destwidth = rect.width ;
    int destheight = rect.height ;
    {
        unsigned char * sourcedata = (unsigned char*) GetRawAccess() ;
        unsigned char * destdata = (unsigned char*) ret.BeginRawAccess() ;
        int sourcelinesize = sourcewidth * 4 ;
        int destlinesize = destwidth * 4 ;
        unsigned char *source = sourcedata + rect.x * 4 + rect.y * sourcelinesize ;
        unsigned char *dest = destdata ;
        for(int yy = 0; yy < destheight; ++yy, source += sourcelinesize , dest += destlinesize)
        {
            memcpy( dest , source , destlinesize ) ;
        }
    }
    ret.EndRawAccess() ;
  
    if ( M_BITMAPDATA->m_bitmapMask )
    {
        wxMemoryBuffer maskbuf ;
        int rowBytes = ( destwidth + 3 ) & 0xFFFFFFC ;
        size_t maskbufsize = rowBytes * destheight ;
        unsigned char * destdata = (unsigned char * ) maskbuf.GetWriteBuf( maskbufsize ) ;

        int sourcelinesize = M_BITMAPDATA->m_bitmapMask->GetBytesPerRow()  ;
        int destlinesize = rowBytes ;
        unsigned char *source = (unsigned char *) M_BITMAPDATA->m_bitmapMask->GetRawAccess() ;
        source += rect.x + rect.y * sourcelinesize ;
        unsigned char *dest = destdata ;

        for(int yy = 0; yy < destheight; ++yy, source += sourcelinesize , dest += destlinesize)
        {
            memcpy( dest , source , destlinesize ) ;
        }
        maskbuf.UngetWriteBuf( maskbufsize ) ;
        ret.SetMask( new wxMask( maskbuf , destwidth , destheight , rowBytes ) ) ;
    }
    else if ( HasAlpha() )
        ret.UseAlpha() ;

    return ret;
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    if ( d < 0 )
        d = wxDisplayDepth() ;

    m_refData = new wxBitmapRefData( w , h , d );

    return M_BITMAPDATA->Ok() ;
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

        return FALSE;
    }

    return handler->Create(this, data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") )

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int height = image.GetHeight();

    m_refData = new wxBitmapRefData( width , height , depth ) ;;

    // Create picture

    bool hasAlpha = false ;
    
    if ( image.HasMask() )
    {
        // takes precedence, don't mix with alpha info
    }
    else
    {
        hasAlpha = image.HasAlpha() ;
    }
    
    if ( hasAlpha )
        UseAlpha() ;
        
    unsigned char* destination = (unsigned char*) BeginRawAccess() ;

    register unsigned char* data = image.GetData();
    const unsigned char *alpha = hasAlpha ? image.GetAlpha() : NULL ;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if ( hasAlpha )
            {
                const unsigned char a = *alpha++;
                *destination++ = a ;
#if wxMAC_USE_PREMULTIPLIED_ALPHA
                *destination++ = ((*data++) * a + 127 ) / 255 ;
                *destination++ = ((*data++) * a + 127 ) / 255 ;
                *destination++ = ((*data++) * a + 127 ) / 255 ;
#else
                *destination++ = *data++ ;
                *destination++ = *data++ ;
                *destination++ = *data++ ;
#endif
            }
            else
            {
                *destination++ = 0xFF ;
                *destination++ = *data++ ;
                *destination++ = *data++ ;
                *destination++ = *data++ ;
            }
        }
    }
    EndRawAccess() ;
    if ( image.HasMask() )
    {
        SetMask( new wxMask( *this , wxColour( image.GetMaskRed() , image.GetMaskGreen() , image.GetMaskBlue() ) ) ) ;
    }
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

    unsigned char* source = (unsigned char*) GetRawAccess() ;

    bool hasAlpha = false ;
    bool hasMask = false ;
    unsigned char *alpha = NULL ;
    unsigned char *mask = NULL ;
    if ( HasAlpha() )
    {
        hasAlpha = true ;
    }

    if ( GetMask() )
    {
        hasMask = true ;
        mask = (unsigned char*) GetMask()->GetRawAccess() ;
    }

    if ( hasAlpha )
    {
        image.SetAlpha() ;
        alpha = image.GetAlpha() ;
    }
    int index = 0;
    
    // The following masking algorithm is the same as well in msw/gtk:
    // the colour used as transparent one in wxImage and the one it is
    // replaced with when it really occurs in the bitmap
    static const int MASK_RED = 1;
    static const int MASK_GREEN = 2;
    static const int MASK_BLUE = 3;
    static const int MASK_BLUE_REPLACEMENT = 2;

    for (int yy = 0; yy < height; yy++)
    {
        for (int xx = 0; xx < width; xx++)
        {
            long color = *((long*) source) ;
            unsigned char a = ((color&0xFF000000) >> 24) ;
            unsigned char r = ((color&0x00FF0000) >> 16) ;
            unsigned char g = ((color&0x0000FF00) >> 8) ;
            unsigned char b = (color&0x000000FF);
            if ( hasMask )
            {
                if ( *mask++ == 0 )
                {
                    if ( r == MASK_RED && g == MASK_GREEN && b == MASK_BLUE )
                        b = MASK_BLUE_REPLACEMENT ;
                }
                else
                {
                    r = MASK_RED ;
                    g = MASK_GREEN ;
                    b = MASK_BLUE ;
                }
            }
            else if ( hasAlpha )
                *alpha++ = a ;

            data[index    ] = r ;
            data[index + 1] = g ;
            data[index + 2] = b ;
            index += 3;
            source += 4 ;
        }
    }
    if ( hasMask )
        image.SetMaskColour( MASK_RED, MASK_GREEN, MASK_BLUE );
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

bool wxBitmap::Ok() const
{
   return (M_BITMAPDATA && M_BITMAPDATA->Ok());
}

int wxBitmap::GetHeight() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->GetHeight();
}

int wxBitmap::GetWidth() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->GetWidth() ;
}

int wxBitmap::GetDepth() const
{
   wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

   return M_BITMAPDATA->GetDepth();
}

int wxBitmap::GetQuality() const
{
    return 0;
}

wxMask *wxBitmap::GetMask() const
{
   wxCHECK_MSG( Ok(), (wxMask *) NULL, wxT("invalid bitmap") );

   return M_BITMAPDATA->m_bitmapMask;
}

bool wxBitmap::HasAlpha() const
{
   wxCHECK_MSG( Ok(), false , wxT("invalid bitmap") );

   return M_BITMAPDATA->HasAlpha() ;
}

void wxBitmap::SetWidth(int w)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->SetWidth(w);
}

void wxBitmap::SetHeight(int h)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->SetHeight(h);
}

void wxBitmap::SetDepth(int d)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->SetDepth(d);
}

void wxBitmap::SetQuality(int WXUNUSED(quality))
{
}

void wxBitmap::SetOk(bool isOk)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->SetOk(isOk);
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

WXHBITMAP wxBitmap::GetHBITMAP(WXHBITMAP* mask) const
{
    return WXHBITMAP(M_BITMAPDATA->GetHBITMAP((GWorldPtr*)mask));
}

// ----------------------------------------------------------------------------
// wxMask
// ----------------------------------------------------------------------------

wxMask::wxMask()
{
    Init() ;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    Init() ;
    Create(bitmap, colour);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    Init() ;
    Create(bitmap);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxMemoryBuffer& data, int width , int height , int bytesPerRow )
{
    Init() ;
    Create(data, width , height , bytesPerRow );
}

wxMask::~wxMask()
{
    if ( m_maskBitmap )
    {
        DisposeGWorld( (GWorldPtr) m_maskBitmap ) ;
        m_maskBitmap = NULL ;
    }
}

void wxMask::Init() 
{
    m_width = m_height = m_bytesPerRow = 0 ;
    m_maskBitmap = NULL ;
}

void *wxMask::GetRawAccess() const
{
    return m_memBuf.GetData() ;
}

void wxMask::RealizeNative() 
{
    if ( m_maskBitmap )
    {
       DisposeGWorld(  (GWorldPtr) m_maskBitmap ) ;
       m_maskBitmap = NULL ;
    }
    Rect rect = { 0 , 0 , m_height , m_width } ;
    verify_noerr( NewGWorldFromPtr( (GWorldPtr*) &m_maskBitmap , k8IndexedGrayPixelFormat , &rect , NULL , NULL , 0 ,
        (char*) m_memBuf.GetData() , m_bytesPerRow ) ) ; 
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxMemoryBuffer& data,int width , int height , int bytesPerRow)
{
    m_memBuf = data ;
    m_width = width ;
    m_height = height ;
    m_bytesPerRow = bytesPerRow ;
    wxASSERT( data.GetDataLen() == (size_t)(height * bytesPerRow) ) ;
    RealizeNative() ;
    return true ;
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& bitmap)
{
    m_width = bitmap.GetWidth() ;
    m_height = bitmap.GetHeight() ;
    m_bytesPerRow = ( m_width + 3 ) & 0xFFFFFFC ;
    size_t size = m_bytesPerRow * m_height ;
    unsigned char * destdatabase = (unsigned char*) m_memBuf.GetWriteBuf( size ) ;
    memset( destdatabase , 0 , size ) ;
    unsigned char * srcdata = (unsigned char*) bitmap.GetRawAccess() ;
    for ( int y = 0 ; y < m_height ; ++y , destdatabase += m_bytesPerRow )
    {
        unsigned char *destdata= destdatabase ;
        for( int x = 0 ; x < m_width ; ++x )
        {
            srcdata++ ;
            unsigned char r = *srcdata++ ;
            unsigned char g = *srcdata++ ;
            unsigned char b = *srcdata++ ;
            if ( ( r + g + b ) > 0x10 )
                *destdata++ = 0x00 ;
            else
                *destdata++ = 0xFF ;
        }
    }
    m_memBuf.UngetWriteBuf( size ) ;
    RealizeNative() ;
    return TRUE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    m_width = bitmap.GetWidth() ;
    m_height = bitmap.GetHeight() ;
    m_bytesPerRow = ( m_width + 3 ) & 0xFFFFFFC ;
    size_t size = m_bytesPerRow * m_height ;

    unsigned char * destdatabase = (unsigned char*) m_memBuf.GetWriteBuf( size ) ;
    memset( destdatabase , 0 , size ) ;
    unsigned char * srcdata = (unsigned char*) bitmap.GetRawAccess() ;
    for ( int y = 0 ; y < m_height ; ++y , destdatabase += m_bytesPerRow)
    {
        unsigned char *destdata= destdatabase ;
        for( int x = 0 ; x < m_width ; ++x )
        {
            srcdata++ ;
            unsigned char r = *srcdata++ ;
            unsigned char g = *srcdata++ ;
            unsigned char b = *srcdata++ ;
            if ( colour == wxColour( r , g , b) )
                *destdata++ = 0x00 ;
            else
                *destdata++ = 0xFF ;
        }
    }
    m_memBuf.UngetWriteBuf( size ) ;
    RealizeNative() ;
    return TRUE;
}

WXHBITMAP wxMask::GetHBITMAP() const
{
    return m_maskBitmap ;
}

// ----------------------------------------------------------------------------
// wxBitmapHandler
// ----------------------------------------------------------------------------

wxBitmapHandler::~wxBitmapHandler()
{
}

bool wxBitmapHandler::Create(wxBitmap *bitmap, void *data, long type, int width, int height, int depth)
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
        int desiredWidth, int desiredHeight)
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
{
    return FALSE;
}

// ----------------------------------------------------------------------------
// Standard Handlers
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPICTResourceHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxPICTResourceHandler)
public:
    inline wxPICTResourceHandler()
    {
        SetName(wxT("Macintosh Pict resource"));
        SetExtension(wxEmptyString);
        SetType(wxBITMAP_TYPE_PICT_RESOURCE);
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
        wxMetafile mf ;
        mf.SetHMETAFILE((WXHMETAFILE) thePict ) ;
        bitmap->Create( mf.GetWidth() , mf.GetHeight() ) ;
        wxMemoryDC dc ;
        dc.SelectObject( *bitmap ) ;
        mf.Play( &dc ) ;
        dc.SelectObject( wxNullBitmap ) ;
        return TRUE ;
    }
    return FALSE ;
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

    data.m_width = GetWidth() ;
    data.m_height = GetHeight() ;
    data.m_stride = GetWidth() * 4 ;
    return GetRawAccess() ;
}

void wxBitmap::UngetRawData(wxPixelDataBase& dataBase)
{
    if ( !Ok() )
        return;

    // TODO : if we have some information about the API we should check
    // this code looks strange...

    if ( M_BITMAPDATA->HasAlpha() )
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
}

void wxBitmap::UseAlpha()
{
    // remember that we are using alpha channel, we'll need to create a proper
    // mask in UngetRawData()
    M_BITMAPDATA->UseAlpha( true );
}
