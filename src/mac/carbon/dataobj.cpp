///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dataobj.cpp
// Purpose:     implementation of wxDataObject class
// Author:      Stefan Csomor
// Modified by:
// Created:     10/21/99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAOBJ

#include "wx/dataobj.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/metafile.h"
#include "wx/tokenzr.h"

#include "wx/mac/uma.h"

#ifdef __DARWIN__
    #include <QuickTime/QuickTime.h>
#endif


// ----------------------------------------------------------------------------
// wxDataFormat
// ----------------------------------------------------------------------------

wxDataFormat::wxDataFormat()
{
    m_type = wxDF_INVALID;
    m_format = 0;
}

wxDataFormat::wxDataFormat( wxDataFormatId vType )
{
    m_format = 0;
    m_type = wxDF_INVALID;
    SetType( vType );
}

wxDataFormat::wxDataFormat( const wxChar *zId )
{
    m_format = 0;
    m_type = wxDF_INVALID;
    SetId( zId );
}

wxDataFormat::wxDataFormat( const wxString& rId )
{
    m_format = 0;
    m_type = wxDF_INVALID;
    SetId( rId );
}

wxDataFormat::wxDataFormat(const wxDataFormat& rFormat)
{
    if ( rFormat.m_format )
        m_format = (NativeFormat) CFStringCreateCopy(NULL, (CFStringRef)rFormat.m_format);
    else
        m_format = 0;
    m_type = rFormat.m_type;
    m_id = rFormat.m_id;
}

wxDataFormat::wxDataFormat( NativeFormat vFormat )
{
    m_format = 0;
    m_type = wxDF_INVALID;
    SetId( vFormat );
}

wxDataFormat::~wxDataFormat()
{
    if ( m_format != 0 )
    {
        CFRelease( (CFStringRef) m_format );
        m_format = 0;
    }
}

// in order to be correct for 10.3 we restrict to the available types there
// http://developer.apple.com/qa/qa2005/qa1406.html
// TODO : Use UTCoreTypes.h constants once we support 10.4+ only

wxDataFormat& wxDataFormat::operator=(const wxDataFormat& rFormat)
{
    if ( m_format != 0 )
    {
        CFRelease( (CFStringRef) m_format );
        m_format = 0;
    }
    if ( rFormat.m_format )
        m_format = (NativeFormat) CFStringCreateCopy(NULL, (CFStringRef)rFormat.m_format);
    m_type = rFormat.m_type;
    m_id = rFormat.m_id;
    return *this;
}

void wxDataFormat::SetType( wxDataFormatId dataType )
{
    m_type = dataType;
    if ( m_format != 0 )
    {
        CFRelease( (CFStringRef) m_format );
        m_format = 0;
    }

    switch (m_type)
    {
    case wxDF_TEXT:
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("public.plain-text") );
        break;

    case wxDF_UNICODETEXT:
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("public.utf16-plain-text") );
        break;

    case wxDF_BITMAP:
#if wxMAC_USE_CORE_GRAPHICS
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("public.tiff") );
#else
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("com.apple.pict") );
#endif
        break;
    case wxDF_METAFILE:
#if wxMAC_USE_CORE_GRAPHICS
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("com.adobe.pdf") );
#else
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("com.apple.pict") );
#endif
        break;

    case wxDF_FILENAME:
        m_format = (long) CFStringCreateCopy( NULL, CFSTR("public.file-url") );
        break;

    default:
       wxFAIL_MSG( wxT("invalid data format") );
       break;
    }
}

wxString wxDataFormat::GetId() const
{
    wxCHECK_MSG( !IsStandard(), wxEmptyString,
                 wxT("name of predefined format cannot be retrieved") );

    return m_id;
}

