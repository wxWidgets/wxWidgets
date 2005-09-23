/////////////////////////////////////////////////////////////////////////////
// Name:        hintanimpl.h
// Purpose:     Header for cbHintAnimationPlugin class.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     9/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __HINTANIMPL_G__
#define __HINTANIMPL_G__

#include "wx/fl/controlbar.h"

#include "wx/timer.h"

class WXDLLIMPEXP_FL cbHintAnimTimer;

/*
A plugin to draw animated hints when the user drags a pane.
*/

class WXDLLIMPEXP_FL cbHintAnimationPlugin : public cbPluginBase
{
    DECLARE_DYNAMIC_CLASS( cbHintAnimationPlugin )
protected:
    friend class cbHintAnimTimer;

    wxScreenDC*      mpScrDc;     // created while tracking hint-rect
    cbHintAnimTimer* mpAnimTimer;

    // FOR NOW:: try it without mutually exculisve locks
    volatile wxRect  mCurRect;

    // state variables

    bool   mAnimStarted;
    bool   mStopPending;

    bool   mPrevInClient;
    bool   mCurInClient;

    wxRect mPrevRect;

public:
    int    mMorphDelay;         // delay between frames in miliseconds,     default: 20
    int    mMaxFrames;          // number  of iterations for hint morphing, default: 30
                                // (morph duration = mMorphDelay * mMaxFrames msec)

    int    mInClientHintBorder; // default: 4 pixels

    bool   mAccelerationOn;     // true, if morph accelerates, otherwise morph
                                // speed is constant. Default: true

    // TBD:: get/set methods for above members

protected:

        // Internal function for starting tracking.
    void StartTracking();

        // Internal function for drawing a hint rectangle.
    void DrawHintRect ( wxRect& rect, bool isInClientRect);

        // Internal function for erasing a hint rectangle.
    void EraseHintRect( wxRect& rect, bool isInClientRect);

        // Internal function for finishing tracking.
    void FinishTracking();

        // Internal function for drawing a hint rectangle.
    void DoDrawHintRect( wxRect& rect, bool isInClientRect);

        // Internal function for translating coordinates.
    void RectToScr( wxRect& frameRect, wxRect& scrRect );

public:
        // Default constructor.
    cbHintAnimationPlugin();

        // Constructor, taking a layout panel and pane mask.
    cbHintAnimationPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

        // Destructor.
    ~cbHintAnimationPlugin();

        // Event handler respoding to hint draw events.
    void OnDrawHintRect( cbDrawHintRectEvent& event );

    DECLARE_EVENT_TABLE()
};


/*
A private helper class.
*/

struct WXDLLIMPEXP_FL MorphInfoT
{
    wxPoint mFrom;
    wxPoint mTill;
};

/*
A private helper class.
*/

class WXDLLIMPEXP_FL cbHintAnimTimer : public wxTimer 
{
protected:

    friend class cbHintAnimationPlugin;

    wxRect      mPrevMorphed;

    MorphInfoT  mUpperLeft;
    MorphInfoT  mLowerRight;
    int         mCurIter;

    long        mLock;

    cbHintAnimationPlugin* mpPl;

    void MorphPoint( wxPoint& origin, MorphInfoT& info, wxPoint& point );

public: 

    cbHintAnimTimer(void);

    virtual void Notify(void);

    virtual bool Init( cbHintAnimationPlugin* pAnimPl, bool reinit );
};

#endif /* __HINTANIMPL_G__ */

