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
#include <wx/animate/animate.h>
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { import wx }
%pythoncode { __docfilter__ = wx._core.__DocFilter(globals()) }


MAKE_CONST_WXSTRING2(AnimationControlNameStr,     wxT("animationControl"));
MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

//---------------------------------------------------------------------------


enum wxAnimationDisposal
{
    wxANIM_UNSPECIFIED = -1,
    wxANIM_DONOTREMOVE = 0,
    wxANIM_TOBACKGROUND = 1,
    wxANIM_TOPREVIOUS = 2
} ;


/* wxAnimationPlayer
 * Create an object of this class, and either pass an wxXXXAnimation object in
 * the constructor, or call SetAnimation. Then call Play().  The wxAnimation
 * object is only destroyed in the destructor if destroyAnimation is true in
 * the constructor.
 */

class wxAnimationPlayer : public wxObject
{
public:
    wxAnimationPlayer(wxAnimationBase *animation = NULL,
                      bool destroyAnimation = false);
    ~wxAnimationPlayer();


    void SetAnimation(wxAnimationBase* animation, bool destroyAnimation = false);
    wxAnimationBase* GetAnimation() const;

    void SetDestroyAnimation(bool destroyAnimation);
    bool GetDestroyAnimation() const;

    void SetCurrentFrame(int currentFrame);
    int GetCurrentFrame() const;

    void SetWindow(wxWindow* window);
    wxWindow* GetWindow() const;

    void SetPosition(const wxPoint& pos);
    wxPoint GetPosition() const;

    void SetLooped(bool looped);
    bool GetLooped() const;

    bool HasAnimation() const;

    bool IsPlaying() const;

    // Specify whether the GIF's background colour is to be shown,
    // or whether the window background should show through (the default)
    void UseBackgroundColour(bool useBackground);
    bool UsingBackgroundColour() const;

    // Set and use a user-specified background colour (valid for transparent
    // animations only)
    void SetCustomBackgroundColour(const wxColour& col,
                                   bool useCustomBackgroundColour = true);

    bool UsingCustomBackgroundColour() const;
    const wxColour& GetCustomBackgroundColour() const;

    // Another refinement - suppose we're drawing the animation in a separate
    // control or window. We may wish to use the background of the parent
    // window as the background of our animation. This allows us to specify
    // whether to grab from the parent or from this window.
    void UseParentBackground(bool useParent);
    bool UsingParentBackground() const;


    // Play
    virtual bool Play(wxWindow& window, const wxPoint& pos = wxPoint(0, 0), bool looped = true);

    // Build animation (list of wxImages). If not called before Play
    // is called, Play will call this automatically.
    virtual bool Build();

    // Stop the animation
    virtual void Stop();

    // Draw the current view of the animation into this DC.
    // Call this from your OnPaint, for example.
    virtual void Draw(wxDC& dc);


    virtual int GetFrameCount() const;
    %newobject GetFrame;
    virtual wxImage* GetFrame(int i) const; // Creates a new wxImage
    virtual wxAnimationDisposal GetDisposalMethod(int i) const;
    virtual wxRect GetFrameRect(int i) const; // Position and size of frame
    virtual int GetDelay(int i) const; // Delay for this frame

    virtual wxSize GetLogicalScreenSize() const;
    virtual bool GetBackgroundColour(wxColour& col) const ;
    virtual bool GetTransparentColour(wxColour& col) const ;


    // Play the frame
    virtual bool PlayFrame(int frame, wxWindow& window, const wxPoint& pos);
    %Rename(PlayNextFrame,
            virtual bool, PlayFrame());
    
    virtual void DrawFrame(int frame, wxDC& dc, const wxPoint& pos);
    virtual void DrawBackground(wxDC& dc, const wxPoint& pos, const wxColour& colour);

    // Clear the wxImage cache
    virtual void ClearCache();

    // Save the pertinent area of the window so we can restore
    // it if drawing transparently
    void SaveBackground(const wxRect& rect);

    wxBitmap& GetBackingStore();

    %property(Animation, GetAnimation, SetAnimation, doc="See `GetAnimation` and `SetAnimation`");
    %property(BackingStore, GetBackingStore, doc="See `GetBackingStore`");
    %property(CurrentFrame, GetCurrentFrame, SetCurrentFrame, doc="See `GetCurrentFrame` and `SetCurrentFrame`");
    %property(CustomBackgroundColour, GetCustomBackgroundColour, SetCustomBackgroundColour, doc="See `GetCustomBackgroundColour` and `SetCustomBackgroundColour`");
    %property(DestroyAnimation, GetDestroyAnimation, SetDestroyAnimation, doc="See `GetDestroyAnimation` and `SetDestroyAnimation`");
    %property(DisposalMethod, GetDisposalMethod, doc="See `GetDisposalMethod`");
    %property(FrameCount, GetFrameCount, doc="See `GetFrameCount`");
    %property(LogicalScreenSize, GetLogicalScreenSize, doc="See `GetLogicalScreenSize`");
    %property(Looped, GetLooped, SetLooped, doc="See `GetLooped` and `SetLooped`");
    %property(Position, GetPosition, SetPosition, doc="See `GetPosition` and `SetPosition`");
    %property(Window, GetWindow, SetWindow, doc="See `GetWindow` and `SetWindow`");
};


//---------------------------------------------------------------------------

/* wxAnimationBase
 * Base class for animations.
 * A wxXXXAnimation only stores the animation, providing accessors to
 * wxAnimationPlayer.  Currently an animation is read-only, but we could
 * extend the API for adding frames programmatically, and perhaps have a
 * wxMemoryAnimation class that stores its frames in memory, and is able to
 * save all files with suitable filenames. You could then use e.g. Ulead GIF
 * Animator to load the image files into a GIF animation.
 */

