#include <wx/mac/uma.h>
#include <wx/mac/aga.h>

#if !TARGET_CARBON

#include <extcdef.h>
#include <PictUtils.h>

pascal SInt32 AGAProgressBarDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param);
pascal SInt32 AGAPlacardDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param);
pascal SInt32 AGAStaticTextDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param) ;
pascal SInt32 AGAListControlDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param) ;
pascal SInt32 AGAEditTextDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param) ;
pascal SInt32 AGAStaticGroupBoxTextDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param) ;

#define B														13
#define	W														0

SInt16 kAGARootControlProcID ;
int gAGABackgroundColor = 0 ;

const RGBColor gAGAColorArray[] = {	{0xFFFF, 0xFFFF, 0xFFFF},		//	W
					{0xEEEE, 0xEEEE, 0xEEEE}, 	//	1
					{0xDDDD, 0xDDDD, 0xDDDD}, 	//	2
					{0xCCCC, 0xCCCC, 0xCCCC}, 	//	3
					{0xBBBB, 0xBBBB, 0xBBBB}, 	//	4
					{0xAAAA, 0xAAAA, 0xAAAA}, 	//	5
					{0x9999, 0x9999, 0x9999}, 	//	6
					{0x8888, 0x8888, 0x8888},		//	7
					{0x7777, 0x7777, 0x7777},		//	8
					{0x6666, 0x6666, 0x6666},		//	9
					{0x5555, 0x5555, 0x5555},		//	10
					{0x4444, 0x4444, 0x4444},		//	11
					{0x2222, 0x2222, 0x2222},		//	12
					{0x0000, 0x0000, 0x0000}		//	B
};

char LAGADefaultOutline_mCorners[4][5][5] =	{
  //	topleft
  {
    { -1, -1, -1, 12,  B },
    { -1, -1,  B,  2,  2 },
    { -1,  B,  2,  2,  5 },
    { 12,  2,  2,  2,  8 },
    {  B,  2,  5,  8, -1 }
  },
  //	topright
  {
    {  B, 12, -1, -1, -1 },
    {  2,  3,  B, -1, -1 },
    {  5,  5,  4,  B, -1 },
    {  8,  5,  5,  7, 12 },
    { -1,  8,  5,  8,  B }
  },
  //	bottomleft
  {
    {  B,  2,  5,  8, -1 },
    { 12,  3,  5,  5,  8 },
    { -1,  B,  4,  5,  5 },
    { -1, -1,  B,  7,  8 },
    { -1, -1, -1, 12,  B }
  },
  //	bottomright
  {
    { -1,  8,  5,  8,  B },
    {  8,  5,  7,  8, 12 },
    {  5,  7,  8,  B, -1 },
    {  8,  8,  B, -1, -1 },
    {  B, 12, -1, -1, -1 }
  }
};

char LAGAPushButton_mCorners[3][4][4][4] =	{
  //	Enabled
  {
    //	topleft
    {
      { -1, -1, 12,  B },
      { -1,  B,  4,  2 },
      { 12,  4,  W,  W },
      {  B,  2,  W,  W }
    },
    //	topright
    {
      {  B, 12, -1, -1 },
      {  2,  4,  B, -1 },
      {  W,  2,  5, 12 },
      {  2,  5,  8,  B }
    },
    //	bottomleft
    {
      {  B,  2,  W,  2 },
      { 12,  4,  2,  5 },
      { -1,  B,  4,  8 },
      { -1, -1, 12,  B }
    },
    //	bottomright
    {
      {  5,  5,  8,  B },
      {  5,  8,  8, 12 },
      {  8,  8,  B, -1 },
      {  B, 12, -1, -1 }
    }
  },
  //	Pressed
  {
    //	topleft
    {
      { -1, -1, 12,  B },
      { -1,  B, 10,  8 },
      { 12, 10,  8,  4 },
      {  B,  8,  4,  4 }
    },
    //	topright
    {
      {  B, 12, -1, -1 },
      {  8, 10,  B, -1 },
      {  4,  4,  8, 12 },
      {  4,  4,  4,  B }
    },
    //	bottomleft
    {
      {  B,  8,  4,  4 },
      { 12, 10,  4,  4 },
      { -1,  B, 10,  4 },
      { -1, -1, 12,  B }
    },
    //	bottomright
    {
      {  4,  4,  4,  B },
      {  4,  4, 10, 12 },
      {  4, 10,  B, -1 },
      {  B, 12, -1, -1 }
    }
  },
  //	Disabled
  {
    //	topleft
    {
      { -1, -1,  7,  7 },
      { -1,  7,  4,  2 },
      {  7,  4,  1,  1 },
      {  7,  2,  1,  1 }
    },
    //	topright
    {
      {  7,  7, -1, -1 },
      {  2,  4,  7, -1 },
      {  1,  2,  4,  7 },
      {  2,  4,  5,  7 }
    },
    //	bottomleft
    {
      {  7,  2,  1,  2 },
      {  7,  4,  2,  4 },
      { -1,  7,  4,  5 },
      { -1, -1,  7,  7 }
    },
    //	bottomright
    {
      {  4,  4,  5,  7 },
      {  4,  5,  5,  7 },
      {  5,  5,  7, -1 },
      {  7,  7, -1, -1 }
    }
  }
};

RGBColor gAGARamp[] =
{
	/* black */
	{ 0 , 0 , 0 } ,
	/* white */
	{ 65535 , 65535 , 65535 } ,
	/* 1 */
	{ 61166 , 61166 , 61166 } ,
	/* 2 */
	{ 56797 , 56797 , 56797 } ,
	/* 3 */
	{ 52428 , 52428 , 52428 } ,
	/* 4 */
	{ 48059 , 48059 , 48059 } ,
	/* 5 */
	{ 43690 , 43690 , 43690 } ,
	/* 6 */
	{ 39321 , 39321 , 39321 } ,
	/* 7 */
	{ 34952 , 34952 , 34952 } ,
	/* 8 */
	{ 30583 , 30583 , 30583 } ,
	/* 9 */
	{ 26214 , 26214 , 26214 } ,
	/* 10 */
	{ 21845 , 21845 , 21845 } ,
	/* 11 */
	{ 17476 , 17476 , 17476 } ,
	/* 12 */
	{ 13107 , 13107 , 13107 } ,
	/* A1 */
	{ 8738 , 8738 , 8738 } ,
	/* A2 */
	{ 4369 , 4369 , 4369 } ,
	
} ;

RGBColor gAGABlueRamp[] =
{
	/* black */
	{ 0 , 0 , 0 } ,
	/* white */
	{ 65535 , 65535 , 65535 } ,
	/* 1 */
	{ 61166 , 61166 , 65535 } ,
	/* 2 */
	{ 56797 , 56797 , 65535 } ,
	/* 3 */
	{ 52428 , 52428 , 65535 } ,
	/* 4 */
	{ 48059 , 48059 , 65535 } ,
	/* 5 */
	{ 43690 , 43690 , 65535 } ,
	/* 6 */
	{ 39321 , 39321 , 65535 } ,
	/* 7 */
	{ 34952 , 34952 , 65535 } ,
	/* 8 */
	{ 30583 , 30583 , 65535 } ,
	/* 9 */
	{ 26214 , 26214 , 65535 } ,
	/* 10 */
	{ 21845 , 21845 , 65535 } ,
	/* 11 */
	{ 17476 , 17476 , 65535 } ,
	/* 12 */
	{ 13107 , 13107 , 65535 } ,
	/* A1 */
	{ 8738 , 8738 , 65535 } ,
	/* A2 */
	{ 4369 , 4369 , 65535 } ,
	
} ;

enum eAGARampColor { 
	kAGATransparent = -1 ,
	kAGABlack = 0 ,
	kAGAWhite = 1 ,
	kAGA1 = 2 ,
	kAGA2 = 3 ,
	kAGA3 = 4 ,
	kAGA4 = 5 ,
	kAGA5 = 6 ,
	kAGA6 = 7 ,
	kAGA7 = 8 ,
	kAGA8 = 9 ,
	kAGA9 = 10 ,
	kAGA10 = 11 ,
	kAGA11 = 12 ,
	kAGA12 = 13 ,
	kAGAA1 = 14 ,
	kAGAA2 = 15 ,
} ;
// defprocs

enum eAGAState {
	kAGAStateEnabled = 0 ,
	kAGAStatePressed = 1 ,
	kAGAStateDisabled = 2 ,
	kAGAStateInactive = 3 
} ;

