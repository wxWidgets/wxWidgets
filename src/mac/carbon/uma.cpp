#include "wx/defs.h"
#include "wx/dc.h"
#ifndef __DARWIN__
#include <Scrap.h>
#endif
#include "wx/mac/uma.h"
#include <MacTextEditor.h>

#ifndef __DARWIN__
#  include <Navigation.h>
#  if defined(TARGET_CARBON)
#    if PM_USE_SESSION_APIS
#      include <PMCore.h>
#    endif
#    include <PMApplication.h>
#  else
#    include <Printing.h>
#  endif
#endif

// since we have decided that we only support 8.6 upwards we are
// checking for these minimum requirements in the startup code of
// the application so all wxWindows code can safely assume that appearance 1.1
// windows manager, control manager, navigation services etc. are
// present

static bool	sUMAHasAppearance = false ;
static long sUMAAppearanceVersion = 0 ;
static long sUMASystemVersion = 0 ;
static bool sUMAHasAquaLayout = false ;
static bool sUMASystemInitialized = false ;

extern int gAGABackgroundColor ;
bool UMAHasAppearance() { return sUMAHasAppearance ; }
long UMAGetAppearanceVersion() { return sUMAAppearanceVersion ; }
long UMAGetSystemVersion() { return sUMASystemVersion ; }

static bool	sUMAHasWindowManager = false ;
static long sUMAWindowManagerAttr = 0 ;

bool UMAHasWindowManager() { return sUMAHasWindowManager ; }
long UMAGetWindowManagerAttr() { return sUMAWindowManagerAttr ; }
bool UMAHasAquaLayout() { return sUMAHasAquaLayout ; }
bool UMASystemIsInitialized() { return sUMASystemInitialized ; }

void UMACleanupToolbox()
{
	if ( sUMAHasAppearance )
	{
		UnregisterAppearanceClient() ;
	}
	if ( NavServicesAvailable() )
	{
		NavUnload() ;
	}
  if ( TXNTerminateTextension != (void*) kUnresolvedCFragSymbolAddress )
	  TXNTerminateTextension( ) ;
}
void UMAInitToolbox( UInt16 inMoreMastersCalls )
{
#if !TARGET_CARBON
	::MaxApplZone();
	for (long i = 1; i <= inMoreMastersCalls; i++)
		::MoreMasters();

	::InitGraf(&qd.thePort);
	::InitFonts();
	::InitMenus();
	::TEInit();
	::InitDialogs(0L);
	::FlushEvents(everyEvent, 0);
	::InitCursor();
	long total,contig;
	PurgeSpace(&total, &contig);
#else
	InitCursor();
#endif

    if ( Gestalt(gestaltSystemVersion, &sUMASystemVersion) != noErr)
    	sUMASystemVersion = 0x0000 ;

	long theAppearance ;
	if ( Gestalt( gestaltAppearanceAttr, &theAppearance ) == noErr )
	{
		sUMAHasAppearance = true ;
		RegisterAppearanceClient();
		if ( Gestalt( gestaltAppearanceVersion, &theAppearance ) == noErr )
		{
			sUMAAppearanceVersion = theAppearance ;
		}
		else
		{
			sUMAAppearanceVersion = 0x0100 ;
		}
	}
	if ( Gestalt( gestaltWindowMgrAttr, &sUMAWindowManagerAttr ) == noErr )
	{
		sUMAHasWindowManager = sUMAWindowManagerAttr & gestaltWindowMgrPresent ;
	}

#if TARGET_CARBON
// Call currently implicitely done :		InitFloatingWindows() ;
#else
	if ( sUMAHasWindowManager )
		InitFloatingWindows() ;
	else
		InitWindows();
#endif

	if ( NavServicesAvailable() )
	{
		NavLoad() ;
	}

  long menuMgrAttr ;
  Gestalt( gestaltMenuMgrAttr , &menuMgrAttr ) ;
  if ( menuMgrAttr & gestaltMenuMgrAquaLayoutMask )
    sUMAHasAquaLayout = true ;

  if ( TXNInitTextension != (void*) kUnresolvedCFragSymbolAddress )
  {
    FontFamilyID fontId ;
	Str255 fontName ;
	SInt16 fontSize ;
	Style fontStyle ;
	GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
    GetFNum( fontName, &fontId );

    TXNMacOSPreferredFontDescription fontDescriptions[] =
    {
        { fontId , (fontSize << 16) ,kTXNDefaultFontStyle, kTXNSystemDefaultEncoding } ,
    } ;
    int noOfFontDescriptions = sizeof( fontDescriptions ) / sizeof(TXNMacOSPreferredFontDescription) ;
#if 0 // TARGET_CARBON
    --noOfFontDescriptions ;
#endif
  	// kTXNAlwaysUseQuickDrawTextMask might be desirable because of speed increases but it crashes the app under OS X upon key stroke
  	OptionBits options = kTXNWantMoviesMask | kTXNWantSoundMask | kTXNWantGraphicsMask ;
#if TARGET_CARBON
    if ( !UMAHasAquaLayout() )
#endif
    {
        options |= kTXNAlwaysUseQuickDrawTextMask ;
    }
  	TXNInitTextension(fontDescriptions,  noOfFontDescriptions, options );
  }


  sUMASystemInitialized = true ;

}

