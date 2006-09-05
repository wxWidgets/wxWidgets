/////////////////////////////////////////////////////////////////////////////
// Name:        animate.h
// Purpose:     Animation classes
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by:
// Created:     13/8/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANIMATEH__
#define _WX_ANIMATEH__

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/list.h>
#include <wx/timer.h>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/control.h>


#ifdef WXMAKINGDLL_ANIMATE
    #define WXDLLIMPEXP_ANIMATE WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_ANIMATE WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_ANIMATE
#endif

class WXDLLIMPEXP_ANIMATE wxAnimationBase;
class WXDLLIMPEXP_ANIMATE wxAnimationPlayer;
class WXDLLEXPORT wxImage;

enum wxAnimationDisposal
{
    wxANIM_UNSPECIFIED = -1,
    wxANIM_DONOTREMOVE = 0,
    wxANIM_TOBACKGROUND = 1,
    wxANIM_TOPREVIOUS = 2
};

class WXDLLIMPEXP_ANIMATE wxAnimationTimer: public wxTimer
{
public:
    wxAnimationTimer()
    { m_player = (wxAnimationPlayer*) NULL; }

    virtual void Notify();

    void SetPlayer(wxAnimationPlayer* player)
    { m_player = player; }

protected:
    wxAnimationPlayer*  m_player;
};

/* wxAnimationPlayer
 * Create an object of this class, and either pass an wxXXXAnimation object in
 * the constructor, or call SetAnimation. Then call Play().  The wxAnimation
 * object is only destroyed in the destructor if destroyAnimation is TRUE in
 * the constructor.
 */

class WXDLLIMPEXP_ANIMATE wxAnimationPlayer : public wxObject
{
    DECLARE_CLASS(wxAnimationPlayer)

public:
    wxAnimationPlayer(wxAnimationBase *animation = (wxAnimationBase *) NULL, bool destroyAnimation = false);
    virtual ~wxAnimationPlayer();

//// Accessors

    void SetAnimation(wxAnimationBase* animation, bool destroyAnimation = false);
    wxAnimationBase* GetAnimation() const
    { return m_animation; }

    void SetDestroyAnimation(bool destroyAnimation)
    { m_destroyAnimation = destroyAnimation; }

    bool GetDestroyAnimation() const
    { return m_destroyAnimation; }

    void SetCurrentFrame(int currentFrame)
    { m_currentFrame = currentFrame; }

    int GetCurrentFrame() const
    { return m_currentFrame; }

    void SetWindow(wxWindow* window)
    { m_window = window; }

    wxWindow* GetWindow() const
    { return m_window; }

    void SetPosition(const wxPoint& pos)
    { m_position = pos; }

    wxPoint GetPosition() const
    { return m_position; }

    void SetLooped(bool looped)
    { m_looped = looped; }

    bool GetLooped() const
    { return m_looped; }

    bool HasAnimation() const
    { return (m_animation != (wxAnimationBase*) NULL); }

    bool IsPlaying() const
    { return m_isPlaying; }

    // Specify whether the GIF's background colour is to be shown,
    // or whether the window background should show through (the default)
    void UseBackgroundColour(bool useBackground)
    { m_useBackgroundColour = useBackground; }

    bool UsingBackgroundColour() const
    { return m_useBackgroundColour; }

    // Set and use a user-specified background colour (valid for transparent
    // animations only)
    void SetCustomBackgroundColour(const wxColour& col, bool useCustomBackgroundColour = true)
    {
        m_customBackgroundColour = col;
        m_useCustomBackgroundColour = useCustomBackgroundColour;
    }

    bool UsingCustomBackgroundColour() const
    { return m_useCustomBackgroundColour; }

    const wxColour& GetCustomBackgroundColour() const
    { return m_customBackgroundColour; }

