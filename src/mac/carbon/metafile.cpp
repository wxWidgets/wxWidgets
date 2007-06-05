/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/metafile.cpp
// Purpose:     wxMetaFile, wxMetaFileDC etc. These classes are optional.
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////
//
// Currently, the only purpose for making a metafile
// is to put it on the clipboard.


#include "wx/wxprec.h"

#if wxUSE_METAFILE

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
#endif

#include "wx/metafile.h"
#include "wx/clipbrd.h"
#include "wx/mac/private.h"
#include "wx/graphics.h"

#include <stdio.h>
#include <string.h>

// NB: The new implementation added in 1.34.4.2 uses a number of APIs which 
// were introduced in Tiger. Unfortunately, this also left the class with
// no implementation for Panther+CoreGraphics (which wxPython currently uses). 
// I've restored compilation for Panther+CoreGraphics but this does little more
// than make it compile. It does not show Metafiles and
#define USE_CG_METAFILE (wxMAC_USE_CORE_GRAPHICS && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4))

IMPLEMENT_DYNAMIC_CLASS(wxMetafile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetafileDC, wxDC)

#define M_METAFILEREFDATA( a ) ((wxMetafileRefData*)(a).GetRefData())

class wxMetafileRefData: public wxGDIRefData
{
public:
#if USE_CG_METAFILE
    // creates a metafile from memory, assumes ownership
    wxMetafileRefData(CFDataRef data);    
#else
    // creates a metafile from memory, assumes ownership
    wxMetafileRefData(PicHandle data);    
#endif
    // prepares a recording metafile
    wxMetafileRefData( int width, int height);    
    // prepares a metafile to be read from a file (if filename is not empty)
    wxMetafileRefData( const wxString& filename);    
    virtual ~wxMetafileRefData();
    
    void Init();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
#if USE_CG_METAFILE
    CGPDFDocumentRef GetPDFDocument() const { return m_pdfDoc; }
    void UpdateDocumentFromData() ;

    const wxCFDataRef& GetData() const { return m_data; }
    CGContextRef GetContext() const { return m_context; }
#else
    PicHandle GetHandle() const { return m_metafile; }
#endif
    // ends the recording
    void Close();
private:
#if USE_CG_METAFILE
    wxCFDataRef m_data;
    wxCFRef<CGPDFDocumentRef> m_pdfDoc;
    CGContextRef m_context;
#else
    PicHandle m_metafile;
#endif
    int m_width ;
    int m_height ;
};

#if! USE_CG_METAFILE
wxMetafileRefData::wxMetafileRefData(PicHandle pict)
{
    Init();
    m_metafile = pict;

    Rect r;
    wxMacGetPictureBounds( m_metafile, &r );
    m_width = r.right - r.left;
    m_height = r.bottom - r.top;
}
#else
wxMetafileRefData::wxMetafileRefData(CFDataRef data) :
    m_data(data)
{
    Init();
    UpdateDocumentFromData();
}
#endif

wxMetafileRefData::wxMetafileRefData( const wxString& filename )
{
    Init();
#if USE_CG_METAFILE
    if ( !filename.empty() )
    {
        wxCFRef<CFMutableStringRef> cfMutableString(CFStringCreateMutableCopy(NULL, 0, wxMacCFStringHolder(filename)));
        CFStringNormalize(cfMutableString,kCFStringNormalizationFormD);
        wxCFRef<CFURLRef> url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfMutableString , kCFURLPOSIXPathStyle, false));
        m_pdfDoc.reset(CGPDFDocumentCreateWithURL(url));
    }
#else
    wxASSERT_MSG( filename.empty(), wxT("no file-based metafile support yet") );
    m_metafile = NULL;
#endif
}


wxMetafileRefData::wxMetafileRefData( int width, int height)
{
    Init();
    
    m_width = width;
    m_height = height;
#if USE_CG_METAFILE
    CGRect r = CGRectMake( 0 , 0 , width  , height );

    CFMutableDataRef data = CFDataCreateMutable(kCFAllocatorDefault, 0);
    m_data.reset(data);
    CGDataConsumerRef dataConsumer = CGDataConsumerCreateWithCFData(data);
    m_context = CGPDFContextCreate( dataConsumer, (width != 0 && height != 0) ? &r : NULL , NULL );
    CGDataConsumerRelease( dataConsumer );
    if ( m_context )
    {
        if ( &CGPDFContextBeginPage != NULL )
            CGPDFContextBeginPage(m_context, NULL);
        else
            CGContextBeginPage(m_context, NULL);

        CGColorSpaceRef genericColorSpace  = wxMacGetGenericRGBColorSpace();

        CGContextSetFillColorSpace( m_context, genericColorSpace );
        CGContextSetStrokeColorSpace( m_context, genericColorSpace );

        CGContextTranslateCTM( m_context , 0 ,  height ) ;
        CGContextScaleCTM( m_context , 1 , -1 ) ;
    }
#else
    Rect r = { 0, 0, height, width };
    m_metafile = OpenPicture( &r ) ;
#endif
}

