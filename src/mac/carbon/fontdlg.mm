/////////////////////////////////////////////////////////////////////////////
// Name:        fontdlg.cpp
// Purpose:     wxFontDialog class.
// Author:      Ryan Norton
// Modified by:
// Created:     2004-10-03
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "fontdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/cmndata.h"
    #include "wx/fontdlg.h"
    #include "wx/fontutil.h"
    #include "wx/log.h"
#endif
    
#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog)
#endif

// ============================================================================
// implementation
// ============================================================================

//Mac OSX 10.2+ only
#if USE_NATIVE_FONT_DIALOG_FOR_MACOSX && defined( __WXMAC_OSX__ ) && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )

// Cocoa headers
#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSFont.h>
#import <AppKit/NSFontManager.h>
#import <AppKit/NSFontPanel.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSColorPanel.h>

// ---------------------------------------------------------------------------
// wxWCDelegate - Window Closed delegate
// ---------------------------------------------------------------------------

@interface wxWCDelegate : NSObject
{
    bool m_bIsClosed;
}

// Delegate methods
- (id)init;
- (BOOL)windowShouldClose:(id)sender;
- (BOOL)isClosed;
@end // interface wxNSFontPanelDelegate : NSObject

@implementation wxWCDelegate : NSObject

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

// ---------------------------------------------------------------------------
// wxWCODelegate - Window Closed or Open delegate
// ---------------------------------------------------------------------------

@interface wxWCODelegate : NSObject
{
    bool m_bIsClosed;
    bool m_bIsOpen;
}

// Delegate methods
- (id)init;
- (BOOL)windowShouldClose:(id)sender;
- (void)windowDidUpdate:(NSNotification *)aNotification;
- (BOOL)isClosed;
- (BOOL)isOpen;
@end // interface wxNSFontPanelDelegate : NSObject

@implementation wxWCODelegate : NSObject

- (id)init
{
    [super init];
    m_bIsClosed = false;
    m_bIsOpen = false;

    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    m_bIsClosed = true;
    m_bIsOpen = false;
    
    [NSApp abortModal];
    [NSApp stopModal];
    return YES;
}

- (void)windowDidUpdate:(NSNotification *)aNotification
{
    if (m_bIsOpen == NO)
    {
        m_bIsClosed = false;
        m_bIsOpen = true;

        [NSApp abortModal];
        [NSApp stopModal];
    }
}

- (BOOL)isClosed
{
    return m_bIsClosed;
}

- (BOOL)isOpen
{
    return m_bIsOpen;
}

@end // wxNSFontPanelDelegate

