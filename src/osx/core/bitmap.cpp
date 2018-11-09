/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/bitmap.cpp
// Purpose:     wxBitmap
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
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

#include "wx/filename.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject);

#include "wx/osx/private.h"

CGColorSpaceRef wxMacGetGenericRGBColorSpace();
CGDataProviderRef wxMacCGDataProviderCreateWithMemoryBuffer( const wxMemoryBuffer& buf );

// Implementation Notes
// --------------------
//
// A bitmap can be represented using an NSImage or a CGBitmapContextRef
// If raw bitmap data needs to be accessed, then even the NSImage has to be
// rendered into a CGBitmapContextRef

class WXDLLEXPORT wxBitmapRefData: public wxGDIRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxIcon;
    friend class WXDLLIMPEXP_FWD_CORE wxCursor;
public:
    wxBitmapRefData(int width , int height , int depth, double logicalscale = 1.0);
    wxBitmapRefData(CGContextRef context);
    wxBitmapRefData(CGImageRef image, double scale);
    wxBitmapRefData(WXImage image);
    wxBitmapRefData();
    wxBitmapRefData(const wxBitmapRefData &tocopy);

    virtual ~wxBitmapRefData();

    bool IsOk() const;

    void Free();

    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;
    int GetBytesPerRow() const;
    bool HasAlpha() const;
    WXImage GetImage() const;
    double GetScaleFactor() const { return m_scaleFactor; }

    const void *GetRawAccess() const;
    void *GetRawAccess();
    void *BeginRawAccess();
    void EndRawAccess();

    void UseAlpha( bool useAlpha );

    bool IsTemplate() const { return m_isTemplate; }
    void SetTemplate(bool is) { m_isTemplate = is; }

public:
#if wxUSE_PALETTE
    wxPalette     m_bitmapPalette;
#endif // wxUSE_PALETTE

    wxMask *      m_bitmapMask; // Optional mask
    CGImageRef    CreateCGImage() const;

    // returns true if the bitmap has a size that
    // can be natively transferred into a true icon
    // if no is returned GetIconRef will still produce
    // an icon but it will be generated via a PICT and
    // rescaled to 16 x 16
    bool          HasNativeSize();

#if wxOSX_USE_ICONREF
    // caller should increase ref count if needed longer
    // than the bitmap exists
    IconRef       GetIconRef();
#endif

    CGContextRef  GetBitmapContext() const;

    void SetSelectedInto(wxDC *dc);
    wxDC *GetSelectedInto() const;

private :
    bool Create(int width , int height , int depth, double logicalscale);
    bool Create( CGImageRef image, double scale );
    bool Create( CGContextRef bitmapcontext);
    bool Create( WXImage image);
    void Init();

    void EnsureBitmapExists() const;

    void FreeDerivedRepresentations();

    WXImage    m_nsImage;
    int           m_rawAccessCount;
    mutable CGImageRef    m_cgImageRef;
    bool          m_isTemplate;

#if wxOSX_USE_ICONREF
    IconRef       m_iconRef;
#endif

    wxCFRef<CGContextRef>  m_hBitmap;
    double        m_scaleFactor;
    wxDC*         m_selectedInto;
};


#define wxOSX_USE_PREMULTIPLIED_ALPHA 1
static const int kBestByteAlignement = 16;
static const int kMaskBytesPerPixel = 1;

static size_t GetBestBytesPerRow( size_t rawBytes )
{
    return (((rawBytes)+kBestByteAlignement-1) & ~(kBestByteAlignement-1) );
}

void wxBitmapRefData::Init()
{
    m_nsImage = NULL;
    m_bitmapMask = NULL ;
    m_cgImageRef = NULL ;
    m_isTemplate = false;

#if wxOSX_USE_ICONREF
    m_iconRef = NULL ;
#endif
    m_hBitmap = NULL ;

    m_rawAccessCount = 0 ;
    m_scaleFactor = 1.0;
    m_selectedInto = NULL;
}

wxBitmapRefData::wxBitmapRefData(const wxBitmapRefData &tocopy) : wxGDIRefData()
{
    Init();
    Create(tocopy.GetWidth(), tocopy.GetHeight(), tocopy.GetDepth(), tocopy.GetScaleFactor());

    if (tocopy.m_bitmapMask)
        m_bitmapMask = new wxMask(*tocopy.m_bitmapMask);
    else if (tocopy.HasAlpha())
        UseAlpha(true);

    unsigned char* dest = (unsigned char*)GetRawAccess();
    unsigned char* source = (unsigned char*)tocopy.GetRawAccess();
    size_t numbytes = GetBytesPerRow() * GetHeight();
    memcpy( dest, source, numbytes );
}

wxBitmapRefData::wxBitmapRefData()
{
    Init() ;
}

wxBitmapRefData::wxBitmapRefData( int w , int h , int d , double logicalscale)
{
    Init() ;
    Create( w , h , d, logicalscale ) ;
}

wxBitmapRefData::wxBitmapRefData(CGContextRef context)
{
    Init();
    Create( context );
}

wxBitmapRefData::wxBitmapRefData(CGImageRef image, double scale)
{
    Init();
    Create( image, scale );
}

wxBitmapRefData::wxBitmapRefData(WXImage image)
{
    Init();
    Create( image );
}

void wxBitmapRefData::EnsureBitmapExists() const
{
    if ( ! m_hBitmap && m_nsImage)
    {
        wxBitmapRefData* t =  const_cast<wxBitmapRefData*>(this);
        t->m_hBitmap = wxOSXCreateBitmapContextFromImage(m_nsImage, &t->m_isTemplate);
    }
}


bool wxBitmapRefData::Create( WXImage image )
{
    m_nsImage = image;

    wxMacCocoaRetain(image);

    m_scaleFactor = wxOSXGetImageScaleFactor(image);

    return true;
}

// code from Technical Q&A QA1509

