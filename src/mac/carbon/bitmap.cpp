/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/bitmap.cpp
// Purpose:     wxBitmap
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif

#include "wx/metafile.h"
#include "wx/xpmdecod.h"

#include "wx/rawbmp.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

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

// we don't dare use premultiplied alpha yet
#define wxMAC_USE_PREMULTIPLIED_ALPHA 0

#if wxUSE_BMPBUTTON

void wxMacCreateBitmapButton( ControlButtonContentInfo*info , const wxBitmap& bitmap , int forceType )
{
    memset( info , 0 , sizeof(ControlButtonContentInfo) ) ;
    if ( bitmap.Ok() )
    {
        wxBitmapRefData * bmap = bitmap.GetBitmapData() ;
        if ( bmap == NULL )
            return ;

        if ( ( bmap->HasNativeSize() && forceType == 0 ) || forceType == kControlContentIconRef )
        {
            wxBitmap scaleBmp ;
            wxBitmapRefData* bmp = bmap ;

            if ( !bmap->HasNativeSize() )
            {
                // as PICT conversion will only result in a 16x16 icon, let's attempt
                // a few scales for better results

                int w = bitmap.GetWidth() ;
                int h = bitmap.GetHeight() ;
                int sz = wxMax( w , h ) ;
                if ( sz == 24 || sz == 64 )
                {
                    scaleBmp = wxBitmap( bitmap.ConvertToImage().Scale( w * 2 , h * 2 ) ) ;
                    bmp = scaleBmp.GetBitmapData() ;
                }
            }

            info->contentType = kControlContentIconRef ;
            info->u.iconRef = bmp->GetIconRef() ;
            AcquireIconRef( info->u.iconRef ) ;
        }
#if defined( __WXMAC_OSX__ ) && MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
        else if ( forceType == kControlContentCGImageRef )
        {
            info->contentType = kControlContentCGImageRef ;
            info->u.imageRef = (CGImageRef) bmap->CGImageCreate() ;
        }
#endif
        else
        {
#ifndef __LP64__
            info->contentType = kControlContentPictHandle ;
            info->u.picture = bmap->GetPictHandle() ;
#endif
        }
    }
}