void wxDataFormat::SetId( NativeFormat format )
{
    if ( m_format != 0 )
    {
        CFRelease( (CFStringRef) m_format );
        m_format = 0;
    }
    m_format = (NativeFormat) CFStringCreateCopy(NULL, (CFStringRef)format);
    if (  UTTypeConformsTo( (CFStringRef)format, CFSTR("public.utf16-plain-text") )  ) 
    {
        m_type = wxDF_UNICODETEXT;
    } 
    else if ( UTTypeConformsTo( (CFStringRef)format, CFSTR("public.plain-text") ) )
    {
        m_type = wxDF_TEXT;
    }
#if wxMAC_USE_CORE_GRAPHICS
    else if (  UTTypeConformsTo( (CFStringRef)format, CFSTR("public.tiff") )  ) 
    {
        m_type = wxDF_BITMAP;
    }
    else if (  UTTypeConformsTo( (CFStringRef)format, CFSTR("com.adobe.pdf") )  ) 
    {
        m_type = wxDF_METAFILE;
    }
#else
    else if (  UTTypeConformsTo( (CFStringRef)format, CFSTR("com.apple.pict") )  ) 
    {
        m_type = wxDF_METAFILE;
    }
#endif
    else if (  UTTypeConformsTo( (CFStringRef)format, CFSTR("public.file-url") )  ) 
    {
        m_type = wxDF_FILENAME;
    }
    else 
    {
        m_type = wxDF_PRIVATE;
        m_id = wxMacCFStringHolder( (CFStringRef) CFRetain((CFStringRef) format )).AsString();
    }
}

void wxDataFormat::SetId( const wxChar* zId )
{
    m_type = wxDF_PRIVATE;
    m_id = zId;
    if ( m_format != 0 )
    {
        CFRelease( (CFStringRef) m_format );
        m_format = 0;
    }
    // since it is private, no need to conform to anything ...
    m_format = (long) wxMacCFStringHolder(m_id).Detach();
}

bool wxDataFormat::operator==(const wxDataFormat& format) const
{
    if (IsStandard() || format.IsStandard())
        return ( format.m_type == m_type );
    else
        return ( UTTypeConformsTo( (CFStringRef) m_format , (CFStringRef) format.m_format ) );
}

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

wxDataObject::wxDataObject()
{
}

bool wxDataObject::IsSupportedFormat( const wxDataFormat& rFormat, Direction vDir ) const
{
    size_t nFormatCount = GetFormatCount( vDir );
    bool found = false;

    if (nFormatCount == 1)
    {
        found = (rFormat == GetPreferredFormat());
    }
    else
    {
        wxDataFormat *pFormats = new wxDataFormat[nFormatCount];
        GetAllFormats( pFormats, vDir );

        for (size_t n = 0; n < nFormatCount; n++)
        {
            if (pFormats[n] == rFormat)
            {
                found = true;
                break;
            }
        }

        delete [] pFormats;
    }

    return found;
}

void wxDataObject::AddToPasteboard( void * pb, int itemID )
{
    PasteboardRef pasteboard = (PasteboardRef) pb;
   // get formats from wxDataObjects
    wxDataFormat *array = new wxDataFormat[ GetFormatCount() ];
    GetAllFormats( array );

    for (size_t i = 0; i < GetFormatCount(); i++)
    {
        wxDataFormat thisFormat = array[ i ];
    
        // add four bytes at the end for data objs like text that 
        // have a datasize = strlen but still need a buffer for the
        // string including trailing zero
        
        size_t datasize = GetDataSize( thisFormat );
        size_t sz = datasize + 4;
        void* buf = malloc( sz );
        if ( buf != NULL )
        {
            // empty the buffer because in some case GetDataHere does not fill buf
            memset( buf, 0, sz );
            if ( GetDataHere( array[ i ], buf ) )
            {
                int counter = 1 ;
                if ( thisFormat.GetType() == wxDF_FILENAME )
                {
                    // the data is D-normalized UTF8 strings of filenames delimited with \n
                    char *fname = strtok((char*) buf,"\n");
                    while (fname != NULL)
                    {
                        // translate the filepath into a fileurl and put that into the pasteobard
                        CFStringRef path = CFStringCreateWithBytes(NULL,(UInt8*)fname,strlen(fname),kCFStringEncodingUTF8,false);
                        CFURLRef url = CFURLCreateWithFileSystemPath(NULL, path , kCFURLPOSIXPathStyle, false);
                        CFRelease(path);
                        CFDataRef data = CFURLCreateData(NULL,url,kCFStringEncodingUTF8,true);
                        CFRelease(url);
                        PasteboardPutItemFlavor( pasteboard, (PasteboardItemID) counter,
                            (CFStringRef) thisFormat.GetFormatId() , data, kPasteboardFlavorNoFlags);
                        CFRelease( data );
                        counter++;
                        fname = strtok (NULL,"\n");
                    }
                    
                }
                else
                {
                    CFDataRef data = CFDataCreate( kCFAllocatorDefault, (UInt8*)buf, datasize );
                    if ( thisFormat.GetType() == wxDF_TEXT )
                         PasteboardPutItemFlavor( pasteboard, (PasteboardItemID) itemID,
                            CFSTR("com.apple.traditional-mac-plain-text") , data, kPasteboardFlavorNoFlags);
                    else
                        PasteboardPutItemFlavor( pasteboard, (PasteboardItemID) itemID,
                            (CFStringRef) thisFormat.GetFormatId() , data, kPasteboardFlavorNoFlags);
                    CFRelease( data );
                }
            }
            free( buf );
        }
    }

    delete [] array;
}