// ---------------------------------------------------------------------------
// wxFontDialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog()
{
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

wxFontDialog::~wxFontDialog()
{
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_fontData = data;

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

    //Get the initial wx font
    wxFont& thewxfont = m_fontData.m_initialFont;
    
    //if the font is valid set the default (selected) font of the 
    //NSFontDialog to that font
    if (thewxfont.Ok())
    {
        NSFontTraitMask theMask = 0;
    
        if(thewxfont.GetStyle() == wxFONTSTYLE_ITALIC)
            theMask |= NSItalicFontMask;
    
        if(thewxfont.IsFixedWidth())
            theMask |= NSFixedPitchFontMask;

        NSFont* theDefaultFont = 
            [[NSFontManager sharedFontManager] fontWithFamily:
                                                    wxNSStringWithWxString(thewxfont.GetFaceName())
                                            traits:theMask
                                            weight:thewxfont.GetWeight() == wxBOLD ? 9 :
                                                    thewxfont.GetWeight() == wxLIGHT ? 0 : 5
                                            size: (float)(thewxfont.GetPointSize())
            ];
            
        wxASSERT_MSG(theDefaultFont, wxT("Invalid default font for wxCocoaFontDialog!"));
     
        //set the initial font of the NSFontPanel 
        //(the font manager calls the appropriate NSFontPanel method)
        [[NSFontManager sharedFontManager] setSelectedFont:theDefaultFont isMultiple:NO];

    }

    if(m_fontData.m_fontColour.Ok())
        [[NSColorPanel sharedColorPanel] setColor:
            [NSColor colorWithCalibratedRed:m_fontData.m_fontColour.Red() / 255.0
                                        green:m_fontData.m_fontColour.Red() / 255.0
                                        blue:m_fontData.m_fontColour.Red() / 255.0
                                        alpha:1.0]
        ];
    else
        [[NSColorPanel sharedColorPanel] setColor:[NSColor blackColor]];   
        
    //We're done - free up the pool
    [thePool release];
    
    return bOK;
}

int wxFontDialog::ShowModal()
{    
    //Start the pool.  Required for carbon interaction 
    //(For those curious, the only thing that happens
    //if you don't do this is a bunch of error
    //messages about leaks on the console,
    //with no windows shown or anything).
    NSAutoreleasePool *thePool;
    thePool = [[NSAutoreleasePool alloc] init];

    //Get the shared color and font panel
    NSFontPanel* theFontPanel 	= [NSFontPanel sharedFontPanel];
    NSColorPanel* theColorPanel = [NSColorPanel sharedColorPanel];

    //Create and assign the delegates (cocoa event handlers) so
    //we can tell if a window has closed/open or not
    wxWCDelegate* theFPDelegate = [[wxWCDelegate alloc] init];
    [theFontPanel setDelegate:theFPDelegate];

    wxWCODelegate* theCPDelegate = [[wxWCODelegate alloc] init];
    [theColorPanel setDelegate:theCPDelegate];
      
    //
    //	Begin the modal loop for the font and color panels
    //
    //	The idea is that we first make the font panel modal,
    //  but if the color panel is opened, unless we stop the
    //  modal loop the color panel opens behind the font panel
    //  with no input acceptable to it - which makes it useless.
    //
    //	So we set up delegates for both the color and font panels,
    //  and the if the font panel opens the color panel, we 
    //  stop the modal loop, and start a seperate modal loop for
    //  the color panel until the color panel closes, switching
    //  back to the font panel modal loop once it does close.
    //
    do
    {
        //
        //	Start the font panel modal loop
        //
        NSModalSession session = [NSApp beginModalSessionForWindow:theFontPanel];
        for (;;) 
        {
            [NSApp runModalSession:session];
            
            //If the font panel is closed or the font panel
            //opened the color panel, break
            if ([theFPDelegate isClosed] || [theCPDelegate isOpen])
                break;
        }
        [NSApp endModalSession:session];
        
        //is the color panel open?
        if ([theCPDelegate isOpen])
        {
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
        }
        //If the font panel is still alive (I.E. we broke
        //out of its modal loop because the color panel was
        //opened) return the font panel modal loop
    }while([theFPDelegate isClosed] == NO);
    
    //free up the memory for the delegates - we don't need them anymore
    [theFPDelegate release];
    [theCPDelegate release];
    
    //Get the font the user selected
    NSFont* theFont = [theFontPanel panelConvertFont:[NSFont userFontOfSize:0]];      
    
    //Get more information about the user's chosen font
    NSFontTraitMask theTraits = [[NSFontManager sharedFontManager] traitsOfFont:theFont];
    int theFontWeight = [[NSFontManager sharedFontManager] weightOfFont:theFont];
    int theFontSize = (int) [theFont pointSize];
    
    //Set the wx font to the appropriate data
    if(theTraits & NSFixedPitchFontMask)
        m_fontData.m_chosenFont.SetFamily(wxTELETYPE);
       
    m_fontData.m_chosenFont.SetFaceName(wxStringWithNSString([theFont familyName]));
    m_fontData.m_chosenFont.SetPointSize(theFontSize);
    m_fontData.m_chosenFont.SetStyle(theTraits & NSItalicFontMask ? wxFONTSTYLE_ITALIC : 0);
    m_fontData.m_chosenFont.SetWeight(theFontWeight < 5 ? wxLIGHT :
                                    theFontWeight >= 9 ? wxBOLD : wxNORMAL);    
                                        
    //Get the shared color panel along with the chosen color and set the chosen color
    NSColor* theColor = [[theColorPanel color] colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
                                    
    m_fontData.m_fontColour.Set((unsigned char) ([theColor redComponent] * 255.0),
                                (unsigned char) ([theColor greenComponent] * 255.0),
                                (unsigned char) ([theColor blueComponent] * 255.0));

    //Friendly debug stuff
#ifdef FONTDLGDEBUG
    wxPrintf(wxT("---Font Panel---\n--NS--\nSize:%f\nWeight:%i\nTraits:%i\n--WX--\nFaceName:%s\nPointSize:%i\nStyle:%i\nWeight:%i\nColor:%i,%i,%i\n---END Font Panel---\n"), 
    
                (float) theFontSize,
                theFontWeight,
                theTraits,
                
                m_fontData.m_chosenFont.GetFaceName().c_str(),
                m_fontData.m_chosenFont.GetPointSize(),
                m_fontData.m_chosenFont.GetStyle(),
                m_fontData.m_chosenFont.GetWeight(),
                    m_fontData.m_fontColour.Red(), 
                    m_fontData.m_fontColour.Green(), 
                    m_fontData.m_fontColour.Blue() );
#endif

    //Release the pool, we're done :)
    [thePool release];

    //Return ID_OK - there are no "apply" buttons or the like
    //on either the font or color panel
    return wxID_OK;
}

//old api stuff (REMOVE ME)
bool wxFontDialog::IsShown() const
{
    return false;
}

#else
   //10.2+ only
   
// ---------------------------------------------------------------------------
// wxFontDialog stub for mac OS's without a native font dialog
// ---------------------------------------------------------------------------

wxFontDialog::wxFontDialog()
{
    m_dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, const wxFontData&  data)
{
    Create(parent, data);
}

void wxFontDialog::SetData(wxFontData& fontdata)
{
    m_fontData = fontdata;
}

bool wxFontDialog::Create(wxWindow *parent, const wxFontData& data)
{
    m_dialogParent = parent;

    m_fontData = data;

    // TODO: you may need to do dialog creation here, unless it's
    // done in ShowModal.
    return TRUE;
}

bool wxFontDialog::IsShown() const
{
    return false;
}

int wxFontDialog::ShowModal()
{
    // TODO: show (maybe create) the dialog
    return wxID_CANCEL;
}

#endif // 10.2+
