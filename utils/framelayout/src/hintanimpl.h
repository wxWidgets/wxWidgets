/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     9/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __HINTANIMPL_G__
#define __HINTANIMPL_G__

#include "controlbar.h"

#include "wx/timer.h"

class cbHintAnimTimer;

class cbHintAnimationPlugin : public cbPluginBase
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

	bool   mAccelerationOn;     // TRUE, if morph accelerates, otherwise morph
								// speed is constant. Default: TRUE

	// TBD:: get/set methods for above members

protected:
	void StartTracking();

	void DrawHintRect ( wxRect& rect, bool isInClientRect);
	void EraseHintRect( wxRect& rect, bool isInClientRect);

	void FinishTracking();

	void DoDrawHintRect( wxRect& rect, bool isInClientRect);

	void RectToScr( wxRect& frameRect, wxRect& scrRect );

public:
	cbHintAnimationPlugin(void);

	~cbHintAnimationPlugin();

	cbHintAnimationPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

	void OnDrawHintRect( cbDrawHintRectEvent& event );        

	DECLARE_EVENT_TABLE()
};


// helper classes

struct MorphInfoT
{
	wxPoint mFrom;
	wxPoint mTill;
};

class cbHintAnimTimer : public wxTimer 
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

#endif