bool wxDataObject::IsFormatInPasteboard( void * pb, const wxDataFormat &dataFormat )
{
    PasteboardRef pasteboard = (PasteboardRef) pb;
    bool hasData = false;
    OSStatus err = noErr;
    ItemCount itemCount;

    // we synchronize here once again, so we don't mind which flags get returned
    PasteboardSynchronize( pasteboard );

    err = PasteboardGetItemCount( pasteboard, &itemCount );
    if ( err == noErr )
    {
        for( UInt32 itemIndex = 1; itemIndex <= itemCount && hasData == false ; itemIndex++ )
        {
            PasteboardItemID    itemID;
            CFArrayRef          flavorTypeArray;
            CFIndex             flavorCount;
     
            err = PasteboardGetItemIdentifier( pasteboard, itemIndex, &itemID );
            if ( err != noErr )
                continue;
     
            err = PasteboardCopyItemFlavors( pasteboard, itemID, &flavorTypeArray );
            if ( err != noErr )
                continue;
     
            flavorCount = CFArrayGetCount( flavorTypeArray );
     
            for( CFIndex flavorIndex = 0; flavorIndex < flavorCount && hasData == false ; flavorIndex++ )
            {
                CFStringRef             flavorType;
     
                flavorType = (CFStringRef)CFArrayGetValueAtIndex( flavorTypeArray,
                                                                     flavorIndex );
                     
                wxDataFormat flavorFormat( (wxDataFormat::NativeFormat) flavorType );
                if ( dataFormat == flavorFormat )
                    hasData = true;
                else if (  dataFormat.GetType() == wxDF_UNICODETEXT && flavorFormat.GetType() == wxDF_TEXT )
                    hasData = true;
            }
            CFRelease (flavorTypeArray);
        }
    }

    return hasData;
}

