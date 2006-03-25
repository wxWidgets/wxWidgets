#include <wx/mac/uma.h>
#include <wx/mac/aga.h>

// init

static bool	sUMAHasAppearance = false ;
static long sUMAAppearanceVersion = 0 ;
extern int gAGABackgroundColor ;
bool UMAHasAppearance() { return sUMAHasAppearance ; }
long UMAGetAppearanceVersion() { return sUMAAppearanceVersion ; }

static bool	sUMAHasWindowManager = false ;
static long sUMAWindowManagerAttr = 0 ;

bool UMAHasWindowManager() { return sUMAHasWindowManager ; }
long UMAGetWindowManagerAttr() { return sUMAWindowManagerAttr ; }

void UMAInitToolbox( UInt16 inMoreMastersCalls )
{
#if !TARGET_CARBON
	::MaxApplZone();
	for (long i = 1; i <= inMoreMastersCalls; i++)
		::MoreMasters();

	::InitGraf(&qd.thePort);
	::InitFonts();
	::InitWindows();
	::InitMenus();
	::TEInit();
	::InitDialogs(0L);
	::FlushEvents(everyEvent, 0);
	::InitCursor();
	long total,contig;
	PurgeSpace(&total, &contig);
#else
	InitMenus() ;
#endif

#if UMA_USE_APPEARANCE
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
#endif // UMA_USE_APPEARANCE
#if UMA_USE_8_6
#if UMA_USE_WINDOWMGR
	if ( Gestalt( gestaltWindowMgrAttr, &sUMAWindowManagerAttr ) == noErr )
	{
		sUMAHasWindowManager = sUMAWindowManagerAttr & gestaltWindowMgrPresent ;
	}
#endif // UMA_USE_WINDOWMGR
#endif
}

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

void UMASetMenuTitle( MenuRef menu , ConstStr255Param title )
{
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
	SetMenuTitle( menu , title ) ;
#endif
}

UInt32 UMAMenuEvent( EventRecord *inEvent )
{
	if ( UMAHasAppearance() )
	{
		return MenuEvent( inEvent ) ;
	}
	else
	{
			if ( inEvent->what == keyDown && inEvent->modifiers & cmdKey)
			{
				return MenuKey( inEvent->message & charCodeMask ) ;
			}
		return NULL ;
	}
}

void 			UMAEnableMenuItem( MenuRef inMenu , MenuItemIndex inItem ) 
{
#if UMA_USE_8_6
	EnableMenuItem( inMenu , inItem ) ;
#else
	EnableItem( inMenu , inItem ) ;
#endif
}

void 			UMADisableMenuItem( MenuRef inMenu , MenuItemIndex inItem ) 
{
#if UMA_USE_8_6
	DisableMenuItem( inMenu , inItem ) ;
#else
	DisableItem( inMenu , inItem ) ;
#endif
}
// quickdraw

#if !TARGET_CARBON

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
		Cursor preservedArrow;
		GetQDGlobalsArrow (&preservedArrow);
		SetQDGlobalsArrow (*watchFob);
		InitCursor ( );
		SetQDGlobalsArrow (&preservedArrow);
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
	return GetWindowPort( inWindowRef ) ; 
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
	c2pstr( (char*)ptitle ) ;
	SetWTitle( inWindowRef , ptitle ) ;
}
void 			UMAGetWTitleC( WindowRef inWindowRef , char *title ) 
{
	GetWTitle( inWindowRef , (unsigned char*)title ) ;
	p2cstr( (unsigned char*)title ) ;
}

void			UMAShowWindow( WindowRef inWindowRef ) 
{
	ShowWindow( inWindowRef ) ;
}

void			UMAHideWindow( WindowRef inWindowRef ) 
{
	HideWindow( inWindowRef) ;
}

void 			UMASelectWindow( WindowRef inWindowRef ) 
{
	SelectWindow( inWindowRef ) ;
}

void			UMABringToFront( WindowRef inWindowRef ) 
{
	BringToFront( inWindowRef ) ;
}

void			UMASendBehind( WindowRef inWindowRef , WindowRef behindWindow ) 
{
	SendBehind( inWindowRef , behindWindow ) ;
}

void			UMACloseWindow(WindowRef inWindowRef) 
{
#if TARGET_CARBON
#else
	CloseWindow( inWindowRef ) ;
#endif
}

// appearance additions

void UMAActivateControl( ControlHandle inControl ) 
{
	if ( UMAHasAppearance() )
	{
   	::ActivateControl( inControl ) ;
   }
   else
   {
   	AGAActivateControl( inControl ) ;
   }
}