    // Another refinement - suppose we're drawing the animation in a separate
    // control or window. We may wish to use the background of the parent
    // window as the background of our animation. This allows us to specify
    // whether to grab from the parent or from this window.
    void UseParentBackground(bool useParent)
    { m_useParentBackground = useParent; }

    bool UsingParentBackground() const
    { return m_useParentBackground; }

//// Operations

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

//// Accessing the current animation

    virtual int GetFrameCount() const;
    virtual wxImage* GetFrame(int i) const; // Creates a new wxImage
    virtual wxAnimationDisposal GetDisposalMethod(int i) const;
    virtual wxRect GetFrameRect(int i) const; // Position and size of frame
    virtual int GetDelay(int i) const; // Delay for this frame

    virtual wxSize GetLogicalScreenSize() const;
    virtual bool GetBackgroundColour(wxColour& col) const;
    virtual bool GetTransparentColour(wxColour& col) const;

//// Implementation

    // Play the frame
    virtual bool PlayFrame(int frame, wxWindow& window, const wxPoint& pos);
    virtual bool PlayFrame();
    virtual void DrawFrame(int frame, wxDC& dc, const wxPoint& pos);
    virtual void DrawBackground(wxDC& dc, const wxPoint& pos, const wxColour& colour);

    // Clear the wxImage cache
    virtual void ClearCache();

    // Save the pertinent area of the window so we can restore
    // it if drawing transparently
    void SaveBackground(const wxRect& rect);

    wxBitmap& GetBackingStore()
    { return m_backingStore; }

//// Data members
protected:
    wxAnimationBase*    m_animation;
    bool                m_destroyAnimation; // Destroy m_animation on deletion of this object
    wxList              m_frames;           // List of cached wxBitmap frames.
    int                 m_currentFrame;     // Current frame
    wxWindow*           m_window;           // Window to draw into
    wxPoint             m_position;         // Position to draw at
    bool                m_looped;           // Looped, or not
    wxAnimationTimer    m_timer;            // The timer
    bool                m_isPlaying;        // Is the animation playing?
    wxBitmap            m_savedBackground;  // Saved background of window portion
    wxBitmap            m_backingStore;     // The player draws into this
    bool                m_useBackgroundColour; // Use colour or background
    wxColour            m_customBackgroundColour; // Override animation background
    bool                m_useCustomBackgroundColour;
    bool                m_useParentBackground; // Grab background from parent?
};

/* wxAnimationBase
 * Base class for animations.
 * A wxXXXAnimation only stores the animation, providing accessors to
 * wxAnimationPlayer.  Currently an animation is read-only, but we could
 * extend the API for adding frames programmatically, and perhaps have a
 * wxMemoryAnimation class that stores its frames in memory, and is able to
 * save all files with suitable filenames. You could then use e.g. Ulead GIF
 * Animator to load the image files into a GIF animation.
 */

class WXDLLIMPEXP_ANIMATE wxAnimationBase : public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxAnimationBase)

public:
    wxAnimationBase() {}
    virtual ~wxAnimationBase() {}

//// Accessors. Should be overridden by each derived class.

    virtual int GetFrameCount() const = 0;
    virtual wxImage* GetFrame(int i) const = 0; // Creates a new wxImage
    virtual wxAnimationDisposal GetDisposalMethod(int i) const = 0;
    virtual wxRect GetFrameRect(int i) const = 0; // Position and size of frame
    virtual int GetDelay(int i) const = 0; // Delay for this frame

    virtual wxSize GetLogicalScreenSize() const = 0;
    virtual bool GetBackgroundColour(wxColour& col) const = 0;
    virtual bool GetTransparentColour(wxColour& col) const = 0;

    // Is the animation OK?
    virtual bool IsValid() const = 0;

//// Operations

    virtual bool LoadFile(const wxString& WXUNUSED(filename))
    { return false; }
};

/* wxGIFAnimation
 * This will be moved to a separate file in due course.
 */

class WXDLLIMPEXP_ANIMATE wxGIFDecoder;

