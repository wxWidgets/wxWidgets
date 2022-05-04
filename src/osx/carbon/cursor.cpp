/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/cursor.cpp
// Purpose:     wxCursor class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif // WX_PRECOMP

#include "wx/xpmdecod.h"

#include "wx/osx/private.h"


wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxGDIObject);


class WXDLLEXPORT wxCursorRefData: public wxGDIRefData
{
public:
    wxCursorRefData();
    wxCursorRefData(const wxCursorRefData& cursor);
    virtual ~wxCursorRefData();

    virtual bool IsOk() const wxOVERRIDE
    {
#if wxOSX_USE_COCOA_OR_CARBON
        if ( m_hCursor != NULL )
            return true;

        return false;
#else
        // in order to avoid asserts, always claim to have a valid cursor
        return true;
#endif
    }

protected:
#if wxOSX_USE_COCOA
    WX_NSCursor m_hCursor;
#elif wxOSX_USE_IPHONE
    void*       m_hCursor;
#endif

    friend class wxCursor;

    wxDECLARE_NO_ASSIGN_CLASS(wxCursorRefData);
};

#define M_CURSORDATA static_cast<wxCursorRefData*>(m_refData)

#if wxOSX_USE_COCOA_OR_CARBON

ClassicCursor gMacCursors[kwxCursorLast+1] =
{

{
{0x0000, 0x03E0, 0x0630, 0x0808, 0x1004, 0x31C6, 0x2362, 0x2222,
0x2362, 0x31C6, 0x1004, 0x0808, 0x0630, 0x03E0, 0x0000, 0x0000},
{0x0000, 0x03E0, 0x07F0, 0x0FF8, 0x1FFC, 0x3FFE, 0x3FFE, 0x3FFE,
0x3FFE, 0x3FFE, 0x1FFC, 0x0FF8, 0x07F0, 0x03E0, 0x0000, 0x0000},
{0x0007, 0x0008}
},

{
{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
{0x0000, 0x0000}
},

{
{0x00F0, 0x0088, 0x0108, 0x0190, 0x0270, 0x0220, 0x0440, 0x0440,
0x0880, 0x0880, 0x1100, 0x1E00, 0x1C00, 0x1800, 0x1000, 0x0000},
{0x00F0, 0x00F8, 0x01F8, 0x01F0, 0x03F0, 0x03E0, 0x07C0, 0x07C0,
0x0F80, 0x0F80, 0x1F00, 0x1E00, 0x1C00, 0x1800, 0x1000, 0x0000},
{0x000E, 0x0003}
},

{
{0x0000, 0x1E00, 0x2100, 0x4080, 0x4080, 0x4080, 0x4080, 0x2180,
0x1FC0, 0x00E0, 0x0070, 0x0038, 0x001C, 0x000E, 0x0006, 0x0000},
{0x3F00, 0x7F80, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0x7FC0,
0x3FE0, 0x1FF0, 0x00F8, 0x007C, 0x003E, 0x001F, 0x000F, 0x0007},
{0x0004, 0x0004}
},

{
{0x0000, 0x07E0, 0x1FF0, 0x3838, 0x3C0C, 0x6E0E, 0x6706, 0x6386,
0x61C6, 0x60E6, 0x7076, 0x303C, 0x1C1C, 0x0FF8, 0x07E0, 0x0000},
{0x0540, 0x0FF0, 0x3FF8, 0x3C3C, 0x7E0E, 0xFF0F, 0x6F86, 0xE7C7,
0x63E6, 0xE1F7, 0x70FE, 0x707E, 0x3C3C, 0x1FFC, 0x0FF0, 0x0540},
{0x0007, 0x0007}
},

{
{0x0000, 0x0380, 0x0380, 0x0380, 0x0380, 0x0380, 0x0380, 0x0FE0,
0x1FF0, 0x1FF0, 0x0000, 0x1FF0, 0x1FF0, 0x1550, 0x1550, 0x1550},
{0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x0FE0, 0x1FF0,
0x3FF8, 0x3FF8, 0x3FF8, 0x3FF8, 0x3FF8, 0x3FF8, 0x3FF8, 0x3FF8},
{0x000B, 0x0007}
},

{
{0x00C0, 0x0140, 0x0640, 0x08C0, 0x3180, 0x47FE, 0x8001, 0x8001,
0x81FE, 0x8040, 0x01C0, 0x0040, 0x03C0, 0xC080, 0x3F80, 0x0000},
{0x00C0, 0x01C0, 0x07C0, 0x0FC0, 0x3F80, 0x7FFE, 0xFFFF, 0xFFFF,
0xFFFE, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFF80, 0x3F80, 0x0000},
{0x0006, 0x000F}
},

{
{0x0100, 0x0280, 0x0260, 0x0310, 0x018C, 0x7FE3, 0x8000, 0x8000,
0x7F80, 0x0200, 0x0380, 0x0200, 0x03C0, 0x0107, 0x01F8, 0x0000},
{0x0100, 0x0380, 0x03E0, 0x03F0, 0x01FC, 0x7FFF, 0xFFFF, 0xFFFF,
0xFFFF, 0x03FF, 0x03FF, 0x03FF, 0x03FF, 0x01FF, 0x01F8, 0x0000},
{0x0006, 0x0000}
},

{
{0x0000, 0x4078, 0x60FC, 0x71CE, 0x7986, 0x7C06, 0x7E0E, 0x7F1C,
0x7FB8, 0x7C30, 0x6C30, 0x4600, 0x0630, 0x0330, 0x0300, 0x0000},
{0xC078, 0xE0FC, 0xF1FE, 0xFBFF, 0xFFCF, 0xFF8F, 0xFF1F, 0xFFBE,
0xFFFC, 0xFE78, 0xFF78, 0xEFF8, 0xCFF8, 0x87F8, 0x07F8, 0x0300},
{0x0001, 0x0001}
},

{
{0x0000, 0x0002, 0x0006, 0x000E, 0x001E, 0x003E, 0x007E, 0x00FE,
0x01FE, 0x003E, 0x0036, 0x0062, 0x0060, 0x00C0, 0x00C0, 0x0000},
{0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF,
0x03FF, 0x07FF, 0x007F, 0x00F7, 0x00F3, 0x01E1, 0x01E0, 0x01C0},
{0x0001, 0x000E}
},

{
{0x0000, 0x0080, 0x01C0, 0x03E0, 0x0080, 0x0080, 0x0080, 0x1FFC,
0x1FFC, 0x0080, 0x0080, 0x0080, 0x03E0, 0x01C0, 0x0080, 0x0000},
{0x0080, 0x01C0, 0x03E0, 0x07F0, 0x0FF8, 0x01C0, 0x3FFE, 0x3FFE,
0x3FFE, 0x3FFE, 0x01C0, 0x0FF8, 0x07F0, 0x03E0, 0x01C0, 0x0080},
{0x0007, 0x0008}
},

{
{0x0000, 0x0080, 0x01C0, 0x03E0, 0x0080, 0x0888, 0x188C, 0x3FFE,
0x188C, 0x0888, 0x0080, 0x03E0, 0x01C0, 0x0080, 0x0000, 0x0000},
{0x0080, 0x01C0, 0x03E0, 0x07F0, 0x0BE8, 0x1DDC, 0x3FFE, 0x7FFF,
0x3FFE, 0x1DDC, 0x0BE8, 0x07F0, 0x03E0, 0x01C0, 0x0080, 0x0000},
{0x0007, 0x0008}
},

{
{0x0000, 0x001E, 0x000E, 0x060E, 0x0712, 0x03A0, 0x01C0, 0x00E0,
0x0170, 0x1238, 0x1C18, 0x1C00, 0x1E00, 0x0000, 0x0000, 0x0000},
{0x007F, 0x003F, 0x0E1F, 0x0F0F, 0x0F97, 0x07E3, 0x03E1, 0x21F0,
0x31F8, 0x3A7C, 0x3C3C, 0x3E1C, 0x3F00, 0x3F80, 0x0000, 0x0000},
{0x0006, 0x0009}
},

{
{0x0000, 0x7800, 0x7000, 0x7060, 0x48E0, 0x05C0, 0x0380, 0x0700,
0x0E80, 0x1C48, 0x1838, 0x0038, 0x0078, 0x0000, 0x0000, 0x0000},
{0xFE00, 0xFC00, 0xF870, 0xF0F0, 0xE9F0, 0xC7E0, 0x87C0, 0x0F84,
0x1F8C, 0x3E5C, 0x3C3C, 0x387C, 0x00FC, 0x01FC, 0x0000, 0x0000},
{0x0006, 0x0006}
},

{
{0x0006, 0x000E, 0x001C, 0x0018, 0x0020, 0x0040, 0x00F8, 0x0004,
0x1FF4, 0x200C, 0x2AA8, 0x1FF0, 0x1F80, 0x3800, 0x6000, 0x8000},
{0x000F, 0x001F, 0x003E, 0x007C, 0x0070, 0x00E0, 0x01FC, 0x3FF6,
0x7FF6, 0x7FFE, 0x7FFC, 0x7FF8, 0x3FF0, 0x7FC0, 0xF800, 0xE000},
{0x000A, 0x0006}
},

{
{0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0810, 0x1088, 0x1088, 0x1088,
0x1388, 0x1008, 0x1008, 0x0810, 0x07E0, 0x07E0, 0x07E0, 0x07E0},
{0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0FF0, 0x1FF8, 0x1FF8, 0x1FF8,
0x1FF8, 0x1FF8, 0x1FF8, 0x0FF0, 0x07E0, 0x07E0, 0x07E0, 0x07E0},
{0x0008, 0x0008}
},
    
};

#endif

wxCursor    gMacCurrentCursor ;

wxCursorRefData::wxCursorRefData()
{
    m_hCursor = NULL;
}

wxCursorRefData::wxCursorRefData(const wxCursorRefData& cursor) : wxGDIRefData()
{
    m_hCursor = NULL;

#if wxOSX_USE_COCOA
    m_hCursor = (WX_NSCursor) wxMacCocoaRetain(cursor.m_hCursor);
#endif
}

wxCursorRefData::~wxCursorRefData()
{
#if wxOSX_USE_COCOA
    if ( m_hCursor )
        wxMacCocoaRelease(m_hCursor);
#endif
}

wxCursor::wxCursor()
{
}

#if wxUSE_IMAGE
wxCursor::wxCursor( const wxImage &image )
{
    InitFromImage( image ) ;
}

wxCursor::wxCursor(const char* const* xpmData)
{
    InitFromImage( wxImage(xpmData) ) ;
}
#endif // wxUSE_IMAGE

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxCursorRefData(*static_cast<const wxCursorRefData *>(data));
}

