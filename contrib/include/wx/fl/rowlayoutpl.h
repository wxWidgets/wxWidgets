/////////////////////////////////////////////////////////////////////////////
// Name:        rowlayoutpl.h
// Purpose:     cbRowLayoutPlugin header.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     02/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ROWLAYOUTPL_G__
#define __ROWLAYOUTPL_G__

#include "wx/fl/controlbar.h"

/*
Simple implementation of a plugin which handles row layout
requests sent from a frame layout.
*/

class WXDLLIMPEXP_FL cbRowLayoutPlugin : public cbPluginBase
{
    DECLARE_DYNAMIC_CLASS( cbRowLayoutPlugin )
protected:
    cbDockPane* mpPane; // is set up temporarily, while handling event
protected:

        // Internal helper relating to not-fixed-bars layout.
    void FitBarsToRange( int from, int till, cbBarInfo* pTheBar, cbRowInfo* pRow ); 

        // Internal helper relating to not-fixed-bars layout.
    void RelayoutNotFixedBarsAround( cbBarInfo* pTheBar, cbRowInfo* pRow );

        // Internal helper relating to not-fixed-bars layout.
    void MinimzeNotFixedBars( cbRowInfo* pRow, cbBarInfo* pBarToPreserve );

        // Internal helper relating to not-fixed-bars layout.
    int  GetRowFreeSpace( cbRowInfo* pRow );


        // Internal helper relating to not-fixed-bars layout.
    void RecalcLengthRatios( cbRowInfo* pRow );

        // Internal helper relating to not-fixed-bars layout.
    void ApplyLengthRatios( cbRowInfo* pRow );

        // Internal helper relating to not-fixed-bars layout.
    void ExpandNotFixedBars( cbRowInfo* pRow );

        // Internal helper relating to not-fixed-bars layout.
    void AdjustLengthOfInserted( cbRowInfo* pRow, cbBarInfo* pTheBar );


        // Internal helper relating to not-fixed-bars layout.
    void DetectBarHandles( cbRowInfo* pRow );

        // Internal helper relating to not-fixed-bars layout.
    void CheckIfAtTheBoundary( cbBarInfo* pTheBar, cbRowInfo& rowInfo );


        // Row layout helper simulating bar 'friction'.
    int  CalcRowHeight( cbRowInfo& row );

        // Row layout helper simulating bar 'friction'.
    void LayoutItemsVertically( cbRowInfo& row );


        // Row layout helper simulating bar 'friction'.
    void StickRightSideBars( cbBarInfo* pToBar );


        // Row layout helper simulating bar 'friction'.
    void SlideLeftSideBars ( cbBarInfo* pTheBar );

        // Row layout helper simulating bar 'friction'.
    void SlideRightSideBars( cbBarInfo* pTheBar );


        // Row layout helper simulating bar 'friction'.
    void ShiftLeftTrashold ( cbBarInfo* pTheBar, cbRowInfo& row );

        // Row layout helper simulating bar 'friction'.
    void ShiftRightTrashold( cbBarInfo* pTheBar, cbRowInfo& row );


        // Insert the bar before the given row.
    void InsertBefore( cbBarInfo* pBeforeBar, 
                       cbBarInfo* pTheBar,
                       cbRowInfo& row
                     );

        // Insert the bar before the given row.
    void DoInsertBar( cbBarInfo* pTheBar, cbRowInfo& row );

public:
        // Default constructor.
    cbRowLayoutPlugin(void);

        // Constructor taking frame layout pane and pane mask.
    cbRowLayoutPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

        // Responds to row resize event.
    void OnResizeRow ( cbResizeRowEvent&  event );

        // Responds to bar insertion event.
    void OnInsertBar ( cbInsertBarEvent&  event );

        // Responds to bar removal event.
    void OnRemoveBar ( cbRemoveBarEvent&  event );

        // Responds to row layout event.
    void OnLayoutRow ( cbLayoutRowEvent&  event );

        // Responds to rows layout event.
    void OnLayoutRows( cbLayoutRowsEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif /* __ROWLAYOUTPL_G__ */

