///////////////////////////////////////////////////////////////////////////////
// Name:        animate.cpp
// Purpose:     Implementation of wxAnimation classes
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by:
// Created:     13/8/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "animate.h"
#endif

#include "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#include "wx/wfstream.h"
#include "wx/image.h"
#include "wx/gifdecod.h"
#include "wx/log.h"
#include "wx/dcmemory.h"
#include "wx/animate/animate.h"
#include "wx/dc.h"
#include "wx/dcclient.h"

/*
 * wxAnimationPlayer
 */

IMPLEMENT_CLASS(wxAnimationPlayer, wxObject)

wxAnimationPlayer::wxAnimationPlayer(wxAnimationBase *animation, bool destroyAnimation)
{
    m_animation = animation;
    m_destroyAnimation = destroyAnimation;
    m_currentFrame = 0;
    m_window = (wxWindow*) NULL;
    m_position = wxPoint(0, 0);
    m_looped = TRUE;
    m_isPlaying = FALSE;
    m_useBackgroundColour = FALSE;
    m_customBackgroundColour = wxColour(0, 0, 0);
    m_useCustomBackgroundColour = FALSE;
    m_useParentBackground = FALSE;
    m_timer.SetPlayer(this);
}

wxAnimationPlayer::~wxAnimationPlayer()
{
    Stop();
    ClearCache();

    if (m_destroyAnimation)
        delete m_animation;
}

void wxAnimationPlayer::SetAnimation(wxAnimationBase* animation, bool destroyAnimation)
{
    ClearCache();
    if (m_destroyAnimation)
        delete m_animation;
    m_animation = animation;
    m_destroyAnimation = destroyAnimation;
}

// Play
bool wxAnimationPlayer::Play(wxWindow& window, const wxPoint& pos, bool WXUNUSED(looped))
{
    m_window = & window;

    if (!m_animation || !m_animation->IsValid())
        return FALSE;

    wxSize sz = GetLogicalScreenSize();
    wxRect rect(pos, sz);
    SaveBackground(rect);

    if (m_frames.GetCount() == 0)
    {
        if (!Build())
        {
            wxLogWarning(_T("wxAnimationPlayer::Play: could not build the image cache."));
            return FALSE;
        }
    }
    m_currentFrame = 0;

    // Create the backing store
    m_backingStore.Create(sz.x, sz.y);

    PlayFrame();

    return TRUE;
}

// Build animation (list of wxImages). If not called before Play
// is called, Play will call this automatically.
bool wxAnimationPlayer::Build()
{
    ClearCache();
    if (m_animation)
    {
        int n = GetFrameCount();
        int i;
        for (i = 0; i < n; i++)
        {
            wxImage* image = GetFrame(i);
            if (image)
            {
                // If the frame has transparency,
                // set the colour so converting to a bitmap
                // will create a mask
                wxColour transparentColour;
                if (GetTransparentColour(transparentColour))
                    image->SetMaskColour(transparentColour.Red(), transparentColour.Green(), transparentColour.Blue());

                wxBitmap* bitmap = new wxBitmap(* image);
                delete image;
                if (bitmap)
                    m_frames.Append(bitmap);
                else
                    return FALSE;
            }
            else
                return FALSE;
        }
        return TRUE;
    }
    else
        return FALSE;
}

// Stop the animation
void wxAnimationPlayer::Stop()
{
    m_timer.Stop();
    m_isPlaying = FALSE;
}

// Draw the current view of the animation into this DC.
// Call this from your OnPaint, for example.
void wxAnimationPlayer::Draw(wxDC& dc)
{
    dc.DrawBitmap(m_backingStore, m_position.x, m_position.y);
}


int wxAnimationPlayer::GetFrameCount() const
{
    if (m_animation)
        return m_animation->GetFrameCount();
    else
        return 0;
}

wxImage* wxAnimationPlayer::GetFrame(int i) const
{
    if (m_animation)
        return m_animation->GetFrame(i);
    else
        return (wxImage*) NULL;
}

wxAnimationDisposal wxAnimationPlayer::GetDisposalMethod(int i) const
{
    if (m_animation)
        return m_animation->GetDisposalMethod(i);
    else
        return wxANIM_UNSPECIFIED;
}

wxRect wxAnimationPlayer::GetFrameRect(int i) const
{
    if (m_animation)
        return m_animation->GetFrameRect(i);
    else
        return wxRect(0, 0, 0, 0);
}