bool wxBitmapRefData::Create(CGImageRef image, double scale)
{
    if (image != NULL)
    {
        size_t m_width = CGImageGetWidth(image);
        size_t m_height = CGImageGetHeight(image);

        m_hBitmap = NULL;
        m_scaleFactor = scale;

        size_t m_bytesPerRow = GetBestBytesPerRow(m_width * 4);
        void* data = NULL;

        CGImageAlphaInfo alpha = CGImageGetAlphaInfo(image);
        if (alpha == kCGImageAlphaNone || alpha == kCGImageAlphaNoneSkipFirst || alpha == kCGImageAlphaNoneSkipLast)
        {
            m_hBitmap = CGBitmapContextCreate((char*)data, m_width, m_height, 8, m_bytesPerRow, wxMacGetGenericRGBColorSpace(), kCGImageAlphaNoneSkipFirst);
        }
        else
        {
            m_hBitmap = CGBitmapContextCreate((char*)data, m_width, m_height, 8, m_bytesPerRow, wxMacGetGenericRGBColorSpace(), kCGImageAlphaPremultipliedFirst);
        }
        CGRect rect = CGRectMake(0, 0, m_width, m_height);
        CGContextDrawImage(m_hBitmap, rect, image);

        wxASSERT_MSG(m_hBitmap, wxT("Unable to create CGBitmapContext context"));
        CGContextTranslateCTM(m_hBitmap, 0, m_height);
        CGContextScaleCTM(m_hBitmap, 1 * m_scaleFactor, -1 * m_scaleFactor);
    }
    return IsOk();
}

bool wxBitmapRefData::Create(CGContextRef context)
{
    if ( context != NULL && CGBitmapContextGetData(context) )
    {
        m_hBitmap = context;
        // our own contexts conform to this, always.
        wxASSERT( GetDepth() == 32 );

        // determine content scale
        CGRect userrect = CGRectMake(0, 0, 10, 10);
        CGRect devicerect;
        devicerect = CGContextConvertRectToDeviceSpace(context, userrect);
        m_scaleFactor = devicerect.size.height / userrect.size.height;
    }
    return IsOk() ;
}

bool wxBitmapRefData::Create(int w, int h, int WXUNUSED(d), double logicalscale)
{
    size_t m_width = wxMax(1, w);
    size_t m_height = wxMax(1, h);

    m_scaleFactor = logicalscale;
    m_hBitmap = NULL;

    size_t m_bytesPerRow = GetBestBytesPerRow(m_width * 4);
    void* data = NULL;
    m_hBitmap = CGBitmapContextCreate((char*)data, m_width, m_height, 8, m_bytesPerRow, wxMacGetGenericRGBColorSpace(), kCGImageAlphaNoneSkipFirst);
    wxASSERT_MSG(m_hBitmap, wxT("Unable to create CGBitmapContext context"));
    CGContextTranslateCTM(m_hBitmap, 0, m_height);
    CGContextScaleCTM(m_hBitmap, 1 * GetScaleFactor(), -1 * GetScaleFactor());

    return IsOk();
}

bool wxBitmapRefData::IsOk() const
{
    return (m_hBitmap.get() != NULL || m_nsImage != NULL);
}

int wxBitmapRefData::GetWidth() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
        return (int) CGBitmapContextGetWidth(m_hBitmap);
    else
        return (int) wxOSXGetImageSize(m_nsImage).width * m_scaleFactor;
}

int wxBitmapRefData::GetHeight() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
        return (int) CGBitmapContextGetHeight(m_hBitmap);
    else
        return (int) wxOSXGetImageSize(m_nsImage).height * m_scaleFactor;
}

int wxBitmapRefData::GetDepth() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
        return (int) CGBitmapContextGetBitsPerPixel(m_hBitmap);
    else
        return 32; // a bitmap converted from an nsimage would have this depth
}
int wxBitmapRefData::GetBytesPerRow() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
        return (int) CGBitmapContextGetBytesPerRow(m_hBitmap);
    else
        return (int) GetBestBytesPerRow( GetWidth() * 4);
}

bool wxBitmapRefData::HasAlpha() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
    {
        CGImageAlphaInfo alpha = CGBitmapContextGetAlphaInfo(m_hBitmap);
        return !(alpha == kCGImageAlphaNone || alpha == kCGImageAlphaNoneSkipFirst || alpha == kCGImageAlphaNoneSkipLast);
    }
    else
    {
        return true; // a bitmap converted from an nsimage would have alpha
    }
}

WXImage wxBitmapRefData::GetImage() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( !m_nsImage )
    {
        wxCFRef< CGImageRef > cgimage(CreateCGImage());
        return wxOSXGetImageFromCGImage( cgimage, GetScaleFactor(), IsTemplate() );
    }

    return m_nsImage;
}

void wxBitmapRefData::UseAlpha( bool use )
{
    wxCHECK_RET( IsOk() , wxT("invalid bitmap") ) ;

    if ( HasAlpha() == use )
        return ;

    CGContextRef hBitmap = CGBitmapContextCreate(NULL, GetWidth(), GetHeight(), 8, GetBytesPerRow(), wxMacGetGenericRGBColorSpace(), use ? kCGImageAlphaPremultipliedFirst : kCGImageAlphaNoneSkipFirst );

    memcpy(CGBitmapContextGetData(hBitmap),CGBitmapContextGetData(m_hBitmap),GetBytesPerRow()*GetHeight());

    wxASSERT_MSG( hBitmap , wxT("Unable to create CGBitmapContext context") ) ;
    CGContextTranslateCTM( hBitmap, 0,  GetHeight() );
    CGContextScaleCTM( hBitmap, GetScaleFactor(), -GetScaleFactor() );

    m_hBitmap.reset(hBitmap);
}

const void *wxBitmapRefData::GetRawAccess() const
{
    wxCHECK_MSG( IsOk(), NULL , wxT("invalid bitmap") ) ;

    EnsureBitmapExists();

    return CGBitmapContextGetData(m_hBitmap);
}

void *wxBitmapRefData::GetRawAccess()
{
    return const_cast<void*>(const_cast<const wxBitmapRefData*>(this)->GetRawAccess());
}


void *wxBitmapRefData::BeginRawAccess()
{
    wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") ) ;
    wxASSERT( m_rawAccessCount == 0 ) ;

#if wxOSX_USE_ICONREF
    wxASSERT_MSG( m_iconRef == NULL ,
                 wxT("Currently, modifing bitmaps that are used in controls already is not supported") ) ;
#endif

    ++m_rawAccessCount ;

    // we must destroy an existing cached image, as
    // the bitmap data may change now
    FreeDerivedRepresentations();

    return GetRawAccess() ;
}

void wxBitmapRefData::EndRawAccess()
{
    wxCHECK_RET( IsOk() , wxT("invalid bitmap") ) ;
    wxASSERT( m_rawAccessCount == 1 ) ;

    --m_rawAccessCount ;
}

bool wxBitmapRefData::HasNativeSize()
{
    int w = GetWidth() ;
    int h = GetHeight() ;
    int sz = wxMax( w , h ) ;

    return ( sz == 128 || sz == 48 || sz == 32 || sz == 16 );
}

