/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by: 19990908 : mj10777@gmx.net
//              - rename to tabpgwin
//              - restruction of Variable declaration
//              - to prevent Warnings under MingW32
// Modified by: 19990909 : mj10777@gmx.net
//              - mNoVertScroll TRUE = no / FALSE = Original Code
//                the Original Code Paints a Vertical Scroll in wxPagedWindow
//                which is not needed in this Version. Use TRUE for this.
// Created:     07/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:    wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <math.h>
#include <stdlib.h>

#include "wx/string.h"
#include "wx/utils.h"     // import wxMin/wxMax macros and wxFileExist(..)

#include "tabpgwin.h"

//---------------------------------------------------------------------------
// Implementation for class twTabInfo
//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS( twTabInfo, wxObject )

//---------------------------------------------------------------------------
twTabInfo::twTabInfo()
: mpContent( 0 )
{}

//---------------------------------------------------------------------------
twTabInfo::~twTabInfo()
{
    // FOR NOW:: nothing
}

//---------------------------------------------------------------------------
int twTabInfo::ImgWidth()
{
    if ( mBitMap.Ok() ) return mBitMap.GetWidth();
    else return 0;
}

//---------------------------------------------------------------------------
int twTabInfo::ImgHeight()
{
    if ( mBitMap.Ok() )
        return mBitMap.GetHeight();
    else
        return 0;
}

//---------------------------------------------------------------------------
int twTabInfo::ImageToTxtGap( int prefGap )
{
    if ( mBitMap.Ok() )
        return prefGap;
    else
        return 0;
}

//---------------------------------------------------------------------------
bool twTabInfo::HasImg()
{
    return mBitMap.Ok();
}

//---------------------------------------------------------------------------
// bool twTabInfo::HasText();
unsigned int twTabInfo::HasText()
{
    return mText.Length();
}

//---------------------------------------------------------------------------
wxBitmap& twTabInfo::GetImg()
{
    return mBitMap;
}

//---------------------------------------------------------------------------
wxString& twTabInfo::GetText()
{
    return mText;
}

//---------------------------------------------------------------------------
wxWindow& twTabInfo::GetContent()
{
    return *mpContent;
}

//---------------------------------------------------------------------------
// Implementation for class wxTabbedWindow
//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS( wxTabbedWindow, wxPanel )

//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE( wxTabbedWindow, wxPanel )
    EVT_SIZE ( wxTabbedWindow::OnSize )
    EVT_PAINT( wxTabbedWindow::OnPaint )
    EVT_LEFT_DOWN( wxTabbedWindow::OnLButtonDown )
    // TDB:: filciker reduction
    // EVT_ERASE_BACKGROUND( wxTabbedWindow::OnBkErase )
END_EVENT_TABLE()

//---------------------------------------------------------------------------
wxTabbedWindow::wxTabbedWindow()

:   mpTabScroll  ( NULL ),
    mpHorizScroll( NULL ),
    mpVertScroll ( NULL ),

    mVertGap ( 0 ),
    mHorizGap( 0 ),

    mTitleVertGap ( 3  ),
    mTitleHorizGap( 4  ),
    mImageTextGap ( 2  ),
    mFirstTitleGap( 11 ),

    mBorderOnlyWidth( 8 ),

    mWhitePen( wxColour(255,255,255), 1, wxSOLID ),
    mGrayPen ( wxColour(192,192,192), 1, wxSOLID ),
    mDarkPen ( wxColour(128,128,128), 1, wxSOLID ),
    mBlackPen( wxColour(  0,  0,  0), 1, wxSOLID ),

    // state variables
    mActiveTab  ( 0 ),
    mTitleHeight( 0 ),
    mLayoutType( wxTITLE_IMG_AND_TEXT )
{}

//---------------------------------------------------------------------------
wxTabbedWindow::~wxTabbedWindow()
{
    wxNode* pTab = mTabs.First();
    
    while( pTab )
    {
        delete ((twTabInfo*)pTab->Data());
        pTab = pTab->Next();
    }
}

//---------------------------------------------------------------------------
void wxTabbedWindow::SizeTabs(int x,int y, int width, int height, bool repant)
{
    wxNode* pTabNode = mTabs.First();
    int n = 0;
    
    while( pTabNode )
    {
        twTabInfo& info = *((twTabInfo*)pTabNode->Data());
        
        if ( n == mActiveTab )
        {
            //wxSizeEvent evt;
            //info.mpContent->GetEventHandler()->ProcessEvent( evt );
            
            info.mpContent->SetSize( x, y, width, height, 0 );
            info.mpContent->Show(TRUE);
            info.mpContent->Refresh();
            
        }
        else
        {
            info.mpContent->Show(FALSE);
        }
        
        pTabNode = pTabNode->Next();
        ++n;
    }
}