WXHCURSOR wxCursor::GetHCURSOR() const
{
    return (M_CURSORDATA ? M_CURSORDATA->m_hCursor : 0);
}

#if wxUSE_IMAGE

void wxCursor::InitFromImage(const wxImage & image)
{
    m_refData = new wxCursorRefData;
    int hotSpotX = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X);
    int hotSpotY = image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y);
#if wxOSX_USE_COCOA
    wxBitmap bmp( image );
    CGImageRef cgimage = bmp.CreateCGImage();
    if ( cgimage )
    {
        M_CURSORDATA->m_hCursor = wxMacCocoaCreateCursorFromCGImage( cgimage, hotSpotX, hotSpotY );
        CFRelease( cgimage );
    }
#endif
}

#endif //wxUSE_IMAGE

wxCursor::wxCursor(const wxString& cursor_file, wxBitmapType flags, int hotSpotX, int hotSpotY)
{
    m_refData = new wxCursorRefData;
    if ( flags == wxBITMAP_TYPE_MACCURSOR_RESOURCE )
    {
#if wxOSX_USE_COCOA
        M_CURSORDATA->m_hCursor = wxMacCocoaCreateCursorFromResource( cursor_file, flags );
#endif
    }
    else
    {
#if wxUSE_IMAGE
        wxImage image ;
        image.LoadFile( cursor_file, flags ) ;
        if ( image.IsOk() )
        {
            image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotX ) ;
            image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotSpotY ) ;
            m_refData->DecRef() ;
            m_refData = NULL ;
            InitFromImage( image ) ;
        }
#endif
    }
}

// Cursors by stock number
void wxCursor::InitFromStock(wxStockCursor cursor_type)
{
    m_refData = new wxCursorRefData;
#if wxOSX_USE_COCOA
    M_CURSORDATA->m_hCursor = wxMacCocoaCreateStockCursor( cursor_type );
#endif
}

void wxCursor::MacInstall() const
{
    gMacCurrentCursor = *this ;
#if wxOSX_USE_COCOA
    if ( IsOk() )
        wxMacCocoaSetCursor( M_CURSORDATA->m_hCursor );
#endif
}

wxCursor::~wxCursor()
{
}

// Global cursor setting
wxCursor gGlobalCursor;
void wxSetCursor(const wxCursor& cursor)
{
    cursor.MacInstall() ;
    gGlobalCursor = cursor;
}