/*
Boolean CanUseATSUI()
	{
	long result;
	OSErr err = Gestalt(gestaltATSUVersion, &result);
	return (err == noErr);
	}
*/
// process manager
long UMAGetProcessMode()
{
	OSErr err ;
	ProcessInfoRec processinfo;
	ProcessSerialNumber procno ;

	procno.highLongOfPSN = NULL ;
	procno.lowLongOfPSN = kCurrentProcess ;
	processinfo.processInfoLength = sizeof(ProcessInfoRec);
	processinfo.processName = NULL;
	processinfo.processAppSpec = NULL;

	err = ::GetProcessInformation( &procno , &processinfo ) ;
	wxASSERT( err == noErr ) ;
	return processinfo.processMode ;
}

bool UMAGetProcessModeDoesActivateOnFGSwitch()
{
	return UMAGetProcessMode() & modeDoesActivateOnFGSwitch ;
}

// menu manager

void UMASetMenuTitle( MenuRef menu , StringPtr title )
{
/*
#if !TARGET_CARBON
	long 			size = GetHandleSize( (Handle) menu ) ;
	const long 		headersize = 14 ;
	int				oldlen = (**menu).menuData[0] + 1;
	int				newlen = title[0] + 1 ;

	if ( oldlen < newlen )
	{
		// enlarge before adjusting
		SetHandleSize( (Handle) menu , size + (newlen - oldlen ) );
	}

	if ( oldlen != newlen )
		memmove( (char*) (**menu).menuData + newlen , (char*) (**menu).menuData + oldlen , size - headersize - oldlen ) ;

	memcpy( (char*) (**menu).menuData , title ,  newlen ) ;
	if ( oldlen > newlen )
	{
		// shrink after
		SetHandleSize( (Handle) menu , size + (newlen - oldlen ) ) ;
	}
#else
*/
	SetMenuTitle( menu , title ) ;
//#endif
}

UInt32 UMAMenuEvent( EventRecord *inEvent )
{
		return MenuEvent( inEvent ) ;
}

void 			UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex inItem )
{
	EnableMenuItem( inMenu , inItem ) ;
}

void 			UMADisableMenuItem( MenuRef inMenu , MenuItemIndex inItem )
{
	DisableMenuItem( inMenu , inItem ) ;
}

void UMAAppendSubMenuItem( MenuRef menu , StringPtr l , SInt16 id )
{
	MacAppendMenu(menu, l);
	SetMenuItemHierarchicalID( menu , CountMenuItems( menu ) , id ) ;
}

void UMAInsertSubMenuItem( MenuRef menu , StringPtr l , MenuItemIndex item , SInt16 id  )
{
	MacInsertMenuItem(menu, l , item);
	SetMenuItemHierarchicalID( menu , item , id ) ;
}