//---------------------------------------------------------------------------
void wxTabbedWindow::AddTab( wxWindow* pContent,
                            wxString  tabText,
                            wxString  imageFileName,
                            long      imageType )
{
    twTabInfo* pTab = new twTabInfo();
    
    pTab->mpContent = pContent;
    pTab->mText     = tabText;
    
    if ( wxFileExists( imageFileName ) &&
        
        pTab->mBitMap.LoadFile( imageFileName, imageType ) )
    {
        pTab->mImageFile = imageFileName;
        pTab->mImageType = imageType;
    }
    
    
    if ( pContent->GetParent() == NULL )
        pContent->Create( this, -1 );
    
    mTabs.Append( (wxObject*)pTab );
    
    RecalcLayout(TRUE);
    
    OnTabAdded( pTab );
}

//---------------------------------------------------------------------------
void wxTabbedWindow::AddTab( wxWindow* pContent,
                            wxString  tabText, wxBitmap* pImage   )
{
    twTabInfo* pTab = new twTabInfo();
    
    pTab->mpContent = pContent;
    pTab->mText     = tabText;
    
    if ( pImage )
        pTab->mBitMap = *pImage;
    
    if ( pContent->GetParent() == NULL )
        pContent->Create( this, -1 );
    
    mTabs.Append( (wxObject*)pTab );
    RecalcLayout(TRUE);
    OnTabAdded( pTab );
}

//---------------------------------------------------------------------------
void wxTabbedWindow::RemoveTab( int tabNo )
{
    twTabInfo* pTab = ((twTabInfo*)(mTabs.Nth( tabNo )->Data()));
    pTab->mpContent->Destroy();
    delete pTab;
    mTabs.DeleteNode( mTabs.Nth( tabNo ) );
    // if ( mActiveTab >= mTabs.Number() );
    if ( mActiveTab >= mTabs.Number() )
        mActiveTab = mTabs.Number() - 1;
    SetActiveTab( mActiveTab );
}

//---------------------------------------------------------------------------
int wxTabbedWindow::GetTabCount()
{
    return mTabs.Number();
}

//---------------------------------------------------------------------------
wxWindow* wxTabbedWindow::GetTab( int tabNo )
{
    return ((twTabInfo*)(mTabs.Nth( tabNo )->Data()))->mpContent;
}

//---------------------------------------------------------------------------
wxWindow* wxTabbedWindow::GetActiveTab()
{
    // FIMXE:: this is lame
    return GetTab( mActiveTab );
}

//---------------------------------------------------------------------------
void wxTabbedWindow::SetActiveTab( int tabNo )
{
    mActiveTab = tabNo;
    RecalcLayout(TRUE);
    Refresh();
}

//---------------------------------------------------------------------------
// width of the decorations border (4 shade-lines), should not be changed
//---------------------------------------------------------------------------
#define BORDER_SZ 4

//---------------------------------------------------------------------------
void wxTabbedWindow::DrawShadedRect( int x, int y, int width, int height,
                                    wxPen& upperPen, wxPen& lowerPen, wxDC& dc
                                    )
{
    // darw the lightened upper-left sides of the rectangle
    
    dc.SetPen( upperPen );
    dc.DrawLine( x,y, x, y + height - 1 ); // vert
    dc.DrawLine( x,y, x + width - 1,  y ); // horiz
    
    // draw the unenlightened lower-right sides of the rectangle
    
    dc.SetPen( lowerPen );
    dc.DrawLine( x + width - 1,  y, x + width - 1, y + height - 1 ); // vert
    dc.DrawLine( x, y + height - 1, x + width, y + height - 1 );     // horiz
}