wxMetafileRefData::~wxMetafileRefData()
{
#if! USE_CG_METAFILE
    if (m_metafile)
    {
        KillPicture( (PicHandle)m_metafile );
        m_metafile = NULL;
    }
#endif
}

void wxMetafileRefData::Init()
{
#if USE_CG_METAFILE
    m_context = NULL;
#else
    m_metafile = NULL;
#endif
    m_width = -1;
    m_height = -1;
}

void wxMetafileRefData::Close()
{
#if USE_CG_METAFILE
    if ( &CGPDFContextEndPage != NULL )
        CGPDFContextEndPage(m_context);
    else
        CGContextEndPage(m_context);

    CGContextRelease(m_context);
    m_context = NULL;
    
    UpdateDocumentFromData();
#else
    ClosePicture();
#endif
}

#if USE_CG_METAFILE
void wxMetafileRefData::UpdateDocumentFromData() 
{
    wxCFRef<CGDataProviderRef> provider(CGDataProviderCreateWithCFData(m_data));
    m_pdfDoc.reset(CGPDFDocumentCreateWithProvider(provider));
    if ( m_pdfDoc != NULL )
    {
        CGPDFPageRef page = CGPDFDocumentGetPage( m_pdfDoc, 1 );
        CGRect rect = CGPDFPageGetBoxRect ( page, kCGPDFMediaBox);
        m_width = rect.size.width;
        m_height = rect.size.height;
    }
}
#endif

wxMetaFile::wxMetaFile(const wxString& file)
{
    m_refData = new wxMetafileRefData(file);
}

wxMetaFile::~wxMetaFile()
{
}

bool wxMetaFile::IsOk() const
{
#if USE_CG_METAFILE
    return (M_METAFILEDATA && (M_METAFILEDATA->GetData() != NULL));
#else
    return (M_METAFILEDATA && (M_METAFILEDATA->GetHandle() != NULL));
#endif
}

WXHMETAFILE wxMetaFile::GetHMETAFILE() const
{
#if USE_CG_METAFILE
    return (WXHMETAFILE) (CFDataRef) M_METAFILEDATA->GetData();
#else
    return (WXHMETAFILE) M_METAFILEDATA->GetHandle();
#endif
}

bool wxMetaFile::SetClipboard(int width, int height)
{
    bool success = true;

#if wxUSE_DRAG_AND_DROP
    if (m_refData == NULL)
        return false;

    bool alreadyOpen = wxTheClipboard->IsOpened();
    if (!alreadyOpen)
    {
        wxTheClipboard->Open();
        wxTheClipboard->Clear();
    }

    wxDataObject *data = new wxMetafileDataObject( *this );
    success = wxTheClipboard->SetData( data );
    if (!alreadyOpen)
        wxTheClipboard->Close();
#endif

    return success;
}

void wxMetafile::SetHMETAFILE(WXHMETAFILE mf)
{
    UnRef();

#if USE_CG_METAFILE
    m_refData = new wxMetafileRefData((CFDataRef)mf);
#else
    m_refData = new wxMetafileRefData((PicHandle)mf);
#endif
}

bool wxMetaFile::Play(wxDC *dc)
{
    if (!m_refData)
        return false;

    if (!dc->Ok())
        return false;

    {
#if USE_CG_METAFILE
        CGContextRef cg = (CGContextRef) dc->GetGraphicsContext()->GetNativeContext();
        CGPDFDocumentRef doc = M_METAFILEDATA->GetPDFDocument();
        CGPDFPageRef page = CGPDFDocumentGetPage( doc, 1 );
        wxMacCGContextStateSaver save(cg);
        CGContextDrawPDFPage( cg, page );
//        CGContextTranslateCTM( cg, 0, bounds.size.width );
//        CGContextScaleCTM( cg, 1, -1 );
#elif wxMAC_USE_CORE_GRAPHICS
        // FIXME: Please check the implementation, this is based solely on
        // the original CoreGraphics impl and I'm not sure if this is right.
        PicHandle pict = (PicHandle)GetHMETAFILE();
        HLock((Handle)pict);
        CGDataProviderRef dataProvider = CGDataProviderCreateWithData ( NULL, *pict,
                                  GetHandleSize((Handle)pict), NULL );
        QDPictRef cgPictRef = QDPictCreateWithProvider(dataProvider);
        CGContextRef cg = (CGContextRef) dc->GetGraphicsContext()->GetNativeContext();
        CGRect bounds = QDPictGetBounds( cgPictRef ) ;

        CGContextSaveGState(cg);
        CGContextTranslateCTM(cg, 0 , bounds.size.width );
        CGContextScaleCTM(cg, 1, -1);
        QDPictDrawToCGContext( cg , bounds , cgPictRef ) ;
        CGContextRestoreGState( cg ) ;
        
        QDPictRelease(cgPictRef);
        CGDataProviderRelease(dataProvider);
 #else
        // FIXME: This code doesn't work for the case where wxMAC_USE_CORE_GRAPHICS
        // is used. However, the old CG code used QDPictRef which is gone from the
        // current code. 
        PicHandle pict = (PicHandle)GetHMETAFILE();
        wxMacPortSetter helper( dc );
        Rect picFrame;
        DrawPicture( pict, wxMacGetPictureBounds( pict, &picFrame ) );
#endif
    }

    return true;
}