/*
typedef struct sControlData
{
	ControlPartCode 	m_part ;
	ResType 					m_tag ;
	Size 							m_size ;
} ;

OSErr AGAFindControlData( ControlHandle inControl , ControlPartCode inPart , ResType inTagName , long *outOffset) ;
OSErr AGAFindControlData( ControlHandle inControl , ControlPartCode inPart , ResType inTagName , long *outOffset) 
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	long offset = 0 ;
	long size = GetHandleSize( info->data ) ;
	char* data = *(info->data) ;
	sControlData* current ;
	while( offset + sizeof( sControlData ) <= size )
	{
		current = (sControlData*) (data+offset ) ;
		if ( ( inPart == kControlEntireControl || inPart == current->m_part ) && inTagName == current->m_tag )
		{
			*outOffset = offset ;
			return noErr ;
		}
	}
	return errDataNotSupported ; // nothing found
}

OSErr AGARemoveControlData( ControlHandle inControl , ControlPartCode inPart , ResType inTagName ) ;
OSErr AGARemoveControlData( ControlHandle inControl , ControlPartCode inPart , ResType inTagName ) 
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	long offset = 0 ;
	long size = GetHandleSize( info->data ) ;
	char* data = *(info->data) ;
	sControlData* current ;
	while( offset + sizeof( sControlData ) <= size )
	{
		current = (sControlData*) (data+offset ) ;
		if ( ( inPart == kControlEntireControl || inPart == current->m_part ) && inTagName == current->m_tag )
		{
			long chunkSize = current->m_size + sizeof ( sControlData ) ;
			memcpy( data + offset , data + offset + chunkSize , size - offset - chunkSize ) ;
			SetHandleSize( info->data , size - chunkSize ) ;
			return noErr ;
		}
	}
	return noErr ;
}

OSErr AGAAddControlData( ControlHandle inControl , ControlPartCode inPart , ResType inTagName , Size	inSize, Ptr inData) ;
OSErr AGAAddControlData( ControlHandle inControl , ControlPartCode inPart , ResType inTagName , Size	inSize, Ptr inData) 
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	long offset = 0 ;
	long size = GetHandleSize( info->data ) ;
	SetHandleSize( info->data , size + sizeof( sControlData ) + inSize ) ;
	if (  MemError() == noErr )
	{
		char* data = *(info->data) ;
		sControlData* current = (sControlData*)( data + size ) ;
		current->m_tag = inTagName ;
		current->m_part = inPart ;
		current->m_size = inSize ;
		memcpy( data + size + sizeof( sControlData ) , inData , inSize ) ;
		return noErr ;
	}
	return errDataNotSupported ;
}


OSErr AGAGetControlDataSize				(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size *					outMaxSize)
{
	long offset ;
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	char* data = *(info->data) ;
	
	OSErr err = AGAFindControlData( inControl , inPart , inTagName , &offset ) ;
	if ( !err )
	{
		sControlData* current = (sControlData*) (data+offset ) ;
		*outMaxSize = current->m_size ;
	}
	return err ;
}

OSErr AGAGetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inBufferSize,
								 Ptr 					outBuffer,
								 Size *					outActualSize)
{
	long offset ;
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	char* data = *(info->data) ;
	
	OSErr err = AGAFindControlData( inControl , inPart , inTagName , &offset ) ;
	if ( !err )
	{
		sControlData* current = (sControlData*) (data+offset ) ;
		*outActualSize = current->m_size ;
		if ( outBuffer )
		{
			if ( inBufferSize <= current->m_size )
				memcpy( outBuffer , data + offset + sizeof( sControlData )  , current->m_size ) ;
			else
				err = errDataNotSupported ;
		}
	}
	return err ;	
}

OSErr AGASetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inSize,
								 Ptr 					inData)
{
	long offset ;
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	char* data = *(info->data) ;
	
	OSErr err = AGAFindControlData( inControl , inPart , inTagName , &offset ) ;
	if ( !err )
	{
		sControlData* current = (sControlData*) (data+offset ) ;
		if ( current->m_size == inSize )
		{
				memcpy( data + offset + sizeof( sControlData )  , inData , inSize) ;
		}
		else
		{
			AGARemoveControlData( inControl , inPart , inTagName ) ;
			AGAAddControlData( inControl , inPart , inTagName , inSize , inData ) ;
		}
	}
	else
	{
		AGAAddControlData( inControl , inPart , inTagName , inSize , inData ) ;
	}
	return err ;	
}
*/

OSErr AGASetControlFontStyle				(ControlHandle 			inControl,
								 const ControlFontStyleRec * inStyle)	
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	if ( inStyle->flags == 0 )
	{
		// back to defaults
	}
	else
	{
		if ( inStyle->flags & kControlUseFontMask )
		{
			info->fontStyle.font = inStyle->font ;
		}
		if ( inStyle->flags & kControlUseSizeMask )
		{
			info->fontStyle.size = inStyle->size ;
		}
		if ( inStyle->flags & kControlUseFaceMask )
		{
			info->fontStyle.style = inStyle->style ;
		}
		if ( inStyle->flags & kControlUseModeMask )
		{
			info->fontStyle.mode = inStyle->mode ;
		}
		if ( inStyle->flags & kControlUseJustMask )
		{
			info->fontStyle.just = inStyle->just ;
		}
		if ( inStyle->flags & kControlUseForeColorMask )
		{
			info->fontStyle.foreColor = inStyle->foreColor ;
		}
		if ( inStyle->flags & kControlUseBackColorMask )
		{
			info->fontStyle.backColor = inStyle->backColor ;
		}
	}
	return noErr ;
}

OSErr AGASetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inSize,
								 Ptr 					inData)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	ControlDataAccessRec rec ;
	
	rec.tag = inTagName ;
	rec.part = inPart ;
	rec.size = inSize ;
	rec.dataPtr = inData ;
	
	return info->defProc( info->procID , inControl , kControlMsgSetData , (long) &rec ) ;
}

SInt16 AGAHandleControlKey				(ControlHandle 			inControl,
								 SInt16 				inKeyCode,
								 SInt16 				inCharCode,
								 SInt16 				inModifiers) 
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	ControlKeyDownRec rec ;
	
	rec.modifiers = inModifiers ;
	rec.keyCode = inKeyCode ;
	rec.charCode = inCharCode ;
	
	return info->defProc( info->procID , inControl , kControlMsgKeyDown , (long) &rec ) ;
}

ControlPartCode AGAHandleControlClick				(ControlHandle 			inControl,
								 Point 					inWhere,
								 SInt16 				inModifiers,
								 ControlActionUPP 		inAction)	
{
	return TrackControl( inControl , inWhere , inAction ) ;
}

Boolean AGATestForNewControl( ControlHandle inControl )
{
	if ( (**inControl).contrlRfCon  > 0x100 && !( (**inControl).contrlRfCon % 2 ) )
	{
			ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
			return info->magic == kExtCDEFMagic ;
	}
	return FALSE ;
}
void AGAIdleControls					(WindowPtr 				inWindow) 
{
	ControlHandle iter = (ControlHandle) ((WindowPeek)inWindow)->controlList ; 
	while ( iter )
	{
		if ( AGATestForNewControl( iter ) )
		{
			ExtCDEFInfo* info = (ExtCDEFInfo* )  (**iter).contrlRfCon ;
			if ( info->defProc( info->procID , iter , kControlMsgTestNewMsgSupport , 0 ) == kControlSupportsNewMessages )
			{
				if ( info->defProc( info->procID , iter , kControlMsgGetFeatures , 0 ) & kControlWantsIdle )
				{
					info->defProc( info->procID , iter , kControlMsgIdle , 0 ) ;
				}
			}
		}
	
		iter = (**iter).nextControl ;
	}
}

void AGAUpdateOneControl( ControlHandle control , RgnHandle inRgn )
{
	DrawOneControl( control ) ;
	ControlHandle iter = (ControlHandle) ((WindowPeek)(**control).contrlOwner)->controlList ; 
	while ( iter )
	{
		if ( AGATestForNewControl( iter ) )
		{
			ExtCDEFInfo* info = (ExtCDEFInfo* )  (**iter).contrlRfCon ;
			if ( info->containerControl == control )
			{ 		
				AGAUpdateOneControl( iter , inRgn ) ;
			}
		}
	
		iter = (**iter).nextControl ;
	}
}

void AGAUpdateControls( WindowPtr inWindow , RgnHandle inRgn ) 
{
	ControlHandle root ;
	AGAGetRootControl( inWindow , &root ) ;
	AGAUpdateOneControl( root , inRgn ) ;
	
	// and then all others
	
	ControlHandle iter = (ControlHandle) ((WindowPeek)inWindow)->controlList ; 
	while ( iter )
	{
		if ( AGATestForNewControl( iter ) )
		{
		}
		else
		{
			DrawOneControl( iter ) ;
		}
	
		iter = (**iter).nextControl ;
	}
}

OSErr AGAGetRootControl( WindowPtr inWindow , ControlHandle *outControl ) 
{
	ControlHandle iter = (ControlHandle) ((WindowPeek)inWindow)->controlList ; 
	while ( iter )
	{
		if ( AGATestForNewControl( iter ) )
		{
			ExtCDEFInfo* info = (ExtCDEFInfo* )  (**iter).contrlRfCon ;
			if ( info->procID == kAGARootControlProcID )
			{ 	
				*outControl  = iter ;
				return noErr ;
			}
		}
	
		iter = (**iter).nextControl ;
	}
	return -1 ;
}