//---------------------------------------------------------------------------
void wxTabbedWindow::DrawDecorations( wxDC& dc )
{
    // Protability NOTE::: DrawLine(..) draws a line from the first position,
    //                     but not including the point specified by last position.
    //                     This way Windows draws lines, not sure how Motif and Gtk
    //                     prots behave...
    
    int width, height;
    GetClientSize( &width, &height );
    
    // check if there's at least a bit of space to draw things
    
    if ( width  < mHorizGap*2 + BORDER_SZ*2+1 ||
        height < mVertGap*2  + BORDER_SZ*2+1 + mTitleHeight
        )
        return;
    
    // step #1 - draw border around the tab content area
    
    // setup position for kind of "pencil"
    int curX = mHorizGap;
    int curY = mVertGap;
    
    int xSize = width  - mHorizGap*2;
    int ySize = height - mVertGap *2  - mTitleHeight;
    
    // layer 1 (upper white)
    DrawShadedRect( curX+0, curY+0, xSize-0, ySize-0,
        mWhitePen, mBlackPen, dc  );
    
    // layer 2 (upper gray)
    DrawShadedRect( curX+1, curY+1, xSize-2-1, ySize-2-1,
        mGrayPen, mGrayPen, dc  );
    
    // layer 3 (upper darkGray)
    DrawShadedRect( curX+2, curY+2, xSize-3-2, ySize-3-2,
        mDarkPen, mWhitePen, dc  );
    
    // layer 4 (upper black)
    DrawShadedRect( curX+3, curY+3, xSize-4-3, ySize-4-3,
        mBlackPen, mGrayPen, dc  );
    
    // add non-siemtric layer from the lower-right side (confroming to MFC-look)
    
    dc.SetPen( mDarkPen );
    dc.DrawLine( curX+1, curY + ySize - 2, curX + xSize - 1, curY + ySize - 2 );   // horiz
    dc.DrawLine( curX + xSize - 2, curY + 1, curX + xSize - 2, curY + ySize - 2 ); // vert
    
    // step #2 - draw tab title bars
    
    curX = mFirstTitleGap;
    curY = height - mVertGap - mTitleHeight;
    
    int tabNo = 0;
    wxNode* pNode = mTabs.First();
    
    while( pNode )
    {
        // "hard-coded metafile" for decorations
        
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        xSize = tab.mDims.x;
        ySize = mTitleHeight;
        
        if ( tabNo == mActiveTab )
        {
            dc.SetPen( mGrayPen );
            dc.DrawLine( curX+1, curY-2, curX+xSize-2, curY-2 );
            dc.DrawLine( curX+1, curY-1, curX+xSize-2, curY-1 );
        }
        
        dc.SetPen( mWhitePen );
        
        if ( tabNo == mActiveTab )
            dc.DrawLine( curX, curY-2, curX, curY+ySize-2 );
        else
            dc.DrawLine( curX, curY, curX, curY+ySize-2 );
        
        dc.SetPen( mDarkPen );
        dc.DrawLine( curX+1, curY+ySize-3, curX+1, curY+ySize-1 ); // to pix down
        dc.DrawLine( curX+2, curY+ySize-2, curX+xSize-2, curY+ySize-2 );
        dc.DrawLine( curX+xSize-3, curY+ySize-3, curX+xSize-2, curY+ySize-3 );
        if ( tabNo == mActiveTab )
            dc.DrawLine( curX+xSize-2, curY+ySize-3, curX+xSize-2, curY-3 );
        else
            dc.DrawLine( curX+xSize-2, curY+ySize-3, curX+xSize-2, curY-1 );
        
        dc.SetPen( mBlackPen );
        dc.DrawLine( curX+xSize-1, curY, curX+xSize-1, curY+ySize-2 );
        dc.DrawLine( curX+xSize-2, curY+ySize-2, curX+xSize-3, curY+ySize-2 );
        dc.DrawLine( curX+xSize-3, curY+ySize-1, curX+1, curY+ySize-1 );
        
        pNode = pNode->Next();
        ++tabNo;
        
        // darw image and (or without) text centered within the
        // title bar rectangle
        
        if ( mLayoutType != wxTITLE_BORDER_ONLY && tab.HasImg() )
        {
            wxMemoryDC tmpDc;
            tmpDc.SelectObject( tab.GetImg() );
            
            dc.Blit( curX + mTitleHorizGap,
                curY + ( ySize - tab.ImgHeight() ) / 2,
                tab.ImgWidth(),
                tab.ImgHeight(),
                &tmpDc, 0, 0, wxCOPY
                );
        }
        
        if ( mLayoutType == wxTITLE_IMG_AND_TEXT && tab.HasText() )
        {
            long x,w,h;
            
            // set select default font of the window into it's device context
            //dc.SetFont( GetLabelingFont() );
            
            dc.SetTextBackground( GetBackgroundColour() );
            
            dc.GetTextExtent(tab.mText, &w, &h );
            
            x = curX + mTitleHorizGap +
                tab.ImgWidth() + tab.ImageToTxtGap(mImageTextGap);
            
            dc.DrawText( tab.GetText(), x, curY + ( ySize - h ) / 2 );
        }
        curX += xSize;
        
    } // end of `while (pNode)'
}  // wxTabbedWindow::DrawDecorations()

