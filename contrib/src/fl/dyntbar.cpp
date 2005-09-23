/////////////////////////////////////////////////////////////////////////////
// Name:        dyntbar.cpp
// Purpose:     wxDynamicToolBar implementation
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     ??/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/utils.h"     // import wxMin,wxMax macros

#include "wx/fl/dyntbar.h"
#include "wx/fl/newbmpbtn.h"

IMPLEMENT_DYNAMIC_CLASS( wxDynamicToolBar, wxObject )

BEGIN_EVENT_TABLE( wxDynamicToolBar, wxToolBarBase )

    EVT_SIZE ( wxDynamicToolBar::OnSize  )
    EVT_PAINT( wxDynamicToolBar::OnPaint )
    //EVT_ERASE_BACKGROUND( wxDynamicToolBar::OnEraseBackground )

END_EVENT_TABLE()

/***** Implementation for class wxToolLayoutItem *****/

IMPLEMENT_DYNAMIC_CLASS(wxToolLayoutItem, wxObject)


/***** Implementation for class wxDynToolInfo *****/

IMPLEMENT_DYNAMIC_CLASS(wxDynToolInfo, wxToolLayoutItem)

/***** Implementation for class wxDynamicToolBar *****/

wxDynamicToolBar::wxDynamicToolBar()
    : mpLayoutMan( NULL ),
      mSepartorSize( 8 ),
      mVertGap ( 0 ),
      mHorizGap( 0 )
{
}

wxDynamicToolBar::wxDynamicToolBar(wxWindow *parent, const wxWindowID id,
                                   const wxPoint& pos, const wxSize& size,
                                   const long style, const int orientation,
                                   const int RowsOrColumns, const wxString& name )
    : mpLayoutMan( NULL ),
      mSepartorSize( 8 ),
      mVertGap ( 0 ),
      mHorizGap( 0 )
{
    Create(parent, id, pos, size, style, orientation, RowsOrColumns, name);

    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE) );
}

bool wxDynamicToolBar::Create(wxWindow *parent, const wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              const long style,
                              const int WXUNUSED(orientation), const int WXUNUSED(RowsOrColumns),
                              const wxString& name)
{
    // cut&pasted from wxtbatsmpl.h

    if ( ! wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ));

    return true;
}

bool wxDynamicToolBar::Realize(void)
{
    // FOR NOW:: nothing
    return true;
}

wxDynamicToolBar::~wxDynamicToolBar(void)
{
    if ( mpLayoutMan )
        delete mpLayoutMan;

    size_t i;
    for( i = 0; i != mTools.Count(); ++i )
    {
        delete mTools[i];
    }
}

void wxDynamicToolBar::AddTool( int toolIndex,
                                wxWindow* pToolWindow,
                                const wxSize& WXUNUSED(size)
                              )
{
    wxDynToolInfo* pInfo = new wxDynToolInfo();

    pInfo->mpToolWnd    = pToolWindow;
    pInfo->mIndex       = toolIndex;
    pInfo->mIsSeparator    = false;

    int x,y;
    pToolWindow->GetSize( &x, &y );
    pInfo->mRealSize.x = x;
    pInfo->mRealSize.y = y;
    pInfo->mRect.width = x;
    pInfo->mRect.height = y;

    mTools.Add( pInfo );
}

void wxDynamicToolBar::AddTool( int toolIndex,
                                const wxString& imageFileName,
                                wxBitmapType imageFileType,
                                const wxString& labelText, bool alignTextRight,
                                bool isFlat )
{
    wxNewBitmapButton* pBtn =

      new wxNewBitmapButton( imageFileName, imageFileType,
                             labelText,
                             ( alignTextRight )
                             ? NB_ALIGN_TEXT_RIGHT
                             : NB_ALIGN_TEXT_BOTTOM,
                             isFlat
                           );

    pBtn->Create( this, toolIndex );

    pBtn->Reshape();

    AddTool( toolIndex, pBtn );
}
void wxDynamicToolBar::AddTool( int toolIndex, wxBitmap labelBmp,
                                const wxString& labelText, bool alignTextRight,
                                bool isFlat )
{
    wxNewBitmapButton* pBtn =

      new wxNewBitmapButton( labelBmp,
                             labelText,
                             ( alignTextRight )
                             ? NB_ALIGN_TEXT_RIGHT
                             : NB_ALIGN_TEXT_BOTTOM,
                             isFlat
                           );

    pBtn->Create( this, toolIndex );

    pBtn->Reshape();

    AddTool( toolIndex, pBtn );
}