void AGADeactivateControl( ControlHandle inControl ) 
{
	if ( AGATestForNewControl( inControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
		if ( info->defProc( info->procID , inControl , kControlMsgTestNewMsgSupport , 0 ) == kControlSupportsNewMessages )
		{
			if ( info->defProc( info->procID , inControl , kControlMsgGetFeatures , 0 ) & kControlWantsActivate )
			{
				info->defProc( info->procID , inControl , kControlMsgActivate , 0 ) ;
			}
		}
	}
	else
	{
		::HiliteControl( inControl , 255 ) ;
	}
}

void AGAActivateControl( ControlHandle inControl ) 
{
	if ( AGATestForNewControl( inControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
		if ( info->defProc( info->procID , inControl , kControlMsgTestNewMsgSupport , 0 ) == kControlSupportsNewMessages )
		{
			if ( info->defProc( info->procID , inControl , kControlMsgGetFeatures , 0 ) & kControlWantsActivate )
			{
				info->defProc( info->procID , inControl , kControlMsgActivate , 1 ) ;
			}
		}
	}
	else
	{
		::HiliteControl( inControl , 0 ) ;
	}
}

OSErr AGASetKeyboardFocus				(WindowPtr 				inWindow,
								 ControlHandle 			inControl,
								 ControlFocusPart 		inPart)
{
	if ( AGATestForNewControl( inControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
		if ( info->defProc( info->procID , inControl , kControlMsgTestNewMsgSupport , 0 ) == kControlSupportsNewMessages )
		{
			if ( info->defProc( info->procID , inControl , kControlMsgGetFeatures , 0 ) & kControlSupportsFocus )
			{
				return info->defProc( info->procID , inControl , kControlMsgFocus , inPart ) ;
			}
		}
	}
	return errControlDoesntSupportFocus ;
}

OSErr AGAGetBestControlRect				(ControlHandle 			inControl,
								 Rect *					outRect,
								 SInt16 *				outBaseLineOffset)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;
	ControlCalcSizeRec rec ;
	rec.width = (**inControl).contrlRect.right - (**inControl).contrlRect.left ;
	rec.height = (**inControl).contrlRect.bottom - (**inControl).contrlRect.top ;
	rec.baseLine = 0;
	
	OSErr err = info->defProc( info->procID , inControl , kControlMsgCalcBestRect , (long) &rec ) ;
	if ( !err )
	{
		outRect->left = outRect->top = 0 ;
		outRect->right = rec. width ;
		outRect->bottom = rec. height ;
		*outBaseLineOffset = rec. baseLine ;		
	}
	return err ;
}


OSErr AGAGetControlData					(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size 					inBufferSize,
								 Ptr 					outBuffer,
								 Size *					outActualSize)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon  ;
	ControlDataAccessRec rec ;
	
	rec.tag = inTagName ;
	rec.part = inPart ;
	rec.size = inBufferSize ;
	rec.dataPtr = outBuffer ;
	
	OSErr err = info->defProc( info->procID , inControl , kControlMsgGetData , (long) &rec ) ;
	*outActualSize = rec.size ;
	return err ;
}

OSErr AGAGetControlDataSize				(ControlHandle 			inControl,
								 ControlPartCode 		inPart,
								 ResType 				inTagName,
								 Size *					outMaxSize)
{
	return AGAGetControlData( inControl , inPart , inTagName , 0 , NULL , outMaxSize ) ;
}


void NewExtCDEFInfo( ControlHandle theControl , ControlDefProcPtr defproc , SInt16 procID , long refcon ) 
{
	ExtCDEFInfo* info = (ExtCDEFInfo* ) malloc( sizeof( ExtCDEFInfo ) ) ;
	info->defProc = defproc ;
	info->hasFocus = false ;
	info->magic = kExtCDEFMagic ;
	info->contrlRfCon = refcon ;
	info->procID = procID ;
	info->containerControl = NULL ;
	info->children = NewHandle(0) ;
	info->fontStyle.flags = 0 ;
	info->fontStyle.font = 0 ;
	info->fontStyle.size = 12 ;
	info->fontStyle.style = 0 ;
	info->fontStyle.mode = 0 ;
	info->fontStyle.just = 0 ;
	info->fontStyle.foreColor = gAGARamp[ kAGABlack ] ;
	info->fontStyle.backColor = gAGARamp[ kAGA2 ] ;
	(**theControl).contrlRfCon = (long) info ;	
	defproc( info->procID, theControl, initCntl, 0) ;
}

void DisposeExtCDEFInfo( ControlHandle theControl) 
{
	if ( AGATestForNewControl( theControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;
		if ( info )
		{
			DisposeHandle( info->children ) ;
			info->children = NULL ;
			free( (void*) (**theControl).contrlRfCon ) ;
			(**theControl).contrlRfCon = NULL ;
		}
	}
}

const short kAGAProgressHeight = 14 ;

void AGADrawRectPlacard( const Rect *inRect , eAGAState inState , bool fill = false ) ;
void AGADrawRectProgress( const Rect *inRect , eAGAState inState , double percentage ) ;

void AGADrawRectPlacard( const Rect *inRect , eAGAState inState , bool fill ) 
{
	Rect rect = *inRect ;
	RGBColor pixelColor ;
	if ( inState == kAGAStateInactive )
	{
		RGBForeColor( &gAGARamp[ kAGA10 ] ) ;
		FrameRect( &rect ) ;
		if ( fill )
		{
			InsetRect( &rect , 1 , 1 ) ;
			RGBForeColor( &gAGARamp[ kAGA1 ] ) ;
			PaintRect( &rect ) ;
		}
		return ;
	}

	RGBForeColor( &gAGARamp[ kAGABlack ] ) ;
	FrameRect( &rect ) ;
	InsetRect( &rect , 1 , 1 ) ;
	
	if ( inState == kAGAStateEnabled || inState == kAGAStateDisabled )
		RGBForeColor( &gAGARamp[ kAGAWhite ] ) ;
	else
		RGBForeColor( &gAGARamp[ kAGABlack ] ) ;
		
	MoveTo( rect.left , rect.bottom - 1 -1 ) ;
	LineTo( rect.left , rect.top ) ;
	LineTo( rect.right-1-1  , rect.top ) ;
	
	if ( inState == kAGAStateEnabled || inState == kAGAStateDisabled )
		pixelColor =  gAGARamp[ kAGA2 ] ;
	else
		pixelColor = gAGARamp[ kAGA8 ] ;

	SetCPixel( rect.right-1  , rect.top , &pixelColor ) ;
	SetCPixel( rect.left , rect.bottom-1  , &pixelColor ) ;
	
	if ( inState == kAGAStateEnabled )
		RGBForeColor( &gAGARamp[ kAGA5 ] ) ;
	else if ( inState == kAGAStateDisabled )
		RGBForeColor( &gAGARamp[ kAGA4 ] ) ;
	else
		RGBForeColor( &gAGARamp[ kAGA6 ] ) ;
	
	MoveTo( rect.left + 1 , rect.bottom-1  ) ;
	LineTo( rect.right-1  , rect.bottom-1  ) ;
	LineTo( rect.right-1  , rect.top-1 ) ;
	
	if ( fill )
	{
		InsetRect( &rect , 1 , 1 ) ;
		if ( inState == kAGAStateEnabled || inState == kAGAStateDisabled )
			RGBForeColor( &gAGARamp[ kAGA2 ] ) ;
		else
			RGBForeColor( &gAGARamp[ kAGA8 ] ) ;
		PaintRect( &rect ) ;
	}
}

void AGADrawRectProgress( const Rect *inRect , eAGAState inState , double percentage ) 
{
	Rect rect = *inRect ;
	rect.bottom = rect.top + 14 ; 
	RGBColor pixelColor ;
	
	RGBForeColor( &gAGARamp[ kAGA5 ]) ;
	MoveTo( rect.left , rect.bottom - 1 -1  ) ;
	LineTo( rect.left , rect.top ) ;
	LineTo( rect.right-1-1  , rect.top ) ;

	RGBForeColor( &gAGARamp[ kAGAWhite ] ) ;
	MoveTo( rect.left + 1 , rect.bottom -1 ) ;
	LineTo( rect.right-1  , rect.bottom -1 ) ;
	LineTo( rect.right-1  , rect.top-1 ) ;

	InsetRect( &rect , 1 , 1 ) ;
	RGBForeColor( &gAGARamp[ kAGABlack ] ) ;
			
	Rect barLeft = rect , barRight = rect ;
	
	int	position = percentage * ( rect.right - rect.left ) ;
	barLeft.right = barLeft.left + position ;
	barRight.left = barLeft.left + position ;
	FrameRect( &barLeft ) ;
	FrameRect( &barRight ) ;
	
	InsetRect( &barLeft , 1 , 1 ) ;

	InsetRect( &barRight , 1 , 1 ) ;
	
	if ( !EmptyRect( &barRight ) )
	{
		RGBForeColor( &gAGARamp[ kAGA10 ] ) ;
		MoveTo( barRight.left , barRight.bottom -1 ) ;
		LineTo( barRight.left , barRight.top ) ;
		barRight.left += 1 ;
		if ( !EmptyRect( &barRight ) )
		{
			RGBForeColor( &gAGARamp[ kAGA7 ] ) ;
			MoveTo( barRight.left , barRight.bottom-1  ) ;
			LineTo( barRight.left , barRight.top ) ;
			LineTo( barRight.right-1-1  , barRight.top ) ;
			RGBForeColor( &gAGARamp[ kAGA2 ] ) ;
			MoveTo( barRight.left + 1 , barRight.bottom -1 ) ;
			LineTo( barRight.right-1  , barRight.bottom -1 ) ;
			LineTo( barRight.right-1  , barRight.top - 1) ;
			pixelColor = gAGARamp[ kAGA4 ]  ;
			SetCPixel( barRight.right-1  , barRight.top , &pixelColor ) ;
			InsetRect( &barRight , 1  , 1)  ;
			if ( !EmptyRect( &barRight ) )
			{
				RGBForeColor( &gAGARamp[ kAGA4 ] ) ;
				PaintRect( &barRight ) ;
			}
		}
		if ( !EmptyRect( &barLeft ) )
		{
			RGBForeColor( &gAGABlueRamp[ kAGA8 ] ) ;
			MoveTo( barLeft.left , barLeft.bottom -1 ) ;
			LineTo( barLeft.left , barLeft.top ) ;
			LineTo( barLeft.left+1 , barLeft.top ) ;
			RGBForeColor( &gAGABlueRamp[ kAGA12 ] ) ;			
			MoveTo( barLeft.left + 2, barLeft.bottom-1  ) ;
			LineTo( barLeft.right-1   , barLeft.bottom-1  ) ;
			LineTo( barLeft.right-1  , barLeft.top -1 ) ;
			RGBForeColor( &gAGABlueRamp[ kAGA10 ] ) ;			
			MoveTo( barLeft.left + 2, barLeft.top ) ;
			LineTo( barLeft.right-1   , barLeft.top ) ;
			pixelColor = gAGABlueRamp[ kAGA10] ;
			SetCPixel( barLeft.left + 1, barLeft.bottom-1  , &pixelColor ) ;
			InsetRect( &barLeft , 1 , 1 ) ;
			if ( !EmptyRect( &barLeft ) )
			{
				// do more here
				RGBForeColor( &gAGABlueRamp[ kAGA3 ] ) ;
				PaintRect( &barLeft ) ;
			}
		}
	}
}

SInt32 AGAGetDataHelper( ControlDataAccessRec *rec , Size size , Ptr data ) ;
SInt32 AGAGetDataHelper( ControlDataAccessRec *rec , Size size , Ptr data ) 
{
	if ( rec->dataPtr == NULL )
	{
		rec->size = size ;
		return noErr ;
	}
	else
	{
		if ( rec->size < size )
			return errDataSizeMismatch ;
		
		rec->size = size ;
		memcpy( rec->dataPtr , data , size ) ;
		return noErr ;
	}
}

void AGASetFontStyle( ControlFontStyleRec *fontstyle ) ;
void AGASetFontStyle( ControlFontStyleRec *fontstyle )
{
		if( fontstyle->font >= 0 )
		{
			::TextFont( fontstyle->font ) ;
			::TextSize( fontstyle->size ) ;
			::TextFace( fontstyle->style ) ;
		}
		else
		{
			switch( fontstyle->font )
			{
				case kControlFontBigSystemFont :
					::TextFont( systemFont ) ; 
					::TextSize( 12 ) ;
					::TextFace( 0 ) ;
					break ;
				case kControlFontSmallSystemFont :
					::TextFont( kFontIDGeneva ) ; 
					::TextSize( 10 ) ;
					::TextFace( 0 ) ;
					break ;
				case kControlFontSmallBoldSystemFont :
					::TextFont( kFontIDGeneva ) ; 
					::TextSize( 10 ) ;
					::TextFace( bold ) ;
					break ;
			}
		}
		::RGBForeColor( &fontstyle->foreColor ) ;
		::RGBBackColor( &fontstyle->backColor ) ;
} ;

pascal SInt32 AGAProgressBarDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{	
	switch( message )
	{
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
				PenState		oldPenState;
				RGBColor		oldForeColor ;
				RGBColor		oldBackColor ;
				RGBColor		backColor ;
				RGBColor		foreColor ;
				
				GetPenState( &oldPenState ) ;
				GetBackColor( &oldBackColor ) ;
				GetForeColor( &oldForeColor ) ;
				
				{
					int			theValue = GetControlValue(theControl) ;
					int			theMinimum = GetControlMinimum(theControl) ;
					int			theMaximum = GetControlMaximum(theControl) ;
					
					AGADrawRectProgress( &(**theControl).contrlRect , kAGAStateEnabled , (( double )( theValue - theMinimum )) / ( theMaximum-theMinimum ) ) ;
				}
			
				RGBForeColor(&oldForeColor);
				RGBBackColor(&oldBackColor);
				SetPenState(&oldPenState);				
			}
			break ;
		case calcCntlRgn :
			RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}


pascal SInt32 AGAPlacardDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	switch( message )
	{
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
				PenState		oldPenState;
				RGBColor		oldForeColor ;
				RGBColor		oldBackColor ;
				RGBColor		backColor ;
				RGBColor		foreColor ;
				
				GetPenState( &oldPenState ) ;
				GetBackColor( &oldBackColor ) ;
				GetForeColor( &oldForeColor ) ;
				
				{
					AGADrawRectPlacard( &(**theControl).contrlRect , kAGAStateEnabled , true  ) ;
				}
			
				RGBForeColor(&oldForeColor);
				RGBBackColor(&oldBackColor);
				SetPenState(&oldPenState);				
			}
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}

pascal SInt32 AGABevelButtonDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	
	switch( message )
	{
		case initCntl :
			info->privateData = (long) malloc( sizeof( ControlButtonContentInfo ) ) ;
			memset( (char*) info->privateData , 0 , sizeof( ControlButtonContentInfo )  ) ;
			break ;
		case dispCntl :
			free( (void*) info->privateData ) ;
			break ;
		case kControlMsgSetData :
			{
				ControlDataAccessRec *rec = (ControlDataAccessRec *) param ;
				if ( rec->tag == kControlBevelButtonContentTag )
				{
					memcpy( (char*) info->privateData , rec->dataPtr , rec->size ) ;
					return noErr ;
				}
				return errDataNotSupported ;
			}
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = 0 ;
				rec->height = 20 ;
				rec->baseLine = 0;
			}
			break ;
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
					
				{
					AGAPortHelper help((**theControl).contrlOwner) ;
					AGASetFontStyle( &info->fontStyle ) ;
					Boolean mRadioBehavior = false ;
					
					int	mValue = GetControlValue( theControl ) ;
					long theValue = (mRadioBehavior ? mValue : 0);
					Boolean inPushed = (**theControl).contrlHilite ;
					Boolean down = inPushed || (theValue != 0);
					Boolean hasColor = 1;
					int mEnabled = 1 ;
					int triState_Off = 3 ;
					Boolean disabled = (mEnabled == triState_Off);
				
					Rect frame = (**theControl).contrlRect ;
					
					//	Draw the black frame;
					::MoveTo(frame.left + 1, frame.top);
					::LineTo(frame.right - 2, frame.top);
					::MoveTo(frame.right - 1, frame.top + 1);
					::LineTo(frame.right - 1, frame.bottom - 2);
					::MoveTo(frame.right - 2, frame.bottom - 1);
					::LineTo(frame.left + 1, frame.bottom - 1);
					::MoveTo(frame.left, frame.bottom - 2);
					::LineTo(frame.left, frame.top + 1);
					
					//	Draw the inside (w/o the Icon)
					::InsetRect(&frame, 1, 1);
					if (hasColor)
						{
							AGASetFontStyle( &info->fontStyle ) ;
							if (down)
								::RGBBackColor(&gAGAColorArray[4]);
						}
					::EraseRect(&frame);
					
					//	Draw the shadows
					if (hasColor)
						{
							::RGBForeColor(&gAGAColorArray[7]);
							if (down)
								{
									::MoveTo(frame.left, frame.bottom - 1);
									::LineTo(frame.left, frame.top);
									::LineTo(frame.right - 1, frame.top);
								}
							else
								{
									::MoveTo(frame.right - 1, frame.top);
									::LineTo(frame.right - 1, frame.bottom - 1);
									::LineTo(frame.left, frame.bottom - 1);
									::MoveTo(frame.right - 2, frame.top + 1);
									::LineTo(frame.right - 2, frame.bottom - 2);
									::LineTo(frame.left + 1, frame.bottom - 2);
									::ForeColor(whiteColor);
									::MoveTo(frame.left, frame.bottom - 2);
									::LineTo(frame.left, frame.top);
									::LineTo(frame.right - 2, frame.top);
								}
						}
					
					//	Draw the Icon
					
					frame = (**theControl).contrlRect ;
 					PictInfo theInfo ;
					PicHandle thePict = ((ControlButtonContentInfo*) info->privateData)->u.picture ;
					if ( thePict )
					{
						GetPictInfo( thePict , &theInfo , 0 , 0 , systemMethod , 0 ) ;
	   				Rect bitmaprect = { 0 , 0 , theInfo.sourceRect.bottom - theInfo.sourceRect.top , 
	   																		theInfo.sourceRect.right - theInfo.sourceRect.left } ;
						::OffsetRect( &bitmaprect , (frame.right + frame.left)/2 - bitmaprect.right / 2 , (frame.bottom + frame.top) / 2 - bitmaprect.bottom / 2 ) ;
						if (inPushed)
							::OffsetRect( &bitmaprect , 1 , 1 ) ;
						::DrawPicture( thePict , &bitmaprect ) ;
					}
				}
			}
			break ;
		case testCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
					return kControlButtonPart ;
				else
					return NULL ;
			}
			break ;
		case calcCntlRgn :
			RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		case posCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
					return kControlButtonPart ;
				else
					return NULL ;
			}
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}

