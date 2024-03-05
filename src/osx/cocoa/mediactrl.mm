/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/mediactrl.mm
// Purpose:     Built-in Media Backends for Cocoa
// Author:      Ryan Norton <wxprojects@comcast.net>, Stefan Csomor
// Created:     02/03/05
// Copyright:   (c) 2004-2005 Ryan Norton, (c) 2005 David Elliot, (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


//---------------------------------------------------------------------------
// Compilation guard
//---------------------------------------------------------------------------
#if wxUSE_MEDIACTRL

#include "wx/mediactrl.h"

#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

#if wxOSX_USE_COCOA
    #define wxOSX_USE_AVKIT 1
#endif

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//
//  wxAVMediaBackend
//
//---------------------------------------------------------------------------

#import <AVFoundation/AVFoundation.h>

#if wxOSX_USE_AVKIT
#import <AVKit/AVKit.h>
#endif

class WXDLLIMPEXP_FWD_MEDIA wxAVMediaBackend;

static void *AVSPPlayerItemStatusContext = &AVSPPlayerItemStatusContext;
static void *AVSPPlayerRateContext = &AVSPPlayerRateContext;

@interface wxAVPlayer : AVPlayer {

    AVPlayerLayer *playerLayer;

    wxAVMediaBackend* m_backend;
}

-(BOOL)isPlaying;

@property (retain) AVPlayerLayer *playerLayer;

@end

class WXDLLIMPEXP_MEDIA wxAVMediaBackend : public wxMediaBackendCommonBase
{
public:

    wxAVMediaBackend();
    ~wxAVMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name) override;

    virtual bool Play() override;
    virtual bool Pause() override;
    virtual bool Stop() override;

    virtual bool Load(const wxString& fileName) override;
    virtual bool Load(const wxURI& location) override;

    virtual wxMediaState GetState() override;

    virtual bool SetPosition(wxLongLong where) override;
    virtual wxLongLong GetPosition() override;
    virtual wxLongLong GetDuration() override;

    virtual void Move(int x, int y, int w, int h) override;
    wxSize GetVideoSize() const override;

    virtual double GetPlaybackRate() override;
    virtual bool SetPlaybackRate(double dRate) override;

    virtual double GetVolume() override;
    virtual bool SetVolume(double dVolume) override;

    void Cleanup();
    void FinishLoad();

    virtual bool   ShowPlayerControls(wxMediaCtrlPlayerControls flags) override;
private:
    void DoShowPlayerControls(wxMediaCtrlPlayerControls flags);

    wxSize m_bestSize;              //Original movie size
    wxAVPlayer* m_player;               //AVPlayer handle/instance

    wxMediaCtrlPlayerControls m_interfaceflags; // Saved interface flags

    wxDECLARE_DYNAMIC_CLASS(wxAVMediaBackend);
};

// --------------------------------------------------------------------------
// wxAVMediaBackend
// --------------------------------------------------------------------------

@implementation wxAVPlayer

@synthesize playerLayer;

- (id) init
{
    if ( self = [super init] )
    {
        [self addObserver:self forKeyPath:@"currentItem.status"
                  options:NSKeyValueObservingOptionNew context:AVSPPlayerItemStatusContext];
        [self addObserver:self forKeyPath:@"rate"
              options:NSKeyValueObservingOptionNew context:AVSPPlayerRateContext];
    }
    return self;
}