void UMASetMenuItemShortcut( MenuRef menu , MenuItemIndex item , SInt16 key , UInt8 modifiers )
{
	if ( key )
	{
	    SInt16 glyph = 0 ;
	    SInt16 macKey = key ;
	    if ( key >= WXK_F1 && key <= WXK_F15 )
	    {
	        macKey = kFunctionKeyCharCode ;
	        glyph = kMenuF1Glyph + ( key - WXK_F1 ) ;
	        if ( key >= WXK_F13 )
	            glyph += 13 ;
	        switch( key )
	        {
	            case WXK_F1 :
	                macKey += ( 0x7a << 8 ) ;
	                break ;
	            case WXK_F2 :
	                macKey += ( 0x78 << 8 ) ;
	                break ;
	            case WXK_F3 :
	                macKey += ( 0x63 << 8 ) ;
	                break ;
	            case WXK_F4 :
	                macKey += ( 0x76 << 8 ) ;
	                break ;
	            case WXK_F5 :
	                macKey += ( 0x60 << 8 ) ;
	                break ;
	            case WXK_F6 :
	                macKey += ( 0x61 << 8 ) ;
	                break ;
	            case WXK_F7 :
	                macKey += ( 0x62 << 8 ) ;
	                break ;
	            case WXK_F8 :
	                macKey += ( 0x64 << 8 ) ;
	                break ;
	            case WXK_F9 :
	                macKey += ( 0x65 << 8 ) ;
	                break ;
	            case WXK_F10 :
	                macKey += ( 0x6D << 8 ) ;
	                break ;
	            case WXK_F11 :
	                macKey += ( 0x67 << 8 ) ;
	                break ;
	            case WXK_F12 :
	                macKey += ( 0x6F << 8 ) ;
	                break ;
	            case WXK_F13 :
	                macKey += ( 0x69 << 8 ) ;
	                break ;
	            case WXK_F14 :
	                macKey += ( 0x6B << 8 ) ;
	                break ;
	            case WXK_F15 :
	                macKey += ( 0x71 << 8 ) ;
	                break ;
	            default :
	                break ;
	        } ;
	        // unfortunately this does not yet trigger the right key ,
	        // for some reason mac justs picks the first function key menu
	        // defined, so we turn this off
	        macKey = 0 ;
	        glyph = 0 ;
  	    }
	    else
    	{
    	    switch( key )
    	    {
    	        case WXK_BACK :
    	            macKey = kBackspaceCharCode ;
    	            glyph = kMenuDeleteLeftGlyph ;
    	            break ;
    	        case WXK_TAB :
    	            macKey = kTabCharCode ;
    	            glyph = kMenuTabRightGlyph ;
    	            break ;
    	        case kEnterCharCode :
    	            macKey = kEnterCharCode ;
    	            glyph = kMenuEnterGlyph ;
    	            break ;
    	        case WXK_RETURN :
    	            macKey = kReturnCharCode ;
    	            glyph = kMenuReturnGlyph ;
    	            break ;
    	        case WXK_ESCAPE :
    	            macKey = kEscapeCharCode ;
    	            glyph = kMenuEscapeGlyph ;
    	            break ;
    	        case WXK_SPACE :
    	            macKey = ' ' ;
    	            glyph = kMenuSpaceGlyph ;
    	            break ;
    	        case WXK_DELETE :
    	            macKey = kDeleteCharCode ;
    	            glyph = kMenuDeleteRightGlyph ;
    	            break ;
    	        case WXK_CLEAR :
    	            macKey = kClearCharCode ;
    	            glyph = kMenuClearGlyph ;
    	            break ;
    	        case WXK_PRIOR : // PAGE UP
    	            macKey = kPageUpCharCode ;
    	            glyph = kMenuPageUpGlyph ;
    	            break ;
    	        case WXK_NEXT :
    	            macKey = kPageDownCharCode ;
    	            glyph = kMenuPageDownGlyph ;
    	            break ;
    	        case WXK_LEFT :
    	            macKey = kLeftArrowCharCode ;
    	            glyph = kMenuLeftArrowGlyph ;
    	            break ;
    	        case WXK_UP :
    	            macKey = kUpArrowCharCode ;
    	            glyph = kMenuUpArrowGlyph ;
    	            break ;
    	        case WXK_RIGHT :
    	            macKey = kRightArrowCharCode ;
    	            glyph = kMenuRightArrowGlyph ;
    	            break ;
    	        case WXK_DOWN :
    	            macKey = kDownArrowCharCode ;
    	            glyph = kMenuDownArrowGlyph ;
    	            break ;
            }
        }

	    SetItemCmd( menu, item , macKey );
	    SetMenuItemModifiers(menu, item , modifiers ) ;

        if ( glyph )
            SetMenuItemKeyGlyph(menu, item , glyph ) ;
	}
}