pascal SInt32 AGAButtonDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	
	switch( message )
	{
		case initCntl :
			(**theControl).contrlData = NULL ;
			break ;
		case kControlMsgSetData :
			{
				ControlDataAccessRec *rec = (ControlDataAccessRec *) param ;
				Boolean isDefault = *((Boolean*)rec->dataPtr ) ;
				(**theControl).contrlData = (Handle) isDefault ;
				return noErr ;
			}
			break ;
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = 0 ;
				rec->height = 20 ;
				rec->baseLine = 0;
			}
			break ;
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
					
				{
					AGAPortHelper help((**theControl).contrlOwner) ;
					AGASetFontStyle( &info->fontStyle ) ;
					Boolean mRadioBehavior = false ;
					
					Rect frame, tempRect;
					int	mValue = GetControlValue( theControl ) ;
					long theValue = (mRadioBehavior ? mValue : 0);
					Boolean inPushed = (**theControl).contrlHilite ;
					Boolean down = inPushed || (theValue != 0);
					Boolean hasColor = 1;
					int mEnabled = 1 ;
					int triState_Off = 3 ;
					Boolean disabled = (mEnabled == triState_Off);
				
//					theState.Normalize();
//					CalcLocalFrameRect(frame);
					frame= (**theControl).contrlRect ;
					
					if ( (**theControl).contrlData )
					{
						InsetRect( &frame , -3 , -3 ) ;
						{
							::MoveTo(frame.left + 3, frame.top);
							::LineTo(frame.right - 4, frame.top);
							::LineTo(frame.right - 1, frame.top + 3);
							::LineTo(frame.right - 1, frame.bottom - 4);
							::LineTo(frame.right - 4, frame.bottom - 1);
							::LineTo(frame.left + 3, frame.bottom - 1);
							::LineTo(frame.left, frame.bottom - 4);
							::LineTo(frame.left, frame.top + 3);
							::LineTo(frame.left + 3, frame.top);
							
							if (hasColor)
								{
									const RGBColor *col = gAGAColorArray;
									for (int i = 0; i < 5; i++)
										for (int j = 0; j < 5; j++)
											{
												if (LAGADefaultOutline_mCorners[0][i][j] != -1)
													::SetCPixel(frame.left + j, frame.top + i, &col[LAGADefaultOutline_mCorners[0][i][j]]);
												if (LAGADefaultOutline_mCorners[1][i][j] != -1)
													::SetCPixel(frame.right - 5 + j, frame.top + i, &col[LAGADefaultOutline_mCorners[1][i][j]]);
												if (LAGADefaultOutline_mCorners[2][i][j] != -1)
													::SetCPixel(frame.left + j, frame.bottom - 5 + i, &col[LAGADefaultOutline_mCorners[2][i][j]]);
												if (LAGADefaultOutline_mCorners[3][i][j] != -1)
													::SetCPixel(frame.right - 5 + j, frame.bottom - 5 + i, &col[LAGADefaultOutline_mCorners[3][i][j]]);
											}
									
									::RGBForeColor(&gAGAColorArray[2]);
									::MoveTo(frame.left + 1, frame.top + 5);
									::LineTo(frame.left + 1, frame.bottom - 6);
									::MoveTo(frame.left + 5, frame.top + 1);
									::LineTo(frame.right - 6, frame.top + 1);
									
									::RGBForeColor(&gAGAColorArray[5]);
									::MoveTo(frame.left + 4, frame.top + 2);
									::LineTo(frame.right - 5, frame.top + 2);
									::LineTo(frame.right - 3, frame.top + 4);
									::LineTo(frame.right - 3, frame.bottom - 5);
									::LineTo(frame.right - 5, frame.bottom - 3);
									::LineTo(frame.left + 4, frame.bottom - 3);
									::LineTo(frame.left + 2, frame.bottom - 5);
									::LineTo(frame.left + 2, frame.top + 4);
									::LineTo(frame.left + 4, frame.top + 2);
									
									::RGBForeColor(&gAGAColorArray[8]);
									::MoveTo(frame.right - 2, frame.top + 5);
									::LineTo(frame.right - 2, frame.bottom - 6);
									::MoveTo(frame.left + 5, frame.bottom - 2);
									::LineTo(frame.right - 6, frame.bottom - 2);
								}
						}		
					}
					
					AGASetFontStyle( &info->fontStyle ) ;
					frame= (**theControl).contrlRect ;

					tempRect = frame;
					::InsetRect(&tempRect, 1, 1);
					if (hasColor)
						{
							if (down)
								::RGBBackColor(&gAGAColorArray[4]);
						}
					EraseRect(&tempRect);
					
					//	Draw the frame;
					if (disabled)
						{
							if (hasColor)
								::RGBForeColor(&gAGAColorArray[7]);
							else
								::PenPat(&qd.gray);
						}
					::MoveTo(frame.left + 2, frame.top);
					::LineTo(frame.right - 3, frame.top);
					::LineTo(frame.right - 1, frame.top + 2);
					::LineTo(frame.right - 1, frame.bottom - 3);
					::LineTo(frame.right - 3, frame.bottom - 1);
					::LineTo(frame.left + 2, frame.bottom - 1);
					::LineTo(frame.left, frame.bottom - 3);
					::LineTo(frame.left, frame.top + 2);
					::LineTo(frame.left + 2, frame.top);
					if (disabled && !hasColor)
						::PenNormal();
					
					//	Draw the four corners around
					if (hasColor)
						{
							short pattern = (disabled ? 2 : (down ? 1 : 0));
							const RGBColor *col = gAGAColorArray;
							for (int i = 0; i < 4; i++)
								for (int j = 0; j < 4; j++)
									{
										if (LAGAPushButton_mCorners[pattern][0][i][j] != -1)
											::SetCPixel(frame.left + j, frame.top + i, &col[LAGAPushButton_mCorners[pattern][0][i][j]]);
										if (LAGAPushButton_mCorners[pattern][1][i][j] != -1)
											::SetCPixel(frame.right - 4 + j, frame.top + i, &col[LAGAPushButton_mCorners[pattern][1][i][j]]);
										if (LAGAPushButton_mCorners[pattern][2][i][j] != -1)
											::SetCPixel(frame.left + j, frame.bottom - 4 + i, &col[LAGAPushButton_mCorners[pattern][2][i][j]]);
										if (LAGAPushButton_mCorners[pattern][3][i][j] != -1)
											::SetCPixel(frame.right - 4 + j, frame.bottom - 4 + i, &col[LAGAPushButton_mCorners[pattern][3][i][j]]);
									}
							
							if (down)
								::RGBForeColor(&gAGAColorArray[8]);
							else
								::RGBForeColor(&gAGAColorArray[2]);
							::MoveTo(frame.left + 1, frame.top + 4);
							::LineTo(frame.left + 1, frame.bottom - 5);
							::MoveTo(frame.left + 4, frame.top + 1);
							::LineTo(frame.right - 5, frame.top + 1);
							
							if (!down)
								{
									if (disabled)
										::RGBForeColor(&gAGAColorArray[1]);
									else
										::ForeColor(whiteColor);
									::MoveTo(frame.left + 2, frame.bottom - 5);
									::LineTo(frame.left + 2, frame.top + 2);
									::LineTo(frame.right - 5, frame.top + 2);
							
									if (disabled)
										::RGBForeColor(&gAGAColorArray[5]);
									else
										::RGBForeColor(&gAGAColorArray[8]);
									::MoveTo(frame.left + 4, frame.bottom - 2);
									::LineTo(frame.right - 5, frame.bottom - 2);
									::MoveTo(frame.right - 2, frame.bottom - 5);
									::LineTo(frame.right - 2, frame.top + 4);
									
									if (disabled)
										::RGBForeColor(&gAGAColorArray[4]);
									else
										::RGBForeColor(&gAGAColorArray[5]);
									::MoveTo(frame.left + 4, frame.bottom - 3);
									::LineTo(frame.right - 5, frame.bottom - 3);
									::MoveTo(frame.right - 3, frame.bottom - 5);
									::LineTo(frame.right - 3, frame.top + 4);
								}
						}
					AGASetFontStyle( &info->fontStyle ) ;
					int x =	( (**theControl).contrlRect.left + (**theControl).contrlRect.right ) / 2 ;
					int y =	( (**theControl).contrlRect.top + 	(**theControl).contrlRect.bottom ) / 2 ;
					FontInfo fi ;
					::GetFontInfo( &fi ) ;
					
					y += fi.ascent / 2 ;
					int length = (**theControl).contrlTitle[0] ;
					if ( length )
					{
						int width = TextWidth( &(**theControl).contrlTitle[1] , 0, length ) ;
						x -= width / 2 ;
						::MoveTo( x , y );
						::DrawText( &(**theControl).contrlTitle[1] , 0, length);
					}
					
				}
			}
			break ;
		case testCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
					return kControlButtonPart ;
				else
					return NULL ;
			}
			break ;
		case calcCntlRgn :
			RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		case posCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
					return kControlButtonPart ;
				else
					return NULL ;
			}
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}