//---------------------------------------------------------------------------
int wxTabbedWindow::HitTest( const wxPoint& pos )
{
    int width, height;
    GetClientSize( &width, &height );
    
    int curX = mFirstTitleGap;
    int curY = height - mVertGap - mTitleHeight;
    
    int     tabNo = 0;
    wxNode* pNode = mTabs.First();
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        int w,h;
        w = tab.mDims.x;
        h = tab.mDims.y;
        // hit test rectangle of the currnet tab title bar
        if ( pos.x >= curX && pos.x < curX + tab.mDims.x  &&
            pos.y >= curY && pos.y < curY + tab.mDims.y
            )
        {
            return tabNo;
        }
        
        curX += tab.mDims.x;
        
        pNode = pNode->Next();
        ++tabNo;
    }
    
    return -1;
}  // wxTabbedWindow::HitTest()

//---------------------------------------------------------------------------
void wxTabbedWindow::HideInactiveTabs( bool andRepaint )
{
    if ( !andRepaint )
        return;
    
    wxNode* pNode = mTabs.First();
    int     tabNo = 0;
    
    while( pNode )
    {
        if ( tabNo != mActiveTab )
        {
            twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
            tab.mpContent->Show(FALSE);
        }
        
        pNode = pNode->Next();
        ++tabNo;
    }
}  // wxTabbedWindow::HideInactiveTabs()

//---------------------------------------------------------------------------
wxFont wxTabbedWindow::GetLabelingFont()
{
    wxFont font;
#ifdef __WINDOWS__
    font.SetFaceName("MS Sans Serif");
#else
    font.SetFamily( wxSWISS );
#endif
    
    font.SetStyle(40);
    font.SetWeight(40);
    font.SetPointSize( 8 );
    
#ifdef __WINDOWS__
    font.RealizeResource();
#endif
    
    return font;
}  // wxTabbedWindow::GetLabelingFont()

//---------------------------------------------------------------------------
void wxTabbedWindow::RecalcLayout(bool andRepaint)
{
    HideInactiveTabs(andRepaint);
    
    // resetup position of the active tab
    
    int width, height;
    GetClientSize( &width, &height );
    
    int curX = mHorizGap + BORDER_SZ;
    int curY = mVertGap  + BORDER_SZ;
    
    int xSize = width  - mHorizGap*2 - BORDER_SZ*2-1;
    int ySize = height - mVertGap*2  - BORDER_SZ*2-1 - mTitleHeight;
    
    SizeTabs( curX, curY, xSize, ySize, andRepaint );
    
    // pass #1 - try to layout assuming it's wxTITLE_IMG_AND_TEXT
    
    mLayoutType = wxTITLE_IMG_AND_TEXT;
    
    wxNode* pNode = mTabs.First();
    
    curX = mFirstTitleGap; // the left-side gap
    mTitleHeight = 0;
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        wxWindowDC dc(this);
        
        long w,h;
        
        // set select default font of the window into it's device context
        //dc.SetFont( GetLabelingFont() );
        
        dc.GetTextExtent(tab.mText, &w, &h );
        
        tab.mDims.x = w + tab.ImageToTxtGap(mImageTextGap) +
            tab.ImgWidth() + mTitleHorizGap*2;
        
        tab.mDims.y  = wxMax( h, tab.ImgHeight() ) + mTitleVertGap*2;
        mTitleHeight = wxMax( mTitleHeight, tab.mDims.y );
        
        curX += tab.mDims.x;
        
        pNode = pNode->Next();
    }
    
    curX += mHorizGap; // the right-side gap
    
    // make all title bars of equel height
    
    pNode = mTabs.First();
    
    while( pNode )
    {
        ((twTabInfo*)(pNode->Data()))->mDims.y = mTitleHeight;;
        pNode = pNode->Next();
    }
    
    // if curX has'nt ran out of bounds, leave TITLE_IMG layout and return
    if ( curX < width - mHorizGap )
        return;
    
    // pass #2 - try to layout assuming wxTITLE_IMG_ONLY
    
    mLayoutType = wxTITLE_IMG_ONLY;
    
    pNode = mTabs.First();
    
    curX = mFirstTitleGap; // the left-side gap
    
    int denomiator = mTabs.Number();
    if ( denomiator == 0 )
        ++denomiator;
    
    mBorderOnlyWidth = (width - mFirstTitleGap - mHorizGap) / denomiator;
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        if ( tab.HasImg() )
        {
            tab.mDims.x = tab.ImgWidth()  + mTitleHorizGap*2;
            tab.mDims.y = tab.ImgHeight() + mTitleVertGap*2;
        }
        else
        {
            tab.mDims.x = mBorderOnlyWidth;
            tab.mDims.y = mTitleHeight;
        }
        
        curX += tab.mDims.x;
        
        pNode = pNode->Next();
    }
    
    curX += mHorizGap; // the right-side gap
    
    // if curX has'nt ran out of bounds, leave IMG_ONLY layout and return
    if ( curX < width  - mHorizGap )
        return;
    
    // pass #3 - set the narrowest layout wxTITLE_BORDER_ONLY
    
    mLayoutType = wxTITLE_BORDER_ONLY;
    
    pNode = mTabs.First();
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        tab.mDims.x = mBorderOnlyWidth;
        tab.mDims.y = mTitleHeight;
        
        pNode = pNode->Next();
    }
}  // wxTabbedWindow::RecalcLayout()

