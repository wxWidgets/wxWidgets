/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/scrolwin.h
// Purpose:     wxScrolledWindow class
// Author:      Robert Roebling
// Modified by: Vadim Zeitlin (2005-10-10): wxScrolledWindow is now common
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_SCROLLWIN_H_
#define _WX_GTK_SCROLLWIN_H_

// ----------------------------------------------------------------------------
// wxScrolledWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrollHelperNative : public wxScrollHelper
{
public:
    // default ctor doesn't do anything
    wxScrollHelperNative(wxWindow *win) : wxScrollHelper(win) { }

    virtual void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                               int noUnitsX, int noUnitsY,
                               int xPos = 0, int yPos = 0,
                               bool noRefresh = false);
    virtual void AdjustScrollbars();
    virtual void Scroll(int x, int y);

protected:
    // this does (each) half of AdjustScrollbars() work
    void DoAdjustScrollbar(GtkAdjustment *adj,
                           int pixelsPerLine,
                           int winSize,
                           int virtSize,
                           int *pos,
                           int *lines,
                           int *linesPerPage);

    // and this does the same for Scroll()
    void DoScroll(int orient,
                  GtkAdjustment *adj,
                  int pos,
                  int pixelsPerLine,
                  int *posOld);

private:
    DECLARE_NO_COPY_CLASS(wxScrollHelperNative)
};

#endif // _WX_GTK_SCROLLWIN_H_

