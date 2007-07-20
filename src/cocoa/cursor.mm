/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/cursor.mm
// Purpose:     wxCursor class for wxCocoa
// Author:      Ryan Norton
//              David Elliott
// Modified by:
// Created:     2004-10-05
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
//              2007, Software 2000 Ltd.
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/cursor.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
#endif //WX_PRECOMP

#import <AppKit/NSCursor.h>
#import <AppKit/NSImage.h>
#include "wx/cocoa/string.h"

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)

typedef struct tagClassicCursor
{
    wxUint16 bits[16];
    wxUint16 mask[16];
    wxInt16 hotspot[2];
}ClassicCursor;

///////////////////////////////////////////////////////////////////////////
// This is a direct copy from src/mac/carbon/cursor.cpp and should be
// changed to use common code if we plan on keeping it this way.
// Note that this is basically an array of classic 'CURS' resources.

const short kwxCursorBullseye = 0 ;
const short kwxCursorBlank = 1 ;
const short kwxCursorPencil = 2 ;
const short kwxCursorMagnifier = 3 ;
const short kwxCursorNoEntry = 4 ;
const short kwxCursorPaintBrush = 5 ;
const short kwxCursorPointRight = 6 ;
const short kwxCursorPointLeft = 7 ;
const short kwxCursorQuestionArrow = 8 ;
const short kwxCursorRightArrow = 9 ;
const short kwxCursorSizeNS = 10 ;
const short kwxCursorSize = 11 ;
const short kwxCursorSizeNESW = 12 ;
const short kwxCursorSizeNWSE = 13 ;
const short kwxCursorRoller = 14 ;
const short kwxCursorLast = kwxCursorRoller ;

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

} ;

// End of data copied from src/mac/carbon/cursor.cpp
///////////////////////////////////////////////////////////////////////////

/* wxStockCursor
 * Returns a newly allocated (i.e. retainCount == 1) NSCursor based on the
 * classic Mac OS cursor data in this source file.  This allows us to
 * implement the "stock" wxWidgets cursors which aren't present in Cocoa.
 * FIXME: This function should be static.
 * FIXME: This function should use "Alloc" in preference to "Get" to follow
 * the proper Cocoa naming conventions.
 */
NSCursor* wxGetStockCursor( short sIndex )
{
    ClassicCursor* pCursor = &gMacCursors[sIndex];

    //Classic mac cursors are 1bps 16x16 black and white with a
    //identical mask that is 1 for on and 0 for off
    NSImage *theImage = [[NSImage alloc] initWithSize:NSMakeSize(16.0,16.0)];

    NSBitmapImageRep *theRep = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes: NULL  // Tell Cocoa to allocate the planes for us.
        pixelsWide: 16      // All classic cursors are 16x16
        pixelsHigh: 16
        bitsPerSample: 1    // All classic cursors are bitmaps with bitmasks
        samplesPerPixel: 2  // Sample 0:image 1:mask
        hasAlpha: YES       // Identify last sample as a mask
        isPlanar: YES       // Use a separate array for each sample
        colorSpaceName: NSCalibratedWhiteColorSpace // 0.0=black 1.0=white
        bytesPerRow: 2      // Rows in each plane are on 2-byte boundaries (no pad)
        bitsPerPixel: 1];   // same as bitsPerSample since data is planar
    // XXX: Should we use NSDeviceWhiteColorSpace? Does it matter?
    
    // Ensure that Cocoa allocated 2 and only 2 of the 5 possible planes
    unsigned char *planes[5];
    [theRep getBitmapDataPlanes:planes];
    wxASSERT(planes[0] != NULL);
    wxASSERT(planes[1] != NULL);
    wxASSERT(planes[2] == NULL);
    wxASSERT(planes[3] == NULL);
    wxASSERT(planes[4] == NULL);

    // NOTE1: The Cursor's bits field is white=0 black=1.. thus the bitwise-not
    // Why not use NSCalibratedBlackColorSpace?  Because that reverses the
    // sense of the alpha (mask) plane.
    // NOTE2: The mask data is 0=off 1=on
    // NOTE3: Cocoa asks for "premultiplied" color planes.  Since we have a
    // 1-bit color plane and a 1-bit alpha plane we can just do a bitwise-and
    // on the two.  The original cursor bitmaps have 0 (white actually) for
    // any masked-off pixels.  Therefore every masked-off pixel would be wrong
    // since we bit-flip all of the picture bits.  In practice, Cocoa doesn't
    // seem to care, but we are following the documentation.

    // Fill in the color (black/white) plane
    for(int i=0; i<16; ++i)
    {
        planes[0][2*i  ] = (~pCursor->bits[i] & pCursor->mask[i]) >> 8 & 0xff;
        planes[0][2*i+1] = (~pCursor->bits[i] & pCursor->mask[i]) & 0xff;
    }
    // Fill in the alpha (i.e. mask) plane
    for(int i=0; i<16; ++i)
    {
        planes[1][2*i  ] = pCursor->mask[i] >> 8 & 0xff;
        planes[1][2*i+1] = pCursor->mask[i] & 0xff;
    }

    //add the representation (data) to the image
    [theImage addRepresentation:theRep];

    //create the new cursor
    NSCursor* theCursor =  [[NSCursor alloc]  initWithImage:theImage
                                    hotSpot:NSMakePoint(pCursor->hotspot[1], pCursor->hotspot[0])
                            ];

    //do the usual cleanups
    [theRep release];
    [theImage release];

    //return the new cursor
    return theCursor;
}