//---------------------------------------------------------------------------
// wx event handlers
//---------------------------------------------------------------------------
void wxTabbedWindow::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc(this);
    DrawDecorations( dc );
}

//---------------------------------------------------------------------------
void wxTabbedWindow::OnSize ( wxSizeEvent& event )
{
    SetBackgroundColour( wxColour( 192,192,192 ) );
    RecalcLayout(TRUE);
}

//---------------------------------------------------------------------------
void wxTabbedWindow::OnBkErase( wxEraseEvent& event )
{
    // do nothing
}

//---------------------------------------------------------------------------
void wxTabbedWindow::OnLButtonDown( wxMouseEvent& event )
{
    // floats, why?
    int x = (int)event.m_x;
    int y = (int)event.m_y;
    
    int tabNo = HitTest( wxPoint(x,y) );
    
    if ( tabNo != -1 )
    {
        SetActiveTab( tabNo );
    }
}

//---------------------------------------------------------------------------
// Implementation for class wxPagedWindow
//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS( wxPagedWindow, wxTabbedWindow )

//---------------------------------------------------------------------------
BEGIN_EVENT_TABLE( wxPagedWindow, wxTabbedWindow )
    EVT_SIZE     ( wxPagedWindow::OnSize        )
    EVT_PAINT    ( wxPagedWindow::OnPaint       )
    EVT_LEFT_DOWN( wxPagedWindow::OnLButtonDown )
    EVT_LEFT_UP  ( wxPagedWindow::OnLButtonUp   )
    EVT_MOTION   ( wxPagedWindow::OnMouseMove   )
    EVT_SCROLL   ( wxPagedWindow::OnScroll      )
END_EVENT_TABLE()

//---------------------------------------------------------------------------
// border for paged-window is 2 shaded-lines
//---------------------------------------------------------------------------
#undef  BORDER_SZ
#define BORDER_SZ 2

//---------------------------------------------------------------------------
wxPagedWindow::wxPagedWindow()

:   mScrollEventInProgress( FALSE ),
    mTabTrianGap(4),
    mWhiteBrush( wxColour(255,255,255), wxSOLID ),
    mGrayBrush ( wxColour(192,192,192), wxSOLID ),
    mCurentRowOfs( 0 ),
    mAdjustableTitleRowLen( 300 ),
    mIsDragged    ( FALSE ),
    mDagOrigin    ( 0 ),
    mCursorChanged( FALSE ),
    mResizeCursor ( wxCURSOR_SIZEWE ),
    mNormalCursor ( wxCURSOR_ARROW  )
{
    mTitleVertGap = 2;
    mTitleHorizGap = 10;
    mNoVertScroll = TRUE;       // Horizontale Scroll abschalten
}

//---------------------------------------------------------------------------
wxPagedWindow::~wxPagedWindow()
{
    // nothing (base class handles destruction)
}

//---------------------------------------------------------------------------
wxFont wxPagedWindow::GetLabelingFont()
{
    wxFont font;
    
#ifdef __WINDOWS__
    font.SetFaceName("Comic Sans MS");
#else
    font.SetFamily( wxSWISS );
#endif
    
    font.SetStyle(40);
    font.SetWeight(40);
    font.SetPointSize( 8 );
    
    return font;
}

//---------------------------------------------------------------------------
void wxPagedWindow::OnTabAdded( twTabInfo* pInfo )
{
    int units = GetWholeTabRowLen() / 20;
    
    mpTabScroll->SetScrollbar( 0, 1, units, 1, FALSE );
}