void UMAAppendMenuItem( MenuRef menu , StringPtr l , SInt16 key, UInt8 modifiers )
{
	MacAppendMenu(menu, "\pA");
	SetMenuItemText(menu, (SInt16) ::CountMenuItems(menu), l);
	UMASetMenuItemShortcut( menu ,  (SInt16) ::CountMenuItems(menu), key , modifiers ) ;
}

void UMAInsertMenuItem( MenuRef menu , StringPtr l , MenuItemIndex item , SInt16 key, UInt8 modifiers )
{
	MacInsertMenuItem( menu , "\pA" , item) ;
	SetMenuItemText(menu, item+1 , l);
	UMASetMenuItemShortcut( menu , item+1 , key , modifiers ) ;
}

// quickdraw

int gPrOpenCounter = 0 ;

OSStatus UMAPrOpen(void *macPrintSession)
{
#if !TARGET_CARBON
	OSErr err = noErr ;
	++gPrOpenCounter ;
	if ( gPrOpenCounter == 1 )
	{
		PrOpen() ;
		err = PrError() ;
		wxASSERT( err == noErr ) ;
	}
	return err ;
#else
	OSStatus err = noErr ;
	++gPrOpenCounter ;
	if ( gPrOpenCounter == 1 )
	{
  #if PM_USE_SESSION_APIS
	    err = PMCreateSession((PMPrintSession *)macPrintSession) ;
  #else
	    err = PMBegin() ;
  #endif
		wxASSERT( err == noErr ) ;
	}
	return err ;
#endif
}

OSStatus UMAPrClose(void *macPrintSession)
{
#if !TARGET_CARBON
	OSErr err = noErr ;
	wxASSERT( gPrOpenCounter >= 1 ) ;
	if ( gPrOpenCounter == 1 )
	{
		PrClose() ;
		err = PrError() ;
		wxASSERT( err == noErr ) ;
	}
	--gPrOpenCounter ;
	return err ;
#else
	OSStatus err = noErr ;
	wxASSERT( gPrOpenCounter >= 1 ) ;
	if ( gPrOpenCounter == 1 )
	{
  #if PM_USE_SESSION_APIS
	    err = PMRelease(*(PMPrintSession *)macPrintSession) ;
	    *(PMPrintSession *)macPrintSession = kPMNoReference;
  #else
	    err = PMEnd() ;
  #endif
	}
	--gPrOpenCounter ;
	return err ;
#endif
}

#if !TARGET_CARBON

pascal QDGlobalsPtr GetQDGlobalsPtr (void) ;
pascal QDGlobalsPtr GetQDGlobalsPtr (void)
{
	return QDGlobalsPtr (* (Ptr*) LMGetCurrentA5 ( ) - 0xCA);
}

#endif

void UMAShowWatchCursor()
{
	OSErr err = noErr;

	CursHandle watchFob = GetCursor (watchCursor);

	if (!watchFob)
		err = nilHandleErr;
	else
	{
	#if TARGET_CARBON
//		Cursor preservedArrow;
//		GetQDGlobalsArrow (&preservedArrow);
//		SetQDGlobalsArrow (*watchFob);
//		InitCursor ( );
//		SetQDGlobalsArrow (&preservedArrow);
		SetCursor (*watchFob);
	#else
		SetCursor (*watchFob);
	#endif
	}
}

void			UMAShowArrowCursor()
{
#if TARGET_CARBON
	Cursor arrow;
	SetCursor (GetQDGlobalsArrow (&arrow));
#else
	SetCursor (&(qd.arrow));
#endif
}

// window manager

GrafPtr		UMAGetWindowPort( WindowRef inWindowRef )
{
	wxASSERT( inWindowRef != NULL ) ;
#if TARGET_CARBON
	return (GrafPtr) GetWindowPort( inWindowRef ) ;
#else
	return (GrafPtr) inWindowRef ;
#endif
}

void		 	UMADisposeWindow( WindowRef inWindowRef )
{
	wxASSERT( inWindowRef != NULL ) ;
	DisposeWindow( inWindowRef ) ;
}

