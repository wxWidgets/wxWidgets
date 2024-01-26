///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/dataobj.cpp
// Purpose:     implementation of wxDataObject class
// Author:      Stefan Csomor
// Created:     10/21/99
// Copyright:   (c) 1999 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAOBJ

#include "wx/dataobj.h"
#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/metafile.h"
#include "wx/scopedarray.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"

#include "wx/osx/private.h"
#include "wx/osx/private/datatransfer.h"

#include <memory>

static CFStringRef kUTTypeTraditionalMacText = CFSTR("com.apple.traditional-mac-plain-text");

static wxString privateUTIPrefix = "org.wxwidgets.private.";

// ----------------------------------------------------------------------------
// wxDataFormat
// ----------------------------------------------------------------------------

wxDataFormat::wxDataFormat()
{
    m_type = wxDF_INVALID;
}

wxDataFormat::wxDataFormat( wxDataFormatId vType )
{
    m_type = wxDF_INVALID;
    SetType( vType );
}

wxDataFormat::wxDataFormat( const wxChar *zId )
{
    m_type = wxDF_INVALID;
    SetId( zId );
}

wxDataFormat::wxDataFormat( const wxString& rId )
{
    m_type = wxDF_INVALID;
    SetId( rId );
}

wxDataFormat::wxDataFormat(const wxDataFormat& rFormat)
{
    m_format = rFormat.m_format;
    m_type = rFormat.m_type;
    m_id = rFormat.m_id;
}

wxDataFormat::wxDataFormat(NativeFormat format)
{
    SetId(format);
}

wxDataFormat::~wxDataFormat()
{
}

wxDataFormat& wxDataFormat::operator=(const wxDataFormat& rFormat)
{
    m_format = rFormat.m_format;
    m_type = rFormat.m_type;
    m_id = rFormat.m_id;
    return *this;
}

wxDataFormat::NativeFormat wxDataFormat::GetFormatForType(wxDataFormatId type)
{
    wxDataFormat::NativeFormat f = nullptr;
    switch (type)
    {
        case wxDF_TEXT:
            f = kUTTypeTraditionalMacText;
            break;
            
        case wxDF_UNICODETEXT:
#ifdef wxNEEDS_UTF8_FOR_TEXT_DATAOBJ
            f = kUTTypeUTF8PlainText;
#elif defined(wxNEEDS_UTF16_FOR_TEXT_DATAOBJ)
            f = kUTTypeUTF16PlainText;
#else
#error "one of wxNEEDS_UTF{8,16}_FOR_TEXT_DATAOBJ must be defined"
#endif
            break;
            
        case wxDF_HTML:
            f = kUTTypeHTML;
            break;
            
        case wxDF_BITMAP:
            f = kUTTypeTIFF;
            break;
            
        case wxDF_METAFILE:
            f = kUTTypePDF;
            break;
            
        case wxDF_FILENAME:
            f = kUTTypeFileURL;
            break;
            
        default:
            wxFAIL_MSG( wxS("unsupported data format") );
            break;
    }
    return f;
}

void wxDataFormat::SetType( wxDataFormatId dataType )
{
    m_type = dataType;
    m_format = wxCFRetain(GetFormatForType(dataType));
    m_id = wxCFStringRef( m_format ).AsString();
}

void wxDataFormat::AddSupportedTypesForSetting(CFMutableArrayRef types) const
{
    DoAddSupportedTypes(types, true);
}

void wxDataFormat::AddSupportedTypesForGetting(CFMutableArrayRef types) const
{
    DoAddSupportedTypes(types, false);
}

void wxDataFormat::DoAddSupportedTypes(CFMutableArrayRef cfarray, bool forSetting) const
{
    if ( GetType() == wxDF_PRIVATE )
    {
        CFArrayAppendValue(cfarray, GetFormatId());
    }
    else
    {
        CFArrayAppendValue(cfarray, GetFormatForType(m_type));
        if ( forSetting )
        {
            // add additional accepted types which we are ready to accept and can
            // convert to our internal formats
            switch (GetType())
            {
                case wxDF_UNICODETEXT:
                    CFArrayAppendValue(cfarray, kUTTypeUTF8PlainText);
                    break;
                case wxDF_FILENAME:
                    CFArrayAppendValue(cfarray, kPasteboardTypeFileURLPromise);
                    break;
                 default:
                    break;
            }
        }
    }
}