bool wxDataObject::GetFromPasteboard( void * pb )
{
    PasteboardRef pasteboard = (PasteboardRef) pb;
    size_t formatcount = GetFormatCount() + 1;
    wxDataFormat *array = new wxDataFormat[ formatcount ];
    array[0] = GetPreferredFormat();
    GetAllFormats( &array[1] );
    ItemCount itemCount = 0;
    wxString filenamesPassed;
    bool transferred = false;

    // we synchronize here once again, so we don't mind which flags get returned
    PasteboardSynchronize( pasteboard );

    OSStatus err = PasteboardGetItemCount( pasteboard, &itemCount );
    if ( err == noErr )
    {
        for (size_t i = 0; !transferred && i < formatcount; i++)
        {
            // go through the data in our order of preference
            wxDataFormat dataFormat = array[ i ];

            for( UInt32 itemIndex = 1; itemIndex <= itemCount && transferred == false ; itemIndex++ )
            {
                PasteboardItemID    itemID = 0;
                CFArrayRef          flavorTypeArray = NULL;
                CFIndex             flavorCount = 0;

                err = PasteboardGetItemIdentifier( pasteboard, itemIndex, &itemID );
                if ( err != noErr )
                    continue;

                err = PasteboardCopyItemFlavors( pasteboard, itemID, &flavorTypeArray );
                if ( err != noErr )
                    continue;
                    
                flavorCount = CFArrayGetCount( flavorTypeArray );

                for( CFIndex flavorIndex = 0; !transferred && flavorIndex < flavorCount ; flavorIndex++ )
                {
                    CFStringRef             flavorType;
                    CFDataRef               flavorData;
                    CFIndex                 flavorDataSize;
         
                    flavorType = (CFStringRef)CFArrayGetValueAtIndex( flavorTypeArray,
                                                                         flavorIndex );

                    // avoid utf8 being treated closer to plain-text than unicode by forcing a conversion
                    if ( UTTypeConformsTo(flavorType, CFSTR("public.utf8-plain-text") ) )
                    {
                        flavorType = CFSTR("public.utf16-plain-text");
                    }
                    wxDataFormat flavorFormat( (wxDataFormat::NativeFormat) flavorType );
 
                    if ( dataFormat == flavorFormat )
                    {
                        err = PasteboardCopyItemFlavorData( pasteboard, itemID, flavorType , &flavorData );
                        if ( err == noErr )
                        {
                            flavorDataSize = CFDataGetLength( flavorData );
                            if (dataFormat.GetType() == wxDF_FILENAME )
                            {
                                // revert the translation and decomposition to arrive at a proper utf8 string again
                                CFURLRef url = CFURLCreateWithBytes( kCFAllocatorDefault, CFDataGetBytePtr( flavorData ), flavorDataSize, kCFStringEncodingUTF8, NULL );
                                CFStringRef cfString = CFURLCopyFileSystemPath( url, kCFURLPOSIXPathStyle );
                                CFRelease( url );
                                CFMutableStringRef cfMutableString = CFStringCreateMutableCopy(NULL, 0, cfString);
                                CFRelease( cfString );
                                CFStringNormalize(cfMutableString,kCFStringNormalizationFormC);
                                wxString path = wxMacCFStringHolder(cfMutableString).AsString();
                                if (!path.empty())
                                    filenamesPassed += path + wxT("\n");
                            }
                            else
                            {
                                // because some data implementation expect trailing a trailing NUL, we add some headroom
                                void *buf = malloc( flavorDataSize + 4 );
                                if ( buf )
                                {
                                    memset( buf, 0, flavorDataSize + 4 );
                                    memcpy( buf, CFDataGetBytePtr( flavorData ), flavorDataSize );
 
                                    if (dataFormat.GetType() == wxDF_TEXT)
                                        wxMacConvertNewlines10To13( (char*) buf );
                                    SetData( flavorFormat, flavorDataSize, buf );
                                    transferred = true;
                                    free( buf );
                                }
                            }
                            CFRelease (flavorData);
                        }
                    }
                    else if ( dataFormat.GetType() == wxDF_UNICODETEXT && flavorFormat.GetType() == wxDF_TEXT )
                    {
                        err = PasteboardCopyItemFlavorData( pasteboard, itemID, flavorType, &flavorData );
                        if ( err == noErr )
                        {
                            flavorDataSize = CFDataGetLength( flavorData );
                            void *asciibuf = malloc( flavorDataSize + 1 );
                            if ( asciibuf )
                            {
                                memset( asciibuf, 0, flavorDataSize + 1 );
                                memcpy( asciibuf, CFDataGetBytePtr( flavorData ), flavorDataSize );
                                CFRelease (flavorData);

                                SetData( wxDF_TEXT, flavorDataSize, asciibuf );
                                transferred = true;
                                free( asciibuf );
                            }
                            else
                                CFRelease (flavorData);
                        }
                    }
                }
                CFRelease( flavorTypeArray );
            }
            if (filenamesPassed.length() > 0)
            {
                wxCharBuffer buf = filenamesPassed.fn_str();
                SetData( wxDF_FILENAME, strlen( buf ), (const char*)buf );
                transferred = true;
            }
        }
    }
    return transferred;
}