void 			UMASetWTitleC( WindowRef inWindowRef , const char *title )
{
	Str255	ptitle ;
	strncpy( (char*)ptitle , title , 96 ) ;
	ptitle[96] = 0 ;
#if TARGET_CARBON
	c2pstrcpy( ptitle, (char *)ptitle ) ;
#else
	c2pstr( (char*)ptitle ) ;
#endif
	SetWTitle( inWindowRef , ptitle ) ;
}

void 			UMAGetWTitleC( WindowRef inWindowRef , char *title )
{
	GetWTitle( inWindowRef , (unsigned char*)title ) ;
#if TARGET_CARBON
	p2cstrcpy( title, (unsigned char *)title ) ;
#else
	p2cstr( (unsigned char*)title ) ;
#endif
}

// appearance additions

void UMAActivateControl( ControlHandle inControl )
{
    // we have to add the control after again to the update rgn
    // otherwise updates get lost
  if ( !IsControlActive( inControl ) )
  {
        bool visible = IsControlVisible( inControl ) ;
        if ( visible )
          SetControlVisibility( inControl , false , false ) ;
        ::ActivateControl( inControl ) ;
        if ( visible ) {
          SetControlVisibility( inControl , true , false ) ;
          Rect ctrlBounds ;
          InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
        }
  }
}

void UMADrawControl( ControlHandle inControl )
{
    WindowRef theWindow = GetControlOwner(inControl) ;
    RgnHandle updateRgn = NewRgn() ;
    GetWindowUpdateRgn( theWindow , updateRgn ) ;
	  Point zero = { 0 , 0 } ;
	  LocalToGlobal( &zero ) ;
	  OffsetRgn( updateRgn , -zero.h , -zero.v ) ;
    ::DrawControlInCurrentPort( inControl ) ;
    InvalWindowRgn( theWindow, updateRgn) ;
	  DisposeRgn( updateRgn ) ;
}

void UMAMoveControl( ControlHandle inControl , short x , short y )
{
  bool visible = IsControlVisible( inControl ) ;
  if ( visible ) {
    SetControlVisibility( inControl , false , false ) ;
    Rect ctrlBounds ;
    InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
  }
  ::MoveControl( inControl , x , y ) ;
  if ( visible ) {
    SetControlVisibility( inControl , true , false ) ;
    Rect ctrlBounds ;
    InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
  }
}

void UMASizeControl( ControlHandle inControl , short x , short y )
{
  bool visible = IsControlVisible( inControl ) ;
  if ( visible ) {
    SetControlVisibility( inControl , false , false ) ;
    Rect ctrlBounds ;
    InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
  }
  ::SizeControl( inControl , x , y ) ;
  if ( visible ) {
    SetControlVisibility( inControl , true , false ) ;
    Rect ctrlBounds ;
    InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
  }
}

void UMADeactivateControl( ControlHandle inControl )
{
    // we have to add the control after again to the update rgn
    // otherwise updates get lost
     bool visible = IsControlVisible( inControl ) ;
     if ( visible )
       SetControlVisibility( inControl , false , false ) ;
     ::DeactivateControl( inControl ) ;
     if ( visible ) {
       SetControlVisibility( inControl , true , false ) ;
       Rect ctrlBounds ;
       InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
     }
}
// shows the control and adds the region to the update region
void UMAShowControl						(ControlHandle 			inControl)
{
        SetControlVisibility( inControl , true , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
}

// shows the control and adds the region to the update region
void UMAHideControl						(ControlHandle 			inControl)
{
        SetControlVisibility( inControl , false , false ) ;
        Rect ctrlBounds ;
        InvalWindowRect(GetControlOwner(inControl),GetControlBounds(inControl,&ctrlBounds) ) ;
}
// keyboard focus
OSErr UMASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart)
{
	OSErr err = noErr;
	GrafPtr port ;
	GetPort( &port ) ;

	SetPortWindowPort( inWindow ) ;

  err = SetKeyboardFocus( inWindow , inControl , inPart ) ;
	SetPort( port ) ;
	return err ;
}




// events
void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn )
{
  RgnHandle updateRgn = NewRgn() ;
  GetWindowUpdateRgn( inWindow , updateRgn ) ;

	Point zero = { 0 , 0 } ;
	LocalToGlobal( &zero ) ;
	OffsetRgn( updateRgn , -zero.h , -zero.v ) ;

  UpdateControls( inWindow , inRgn ) ;
  InvalWindowRgn( inWindow, updateRgn) ;
	DisposeRgn( updateRgn ) ;

}