void UMADrawControl( ControlHandle inControl ) 
{
	if ( UMAHasAppearance() )
	{
   	::DrawControlInCurrentPort( inControl ) ;
   }
   else
   {
   	AGADrawControl( inControl ) ;
   }
}

void UMAMoveControl( ControlHandle inControl , short x , short y ) 
{
	if ( UMAHasAppearance() )
	{
	  	::MoveControl( inControl , x , y ) ;
   }
   else
   {
   	AGAMoveControl( inControl , x ,y  ) ;
   }
}

void UMASizeControl( ControlHandle inControl , short x , short y ) 
{
	if ( UMAHasAppearance() )
	{
	  	::SizeControl( inControl , x , y ) ;
   }
   else
   {
   	AGASizeControl( inControl , x ,y  ) ;
   }
}

void UMADeactivateControl( ControlHandle inControl ) 
{
	if ( UMAHasAppearance() )
	{
   	::DeactivateControl( inControl ) ;
   }
   else
   {
   	 AGADeactivateControl( inControl ) ;
   }
}

void			UMASetThemeWindowBackground		(WindowRef 				inWindow,
								 ThemeBrush 			inBrush,
								 Boolean 				inUpdate){
	if ( UMAHasAppearance() )
	{
   	::SetThemeWindowBackground( inWindow ,inBrush , inUpdate ) ;
   }
   else
   {
   	AGASetThemeWindowBackground( inWindow , inBrush , inUpdate ) ;
   }
}


ControlHandle UMANewControl(WindowPtr 				owningWindow,
								 const Rect *			boundsRect,
								 ConstStr255Param 		controlTitle,
								 Boolean 				initiallyVisible,
								 SInt16 				initialValue,
								 SInt16 				minimumValue,
								 SInt16 				maximumValue,
								 SInt16 				procID,
								 SInt32 				controlReference)
{
	ControlHandle theControl = NULL ;
	if ( UMAHasAppearance() )
	{
   theControl =  NewControl( owningWindow , boundsRect , controlTitle , initiallyVisible ,
   		initialValue , minimumValue , maximumValue , procID , controlReference ) ;
	}
	else
	{
		theControl = AGANewControl( owningWindow , boundsRect , controlTitle , initiallyVisible ,
   		initialValue , minimumValue , maximumValue , procID , controlReference ) ;
	}
	return theControl ;
}

void UMADisposeControl (ControlHandle 			theControl)
{
	if ( UMAHasAppearance() )
	{
   	::DisposeControl( theControl ) ;
   }
   else
   {
   	::DisposeControl( theControl ) ;
   }
}


void UMAHiliteControl	(ControlHandle 			theControl,
								 ControlPartCode 		hiliteState)
								 {
	if ( UMAHasAppearance() )
	{
   	::HiliteControl( theControl , hiliteState ) ;
   }
   else
   {
   	::HiliteControl( theControl , hiliteState ) ;
	}
}


void UMAShowControl						(ControlHandle 			theControl)
{
	if ( UMAHasAppearance() )
	{
   	::ShowControl( theControl ) ;
   }
   else
   {
   	::ShowControl( theControl ) ;
   }
}


void UMAHideControl						(ControlHandle 			theControl)
{
	if ( UMAHasAppearance() )
	{
   	::HideControl( theControl ) ;
   }
   else
   {
   	::HideControl( theControl ) ;
   }
}


void UMASetControlVisibility			(ControlHandle 			inControl,
								 Boolean 				inIsVisible,
								 Boolean 				inDoDraw)
								 {
	if ( UMAHasAppearance() )
	{
   	::SetControlVisibility( inControl , inIsVisible, inDoDraw ) ;
   }
}



bool UMAIsControlActive					(ControlHandle 			inControl)
{
	if ( UMAHasAppearance() )
	{
   	return IsControlActive( inControl ) ;
   }
  else
  	return (**inControl).contrlHilite == 0 ;
}


bool UMAIsControlVisible				(ControlHandle 			inControl)
{
	if ( UMAHasAppearance() )
	{
   	return IsControlVisible( inControl ) ;
   }
  	return true ;
}

OSErr UMAGetBestControlRect				(ControlHandle 			inControl,
								 Rect *					outRect,
								 SInt16 *				outBaseLineOffset)
{
	if ( UMAHasAppearance() )
	{
   	return GetBestControlRect( inControl , outRect , outBaseLineOffset ) ;
   }
  else
  {
  	return AGAGetBestControlRect( inControl , outRect , outBaseLineOffset ) ;
  }
}