bool wxDataObject::HasDataInPasteboard( void * pb )
{
    PasteboardRef pasteboard = (PasteboardRef) pb;
    size_t formatcount = GetFormatCount() + 1;
    wxDataFormat *array = new wxDataFormat[ formatcount ];
    array[0] = GetPreferredFormat();
    GetAllFormats( &array[1] );
    ItemCount itemCount = 0;
    bool hasData = false;

    // we synchronize here once again, so we don't mind which flags get returned
    PasteboardSynchronize( pasteboard );

    OSStatus err = PasteboardGetItemCount( pasteboard, &itemCount );
    if ( err == noErr )
    {
        for (size_t i = 0; !hasData && i < formatcount; i++)
        {
            // go through the data in our order of preference
            wxDataFormat dataFormat = array[ i ];

            for( UInt32 itemIndex = 1; itemIndex <= itemCount && hasData == false ; itemIndex++ )
            {
                PasteboardItemID    itemID = 0;
                CFArrayRef          flavorTypeArray = NULL;
                CFIndex             flavorCount = 0;

                err = PasteboardGetItemIdentifier( pasteboard, itemIndex, &itemID );
                if ( err != noErr )
                    continue;

                err = PasteboardCopyItemFlavors( pasteboard, itemID, &flavorTypeArray );
                if ( err != noErr )
                    continue;
                    
                flavorCount = CFArrayGetCount( flavorTypeArray );

                for( CFIndex flavorIndex = 0; !hasData && flavorIndex < flavorCount ; flavorIndex++ )
                {
                    CFStringRef             flavorType;
          
                    flavorType = (CFStringRef)CFArrayGetValueAtIndex( flavorTypeArray,
                                                                         flavorIndex );

                    wxDataFormat flavorFormat( (wxDataFormat::NativeFormat) flavorType );
 
                    if ( dataFormat == flavorFormat || 
                        dataFormat.GetType() == wxDF_UNICODETEXT && flavorFormat.GetType() == wxDF_TEXT )
                    {
                        hasData = true;
                    }
                }
                CFRelease( flavorTypeArray );
            }
        }
    }
    return hasData;
}

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

#if wxUSE_UNICODE
void wxTextDataObject::GetAllFormats( wxDataFormat *formats, wxDataObjectBase::Direction dir ) const
{
    *formats++ = wxDataFormat( wxDF_TEXT );
    *formats = wxDataFormat( wxDF_UNICODETEXT );
}
#endif

// ----------------------------------------------------------------------------
// wxFileDataObject
// ----------------------------------------------------------------------------

void wxFileDataObject::GetFileNames( wxCharBuffer &buf ) const
{
    wxString filenames;

    for (size_t i = 0; i < m_filenames.GetCount(); i++)
    {
        filenames += m_filenames[i];
        filenames += wxT('\n');
    }

    buf = filenames.fn_str();
}

bool wxFileDataObject::GetDataHere( void *pBuf ) const
{
    if (pBuf == NULL)
        return false;

    wxCharBuffer buf;
    size_t buffLength;

    GetFileNames( buf );
    buffLength = strlen( buf );
    memcpy( pBuf, (const char*)buf, buffLength + 1 );

    return true;
}

size_t wxFileDataObject::GetDataSize() const
{
    wxCharBuffer buf;
    size_t buffLength;

    GetFileNames( buf );
    buffLength = strlen( buf );
    // terminating 0
    return buffLength + 1;
}

bool wxFileDataObject::SetData( size_t nSize, const void *pBuf )
{
    wxString filenames;

#if wxUSE_UNICODE
    filenames = wxString( (const char*)pBuf, *wxConvFileName );
#else
    filenames = wxString (wxConvLocal.cWC2WX(wxConvFileName->cMB2WC( (const char*)pBuf)));
#endif

    m_filenames = wxStringTokenize( filenames, wxT("\n"), wxTOKEN_STRTOK );

    return true;
}

void wxFileDataObject::AddFile( const wxString& rFilename )
{
    m_filenames.Add( rFilename );
}

// ----------------------------------------------------------------------------
// wxBitmapDataObject
// ----------------------------------------------------------------------------

wxBitmapDataObject::wxBitmapDataObject()
{
    Init();
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap& rBitmap )
: wxBitmapDataObjectBase( rBitmap )
{
    Init();

    if (m_bitmap.Ok())
    {
#if wxMAC_USE_CORE_GRAPHICS
		SetBitmap( rBitmap );
#else
        m_pictHandle = m_bitmap.GetBitmapData()->GetPictHandle();
        m_pictCreated = false;
#endif
    }
}