wxToolBarToolBase*
      wxDynamicToolBar::AddTool(const int toolIndex, const wxBitmap& bitmap,
                                const wxBitmap& WXUNUSED(pushedBitmap),
                                const bool WXUNUSED(toggle), const long WXUNUSED(xPos),
                                const long WXUNUSED(yPos), wxObject *WXUNUSED(clientData),
                                const wxString& helpString1, const wxString& WXUNUSED(helpString2))
{
    wxNewBitmapButton* pBmpBtn = new wxNewBitmapButton( bitmap );

    pBmpBtn->Create( this, toolIndex );

    pBmpBtn->Reshape();

#if wxUSE_TOOLTIPS
    pBmpBtn->SetToolTip( helpString1 );
#else
    wxUnusedVar( helpString1 );
#endif  // wxUSE_TOOLTIPS

    AddTool( toolIndex, pBmpBtn );

    return NULL;
}


wxDynToolInfo* wxDynamicToolBar::GetToolInfo( int toolIndex )
{
    size_t i;
    for( i = 0; i != mTools.Count(); ++i )
    {
        if ( mTools[i]->mIndex == toolIndex )
            return mTools[i];
    }

    return NULL;
}

void wxDynamicToolBar::RemveTool( int toolIndex )
{
    size_t i;
    for( i = 0; i != mTools.Count(); ++i )
    {
        if ( mTools[i]->mIndex == toolIndex )
        {
            if ( mTools[i]->mpToolWnd )
            {
                mTools[i]->mpToolWnd->Destroy();
            }
            delete mTools[i]; // HVL To be tested!!!
#if wxCHECK_VERSION(2,3,2)
            mTools.RemoveAt(i);
#else
            mTools.Remove(i);
#endif
            Layout();

            return;
        }
    }
    // TODO:: if not found, should it be an assertion?
}

void wxDynamicToolBar::AddSeparator( wxWindow* pSepartorWnd )
{
    wxDynToolInfo* pInfo = new wxDynToolInfo();

    pInfo->mpToolWnd    = pSepartorWnd;
    pInfo->mIndex       = -1;
    pInfo->mIsSeparator = true;

    // Do we draw a separator or is a other object?
    if ( pSepartorWnd )
    {
        // hvl => Is there a way to know if it was already created?
        // hvl => shouldn't the pSepartorWnd be created? (like one should expect?)
        // pSepartorWnd->Create( this, -1 );

        int x,y;
        pSepartorWnd->GetSize( &x, &y );
        pInfo->mRealSize.x = x;
        pInfo->mRealSize.y = y;

        pInfo->mRect.width = x;
        pInfo->mRect.height = y;
    }
    else
    {
        // Init x and y to the default.
        pInfo->mRealSize.x = 0;
        pInfo->mRealSize.y = 0;

        // Init height and width to the normal size of a separator.
        pInfo->mRect.width  = mSepartorSize;
        pInfo->mRect.height = mSepartorSize;
    }

    mTools.Add( pInfo );
}

void wxDynamicToolBar::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
    // FOR NOW:: nothing
}

void wxDynamicToolBar::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    //SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE ) );

    Layout();
}

void wxDynamicToolBar::DrawSeparator( wxDynToolInfo& info, wxDC& dc )
{
    // check the orientation of separator
    if ( info.mRect.width < info.mRect.height )
    {
        int midX = info.mRect.x + info.mRect.width/2 - 1;

        dc.SetPen( *wxGREY_PEN );
        dc.DrawLine( midX, info.mRect.y,
                     midX, info.mRect.y + info.mRect.height+1 );

        dc.SetPen( *wxWHITE_PEN );
        dc.DrawLine( midX+1, info.mRect.y,
                     midX+1, info.mRect.y + info.mRect.height+1 );
    }
    else
    {
        int midY = info.mRect.y + info.mRect.height/2 - 1;

        dc.SetPen( *wxGREY_PEN );
        dc.DrawLine( info.mRect.x, midY,
                     info.mRect.x + info.mRect.width+1, midY );

        dc.SetPen( *wxWHITE_PEN );
        dc.DrawLine( info.mRect.x, midY + 1,
                     info.mRect.x + info.mRect.width+1, midY + 1 );
    }
}