int wxAnimationPlayer::GetDelay(int i) const
{
    if (m_animation)
        return m_animation->GetDelay(i);
    else
        return 0;
}

wxSize wxAnimationPlayer::GetLogicalScreenSize() const
{
    if (m_animation)
        return m_animation->GetLogicalScreenSize();
    else
        return wxSize(0, 0);
}

bool wxAnimationPlayer::GetBackgroundColour(wxColour& col) const
{
    if (m_animation)
        return m_animation->GetBackgroundColour(col);
    else
        return FALSE;
}

bool wxAnimationPlayer::GetTransparentColour(wxColour& col) const
{
    if (m_animation)
        return m_animation->GetTransparentColour(col);
    else
        return FALSE;
}

// Play the frame
bool wxAnimationPlayer::PlayFrame(int frame, wxWindow& window, const wxPoint& WXUNUSED(pos))
{
    wxMemoryDC dc;
    dc.SelectObject(m_backingStore);

    // Draw the background: colour or area beneath animation
    wxColour col(255, 255, 255);

    if (UsingBackgroundColour())
    {
        if (UsingCustomBackgroundColour())
            col = GetCustomBackgroundColour();
        else
        {
            GetBackgroundColour(col);
        }

        // Draw the background colour loaded from the animation
        // (or set by the user)
        DrawBackground(dc, wxPoint(0, 0), col);
    }
    else
    {
        // Draw background we saved
        dc.DrawBitmap(m_savedBackground, 0, 0);
    }

    // Draw all intermediate frames that haven't been removed from the
    // animation
    int i;
    for (i = 0; i < (frame - 1); i++)
    {
        if ((GetDisposalMethod(i) == wxANIM_DONOTREMOVE) || (GetDisposalMethod(i) == wxANIM_UNSPECIFIED))
        {
            DrawFrame(i, dc, wxPoint(0, 0));
        }
    }
    DrawFrame(frame, dc, wxPoint(0, 0));

    dc.SelectObject(wxNullBitmap);

    // Draw from backing bitmap onto window
    wxClientDC clientDC(& window);
    Draw(clientDC);

    return TRUE;
}

bool wxAnimationPlayer::PlayFrame()
{
    m_isPlaying = TRUE;

    PlayFrame(GetCurrentFrame(), * GetWindow(), GetPosition());

    // Set the timer for the next frame
    m_timer.Start(GetDelay(GetCurrentFrame()));

    m_currentFrame ++;

    if (m_currentFrame == GetFrameCount())
    {
        // Should a non-looped animation display the last frame?
        if (!m_looped)
        {
            m_timer.Stop();
            m_isPlaying = FALSE;
        }
        else
            m_currentFrame = 0;
    }

    return TRUE;
}

// Clear the wxImage cache
void wxAnimationPlayer::ClearCache()
{
    wxList::compatibility_iterator node = m_frames.GetFirst();
    while (node)
    {
        wxList::compatibility_iterator next = node->GetNext();
        wxBitmap* bitmap = (wxBitmap*) node->GetData();
        delete bitmap;
        m_frames.Erase(node);
        node = next;
    }
}

