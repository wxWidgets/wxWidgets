/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/fontdlgosx.mm
// Purpose:     wxFontDialog class.
// Author:      Ryan Norton
// Created:     2004-10-03
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_FONTDLG

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#include "wx/fontdlg.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/cmndata.h"
#endif

#include "wx/fontutil.h"
#include "wx/modalhook.h"

// ============================================================================
// implementation
// ============================================================================


#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "wx/osx/private.h"


@interface wxFontPanelDelegate : NSObject<NSWindowDelegate>
{
    @public
    bool m_isUnderline;
    bool m_isStrikethrough;
}

// Delegate methods
- (id)init;
- (void)changeAttributes:(id)sender;
- (void)changeFont:(id)sender;
@end // interface wxNSFontPanelDelegate : NSObject



@implementation wxFontPanelDelegate : NSObject

- (id)init
{
    if (self = [super init])
    {
        m_isUnderline = false;
        m_isStrikethrough = false;
    }
    return self;
}

- (void)changeAttributes:(id)sender
{
    NSDictionary *dummyAttribs = [NSDictionary dictionaryWithObjectsAndKeys:
                                   [NSNumber numberWithInt:m_isUnderline?NSUnderlineStyleSingle:NSUnderlineStyleNone], NSUnderlineStyleAttributeName,
                                   [NSNumber numberWithInt:m_isStrikethrough?NSUnderlineStyleSingle:NSUnderlineStyleNone], NSStrikethroughStyleAttributeName,
                                   nil];
    NSDictionary *attribs = [sender convertAttributes:dummyAttribs];

    m_isUnderline = m_isStrikethrough = false;
    for (id key in attribs) {
        NSNumber *number = static_cast<NSNumber *>([attribs objectForKey:key]);
        if ([key isEqual:NSUnderlineStyleAttributeName]) {
            m_isUnderline = [number intValue] != NSUnderlineStyleNone;
        } else if ([key isEqual:NSStrikethroughStyleAttributeName]) {
            m_isStrikethrough = [number intValue] != NSUnderlineStyleNone;
        }
    }

    NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                [NSNumber numberWithInt:m_isUnderline?NSUnderlineStyleSingle:NSUnderlineStyleNone], NSUnderlineStyleAttributeName,
                                [NSNumber numberWithInt:m_isStrikethrough?NSUnderlineStyleSingle:NSUnderlineStyleNone], NSStrikethroughStyleAttributeName,
                                nil];
    [[NSFontManager sharedFontManager] setSelectedAttributes:attributes isMultiple:false];
}
- (void)changeFont:(id)sender
{
    NSFont *dummyFont = [NSFont userFontOfSize:12.0];
    [[NSFontPanel sharedFontPanel] setPanelFont:[sender convertFont:dummyFont] isMultiple:NO];
    [[NSFontManager sharedFontManager] setSelectedFont:[sender convertFont:dummyFont] isMultiple:false];
}
@end

@interface wxMacFontPanelAccView : NSView
{
    BOOL m_okPressed ;
    BOOL m_shouldClose ;
    NSButton* m_cancelButton ;
    NSButton* m_okButton ;
}

- (IBAction)cancelPressed:(id)sender;
- (IBAction)okPressed:(id)sender;
- (void)resetFlags;
- (BOOL)closedWithOk;
- (BOOL)shouldCloseCarbon;
- (NSButton*)okButton;
@end

@implementation wxMacFontPanelAccView : NSView
- (id)initWithFrame:(NSRect)rectBox
{
    if ( self = [super initWithFrame:rectBox] )
    {
        wxCFStringRef cfOkString( wxT("OK") );
        wxCFStringRef cfCancelString( wxT("Cancel") );

        NSRect rectCancel = NSMakeRect( (CGFloat) 10.0 , (CGFloat)10.0 , (CGFloat)82  , (CGFloat)24 );
        NSRect rectOK = NSMakeRect( (CGFloat)100.0 , (CGFloat)10.0 , (CGFloat)82  , (CGFloat)24 );

        NSButton* cancelButton = [[NSButton alloc] initWithFrame:rectCancel];
        [cancelButton setTitle:(NSString*)wxCFRetain((CFStringRef)cfCancelString)];
        [cancelButton setBezelStyle:NSRoundedBezelStyle];
        [cancelButton setButtonType:NSMomentaryPushInButton];
        [cancelButton setAction:@selector(cancelPressed:)];
        [cancelButton setTarget:self];
        m_cancelButton = cancelButton ;

        NSButton* okButton = [[NSButton alloc] initWithFrame:rectOK];
        [okButton setTitle:(NSString*)wxCFRetain((CFStringRef)cfOkString)];
        [okButton setBezelStyle:NSRoundedBezelStyle];
        [okButton setButtonType:NSMomentaryPushInButton];
        [okButton setAction:@selector(okPressed:)];
        [okButton setTarget:self];
        // doesn't help either, the button is not highlighted after a color dialog has been used
        // [okButton setKeyEquivalent:@"\r"];
        m_okButton = okButton ;


        [self addSubview:cancelButton];
        [self addSubview:okButton];

        [self resetFlags];
    }
    return self;
}

