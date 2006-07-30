#include "extldef.h"  

/*************************************************************************

	function prototypes

*************************************************************************/

void DrawMsg(Boolean fSelect, Rect *r, Cell cell, ListHandle lh);
void HiliteMsg(Boolean fSelect, Rect *r);

/*************************************************************************

	main

*************************************************************************/

pascal void main(short message, Boolean fSelect, Rect *r, Cell cell,
				 short dataOffset, short dataLen, ListHandle lh)
{

	switch(message) {
		case lInitMsg:
			break;
		
		case lDrawMsg:
			DrawMsg(fSelect, r, cell, lh);
			break;
		
		case lHiliteMsg:
			HiliteMsg(fSelect, r);
			break;
			
		case lCloseMsg:
			break;

		default:
			break;
	}
}

/*************************************************************************

	DrawMsg

*************************************************************************/

void DrawMsg(Boolean fSelect, Rect *r, Cell cell, ListHandle lh)
{
	ExtLDEFInfo* info = (ExtLDEFInfo*) (**lh).refCon ;
	GrafPtr			savePort;
	
	// set up the port
	GetPort(&savePort);
	SetPort((**lh).port);
	PenNormal();
	ForeColor( blackColor ) ;
	BackColor( whiteColor ) ; 
	
	EraseRect(r);
	if ( info )
	{
		if ( info->drawProc) 
		{
			CallExtLDEFDrawProc(info->drawProc, r, cell, lh, info->refCon );
		}
		
	}
	// hilite if selected
	if (fSelect)
		HiliteMsg(fSelect, r);

	SetPort(savePort);	

}

/*************************************************************************

	HiliteMsg

*************************************************************************/

void HiliteMsg(Boolean fSelect, Rect *r)
{
	unsigned char	hMode;

	hMode = LMGetHiliteMode();
	BitClr((Ptr)(&hMode),(long)pHiliteBit);
	LMSetHiliteMode(hMode);
	InvertRect(r);
}