bool UMAIsWindowFloating( WindowRef inWindow )
{
	WindowClass cl ;

	GetWindowClass( inWindow , &cl ) ;
	return cl == kFloatingWindowClass ;
}

bool UMAIsWindowModal( WindowRef inWindow )
{
	WindowClass cl ;

	GetWindowClass( inWindow , &cl ) ;
	return cl < kFloatingWindowClass ;
}

// others

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate )
{
	if ( inWindowRef )
	{
//		bool isHighlighted = IsWindowHighlited( inWindowRef ) ;
//		if ( inActivate != isHightlited )
		GrafPtr port ;
		GetPort( &port ) ;
		SetPortWindowPort( inWindowRef ) ;
		HiliteWindow( inWindowRef , inActivate ) ;
		ControlHandle control = NULL ;
		::GetRootControl( inWindowRef , & control ) ;
		if ( control )
		{
			if ( inActivate )
				UMAActivateControl( control ) ;
			else
				UMADeactivateControl( control ) ;
		}
		SetPort( port ) ;
	}
}
OSStatus UMADrawThemePlacard( const Rect *inRect , ThemeDrawState inState )
{
  return ::DrawThemePlacard( inRect , inState ) ;
}

#if !TARGET_CARBON
static OSStatus helpMenuStatus = noErr ;
static MenuItemIndex firstCustomItemIndex = 0 ;
#endif

OSStatus UMAGetHelpMenu(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex)
{
#if TARGET_CARBON
  return HMGetHelpMenu( outHelpMenu , outFirstCustomItemIndex ) ;
#else
  MenuRef helpMenuHandle ;
  helpMenuStatus = HMGetHelpMenuHandle( &helpMenuHandle ) ;
  if ( firstCustomItemIndex == 0 && helpMenuStatus == noErr )
  {
    firstCustomItemIndex = CountMenuItems( helpMenuHandle ) + 1 ;
  }
  if ( outFirstCustomItemIndex )
  {
    *outFirstCustomItemIndex = firstCustomItemIndex ;
  }
  *outHelpMenu = helpMenuHandle ;
  return helpMenuStatus ;
#endif
}

wxMacPortStateHelper::wxMacPortStateHelper( GrafPtr newport)
{
  m_clip = NULL ;
  Setup( newport ) ;
}

wxMacPortStateHelper::wxMacPortStateHelper()
{
	m_clip = NULL ;
}

void wxMacPortStateHelper::Setup( GrafPtr newport )
{
	GetPort( &m_oldPort ) ;
	SetPort( newport ) ;
	wxASSERT_MSG( m_clip == NULL , "Cannot call setup twice" ) ;
	m_clip = NewRgn() ;
	GetClip( m_clip );
	m_textFont = GetPortTextFont( (CGrafPtr) newport);
	m_textSize = GetPortTextSize( (CGrafPtr) newport);
	m_textStyle = GetPortTextFace( (CGrafPtr) newport);
	m_textMode = GetPortTextMode( (CGrafPtr) newport);
	GetThemeDrawingState( &m_drawingState ) ;
	m_currentPort = newport ;
}
void wxMacPortStateHelper::Clear()
{
	if ( m_clip )
	{
		DisposeRgn( m_clip ) ;
		DisposeThemeDrawingState( m_drawingState ) ;
		m_clip = NULL ;
	}
}

wxMacPortStateHelper::~wxMacPortStateHelper()
{
	if ( m_clip )
	{
		SetPort( m_currentPort ) ;
		SetClip( m_clip ) ;
		DisposeRgn( m_clip ) ;
		TextFont( m_textFont );
		TextSize( m_textSize );
		TextFace( m_textStyle );
		TextMode( m_textMode );
		SetThemeDrawingState( m_drawingState , true ) ;
		SetPort( m_oldPort ) ;
	}
}

OSStatus UMAPutScrap( Size size , OSType type , void *data )
{
	OSStatus err = noErr ;
#if !TARGET_CARBON
    err = PutScrap( size , type , data ) ;
#else
    ScrapRef    scrap;
    err = GetCurrentScrap (&scrap);
    if ( !err )
    {
        err = PutScrapFlavor (scrap, type , 0, size, data);
    }
#endif
	return err ;
}