wxString wxDataFormat::GetId() const
{
    return m_id;
}

void wxDataFormat::SetId( NativeFormat format )
{
    m_format = wxCFRetain(format);
    m_id = wxCFStringRef( m_format ).AsString();
    if ( m_id.StartsWith(privateUTIPrefix) )
        m_id = m_id.Mid(privateUTIPrefix.length());

    if ( UTTypeConformsTo( (CFStringRef)format, kUTTypeHTML ) )
    {
        m_type = wxDF_HTML;
    }
    if (  UTTypeConformsTo( (CFStringRef)format, kUTTypeUTF16PlainText ) )
    {
        m_type = wxDF_UNICODETEXT;
    }
    else if (  UTTypeConformsTo( (CFStringRef)format,kUTTypeUTF16ExternalPlainText ) )
    {
        m_type = wxDF_UNICODETEXT;
    }
    else if (  UTTypeConformsTo( (CFStringRef)format,kUTTypeUTF8PlainText ) )
    {
        m_type = wxDF_UNICODETEXT;
    }
    else if ( UTTypeConformsTo( (CFStringRef)format, kUTTypePlainText ) )
    {
        m_type = wxDF_TEXT;
    }
    else if (  UTTypeConformsTo( (CFStringRef)format, kUTTypeImage ) )
    {
        m_type = wxDF_BITMAP;
    }
    else if (  UTTypeConformsTo( (CFStringRef)format, kUTTypePDF ) )
    {
        m_type = wxDF_METAFILE;
    }
    else if (  UTTypeConformsTo( (CFStringRef)format, kUTTypeFileURL ) ||
             UTTypeConformsTo( (CFStringRef)format, kPasteboardTypeFileURLPromise))
    {
        m_type = wxDF_FILENAME;
    }
    else
    {
        m_type = wxDF_PRIVATE;
    }
}

void wxDataFormat::SetId( const wxString& zId )
{
    m_type = wxDF_PRIVATE;
    // in newer macOS version this must conform to a UTI
    // https://developer.apple.com/library/archive/documentation/General/Conceptual/DevPedia-CocoaCore/UniformTypeIdentifier.html

    // first filter characters
    wxString utiString = zId;
    wxString::iterator it;
    for (it = utiString.begin(); it != utiString.end(); ++it)
    {
        wxUniChar c = *it;
        if ( !( c >= 'A' && c <='Z') && !( c >= 'a' && c <='z') && !( c >= '0' && c <='9') &&
            c != '.' && c !='-' )
            *it= '-';
    }

    m_id = utiString;

    // make sure it follows a reverse DNS notation
    if ( utiString.Find('.') != wxNOT_FOUND )
        m_format = wxCFStringRef(utiString);
    else
        m_format = wxCFStringRef(privateUTIPrefix+utiString);
}

bool wxDataFormat::operator==(const wxDataFormat& format) const
{
    if (IsStandard() || format.IsStandard())
        return (format.m_type == m_type);
    else
        return ( UTTypeConformsTo( m_format , (CFStringRef) format.m_format ) );
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
        wxScopedArray<wxDataFormat> array(nFormatCount);
        GetAllFormats( array.get(), vDir );

        for (size_t n = 0; n < nFormatCount; n++)
        {
            if (array[n] == rFormat)
            {
                found = true;
                break;
            }
        }
    }

    return found;
}