- (void)dealloc
{
    [playerLayer release];
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [self removeObserver:self forKeyPath:@"rate" context:AVSPPlayerRateContext];
    [self removeObserver:self forKeyPath:@"currentItem.status" context:AVSPPlayerItemStatusContext];

    [super dealloc];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (context == AVSPPlayerItemStatusContext)
    {
        id val = [change objectForKey:NSKeyValueChangeNewKey];
        if ( val != [NSNull null ] )
        {
            AVPlayerItemStatus status = (AVPlayerItemStatus) [ val integerValue];

            switch (status)
            {
                case AVPlayerItemStatusUnknown:
                    break;
                case AVPlayerItemStatusReadyToPlay:
                    [[NSNotificationCenter defaultCenter]
                     addObserver:self selector:@selector(playerItemDidReachEnd:)
                     name:AVPlayerItemDidPlayToEndTimeNotification object:self.currentItem];
                    m_backend->FinishLoad();
                    break;
                case AVPlayerItemStatusFailed:
                    break;
                default:
                    break;
            }
        }
    }
    else if (context == AVSPPlayerRateContext)
    {
        NSNumber* newRate = [change objectForKey:NSKeyValueChangeNewKey];
        if ([newRate intValue] == 0)
        {
            m_backend->QueuePauseEvent();
        }
        else if ( [self isPlaying] == NO )
        {
            m_backend->QueuePlayEvent();
        }
    }
    else
    {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

-(wxAVMediaBackend*) backend
{
    return m_backend;
}

-(void) setBackend:(wxAVMediaBackend*) backend
{
    m_backend = backend;
}

- (void)playerItemDidReachEnd:(NSNotification *)notification
{
    wxUnusedVar(notification);
    if ( m_backend )
    {
        if ( m_backend->SendStopEvent() )
            m_backend->QueueFinishEvent();
    }
}

-(BOOL)isPlaying
{
    if ([self rate] == 0)
    {
        return NO;
    }

    return YES;
}

@end

#if wxOSX_USE_IPHONE

@interface wxAVView : UIView
{
}

@end

@implementation wxAVView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

+ (Class)layerClass
{
    return [AVPlayerLayer class];
}

- (id) initWithFrame:(CGRect)rect player:(wxAVPlayer*) player
{
    if ( self = [super initWithFrame:rect] )
    {
        AVPlayerLayer* playerLayer = (AVPlayerLayer*) [self layer];
        [playerLayer setPlayer: player];
        [player setPlayerLayer:playerLayer];
    }
    return self;
}

- (AVPlayerLayer*) playerLayer
{
    return (AVPlayerLayer*) [self layer];
}
@end

#else

#if wxOSX_USE_AVKIT

@interface wxAVPlayerView : AVPlayerView
{
}

@end

@implementation wxAVPlayerView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (id) initWithFrame:(NSRect)rect player:(wxAVPlayer*) player
{
    if ( self = [super initWithFrame:rect] )
    {
        self.player = player;
    }
    return self;
}

- (AVPlayerLayer*) playerLayer
{
    return (AVPlayerLayer*) [[[self layer] sublayers] firstObject];
}

@end

#endif // wxOSX_USE_AVKIT

@interface wxAVView : NSView
{
}

@end

@implementation wxAVView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (id) initWithFrame:(NSRect)rect player:(wxAVPlayer*) player
{
    if ( self = [super initWithFrame:rect] )
    {
        [self setWantsLayer:YES];
        AVPlayerLayer* playerlayer = [AVPlayerLayer playerLayerWithPlayer: player];
        [player setPlayerLayer:playerlayer];

        [playerlayer setFrame:[[self layer] bounds]];
        [playerlayer setAutoresizingMask:kCALayerWidthSizable | kCALayerHeightSizable];
        [[self layer] addSublayer:playerlayer];
    }

    return self;
}

- (AVPlayerLayer*) playerLayer
{
    return (AVPlayerLayer*) [[[self layer] sublayers] firstObject];
}

@end

#endif

wxIMPLEMENT_DYNAMIC_CLASS(wxAVMediaBackend, wxMediaBackend);

wxAVMediaBackend::wxAVMediaBackend() :
m_player(nil),
m_interfaceflags(wxMEDIACTRLPLAYERCONTROLS_NONE)
{
}

wxAVMediaBackend::~wxAVMediaBackend()
{
    Cleanup();
}

bool wxAVMediaBackend::CreateControl(wxControl* inctrl, wxWindow* parent,
                                     wxWindowID wid,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    wxMediaCtrl* mediactrl = (wxMediaCtrl*) inctrl;

    mediactrl->DontCreatePeer();

    if ( !mediactrl->wxControl::Create(
                                       parent, wid, pos, size,
                                       wxWindow::MacRemoveBordersFromStyle(style),
                                       validator, name))
    {
        return false;
    }

    m_player = [[wxAVPlayer alloc] init];
    [m_player setBackend:this];

    WXRect r = wxOSXGetFrameForControl( mediactrl, pos , size ) ;

    WXWidget view = nullptr;
#if wxOSX_USE_AVKIT
    view = [[wxAVPlayerView alloc] initWithFrame: r player:m_player];
    [(wxAVPlayerView*) view setControlsStyle:AVPlayerViewControlsStyleNone];
#else
    view = [[wxAVView alloc] initWithFrame: r player:m_player];
#endif

#if wxOSX_USE_IPHONE
    wxWidgetIPhoneImpl* impl = new wxWidgetIPhoneImpl(mediactrl,view);
#else
    wxWidgetCocoaImpl* impl = new wxWidgetCocoaImpl(mediactrl,view);
#endif
    mediactrl->SetPeer(impl);

    m_ctrl = mediactrl;
    return true;
}

bool wxAVMediaBackend::Load(const wxString& fileName)
{
    return Load(
                wxURI(
                      wxString( wxT("file://") ) + fileName
                      )
                );
}

bool wxAVMediaBackend::Load(const wxURI& location)
{
    wxCFStringRef uri(location.BuildURI());
    NSURL *url = [NSURL URLWithString: uri.AsNSString()];

    AVAsset* asset = [AVAsset assetWithURL:url];
    if (! asset )
        return false;

    if ( [asset isPlayable] )
    {
        AVPlayerItem *playerItem = [AVPlayerItem playerItemWithAsset:asset];
        [m_player replaceCurrentItemWithPlayerItem:playerItem];

        return playerItem != nil;
    }
    return false;
}

void wxAVMediaBackend::FinishLoad()
{
    DoShowPlayerControls(m_interfaceflags);

    AVPlayerItem *playerItem = [m_player currentItem];

    CGSize s = [playerItem presentationSize];
    m_bestSize = wxSize(s.width, s.height);

    NotifyMovieLoaded();
}

bool wxAVMediaBackend::Play()
{
    [m_player play];
    return true;
}

bool wxAVMediaBackend::Pause()
{
    [m_player pause];
    return true;
}

bool wxAVMediaBackend::Stop()
{
    [m_player pause];
    [m_player seekToTime:kCMTimeZero];
    return true;
}

double wxAVMediaBackend::GetVolume()
{
    return [m_player volume];
}

bool wxAVMediaBackend::SetVolume(double dVolume)
{
    [m_player setVolume:dVolume];
    return true;
}
double wxAVMediaBackend::GetPlaybackRate()
{
    return [m_player rate];
}

bool wxAVMediaBackend::SetPlaybackRate(double dRate)
{
    [m_player setRate:dRate];
    return true;
}

bool wxAVMediaBackend::SetPosition(wxLongLong where)
{
    [m_player seekToTime:CMTimeMakeWithSeconds(where.GetValue() / 1000.0, 60000)
              toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];

    return true;
}

wxLongLong wxAVMediaBackend::GetPosition()
{
    return CMTimeGetSeconds([m_player currentTime])*1000.0;
}

wxLongLong wxAVMediaBackend::GetDuration()
{
    AVPlayerItem *playerItem = [m_player currentItem];

    if ([playerItem status] == AVPlayerItemStatusReadyToPlay)
        return CMTimeGetSeconds([[playerItem asset] duration])*1000.0;
    else
        return 0.f;
}

wxMediaState wxAVMediaBackend::GetState()
{
    if ( [m_player isPlaying] )
        return wxMEDIASTATE_PLAYING;
    else
    {
        if ( GetPosition() == 0 )
            return wxMEDIASTATE_STOPPED;
        else
            return wxMEDIASTATE_PAUSED;
    }
}

void wxAVMediaBackend::Cleanup()
{
    [m_player pause];
    [m_player release];
    m_player = nil;
}

wxSize wxAVMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

void wxAVMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y), int WXUNUSED(w), int WXUNUSED(h))
{
    // as we have a native player, no need to move the video area
}

bool wxAVMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    if ( m_interfaceflags != flags )
        DoShowPlayerControls(flags);

    m_interfaceflags = flags;
    return true;
}

void wxAVMediaBackend::DoShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
#if wxOSX_USE_AVKIT
    NSView* view = m_ctrl->GetHandle();
    if ( [view isKindOfClass:[wxAVPlayerView class]] )
    {
        wxAVPlayerView* playerView = (wxAVPlayerView*) view;
        if (flags == wxMEDIACTRLPLAYERCONTROLS_NONE )
            playerView.controlsStyle = AVPlayerViewControlsStyleNone;
        else
            playerView.controlsStyle = AVPlayerViewControlsStyleDefault;
    }
#endif
}

//in source file that contains stuff you don't directly use
#include "wx/html/forcelnk.h"
FORCE_LINK_ME(basewxmediabackends);

#endif //wxUSE_MEDIACTRL
