/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.mm
// Purpose:     wxCursor class for wxCocoa
// Author:      Ryan Norton
// Modified by:
// Created:     2004-10-05
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/cursor.h"
#endif //WX_PRECOMP

#import <AppKit/NSCursor.h>
#import <AppKit/NSImage.h>
#include <wx/cocoa/string.h>

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)
#endif

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
/* TODO
    case wxCURSOR_WAIT:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_WAIT);
      break;
    case wxCURSOR_CROSS:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_HAND:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_HAND");
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_BULLSEYE");
      break;
    }
    case wxCURSOR_PENCIL:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PENCIL");
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_MAGNIFIER");
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_NO_ENTRY");
      break;
    }
    case wxCURSOR_LEFT_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_SIZING:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_SIZING");
      break;
    }
    case wxCURSOR_WATCH:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_WATCH");
      break;
    }
    case wxCURSOR_SPRAYCAN:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_ROLLER");
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PBRUSH");
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PLEFT");
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PRIGHT");
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_QARROW");
      break;
    }
    case wxCURSOR_BLANK:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_BLANK");
      break;
    }
*/
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
void wxBeginBusyCursor(wxCursor *cursor)
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

// TRUE if we're between the above two calls
bool wxIsBusy()
{
  return (wxBusyCursorCount > 0);
}    