// Draw the background colour
void wxAnimationPlayer::DrawBackground(wxDC& dc, const wxPoint& pos, const wxColour& colour)
{
    wxASSERT_MSG( (m_animation != NULL), _T("Animation not present in wxAnimationPlayer"));
    wxASSERT_MSG( (m_frames.GetCount() != 0), _T("Animation cache not present in wxAnimationPlayer"));

    // Optimization: if the first frame fills the whole area, and is non-transparent,
    // don't bother drawing the background

    wxBitmap* firstBitmap = (wxBitmap*) m_frames.GetFirst()->GetData() ;
    wxSize screenSize = GetLogicalScreenSize();
    if (!firstBitmap->GetMask() && (firstBitmap->GetWidth() == screenSize.x) && (firstBitmap->GetHeight() == screenSize.y))
    {
        return;
    }

    wxBrush brush(colour, wxSOLID);
    wxPen pen(colour, 1, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(pen);
    dc.SetLogicalFunction(wxCOPY);

    dc.DrawRectangle(pos.x, pos.y, screenSize.x, screenSize.y);
}

// Save the pertinent area of the window so we can restore
// it if drawing transparently
void wxAnimationPlayer::SaveBackground(const wxRect& rect)
{
    wxASSERT( GetWindow() );

    if (!GetWindow())
        return;

    m_savedBackground.Create(rect.width, rect.height);

    wxMemoryDC memDC;
    memDC.SelectObject(m_savedBackground);

    if (m_useParentBackground && GetWindow()->GetParent())
    {
        wxWindow* parent = GetWindow()->GetParent();
        wxClientDC dc(parent);

        // Translate the point to coordinates in the
        // parent's client area, going via screen coordinates
        wxPoint pt(rect.x, rect.y);
        wxPoint screenPt = GetWindow()->ClientToScreen(pt);
        wxPoint parentPt = parent->ScreenToClient(screenPt);

        memDC.Blit(0, 0, rect.width, rect.height, & dc, parentPt.x, parentPt.y);
    }
    else
    {
        wxClientDC dc(GetWindow());

        memDC.Blit(0, 0, rect.width, rect.height, & dc, rect.x, rect.y);
    }
    memDC.SelectObject(wxNullBitmap);
}

// Draw this frame
void wxAnimationPlayer::DrawFrame(int frame, wxDC& dc, const wxPoint& pos)
{
    wxASSERT_MSG( (m_animation != NULL), _T("Animation not present in wxAnimationPlayer"));
    wxASSERT_MSG( (m_frames.GetCount() != 0), _T("Animation cache not present in wxAnimationPlayer"));
    wxASSERT_MSG( !!m_frames.Item(frame), _T("Image not present in wxAnimationPlayer::DrawFrame"));

    wxBitmap* bitmap = (wxBitmap*) m_frames.Item(frame)->GetData() ;

    wxRect rect = GetFrameRect(frame);

    dc.DrawBitmap(* bitmap, pos.x + rect.x, pos.y + rect.y, (bitmap->GetMask() != NULL));
}

void wxAnimationTimer::Notify()
{
    m_player->PlayFrame();
}

/*
 * wxAnimationBase
 */

IMPLEMENT_ABSTRACT_CLASS(wxAnimationBase, wxObject)

/*
 * wxGIFAnimation
 */

IMPLEMENT_CLASS(wxGIFAnimation, wxAnimationBase)

wxGIFAnimation::wxGIFAnimation()
{
    m_decoder = (wxGIFDecoder*) NULL;
}

wxGIFAnimation::~wxGIFAnimation()
{
    delete m_decoder;
}

int wxGIFAnimation::GetFrameCount() const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    return m_decoder->GetNumberOfFrames();
}

wxImage* wxGIFAnimation::GetFrame(int i) const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    m_decoder->GoFrame(i);

    wxImage* image = new wxImage;
    m_decoder->ConvertToImage(image);
    return image;
}

wxAnimationDisposal wxGIFAnimation::GetDisposalMethod(int i) const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    m_decoder->GoFrame(i);

    int disposalMethod = m_decoder->GetDisposalMethod();
    return (wxAnimationDisposal) disposalMethod;
}

wxRect wxGIFAnimation::GetFrameRect(int i) const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    m_decoder->GoFrame(i);

    wxRect rect(m_decoder->GetLeft(), m_decoder->GetTop(), m_decoder->GetWidth(), m_decoder->GetHeight());
    return rect;
}

int wxGIFAnimation::GetDelay(int i) const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    m_decoder->GoFrame(i);
    return m_decoder->GetDelay();
}

wxSize wxGIFAnimation::GetLogicalScreenSize() const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    return wxSize(m_decoder->GetLogicalScreenWidth(), m_decoder->GetLogicalScreenHeight());
}

bool wxGIFAnimation::GetBackgroundColour(wxColour& col) const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    int i = m_decoder->GetBackgroundColour();
    if (i == -1)
        return FALSE;
    else
    {
        unsigned char* pal = m_decoder->GetPalette();

        if (pal)
        {
            col = wxColour(pal[3*i + 0], pal[3*i + 1], pal[3*i + 2]);
            return TRUE;
        }
        else
            return FALSE;
    }
}

bool wxGIFAnimation::GetTransparentColour(wxColour& col) const
{
    wxASSERT_MSG( (m_decoder != (wxGIFDecoder*) NULL), _T("m_decoder must be non-NULL"));

    int i = m_decoder->GetTransparentColour();
    if (i == -1)
        return FALSE;
    else
    {
        unsigned char* pal = m_decoder->GetPalette();

        if (pal)
        {
            col = wxColour(pal[3*i + 0], pal[3*i + 1], pal[3*i + 2]);
            return TRUE;
        }
        else
            return FALSE;
    }
}