wxSize wxMetaFile::GetSize() const
{
    wxSize dataSize = wxDefaultSize;

    if (Ok())
    {
        dataSize.x = M_METAFILEDATA->GetWidth();
        dataSize.y = M_METAFILEDATA->GetHeight();
    }

    return dataSize;
}

// Metafile device context

// New constructor that takes origin and extent. If you use this, don't
// give origin/extent arguments to wxMakeMetaFilePlaceable.

wxMetaFileDC::wxMetaFileDC(
    const wxString& filename,
    int width, int height,
    const wxString& WXUNUSED(description) )
{
    wxASSERT_MSG( width != 0 || height != 0, wxT("no arbitration of metafile size supported") );
    wxASSERT_MSG( filename.empty(), wxT("no file based metafile support yet"));

    m_metaFile = new wxMetaFile( filename );
    wxMetafileRefData* metafiledata = new wxMetafileRefData(width, height);
    m_metaFile->UnRef();
    m_metaFile->SetRefData( metafiledata );
#if USE_CG_METAFILE
    SetGraphicsContext( wxGraphicsContext::CreateFromNative(metafiledata->GetContext()));
    m_ok = (m_graphicContext != NULL) ;
#elif wxMAC_USE_CORE_GRAPHICS
    // we don't do anything here for now
#else
    Rect r = { 0, 0, height, width };
    RectRgn( (RgnHandle)m_macBoundaryClipRgn, &r );
    CopyRgn( (RgnHandle)m_macBoundaryClipRgn, (RgnHandle)m_macCurrentClipRgn );
    ::GetPort( (GrafPtr*)&m_macPort );
    m_ok = true;
#endif

    SetMapMode( wxMM_TEXT );
}

wxMetaFileDC::~wxMetaFileDC()
{
}

void wxMetaFileDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_metaFile, wxT("GetSize() doesn't work without a metafile") );

    wxSize sz = m_metaFile->GetSize();
    if (width)
        (*width) = sz.x;
    if (height)
        (*height) = sz.y;
}

wxMetaFile *wxMetaFileDC::Close()
{
#if USE_CG_METAFILE
    delete m_graphicContext;
    m_graphicContext = NULL;
    m_ok = false;
#endif

    M_METAFILEREFDATA(*m_metaFile)->Close();

    return m_metaFile;
}

#if wxUSE_DATAOBJ
size_t wxMetafileDataObject::GetDataSize() const
{
#if USE_CG_METAFILE
    CFIndex length = 0;
    wxMetafileRefData* refData = M_METAFILEREFDATA(m_metafile);
    if ( refData )
        length = refData->GetData().GetLength();
    return length;
#else
    return GetHandleSize( (Handle) (*((wxMetafile*)&m_metafile)).GetHMETAFILE() );
#endif
}

bool wxMetafileDataObject::GetDataHere(void *buf) const
{
    bool result = false;
#if USE_CG_METAFILE
    wxMetafileRefData* refData = M_METAFILEREFDATA(m_metafile);
    if ( refData )
    {   
        CFIndex length = refData->GetData().GetLength();
        if ( length > 0 )
        {
            result = true ;
            refData->GetData().GetBytes(CFRangeMake(0,length), (UInt8 *) buf);
        }
    } 
#else
    Handle pictH = (Handle)(*((wxMetafile*)&m_metafile)).GetHMETAFILE();
    result = (pictH != NULL);

    if (result)
        memcpy( buf, *pictH, GetHandleSize( pictH ) );

#endif
    return result;
}

bool wxMetafileDataObject::SetData(size_t len, const void *buf)
{
#if USE_CG_METAFILE
    wxMetafileRefData* metafiledata = new wxMetafileRefData(wxCFRefFromGet(wxCFDataRef((UInt8*)buf, len).get()));
    m_metafile.UnRef();
    m_metafile.SetRefData( metafiledata );
#else
    Handle handle = NewHandle( len );
    SetHandleSize( handle, len );
    memcpy( *handle, buf, len );
    m_metafile.SetHMETAFILE( (WXHMETAFILE) handle );
#endif
    return true;
}
#endif

#endif
