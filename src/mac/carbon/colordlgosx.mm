/////////////////////////////////////////////////////////////////////////////
// Name:        colordlg.cpp
// Purpose:     wxColourDialog class. NOTE: you can use the generic class
//              if you wish, instead of implementing this.
// Author:      Ryan Norton
// Modified by:
// Created:     2004-11-16
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "wx/mac/colordlg.h"
#include "wx/fontdlg.h"

// ============================================================================
// implementation
// ============================================================================

//Mac OSX 10.2+ only
#if USE_NATIVE_FONT_DIALOG_FOR_MACOSX

IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)

// Cocoa headers
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSFont.h>
#import <AppKit/NSFontManager.h>
#import <AppKit/NSFontPanel.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSColorPanel.h>

// ---------------------------------------------------------------------------
// wxCPWCDelegate - Window Closed delegate
// ---------------------------------------------------------------------------

@interface wxCPWCDelegate : NSObject
{
    bool m_bIsClosed;
}

// Delegate methods
- (id)init;
- (BOOL)windowShouldClose:(id)sender;
- (BOOL)isClosed;
@end // interface wxNSFontPanelDelegate : NSObject

@implementation wxCPWCDelegate : NSObject

- (id)init
{
    [super init];
    m_bIsClosed = false;

    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    m_bIsClosed = true;
    
    [NSApp abortModal];
    [NSApp stopModal];
    return YES;
}

- (BOOL)isClosed
{
    return m_bIsClosed;
}

@end // wxNSFontPanelDelegate

/*
 * wxColourDialog
 */

wxColourDialog::wxColourDialog()
{
    m_dialogParent = NULL;
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
    Create(parent, data);
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
    m_dialogParent = parent;
  
    if (data)
        m_colourData = *data;

    //
    //	This is the key call - this initializes
    //	events and window stuff for cocoa for carbon
    //	applications.
    //
    //	This is also the only call here that is 
    //	10.2+ specific (the rest is OSX only),
    //	which, ironically, the carbon font
    //	panel requires.
    //
    bool bOK = NSApplicationLoad();

    //autorelease pool - req'd for carbon
    NSAutoreleasePool *thePool;
    thePool = [[NSAutoreleasePool alloc] init];

    if(m_colourData.m_dataColour.Ok())
        [[NSColorPanel sharedColorPanel] setColor:
            [NSColor colorWithCalibratedRed:m_colourData.m_dataColour.Red() / 255.0
                                        green:m_colourData.m_dataColour.Green() / 255.0
                                        blue:m_colourData.m_dataColour.Blue() / 255.0
                                        alpha:1.0]
        ];
    else
        [[NSColorPanel sharedColorPanel] setColor:[NSColor blackColor]];        
        
    //We're done - free up the pool
    [thePool release];
    
    return bOK;
}
int wxColourDialog::ShowModal()
{
    //Start the pool.  Required for carbon interaction 
    //(For those curious, the only thing that happens
    //if you don't do this is a bunch of error
    //messages about leaks on the console,
    //with no windows shown or anything).
    NSAutoreleasePool *thePool;
    thePool = [[NSAutoreleasePool alloc] init];

    //Get the shared color and font panel
    NSColorPanel* theColorPanel = [NSColorPanel sharedColorPanel];

    //Create and assign the delegates (cocoa event handlers) so
    //we can tell if a window has closed/open or not
    wxCPWCDelegate* theCPDelegate = [[wxCPWCDelegate alloc] init];
    [theColorPanel setDelegate:theCPDelegate];
      
            //
            //	Start the color panel modal loop
            //
            NSModalSession session = [NSApp beginModalSessionForWindow:theColorPanel];
            for (;;) 
            {
                [NSApp runModalSession:session];
                
                //If the color panel is closed, return the font panel modal loop
                if ([theCPDelegate isClosed])
                    break;
            }
            [NSApp endModalSession:session];
    
    //free up the memory for the delegates - we don't need them anymore
    [theCPDelegate release];
                                            
    //Get the shared color panel along with the chosen color and set the chosen color
    NSColor* theColor = [[theColorPanel color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
                                    
    m_colourData.m_dataColour.Set(
                                (unsigned char) ([theColor redComponent] * 255.0),
                                (unsigned char) ([theColor greenComponent] * 255.0),
                                (unsigned char) ([theColor blueComponent] * 255.0)
                                   );

    //Release the pool, we're done :)
    [thePool release];

    //Return ID_OK - there are no "apply" buttons or the like
    //on either the font or color panel
    return wxID_OK;
}

#endif //use native font dialog