OSErr UMASetControlFontStyle				(ControlHandle 			inControl,
								 const ControlFontStyleRec * inStyle)	
{
	if ( UMAHasAppearance() )
	{
		return ::SetControlFontStyle( inControl , inStyle ) ;
	}
	else
		return AGASetControlFontStyle( inControl , inStyle ) ;
}



// control hierarchy

OSErr UMACreateRootControl				(WindowPtr 				inWindow,
								 ControlHandle *		outControl)
{
	if ( UMAHasAppearance() )
	{
   	return CreateRootControl( inWindow , outControl  ) ;
   }
  else
  	return AGACreateRootControl( inWindow , outControl ) ;
}



OSErr UMAEmbedControl					(ControlHandle 			inControl,
								 ControlHandle 			inContainer)
{
	if ( UMAHasAppearance() )
	{
   	return EmbedControl( inControl , inContainer ) ;
   }
  else
  	return AGAEmbedControl( inControl , inContainer ) ; ;
}



// keyboard focus
OSErr UMASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart)
{
	if ( UMAHasAppearance() )
	{
   	return SetKeyboardFocus( inWindow , inControl , inPart ) ;
   }
  else
  	return AGASetKeyboardFocus( inWindow , inControl , inPart ) ;
}




// events

ControlPartCode UMAHandleControlClick				(ControlHandle 			inControl,
								 Point 					inWhere,
								 SInt16 				inModifiers,
								 ControlActionUPP 		inAction)		 
{
	if ( UMAHasAppearance() )
	{
   	return HandleControlClick( inControl , inWhere , inModifiers , inAction ) ;
   }
   else
   {
   	return AGAHandleControlClick( inControl , inWhere , inModifiers , inAction ) ;
   }
}


SInt16 UMAHandleControlKey				(ControlHandle 			inControl,
								 SInt16 				inKeyCode,
								 SInt16 				inCharCode,
								 SInt16 				inModifiers)
{
	if ( UMAHasAppearance() )
	{
   	return HandleControlKey( inControl , inKeyCode , inCharCode , inModifiers ) ;
   }
   else
   {
   	return AGAHandleControlKey(inControl , inKeyCode , inCharCode , inModifiers ) ;
   }
}


								 
void UMAIdleControls					(WindowPtr 				inWindow)
{
	if ( UMAHasAppearance() )
	{
   		IdleControls( inWindow ) ;
   }
   else
   {
   		AGAIdleControls( inWindow ) ;
   }
}

void UMAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) 
{
	if ( UMAHasAppearance() )
	{
		UpdateControls( inWindow , inRgn ) ;
	}
	else
	{
		AGAUpdateControls( inWindow , inRgn ) ;
	}
}

OSErr UMAGetRootControl( WindowPtr inWindow , ControlHandle *outControl ) 
{
	if ( UMAHasAppearance() )
	{
		return GetRootControl( inWindow , outControl ) ;
	}
	else
	{
		return AGAGetRootControl( inWindow , outControl ) ;
	}
}


// handling control data

OSErr UMASetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inSize,
								 Ptr 					inData)
{
	if ( UMAHasAppearance() )
	{
   	 return SetControlData( inControl , inPart , inTagName , inSize , inData ) ;
   }
   else
		return AGASetControlData( inControl , inPart , inTagName , inSize , inData ) ;
}



OSErr UMAGetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inBufferSize,
								 Ptr 					outBuffer,
								 Size *					outActualSize)
{
	if ( UMAHasAppearance() )
	{
		return ::GetControlData( inControl , inPart , inTagName , inBufferSize , outBuffer , outActualSize ) ;
	}
	else
	{
		return AGAGetControlData( inControl , inPart , inTagName , inBufferSize , outBuffer , outActualSize ) ;
	}
	
}


OSErr UMAGetControlDataSize				(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size *					outMaxSize)
{
	if ( UMAHasAppearance() )
	{
		return GetControlDataSize( inControl , inPart , inTagName , outMaxSize ) ;
	}
	else
	{
		return AGAGetControlDataSize( inControl , inPart , inTagName , outMaxSize ) ;
	}
}





// system 8.0 changes

short UMAFindWindow( Point inPoint , WindowRef *outWindow )
{
	// todo add the additional area codes
	return FindWindow( inPoint , outWindow ) ;
}

OSStatus UMAGetWindowFeatures( WindowRef inWindowRef , UInt32 *outFeatures )
{
#if UMA_USE_WINDOWMGR
	return GetWindowFeatures( inWindowRef , outFeatures ) ;
#else
	return 0 ;
#endif
}