wxBitmapDataObject::~wxBitmapDataObject()
{
    Clear();
}

void wxBitmapDataObject::SetBitmap( const wxBitmap& rBitmap )
{
    Clear();
    wxBitmapDataObjectBase::SetBitmap( rBitmap );
    if (m_bitmap.Ok())
    {
#if wxMAC_USE_CORE_GRAPHICS
        CGImageRef cgImageRef = (CGImageRef) m_bitmap.CGImageCreate();
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
        if ( UMAGetSystemVersion() >= 0x1040 )
        {
            CFMutableDataRef data = CFDataCreateMutable(kCFAllocatorDefault, 0);
            CGImageDestinationRef destination = CGImageDestinationCreateWithData( data , kUTTypeTIFF , 1 , NULL );
            if ( destination )
            {
                CGImageDestinationAddImage( destination, cgImageRef, NULL );
                CGImageDestinationFinalize( destination );
                CFRelease( destination );
            }
            m_pictHandle = NewHandle(CFDataGetLength(data));
            if ( m_pictHandle )
            {
                memcpy( *(Handle)m_pictHandle, (const char *)CFDataGetBytePtr(data), CFDataGetLength(data) );
            }
            CFRelease( data );
        }
        else
#endif
#ifndef __LP64__
        {
            // export as TIFF
            GraphicsExportComponent exporter = 0;
            OSStatus err = OpenADefaultComponent(GraphicsExporterComponentType, kQTFileTypeTIFF, &exporter);
            if (noErr == err)
            {
                m_pictHandle = NewHandle(0);
                if ( m_pictHandle )
                {
                    err = GraphicsExportSetInputCGImage( exporter, cgImageRef);
                    err = GraphicsExportSetOutputHandle(exporter, (Handle)m_pictHandle);
                    err = GraphicsExportDoExport(exporter, NULL);
                }
                CloseComponent( exporter );
            }
        }
#endif
        CGImageRelease(cgImageRef);
#else
        m_pictHandle = m_bitmap.GetBitmapData()->GetPictHandle();
        m_pictCreated = false;
#endif
    }
}

void wxBitmapDataObject::Init()
{
    m_pictHandle = NULL;
    m_pictCreated = false;
}

void wxBitmapDataObject::Clear()
{
    if (m_pictHandle != NULL)
    {
#if wxMAC_USE_CORE_GRAPHICS
        DisposeHandle( (Handle) m_pictHandle );
#else
        if (m_pictCreated)
            KillPicture( (PicHandle)m_pictHandle );
#endif
        m_pictHandle = NULL;
    }
    m_pictCreated = false;
}

bool wxBitmapDataObject::GetDataHere( const wxDataFormat& format, void *pBuf ) const
{
    if (m_pictHandle == NULL)
    {
        wxFAIL_MSG( wxT("attempt to copy empty bitmap failed") );
        return false;
    }

    if (pBuf == NULL)
        return false;

    if ( format != GetPreferredFormat() )
        return false;

    memcpy( pBuf, *(Handle)m_pictHandle, GetHandleSize( (Handle)m_pictHandle ) );

    return true;
}

size_t wxBitmapDataObject::GetDataSize( const wxDataFormat& format ) const
{
    if ( format != GetPreferredFormat() )
        return 0;

    if (m_pictHandle != NULL)
        return GetHandleSize( (Handle)m_pictHandle );
    else
        return 0;
}

Handle MacCreateDataReferenceHandle(Handle theDataHandle)
{
    Handle  dataRef = NULL;
    OSErr   err     = noErr;

    // Create a data reference handle for our data.
    err = PtrToHand( &theDataHandle, &dataRef, sizeof(Handle));

    return dataRef;
}

size_t wxBitmapDataObject::GetDataSize() const 
{ 
    return GetDataSize(GetPreferredFormat()); 
}

bool wxBitmapDataObject::GetDataHere(void *buf) const 
{ 
    return GetDataHere(GetPreferredFormat(), buf); 
}

bool wxBitmapDataObject::SetData(size_t len, const void *buf) 
{   
    return SetData(GetPreferredFormat(), len, buf); 
}