const int kAGACheckBoxWidth = 12 ;
const int kAGACheckBoxHeigth = 12 ;

pascal SInt32 AGACheckBoxDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	
	switch( message )
	{
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
				PenState		oldPenState;
				RGBColor		oldForeColor ;
				RGBColor		oldBackColor ;
				RGBColor		backColor ;
				RGBColor		foreColor ;
				
				
				{
					if (!(**theControl).contrlVis)				
						return 0 ;
						
					{
						AGAPortHelper help((**theControl).contrlOwner) ;
						Rect frame =  (**theControl).contrlRect ;
						Boolean hasColor = true;
						Boolean disabled = (*theControl)->contrlHilite == 255 ;
						int	mValue = GetControlValue( theControl ) ;
						Boolean inPushed = (**theControl).contrlHilite ;
						int mEnabled = 1 ;
						int triState_Off = 3 ;
						
						frame.right = frame.left + kAGACheckBoxWidth;
						frame.bottom = frame.top + kAGACheckBoxHeigth;
						
						//	Draw the frame of the checkbox
							if (disabled)
								if (hasColor)
									::RGBForeColor(&gAGAColorArray[7]);
								else
									PenPat(&qd.gray);
							
						::FrameRect(&frame);
						
						if (!hasColor)
							PenNormal();
							
						::InsetRect(&frame, 1, 1);
						if (hasColor)
							::RGBBackColor(inPushed ? &gAGAColorArray[8] : &gAGAColorArray[2]);
						::EraseRect(&frame);
						
						if (hasColor)
							{
								if (inPushed)
									::RGBForeColor(&gAGAColorArray[10]);
								else
									ForeColor(whiteColor);
								::MoveTo(frame.left, frame.bottom - 2);
								::LineTo(frame.left, frame.top);
								::LineTo(frame.right - 2, frame.top);
						
								if (inPushed)
									::RGBForeColor(&gAGAColorArray[6]);
								else
									::RGBForeColor(mEnabled != triState_Off ? &gAGAColorArray[7] : &gAGAColorArray[5]);
								::MoveTo(frame.left + 1, frame.bottom - 1);
								::LineTo(frame.right - 1, frame.bottom - 1);
								::LineTo(frame.right - 1, frame.top + 1);
							}
						
						if (mValue)
							{
								if (mValue == 1)
									{
										//	Checked state
										if (hasColor)
											{
												::RGBForeColor(inPushed ? &gAGAColorArray[10] : &gAGAColorArray[5]);
												::MoveTo(frame.left + 3, frame.bottom - 2);
												::LineTo(frame.right - 2, frame.top + 3);
												::MoveTo(frame.right - 2, frame.bottom - 2);
												::LineTo(frame.right - 2, frame.bottom - 2);
										
												if (mEnabled != triState_Off)
													::RGBForeColor(inPushed ? &gAGAColorArray[11] : &gAGAColorArray[8]);
												::MoveTo(frame.left + 3, frame.bottom - 3);
												::LineTo(frame.right - 2, frame.top + 2);
												::MoveTo(frame.right - 2, frame.bottom - 3);
												::LineTo(frame.right - 2, frame.bottom - 3);
												
												if (mEnabled != triState_Off)
													::ForeColor(blackColor);
												else
													::RGBForeColor(&gAGAColorArray[7]);
											}
										::MoveTo(frame.left + 2, frame.top + 1);
										::LineTo(frame.right - 3, frame.bottom - 4);
										::MoveTo(frame.left + 2, frame.top + 2);
										::LineTo(frame.right - 3, frame.bottom - 3);
										::MoveTo(frame.left + 2, frame.bottom - 4);
										::LineTo(frame.right - 3, frame.top + 1);
										::MoveTo(frame.left + 2, frame.bottom - 3);
										::LineTo(frame.right - 3, frame.top + 2);
									}
								else
									{
										//	Mixed state
										if (hasColor)
											{
												::RGBForeColor(inPushed ? &gAGAColorArray[10] : &gAGAColorArray[5]);
												::MoveTo(frame.left + 3, frame.top + 6);
												::LineTo(frame.right - 2, frame.top + 6);
												::LineTo(frame.right - 2, frame.top + 4);
										
												if (mEnabled != triState_Off)
													::ForeColor(blackColor);
												else
													::RGBForeColor(&gAGAColorArray[7]);
											}
										::MoveTo(frame.left + 2, frame.top + 4);
										::LineTo(frame.right - 3, frame.top + 4);
										::MoveTo(frame.left + 2, frame.top + 5);
										::LineTo(frame.right - 3, frame.top + 5);
									}
							}
							
						if (inPushed && !hasColor)
							InvertRect(&frame);
						
						AGASetFontStyle( &info->fontStyle ) ;
						int x =	(**theControl).contrlRect.left + kAGACheckBoxWidth + 5 ;
						int y =	( (**theControl).contrlRect.top + (**theControl).contrlRect.bottom ) / 2 ;
						FontInfo fi ;
						::GetFontInfo( &fi ) ;
						
						y += fi.ascent / 2 ;
						::MoveTo( x , y );
						int length = (**theControl).contrlTitle[0] ;
						if ( length )
						{
							::DrawText( &(**theControl).contrlTitle[1] , 0, length);
						}
					}
				}
			
			}
			break ;
		case testCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) && ((*theControl)->contrlVis != 0) && ((*theControl)->contrlHilite != 255) )
					return kControlCheckBoxPart ;
				else
					return NULL ;
			}
			break ;
		case posCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
			  {
			  	if ( (**theControl).contrlValue )
			  		(**theControl).contrlValue = 0 ;
			  	else
			  		(**theControl).contrlValue = 1 ;
			  		
					return 0 ;
				}
				else
					return 0 ;
			}
			break ;
		case calcThumbRgn :
		case calcCntlRgn :
			if ( !EmptyRect(&(**theControl).contrlRect ) )
				RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = (**theControl).contrlRect.right - (**theControl).contrlRect.left ;
				rec->height = kAGACheckBoxHeigth ;
				rec->baseLine = 0;
			}
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}