#if wxOSX_USE_ICONREF
IconRef wxBitmapRefData::GetIconRef()
{
    if ( m_iconRef == NULL )
    {
        // Create Icon Family Handle

        IconFamilyHandle iconFamily = (IconFamilyHandle) NewHandle( 0 );

        int w = GetWidth() ;
        int h = GetHeight() ;
        int sz = wxMax( w , h ) ;

        OSType dataType = 0 ;
        OSType maskType = 0 ;

        // since we don't have PICT conversion available, use
        // the next larger standard icon size
        // TODO: Use NSImage
        if (sz <= 16)
            sz = 16;
        else if ( sz <= 32)
            sz = 32;
        else if ( sz <= 48)
            sz = 48;
        else if ( sz <= 128)
            sz = 128;
        else if ( sz <= 256)
            sz = 256;
        else if ( sz <= 512)
            sz = 512;
        else if ( sz <= 1024)
            sz = 1024;
        
        switch (sz)
        {
            case 1024:
                dataType = kIconServices1024PixelDataARGB;
                break;

            case 512:
                dataType = kIconServices512PixelDataARGB;
                break;
                
            case 256:
                dataType = kIconServices256PixelDataARGB;
                break;
 
            case 128:
                dataType = kIconServices128PixelDataARGB ;
                break;

            case 48:
                dataType = kIconServices48PixelDataARGB ;
                break;

            case 32:
                dataType = kIconServices32PixelDataARGB ;
                break;

            case 16:
                dataType = kIconServices16PixelDataARGB ;
                break;

            default:
                break;
        }

        if ( dataType != 0 )
        {
            if (  maskType == 0 )
            {
                size_t datasize = sz * sz * 4 ;
                Handle data = NewHandle( datasize ) ;
                HLock( data ) ;
                unsigned char* ptr = (unsigned char*) *data ;
                memset( ptr, 0, datasize );
                bool hasAlpha = HasAlpha() ;
                wxMask *mask = m_bitmapMask ;
                unsigned char * sourcePtr = (unsigned char*) GetRawAccess() ;
                unsigned char * masksourcePtr = mask ? (unsigned char*) mask->GetRawAccess() : NULL ;

                for ( int y = 0 ; y < h ; ++y, sourcePtr += GetBytesPerRow() , masksourcePtr += mask ? mask->GetBytesPerRow() : 0 )
                {
                    unsigned char * source = sourcePtr;
                    unsigned char * masksource = masksourcePtr;
                    unsigned char * dest = ptr + y * sz * 4 ;
                    unsigned char a, r, g, b;

                    for ( int x = 0 ; x < w ; ++x )
                    {
                        a = *source ++ ;
                        r = *source ++ ;
                        g = *source ++ ;
                        b = *source ++ ;

                        if ( mask )
                        {
                            a = 0xFF - *masksource++ ;
                        }
                        else if ( !hasAlpha )
                            a = 0xFF ;
                        else
                        {
#if wxOSX_USE_PREMULTIPLIED_ALPHA
                            // this must be non-premultiplied data
                            if ( a != 0xFF && a!= 0 )
                            {
                                r = r * 255 / a;
                                g = g * 255 / a;
                                b = b * 255 / a;
                            }
#endif
                        }
                        *dest++ = a ;
                        *dest++ = r ;
                        *dest++ = g ;
                        *dest++ = b ;

                     }
                }
                HUnlock( data );

                OSStatus err = SetIconFamilyData( iconFamily, dataType , data );
                if ( err != noErr )
                {
                    wxFAIL_MSG("Error when adding bitmap");
                }

                DisposeHandle( data );
            }
            else
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
                unsigned char * sourcePtr = (unsigned char*) GetRawAccess() ;
                unsigned char * masksourcePtr = mask ? (unsigned char*) mask->GetRawAccess() : NULL ;

                for ( int y = 0 ; y < h ; ++y, sourcePtr += GetBytesPerRow() , masksourcePtr += mask ? mask->GetBytesPerRow() : 0 )
                {
                    unsigned char * source = sourcePtr;
                    unsigned char * masksource = masksourcePtr;
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
                            *maskdest++ = 0xFF - *masksource++ ;
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
        }
        // transform into IconRef

        // cleaner version existing from 10.3 upwards
        HLock((Handle) iconFamily);
        OSStatus err = GetIconRefFromIconFamilyPtr( *iconFamily, GetHandleSize((Handle) iconFamily), &m_iconRef );
        HUnlock((Handle) iconFamily);
        DisposeHandle( (Handle) iconFamily ) ;

        wxCHECK_MSG( err == noErr, NULL, wxT("Error when constructing icon ref") );
    }

    return m_iconRef ;
}
#endif // wxOSX_USE_ICONREF

CGImageRef wxBitmapRefData::CreateCGImage() const
{
    wxASSERT( IsOk() ) ;
    wxASSERT( m_rawAccessCount >= 0 ) ;
    CGImageRef image ;
    if ( m_rawAccessCount > 0 || m_cgImageRef == NULL )
    {
        if (m_nsImage)
        {
            image = wxOSXCreateCGImageFromImage(m_nsImage);
        }
        else
        {
            if (GetDepth() != 1 && m_bitmapMask == NULL)
            {
#if 0
            // in order for this code to work properly, wxMask would have to invert black and white
            // in the native bitmap
            if ( m_bitmapMask )
            {
                CGImageRef tempImage = CGBitmapContextCreateImage( m_hBitmap );
                CGImageRef tempMask = CGBitmapContextCreateImage((CGContextRef) m_bitmapMask->GetHBITMAP() );
                image = CGImageCreateWithMask( tempImage, tempMask );
                CGImageRelease(tempMask);
                CGImageRelease(tempImage);
            }
            else
#endif
                image = CGBitmapContextCreateImage(m_hBitmap);
            }
            else
            {
                size_t imageSize = GetHeight() * GetBytesPerRow();

                int w = GetWidth();
                int h = GetHeight();
                CGImageAlphaInfo alphaInfo = kCGImageAlphaNoneSkipFirst;
                wxMemoryBuffer membuf;

                if (m_bitmapMask)
                {
                    alphaInfo = kCGImageAlphaFirst;
                    unsigned char* destalphastart = (unsigned char*)membuf.GetWriteBuf(imageSize);
                    memcpy(destalphastart, GetRawAccess(), imageSize);
                    unsigned char* sourcemaskstart = (unsigned char*)m_bitmapMask->GetRawAccess();
                    int maskrowbytes = m_bitmapMask->GetBytesPerRow();
                    for (int y = 0; y < h; ++y, destalphastart += GetBytesPerRow(), sourcemaskstart += maskrowbytes)
                    {
                        unsigned char* sourcemask = sourcemaskstart;
                        unsigned char* destalpha = destalphastart;
                        for (int x = 0; x < w; ++x, sourcemask += kMaskBytesPerPixel, destalpha += 4)
                        {
                            *destalpha = 0xFF - *sourcemask;
                        }
                    }
                    membuf.UngetWriteBuf(imageSize);
                }
                else
                {
                    if (HasAlpha())
                    {
#if wxOSX_USE_PREMULTIPLIED_ALPHA
                        alphaInfo = kCGImageAlphaPremultipliedFirst;
#else
                        alphaInfo = kCGImageAlphaFirst;
#endif
                    }
                    memcpy(membuf.GetWriteBuf(imageSize), GetRawAccess(), imageSize);
                    membuf.UngetWriteBuf(imageSize);
                }

                CGDataProviderRef dataProvider = NULL;
                if (GetDepth() == 1)
                {
                    // TODO CHECK ALIGNMENT
                    wxMemoryBuffer maskBuf;
                    unsigned char* maskBufData = (unsigned char*)maskBuf.GetWriteBuf(GetWidth() * GetHeight());
                    unsigned char* bufData = (unsigned char*)membuf.GetData();
                    // copy one color component
                    size_t i = 0;
                    for (int y = 0; y < GetHeight(); bufData += GetBytesPerRow(), ++y)
                    {
                        unsigned char* bufDataIter = bufData + 3;
                        for (int x = 0; x < GetWidth(); bufDataIter += 4, ++x, ++i)
                        {
                            maskBufData[i] = *bufDataIter;
                        }
                    }
                    maskBuf.UngetWriteBuf(GetWidth() * GetHeight());

                    dataProvider = wxMacCGDataProviderCreateWithMemoryBuffer(maskBuf);

                    image = ::CGImageMaskCreate(w, h, 8, 8, GetWidth(), dataProvider, NULL, false);
                }
                else
                {
                    CGColorSpaceRef colorSpace = wxMacGetGenericRGBColorSpace();
                    dataProvider = wxMacCGDataProviderCreateWithMemoryBuffer(membuf);
                    image = ::CGImageCreate(
                        w, h, 8, 32, GetBytesPerRow(), colorSpace, alphaInfo,
                        dataProvider, NULL, false, kCGRenderingIntentDefault);
                }
                CGDataProviderRelease(dataProvider);
            }
        }
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

CGContextRef wxBitmapRefData::GetBitmapContext() const
{
    return m_hBitmap;
}

void wxBitmapRefData::SetSelectedInto(wxDC *dc)
{
    if ( dc == NULL )
    {
        if ( m_selectedInto != NULL )
            EndRawAccess();
    }
    else
    {
        wxASSERT_MSG( m_selectedInto == NULL || m_selectedInto == dc, "Bitmap already selected into a different dc");

        if ( m_selectedInto == NULL )
            (void) BeginRawAccess();
    }

    m_selectedInto = dc;
}

wxDC *wxBitmapRefData::GetSelectedInto() const
{
    return m_selectedInto;
}

void wxBitmapRefData::FreeDerivedRepresentations()
{
    if ( m_cgImageRef )
    {
        CGImageRelease( m_cgImageRef ) ;
        m_cgImageRef = NULL ;
    }
#if wxOSX_USE_ICONREF
    if ( m_iconRef )
    {
        ReleaseIconRef( m_iconRef ) ;
        m_iconRef = NULL ;
    }
#endif // wxOSX_USE_ICONREF
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( m_rawAccessCount == 0 , wxT("Bitmap still selected when destroyed") ) ;

    FreeDerivedRepresentations();

    wxMacCocoaRelease(m_nsImage);

    m_hBitmap.reset();
    wxDELETE(m_bitmapMask);
}

wxBitmapRefData::~wxBitmapRefData()
{
    Free() ;
}



// ----------------------------------------------------------------------------
// wxBitmap
// ----------------------------------------------------------------------------

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    return Create( icon.GetImage() );
}

wxBitmap::wxBitmap(const char bits[], int the_width, int the_height, int no_bits)
{
    m_refData = new wxBitmapRefData( the_width , the_height , no_bits ) ;

    if (GetBitmapData()->IsOk())
    {
        if ( no_bits == 1 )
        {
            int linesize = the_width / 8;
            if ( the_width % 8 )
                linesize++;

            unsigned char* linestart = (unsigned char*) bits ;
            unsigned char* destptr = (unsigned char*) GetBitmapData()->BeginRawAccess() ;

            for ( int y = 0 ; y < the_height ; ++y , linestart += linesize, destptr += GetBitmapData()->GetBytesPerRow() )
            {
                unsigned char* destination = destptr;
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

            GetBitmapData()->EndRawAccess() ;
        }
        else
        {
            wxFAIL_MSG(wxT("multicolor BITMAPs not yet implemented"));
        }
    }
}

wxBitmap::wxBitmap(const void* data, wxBitmapType type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);
}

wxBitmap::wxBitmap(int width, int height, const wxDC& dc)
{
    (void)Create(width, height, dc);
}

wxBitmap::wxBitmap(const wxString& filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(CGImageRef image, double scale)
{
    (void) Create(image,scale);
}

wxGDIRefData* wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData;
}

wxGDIRefData* wxBitmap::CloneGDIRefData(const wxGDIRefData* data) const
{
    return new wxBitmapRefData(*static_cast<const wxBitmapRefData *>(data));
}

#if WXWIN_COMPATIBILITY_3_0
const void * wxBitmap::GetRawAccess() const
{
    wxCHECK_MSG( IsOk() , NULL , wxT("invalid bitmap") ) ;

    return GetBitmapData()->GetRawAccess() ;
}

void * wxBitmap::BeginRawAccess()
{
    wxCHECK_MSG( IsOk() , NULL , wxT("invalid bitmap") ) ;

    return GetBitmapData()->BeginRawAccess() ;
}

void wxBitmap::EndRawAccess()
{
    wxCHECK_RET( IsOk() , wxT("invalid bitmap") ) ;

    GetBitmapData()->EndRawAccess() ;
}
#endif

CGImageRef wxBitmap::CreateCGImage() const
{
    wxCHECK_MSG( IsOk(), NULL , wxT("invalid bitmap") ) ;

    return GetBitmapData()->CreateCGImage() ;
}

#if wxOSX_USE_ICONREF

IconRef wxBitmap::GetIconRef() const
{
    wxCHECK_MSG( IsOk(), NULL , wxT("invalid bitmap") ) ;

    return GetBitmapData()->GetIconRef() ;
}

IconRef wxBitmap::CreateIconRef() const
{
    IconRef icon = GetIconRef();
    __Verify_noErr(AcquireIconRef(icon));
    return icon;
}
#endif

wxBitmap::wxBitmap(WXImage image)
{
    (void)Create(image);
}

bool wxBitmap::Create(WXImage image)
{
    UnRef();

    m_refData = new wxBitmapRefData( image );

    return GetBitmapData()->IsOk() ;
}

wxBitmap::wxBitmap(CGContextRef bitmapcontext)
{
    (void)Create(bitmapcontext);
}

bool wxBitmap::Create(CGContextRef bitmapcontext)
{
    UnRef();

    m_refData = new wxBitmapRefData( bitmapcontext );

    return GetBitmapData()->IsOk() ;
}

WXImage wxBitmap::GetImage() const
{
    return GetBitmapData()->GetImage();
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect &rect) const
{
    wxCHECK_MSG( IsOk() &&
                (rect.x >= 0) && (rect.y >= 0) &&
                (rect.x+rect.width <= GetWidth()) &&
                (rect.y+rect.height <= GetHeight()),
                wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret;
    double scale = GetScaleFactor();
    ret.CreateScaled( rect.width, rect.height, GetDepth(), scale );
    wxASSERT_MSG( ret.IsOk(), wxT("GetSubBitmap error") );

    int destwidth = rect.width*scale ;
    int destheight = rect.height*scale ;

    {
        unsigned char *sourcedata = (unsigned char*) GetBitmapData()->GetRawAccess() ;
        unsigned char *destdata = (unsigned char*) ret.GetBitmapData()->BeginRawAccess() ;
        wxASSERT((sourcedata != NULL) && (destdata != NULL));

        if ( (sourcedata != NULL) && (destdata != NULL) )
        {
            int sourcelinesize = GetBitmapData()->GetBytesPerRow() ;
            int destlinesize = ret.GetBitmapData()->GetBytesPerRow() ;
            unsigned char *source = sourcedata + int(rect.x * scale * 4 + rect.y *scale * sourcelinesize) ;
            unsigned char *dest = destdata ;

            for (int yy = 0; yy < destheight; ++yy, source += sourcelinesize , dest += destlinesize)
            {
                memcpy( dest , source , destlinesize ) ;
            }
        }
        ret.GetBitmapData()->EndRawAccess() ;
    }

    if ( GetBitmapData()->m_bitmapMask )
    {
        wxMemoryBuffer maskbuf ;
        int rowBytes = GetBestBytesPerRow( destwidth * kMaskBytesPerPixel );
        size_t maskbufsize = rowBytes * destheight ;

        int sourcelinesize = GetBitmapData()->m_bitmapMask->GetBytesPerRow() ;
        int destlinesize = rowBytes ;

        unsigned char *source = (unsigned char *) GetBitmapData()->m_bitmapMask->GetRawAccess() ;
        unsigned char *destdata = (unsigned char * ) maskbuf.GetWriteBuf( maskbufsize ) ;
        wxASSERT( (source != NULL) && (destdata != NULL) ) ;

        if ( (source != NULL) && (destdata != NULL) )
        {
            source += rect.x * kMaskBytesPerPixel + rect.y * sourcelinesize ;
            unsigned char *dest = destdata ;

            for (int yy = 0; yy < destheight; ++yy, source += sourcelinesize , dest += destlinesize)
            {
                memcpy( dest , source , destlinesize ) ;
            }

            maskbuf.UngetWriteBuf( maskbufsize ) ;
        }
        wxMask* const mask = new wxMask();
        mask->OSXCreate( maskbuf , destwidth , destheight , rowBytes );
        ret.SetMask(mask) ;
    }
    else if ( HasAlpha() )
        ret.UseAlpha() ;

    return ret;
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    wxCHECK_MSG(w > 0 && h > 0, false, "invalid bitmap size");

    if ( d < 0 )
        d = wxDisplayDepth() ;

    m_refData = new wxBitmapRefData( w , h , d );

    return GetBitmapData()->IsOk() ;
}

bool wxBitmap::Create(int w, int h, const wxDC& dc)
{
    double factor = dc.GetContentScaleFactor();
    return CreateScaled(w,h,wxBITMAP_SCREEN_DEPTH, factor);
}

bool wxBitmap::CreateScaled(int w, int h, int d, double logicalScale)
{
    UnRef();

    if ( d < 0 )
        d = wxDisplayDepth() ;

    m_refData = new wxBitmapRefData( w*logicalScale , h*logicalScale , d, logicalScale );

    return GetBitmapData()->IsOk() ;
}

bool wxBitmap::Create(CGImageRef image, double scale)
{
    UnRef();

    m_refData = new wxBitmapRefData( image, scale );

    return GetBitmapData()->IsOk() ;
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
        double scale = 1.0;
        wxString fname = filename;

        if  ( type == wxBITMAP_TYPE_PNG )
        {
            if ( wxOSXGetMainScreenContentScaleFactor() > 1.9 )
            {
                wxFileName fn(filename);
                fn.MakeAbsolute();
                fn.SetName(fn.GetName()+"@2x");

                if ( fn.Exists() )
                {
                    fname = fn.GetFullPath();
                    scale = 2.0;
                }
            }
        }

        wxImage loadimage(fname, type);
        if (loadimage.IsOk())
        {
            *this = wxBitmap(loadimage,-1,scale);

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

wxBitmap::wxBitmap(const wxImage& image, int depth, double scale)
{
    wxCHECK_RET( image.IsOk(), wxT("invalid image") );

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int height = image.GetHeight();
    // we always use 32 bit internally here
    depth = 32;

    wxBitmapRefData* bitmapRefData;

    m_refData = bitmapRefData = new wxBitmapRefData( width, height, depth, scale) ;

    if ( bitmapRefData->IsOk())
    {
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

        unsigned char* destinationstart = (unsigned char*) GetBitmapData()->BeginRawAccess() ;
        unsigned char* data = image.GetData();
        if ( destinationstart != NULL && data != NULL )
        {
            const unsigned char *alpha = hasAlpha ? image.GetAlpha() : NULL ;
            for (int y = 0; y < height; destinationstart += GetBitmapData()->GetBytesPerRow(), y++)
            {
                unsigned char * destination = destinationstart;
                for (int x = 0; x < width; x++)
                {
                    if ( hasAlpha )
                    {
                        const unsigned char a = *alpha++;
                        *destination++ = a ;

    #if wxOSX_USE_PREMULTIPLIED_ALPHA
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

            GetBitmapData()->EndRawAccess() ;
        }
        if ( image.HasMask() )
            SetMask( new wxMask( *this , wxColour( image.GetMaskRed() , image.GetMaskGreen() , image.GetMaskBlue() ) ) ) ;
    }
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;

    wxCHECK_MSG( IsOk(), wxNullImage, wxT("invalid bitmap") );

    // this call may trigger a conversion from platform image to bitmap, issue it
    // before any measurements are taken, multi-resolution platform images may be
    // rendered incorrectly otherwise
    unsigned char* sourcestart = (unsigned char*) GetBitmapData()->GetRawAccess() ;

    // create an wxImage object
    int width = GetWidth();
    int height = GetHeight();
    image.Create( width, height );

    unsigned char *data = image.GetData();
    wxCHECK_MSG( data, wxNullImage, wxT("Could not allocate data for image") );

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

    for (int yy = 0; yy < height; yy++ , sourcestart += GetBitmapData()->GetBytesPerRow() , mask += maskBytesPerRow )
    {
        unsigned char * maskp = mask ;
        const wxUint32 * source = (wxUint32*)sourcestart;
        unsigned char a, r, g, b;

        for (int xx = 0; xx < width; xx++)
        {
            const wxUint32 color = *source++;
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
            }
            else if ( hasAlpha )
            {
                *alpha++ = a ;
#if wxOSX_USE_PREMULTIPLIED_ALPHA
                // this must be non-premultiplied data
                if ( a != 0xFF && a!= 0 )
                {
                    r = r * 255 / a;
                    g = g * 255 / a;
                    b = b * 255 / a;
                }
#endif
            }

            data[index    ] = r ;
            data[index + 1] = g ;
            data[index + 2] = b ;

            index += 3;
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

int wxBitmap::GetHeight() const
{
   wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

   return GetBitmapData()->GetHeight();
}

int wxBitmap::GetWidth() const
{
   wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

   return GetBitmapData()->GetWidth() ;
}

double wxBitmap::GetScaleFactor() const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

    return GetBitmapData()->GetScaleFactor() ;
}

int wxBitmap::GetDepth() const
{
   wxCHECK_MSG( IsOk(), -1, wxT("invalid bitmap") );

   return GetBitmapData()->GetDepth();
}

wxMask *wxBitmap::GetMask() const
{
   wxCHECK_MSG( IsOk(), NULL, wxT("invalid bitmap") );

   return GetBitmapData()->m_bitmapMask;
}

bool wxBitmap::HasAlpha() const
{
   wxCHECK_MSG( IsOk(), false , wxT("invalid bitmap") );

   return GetBitmapData()->HasAlpha() ;
}

#if WXWIN_COMPATIBILITY_3_0
void wxBitmap::SetWidth(int w)
{
    AllocExclusive();
    wxASSERT_MSG( GetWidth() == w, "Changing the bitmap width is not supported");
}

void wxBitmap::SetHeight(int h)
{
    AllocExclusive();
    wxASSERT_MSG( GetHeight() == h, "Changing the bitmap height is not supported");
}

void wxBitmap::SetDepth(int d)
{
    AllocExclusive();
    wxASSERT_MSG( d == -1 || GetDepth() == d, "Changing the bitmap depth is not supported");
}
#endif

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
   wxCHECK_MSG( IsOk(), NULL, wxT("Invalid bitmap  GetPalette()") );

   return & const_cast<wxBitmapRefData*>(GetBitmapData())->m_bitmapPalette;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    AllocExclusive();
    GetBitmapData()->m_bitmapPalette = palette ;
}
#endif // wxUSE_PALETTE

void wxBitmap::SetMask(wxMask *mask)
{
    AllocExclusive();
    // Remove existing mask if there is one.
    delete GetBitmapData()->m_bitmapMask;

    GetBitmapData()->m_bitmapMask = mask ;
}

WXHBITMAP wxBitmap::GetHBITMAP(WXHBITMAP* mask) const
{
    wxUnusedVar(mask);

    return WXHBITMAP(GetBitmapData()->GetBitmapContext());
}

// ----------------------------------------------------------------------------
// wxMask
// ----------------------------------------------------------------------------

wxMask::wxMask()
{
    Init() ;
}

wxMask::wxMask(const wxMask &tocopy) : wxMaskBase()
{
    Init();

    DoCreateMaskBitmap( tocopy.GetWidth(), tocopy.GetHeight(), tocopy.GetBytesPerRow());
    int size = tocopy.GetHeight() * tocopy.GetBytesPerRow();
    memcpy( GetRawAccess(), tocopy.GetRawAccess(), size);
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

wxMask::~wxMask()
{
}

void wxMask::Init()
{
}

void *wxMask::GetRawAccess() const
{
    wxCHECK_MSG( m_maskBitmap, NULL , wxT("invalid mask") ) ;
    return CGBitmapContextGetData(m_maskBitmap);
}

int wxMask::GetBytesPerRow() const
{
    return CGBitmapContextGetBytesPerRow(m_maskBitmap);
}

int wxMask::GetWidth() const
{
    return CGBitmapContextGetWidth(m_maskBitmap);
}

int wxMask::GetHeight() const
{
    return CGBitmapContextGetHeight(m_maskBitmap);
}


// The default ColorTable for k8IndexedGrayPixelFormat in Intel appears to be broken, so we'll use an non-indexed
// bitmap mask instead; in order to keep the code simple, the change applies to PowerPC implementations as well

void wxMask::DoCreateMaskBitmap(int width, int height, int bytesPerRow)
{
    wxCFRef<CGColorSpaceRef> colorspace = CGColorSpaceCreateDeviceGray();

   if ( bytesPerRow < 0 )
        bytesPerRow = GetBestBytesPerRow(width * kMaskBytesPerPixel);

    m_maskBitmap = CGBitmapContextCreate(NULL, width, height, kMaskBytesPerPixel * 8, bytesPerRow, colorspace,
        kCGImageAlphaNone);
    wxASSERT_MSG(m_maskBitmap, wxT("Unable to create CGBitmapContext context"));
}

void wxMask::RealizeNative()
{
#if 0
    if ( m_maskBitmap )
    {
        CGContextRelease( (CGContextRef) m_maskBitmap );
       m_maskBitmap = NULL ;
    }

    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceGray();
    // from MouseTracking sample :
    // Ironically, due to a bug in CGImageCreateWithMask, you cannot use
    // CGColorSpaceCreateWithName(kCGColorSpaceGenericGray) at this point!

    m_maskBitmap = CGBitmapContextCreate((char*) m_memBuf.GetData(), m_width, m_height, 8, m_bytesPerRow, colorspace,
        kCGImageAlphaNone );
    CGColorSpaceRelease( colorspace );
    wxASSERT_MSG( m_maskBitmap , wxT("Unable to create CGBitmapContext context") ) ;
#endif
}

// Create a mask from a mono bitmap (copies the bitmap).

bool wxMask::OSXCreate(const wxMemoryBuffer& data,int width , int height , int bytesPerRow)
{
    wxASSERT( data.GetDataLen() == (size_t)(height * bytesPerRow) ) ;

    DoCreateMaskBitmap(width, height, bytesPerRow);

    RealizeNative() ;

    return true ;
}

bool wxMask::InitFromMonoBitmap(const wxBitmap& bitmap)
{
    int m_width, m_height, m_bytesPerRow;
    m_width = bitmap.GetWidth() ;
    m_height = bitmap.GetHeight() ;

    DoCreateMaskBitmap(m_width, m_height);
    m_bytesPerRow = GetBytesPerRow();

    // pixel access needs a non-const bitmap currently
    wxBitmap bmp(bitmap);
    wxNativePixelData data(bmp);

    wxNativePixelData::Iterator p(data);

    unsigned char * destdatabase = (unsigned char*) GetRawAccess();
    wxASSERT( destdatabase != NULL ) ;
    if ( destdatabase != NULL)
    {
        for ( int y = 0 ; y < m_height; ++y,  destdatabase += m_bytesPerRow )
        {
            wxNativePixelData::Iterator rowStart = p;
            unsigned char *destdata = destdatabase ;
            for ( int x = 0 ; x < m_width ; ++x, ++p )
            {
                if ( ( p.Red() + p.Green() + p.Blue() ) > 0x10 )
                    *destdata++ = 0xFF ;
                else
                    *destdata++ = 0x00 ;
            }
            p = rowStart;
            p.OffsetY(data, 1);
        }
    }

    return true;
}

bool wxMask::InitFromColour(const wxBitmap& bitmap, const wxColour& colour)
{
    int m_width, m_height, m_bytesPerRow;

    m_width = bitmap.GetWidth() ;
    m_height = bitmap.GetHeight() ;

    DoCreateMaskBitmap(m_width, m_height);
    m_bytesPerRow = GetBytesPerRow();

    // pixel access needs a non-const bitmap currently
    wxBitmap bmp(bitmap);
    wxNativePixelData data(bmp);

    wxNativePixelData::Iterator p(data);

    unsigned char * destdatabase = (unsigned char*) GetRawAccess();
    wxASSERT( destdatabase != NULL ) ;
    if ( destdatabase != NULL)
    {
        for ( int y = 0 ; y < m_height; ++y,  destdatabase += m_bytesPerRow )
        {
            wxNativePixelData::Iterator rowStart = p;
            unsigned char *destdata = destdatabase ;
            for ( int x = 0 ; x < m_width ; ++x, ++p )
            {
                if ( wxColour( p.Red(), p.Green(), p.Blue() ) == colour )
                    *destdata++ = 0xFF ;
                else
                    *destdata++ = 0x00 ;
           }
            p = rowStart;
            p.OffsetY(data, 1);
        }
    }

    RealizeNative() ;

    return true;
}

wxBitmap wxMask::GetBitmap() const
{
    int m_width, m_height, m_bytesPerRow;
    m_width = GetWidth();
    m_height = GetHeight();
    m_bytesPerRow = GetBytesPerRow();


    wxBitmap bitmap(m_width, m_height, 32);
    wxNativePixelData data(bitmap);

    wxNativePixelData::Iterator p(data);

    const unsigned char* srcbase = static_cast<unsigned char*>(GetRawAccess());

    for (int y = 0; y < m_height; ++y, srcbase += m_bytesPerRow)
    {
        wxNativePixelData::Iterator rowStart = p;
        const unsigned char* src = srcbase;
        for (int x = 0; x < m_width; ++x, ++p, ++src)
        {
            const unsigned char byte = *src;
            p.Alpha() = 0xff;
            p.Red() = byte;
            p.Green() = byte;
            p.Blue() = byte;
        }
        p = rowStart;
        p.OffsetY(data, 1);
    }

    return bitmap;
}

WXHBITMAP wxMask::GetHBITMAP() const
{
    return m_maskBitmap ;
}

void wxMask::FreeData()
{
    return m_maskBitmap.reset();
}


// ----------------------------------------------------------------------------
// Standard Handlers
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBundleResourceHandler: public wxBitmapHandler
{
    wxDECLARE_ABSTRACT_CLASS(wxBundleResourceHandler);

public:
    inline wxBundleResourceHandler()
    {
    }

    virtual bool LoadFile(wxBitmap *bitmap,
                          const wxString& name,
                          wxBitmapType type,
                          int desiredWidth,
                          int desiredHeight) wxOVERRIDE;
};

wxIMPLEMENT_ABSTRACT_CLASS(wxBundleResourceHandler, wxBitmapHandler);

class WXDLLEXPORT wxPNGResourceHandler: public wxBundleResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxPNGResourceHandler);

public:
    inline wxPNGResourceHandler()
    {
        SetName(wxT("PNG resource"));
        SetExtension("PNG");
        SetType(wxBITMAP_TYPE_PNG_RESOURCE);
    }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxPNGResourceHandler, wxBundleResourceHandler);

class WXDLLEXPORT wxJPEGResourceHandler: public wxBundleResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxJPEGResourceHandler);

public:
    inline wxJPEGResourceHandler()
    {
        SetName(wxT("JPEG resource"));
        SetExtension("JPEG");
        SetType(wxBITMAP_TYPE_JPEG_RESOURCE);
    }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxJPEGResourceHandler, wxBundleResourceHandler);

#if wxOSX_USE_COCOA

class WXDLLEXPORT wxICNSResourceHandler: public wxBundleResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxICNSResourceHandler);

public:
    inline wxICNSResourceHandler()
    {
        SetName(wxT("icns resource"));
        SetExtension("icns");
        SetType(wxBITMAP_TYPE_ICON_RESOURCE);
    }

    virtual bool LoadFile(wxBitmap *bitmap,
                          const wxString& name,
                          wxBitmapType type,
                          int desiredWidth,
                          int desiredHeight) wxOVERRIDE;

};

wxIMPLEMENT_DYNAMIC_CLASS(wxICNSResourceHandler, wxBundleResourceHandler);

bool wxICNSResourceHandler::LoadFile(wxBitmap *bitmap,
                                       const wxString& resourceName,
                                       wxBitmapType type,
                                       int desiredWidth,
                                       int desiredHeight)
{
    OSType theId = 0 ;

    if ( resourceName == wxT("wxICON_INFORMATION") )
    {
        theId = kAlertNoteIcon ;
    }
    else if ( resourceName == wxT("wxICON_QUESTION") )
    {
        theId = kAlertCautionIcon ;
    }
    else if ( resourceName == wxT("wxICON_WARNING") )
    {
        theId = kAlertCautionIcon ;
    }
    else if ( resourceName == wxT("wxICON_ERROR") )
    {
        theId = kAlertStopIcon ;
    }
    else if ( resourceName == wxT("wxICON_FOLDER") )
    {
        theId = kGenericFolderIcon ;
    }
    else if ( resourceName == wxT("wxICON_FOLDER_OPEN") )
    {
        theId = kOpenFolderIcon ;
    }
    else if ( resourceName == wxT("wxICON_NORMAL_FILE") )
    {
        theId = kGenericDocumentIcon ;
    }
    else if ( resourceName == wxT("wxICON_EXECUTABLE_FILE") )
    {
        theId = kGenericApplicationIcon ;
    }
    else if ( resourceName == wxT("wxICON_CDROM") )
    {
        theId = kGenericCDROMIcon ;
    }
    else if ( resourceName == wxT("wxICON_FLOPPY") )
    {
        theId = kGenericFloppyIcon ;
    }
    else if ( resourceName == wxT("wxICON_HARDDISK") )
    {
        theId = kGenericHardDiskIcon ;
    }
    else if ( resourceName == wxT("wxICON_REMOVABLE") )
    {
        theId = kGenericRemovableMediaIcon ;
    }
    else if ( resourceName == wxT("wxICON_DELETE") )
    {
        theId = kToolbarDeleteIcon ;
    }
    else if ( resourceName == wxT("wxICON_GO_BACK") )
    {
        theId = kBackwardArrowIcon ;
    }
    else if ( resourceName == wxT("wxICON_GO_FORWARD") )
    {
        theId = kForwardArrowIcon ;
    }
    else if ( resourceName == wxT("wxICON_GO_HOME") )
    {
        theId = kToolbarHomeIcon ;
    }
    else if ( resourceName == wxT("wxICON_HELP_SETTINGS") )
    {
        theId = kGenericFontIcon ;
    }
    else if ( resourceName == wxT("wxICON_HELP_PAGE") )
    {
        theId = kGenericDocumentIcon ;
    }
    else if ( resourceName == wxT( "wxICON_PRINT" ) )
    {
        theId = kPrintMonitorFolderIcon;
    }
    else if ( resourceName == wxT( "wxICON_HELP_FOLDER" ) )
    {
        theId = kHelpFolderIcon;
    }

    if ( theId != 0 )
    {
        IconRef iconRef = NULL ;
        __Verify_noErr(GetIconRef( kOnSystemDisk, kSystemIconsCreator, theId, &iconRef )) ;
        if ( iconRef )
        {
            WXImage img = wxOSXGetNSImageFromIconRef(iconRef);
            bitmap->Create(img);
            return true;
        }
    }

    return wxBundleResourceHandler::LoadFile( bitmap, resourceName, type, desiredWidth, desiredHeight);
}

#endif // wxOSX_USE_COCOA

bool wxBundleResourceHandler::LoadFile(wxBitmap *bitmap,
                                     const wxString& name,
                                     wxBitmapType WXUNUSED(type),
                                     int WXUNUSED(desiredWidth),
                                     int WXUNUSED(desiredHeight))
{
    wxString ext = GetExtension().Lower();
    wxCFStringRef resname(name);
    wxCFStringRef resname2x(name+"@2x");
    wxCFStringRef restype(ext);
    double scale = 1.0;
    
    wxCFRef<CFURLRef> imageURL;
    
    if ( wxOSXGetMainScreenContentScaleFactor() > 1.9 )
    {
        imageURL.reset(CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname2x, restype, NULL));
        scale = 2.0;
    }
    
    if ( imageURL.get() == NULL )
    {
        imageURL.reset(CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname, restype, NULL));
        scale = 1.0;
    }
    
    if ( imageURL.get() != NULL )
    {
        // Create the data provider object
        wxCFRef<CGDataProviderRef> provider(CGDataProviderCreateWithURL (imageURL) );
        CGImageRef image = NULL;
        
        if ( ext == "jpeg" )
            image = CGImageCreateWithJPEGDataProvider (provider, NULL, true,
                                                   kCGRenderingIntentDefault);
        else if ( ext == "png" )
            image = CGImageCreateWithPNGDataProvider (provider, NULL, true,
                                                       kCGRenderingIntentDefault);
        if ( image != NULL )
        {
            bitmap->Create(image,scale);
            CGImageRelease(image);
        }
    }
        
    return false ;
}

/* static */
wxBitmap wxBitmapHelpers::NewFromPNGData(const void* data, size_t size)
{
    wxCFRef<CGDataProviderRef>
        provider(CGDataProviderCreateWithData(NULL, data, size, NULL) );
    wxCFRef<CGImageRef>
        image(CGImageCreateWithPNGDataProvider(provider, NULL, true,
                                                kCGRenderingIntentDefault));

    return wxBitmap(image);
}

void wxBitmap::InitStandardHandlers()
{
#if wxOSX_USE_COCOA_OR_CARBON
    // no icns on iOS
    AddHandler( new wxICNSResourceHandler ) ;
#endif
    AddHandler( new wxPNGResourceHandler );
    AddHandler( new wxJPEGResourceHandler );
}

// ----------------------------------------------------------------------------
// raw bitmap access support
// ----------------------------------------------------------------------------

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if ( !IsOk() )
        // no bitmap, no data (raw or otherwise)
        return NULL;

    if ( bpp != GetDepth() )
        return NULL;

    data.m_width = GetWidth() ;
    data.m_height = GetHeight() ;
    data.m_stride = GetBitmapData()->GetBytesPerRow() ;

    return GetBitmapData()->BeginRawAccess() ;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(dataBase))
{
    GetBitmapData()->EndRawAccess() ;
}

void wxBitmap::UseAlpha(bool use )
{
    // remember that we are using alpha channel:
    // we'll need to create a proper mask in UngetRawData()
    GetBitmapData()->UseAlpha( use );
}

void wxBitmap::SetSelectedInto(wxDC *dc)
{
    GetBitmapData()->SetSelectedInto(dc) ;
}

wxDC* wxBitmap::GetSelectedInto() const
{
    return GetBitmapData()->GetSelectedInto() ;
}