size_t wxBitmapDataObject::GetFormatCount(Direction WXUNUSED(dir) ) const 
{ 
#if wxMAC_USE_CORE_GRAPHICS
    return 2;
#else
    return 1;
#endif
}

static wxDataFormat s_pict( _T("com.apple.pict") ); 

void wxBitmapDataObject::GetAllFormats(wxDataFormat *formats,
                               wxDataObjectBase::Direction WXUNUSED(dir) ) const
{
    *formats++ = wxDataFormat( wxDF_BITMAP );
#if wxMAC_USE_CORE_GRAPHICS
    *formats = s_pict;
#endif
}


bool wxBitmapDataObject::SetData( const wxDataFormat& format, size_t nSize, const void *pBuf )
{
    Clear();

    if ((pBuf == NULL) || (nSize == 0))
        return false;

#if wxMAC_USE_CORE_GRAPHICS
    Handle picHandle = NULL ;
    m_pictHandle = NewHandle( nSize );
    memcpy( *(Handle) m_pictHandle, pBuf, nSize );
    
    if ( format == s_pict )
    {
        // pict for IO expects a 512 byte header
        picHandle = NewHandle( nSize + 512 );
        memset( *picHandle , 0 , 512 );
        memcpy( *picHandle+512, pBuf, nSize );
    }
    else
    {
        picHandle = (Handle) m_pictHandle;
    }

    CGImageRef cgImageRef = 0;
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    if ( UMAGetSystemVersion() >= 0x1040 )
    {
        CFDataRef data = NULL; 
        
        HLock( picHandle );
        data = CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, (const UInt8*) *picHandle, GetHandleSize(picHandle), kCFAllocatorNull);
        CGImageSourceRef source = CGImageSourceCreateWithData( data, NULL );
        if ( source )
        {
            cgImageRef = CGImageSourceCreateImageAtIndex(source, 0, NULL);
        }
        CFRelease( source );
        CFRelease( data );
        HUnlock( picHandle );
    }
    else
#endif
#ifndef __LP64__
    {
        // import from TIFF
        GraphicsImportComponent importer = 0;
        OSStatus err = OpenADefaultComponent(GraphicsImporterComponentType, s_pict == format ? kQTFileTypePicture : kQTFileTypeTIFF, &importer);
        if (noErr == err)
        {
            if ( picHandle )
            {
                ComponentResult result = GraphicsImportSetDataHandle(importer, picHandle);
                if ( result == noErr )
                {
                    Rect frame;
                    GraphicsImportGetNaturalBounds( importer, &frame );
                    GraphicsImportCreateCGImage( importer, &cgImageRef, kGraphicsImportCreateCGImageUsingCurrentSettings );
                }
            }
            CloseComponent( importer );
        }
    }
#endif
    if ( format == s_pict )
    {
        DisposeHandle( picHandle );
    }
    if ( cgImageRef )
    {
        m_bitmap.Create( CGImageGetWidth(cgImageRef)  , CGImageGetHeight(cgImageRef) );
        CGRect r = CGRectMake( 0 , 0 , CGImageGetWidth(cgImageRef)  , CGImageGetHeight(cgImageRef) );
        // since our context is upside down we dont use CGContextDrawImage
        HIViewDrawCGImage( (CGContextRef) m_bitmap.GetHBITMAP() , &r, cgImageRef ) ;
        CGImageRelease(cgImageRef);
        cgImageRef = NULL;
    }
#else
    PicHandle picHandle = (PicHandle)NewHandle( nSize );
    memcpy( *picHandle, pBuf, nSize );
    m_pictHandle = picHandle;
    // ownership is transferred to the bitmap
    m_pictCreated = false;
#ifndef __LP64__
    Rect frame;
    wxMacGetPictureBounds( picHandle, &frame );
#if wxUSE_METAFILE
    wxMetafile mf;
    mf.SetHMETAFILE( (WXHMETAFILE)m_pictHandle );
#endif
    wxMemoryDC mdc;
    m_bitmap.Create( frame.right - frame.left, frame.bottom - frame.top );
    mdc.SelectObject( m_bitmap );
#if wxUSE_METAFILE  
    mf.Play( &mdc );
#endif
    mdc.SelectObject( wxNullBitmap );
#endif
#endif

    return m_bitmap.Ok();
}

#endif
