



/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/bitmap.cpp
// Purpose:     wxBitmap
// Author:      Stefan Csomor
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
    #include "wx/math.h"
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

static inline bool IsCGImageAlphaFlag(CGImageAlphaInfo alphaInfo)
{
    return !(alphaInfo == kCGImageAlphaNone || alphaInfo == kCGImageAlphaNoneSkipFirst || alphaInfo == kCGImageAlphaNoneSkipLast);
}

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

    bool IsOk() const override;

    void Free();

    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;
    int GetBytesPerRow() const;
    bool HasAlpha() const;
    WXImage GetImage() const;

    void SetScaleFactor(double scale);
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
    IconRef       GetIconRef() const;
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
    mutable IconRef       m_iconRef;
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
    m_nsImage = nullptr;
    m_bitmapMask = nullptr ;
    m_cgImageRef = nullptr ;
    m_isTemplate = false;

#if wxOSX_USE_ICONREF
    m_iconRef = nullptr ;
#endif
    m_hBitmap = nullptr ;

    m_rawAccessCount = 0 ;
    m_scaleFactor = 1.0;
    m_selectedInto = nullptr;
}

wxBitmapRefData::wxBitmapRefData(const wxBitmapRefData &tocopy) : wxGDIRefData()
{
    Init();
    Create(tocopy.GetWidth(), tocopy.GetHeight(), tocopy.GetDepth(), tocopy.GetScaleFactor());

    // Bitmap can have both mask and alpha channel so we have to copy both form the source.
    if (tocopy.m_bitmapMask)
        m_bitmapMask = new wxMask(*tocopy.m_bitmapMask);

    if (tocopy.HasAlpha())
        UseAlpha(true);

    unsigned char* dest = (unsigned char*)GetRawAccess();
    const unsigned char* source = static_cast<const unsigned char*>(tocopy.GetRawAccess());
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
    if (image != nullptr)
    {
        size_t width = CGImageGetWidth(image);
        size_t height = CGImageGetHeight(image);

        m_hBitmap = nullptr;
        m_scaleFactor = scale;

        size_t bytesPerRow = GetBestBytesPerRow(width * 4);

        CGImageAlphaInfo alpha = CGImageGetAlphaInfo(image);
        bool hasAlpha = IsCGImageAlphaFlag(alpha);
        m_hBitmap = CGBitmapContextCreate(nullptr, width, height, 8, bytesPerRow, wxMacGetGenericRGBColorSpace(),
                                          hasAlpha ? kCGImageAlphaPremultipliedFirst : kCGImageAlphaNoneSkipFirst);
        wxCHECK_MSG(m_hBitmap, false, "Unable to create CGBitmapContext context");
        CGRect rect = CGRectMake(0, 0, width, height);
        CGContextDrawImage(m_hBitmap, rect, image);

        CGContextTranslateCTM(m_hBitmap, 0, height);
        CGContextScaleCTM(m_hBitmap, 1 * m_scaleFactor, -1 * m_scaleFactor);
    }
    return IsOk();
}

bool wxBitmapRefData::Create(CGContextRef context)
{
    if ( context != nullptr && CGBitmapContextGetData(context) )
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
    size_t width = wxMax(1, w);
    size_t height = wxMax(1, h);

    m_scaleFactor = logicalscale;
    m_hBitmap = nullptr;

    size_t bytesPerRow = GetBestBytesPerRow(width * 4);
    m_hBitmap = CGBitmapContextCreate(nullptr, width, height, 8, bytesPerRow, wxMacGetGenericRGBColorSpace(), kCGImageAlphaNoneSkipFirst);
    wxCHECK_MSG(m_hBitmap, false, "Unable to create CGBitmapContext context");
    CGContextTranslateCTM(m_hBitmap, 0, height);
    CGContextScaleCTM(m_hBitmap, 1 * GetScaleFactor(), -1 * GetScaleFactor());

    return IsOk();
}

bool wxBitmapRefData::IsOk() const
{
    return (m_hBitmap.get() != nullptr || m_nsImage != nullptr);
}

int wxBitmapRefData::GetWidth() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
        return (int) CGBitmapContextGetWidth(m_hBitmap);
    else
        return int(wxOSXGetImageSize(m_nsImage).width * m_scaleFactor);
}

int wxBitmapRefData::GetHeight() const
{
    wxCHECK_MSG( IsOk() , 0 , "Invalid Bitmap");

    if ( m_hBitmap )
        return (int) CGBitmapContextGetHeight(m_hBitmap);
    else
        return int(wxOSXGetImageSize(m_nsImage).height * m_scaleFactor);
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

    // Row stride length makes sense only for a bitmap representation
    EnsureBitmapExists();
    return (int) CGBitmapContextGetBytesPerRow(m_hBitmap);
}