class wxAnimationBase : public wxObject
{
public:
    //wxAnimationBase() {};  // It's an ABC
    ~wxAnimationBase() {};

//// Accessors. Should be overridden by each derived class.

    virtual int GetFrameCount() const;
    %newobject GetFrame;
    virtual wxImage* GetFrame(int i) const;
    virtual wxAnimationDisposal GetDisposalMethod(int i) const;
    virtual wxRect GetFrameRect(int i) const; // Position and size of frame
    virtual int GetDelay(int i) const; // Delay for this frame

    virtual wxSize GetLogicalScreenSize() const;
    virtual bool GetBackgroundColour(wxColour& col) const;
    virtual bool GetTransparentColour(wxColour& col) const;

    // Is the animation OK?
    virtual bool IsValid() const;

    virtual bool LoadFile(const wxString& filename);

    %property(DisposalMethod, GetDisposalMethod, doc="See `GetDisposalMethod`");
    %property(FrameCount, GetFrameCount, doc="See `GetFrameCount`");
    %property(LogicalScreenSize, GetLogicalScreenSize, doc="See `GetLogicalScreenSize`");
};


// TODO:  Add a wxPyAnimationBase class

//---------------------------------------------------------------------------

/* wxGIFAnimation
 * This will be moved to a separate file in due course.
 */
class wxGIFAnimation : public wxAnimationBase
{
public:
    wxGIFAnimation() ;
    ~wxGIFAnimation() ;

//// Accessors

    virtual int GetFrameCount() const;
    %newobject GetFrame;
    virtual wxImage* GetFrame(int i) const;
    virtual wxAnimationDisposal GetDisposalMethod(int i) const;
    virtual wxRect GetFrameRect(int i) const; // Position and size of frame
    virtual int GetDelay(int i) const; // Delay for this frame

    virtual wxSize GetLogicalScreenSize() const ;
    virtual bool GetBackgroundColour(wxColour& col) const ;
    virtual bool GetTransparentColour(wxColour& col) const ;

    virtual bool IsValid() const;

//// Operations

    virtual bool LoadFile(const wxString& filename);

};


//---------------------------------------------------------------------------

/*
 * wxAnimationCtrlBase
 * Abstract base class for format-specific animation controls.
 * This class implements most of the functionality; all a derived
 * class has to do is create the appropriate animation class on demand.
 */

// Resize to animation size if this is set
enum { wxAN_FIT_ANIMATION };


// class  wxAnimationCtrlBase: public wxControl
// {
// public:
//     %pythonAppend wxAnimationCtrlBase         "self._setOORInfo(self)"
//     %pythonAppend wxAnimationCtrlBase()       ""

//     wxAnimationCtrlBase(wxWindow *parent, wxWindowID id,
//                         const wxString& filename = wxPyEmptyString,
//                         const wxPoint& pos = wxDefaultPosition,
//                         const wxSize& size = wxDefaultSize,
//                         long style = wxAN_FIT_ANIMATION|wxNO_BORDER,
//                         const wxString& name = wxPyAnimationControlNameStr);
//     %RenameCtor(PreAnimationCtrlBase, wxAnimationCtrlBase());

//     bool Create(wxWindow *parent, wxWindowID id,
//                 const wxString& filename = wxPyEmptyString,
//                 const wxPoint& pos = wxDefaultPosition,
//                 const wxSize& size = wxDefaultSize,
//                 long style = wxAN_FIT_ANIMATION|wxNO_BORDER,
//                 const wxString& name = wxPyAnimationControlNameStr);

//     //// Operations
//     virtual bool LoadFile(const wxString& filename = wxPyEmptyString);
//     virtual bool Play(bool looped = true) ;
//     virtual void Stop();
//     virtual void FitToAnimation();

//     //// Accessors
//     virtual bool IsPlaying() const;
//     virtual wxAnimationPlayer& GetPlayer();
//     virtual wxAnimationBase* GetAnimation();

//     const wxString& GetFilename() const;
//     void SetFilename(const wxString& filename);

// };


/*
 * wxGIFAnimationCtrl
 * Provides a GIF animation class when required.
 */

MustHaveApp(wxGIFAnimationCtrl);
class wxGIFAnimationCtrl: public wxControl  //wxAnimationCtrlBase
{
public:
    %pythonAppend wxGIFAnimationCtrl         "self._setOORInfo(self)"
    %pythonAppend wxGIFAnimationCtrl()       ""

    wxGIFAnimationCtrl(wxWindow *parent, wxWindowID id=-1,
                       const wxString& filename = wxPyEmptyString,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxAN_FIT_ANIMATION|wxNO_BORDER,
                       const wxString& name = wxPyAnimationControlNameStr);
    %RenameCtor(PreGIFAnimationCtrl, wxGIFAnimationCtrl());

    bool Create(wxWindow *parent, wxWindowID id=-1,
                const wxString& filename = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAN_FIT_ANIMATION|wxNO_BORDER,
                const wxString& name = wxPyAnimationControlNameStr);

    //// Operations
    virtual bool LoadFile(const wxString& filename = wxPyEmptyString);
    virtual bool Play(bool looped = true) ;
    virtual void Stop();
    virtual void FitToAnimation();

    //// Accessors
    virtual bool IsPlaying() const;
    virtual wxAnimationPlayer& GetPlayer();
    virtual wxAnimationBase* GetAnimation();

    const wxString& GetFilename() const;
    void SetFilename(const wxString& filename);
    
    %property(Animation, GetAnimation, doc="See `GetAnimation`");
    %property(Filename, GetFilename, SetFilename, doc="See `GetFilename` and `SetFilename`");
    %property(Player, GetPlayer, doc="See `GetPlayer`");
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