bool wxGIFAnimation::IsValid() const
{
    return ((m_decoder != NULL) && (m_decoder->IsAnimation()));
}

bool wxGIFAnimation::LoadFile(const wxString& filename)
{
    if (m_decoder)
        delete m_decoder;
    m_decoder = NULL;

    if (wxFileExists(filename))
    {
        wxFileInputStream stream(filename);
        m_decoder = new wxGIFDecoder(& stream, TRUE);

        if (m_decoder->ReadGIF() != wxGIF_OK)
        {
            delete m_decoder;
            m_decoder = NULL;
            return FALSE;
        }

        if (!m_decoder->IsAnimation())
        {
            delete m_decoder;
            m_decoder = NULL;

            return FALSE;
        }
        else
            return TRUE;
    }
    else
        return FALSE;
}

/*
 * wxAnimationCtrlBase
 * Abstract base class for format-specific animation controls.
 * This class implements most of the functionality; all a derived
 * class has to do is create the appropriate animation class on demand.
 */

IMPLEMENT_ABSTRACT_CLASS(wxAnimationCtrlBase, wxControl)

BEGIN_EVENT_TABLE(wxAnimationCtrlBase, wxControl)
    EVT_PAINT(wxAnimationCtrlBase::OnPaint)
END_EVENT_TABLE()

bool wxAnimationCtrlBase::Create(wxWindow *parent, wxWindowID id,
            const wxString& filename, const wxPoint& pos,
            const wxSize& size, long style, const wxString& name)
{
    m_animation = NULL;
    m_filename = filename;

    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return FALSE;

    SetBackgroundColour(parent->GetBackgroundColour());

    m_animationPlayer.SetCustomBackgroundColour(GetBackgroundColour());

    // Want to give the impression of transparency by painting
    // the parent background
//    if (parent)
//        m_animationPlayer.UseParentBackground(TRUE);
    m_animationPlayer.SetWindow(this);
    m_animationPlayer.SetPosition(wxPoint(0, 0));
    m_animationPlayer.SetDestroyAnimation(FALSE);

    return TRUE;
}

wxAnimationCtrlBase::~wxAnimationCtrlBase()
{
    if (m_animationPlayer.IsPlaying())
        m_animationPlayer.Stop();
    m_animationPlayer.SetAnimation(NULL, FALSE);
    delete m_animation;
}

bool wxAnimationCtrlBase::LoadFile(const wxString& filename)
{
    if (m_animationPlayer.IsPlaying())
        m_animationPlayer.Stop();

    wxString filename1(filename);

    if (filename1.IsEmpty())
        filename1 = m_filename;

    if (filename1.IsEmpty())
        return FALSE;

    if (m_animation)
    {
        delete m_animation;
        m_animation = NULL;
    }

    m_animation = DoCreateAnimation(filename1);
    if (!m_animation)
        return FALSE;

    if (!m_animation->LoadFile(filename) || !m_animation->IsValid())
    {
        delete m_animation;
        m_animation = NULL;
        return FALSE;
    }
    m_animationPlayer.SetAnimation(m_animation, FALSE);

    if (GetWindowStyle() & wxAN_FIT_ANIMATION)
        FitToAnimation();

    return TRUE;
}

bool wxAnimationCtrlBase::Play(bool looped)
{
    return m_animationPlayer.Play(*this, wxPoint(0, 0), looped);
}

wxSize wxAnimationCtrlBase::DoGetBestSize() const
{
    if (m_animationPlayer.HasAnimation() && (GetWindowStyle() & wxAN_FIT_ANIMATION))
    {
        return m_animationPlayer.GetLogicalScreenSize();
    }
    else
    {
        return GetSize();
    }
}

void wxAnimationCtrlBase::FitToAnimation()
{
    if (!m_animationPlayer.HasAnimation())
        return;

    wxSize sz = m_animationPlayer.GetLogicalScreenSize();
    SetClientSize(sz);
}

void wxAnimationCtrlBase::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    if (GetPlayer().IsPlaying())
    {
        GetPlayer().Draw(dc);
    }
}

/*
 * wxGIFAnimationCtrl
 * Provides a GIF animation class when required.
 */

IMPLEMENT_ABSTRACT_CLASS(wxGIFAnimationCtrl, wxAnimationCtrlBase)

wxAnimationBase* wxGIFAnimationCtrl::DoCreateAnimation(const wxString& WXUNUSED(filename))
{
    return new wxGIFAnimation;
}