- (void)resetFlags
{
    m_okPressed = NO ;
    m_shouldClose = NO ;
}

- (IBAction)cancelPressed:(id)sender
{
    wxUnusedVar(sender);
    m_shouldClose = YES ;
    [NSApp stopModal];
}

- (IBAction)okPressed:(id)sender
{
    wxUnusedVar(sender);
    m_okPressed = YES ;
    m_shouldClose = YES ;
    [NSApp stopModal];
}

-(BOOL)closedWithOk
{
    return m_okPressed ;
}

-(BOOL)shouldCloseCarbon
{
    return m_shouldClose ;
}

-(NSButton*)okButton
{
    return m_okButton ;
}
@end


extern "C" int RunMixedFontDialog(wxFontDialog* dialog) ;

int RunMixedFontDialog(wxFontDialog* dialog)
{
#if wxOSX_USE_COCOA
    wxFontData& fontdata= ((wxFontDialog*)dialog)->GetFontData() ;
#else
    wxUnusedVar(dialog);
#endif
    int retval = wxID_CANCEL ;

    wxMacAutoreleasePool pool;

    // setting up the ok/cancel buttons
    NSFontPanel* fontPanel = [NSFontPanel sharedFontPanel] ;

    wxFontPanelDelegate* theFPDelegate = [[wxFontPanelDelegate alloc] init];
    [fontPanel setDelegate:theFPDelegate];


    [fontPanel setFloatingPanel:NO] ;
    [[fontPanel standardWindowButton:NSWindowCloseButton] setEnabled:NO] ;

    wxMacFontPanelAccView* accessoryView = nil;
    if ( [fontPanel accessoryView] == nil || [[fontPanel accessoryView] class] != [wxMacFontPanelAccView class] )
    {
        NSRect rectBox = NSMakeRect( 0 , 0 , 192 , 40 );
        accessoryView = [[wxMacFontPanelAccView alloc] initWithFrame:rectBox];
        [fontPanel setAccessoryView:accessoryView];
        [accessoryView release];

        [fontPanel setDefaultButtonCell:[[accessoryView okButton] cell]] ;
    }
    else
    {
        accessoryView = (wxMacFontPanelAccView*)[fontPanel accessoryView];
    }

    [accessoryView resetFlags];
#if wxOSX_USE_COCOA
    wxFont font = *wxNORMAL_FONT ;
    if ( fontdata.m_initialFont.IsOk() )
    {
        font = fontdata.m_initialFont ;
    }
    theFPDelegate->m_isStrikethrough = font.GetStrikethrough();
    theFPDelegate->m_isUnderline = font.GetUnderlined();

    [[NSFontPanel sharedFontPanel] setPanelFont: font.OSXGetNSFont() isMultiple:NO];
    [[NSFontManager sharedFontManager] setSelectedFont:font.OSXGetNSFont() isMultiple:false];

    NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                [NSNumber numberWithInt:font.GetUnderlined()
                                    ? NSUnderlineStyleSingle
                                    : NSUnderlineStyleNone],
                                NSUnderlineStyleAttributeName,
                                [NSNumber numberWithInt:font.GetStrikethrough()
                                    ? NSUnderlineStyleSingle
                                    : NSUnderlineStyleNone],
                                NSStrikethroughStyleAttributeName,
                                nil];

    [[NSFontManager sharedFontManager] setSelectedAttributes:attributes isMultiple:false];

    if(fontdata.m_fontColour.IsOk())
        [[NSColorPanel sharedColorPanel] setColor: fontdata.m_fontColour.OSXGetNSColor()];
    else
        [[NSColorPanel sharedColorPanel] setColor:[NSColor blackColor]];
#endif
    
    [NSApp runModalForWindow:fontPanel];
    
    // if we don't reenable it, FPShowHideFontPanel does not work
    [[fontPanel standardWindowButton:NSWindowCloseButton] setEnabled:YES] ;
    // we must pick the selection before closing, otherwise a native textcontrol interferes
    NSFont* theFont = [fontPanel panelConvertFont:[NSFont userFontOfSize:0]];
    [fontPanel close];

    if ( [accessoryView closedWithOk])
    {
#if wxOSX_USE_COCOA
        fontdata.m_chosenFont = wxFont(theFont);
        // copy the attributes not contained in a native CTFont
        fontdata.m_chosenFont.SetUnderlined(theFPDelegate->m_isUnderline);
        fontdata.m_chosenFont.SetStrikethrough(theFPDelegate->m_isStrikethrough);

        //Get the shared color panel along with the chosen color and set the chosen color
        fontdata.m_fontColour = wxColour([[NSColorPanel sharedColorPanel] color]);
#endif
        retval = wxID_OK ;
    }
    [fontPanel setAccessoryView:nil];
    [theFPDelegate release];
    return retval ;
}

#endif