bool wxBitmapRefData::HasAlpha() const
{
    wxCHECK_MSG( IsOk() , false , "Invalid Bitmap");

    if ( m_hBitmap )
    {
        CGImageAlphaInfo alpha = CGBitmapContextGetAlphaInfo(m_hBitmap);
        return IsCGImageAlphaFlag(alpha);
    }
    else
    {
        return true; // a bitmap converted from an nsimage would have alpha
    }
}

WXImage wxBitmapRefData::GetImage() const
{
    wxCHECK_MSG( IsOk() , nullptr , "Invalid Bitmap");

    if ( !m_nsImage )
    {
        wxCFRef< CGImageRef > cgimage(CreateCGImage());
        return wxOSXGetImageFromCGImage( cgimage, GetScaleFactor(), IsTemplate() );
    }

    return m_nsImage;
}

void wxBitmapRefData::SetScaleFactor( double scale )
{
    wxCHECK_RET( IsOk() , "invalid bitmap" ) ;

    if ( m_scaleFactor == scale )
        return ;

    CGContextScaleCTM( m_hBitmap, 1 / GetScaleFactor(), -1 / GetScaleFactor() );
    m_scaleFactor = scale;
    CGContextScaleCTM( m_hBitmap, GetScaleFactor(), -GetScaleFactor() );
}

void wxBitmapRefData::UseAlpha( bool use )
{
    wxCHECK_RET( IsOk() , "invalid bitmap" ) ;

    if ( HasAlpha() == use )
        return ;

    CGContextRef hBitmap = CGBitmapContextCreate(nullptr, GetWidth(), GetHeight(), 8, GetBytesPerRow(), wxMacGetGenericRGBColorSpace(), use ? kCGImageAlphaPremultipliedFirst : kCGImageAlphaNoneSkipFirst );

    memcpy(CGBitmapContextGetData(hBitmap),CGBitmapContextGetData(m_hBitmap),GetBytesPerRow()*GetHeight());
    wxCHECK_RET( hBitmap , "Unable to create CGBitmapContext context" ) ;
    CGContextTranslateCTM( hBitmap, 0,  GetHeight() );
    CGContextScaleCTM( hBitmap, GetScaleFactor(), -GetScaleFactor() );

    m_hBitmap.reset(hBitmap);
}

const void *wxBitmapRefData::GetRawAccess() const
{
    wxCHECK_MSG( IsOk(), nullptr , "invalid bitmap" ) ;

    EnsureBitmapExists();

    return CGBitmapContextGetData(m_hBitmap);
}

void *wxBitmapRefData::GetRawAccess()
{
    return const_cast<void*>(const_cast<const wxBitmapRefData*>(this)->GetRawAccess());
}


void *wxBitmapRefData::BeginRawAccess()
{
    wxCHECK_MSG( IsOk(), nullptr, "invalid bitmap" ) ;
    wxASSERT( m_rawAccessCount == 0 ) ;

#if wxOSX_USE_ICONREF
    wxASSERT_MSG( m_iconRef == nullptr ,
                 "Currently, modifying bitmaps that are used in controls already is not supported" ) ;
#endif

    ++m_rawAccessCount ;

    // we must destroy an existing cached image, as
    // the bitmap data may change now
    FreeDerivedRepresentations();

    return GetRawAccess() ;
}

void wxBitmapRefData::EndRawAccess()
{
    wxCHECK_RET( IsOk() , "invalid bitmap" ) ;
    wxASSERT( m_rawAccessCount == 1 ) ;

    --m_rawAccessCount ;

    // Update existing NSImage with new bitmap data
    if ( m_nsImage )
    {
        wxCFRef<CGImageRef> image(CGBitmapContextCreateImage(m_hBitmap));
        wxMacCocoaRelease(m_nsImage);
        m_nsImage = wxOSXGetImageFromCGImage(image, GetScaleFactor(), IsTemplate());
        wxMacCocoaRetain(m_nsImage);
    }
}

bool wxBitmapRefData::HasNativeSize()
{
    int w = GetWidth() ;
    int h = GetHeight() ;
    int sz = wxMax( w , h ) ;

    return ( sz == 128 || sz == 48 || sz == 32 || sz == 16 );
}

#if wxOSX_USE_ICONREF
IconRef wxBitmapRefData::GetIconRef() const
{
    if ( m_iconRef == nullptr )
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
                unsigned char * masksourcePtr = mask ? (unsigned char*) mask->GetRawAccess() : nullptr ;

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
                            a = *masksource++ ;
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

                Handle data = nullptr ;
                Handle maskdata = nullptr ;
                unsigned char * maskptr = nullptr ;
                unsigned char * ptr = nullptr ;
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
                unsigned char * masksourcePtr = mask ? (unsigned char*) mask->GetRawAccess() : nullptr ;

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
                            *maskdest++ = *masksource++ ;
                        else if ( hasAlpha )
                            *maskdest++ = a ;
                        else
                            *maskdest++ = 0xFF ;
                    }
                }

                OSStatus err = SetIconFamilyData( iconFamily, dataType , data ) ;
                wxASSERT_MSG( err == noErr , "Error when adding bitmap" ) ;

                err = SetIconFamilyData( iconFamily, maskType , maskdata ) ;
                wxASSERT_MSG( err == noErr , "Error when adding mask" ) ;

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

        wxCHECK_MSG( err == noErr, nullptr, "Error when constructing icon ref" );
    }

    return m_iconRef ;
}
#endif // wxOSX_USE_ICONREF