void wxDataObject::WriteToSink(wxOSXDataSink * datatransfer) const
{
   // get formats from wxDataObjects
    wxScopedArray<wxDataFormat> array(GetFormatCount());
    GetAllFormats( array.get() );

    wxOSXDataSinkItem* sinkItem = nullptr;

    for (size_t i = 0; i < GetFormatCount(); i++)
    {
        wxDataFormat thisFormat = array[ i ];

        size_t datasize = GetDataSize( thisFormat );
        if ( datasize == wxCONV_FAILED && thisFormat.GetType() == wxDF_TEXT)
        {
            // conversion to local text failed, so we must use unicode
            // if wxDF_UNICODETEXT is already on the 'todo' list, skip this iteration
            // otherwise force it
            bool hasUnicodeFormat = false;
            for (size_t j = 0; j < GetFormatCount(); j++)
            {
                if ( array[j].GetType() == wxDF_UNICODETEXT )
                {
                    hasUnicodeFormat = true;
                    break;
                }
            }
            if ( hasUnicodeFormat )
                continue;

            thisFormat.SetType(wxDF_UNICODETEXT);
            datasize = GetDataSize( thisFormat );
        }

        // add four bytes at the end for data objs like text that
        // have a datasize = strlen but still need a buffer for the
        // string including trailing zero

        size_t sz = datasize + 4;
        wxMemoryBuffer databuf( datasize+4 );
        void* buf = databuf.GetWriteBuf(datasize+4);
        if ( buf != nullptr )
        {
            // empty the buffer because in some case GetDataHere does not fill buf
            memset( buf, 0, sz );
            bool datavalid = GetDataHere( thisFormat, buf );
            databuf.UngetWriteBuf(datasize);
            if ( datavalid )
            {
                if ( !sinkItem )
                    sinkItem = datatransfer->CreateItem();

                if ( thisFormat.GetType() == wxDF_FILENAME )
                {
                    wxString filenames;
                    
                    filenames = wxString( (const char*)buf, *wxConvFileName );
                    wxArrayString files = wxStringTokenize( filenames, wxT("\n"), wxTOKEN_STRTOK );

                    for ( size_t j = 0 ; j < files.GetCount(); ++j )
                    {
                        sinkItem->SetFilename(files[j]);
                        // if there is another filepath, macOS needs another item
                       if ( j + 1 < files.GetCount() )
                            sinkItem = datatransfer->CreateItem();
                    }
                }
                else
                {
                    sinkItem->SetData(thisFormat.GetFormatId(), databuf.GetData(), databuf.GetDataLen());
                }
            }
        }
    }
}

bool wxDataObject::ReadFromSource(wxDataObject * source)
{
    bool transferred = false;
    
    size_t formatcount = source->GetFormatCount();
    wxScopedArray<wxDataFormat> array(formatcount);
    source->GetAllFormats( array.get() );
    for (size_t i = 0; !transferred && i < formatcount; i++)
    {
        wxDataFormat format = array[i];
        if ( IsSupported( format, wxDataObject::Set ) )
        {
            int size = source->GetDataSize( format );
            transferred = true;
            
            if (size == 0)
            {
                SetData( format, 0, nullptr );
            }
            else
            {
                wxCharBuffer d(size);
                source->GetDataHere( format, d.data() );
                SetData( format, size, d.data() );
            }
        }
    }
    return transferred;
}