//---------------------------------------------------------------------------
wxScrollBar& wxPagedWindow::GetVerticalScrollBar()
{
    return *mpVertScroll;
}

//---------------------------------------------------------------------------
wxScrollBar& wxPagedWindow::GetHorizontalScrollBar()
{
    return *mpHorizScroll;
}

//---------------------------------------------------------------------------
int wxPagedWindow::GetWholeTabRowLen()
{
    wxNode* pNode = mTabs.First();
    
    int len = 0;
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        len += tab.mDims.x;
        pNode = pNode->Next();
    }
    
    return len;
}  // wxPagedWindow::GetWholeTabRowLen()

//---------------------------------------------------------------------------
void wxPagedWindow::DrawPaperBar( twTabInfo& tab, int x, int y,
                                  wxBrush& brush, wxPen& pen, wxDC& dc )
{
    wxPoint poly[4];
    
    // draw organizer-style paper outlet
    
    poly[0].x = x - mTabTrianGap;
    poly[0].y = y;
    
    poly[1].x = x + mTabTrianGap;
    poly[1].y = y + tab.mDims.y-1;
    
    poly[2].x = x + tab.mDims.x - mTabTrianGap;
    poly[2].y = y + tab.mDims.y-1;
    
    poly[3].x = x + tab.mDims.x + mTabTrianGap;
    poly[3].y = y;
    
    dc.SetPen( pen );
    dc.SetBrush( brush );
    
    dc.DrawPolygon( 4, poly );
    
    long w,h;
    
    // set select default font of the window into it's device context
    //dc.SetFont( GetLabelingFont() );
    
    dc.SetTextBackground( brush.GetColour() );
    
    dc.GetTextExtent(tab.mText, &w, &h );
    
    if ( tab.HasImg() )
    {
        wxMemoryDC tmpDc;
        tmpDc.SelectObject( tab.GetImg() );
        
        dc.Blit( x + mTitleHorizGap,
            y + ( tab.mDims.y - tab.ImgHeight() ) / 2,
            tab.ImgWidth(),
            tab.ImgHeight(),
            &tmpDc, 0, 0, wxCOPY
            );
    }
    
    if ( tab.HasText() )
    {
        int tx = x + mTitleHorizGap +
            tab.ImgWidth() + tab.ImageToTxtGap(mImageTextGap);
        
        dc.DrawText( tab.GetText(), tx, y + ( tab.mDims.y - h ) / 2 );
    }
}  // wxPagedWindow::DrawPaperBar()

//---------------------------------------------------------------------------
void wxPagedWindow::DrawDecorations( wxDC& dc )
{
    // FIXME:: the is big body have to be split!

    int width, height;
    GetClientSize( &width, &height );

    int curX = mHorizGap;
    int curY = mVertGap;

    int xSize = width  - mHorizGap*2;
    int ySize = height - mVertGap*2;

    DrawShadedRect( curX, curY, xSize, ySize,
        mDarkPen, mWhitePen, dc );

    DrawShadedRect( curX+1, curY+1, xSize-2, ySize-2,
        mBlackPen, mGrayPen, dc );

    // draw inactive tab title bars frist (left-to-right)

    wxNode* pNode = mTabs.First();
    int     tabNo = 0;

    /* OLD STUFF::
    curX = mTitleRowStart;
    curY = height - mVertGap - BORDER_SZ - mTitleHeight;
    */

    curX = mTabTrianGap;
    curY = 0;

    // FOR NOW:: avoid creating bitmap with invalid dimensions

    if ( mTitleRowLen < 1 || mTitleHeight < 1 )
        return;

    wxMemoryDC tmpDc;
    wxBitmap   tmpBmp( mTitleRowLen, mTitleHeight );

    tmpDc.SelectObject( tmpBmp );
    tmpDc.SetPen( mGrayPen );
    tmpDc.SetBrush( mGrayBrush );
    tmpDc.DrawRectangle( 0,0, mTitleRowLen, mTitleHeight );

    tmpDc.SetDeviceOrigin( mCurentRowOfs, 0 );

    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));

        if ( tabNo != mActiveTab )
            DrawPaperBar( tab, curX, curY, mGrayBrush, mBlackPen, tmpDc );

        curX += tab.mDims.x;

        pNode = pNode->Next();
        ++tabNo;
    }
    
    // finally, draw the active tab (white-filled)
    
    pNode = mTabs.First();
    tabNo = 0;
    
    curX = mTabTrianGap;
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        if ( tabNo == mActiveTab )
        {
            DrawPaperBar( tab, curX, curY, mWhiteBrush, mBlackPen, tmpDc );
            
            tmpDc.SetPen( mWhitePen );
            
            tmpDc.DrawLine( curX - mTabTrianGap+1, curY,
                curX + tab.mDims.x + mTabTrianGap, curY );
            break;
        }
        curX += tab.mDims.x;
        
        pNode = pNode->Next();
        ++tabNo;
    }
    
    // back to initial device origin
    
    tmpDc.SetDeviceOrigin( 0, 0 );
    
    // draw resize-hint-stick
    
    curX = mTitleRowLen - 6;
    
    DrawShadedRect( curX+0, 0+0, 6,   mTitleHeight,   mGrayPen,  mBlackPen, tmpDc );
    DrawShadedRect( curX+1, 0+1, 6-2, mTitleHeight-2, mWhitePen, mDarkPen,  tmpDc );
    DrawShadedRect( curX+2, 0+2, 6-4, mTitleHeight-4, mGrayPen,  mGrayPen,  tmpDc );
    
    
    
    dc.Blit( mTitleRowStart,
        height - mVertGap - BORDER_SZ - mTitleHeight,
        mTitleRowLen, mTitleHeight,
        &tmpDc, 0,0, wxCOPY );
}  // wxPagedWindow::DrawDecorations()