class WXDLLIMPEXP_ANIMATE wxGIFAnimation : public wxAnimationBase
{
    DECLARE_CLASS(wxGIFAnimation)

public:
    wxGIFAnimation();
    virtual ~wxGIFAnimation();

//// Accessors

    virtual int GetFrameCount() const;
    virtual wxImage* GetFrame(int i) const;
    virtual wxAnimationDisposal GetDisposalMethod(int i) const;
    virtual wxRect GetFrameRect(int i) const; // Position and size of frame
    virtual int GetDelay(int i) const; // Delay for this frame

    virtual wxSize GetLogicalScreenSize() const;
    virtual bool GetBackgroundColour(wxColour& col) const;
    virtual bool GetTransparentColour(wxColour& col) const;

    virtual bool IsValid() const;

//// Operations

    virtual bool LoadFile(const wxString& filename);

protected:
    wxGIFDecoder*   m_decoder;
};

/*
 * wxAnimationCtrlBase
 * Abstract base class for format-specific animation controls.
 * This class implements most of the functionality; all a derived
 * class has to do is create the appropriate animation class on demand.
 */

// Resize to animation size if this is set
#define wxAN_FIT_ANIMATION       0x0010

class WXDLLIMPEXP_ANIMATE wxAnimationCtrlBase: public wxControl
{
public:
    wxAnimationCtrlBase() {}
    wxAnimationCtrlBase(wxWindow *parent, wxWindowID id,
            const wxString& filename = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxAN_FIT_ANIMATION|wxNO_BORDER,
            const wxString& name = wxT("animationControl"))
    {
        Create(parent, id, filename, pos, size, style, name);
    }
    virtual ~wxAnimationCtrlBase();

    bool Create(wxWindow *parent, wxWindowID id,
            const wxString& filename = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxAN_FIT_ANIMATION | wxNO_BORDER,
            const wxString& name = wxT("animationControl"));

    //// Operations
    virtual bool LoadFile(const wxString& filename = wxEmptyString);
    virtual bool Play(bool looped = true);
    virtual void Stop()
    { m_animationPlayer.Stop(); }

    virtual void FitToAnimation();

    //// Accessors
    virtual bool IsPlaying() const
    { return m_animationPlayer.IsPlaying(); }

    virtual wxAnimationPlayer& GetPlayer()
    { return m_animationPlayer; }

    virtual wxAnimationBase* GetAnimation()
    { return m_animation; }

    const wxString& GetFilename() const
    { return m_filename; }

    void SetFilename(const wxString& filename)
    { m_filename = filename; }

    //// Event handlers
    void OnPaint(wxPaintEvent& event);

protected:
    virtual wxSize DoGetBestSize() const;

    // Override this in derived classes
    virtual wxAnimationBase* DoCreateAnimation(const wxString& filename) = 0;

    wxAnimationPlayer   m_animationPlayer;
    wxAnimationBase*    m_animation;
    wxString            m_filename;

private:
    DECLARE_ABSTRACT_CLASS(wxAnimationCtrlBase)
    DECLARE_EVENT_TABLE()
};

/*
 * wxGIFAnimationCtrl
 * Provides a GIF animation class when required.
 */

class WXDLLIMPEXP_ANIMATE wxGIFAnimationCtrl: public wxAnimationCtrlBase
{
public:
    wxGIFAnimationCtrl() {}
    wxGIFAnimationCtrl(wxWindow *parent,
            wxWindowID id,
            const wxString& filename = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxAN_FIT_ANIMATION | wxNO_BORDER,
            const wxString& name = wxT("animationControl"))
    {
        Create(parent, id, filename, pos, size, style, name);
    }

protected:
    virtual wxAnimationBase* DoCreateAnimation(const wxString& filename);

private:
    DECLARE_DYNAMIC_CLASS(wxGIFAnimationCtrl)
};

#endif // _WX_ANIMATEH__