void wxDynamicToolBar::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // draw separators if any
    wxPaintDC dc(this);

    size_t i;
    for( i = 0; i != mTools.Count(); ++i )
    {
        if ( mTools[i]->mIsSeparator )
        {
            // check if separator doesn't have it's own window
            // if so, then draw it using built-in drawing method
            if ( !mTools[i]->mpToolWnd )
                DrawSeparator( *mTools[i], dc );
        }
    }
}

// FOR NOW:: quick fix
#include "wx/choice.h"

void wxDynamicToolBar::SizeToolWindows()
{
    bool bStateCheckDone = false;
    bool bHorzSeparator  = false;
    int maxWidth         = 0;
    int maxHeight        = 0;

    size_t i;
    for( i = 0; i != mTools.Count(); ++i )
    {
        wxDynToolInfo& info = *mTools[i];

        if ( !info.mIsSeparator )
        {
            // center real rectangle within the rectangle
            // provided by the layout manager

            int x = info.mRect.x;
            int y = info.mRect.y + (info.mRect.height - info.mRealSize.y)/2;

            // FOR NOW FOR NOW:: quick & dirty fix
            if ( info.mpToolWnd->IsKindOf( CLASSINFO( wxChoice ) ) )
            {
                info.mpToolWnd->SetSize( x, y,
                                         info.mRealSize.x - 3,
                                         info.mRealSize.y);
            }
            else
            {
                info.mpToolWnd->SetSize( x, y,
                                         info.mRealSize.x,
                                         info.mRealSize.y );
            }
        }
        else
        {
            // We performer this code here, so we only execute it when we have
            // separators and we do it only once (all to do with performance...)
            if (!bStateCheckDone)
            {
                bStateCheckDone = true;

                size_t j;
                wxDynToolInfo *pInfo;
                wxDynToolInfo *pPrevInfo = NULL;
                int nVertSeparators = 0;

                for( j = 0; j != mTools.Count(); ++j )
                {
                    pInfo = mTools[j];

                    // Count all Vert Separators.
                    if ( pInfo->mIsSeparator )
                        nVertSeparators++;

                    // Check if the new row starts with a Separator.
                    if ( pPrevInfo && pInfo->mIsSeparator &&
                         // pPrevInfo->mRect.x >= pInfo->mRect.x &&
                         pPrevInfo->mRect.y < pInfo->mRect.y)
                    {
                        // If the Separator is shown on the next row and it's
                        // the only object on the row it would mean that the
                        // Separator should be shown as Horizontal one.
                        if (j+1 != mTools.Count())
                        {
                            if (pInfo->mRect.y < mTools[j+1]->mRect.y)
                                nVertSeparators--;
                        }
                        else
                        {
                            nVertSeparators--;
                        }
                    }

                    pPrevInfo = pInfo;

                    maxWidth = wxMax(pInfo->mRect.width, maxWidth);
                    maxHeight = wxMax(pInfo->mRect.height, maxHeight);
                }

                bHorzSeparator = nVertSeparators == 0;
            }

            // Check if we should draw Horz or Vert...
            if ( !bHorzSeparator )
            {
                info.mRect.width = mSepartorSize;
                info.mRect.height = maxHeight;
            }
            else
            {
                info.mRect.width = maxWidth;
                info.mRect.height = mSepartorSize;
            }

            // Do we need to set a new size to a separator object?
            if ( info.mpToolWnd )
            {
                info.mpToolWnd->SetSize( info.mRect.x,
                                         info.mRect.y,
                                         info.mRect.width,
                                         info.mRect.height);
            }

        }
    }
}

bool wxDynamicToolBar::Layout()
{
    int x,y;
    GetSize( &x, &y );
    wxSize wndDim(x,y);
    wxSize result;
    size_t i;
    wxDynToolInfo *pInfo;

    // Reset the size of separators...
    for( i = 0; i != mTools.Count(); ++i )
    {
        pInfo = mTools[i];

        if ( pInfo->mIsSeparator )
        {
            pInfo->mRect.width  = mSepartorSize;
            pInfo->mRect.height = mSepartorSize;
        }
    }

    // Calc and set the best layout
    GetPreferredDim( wndDim, result );

    SizeToolWindows();
    return true;
}

