/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/buttonstrip.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTONSTRIP_H_
#define _WX_BUTTONSTRIP_H_

/**
 @class wxButtonStrip
 
 @category{wxbileImplementation}
 */

class WXDLLEXPORT wxButtonStrip: public wxButtonStripBase
{
public:
    typedef enum {
        TabBar,
        ToolBar,
        SegmentedCtrl
    } ButtonStripType;
    
    typedef enum {
        BitmapButton,
        TextButton,
        TextAndBitmapButton
    } ButtonStripFace;
    
    typedef enum {
        NoSelection,
        SingleSelection
    } SelectionStyle;
    
    typedef enum {
        LayoutEvenButtons, // buttons have the same size
        LayoutEvenSeparators, // buttons have differing size, separation is the same
        LayoutStretchingSeparators // buttons have fixed size, but separators determine spacing
    } LayoutStyle;
    
    DECLARE_DYNAMIC_CLASS(wxButtonStrip)
public:
    
    /// Constructor.
    wxButtonStrip() { Init(); }
    virtual ~wxButtonStrip();
        
protected:
    
    void Init();    
};

#endif
    // _WX_BUTTONSTRIP_H_