bool wxDataObject::ReadFromSource(wxOSXDataSource * source)
{
    bool transferred = false;

    size_t formatcount = GetFormatCount(wxDataObject::Set);
    wxScopedArray<wxDataFormat> array(formatcount);
    GetAllFormats(array.get(), wxDataObject::Set);

    wxString filenamesPassed;

    for (size_t i = 0; !transferred && i < formatcount; i++)
    {
        // go through the data in our order of preference
        wxDataFormat dataFormat = array[i];

        if (source->IsSupported(dataFormat))
        {
            wxCFMutableArrayRef<CFStringRef> typesarray;
            dataFormat.AddSupportedTypesForSetting(typesarray);
            size_t itemCount = source->GetItemCount();
            
            for ( size_t itemIndex = 0; itemIndex < itemCount && !transferred; ++itemIndex)
            {
                std::unique_ptr<const wxOSXDataSourceItem> sitem(source->GetItem(itemIndex));
                
                wxDataFormat::NativeFormat nativeFormat = sitem->AvailableType(typesarray);
                CFDataRef flavorData = sitem->DoGetData(nativeFormat);
                if (flavorData)
                {
                    CFIndex flavorDataSize = CFDataGetLength(flavorData);
                    size_t sz = flavorDataSize + 4;
                    wxMemoryBuffer databuf( sz );
                    void* buf = databuf.GetWriteBuf( sz );
                    if (buf)
                    {
                        memset(buf, 0, sz);
                        memcpy(buf, CFDataGetBytePtr(flavorData), flavorDataSize);
                        databuf.UngetWriteBuf(flavorDataSize);

                        if (nativeFormat != dataFormat.GetFormatId())
                        {
                            // data is accepted but needs conversion

                            // UTF8 data to our UTF16
                            if (dataFormat.GetType() == wxDF_UNICODETEXT && CFStringCompare(nativeFormat, kUTTypeUTF8PlainText, 0) == 0)
                            {
                                wxMBConvUTF16 UTF16Converter;

                                wxString s((char*)buf, wxConvUTF8, flavorDataSize);

                                const wxCharBuffer cb = s.mb_str(UTF16Converter);
                                flavorDataSize = cb.length();
                                sz = flavorDataSize + 2;
                                buf = databuf.GetWriteBuf(sz);
                                memset(buf, 0, sz);
                                memcpy(buf, cb.data(), flavorDataSize);
                                databuf.UngetWriteBuf(flavorDataSize);
                            }
                        }
                        
                        if (dataFormat.GetType() == wxDF_FILENAME)
                        {
                            // revert the translation and decomposition to arrive at a proper utf8 string again
                            
                            wxCFRef<CFURLRef> url = CFURLCreateWithBytes(kCFAllocatorDefault, (UInt8*) buf, flavorDataSize, kCFStringEncodingUTF8, nullptr);
                            wxCFStringRef cfString = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
                                
                            CFMutableStringRef cfMutableString = CFStringCreateMutableCopy(nullptr, 0, cfString);
                            CFStringNormalize(cfMutableString, kCFStringNormalizationFormC);
                            // cfMutableString is released by the wxCFStringRef
                            wxString path = wxCFStringRef(cfMutableString).AsString();
                            if (!path.empty())
                                filenamesPassed += path + wxS("\n");
                            
                            // if it's the last item, we set the wx data
                            if ( itemIndex + 1 == itemCount )
                            {
                                wxCharBuffer filenamebuf = filenamesPassed.char_str();
                                SetData(dataFormat, filenamebuf.length(), filenamebuf.data());
                                transferred = true;
                            }
                        }
                        else
                        {
                            // because some data implementation expect trailing a trailing NUL, we add some headroom
                            
                            if (dataFormat.GetType() == wxDF_TEXT)
                            {
                                for (char* p = static_cast<char*>(buf); *p; p++)
                                    if (*p == '\r')
                                        *p = '\n';
                            }
                            else if (dataFormat.GetType() == wxDF_UNICODETEXT)
                            {
                                for (wxChar16* p = static_cast<wxChar16*>(buf); *p; p++)
                                    if (*p == '\r')
                                        *p = '\n';
                            }
                            SetData(dataFormat, flavorDataSize, buf);
                            // multiple items are not supported in wx, set flag to true
                            transferred = true;
                        }
                    }
                }
            }
        }
    }
    return transferred;
}

wxDataFormat wxDataObject::GetSupportedFormatInSource(wxDataObject *source) const
{
    wxDataFormat format;
    size_t formatcount = source->GetFormatCount();
    wxScopedArray<wxDataFormat> array(formatcount);
    
    source->GetAllFormats( array.get() );
    for (size_t i = 0; i < formatcount; i++)
    {
        wxDataFormat testFormat = array[i];
        if ( IsSupported( testFormat, wxDataObject::Set ) )
        {
            format = testFormat;
            break;
        }
    }
    return format;
}


wxDataFormat wxDataObject::GetSupportedFormatInSource(wxOSXDataSource *source) const
{
    wxDataFormat format;
    
    size_t formatcount = GetFormatCount(wxDataObject::Set);
    wxScopedArray<wxDataFormat> array(formatcount);
    GetAllFormats(array.get(), wxDataObject::Set);
    
    for (size_t i = 0; i < formatcount; i++)
    {
        // go through the data in our order of preference
        wxDataFormat dataFormat = array[i];
        
        if (source->IsSupported(dataFormat))
        {
            format = dataFormat;
            break;
        }
    }
    
    return format;
}