OSStatus UMAGetWindowRegion( WindowRef inWindowRef , WindowRegionCode inRegionCode , RgnHandle ioWinRgn )
{
#if UMA_USE_WINDOWMGR
	return GetWindowRegion( inWindowRef , inRegionCode , ioWinRgn ) ;
#else
	return 0 ;
#endif
}

void UMADrawGrowIcon( WindowRef inWindowRef )
{
	DrawGrowIcon( inWindowRef ) ;
}

OSStatus UMACollapseWindow( WindowRef inWindowRef , Boolean inCollapseIt )
{
	return CollapseWindow( inWindowRef , inCollapseIt ) ;
}

OSStatus UMACollapseAllWindows( Boolean inCollapseEm )
{
	return CollapseAllWindows( inCollapseEm ) ;
}

Boolean UMAIsWindowCollapsed( WindowRef inWindowRef )
{
	return IsWindowCollapsed( inWindowRef ) ;
}

Boolean UMAIsWindowCollapsable( WindowRef inWindowRef )
{
	return IsWindowCollapsable( inWindowRef ) ;
}

// system 8.5 changes<MacWindows.h>
OSStatus 	UMACreateNewWindow( WindowClass windowClass , WindowAttributes attributes , const Rect *bounds,  WindowRef *outWindow ) 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return CreateNewWindow( windowClass , attributes, bounds, outWindow ) ;
	}
	else
#endif
	{
		short procID ;
		if ( UMAHasAppearance() )
		{
			switch( windowClass )
			{
				case kMovableModalWindowClass :
					procID = kWindowMovableModalDialogProc;
					break ;
				case kDocumentWindowClass :
					procID = kWindowFullZoomGrowDocumentProc;
					break ;
				default :
					procID = kWindowMovableModalDialogProc;
					break ;
			}
		}
		else
		{
			switch( windowClass )
			{
				case kMovableModalWindowClass :
					procID = movableDBoxProc;
//					procID += kMovableModalDialogVariantCode;
					break ;
				case kDocumentWindowClass :
					procID = zoomDocProc;
					break ;
				default :
					procID = documentProc;
					break ;
			}
		}
		*outWindow = NewCWindow(nil, bounds, "\p", false, procID,  (WindowRef) -1 /*behind*/,
				attributes & kWindowCloseBoxAttribute , (long)NULL);
		return noErr ;		
	}
}

OSStatus 	UMAGetWindowClass( WindowRef inWindowRef , WindowClass *outWindowClass ) 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return GetWindowClass( inWindowRef , outWindowClass ) ;
	}
	else
#endif
		return kDocumentWindowClass ;
}

OSStatus	UMAGetWindowAttributes( WindowRef inWindowRef , WindowAttributes *outAttributes ) 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return GetWindowAttributes( inWindowRef , outAttributes ) ;
	}
#endif
	return kWindowNoAttributes ;
}

void UMAShowFloatingWindows() 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		ShowFloatingWindows() ;
	}
#endif
}

void UMAHideFloatingWindows() 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		HideFloatingWindows() ;
	}
#endif
}

Boolean 	UMAAreFloatingWindowsVisible() 
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return AreFloatingWindowsVisible() ;
	}
#endif
	return false ;
}

WindowRef UMAFrontNonFloatingWindow()
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return FrontNonFloatingWindow() ;
	}
	else
#endif
	{
		return FrontWindow() ;
	}
}

WindowRef UMAFrontWindow()
{
#if UMA_USE_WINDOWMGR
	if ( UMAHasWindowManager() )
	{
		return FrontWindow() ;
	}
	else
#endif
	{
		return FrontWindow() ;
	}
}

WindowRef UMAGetActiveNonFloatingWindow()
{
	return NULL ;
}

bool UMAIsWindowFloating( WindowRef inWindow ) 
{
	WindowClass cl ;
	
	UMAGetWindowClass( inWindow , &cl ) ;
	return cl == kFloatingWindowClass ;
}

bool UMAIsWindowModal( WindowRef inWindow ) 
{
	WindowClass cl ;
	
	UMAGetWindowClass( inWindow , &cl ) ;
	return cl < kFloatingWindowClass ;
}

// others

void UMAHighlightAndActivateWindow( WindowRef inWindowRef , bool inActivate )
{
	if ( inWindowRef )
	{
//		bool isHighlighted = IsWindowHighlited( inWindowRef ) ;
//		if ( inActivate != isHightlited )
				HiliteWindow( inWindowRef , inActivate ) ;
	}
}