CGImageRef wxBitmapRefData::CreateCGImage() const
{
    wxASSERT( IsOk() ) ;
    wxASSERT( m_rawAccessCount >= 0 ) ;
    CGImageRef image ;
    if ( m_rawAccessCount > 0 || m_cgImageRef == nullptr )
    {
        if ( m_hBitmap )
        {
            image = CGBitmapContextCreateImage(m_hBitmap);
        }
        else
        {
            image = wxOSXCreateCGImageFromImage(m_nsImage);
        }

        if ( m_bitmapMask )
        {
            // First apply mask to image
            CGImageRef imageMask = CGBitmapContextCreateImage(m_bitmapMask->GetHBITMAP());
            CGImageRef imageMasked = CGImageCreateWithMask(image, imageMask);

            // Convert masked image to plain ARGB image without mask
            int w = GetWidth();
            int h = GetHeight();
            CGContextRef hBmpAlpha = CGBitmapContextCreate(nullptr, w, h, 8, GetBytesPerRow(), wxMacGetGenericRGBColorSpace(),                   kCGImageAlphaPremultipliedFirst);
            CGRect r = CGRectMake(0, 0, w, h);
            CGContextDrawImage(hBmpAlpha, r, imageMasked);
            CGContextTranslateCTM(hBmpAlpha, 0, h);
            CGContextScaleCTM(hBmpAlpha, GetScaleFactor(), -GetScaleFactor());

            CGImageRelease(imageMasked);
            CGImageRelease(imageMask);

            CGImageRelease(image);
            image = CGBitmapContextCreateImage(hBmpAlpha);
            CGContextRelease(hBmpAlpha);
        }
    }
    else
    {
        image = m_cgImageRef ;
        CGImageRetain( image ) ;
    }

    if ( m_rawAccessCount == 0 && m_cgImageRef == nullptr)
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
    if ( dc == nullptr )
    {
        if ( m_selectedInto != nullptr )
            EndRawAccess();
    }
    else
    {
        wxASSERT_MSG( m_selectedInto == nullptr || m_selectedInto == dc, "Bitmap already selected into a different dc");

        if ( m_selectedInto == nullptr )
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
        m_cgImageRef = nullptr ;
    }
#if wxOSX_USE_ICONREF
    if ( m_iconRef )
    {
        ReleaseIconRef( m_iconRef ) ;
        m_iconRef = nullptr ;
    }
#endif // wxOSX_USE_ICONREF
}

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( m_rawAccessCount == 0 , "Bitmap still selected when destroyed" ) ;

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

            const unsigned char* linestart = reinterpret_cast<const unsigned char*>(bits);
            unsigned char* destptr = (unsigned char*) GetBitmapData()->BeginRawAccess() ;

            for ( int y = 0 ; y < the_height ; ++y , linestart += linesize, destptr += GetBitmapData()->GetBytesPerRow() )
            {
                unsigned char* destination = destptr;

                for ( int x = 0 ; x < the_width ; ++x )
                {
                    int index, bit, mask;
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
            wxFAIL_MSG("multicolor BITMAPs not yet implemented");
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
    wxCHECK_MSG( IsOk() , nullptr , "invalid bitmap" ) ;

    return GetBitmapData()->GetRawAccess() ;
}

void * wxBitmap::BeginRawAccess()
{
    wxCHECK_MSG( IsOk() , nullptr , "invalid bitmap" ) ;

    return GetBitmapData()->BeginRawAccess() ;
}

void wxBitmap::EndRawAccess()
{
    wxCHECK_RET( IsOk() , "invalid bitmap" ) ;

    GetBitmapData()->EndRawAccess() ;
}
#endif

CGImageRef wxBitmap::CreateCGImage() const
{
    wxCHECK_MSG( IsOk(), nullptr , "invalid bitmap" ) ;

    return GetBitmapData()->CreateCGImage() ;
}

#if wxOSX_USE_ICONREF

IconRef wxBitmap::GetIconRef() const
{
    wxCHECK_MSG( IsOk(), nullptr , "invalid bitmap" ) ;

    return GetBitmapData()->GetIconRef() ;
}

IconRef wxBitmap::CreateIconRef() const
{
    IconRef icon = GetIconRef();
    wxOSX_VERIFY_NOERR(AcquireIconRef(icon));
    return icon;
}
#endif

wxBitmap::wxBitmap(WXImage image)
{
    (void)Create(image);
}

#if wxOSX_USE_COCOA
wxBitmap::wxBitmap(const wxCursor &cursor)
{
    m_refData = new wxBitmapRefData( wxOSXGetNSImageFromNSCursor( cursor.GetHCURSOR() ) );
}
#endif

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

WXImage wxBitmap::OSXGetImage() const
{
    return IsOk() ? GetBitmapData()->GetImage() : nullptr;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect &rect) const
{
    wxCHECK_MSG( IsOk(), wxNullBitmap, "invalid bitmap" );

    wxCHECK_MSG((rect.x >= 0) && (rect.y >= 0) &&
                (rect.x+rect.width <= GetLogicalWidth()) &&
                (rect.y+rect.height <= GetLogicalHeight()),
                wxNullBitmap, "invalid bitmap region" );

    wxBitmap ret;
    double scale = GetScaleFactor();
    ret.CreateWithLogicalSize( rect.GetSize(), scale, GetDepth() );
    wxASSERT_MSG( ret.IsOk(), "GetSubBitmap error" );
    if ( HasAlpha() )
        ret.UseAlpha() ;

    int destwidth = int(rect.width * scale);
    int destheight = int(rect.height * scale);

    {
        const unsigned char* sourcedata = static_cast<const unsigned char*>(GetBitmapData()->GetRawAccess());
        unsigned char *destdata = (unsigned char*) ret.GetBitmapData()->BeginRawAccess() ;
        wxASSERT((sourcedata != nullptr) && (destdata != nullptr));

        if ( (sourcedata != nullptr) && (destdata != nullptr) )
        {
            int sourcelinesize = GetBitmapData()->GetBytesPerRow() ;
            int destlinesize = ret.GetBitmapData()->GetBytesPerRow() ;
            const unsigned char* source = sourcedata + size_t(rect.x * scale) * 4 + size_t(rect.y * scale) * sourcelinesize;
            unsigned char* dest = destdata;

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
        wxASSERT( (source != nullptr) && (destdata != nullptr) ) ;

        if ( (source != nullptr) && (destdata != nullptr) )
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
    return CreateWithDIPSize(w, h, factor);
}

bool wxBitmap::DoCreate(const wxSize& size, double scale, int d)
{
    UnRef();

    if ( d < 0 )
        d = wxDisplayDepth() ;

    const wxSize sizePhys = size*scale;
    m_refData = new wxBitmapRefData( sizePhys.x, sizePhys.y, d, scale );

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
            const int contentScaleFactor = wxRound(wxOSXGetMainScreenContentScaleFactor());
            if ( contentScaleFactor > 1 )
            {
                wxFileName fn(filename);
                fn.MakeAbsolute();
                fn.SetName(fn.GetName()+wxString::Format("@%dx",contentScaleFactor));

                if ( fn.Exists() )
                {
                    fname = fn.GetFullPath();
                    scale = contentScaleFactor;
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

    wxLogWarning("no bitmap handler for type %d defined.", type);

    return false;
}

bool wxBitmap::Create(const void* data, wxBitmapType type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == nullptr )
    {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return false;
    }

    return handler->Create(this, data, type, width, height, depth);
}

#if wxUSE_IMAGE

void wxBitmap::InitFromImage(const wxImage& image, int depth, double scale)
{
    wxCHECK_RET( image.IsOk(), "invalid image" );

    // width and height of the device-dependent bitmap
    int width = image.GetWidth();
    int height = image.GetHeight();
    // we always use 32 bit internally here
    depth = 32;

    wxBitmapRefData* bitmapRefData;

    m_refData = bitmapRefData = new wxBitmapRefData( width, height, depth, scale) ;

    if ( bitmapRefData->IsOk())
    {
        bool hasMask = image.HasMask();
        bool hasAlpha = image.HasAlpha();

        // convert this bitmap to use its alpha channel
        if ( hasAlpha )
            UseAlpha();

        const unsigned char *sourcePixels = image.GetData();
        unsigned char* destinationPixels = (unsigned char*) GetBitmapData()->BeginRawAccess() ;

        if ( destinationPixels != nullptr && sourcePixels != nullptr )
        {
            const unsigned char *sourceAlpha = hasAlpha ? image.GetAlpha() : nullptr ;

            const unsigned char mr = hasMask ? image.GetMaskRed() : 0;
            const unsigned char mg = hasMask ? image.GetMaskGreen() : 0;
            const unsigned char mb = hasMask ? image.GetMaskBlue() : 0;

            unsigned char* destinationRowStart = destinationPixels;

            wxMemoryBuffer maskbuf ;
            int maskRowBytes = GetBestBytesPerRow( width * kMaskBytesPerPixel );
            size_t maskbufsize = maskRowBytes * height ;

            unsigned char *destinationMask = nullptr;
            if ( hasMask )
                destinationMask = (unsigned char * ) maskbuf.GetWriteBuf( maskbufsize ) ;

            unsigned char* destinationMaskRowStart = destinationMask;

            for (int y = 0; y < height;
                destinationRowStart += GetBitmapData()->GetBytesPerRow(),
                destinationMaskRowStart += hasMask ? maskRowBytes : 0,
                y++)
            {
                unsigned char * destination = destinationRowStart;
                unsigned char * destinationMask = destinationMaskRowStart;
                for (int x = 0; x < width; x++)
                {
                    if ( hasMask )
                    {
                        bool isMasked = sourcePixels[0] == mr && sourcePixels[1] == mg && sourcePixels[2] == mb;
                        *destinationMask++ = isMasked ? wxIMAGE_ALPHA_TRANSPARENT : wxIMAGE_ALPHA_OPAQUE ;
                    }

                    if ( hasAlpha )
                    {
                        unsigned char a = *sourceAlpha++;

                        *destination++ = a ;
#if wxOSX_USE_PREMULTIPLIED_ALPHA
                        *destination++ = ((*sourcePixels++) * a + 127) / 255 ;
                        *destination++ = ((*sourcePixels++) * a + 127) / 255 ;
                        *destination++ = ((*sourcePixels++) * a + 127) / 255 ;
#else
                        *destination++ = *sourcePixels++ ;
                        *destination++ = *sourcePixels++ ;
                        *destination++ = *sourcePixels++ ;
#endif
                    }
                    else
                    {
                        *destination++ = wxIMAGE_ALPHA_OPAQUE ;
                        *destination++ = *sourcePixels++ ;
                        *destination++ = *sourcePixels++ ;
                        *destination++ = *sourcePixels++ ;

                    }
                }
            }
            GetBitmapData()->EndRawAccess() ;

            if ( hasMask )
            {
                maskbuf.UngetWriteBuf( maskbufsize ) ;

                wxMask* mask = new wxMask();
                mask->OSXCreate(maskbuf, width , height , maskRowBytes ) ;

                SetMask( mask );
            }
        }
    }
}

wxBitmap::wxBitmap(const wxImage& image, int depth, double scale)
{
    InitFromImage(image, depth, scale);
}

wxBitmap::wxBitmap(const wxImage& image, const wxDC& dc)
{
    InitFromImage(image, -1, dc.GetContentScaleFactor());
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;

    wxCHECK_MSG( IsOk(), wxNullImage, "invalid bitmap" );

    // this call may trigger a conversion from platform image to bitmap, issue it
    // before any measurements are taken, multi-resolution platform images may be
    // rendered incorrectly otherwise
    const unsigned char* sourcestart = static_cast<const unsigned char*>(GetBitmapData()->GetRawAccess());

    // create an wxImage object
    int width = GetWidth();
    int height = GetHeight();
    image.Create( width, height );

    unsigned char *data = image.GetData();
    wxCHECK_MSG( data, wxNullImage, "Could not allocate data for image" );

    bool hasAlpha = false ;
    bool hasMask = false ;
    int maskBytesPerRow = 0 ;
    unsigned char *alpha = nullptr ;
    unsigned char *mask = nullptr ;

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
        const wxUint32* source = reinterpret_cast<const wxUint32*>(sourcestart);

        for (int xx = 0; xx < width; xx++)
        {
            const wxUint32 color = *source++;
            unsigned char a, r, g, b;
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
            bool isMasked = false;
            if ( hasMask )
            {
                if ( *maskp++ == 0x00 )
                {
                    r = MASK_RED ;
                    g = MASK_GREEN ;
                    b = MASK_BLUE ;
                    isMasked = true;
                }
                else if ( r == MASK_RED && g == MASK_GREEN && b == MASK_BLUE )
                    b = MASK_BLUE_REPLACEMENT ;
            }

            if ( hasAlpha )
            {
                *alpha++ = a ;
#if wxOSX_USE_PREMULTIPLIED_ALPHA
                // this must be non-premultiplied data
                if ( !isMasked && a != 0xFF && a!= 0 )
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
        wxLogWarning("no bitmap handler for type %d defined.", type);
#endif
    }

    return success;
}

int wxBitmap::GetHeight() const
{
   wxCHECK_MSG( IsOk(), -1, "invalid bitmap" );

   return GetBitmapData()->GetHeight();
}

int wxBitmap::GetWidth() const
{
   wxCHECK_MSG( IsOk(), -1, "invalid bitmap" );

   return GetBitmapData()->GetWidth() ;
}

void wxBitmap::SetScaleFactor(double scale)
{
    wxCHECK_RET( IsOk(), "invalid bitmap" );

    if ( GetBitmapData()->GetScaleFactor() != scale )
    {
        AllocExclusive();

        GetBitmapData()->SetScaleFactor(scale) ;
    }
}

double wxBitmap::GetScaleFactor() const
{
    wxCHECK_MSG( IsOk(), -1, "invalid bitmap" );

    return GetBitmapData()->GetScaleFactor() ;
}

int wxBitmap::GetDepth() const
{
   wxCHECK_MSG( IsOk(), -1, "invalid bitmap" );

   return GetBitmapData()->GetDepth();
}

wxMask *wxBitmap::GetMask() const
{
   wxCHECK_MSG( IsOk(), nullptr, "invalid bitmap" );

   return GetBitmapData()->m_bitmapMask;
}

bool wxBitmap::HasAlpha() const
{
   wxCHECK_MSG( IsOk(), false , "invalid bitmap" );

   return GetBitmapData()->HasAlpha() ;
}

#if WXWIN_COMPATIBILITY_3_0
void wxBitmap::SetWidth(int WXUNUSED_UNLESS_DEBUG(w))
{
    AllocExclusive();
    wxASSERT_MSG( GetWidth() == w, "Changing the bitmap width is not supported");
}

void wxBitmap::SetHeight(int WXUNUSED_UNLESS_DEBUG(h))
{
    AllocExclusive();
    wxASSERT_MSG( GetHeight() == h, "Changing the bitmap height is not supported");
}

void wxBitmap::SetDepth(int WXUNUSED_UNLESS_DEBUG(d))
{
    AllocExclusive();
    wxASSERT_MSG( d == -1 || GetDepth() == d, "Changing the bitmap depth is not supported");
}
#endif

#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
   wxCHECK_MSG( IsOk(), nullptr, "Invalid bitmap  GetPalette()" );

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
    wxCHECK_MSG( m_maskBitmap, nullptr , "invalid mask" ) ;
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

    m_maskBitmap = CGBitmapContextCreate(nullptr, width, height, kMaskBytesPerPixel * 8, bytesPerRow, colorspace,
        kCGImageAlphaNone);
    wxASSERT_MSG(m_maskBitmap, "Unable to create CGBitmapContext context");
}

void wxMask::RealizeNative()
{
#if 0
    if ( m_maskBitmap )
    {
        CGContextRelease( (CGContextRef) m_maskBitmap );
       m_maskBitmap = nullptr ;
    }

    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceGray();
    // from MouseTracking sample :
    // Ironically, due to a bug in CGImageCreateWithMask, you cannot use
    // CGColorSpaceCreateWithName(kCGColorSpaceGenericGray) at this point!

    m_maskBitmap = CGBitmapContextCreate((char*) m_memBuf.GetData(), m_width, m_height, 8, m_bytesPerRow, colorspace,
        kCGImageAlphaNone );
    CGColorSpaceRelease( colorspace );
    wxASSERT_MSG( m_maskBitmap , "Unable to create CGBitmapContext context" ) ;
#endif
}

// Construct a mask from a 8 bpp memory buffer
bool wxMask::OSXCreate(const wxMemoryBuffer& data,int width , int height , int bytesPerRow)
{
    size_t dataLen = data.GetDataLen();
    wxCHECK( dataLen == (size_t)(height * bytesPerRow), false );
    const void* srcdata = data.GetData();
    wxCHECK( srcdata, false );

    DoCreateMaskBitmap(width, height, bytesPerRow);
    void* destdata = GetRawAccess();
    wxCHECK( destdata, false );

    memcpy(destdata, srcdata, dataLen);
    return true;
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::InitFromMonoBitmap(const wxBitmap& bitmap)
{
    int width, height, bytesPerRow;
    width = bitmap.GetWidth() ;
    height = bitmap.GetHeight() ;

    DoCreateMaskBitmap(width, height);
    bytesPerRow = GetBytesPerRow();

    // pixel access needs a non-const bitmap currently
    wxBitmap bmp(bitmap);
    wxNativePixelData data(bmp);

    wxNativePixelData::Iterator p(data);

    unsigned char * destdatabase = (unsigned char*) GetRawAccess();
    wxASSERT( destdatabase != nullptr ) ;
    if ( destdatabase != nullptr)
    {
        for ( int y = 0 ; y < height; ++y,  destdatabase += bytesPerRow )
        {
            wxNativePixelData::Iterator rowStart = p;
            unsigned char *destdata = destdatabase ;
            for ( int x = 0 ; x < width ; ++x, ++p )
            {
                int v = p.Red() + p.Green() + p.Blue();
                wxASSERT_MSG( v == 0 || v == 3*0xFF, "Non-monochrome bitmap supplied" );
                *destdata++ = v < (3 * 0xFF) / 2 ? 0 : 0xFF;
            }
            p = rowStart;
            p.OffsetY(data, 1);
        }
    }

    return true;
}

bool wxMask::InitFromColour(const wxBitmap& bitmap, const wxColour& colour)
{
    int width, height, bytesPerRow;

    width = bitmap.GetWidth() ;
    height = bitmap.GetHeight() ;

    DoCreateMaskBitmap(width, height);
    bytesPerRow = GetBytesPerRow();

    // pixel access needs a non-const bitmap currently
    wxBitmap bmp(bitmap);
    wxNativePixelData data(bmp);

    wxNativePixelData::Iterator p(data);

    unsigned char * destdatabase = (unsigned char*) GetRawAccess();
    wxASSERT( destdatabase != nullptr ) ;
    if ( destdatabase != nullptr)
    {
        for ( int y = 0 ; y < height; ++y,  destdatabase += bytesPerRow )
        {
            wxNativePixelData::Iterator rowStart = p;
            unsigned char *destdata = destdatabase ;
            for ( int x = 0 ; x < width ; ++x, ++p )
            {
                if ( wxColour( p.Red(), p.Green(), p.Blue() ) == colour )
                    *destdata++ = 0x0 ;
                else
                    *destdata++ = 0xFF ;
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
    int width, height, bytesPerRow;
    width = GetWidth();
    height = GetHeight();
    bytesPerRow = GetBytesPerRow();


    wxBitmap bitmap(width, height, 32);
    wxNativePixelData data(bitmap);

    wxNativePixelData::Iterator p(data);

    const unsigned char* srcbase = static_cast<unsigned char*>(GetRawAccess());

    for (int y = 0; y < height; ++y, srcbase += bytesPerRow)
    {
        wxNativePixelData::Iterator rowStart = p;
        const unsigned char* src = srcbase;
        for (int x = 0; x < width; ++x, ++p, ++src)
        {
            const unsigned char byte = *src;
            wxASSERT( byte == 0 || byte == 0xFF );
            p.Red() = p.Green() = p.Blue() = byte;
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
                          int desiredHeight) override;
};

wxIMPLEMENT_ABSTRACT_CLASS(wxBundleResourceHandler, wxBitmapHandler);

class WXDLLEXPORT wxPNGResourceHandler: public wxBundleResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxPNGResourceHandler);

public:
    inline wxPNGResourceHandler()
    {
        SetName("PNG resource");
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
        SetName("JPEG resource");
        SetExtension("JPEG");
        SetType(wxBITMAP_TYPE_JPEG_RESOURCE);
    }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxJPEGResourceHandler, wxBundleResourceHandler);

#if wxOSX_USE_COCOA

class WXDLLEXPORT wxICNSHandler: public wxBitmapHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxICNSHandler);

public:
    inline wxICNSHandler()
    {
        SetName("icns file");
        SetExtension("icns");
        SetType(wxBITMAP_TYPE_ICON);
    }

    bool LoadFile(wxBitmap *bitmap,
                          const wxString& name,
                          wxBitmapType type,
                          int desiredWidth,
                          int desiredHeight) override
    {
        wxCFRef<CFURLRef> iconURL;
        wxCFStringRef filePath(name);

        iconURL.reset(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, filePath, kCFURLPOSIXPathStyle, false));

        WXImage img = wxOSXGetNSImageFromCFURL(iconURL);

        if ( img )
        {
            bitmap->Create(img);
            return true;
        }

        return wxBitmapHandler::LoadFile( bitmap, name, type, desiredWidth, desiredHeight);
    }

};

wxIMPLEMENT_DYNAMIC_CLASS(wxICNSHandler, wxBitmapHandler);

class WXDLLEXPORT wxICNSResourceHandler: public wxBundleResourceHandler
{
    wxDECLARE_DYNAMIC_CLASS(wxICNSResourceHandler);

public:
    inline wxICNSResourceHandler()
    {
        SetName("icns resource");
        SetExtension("icns");
        SetType(wxBITMAP_TYPE_ICON_RESOURCE);
    }

    virtual bool LoadFile(wxBitmap *bitmap,
                          const wxString& name,
                          wxBitmapType type,
                          int desiredWidth,
                          int desiredHeight) override;

};

wxIMPLEMENT_DYNAMIC_CLASS(wxICNSResourceHandler, wxBundleResourceHandler);

bool wxICNSResourceHandler::LoadFile(wxBitmap *bitmap,
                                       const wxString& resourceName,
                                       wxBitmapType type,
                                       int desiredWidth,
                                       int desiredHeight)
{
    OSType theId = 0 ;

    if ( resourceName == "wxICON_INFORMATION" )
    {
        theId = kAlertNoteIcon ;
    }
    else if ( resourceName == "wxICON_QUESTION" )
    {
        theId = kAlertCautionIcon ;
    }
    else if ( resourceName == "wxICON_WARNING" )
    {
        theId = kAlertCautionIcon ;
    }
    else if ( resourceName == "wxICON_ERROR" )
    {
        theId = kAlertStopIcon ;
    }
    else if ( resourceName == "wxICON_FOLDER" )
    {
        theId = kGenericFolderIcon ;
    }
    else if ( resourceName == "wxICON_FOLDER_OPEN" )
    {
        theId = kOpenFolderIcon ;
    }
    else if ( resourceName == "wxICON_NORMAL_FILE" )
    {
        theId = kGenericDocumentIcon ;
    }
    else if ( resourceName == "wxICON_EXECUTABLE_FILE" )
    {
        theId = kGenericApplicationIcon ;
    }
    else if ( resourceName == "wxICON_CDROM" )
    {
        theId = kGenericCDROMIcon ;
    }
    else if ( resourceName == "wxICON_FLOPPY" )
    {
        theId = kGenericFloppyIcon ;
    }
    else if ( resourceName == "wxICON_HARDDISK" )
    {
        theId = kGenericHardDiskIcon ;
    }
    else if ( resourceName == "wxICON_REMOVABLE" )
    {
        theId = kGenericRemovableMediaIcon ;
    }
    else if ( resourceName == "wxICON_DELETE" )
    {
        theId = kToolbarDeleteIcon ;
    }
    else if ( resourceName == "wxICON_GO_BACK" )
    {
        theId = kBackwardArrowIcon ;
    }
    else if ( resourceName == "wxICON_GO_FORWARD" )
    {
        theId = kForwardArrowIcon ;
    }
    else if ( resourceName == "wxICON_GO_HOME" )
    {
        theId = kToolbarHomeIcon ;
    }
    else if ( resourceName == "wxICON_HELP_SETTINGS" )
    {
        theId = kGenericFontIcon ;
    }
    else if ( resourceName == "wxICON_HELP_PAGE" )
    {
        theId = kGenericDocumentIcon ;
    }
    else if ( resourceName == "wxICON_PRINT" )
    {
        theId = kPrintMonitorFolderIcon;
    }
    else if ( resourceName == "wxICON_HELP_FOLDER" )
    {
        theId = kHelpFolderIcon;
    }

    WXImage img = nullptr;

    if ( theId != 0 )
    {
        IconRef iconRef = nullptr ;
        
        wxOSX_VERIFY_NOERR(GetIconRef( kOnSystemDisk, kSystemIconsCreator, theId, &iconRef )) ;
        img = wxOSXGetNSImageFromIconRef(iconRef);
    }
    else
    {
        wxCFRef<CFURLRef> iconURL;
        wxCFStringRef resname(resourceName);
        wxCFStringRef restype(GetExtension().Lower());

        iconURL.reset(CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname, restype, nullptr));
        
        img = wxOSXGetNSImageFromCFURL(iconURL);
    }
    
    if ( img )
    {
        bitmap->Create(img);
        return true;
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
    wxCFStringRef restype(ext);
    double scale = 1.0;
    
    wxCFRef<CFURLRef> imageURL;
    
    const int contentScaleFactor = wxRound(wxOSXGetMainScreenContentScaleFactor());
    if ( contentScaleFactor > 1 )
    {
        wxCFStringRef resname(wxString::Format("%s@%dx", name, contentScaleFactor));
        imageURL.reset(CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname, restype, nullptr));
        scale = contentScaleFactor;
    }
    
    if ( imageURL.get() == nullptr )
    {
        wxCFStringRef resname(name);
        imageURL.reset(CFBundleCopyResourceURL(CFBundleGetMainBundle(), resname, restype, nullptr));
        scale = 1.0;
    }
    
    if ( imageURL.get() != nullptr )
    {
        // Create the data provider object
        wxCFRef<CGDataProviderRef> provider(CGDataProviderCreateWithURL (imageURL) );
        CGImageRef image = nullptr;
        
        if ( ext == "jpeg" )
            image = CGImageCreateWithJPEGDataProvider (provider, nullptr, true,
                                                   kCGRenderingIntentDefault);
        else if ( ext == "png" )
            image = CGImageCreateWithPNGDataProvider (provider, nullptr, true,
                                                       kCGRenderingIntentDefault);
        if ( image != nullptr )
        {
            bitmap->Create(image,scale);
            CGImageRelease(image);
            return true;
        }
    }
        
    return false ;
}

/* static */
wxBitmap wxBitmapHelpers::NewFromPNGData(const void* data, size_t size)
{
    wxCFRef<CGDataProviderRef>
        provider(CGDataProviderCreateWithData(nullptr, data, size, nullptr) );
    wxCFRef<CGImageRef>
        image(CGImageCreateWithPNGDataProvider(provider, nullptr, true,
                                                kCGRenderingIntentDefault));

    return wxBitmap(image);
}

void wxBitmap::InitStandardHandlers()
{
#if wxOSX_USE_COCOA_OR_CARBON
    // no icns on iOS
    AddHandler( new wxICNSHandler );
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
        return nullptr;

    if ( bpp != GetDepth() )
        return nullptr;

    data.m_width = GetWidth() ;
    data.m_height = GetHeight() ;
    data.m_stride = GetBitmapData()->GetBytesPerRow() ;

    return GetBitmapData()->BeginRawAccess() ;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(dataBase))
{
    GetBitmapData()->EndRawAccess() ;
}

bool wxBitmap::UseAlpha(bool use)
{
    // remember that we are using alpha channel:
    // we'll need to create a proper mask in UngetRawData()
    GetBitmapData()->UseAlpha( use );

    return true;
}

void wxBitmap::SetSelectedInto(wxDC *dc)
{
    GetBitmapData()->SetSelectedInto(dc) ;
}

wxDC* wxBitmap::GetSelectedInto() const
{
    return GetBitmapData()->GetSelectedInto() ;
}