void wxMacReleaseBitmapButton( ControlButtonContentInfo*info )
{
    if ( info->contentType == kControlContentIconRef )
    {
        ReleaseIconRef( info->u.iconRef ) ;
    }
    else if ( info->contentType == kControlNoContent )
    {
        // there's no bitmap at all, fall through silently
    }
    else if ( info->contentType == kControlContentPictHandle )
    {
        // owned by the bitmap, no release here
    }
#if defined( __WXMAC_OSX__ ) && MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
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

#endif //wxUSE_BMPBUTTON

#define M_BITMAPDATA ((wxBitmapRefData *)m_refData)

void wxBitmapRefData::Init()
{
    m_width = 0 ;
    m_height = 0 ;
    m_depth = 0 ;
    m_ok = false ;
    m_bitmapMask = NULL ;

#ifdef __WXMAC_OSX__
    m_cgImageRef = NULL ;
#endif

    m_iconRef = NULL ;
    m_pictHandle = NULL ;
    m_hBitmap = NULL ;
    m_hMaskBitmap = NULL;
    m_maskBytesPerRow = 0 ;

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
    m_width = wxMax(1, w);
    m_height = wxMax(1, h);
    m_depth = d ;

    m_bytesPerRow = w * 4 ;
    size_t size = m_bytesPerRow * h ;
    void* data = m_memBuf.GetWriteBuf( size ) ;
    memset( data , 0 , size ) ;
    m_memBuf.UngetWriteBuf( size ) ;

    m_hBitmap = NULL ;
    Rect rect = { 0 , 0 , m_height , m_width } ;
#ifndef __LP64__
    verify_noerr( NewGWorldFromPtr( (GWorldPtr*) &m_hBitmap , k32ARGBPixelFormat , &rect , NULL , NULL , 0 ,
        (char*) data , m_bytesPerRow ) ) ;
    wxASSERT_MSG( m_hBitmap , wxT("Unable to create GWorld context") ) ;
#endif
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
        wxASSERT( m_hMaskBitmap == NULL ) ;

        int width = GetWidth() ;
        int height = GetHeight() ;
        m_maskBytesPerRow = ( width * 4 + 3 ) & 0xFFFFFFC ;
        size_t size = height * m_maskBytesPerRow ;
        unsigned char * data = (unsigned char * ) m_maskMemBuf.GetWriteBuf( size ) ;
        wxASSERT( data != NULL ) ;

        memset( data , 0 , size ) ;
        Rect rect = { 0 , 0 , height , width } ;
#ifndef __LP64__
        verify_noerr( NewGWorldFromPtr( (GWorldPtr*) &m_hMaskBitmap , k32ARGBPixelFormat , &rect , NULL , NULL , 0 ,
            (char*) data , m_maskBytesPerRow ) ) ;
        wxASSERT_MSG( m_hMaskBitmap , wxT("Unable to create GWorld context for alpha mask") ) ;
#endif
        m_maskMemBuf.UngetWriteBuf(size) ;

#if !wxMAC_USE_CORE_GRAPHICS
        UpdateAlphaMask() ;
#endif
    }
    else
    {
#ifndef __LP64__
        DisposeGWorld( m_hMaskBitmap ) ;
#endif
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
    wxASSERT_MSG( m_pictHandle == NULL && m_iconRef == NULL ,
        wxT("Currently, modifing bitmaps that are used in controls already is not supported") ) ;

    ++m_rawAccessCount ;

#ifdef __WXMAC_OSX__
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

bool wxBitmapRefData::HasNativeSize()
{
    int w = GetWidth() ;
    int h = GetHeight() ;
    int sz = wxMax( w , h ) ;

    return ( sz == 128 || sz == 48 || sz == 32 || sz == 16 );
}

IconRef wxBitmapRefData::GetIconRef()
{
    if ( m_iconRef == NULL )
    {
        // Create Icon Family Handle

        IconFamilyHandle iconFamily = NULL ;

#ifdef WORDS_BIGENDIAN
        iconFamily = (IconFamilyHandle) NewHandle( 8 ) ;
        (**iconFamily).resourceType = kIconFamilyType ;
        (**iconFamily).resourceSize = sizeof(OSType) + sizeof(Size);
#else
        // test this solution on big endian as well
        iconFamily = (IconFamilyHandle) NewHandle( 0 ) ;
#endif

        int w = GetWidth() ;
        int h = GetHeight() ;
        int sz = wxMax( w , h ) ;

        OSType dataType = 0 ;
        OSType maskType = 0 ;

        switch (sz)
        {
            case 128:
                dataType = kThumbnail32BitData ;
                maskType = kThumbnail8BitMask ;
                break;

            case 48:
                dataType = kHuge32BitData ;
                maskType = kHuge8BitMask ;
                break;

            case 32:
                dataType = kLarge32BitData ;
                maskType = kLarge8BitMask ;
                break;

            case 16:
                dataType = kSmall32BitData ;
                maskType = kSmall8BitMask ;
                break;

            default:
                break;
        }

        if ( dataType != 0 )
        {
            // setup the header properly

            Handle data = NULL ;
            Handle maskdata = NULL ;
            unsigned char * maskptr = NULL ;
            unsigned char * ptr = NULL ;
            size_t datasize, masksize ;

            datasize = sz * sz * 4 ;
            data = NewHandle( datasize ) ;
            HLock( data ) ;
            ptr = (unsigned char*) *data ;
            memset( ptr , 0, datasize ) ;

            masksize = sz * sz ;
            maskdata = NewHandle( masksize ) ;
            HLock( maskdata ) ;
            maskptr = (unsigned char*) *maskdata ;
            memset( maskptr , 0 , masksize ) ;

            bool hasAlpha = HasAlpha() ;
            wxMask *mask = m_bitmapMask ;
            unsigned char * source = (unsigned char*) GetRawAccess() ;
            unsigned char * masksource = mask ? (unsigned char*) mask->GetRawAccess() : NULL ;

            for ( int y = 0 ; y < h ; ++y )
            {
                unsigned char * dest = ptr + y * sz * 4 ;
                unsigned char * maskdest = maskptr + y * sz ;
                unsigned char a, r, g, b;

                for ( int x = 0 ; x < w ; ++x )
                {
                    a = *source ++ ;
                    r = *source ++ ;
                    g = *source ++ ;
                    b = *source ++ ;

                    *dest++ = 0 ;
                    *dest++ = r ;
                    *dest++ = g ;
                    *dest++ = b ;

                    if ( mask )
                    {
                        *maskdest++ = 0xFF - *masksource++ ;
                        masksource++ ;
                        masksource++ ;
                        masksource++ ;
                    }
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
        }
        else
        {
            PicHandle pic = GetPictHandle() ;
            SetIconFamilyData( iconFamily, 'PICT' , (Handle) pic ) ;
        }
        // transform into IconRef
#if defined( __WXMAC_OSX__ ) && MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
        // cleaner version existing from 10.3 upwards
        HLock((Handle) iconFamily);
        OSStatus err = GetIconRefFromIconFamilyPtr( *iconFamily, GetHandleSize((Handle) iconFamily), &m_iconRef );
        HUnlock((Handle) iconFamily);
        wxASSERT_MSG( err == noErr , wxT("Error when constructing icon ref") );
#else
        static int iconCounter = 2 ;

        OSStatus err = RegisterIconRefFromIconFamily( 'WXNG' , (OSType) iconCounter, iconFamily, &m_iconRef ) ;
        wxASSERT_MSG( err == noErr , wxT("Error when adding bitmap") ) ;

        // we have to retain a reference, as Unregister will decrement it
        AcquireIconRef( m_iconRef ) ;
        UnregisterIconRef( 'WXNG' , (OSType) iconCounter ) ;
        ++iconCounter ;
#endif
        DisposeHandle( (Handle) iconFamily ) ;
    }

    return m_iconRef ;
}

PicHandle wxBitmapRefData::GetPictHandle()
{
    if ( m_pictHandle == NULL )
    {
#ifndef __LP64__
        CGrafPtr origPort = NULL ;
        GDHandle origDev = NULL ;
        GWorldPtr wp = NULL ;
        GWorldPtr mask = NULL ;
        int height = GetHeight() ;
        int width = GetWidth() ;

        Rect rect = { 0 , 0 , height , width } ;
        RgnHandle clipRgn = NULL ;

        GetGWorld( &origPort , &origDev ) ;
        wp = GetHBITMAP( &mask ) ;

        if ( mask )
        {
            GWorldPtr monoworld ;
            clipRgn = NewRgn() ;
            OSStatus err = NewGWorld( &monoworld , 1 , &rect , NULL , NULL , 0 ) ;
            verify_noerr(err) ;
            LockPixels( GetGWorldPixMap( monoworld ) ) ;
            LockPixels( GetGWorldPixMap( mask ) ) ;
            SetGWorld( monoworld , NULL ) ;

            RGBColor white = { 0xffff , 0xffff , 0xffff } ;
            RGBColor black = { 0x0000 , 0x0000 , 0x0000 } ;
            RGBForeColor( &black ) ;
            RGBBackColor( &white ) ;

            CopyBits(GetPortBitMapForCopyBits(mask),
                    GetPortBitMapForCopyBits(monoworld),
                    &rect,
                    &rect,
                    srcCopy, NULL);
            BitMapToRegion( clipRgn , (BitMap*) *GetGWorldPixMap( monoworld ) ) ;

            UnlockPixels( GetGWorldPixMap( monoworld ) ) ;
            UnlockPixels( GetGWorldPixMap( mask ) ) ;
            DisposeGWorld( monoworld ) ;
        }

        SetGWorld( wp , NULL ) ;
        Rect portRect ;
        GetPortBounds( wp , &portRect ) ;
        m_pictHandle = OpenPicture(&portRect);

        if (m_pictHandle)
        {
            RGBColor white = { 0xffff , 0xffff , 0xffff } ;
            RGBColor black = { 0x0000 , 0x0000 , 0x0000 } ;

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
#endif
    }

    return m_pictHandle ;
}

#ifdef __WXMAC_OSX__
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
            alphaInfo = kCGImageAlphaFirst ;
            membuf = new wxMemoryBuffer( imageSize ) ;
            memcpy( membuf->GetData() , dataBuffer , imageSize ) ;
            unsigned char *sourcemaskstart = (unsigned char *) m_bitmapMask->GetRawAccess() ;
            int maskrowbytes = m_bitmapMask->GetBytesPerRow() ;
            unsigned char *destalpha = (unsigned char *) membuf->GetData() ;
            for ( int y = 0 ; y < h ; ++y , sourcemaskstart += maskrowbytes)
            {
                unsigned char *sourcemask = sourcemaskstart ;
                for ( int x = 0 ; x < w ; ++x , sourcemask += 4 , destalpha += 4 )
                {
                    *destalpha = 0xFF - *sourcemask ;
                }
            }
        }
        else
        {
            if ( m_hasAlpha )
            {
#if wxMAC_USE_PREMULTIPLIED_ALPHA
                alphaInfo = kCGImageAlphaPremultipliedFirst ;
#else
                alphaInfo = kCGImageAlphaFirst ;
#endif
            }

            membuf = new wxMemoryBuffer( m_memBuf ) ;
        }
        
        CGDataProviderRef dataProvider = NULL ;
        if ( m_depth == 1 )
        {
            wxMemoryBuffer* maskBuf = new wxMemoryBuffer( m_width * m_height );
            unsigned char * maskBufData = (unsigned char *) maskBuf->GetData();
            unsigned char * bufData = (unsigned char *) membuf->GetData() ;
            // copy one color component
            for( int i = 0 ; i < m_width * m_height ; ++i )
                maskBufData[i] = bufData[i*4+3];
            dataProvider =
                CGDataProviderCreateWithData(
                    maskBuf , (const void *) maskBufData , m_width * m_height,
                    wxMacMemoryBufferReleaseProc );
            // as we are now passing the mask buffer to the data provider, we have
            // to release the membuf ourselves
            delete membuf ;

            image = ::CGImageMaskCreate( w, h, 8, 8, m_width , dataProvider, NULL, false );
        }
        else
        {
            CGColorSpaceRef colorSpace = wxMacGetGenericRGBColorSpace();
            dataProvider =
                CGDataProviderCreateWithData(
                    membuf , (const void *)membuf->GetData() , imageSize,
                    wxMacMemoryBufferReleaseProc );
            image =
            ::CGImageCreate(
                w, h, 8 , 32 , 4 * m_width , colorSpace, alphaInfo ,
                dataProvider, NULL , false , kCGRenderingIntentDefault );
        }
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
        {
            *mask = (GWorldPtr) m_bitmapMask->GetHBITMAP() ;
        }
        else if ( m_hasAlpha )
        {
#if !wxMAC_USE_CORE_GRAPHICS
            if ( m_rawAccessCount > 0 )
                UpdateAlphaMask() ;
#else
            // this structure is not kept in synch when using CG, so if something
            // is really accessing the GrafPorts, we have to sync it
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

            for ( int x = 0 ; x < w ; ++x , sourcemask += 4 )
            {
                // we must have 24 bit depth for non quartz smooth alpha
                *destalpha++ = 255 ;
                *destalpha++ = 255  - *sourcemask ;
                *destalpha++ = 255  - *sourcemask ;
                *destalpha++ = 255  - *sourcemask ;
            }
        }
    }
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( m_rawAccessCount == 0 , wxT("Bitmap still selected when destroyed") ) ;

#ifdef __WXMAC_OSX__
    if ( m_cgImageRef )
    {
        CGImageRelease( m_cgImageRef ) ;
        m_cgImageRef = NULL ;
    }
#endif

    if ( m_iconRef )
    {
        ReleaseIconRef( m_iconRef ) ;
        m_iconRef = NULL ;
    }

#ifndef __LP64__
    if ( m_pictHandle )
    {
        KillPicture( m_pictHandle ) ;
        m_pictHandle = NULL ;
    }

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
#endif
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
    bool created = false ;
    int w = icon.GetWidth() ;
    int h = icon.GetHeight() ;

    Create( icon.GetWidth() , icon.GetHeight() ) ;

    if ( w == h && ( w == 16 || w == 32 || w == 48 || w == 128 ) )
    {
        IconFamilyHandle iconFamily = NULL ;
        Handle imagehandle = NewHandle( 0 ) ;
        Handle maskhandle = NewHandle( 0 ) ;

        OSType maskType = 0;
        OSType dataType = 0;
        IconSelectorValue selector = 0 ;

        switch (w)
        {
            case 128:
                dataType = kThumbnail32BitData ;
                maskType = kThumbnail8BitMask ;
                selector = kSelectorAllAvailableData ;
                break;

            case 48:
                dataType = kHuge32BitData ;
                maskType = kHuge8BitMask ;
                selector = kSelectorHuge32Bit | kSelectorHuge8BitMask ;
                break;

            case 32:
                dataType = kLarge32BitData ;
                maskType = kLarge8BitMask ;
                selector = kSelectorLarge32Bit | kSelectorLarge8BitMask ;
                break;

            case 16:
                dataType = kSmall32BitData ;
                maskType = kSmall8BitMask ;
                selector = kSelectorSmall32Bit | kSelectorSmall8BitMask ;
                break;

            default:
                break;
        }

        OSStatus err = IconRefToIconFamily( MAC_WXHICON(icon.GetHICON()) , selector , &iconFamily ) ;

        err = GetIconFamilyData( iconFamily , dataType , imagehandle ) ;
        err = GetIconFamilyData( iconFamily , maskType , maskhandle ) ;
        size_t imagehandlesize = GetHandleSize( imagehandle ) ;
        size_t maskhandlesize = GetHandleSize( maskhandle ) ;

        if ( imagehandlesize != 0 && maskhandlesize != 0 )
        {
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
            created = true ;
        }

        DisposeHandle( (Handle) iconFamily ) ;
    }

    if ( !created )
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
        if ( the_width % (sizeof(unsigned char) * 8) )
            linesize += sizeof(unsigned char);

        unsigned char* linestart = (unsigned char*) bits ;
        unsigned char* destination = (unsigned char*) BeginRawAccess() ;

        for ( int y = 0 ; y < the_height ; ++y , linestart += linesize )
        {
            int index, bit, mask;

            for ( int x = 0 ; x < the_width ; ++x )
            {
                index = x / 8 ;
                bit = x % 8 ;
                mask = 1 << bit ;

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

wxBitmap::wxBitmap(const void* data, wxBitmapType type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);
}

wxBitmap::wxBitmap(const wxString& filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

void * wxBitmap::GetRawAccess() const
{
    wxCHECK_MSG( Ok() , NULL , wxT("invalid bitmap") ) ;

    return M_BITMAPDATA->GetRawAccess() ;
}

void * wxBitmap::BeginRawAccess()
{
    wxCHECK_MSG( Ok() , NULL , wxT("invalid bitmap") ) ;

    return M_BITMAPDATA->BeginRawAccess() ;
}

void wxBitmap::EndRawAccess()
{
    wxCHECK_RET( Ok() , wxT("invalid bitmap") ) ;

    M_BITMAPDATA->EndRawAccess() ;
}

#ifdef __WXMAC_OSX__
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
        unsigned char *sourcedata = (unsigned char*) GetRawAccess() ;
        unsigned char *destdata = (unsigned char*) ret.BeginRawAccess() ;
        wxASSERT( (sourcedata != NULL) && (destdata != NULL) ) ;

        int sourcelinesize = sourcewidth * 4 ;
        int destlinesize = destwidth * 4 ;
        unsigned char *source = sourcedata + rect.x * 4 + rect.y * sourcelinesize ;
        unsigned char *dest = destdata ;

        for (int yy = 0; yy < destheight; ++yy, source += sourcelinesize , dest += destlinesize)
        {
            memcpy( dest , source , destlinesize ) ;
        }
    }

    ret.EndRawAccess() ;

    if ( M_BITMAPDATA->m_bitmapMask )
    {
        wxMemoryBuffer maskbuf ;
        int rowBytes = ( destwidth * 4 + 3 ) & 0xFFFFFFC ;
        size_t maskbufsize = rowBytes * destheight ;

        int sourcelinesize = M_BITMAPDATA->m_bitmapMask->GetBytesPerRow() ;
        int destlinesize = rowBytes ;

        unsigned char *source = (unsigned char *) M_BITMAPDATA->m_bitmapMask->GetRawAccess() ;
        unsigned char *destdata = (unsigned char * ) maskbuf.GetWriteBuf( maskbufsize ) ;
        wxASSERT( (source != NULL) && (destdata != NULL) ) ;

        source += rect.x * 4 + rect.y * sourcelinesize ;
        unsigned char *dest = destdata ;

        for (int yy = 0; yy < destheight; ++yy, source += sourcelinesize , dest += destlinesize)
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
#if wxUSE_IMAGE
        wxImage loadimage(filename, type);
        if (loadimage.Ok())
        {
            *this = loadimage;

            return true;
        }
#endif
    }

    wxLogWarning(wxT("no bitmap handler for type %d defined."), type);

    return false;
}

bool wxBitmap::Create(const void* data, wxBitmapType type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL )
    {
        wxLogWarning(wxT("no bitmap handler for type %d defined."), type);

        return false;
    }

    return handler->Create(this, data, type, width, height, depth);
}

#if wxUSE_IMAGE

wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    wxCHECK_RET( image.Ok(), wxT("invalid image") );

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int height = image.GetHeight();

    m_refData = new wxBitmapRefData( width , height , depth ) ;

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
                *destination++ = ((*data++) * a + 127) / 255 ;
                *destination++ = ((*data++) * a + 127) / 255 ;
                *destination++ = ((*data++) * a + 127) / 255 ;
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
        SetMask( new wxMask( *this , wxColour( image.GetMaskRed() , image.GetMaskGreen() , image.GetMaskBlue() ) ) ) ;
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
    int maskBytesPerRow = 0 ;
    unsigned char *alpha = NULL ;
    unsigned char *mask = NULL ;

    if ( HasAlpha() )
        hasAlpha = true ;

    if ( GetMask() )
    {
        hasMask = true ;
        mask = (unsigned char*) GetMask()->GetRawAccess() ;
        maskBytesPerRow = GetMask()->GetBytesPerRow() ;
    }

    if ( hasAlpha )
    {
        image.SetAlpha() ;
        alpha = image.GetAlpha() ;
    }

    int index = 0;

    // The following masking algorithm is the same as well in msw/gtk:
    // the colour used as transparent one in wxImage and the one it is
    // replaced with when it actually occurs in the bitmap
    static const int MASK_RED = 1;
    static const int MASK_GREEN = 2;
    static const int MASK_BLUE = 3;
    static const int MASK_BLUE_REPLACEMENT = 2;

    for (int yy = 0; yy < height; yy++ , mask += maskBytesPerRow )
    {
        unsigned char * maskp = mask ;
        unsigned char a, r, g, b;
        long color;

        for (int xx = 0; xx < width; xx++)
        {
            color = *((long*) source) ;
#ifdef WORDS_BIGENDIAN
            a = ((color&0xFF000000) >> 24) ;
            r = ((color&0x00FF0000) >> 16) ;
            g = ((color&0x0000FF00) >> 8) ;
            b = (color&0x000000FF);
#else
            b = ((color&0xFF000000) >> 24) ;
            g = ((color&0x00FF0000) >> 16) ;
            r = ((color&0x0000FF00) >> 8) ;
            a = (color&0x000000FF);
#endif
            if ( hasMask )
            {
                if ( *maskp++ == 0xFF )
                {
                    r = MASK_RED ;
                    g = MASK_GREEN ;
                    b = MASK_BLUE ;
                }
                else if ( r == MASK_RED && g == MASK_GREEN && b == MASK_BLUE )
                    b = MASK_BLUE_REPLACEMENT ;

                maskp++ ;
                maskp++ ;
                maskp++ ;
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

#endif //wxUSE_IMAGE

bool wxBitmap::SaveFile( const wxString& filename,
    wxBitmapType type, const wxPalette *palette ) const
{
    bool success = false;
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler )
    {
        success = handler->SaveFile(this, filename, type, palette);
    }
    else
    {
#if wxUSE_IMAGE
        wxImage image = ConvertToImage();
        success = image.SaveFile(filename, type);
#else
        wxLogWarning(wxT("no bitmap handler for type %d defined."), type);
#endif
    }

    return success;
}

bool wxBitmap::IsOk() const
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

#if WXWIN_COMPATIBILITY_2_4
int wxBitmap::GetQuality() const
{
    return 0;
}

void wxBitmap::SetQuality(int WXUNUSED(quality))
{
}
#endif

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
wxMask::wxMask( const wxBitmap& bitmap, const wxColour& colour )
{
    Init() ;
    Create( bitmap, colour );
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask( const wxBitmap& bitmap )
{
    Init() ;
    Create( bitmap );
}

// Construct a mask from a mono bitmap (copies the bitmap).

wxMask::wxMask( const wxMemoryBuffer& data, int width , int height , int bytesPerRow )
{
    Init() ;
    Create( data, width , height , bytesPerRow );
}

wxMask::~wxMask()
{
#ifndef __LP64__
    if ( m_maskBitmap )
    {
        DisposeGWorld( (GWorldPtr)m_maskBitmap ) ;
        m_maskBitmap = NULL ;
    }
#endif
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

// The default ColorTable for k8IndexedGrayPixelFormat in Intel appears to be broken, so we'll use an non-indexed
// bitmap mask instead; in order to keep the code simple, the change applies to PowerPC implementations as well

void wxMask::RealizeNative()
{
#ifndef __LP64__
    if ( m_maskBitmap )
    {
       DisposeGWorld( (GWorldPtr)m_maskBitmap ) ;
       m_maskBitmap = NULL ;
    }

    Rect rect = { 0 , 0 , m_height , m_width } ;

    OSStatus err = NewGWorldFromPtr(
        (GWorldPtr*) &m_maskBitmap , k32ARGBPixelFormat , &rect , NULL , NULL , 0 ,
        (char*) m_memBuf.GetData() , m_bytesPerRow ) ;
    verify_noerr( err ) ;
#endif
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
    m_bytesPerRow = ( m_width * 4 + 3 ) & 0xFFFFFFC ;

    size_t size = m_bytesPerRow * m_height ;
    unsigned char * destdatabase = (unsigned char*) m_memBuf.GetWriteBuf( size ) ;
    wxASSERT( destdatabase != NULL ) ;

    memset( destdatabase , 0 , size ) ;
    unsigned char * srcdata = (unsigned char*) bitmap.GetRawAccess() ;

    for ( int y = 0 ; y < m_height ; ++y , destdatabase += m_bytesPerRow )
    {
        unsigned char *destdata = destdatabase ;
        unsigned char r, g, b;

        for ( int x = 0 ; x < m_width ; ++x )
        {
            srcdata++ ;
            r = *srcdata++ ;
            g = *srcdata++ ;
            b = *srcdata++ ;

            if ( ( r + g + b ) > 0x10 )
            {
                *destdata++ = 0xFF ;
                *destdata++ = 0xFF ;
                *destdata++ = 0xFF ;
                *destdata++ = 0xFF ;
            }
            else
            {
                *destdata++ = 0x00 ;
                *destdata++ = 0x00 ;
                *destdata++ = 0x00 ;
                *destdata++ = 0x00 ;
            }
        }
    }

    m_memBuf.UngetWriteBuf( size ) ;
    RealizeNative() ;

    return true;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    m_width = bitmap.GetWidth() ;
    m_height = bitmap.GetHeight() ;
    m_bytesPerRow = ( m_width * 4 + 3 ) & 0xFFFFFFC ;

    size_t size = m_bytesPerRow * m_height ;
    unsigned char * destdatabase = (unsigned char*) m_memBuf.GetWriteBuf( size ) ;
    wxASSERT( destdatabase != NULL ) ;

    memset( destdatabase , 0 , size ) ;
    unsigned char * srcdata = (unsigned char*) bitmap.GetRawAccess() ;

    for ( int y = 0 ; y < m_height ; ++y , destdatabase += m_bytesPerRow)
    {
        unsigned char *destdata = destdatabase ;
        unsigned char r, g, b;

        for ( int x = 0 ; x < m_width ; ++x )
        {
            srcdata++ ;
            r = *srcdata++ ;
            g = *srcdata++ ;
            b = *srcdata++ ;

            if ( colour == wxColour( r , g , b ) )
            {
                *destdata++ = 0xFF ;
                *destdata++ = 0xFF ;
                *destdata++ = 0xFF ;
                *destdata++ = 0xFF ;
            }
            else
            {
                *destdata++ = 0x00 ;
                *destdata++ = 0x00 ;
                *destdata++ = 0x00 ;
                *destdata++ = 0x00 ;
            }
        }
    }

    m_memBuf.UngetWriteBuf( size ) ;
    RealizeNative() ;

    return true;
}

WXHBITMAP wxMask::GetHBITMAP() const
{
    return m_maskBitmap ;
}

// ----------------------------------------------------------------------------
// wxBitmapHandler
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler, wxBitmapHandlerBase)

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


bool wxPICTResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight)
{
#if wxUSE_METAFILE
    Str255 theName ;
    wxMacStringToPascal( name , theName ) ;

    PicHandle thePict = (PicHandle ) GetNamedResource( 'PICT' , theName ) ;
    if ( thePict )
    {
        wxMetafile mf ;

        mf.SetHMETAFILE( (WXHMETAFILE) thePict ) ;
        bitmap->Create( mf.GetWidth() , mf.GetHeight() ) ;
        wxMemoryDC dc ;
        dc.SelectObject( *bitmap ) ;
        mf.Play( &dc ) ;
        dc.SelectObject( wxNullBitmap ) ;

        return true ;
    }
#endif

    return false ;
}

void wxBitmap::InitStandardHandlers()
{
    AddHandler( new wxPICTResourceHandler ) ;
    AddHandler( new wxICONResourceHandler ) ;
}

// ----------------------------------------------------------------------------
// raw bitmap access support
// ----------------------------------------------------------------------------

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if ( !Ok() )
        // no bitmap, no data (raw or otherwise)
        return NULL;

    data.m_width = GetWidth() ;
    data.m_height = GetHeight() ;
    data.m_stride = GetWidth() * 4 ;

    return BeginRawAccess() ;
}

void wxBitmap::UngetRawData(wxPixelDataBase& dataBase)
{
    EndRawAccess() ;
}

void wxBitmap::UseAlpha()
{
    // remember that we are using alpha channel:
    // we'll need to create a proper mask in UngetRawData()
    M_BITMAPDATA->UseAlpha( true );
}