pascal SInt32 AGARadioButtonDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
//TO DO
/*	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	
	switch( message )
	{
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
				PenState		oldPenState;
				RGBColor		oldForeColor ;
				RGBColor		oldBackColor ;
				RGBColor		backColor ;
				RGBColor		foreColor ;
				
				
				{
					if (!(**theControl).contrlVis)				
						return 0 ;
						
					{
						AGAPortHelper help() ;
						Rect frame =  (**theControl).contrlRect ;
						Boolean hasColor = true;
						Boolean disabled = (*theControl)->contrlHilite == 255 ;
						int	mValue = GetControlValue( theControl ) ;
						Boolean inPushed = (**theControl).contrlHilite ;
						int mEnabled = 1 ;
						int triState_Off = 3 ;
						
						frame.right = frame.left + kAGACheckBoxWidth;
						frame.bottom = frame.top + kAGACheckBoxHeigth;
						
						//	Draw the frame of the checkbox
							if (disabled)
								if (hasColor)
									::RGBForeColor(&gAGAColorArray[7]);
								else
									PenPat(&qd.gray);
							
						::FrameRect(&frame);
						
						if (!hasColor)
							PenNormal();
							
						::InsetRect(&frame, 1, 1);
						if (hasColor)
							::RGBBackColor(inPushed ? &gAGAColorArray[8] : &gAGAColorArray[2]);
						::EraseRect(&frame);
						
						if (hasColor)
							{
								if (inPushed)
									::RGBForeColor(&gAGAColorArray[10]);
								else
									ForeColor(whiteColor);
								::MoveTo(frame.left, frame.bottom - 2);
								::LineTo(frame.left, frame.top);
								::LineTo(frame.right - 2, frame.top);
						
								if (inPushed)
									::RGBForeColor(&gAGAColorArray[6]);
								else
									::RGBForeColor(mEnabled != triState_Off ? &gAGAColorArray[7] : &gAGAColorArray[5]);
								::MoveTo(frame.left + 1, frame.bottom - 1);
								::LineTo(frame.right - 1, frame.bottom - 1);
								::LineTo(frame.right - 1, frame.top + 1);
							}
						
						if (mValue)
							{
								if (mValue == 1)
									{
										//	Checked state
										if (hasColor)
											{
												::RGBForeColor(inPushed ? &gAGAColorArray[10] : &gAGAColorArray[5]);
												::MoveTo(frame.left + 3, frame.bottom - 2);
												::LineTo(frame.right - 2, frame.top + 3);
												::MoveTo(frame.right - 2, frame.bottom - 2);
												::LineTo(frame.right - 2, frame.bottom - 2);
										
												if (mEnabled != triState_Off)
													::RGBForeColor(inPushed ? &gAGAColorArray[11] : &gAGAColorArray[8]);
												::MoveTo(frame.left + 3, frame.bottom - 3);
												::LineTo(frame.right - 2, frame.top + 2);
												::MoveTo(frame.right - 2, frame.bottom - 3);
												::LineTo(frame.right - 2, frame.bottom - 3);
												
												if (mEnabled != triState_Off)
													::ForeColor(blackColor);
												else
													::RGBForeColor(&gAGAColorArray[7]);
											}
										::MoveTo(frame.left + 2, frame.top + 1);
										::LineTo(frame.right - 3, frame.bottom - 4);
										::MoveTo(frame.left + 2, frame.top + 2);
										::LineTo(frame.right - 3, frame.bottom - 3);
										::MoveTo(frame.left + 2, frame.bottom - 4);
										::LineTo(frame.right - 3, frame.top + 1);
										::MoveTo(frame.left + 2, frame.bottom - 3);
										::LineTo(frame.right - 3, frame.top + 2);
									}
								else
									{
										//	Mixed state
										if (hasColor)
											{
												::RGBForeColor(inPushed ? &gAGAColorArray[10] : &gAGAColorArray[5]);
												::MoveTo(frame.left + 3, frame.top + 6);
												::LineTo(frame.right - 2, frame.top + 6);
												::LineTo(frame.right - 2, frame.top + 4);
										
												if (mEnabled != triState_Off)
													::ForeColor(blackColor);
												else
													::RGBForeColor(&gAGAColorArray[7]);
											}
										::MoveTo(frame.left + 2, frame.top + 4);
										::LineTo(frame.right - 3, frame.top + 4);
										::MoveTo(frame.left + 2, frame.top + 5);
										::LineTo(frame.right - 3, frame.top + 5);
									}
							}
							
						if (inPushed && !hasColor)
							InvertRect(&frame);
						
						AGASetFontStyle( &info->fontStyle ) ;
						int x =	(**theControl).contrlRect.left + kAGACheckBoxWidth + 5 ;
						int y =	( (**theControl).contrlRect.top + (**theControl).contrlRect.bottom ) / 2 ;
						FontInfo fi ;
						::GetFontInfo( &fi ) ;
						
						y += fi.ascent / 2 ;
						::MoveTo( x , y );
						int length = (**theControl).contrlTitle[0] ;
						if ( length )
						{
							::DrawText( &(**theControl).contrlTitle[1] , 0, length);
						}
					}
				}
			
			}
			break ;
		case testCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) && ((*theControl)->contrlVis != 0) && ((*theControl)->contrlHilite != 255) )
					return kControlCheckBoxPart ;
				else
					return NULL ;
			}
			break ;
		case posCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
			  {
			  	if ( (**theControl).contrlValue )
			  		(**theControl).contrlValue = 0 ;
			  	else
			  		(**theControl).contrlValue = 1 ;
			  		
					return 0 ;
				}
				else
					return 0 ;
			}
			break ;
		case calcThumbRgn :
		case calcCntlRgn :
			if ( !EmptyRect(&(**theControl).contrlRect ) )
				RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = (**theControl).contrlRect.right - (**theControl).contrlRect.left ;
				rec->height = kAGACheckBoxHeigth ;
				rec->baseLine = 0;
			}
			break ;
		default :
			break ;
	}
	*/
	return 0 ;									
}

pascal SInt32 AGAStaticGroupBoxTextDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	

	switch( message )
	{
		case initCntl :
			break ;
		case dispCntl :
			break ;
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
					
				{
					bool disabled = false ;
					bool hasColor = true ;
					AGAPortHelper help((**theControl).contrlOwner) ;
					AGASetFontStyle( &info->fontStyle ) ;
					FontInfo fi ;
					::GetFontInfo( &fi ) ;
					
					Rect labelRect = (**theControl).contrlRect ;
					Rect theFrame = (**theControl).contrlRect ;
					int width = StringWidth((**theControl).contrlTitle) ;
					theFrame.top += fi.ascent + fi.leading >> 1 ;
					labelRect.bottom = theFrame.top + 8 ;
					labelRect.left += 10 ;
					labelRect.right =labelRect.left + width + 6 ;
					
					theFrame.bottom--;
					theFrame.right--;
					
					if (disabled)
					{
						RGBForeColor( &gAGAColorArray[4] ) ;
					}
					else
					{
						RGBForeColor( &gAGAColorArray[7] ) ;
					}
					FrameRect( &theFrame ) ;
					
					if (disabled)
						::RGBForeColor(&gAGAColorArray[1]);
					else
						::ForeColor(whiteColor);
					::MoveTo(theFrame.left + 1, theFrame.bottom - 2);
					::LineTo(theFrame.left + 1, theFrame.top + 1);
					::LineTo(theFrame.right - 2, theFrame.top + 1);
					::MoveTo(theFrame.left , theFrame.bottom);
					::LineTo(theFrame.right, theFrame.bottom);
					::LineTo(theFrame.right, theFrame.top);
					
					AGASetFontStyle( &info->fontStyle ) ;
					::EraseRect( &labelRect ) ;
					::MoveTo(labelRect.left + 3, labelRect.top + fi.ascent + (fi.leading >> 1));
					::DrawString((**theControl).contrlTitle);
				}
			}
			break ;
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = (**theControl).contrlRect.right - (**theControl).contrlRect.left ;
				rec->height = (**theControl).contrlRect.bottom - (**theControl).contrlRect.top  ;
				rec->baseLine = 0;
			}
			break ;
		case kControlMsgSetData :
			break ;
		case calcCntlRgn :
			RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}

pascal SInt32 AGAStaticTextDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	Handle macText = (**theControl).contrlData ;
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	

	switch( message )
	{
		case initCntl :
			{
				macText = NewHandle(0) ;
				 (**theControl).contrlData = (Handle) macText ;
			}
			break ;
		case dispCntl :
			DisposeHandle( macText ) ;
			break ;
		case drawCntl :
			{
				if (!(**theControl).contrlVis)				
					return 0 ;
					
				{
					AGAPortHelper help((**theControl).contrlOwner) ;
					AGASetFontStyle( &info->fontStyle ) ;
					int x =	(**theControl).contrlRect.left ;
					int y =	(**theControl).contrlRect.top ;
					int lineheight ;
					FontInfo fi ;
					::GetFontInfo( &fi ) ;
					
					y += fi.ascent ;
					lineheight = fi.ascent + fi.descent + fi.leading ;
					int length = GetHandleSize( macText ) ;
					if ( length )
					{
						int laststop = 0 ;
						int i = 0 ;
						HLock( macText ) ;
						RGBColor gray = { 0xDDDD , 0xDDDD, 0xDDDD } ;
						::RGBBackColor( &gray ) ;
						while( i < length )
						{
							if ( (*macText)[i] == 0x0d )
							{
								::MoveTo( x , y );
								::DrawText( *macText , laststop, i-laststop);
								laststop = i+1 ;
								y += lineheight ;
							}
							i++ ;
						}
						::MoveTo( x , y );
						::DrawText( *macText , laststop, i-laststop);
						laststop = i+1 ;
						HUnlock( macText ) ;
					}
				}
			}
			break ;
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = (**theControl).contrlRect.right - (**theControl).contrlRect.left ;
				rec->height = (**theControl).contrlRect.bottom - (**theControl).contrlRect.top  ;
				rec->baseLine = 0;
			}
			break ;
		case kControlMsgSetData :
			{
				ControlDataAccessRec *rec = (ControlDataAccessRec *) param ;
				if ( rec->tag == kControlStaticTextTextTag )
				{
					SetHandleSize( macText , rec->size ) ;
					memcpy( *macText , rec->dataPtr , rec->size ) ;
					return noErr ;
				}
				return errDataNotSupported ;
			}
		default :
			break ;
	}
	
	return 0 ;									
}