bool wxDataObject::CanReadFromSource( wxOSXDataSource * source ) const
{
    return GetSupportedFormatInSource(source) != wxDF_INVALID;
}

bool wxDataObject::CanReadFromSource( wxDataObject * source ) const
{
    return GetSupportedFormatInSource(source) != wxDF_INVALID;
}

void wxDataObject::AddSupportedTypes( CFMutableArrayRef cfarray, Direction dir) const
{
    size_t nFormats = GetFormatCount(dir);
    wxScopedArray<wxDataFormat> array(nFormats);
    GetAllFormats(array.get(), dir);

    for (size_t i = 0; i < nFormats; i++)
    {
        if ( dir == Direction::Get)
            array[i].AddSupportedTypesForGetting(cfarray);
        else
            array[i].AddSupportedTypesForSetting(cfarray);
    }
}

// ----------------------------------------------------------------------------
// wxTextDataObject
// ----------------------------------------------------------------------------

void wxTextDataObject::GetAllFormats(wxDataFormat *formats,
                                     wxDataObjectBase::Direction WXUNUSED(dir)) const
{
    *formats++ = wxDataFormat(wxDF_UNICODETEXT);
    *formats = wxDataFormat(wxDF_TEXT);
}

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
    if (pBuf == nullptr)
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

bool wxFileDataObject::SetData( size_t WXUNUSED(nSize), const void *pBuf )
{
    wxString filenames;

    filenames = wxString( (const char*)pBuf, *wxConvFileName );

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

    if (m_bitmap.IsOk())
    {
        SetBitmap( rBitmap );
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
    if (m_bitmap.IsOk())
    {
        CGImageRef cgImageRef = (CGImageRef) m_bitmap.CreateCGImage();

        CFMutableDataRef data = CFDataCreateMutable(kCFAllocatorDefault, 0);
        CGImageDestinationRef destination = CGImageDestinationCreateWithData( data , kUTTypeTIFF , 1 , nullptr );
        if ( destination )
        {
            CGImageDestinationAddImage( destination, cgImageRef, nullptr );
            CGImageDestinationFinalize( destination );
            CFRelease( destination );
        }
        m_pictData = data;

        CGImageRelease(cgImageRef);
    }
}

void wxBitmapDataObject::Init()
{
    m_pictData = nullptr;
}

void wxBitmapDataObject::Clear()
{
    if (m_pictData != nullptr)
    {
        CFRelease( m_pictData );
        m_pictData = nullptr;
    }
}

bool wxBitmapDataObject::GetDataHere( void *pBuf ) const
{
    if (m_pictData == nullptr)
    {
        wxFAIL_MSG( wxT("attempt to copy empty bitmap failed") );
        return false;
    }

    if (pBuf == nullptr)
        return false;

    memcpy( pBuf, (const char *)CFDataGetBytePtr(m_pictData), CFDataGetLength(m_pictData) );

    return true;
}

size_t wxBitmapDataObject::GetDataSize() const
{
    if (m_pictData != nullptr)
        return CFDataGetLength(m_pictData);
    else
        return 0;
}

bool wxBitmapDataObject::SetData( size_t nSize, const void *pBuf )
{
    Clear();

    if ((pBuf == nullptr) || (nSize == 0))
        return false;

    CGImageRef cgImageRef = nullptr;

    CFDataRef data = CFDataCreate( kCFAllocatorDefault, (const UInt8*) pBuf, nSize);
    CGImageSourceRef source = CGImageSourceCreateWithData( data, nullptr );
    if ( source )
    {
        cgImageRef = CGImageSourceCreateImageAtIndex(source, 0, nullptr);
        CFRelease( source );
    }
    m_pictData = data;

    if ( cgImageRef )
    {
        m_bitmap.Create( cgImageRef );
        CGImageRelease(cgImageRef);
        cgImageRef = nullptr;
    }

    return m_bitmap.IsOk();
}

#endif