void wxDynamicToolBar::GetPreferredDim( const wxSize& givenDim, wxSize& prefDim )
{
    if ( !mpLayoutMan )
        mpLayoutMan = CreateDefaultLayout();

    wxLayoutItemArrayT items;

    // safe conversion
    size_t i;
    for( i = 0; i != mTools.Count(); ++i )
        items.Add( mTools[i] );

    mpLayoutMan->Layout( givenDim, prefDim, items, mVertGap, mHorizGap );
}

void wxDynamicToolBar::SetLayout( LayoutManagerBase* pLayout )
{
    if ( mpLayoutMan )
        delete mpLayoutMan;

    mpLayoutMan = pLayout;

    Layout();
}

void wxDynamicToolBar::EnableTool(int toolIndex, bool enable )
{
    wxDynToolInfo* pInfo = GetToolInfo( toolIndex );

    if ( !pInfo )
        return;

    if ( pInfo->mIsSeparator || !pInfo->mpToolWnd )
        return;

    pInfo->mpToolWnd->Enable( enable );
}

/***** Implementation for class BagLayout *****/

void BagLayout::Layout(  const wxSize&       parentDim,
                         wxSize&             resultingDim,
                         wxLayoutItemArrayT& items,
                         int                 horizGap,
                         int                 vertGap
                      )
{
    int maxWidth = 0;
    int curY  = 0;
    int nRows = 0;

    size_t i = 0;

    while( i < items.Count() )
    {
        int curX   = 0;
        int height = 0;
        // int nItems = 0;

        // int firstItem  = i;
        int itemsInRow = 0;

        if ( nRows > 0 )
            curY += vertGap;

        // step #1 - arrange horizontal positions of items in the row

        do
        {
            if ( itemsInRow > 0 )
                curX += horizGap;

            wxRect& r = items[i]->mRect;

            if ( curX + r.width > parentDim.x )
            {
                if ( itemsInRow > 0 )
                    break;
            }
            r.x = curX;
            r.y = curY;

            curX += r.width;

            height = wxMax( height, r.height );

            ++itemsInRow;
            ++i;

        } while( i < items.Count() );

        curY += height;

        maxWidth = wxMax( maxWidth, curX );
    }

    resultingDim.x = maxWidth;
    resultingDim.y = curY;
}

//////// stuff from 2.1.15 ///////////

wxToolBarToolBase* wxDynamicToolBar::FindToolForPosition( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y) ) const
{
    return NULL;
}

bool wxDynamicToolBar::DoInsertTool( size_t WXUNUSED(pos), wxToolBarToolBase* WXUNUSED(tool) )
{
    return true;
}

bool wxDynamicToolBar::DoDeleteTool( size_t WXUNUSED(pos), wxToolBarToolBase* WXUNUSED(tool) )
{
    return true;
}

void wxDynamicToolBar::DoEnableTool( wxToolBarToolBase* WXUNUSED(tool), bool WXUNUSED(enable) )
{
}

void wxDynamicToolBar::DoToggleTool( wxToolBarToolBase* WXUNUSED(tool), bool WXUNUSED(toggle) )
{
}

void wxDynamicToolBar::DoSetToggle( wxToolBarToolBase* WXUNUSED(tool), bool WXUNUSED(toggle) )
{
}

wxToolBarToolBase* wxDynamicToolBar::CreateTool( int WXUNUSED(id),
                                                 const wxString& WXUNUSED(label),
                                                 const wxBitmap& WXUNUSED(bmpNormal),
                                                 const wxBitmap& WXUNUSED(bmpDisabled),
                                                 wxItemKind WXUNUSED(kind),
                                                 wxObject *WXUNUSED(clientData),
                                                 const wxString& WXUNUSED(shortHelp),
                                                 const wxString& WXUNUSED(longHelp)
                                                )
{
    return NULL;
}

wxToolBarToolBase* wxDynamicToolBar::CreateTool( wxControl* WXUNUSED(control) )
{
    return NULL;
}