//---------------------------------------------------------------------------
int wxPagedWindow::HitTest( const wxPoint& pos )
{
    return wxTabbedWindow::HitTest( pos );
}

//---------------------------------------------------------------------------
void wxPagedWindow::RecalcLayout(bool andRepaint)
{
    mTitleRowLen = mAdjustableTitleRowLen;
    
    if ( int(mpTabScroll) == -1 ) return;
    
    // scroll bars should be created after Create() for this window is called
    if ( !mpTabScroll )
    {
        mpTabScroll   =
            new wxScrollBar( this, -1, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
        
        mpHorizScroll =
            new wxScrollBar( this, -1, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
        if (!mNoVertScroll)       // Vertical Scroll (Original)
            mpVertScroll = new wxScrollBar( this, -1, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
    }
    
    {
        int units = GetWholeTabRowLen() / 20;
        
        mpTabScroll->SetScrollbar( 0, 1, units, 1, FALSE );
    }
    
    // resetup position of the active tab
    
    int thumbLen = 16; // FOR NOW:: hardcoded
    
    int width, height;
    GetClientSize( &width, &height );
    
    mTitleHeight = thumbLen;
    
    int curX = mHorizGap + BORDER_SZ;
    int curY = mVertGap  + BORDER_SZ;
    
    int xSize;
    if (!mNoVertScroll)       // Vertical Scroll (Original)
        xSize = width  - mHorizGap*2 - BORDER_SZ*2 - thumbLen;
    else
        xSize = width  - mHorizGap*2 - BORDER_SZ*2;
    
    int ySize = height - mVertGap*2  - BORDER_SZ*2 - mTitleHeight;
    
    SizeTabs( curX, curY, xSize, ySize, andRepaint );
    
    // setup title bar LINES's horizontal scroll bar
    
    curY = height - mVertGap - BORDER_SZ - thumbLen;
    
    mpTabScroll->SetSize( curX, curY, thumbLen*2, thumbLen );
    
    // setup view's HORIZONTAL scroll bar
    curX += thumbLen*2;
    
    mTitleRowStart = curX;
    mFirstTitleGap = curX + mCurentRowOfs + mTabTrianGap;
    
    mTitleRowLen = wxMin( mAdjustableTitleRowLen,
        width - mHorizGap - BORDER_SZ - thumbLen*4 - curX );
    
    curX += mTitleRowLen;
    
    if (!mNoVertScroll)       // Vertical Scroll (Original)
        mpHorizScroll->SetSize( curX, curY,width - curX - mHorizGap - BORDER_SZ - thumbLen, thumbLen );
    else
        mpHorizScroll->SetSize( curX, curY,width - curX - mHorizGap - BORDER_SZ-4, thumbLen );
    
    // setup view's VERTICAL scroll bar
    if (!mNoVertScroll)       // Vertical Scroll (Original)
    {
        curX = width - mHorizGap - BORDER_SZ - thumbLen;
        curY = mVertGap  + BORDER_SZ;
        mpVertScroll->SetSize( curX, curY, thumbLen,height - curY - mVertGap - BORDER_SZ - thumbLen);
    }
    // layout tab title bars
    
    mLayoutType = wxTITLE_IMG_AND_TEXT;
    
    wxNode* pNode = mTabs.First();
    
    while( pNode )
    {
        twTabInfo& tab = *((twTabInfo*)(pNode->Data()));
        
        wxWindowDC dc(this);
        
        long w,h;
        
        // set select default font of the window into it's device context
        //dc.SetFont( GetLabelingFont() );
        dc.GetTextExtent(tab.mText, &w, &h );
        
        tab.mDims.x = w + tab.ImageToTxtGap(mImageTextGap) +
            tab.ImgWidth() + mTitleHorizGap*2;
        
        tab.mDims.y  = mTitleHeight;
        
        pNode = pNode->Next();
    }
    
    // disable title-bar scroller if there's nowhere to scroll to
    
    mpTabScroll->Enable( mTitleRowLen < GetWholeTabRowLen() || mCurentRowOfs < 0  );
}

//---------------------------------------------------------------------------
// event handlers
//---------------------------------------------------------------------------
void wxPagedWindow::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc(this);
    DrawDecorations( dc );
}

//---------------------------------------------------------------------------
void wxPagedWindow::OnSize ( wxSizeEvent& event )
{
    wxTabbedWindow::OnSize(event);
}

//---------------------------------------------------------------------------
void wxPagedWindow::OnLButtonDown( wxMouseEvent& event )
{
    if ( mCursorChanged )
    {
        mIsDragged = TRUE;
        mDagOrigin = event.m_x;
        
        mOriginalTitleRowLen = mAdjustableTitleRowLen;
        
        CaptureMouse();
    }
    else
    {
        wxTabbedWindow::OnLButtonDown( event );
    }
}  // wxPagedWindow::OnLButtonDown()

//---------------------------------------------------------------------------
void wxPagedWindow::OnLButtonUp( wxMouseEvent& event )
{
    if ( mIsDragged )
    {
        mIsDragged     = FALSE;
        mCursorChanged = FALSE;
        SetCursor( mNormalCursor );
        
        ReleaseMouse();
    }
} // wxPagedWindow::OnLButtonUp()

//---------------------------------------------------------------------------
void wxPagedWindow::OnMouseMove( wxMouseEvent& event )
{
    int width, height;
    GetClientSize( &width, &height );
    
    if ( !mIsDragged )
    {
        int y = height - mVertGap - BORDER_SZ - mTitleHeight;
        int x = mTitleRowStart + mTitleRowLen - 6;
        
        if ( event.m_x >= x && event.m_y >= y &&
            event.m_x <  x + 6               &&
            event.m_y <  y + mTitleHeight
            )
        {
            if ( !mCursorChanged )
            {
                SetCursor( mResizeCursor );
                
                mCursorChanged = TRUE;
            }
        }
        else
            if ( mCursorChanged )
            {
                SetCursor( mNormalCursor );
                
                mCursorChanged = FALSE;
            }
    }
    else
    {
        if ( mIsDragged )
        {
            mAdjustableTitleRowLen = mOriginalTitleRowLen + ( event.m_x - mDagOrigin );
            
            // FOR NOW:: fixed
            if ( mAdjustableTitleRowLen < 6 ) mAdjustableTitleRowLen = 6;
            
            wxWindowDC dc(this);
            DrawDecorations( dc );
            
            RecalcLayout(FALSE);
            
            //Refresh();
        }
    }
}  // wxPagedWindow::OnMouseMove()

//---------------------------------------------------------------------------
void wxPagedWindow::OnScroll( wxScrollEvent& event )
{
    wxScrollBar* pSender = (wxScrollBar*)event.GetEventObject();
    // wxMessageBox("wxPagedWindow::OnScroll","-I->");
    if ( pSender == mpTabScroll )
    {
        
        int maxUnits = GetWholeTabRowLen() / 20;
        
        mCurentRowOfs = -event.GetPosition()*maxUnits;
        
        mFirstTitleGap = mTitleRowStart + mCurentRowOfs + mTabTrianGap;
        
        // let' it automatically disable itself if it's time
        mpTabScroll->Enable( mTitleRowLen < GetWholeTabRowLen() || mCurentRowOfs < 0 );
        
        // repaint title bars
        wxWindowDC dc(this);
        DrawDecorations( dc );
    }
    else
    {
        if ( !mScrollEventInProgress )
        {
            mScrollEventInProgress = TRUE;
            
            GetActiveTab()->GetEventHandler()->ProcessEvent( event );
        }
        else
        {
            // event bounced back to us, from here we
            // know that it has traveled the loop - thus it's processed!
            
            mScrollEventInProgress = FALSE;
        }
    }
}  // wxPagedWindow::OnScroll()
//---------------------------------------------------------------------------