void AGAMoveControl(ControlHandle inControl , short x , short y ) 
{
	if ( AGATestForNewControl( inControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;	
		if ( info->magic == kExtCDEFMagic )
		{
			if ( info->procID == kControlEditTextProc)
			{
				int dx ;
				int dy ;
				
				dx= x - (**inControl).contrlRect.left  ;
				dy= y - (**inControl).contrlRect.top ;
				
				MoveControl( inControl , x ,y  ) ;
				TEHandle macTE ;
	
				macTE = (TEHandle) (**inControl).contrlData ;
				(**macTE).destRect.left += dx ;
				(**macTE).destRect.top += dy ;
				(**macTE).destRect.right += dx ;
				(**macTE).destRect.bottom += dy ;
				(**macTE).viewRect.left += dx ;
				(**macTE).viewRect.top += dy ;
				(**macTE).viewRect.right += dx ;
				(**macTE).viewRect.bottom += dy ;
				return ;
			}
			if ( info->procID == kControlListBoxProc)
			{
				int dx ;
				int dy ;
				
				dx= x - (**inControl).contrlRect.left  ;
				dy= y - (**inControl).contrlRect.top ;
				
				MoveControl( inControl , x ,y  ) ;
				ListHandle macList ;
	
				macList = (ListHandle) (**inControl).contrlData ;
				(**macList).rView.left += dx ;
				(**macList).rView.top += dy ;
				(**macList).rView.right += dx ;
				(**macList).rView.bottom += dy ;
				return ;
			}
		}
	}
	MoveControl( inControl , x ,y  ) ;
}

void AGASizeControl(ControlHandle inControl , short x , short y ) 
{
	if ( AGATestForNewControl( inControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;	
		if ( info->magic == kExtCDEFMagic )
		{
			if ( info->procID == kControlEditTextProc)
			{
				int dx ;
				int dy ;
				
				dx= x - ( (**inControl).contrlRect.right - (**inControl).contrlRect.left ) ;
				dy= y - ( (**inControl).contrlRect.bottom - (**inControl).contrlRect.top ) ;
				
				SizeControl( inControl , x ,y  ) ;
				TEHandle macTE ;
	
				macTE = (TEHandle) (**inControl).contrlData ;
				(**macTE).destRect.right += dx ;
				(**macTE).destRect.bottom += dy ;
				(**macTE).viewRect.right += dx ;
				(**macTE).viewRect.bottom += dy ;
				return ;
			}
			if ( info->procID == kControlListBoxProc)
			{
				int dx ;
				int dy ;
				
				dx= x - ( (**inControl).contrlRect.right - (**inControl).contrlRect.left ) ;
				dy= y - ( (**inControl).contrlRect.bottom - (**inControl).contrlRect.top ) ;
				
				SizeControl( inControl , x ,y  ) ;
				ListHandle macList ;
	
				macList = (ListHandle) (**inControl).contrlData ;
				(**macList).rView.right += dx ;
				(**macList).rView.bottom += dy ;
				return ;
			}
		}
	}
	SizeControl( inControl , x ,y  ) ;
}

pascal SInt32 AGARootControlDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	

	switch( message )
	{
		case initCntl :
			break ;
		case dispCntl :
			break ;
		case drawCntl :
			break ;
		case kControlMsgCalcBestRect :
			break ;
		case kControlMsgSetData :
		default :
			break ;
	}
	
	return 0 ;									
}

pascal SInt32 AGAEditTextDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	
	Size result ;
	TEHandle macTE ;
	
	macTE = (TEHandle) (**theControl).contrlData ;
	
	switch( message )
	{
		case initCntl :
			{
				AGAPortHelper help((**theControl).contrlOwner) ;
				SetPort( (**theControl).contrlOwner ) ;
				::TextFont( kFontIDGeneva ) ; 
				::TextSize( 10 ) ;
				::TextFace( 0 ) ;
				RGBBackColor( &gAGARamp[ kAGAWhite ] ) ;
				RGBForeColor( &gAGARamp[ kAGABlack ] ) ;
				Rect bounds =  (**theControl).contrlRect ;				
				InsetRect( &bounds , 4 , 1 ) ;
				FontInfo fi ;
				::GetFontInfo( &fi ) ;
				
				bounds.top = bounds.bottom - abs( fi.descent ) - fi.ascent - 3;
				macTE = TENew( &bounds , &bounds) ;
				 (**theControl).contrlData = (Handle) macTE ;
			}
			break ;
		case dispCntl :
			TEDispose( macTE ) ;
			break ;
		case drawCntl :
			{
				AGAPortHelper help((**theControl).contrlOwner) ;
				AGASetFontStyle( &info->fontStyle ) ;
				SetPort( (**theControl).contrlOwner ) ;
				RGBBackColor( &gAGARamp[ kAGAWhite ] ) ;
				RGBForeColor( &gAGARamp[ kAGABlack ] ) ;
				EraseRect( &(**theControl).contrlRect ) ;
				FrameRect( &(**theControl).contrlRect ) ;
				TEUpdate( &(**theControl).contrlRect , macTE ) ;
			}
			break ;
		case testCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
					return kControlButtonPart ;
				else
					return NULL ;
			}
			break ;
		case posCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  if ( PtInRect( pt , &(**theControl).contrlRect  ) )
			  {
			  	TEClick( pt , false , macTE ) ;
				}
			}
			break ;
		case kControlMsgGetData :
			{
				ControlDataAccessRec *rec = (ControlDataAccessRec *) param ;
				if ( rec->tag == kControlEditTextTEHandleTag )
				{
					return AGAGetDataHelper( rec , sizeof( TEHandle ) , (char*) &macTE ) ;
				}
				else if ( rec->tag == kControlEditTextTextTag )
				{
					return AGAGetDataHelper( rec , (**macTE).teLength , (char*) *(**macTE).hText ) ;
				}
				else if ( rec->tag == kControlEditTextSelectionTag )
				{
				}
				return errDataNotSupported ;
			}
			break ;
		case kControlMsgSetData :
			{
				ControlDataAccessRec *rec = (ControlDataAccessRec *) param ;
				if ( rec->tag == kControlEditTextTextTag )
				{
					TESetText( rec->dataPtr , rec->size ,  macTE ) ;
					return noErr ;
				}
				return errDataNotSupported ;
			}
		case kControlMsgCalcBestRect :
			{
				ControlCalcSizeRec *rec = (ControlCalcSizeRec *) param ;
				rec->width = (**theControl).contrlRect.right - (**theControl).contrlRect.left ;
				rec->height = (**theControl).contrlRect.bottom - (**theControl).contrlRect.top  ;
				rec->baseLine = 0;
			}
		case kControlMsgFocus :
			{
				if ( param == kControlFocusNoPart )
				{
					info->hasFocus = false ;
					TEDeactivate( macTE ) ;
					return kControlFocusNoPart ;
				}
				else if ( param == kControlFocusNextPart || param == kControlFocusPrevPart )
				{
					if ( info->hasFocus )
					{
						info->hasFocus = false ;
						TEDeactivate( macTE ) ;
						return kControlFocusNoPart ;
					}
					else
					{
						info->hasFocus = true ;
						TEActivate( macTE ) ;
						return kControlEditTextPart ;
					}
				}
				else if ( param == kControlEditTextPart )
				{
					if ( !info->hasFocus )
					{
						info->hasFocus = true ;
						TEActivate( macTE ) ;
						return kControlEditTextPart ;
					}
				}
			}
			break ;
		case kControlMsgIdle :
			{
				TEIdle( macTE ) ;
			}
			break ;
		case kControlMsgKeyDown :
			{
				AGAPortHelper help( (**theControl).contrlOwner ) ;
				AGASetFontStyle( &info->fontStyle ) ;
				RGBBackColor( &gAGARamp[ kAGAWhite ] ) ;
				RGBForeColor( &gAGARamp[ kAGABlack ] ) ;
				ControlKeyDownRec * rec = (ControlKeyDownRec*) param ;
				TEKey( rec->charCode , macTE ) ;
			}
			break ;
		case kControlMsgActivate :
			{
			 /*
				if ( param )
					TEActivate( macTE ) ;
				else
					TEDeactivate( macTE ) ;
				*/
			}
			break ;
		case calcCntlRgn :
			RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		case kControlMsgTestNewMsgSupport :
			return kControlSupportsNewMessages ;
		case kControlMsgGetFeatures :
			return 
//				kControlSupportsGhosting |
//				kControlSupportsEmbedding |
				kControlSupportsFocus | 
				kControlWantsIdle | 
				kControlWantsActivate | 
//				kControlHandlesTracking |
				kControlSupportsDataAccess |
//				kControlHasSpecialBackground |
//				kControlGetsFocusOnClick |
				kControlSupportsCalcBestRect |
//				kControlSupportsLiveFeedback |
//				kControlHasRadioBehavior |
				0 ;
		default :
			break ;
	}
	
	return 0 ;									
}