wxCursorRefData::wxCursorRefData() :
    m_width(32), m_height(32), m_hCursor(nil)
{
}

wxCursorRefData::~wxCursorRefData()
{
    if (m_hCursor)
        [m_hCursor release];
}

// Cursors
wxCursor::wxCursor()
{
}

wxCursor::wxCursor(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height),
    int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY), const char WXUNUSED(maskBits)[])
{
}

wxCursor::wxCursor(const wxString& cursor_file, long flags, int hotSpotX, int hotSpotY)
{
    m_refData = new wxCursorRefData;

    //TODO:  Not sure if this works or not
    NSImage* theImage;

    if (flags & wxBITMAP_TYPE_MACCURSOR_RESOURCE)
    {
        //[NSBundle bundleForClass:[self class]]?
        theImage = [[NSImage alloc]
                        initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:wxNSStringWithWxString(cursor_file) ofType:nil]
                    ];

    }
    else
        theImage = [[NSImage alloc] initByReferencingFile:wxNSStringWithWxString(cursor_file)
                ];

    wxASSERT(theImage);

    M_CURSORDATA->m_hCursor = [[NSCursor alloc] initWithImage:theImage
                                        hotSpot:NSMakePoint(hotSpotX, hotSpotY)
                                ];

    [theImage release];
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  m_refData = new wxCursorRefData;

  switch (cursor_type)
  {
    case wxCURSOR_IBEAM:
      M_CURSORDATA->m_hCursor = [[NSCursor IBeamCursor] retain];
      break;
    case wxCURSOR_ARROW:
      M_CURSORDATA->m_hCursor = [[NSCursor arrowCursor] retain];
      break;
/* TODO:
    case wxCURSOR_COPY_ARROW:
        M_CURSORDATA->m_themeCursor = kThemeCopyArrowCursor ;
        break;
    case wxCURSOR_WAIT:
        M_CURSORDATA->m_themeCursor = kThemeWatchCursor ;
        break;
    case wxCURSOR_CROSS:
        M_CURSORDATA->m_themeCursor = kThemeCrossCursor;
        break;
    case wxCURSOR_SIZENWSE:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorSizeNWSE);
        }
        break;
*/
    case wxCURSOR_SIZENESW:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorSizeNESW);
        }
        break;
/* TODO:
    case wxCURSOR_SIZEWE:
        {
            M_CURSORDATA->m_themeCursor = kThemeResizeLeftRightCursor;
        }
        break;
*/
    case wxCURSOR_SIZENS:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorSizeNS);
        }
        break;
    case wxCURSOR_SIZING:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorSize);
        }
        break;
/* TODO:
    case wxCURSOR_HAND:
        {
            M_CURSORDATA->m_themeCursor = kThemePointingHandCursor;
        }
        break;
*/
    case wxCURSOR_BULLSEYE:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorBullseye);
        }
        break;
    case wxCURSOR_PENCIL:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorPencil);
        }
        break;
    case wxCURSOR_MAGNIFIER:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorMagnifier);
        }
        break;
    case wxCURSOR_NO_ENTRY:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorNoEntry);
        }
        break;
/*  TODO:
    case wxCURSOR_WATCH:
        {
            M_CURSORDATA->m_themeCursor = kThemeWatchCursor;
            break;
        }
*/
    case wxCURSOR_PAINT_BRUSH:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorPaintBrush);
            break;
        }
    case wxCURSOR_POINT_LEFT:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorPointLeft);
            break;
        }
    case wxCURSOR_POINT_RIGHT:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorPointRight);
            break;
        }
    case wxCURSOR_QUESTION_ARROW:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorQuestionArrow);
            break;
        }
    case wxCURSOR_BLANK:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorBlank);
            break;
        }
    case wxCURSOR_RIGHT_ARROW:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorRightArrow);
            break;
        }
    case wxCURSOR_SPRAYCAN:
        {
            M_CURSORDATA->m_hCursor = wxGetStockCursor(kwxCursorRoller);
            break;
        }
    case wxCURSOR_CHAR:
    case wxCURSOR_LEFT_BUTTON:
    case wxCURSOR_RIGHT_BUTTON:
    case wxCURSOR_MIDDLE_BUTTON:
    default:
        break;
    }
}

wxCursor::~wxCursor()
{
}

// Global cursor setting
void wxSetCursor(const wxCursor& cursor)
{
    if (cursor.GetNSCursor())
        [cursor.GetNSCursor() push];
}

static int wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(const wxCursor *cursor)
{
    wxBusyCursorCount ++;
    if (wxBusyCursorCount == 1)
    {
        // TODO
    }
    else
    {
        // TODO
    }
}

// Restore cursor to normal
void wxEndBusyCursor()
{
    if (wxBusyCursorCount == 0)
        return;

    wxBusyCursorCount --;
    if (wxBusyCursorCount == 0)
    {
        // TODO
    }
}

// true if we're between the above two calls
bool wxIsBusy()
{
  return (wxBusyCursorCount > 0);
}
