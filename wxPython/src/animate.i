/////////////////////////////////////////////////////////////////////////////
// Name:        animate.i
// Purpose:     Wrappers for the animation classes in wx/contrib
//
// Author:      Robin Dunn
//
// Created:     4-April-2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"Simple animation player classes, including `GIFAnimationCtrl` for displaying
animated GIF files
"
%enddef

%module(package="wx", docstring=DOCSTRING) animate


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/pyistream.h"    
#include <wx/animate.h>
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { import wx }
%pythoncode { __docfilter__ = wx._core.__DocFilter(globals()) }


MAKE_CONST_WXSTRING(AnimationCtrlNameStr);
MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

//---------------------------------------------------------------------------

enum wxAnimationDisposal
{
    // No disposal specified. The decoder is not required to take any action.
    wxANIM_UNSPECIFIED = -1,

    // Do not dispose. The graphic is to be left in place.
    wxANIM_DONOTREMOVE = 0,

    // Restore to background color. The area used by the graphic must be 
    // restored to the background color.
    wxANIM_TOBACKGROUND = 1,

    // Restore to previous. The decoder is required to restore the area 
    // overwritten by the graphic with what was there prior to rendering the graphic.
    wxANIM_TOPREVIOUS = 2
};

enum wxAnimationType
{
    wxANIMATION_TYPE_INVALID,
    wxANIMATION_TYPE_GIF,
    wxANIMATION_TYPE_ANI,

    wxANIMATION_TYPE_ANY
};


//---------------------------------------------------------------------------


class wxAnimationBase : public wxObject
{
public:
    //wxAnimationBase() {};  // It's an ABC
    ~wxAnimationBase() {};

    virtual bool IsOk() const;

    // can be -1
    virtual int GetDelay(size_t i) const;

    virtual size_t GetFrameCount() const;

    %newobject GetFrame;
    virtual wxImage GetFrame(size_t i) const;
    virtual wxSize GetSize() const;

    virtual bool LoadFile(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY);
    virtual bool Load(wxInputStream &stream, wxAnimationType type = wxANIMATION_TYPE_ANY);

};




class wxAnimation : public wxAnimationBase
{
public:
    %nokwargs wxAnimation;
    wxAnimation();
    %extend {
        wxAnimation(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY) {
            wxAnimation* ani = new wxAnimation();
            ani->LoadFile(name, type);
            return ani;
        }
    }

    ~wxAnimation();


public:     // extended interface used by the generic implementation of wxAnimationCtrl

#ifndef __WXGTK__
    wxPoint GetFramePosition(size_t frame) const;
    wxAnimationDisposal GetDisposalMethod(size_t frame) const;
    wxColour GetBackgroundColour() const;
#else
    %extend {
        wxPoint GetFramePosition(size_t frame) const { return wxDefaultPosition; }
        wxAnimationDisposal GetDisposalMethod(size_t frame) const { return wxANIM_UNSPECIFIED; }
        wxColour GetBackgroundColour() const { return wxNullColour; }
    }
#endif

// public:
//     static inline wxAnimationDecoderList& GetHandlers() { return sm_handlers; }
//     static void AddHandler(wxAnimationDecoder *handler);
//     static void InsertHandler(wxAnimationDecoder *handler);
//     static const wxAnimationDecoder *FindHandler( wxAnimationType animType );
//     static void CleanUpHandlers();
//     static void InitStandardHandlers();
};



%immutable;
%threadWrapperOff;

const wxAnimation wxNullAnimation;

%threadWrapperOn;
%mutable;



%{// for backwards compatibility
#ifndef wxAN_FIT_ANIMATION
#define wxAN_FIT_ANIMATION 0x0010
#endif
%}


enum {
    wxAC_NO_AUTORESIZE,
    wxAC_DEFAULT_STYLE,
    
    wxAN_FIT_ANIMATION
};


class wxAnimationCtrlBase : public wxControl
{
public:
    // wxAnimationCtrlBase() {}   *** It's an ABC

    virtual bool LoadFile(const wxString& filename, wxAnimationType type = wxANIMATION_TYPE_ANY);

    virtual void SetAnimation(const wxAnimation &anim);
    virtual wxAnimation GetAnimation() const;
    %property(Animation, GetAnimation, SetAnimation);

    virtual bool Play();
    virtual void Stop();

    virtual bool IsPlaying() const;
};



MustHaveApp(wxAnimationCtrl);

class wxAnimationCtrl: public wxAnimationCtrlBase
{
public:
    %pythonAppend wxAnimationCtrl         "self._setOORInfo(self)"
    %pythonAppend wxAnimationCtrl()       ""

    wxAnimationCtrl(wxWindow *parent,
                    wxWindowID id=-1,
                    const wxAnimation& anim = wxNullAnimation,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxAC_DEFAULT_STYLE,
                    const wxString& name = wxPyAnimationCtrlNameStr);
    
    %RenameCtor(PreAnimationCtrl, wxAnimationCtrl());

    
    bool Create(wxWindow *parent, wxWindowID id,
                const wxAnimation& anim = wxNullAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxPyAnimationCtrlNameStr);




public:     // extended API specific to this implementation of wxAnimateCtrl

#ifndef __WXGTK__
    // Specify whether the animation's background colour is to be shown (the default),
    // or whether the window background should show through
    void SetUseWindowBackgroundColour(bool useWinBackground = true);
    bool IsUsingWindowBackgroundColour() const;

//     // This overload of Play() lets you specify if the animation must loop or not
//     bool Play(bool looped);

    // Draw the current frame of the animation into given DC.
    // This is fast as current frame is always cached.
    void DrawCurrentFrame(wxDC& dc);

    // Returns a wxBitmap with the current frame drawn in it
    wxBitmap& GetBackingStore();
#else
    %extend {
        void SetUseWindowBackgroundColour(bool useWinBackground = true) {}
        bool IsUsingWindowBackgroundColour() const { return false; }
        void DrawCurrentFrame(wxDC& dc) {}
        wxBitmap& GetBackingStore() { return wxNullBitmap; }
    }
#endif
    
};



%pythoncode {

    class GIFAnimationCtrl(AnimationCtrl):
        """
        Backwards compatibility class for AnimationCtrl.
        """
        def __init__(self, parent, id=-1, filename="",
                     pos=wx.DefaultPosition, size=wx.DefaultSize,
                     style=AC_DEFAULT_STYLE,
                     name="gifAnimation"):
            AnimationCtrl.__init__(self, parent, id, NullAnimation, pos, size, style, name)
            self.LoadFile(filename)

        def GetPlayer(self):
            return self

        def UseBackgroundColour(self, useBackground=True):
            self.SetUseWindowBackgroundColour(useBackground)
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