pascal SInt32 AGAListControlDefProc (SInt16 procID, ControlHandle theControl, ControlDefProcMessage message, SInt32 param)
{
	Size result ;
	ListHandle macList ;
	ExtCDEFInfo* info = (ExtCDEFInfo* )  (**theControl).contrlRfCon ;	
	
	macList = (ListHandle) (**theControl).contrlData ;

	static inLoop = false ;
	static lastClick = NULL ;
	
	switch( message )
	{
		case initCntl :
			{
				Rect databounds = { 0,0,0,0} ;
				Point cellsize = { 0,0} ;
				Rect listBounds =  (**theControl).contrlRect ;
				
				InsetRect( &listBounds , 1 , 1 ) ;
				
				macList = LNew( &listBounds , &databounds , cellsize , 128 , 
					(**theControl).contrlOwner , false /*drawit*/ , false /*hasGrow*/,
					false /* horzScroll */, true /*vertScroll*/ ) ;
				 (**theControl).contrlData = (Handle) macList ;
				info->privateData = NULL ;
			}
			break ;
		case dispCntl :
			(**macList).vScroll = NULL ; // will be disposed by the window itself
			(**macList).hScroll = NULL ; // will be disposed by the window itself
			LDispose( macList ) ;
			break ;
		case calcCntlRgn :
			RectRgn((RgnHandle) param , &(**theControl).contrlRect ) ;
			break ;
		case testCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  Rect	listRect ;
			  Rect	allRect = (**theControl).contrlRect ;
			  listRect = allRect ;
			  if ( !inLoop )
					allRect.right += 16 ;
			  if ( PtInRect( pt , &allRect ) )
			  {
			  	if ( !inLoop && StillDown() )
			  	{
			  		inLoop = true ;
			  		LActivate( true , macList ) ;
					Boolean doubleClick = LClick( pt , 0 /* todo modifiers*/ , macList ) ;
					info->privateData = doubleClick ;
					inLoop = false ;
				}
				return kControlListBoxPart ;
			  }
			  else
				return NULL ;
			}
			break ;
		case drawCntl :
			{
				AGAPortHelper help((**theControl).contrlOwner) ;
				AGASetFontStyle( &info->fontStyle ) ;
				RGBBackColor( &gAGARamp[ kAGAWhite ] ) ;
				EraseRect( &(**theControl).contrlRect ) ;
				FrameRect( &(**theControl).contrlRect ) ;
				RgnHandle controlRgn = NewRgn() ;
				SetRectRgn( controlRgn, (**theControl).contrlRect.left ,
					(**theControl).contrlRect.top,
					(**theControl).contrlRect.right,
					(**theControl).contrlRect.bottom ) ;
				LUpdate( controlRgn,  macList ) ;
			}
			break ;
		case posCntl :
			{
			  Point pt ;
			  pt.h = LoWord( param ) ;
			  pt.v = HiWord( param ) ;
			  Rect	listRect ;
			  Rect	allRect = (**theControl).contrlRect ;
			  listRect = allRect ;
			  allRect.right += 16 ;
			  if ( PtInRect( pt , &listRect ) )
			  {
			  	if ( !inLoop && StillDown() )
			  	{
			  		inLoop = true ;
			  		LActivate( true , macList ) ;
						Boolean doubleClick = LClick( pt , 0 /* todo modifiers*/ , macList ) ;
						info->privateData = doubleClick ;
						inLoop = false ;
					}
					return kControlListBoxPart ;
				}
				else if ( PtInRect( pt , &allRect ) )
				{
			  	if ( !inLoop && StillDown() )
			  	{
			  		inLoop = true ;
			  		// LActivate( true , macList ) ;
						Boolean doubleClick = LClick( pt , 0 /* todo modifiers*/ , macList ) ;
						info->privateData = doubleClick ;
						inLoop = false ;
					}
					return kControlPageDownPart ;
				}
				else
					return NULL ;
			}
			break ;
		case kControlMsgGetData :
			{
				ControlDataAccessRec *rec = (ControlDataAccessRec *) param ;
				if ( rec->tag == kControlListBoxListHandleTag )
				{
					return AGAGetDataHelper( rec , sizeof( ListHandle ) , (char*) &macList ) ;
				}
				else if ( rec->tag == kControlListBoxDoubleClickTag )
				{
					Boolean doubleClick = info->privateData ;
					return AGAGetDataHelper( rec , sizeof( Boolean ) , (char*) &doubleClick ) ;
				}
				return errDataNotSupported ;
			}
			break ;
		default :
			break ;
	}
	
	return 0 ;									
}

OSErr AGACreateRootControl				(WindowPtr 				inWindow,
								 ControlHandle *		outControl) 
{
	ControlHandle theControl = NULL ;

	SInt16 extCDEFID = kExtCDEFID << 4 + 0 ;

 	theControl = NewControl( inWindow , &inWindow->portRect , "\p" , true ,
 			0 , 0 , 1 , extCDEFID , 0 ) ;
 	NewExtCDEFInfo( theControl , AGARootControlDefProc , kAGARootControlProcID , 0 ) ;
 	
 	*outControl = theControl ;
	return noErr ;
}

OSErr AGAEmbedControl					(ControlHandle 			inControl,
								 ControlHandle 			inContainer) 
{
	if ( AGATestForNewControl( inControl ) )
	{
		ExtCDEFInfo* info = (ExtCDEFInfo* )  (**inControl).contrlRfCon ;	
		info->containerControl = inContainer ;
	}
	return noErr ;
}

void AGADrawControl( ControlHandle control )
{
	DrawOneControl( control ) ;
	ControlHandle iter = (ControlHandle) ((WindowPeek)(**control).contrlOwner)->controlList ; 
	while ( iter )
	{
		if ( AGATestForNewControl( iter ) )
		{
			ExtCDEFInfo* info = (ExtCDEFInfo* )  (**iter).contrlRfCon ;
			if ( info->containerControl == control )
			{ 		
				AGADrawControl( iter  ) ;
			}
		}
	
		iter = (**iter).nextControl ;
	}
}


ControlHandle AGANewControl(WindowPtr 				owningWindow,
								 const Rect *			bounds,
								 ConstStr255Param 		controlTitle,
								 Boolean 				initiallyVisible,
								 SInt16 				initialValue,
								 SInt16 				minimumValue,
								 SInt16 				maximumValue,
								 SInt16 				procID,
								 SInt32 				controlReference)
{
	ControlHandle theControl = NULL ;
	Rect boundsRect = *bounds ;
	SInt16 extCDEFID = kExtCDEFID << 4 + 0 ;
 	SInt16 oldProcID = extCDEFID ;
 	switch( procID ) 
 	{
 		case kControlScrollBarProc :
 		case kControlScrollBarLiveProc :
 			oldProcID = scrollBarProc ;
 			break ;
 		case kControlListBoxProc :
 			initialValue = 0 ;
		default :
 			break ;
 	}
 	if ( oldProcID == extCDEFID && procID == kControlListBoxProc )
 	{
 		boundsRect.right -= 16 ;
	}
 	theControl = NewControl( owningWindow , &boundsRect , controlTitle , initiallyVisible ,
 			initialValue , minimumValue , maximumValue , oldProcID , controlReference ) ;
 	if ( oldProcID == extCDEFID )
 	{
 		ControlDefProcPtr theProcPtr = NULL ;
 		SInt16 theVarCode = 0 ;
 		switch( procID )
 		{
 			case kControlPushButtonProc :
 				theProcPtr = AGAButtonDefProc ;
 				break ;
 			case kControlCheckBoxProc :
 				theProcPtr = AGACheckBoxDefProc ;
 				break ;
 			case kControlRadioButtonProc :
 				theProcPtr = AGARadioButtonDefProc ;
 				break ;
 			case kControlProgressBarProc :
 				theProcPtr = AGAProgressBarDefProc ;
 				break ;
 			case kControlPlacardProc :
 				theProcPtr = AGAPlacardDefProc ;
 				break ;
 			case kControlStaticTextProc :
 				theProcPtr = AGAStaticTextDefProc ;
 				break ;
 			case kControlListBoxProc :
 				theProcPtr = AGAListControlDefProc ;
 				break ;
 			case kControlEditTextProc :
 				theProcPtr = AGAEditTextDefProc ;
 				break ;
 			case kControlGroupBoxTextTitleProc :
 				theProcPtr = AGAStaticGroupBoxTextDefProc ;
 				break ;
 			case kControlBevelButtonNormalBevelProc :
 				theProcPtr = AGABevelButtonDefProc ;
 				break ;
 		}
 		
 		if ( theProcPtr )
 		{
 			NewExtCDEFInfo( theControl , theProcPtr , procID , controlReference ) ;
 		}
	}
	return theControl ;
}

void			AGASetThemeWindowBackground		(WindowRef 				inWindow,
								 ThemeBrush 			inBrush,
								 Boolean 				inUpdate) 
{
	GrafPtr port ;
	GetPort( &port ) ;
	SetPort( inWindow ) ;
	if ( inBrush == kThemeBrushDialogBackgroundActive )
	{
			gAGABackgroundColor = 2 ;
	}
	else
	{
			gAGABackgroundColor = 0 ;
	}
	RGBBackColor( &gAGAColorArray[gAGABackgroundColor] ) ;
	SetPort( port ) ;
}

void AGAApplyThemeBackground(ThemeBackgroundKind 	inKind,
								 const Rect *			bounds,
								 ThemeDrawState 		inState,
								 SInt16 				inDepth,
								 Boolean 				inColorDev) 
{
}

#endif
AGAPortHelper::AGAPortHelper( GrafPtr newport) 
{
	GetPort( &port ) ;
	SetPort( newport ) ;
//	wxASSERT( newport->portRect.left == 0 && newport->portRect.top == 0 ) ; 
	GetPenState( &oldPenState ) ;
	GetBackColor( &oldBackColor ) ;
	GetForeColor( &oldForeColor ) ;

	clip = NewRgn() ;
	GetClip( clip );
	font = GetPortTextFont( newport);
	size = GetPortTextSize( newport);
	style = GetPortTextFace( newport);
	mode = GetPortTextMode( newport);	
	nport = newport ;

}
AGAPortHelper::AGAPortHelper()
{
	clip = NULL ;
}
void AGAPortHelper::Setup( GrafPtr newport )
{
	GetPort( &port ) ;
	SetPort( newport ) ;
//	wxASSERT( newport->portRect.left == 0 && newport->portRect.top == 0 ) ; 
	GetPenState( &oldPenState ) ;
	GetBackColor( &oldBackColor ) ;
	GetForeColor( &oldForeColor ) ;

	clip = NewRgn() ;
	GetClip( clip );
	font = GetPortTextFont( newport);
	size = GetPortTextSize( newport);
	style = GetPortTextFace( newport);
	mode = GetPortTextMode( newport);	
	nport = newport ;
}
void AGAPortHelper::Clear()
{
	if ( clip )
	{
		DisposeRgn( clip ) ;
		clip = NULL ;
	}
}
AGAPortHelper::~AGAPortHelper()
{
	if ( clip )
	{
		SetPort( nport ) ;
		SetClip( clip ) ;
		DisposeRgn( clip ) ;
		RGBForeColor(&oldForeColor);
		RGBBackColor(&oldBackColor);
		SetPenState(&oldPenState);				

		TextFont( font );
		TextSize( size );
		TextFace( style );
		TextMode( mode );
		SetOrigin( 0 , 0 ) ;
		SetPort( port ) ;
	}
}

