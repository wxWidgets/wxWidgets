/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cocoa/private.h
// Purpose:     Private declarations: as this header is only included by
//              wxWidgets itself, it may contain identifiers which don't start
//              with "wx".
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_COCOA_H_
#define _WX_PRIVATE_COCOA_H_

#include <ApplicationServices/ApplicationServices.h>

#ifdef __OBJC__
    #import <Cocoa/Cocoa.h>
#endif

//
// shared between Cocoa and Carbon
//

#include "wx/bmpbndl.h"

// bring in theming types without pulling in the headers

#if wxUSE_GUI
typedef SInt16 ThemeBrush;
CGColorRef WXDLLIMPEXP_CORE wxMacCreateCGColorFromHITheme( ThemeBrush brush ) ;
OSStatus WXDLLIMPEXP_CORE wxMacDrawCGImage(
                               CGContextRef    inContext,
                               const CGRect *  inBounds,
                               CGImageRef      inImage) ;
void WXDLLIMPEXP_CORE wxOSXDrawNSImage(
                                           CGContextRef    inContext,
                                           const CGRect *  inBounds,
                                           WX_NSImage      inImage) ;
WX_NSImage WXDLLIMPEXP_CORE wxOSXGetSystemImage(const wxString& name);
WX_NSImage WXDLLIMPEXP_CORE wxOSXGetNSImageFromCGImage( CGImageRef image, double scale = 1.0, bool isTemplate = false);
WX_NSImage WXDLLIMPEXP_CORE wxOSXGetNSImageFromIconRef( WXHICON iconref );
WX_NSImage WXDLLIMPEXP_CORE wxOSXGetNSImageFromCFURL( CFURLRef urlref );
WX_NSImage WXDLLIMPEXP_CORE wxOSXGetIconForType(OSType type );
void WXDLLIMPEXP_CORE wxOSXSetImageSize(WX_NSImage image, CGFloat width, CGFloat height);
wxBitmapBundle WXDLLIMPEXP_CORE wxOSXCreateSystemBitmapBundle(const wxString& id, const wxString &client, const wxSize& size);
WXWindow WXDLLIMPEXP_CORE wxOSXGetMainWindow();
WXWindow WXDLLIMPEXP_CORE wxOSXGetKeyWindow();
WXImage WXDLLIMPEXP_CORE wxOSXGetNSImageFromNSCursor(const WXHCURSOR cursor);

class WXDLLIMPEXP_FWD_CORE wxDialog;

class WXDLLIMPEXP_FWD_CORE wxWidgetCocoaImpl;

// a class which disables sending wx keydown events useful when adding text programmatically, for wx-internal use only
class wxWidgetCocoaNativeKeyDownSuspender
{
public:
    // stops sending keydown events for text inserted into this widget
    explicit wxWidgetCocoaNativeKeyDownSuspender(wxWidgetCocoaImpl *target);
    
    // resumes sending keydown events
    ~wxWidgetCocoaNativeKeyDownSuspender();
    
private:
    wxWidgetCocoaImpl *m_target;
    NSEvent* m_nsevent;
    bool m_wxsent;

    wxDECLARE_NO_COPY_CLASS(wxWidgetCocoaNativeKeyDownSuspender);
};

class WXDLLIMPEXP_CORE wxWidgetCocoaImpl : public wxWidgetImpl
{
public :
    wxWidgetCocoaImpl( wxWindowMac* peer , WXWidget w, int flags = 0 ) ;
    wxWidgetCocoaImpl() ;
    ~wxWidgetCocoaImpl();

    void Init();

    virtual bool        IsVisible() const override;
    virtual void        SetVisibility(bool) override;

    // we provide a static function which can be reused from
    // wxNonOwnedWindowCocoaImpl too
    static bool ShowViewOrWindowWithEffect(wxWindow *win,
                                           bool show,
                                           wxShowEffect effect,
                                           unsigned timeout);

    virtual bool ShowWithEffect(bool show,
                                wxShowEffect effect,
                                unsigned timeout) override;

    virtual void        Raise() override;

    virtual void        Lower() override;

    virtual void        ScrollRect( const wxRect *rect, int dx, int dy ) override;

    virtual WXWidget    GetWXWidget() const override { return m_osxView; }

    virtual void        SetBackgroundColour(const wxColour&) override;
    virtual bool        SetBackgroundStyle(wxBackgroundStyle style) override;
    virtual void        SetForegroundColour(const wxColour& col) override;

    virtual void        GetContentArea( int &left, int &top, int &width, int &height ) const override;
    virtual void        Move(int x, int y, int width, int height) override;
    virtual void        GetPosition( int &x, int &y ) const override;
    virtual void        GetSize( int &width, int &height ) const override;
    virtual void        SetControlSize( wxWindowVariant variant ) override;
    virtual void        GetLayoutInset(int &left , int &top , int &right, int &bottom) const override;
    virtual void        SetNeedsDisplay( const wxRect* where = nullptr ) override;
    virtual bool        GetNeedsDisplay() const override;

    virtual void        EnableFocusRing(bool enabled) override;

    virtual void        SetDrawingEnabled(bool enabled) override;

    virtual bool        CanFocus() const override;
    // return true if successful
    virtual bool        SetFocus() override;
    virtual bool        HasFocus() const override;

    void                RemoveFromParent() override;
    void                Embed( wxWidgetImpl *parent ) override;

    void                SetDefaultButton( bool isDefault ) override;
    void                PerformClick() override;
    virtual void        SetLabel(const wxString& title, wxFontEncoding encoding) override;

    void                SetCursor( const wxCursor & cursor ) override;
    void                CaptureMouse() override;
    void                ReleaseMouse() override;
#if wxUSE_DRAG_AND_DROP
    void                SetDropTarget(wxDropTarget* target) override;
#endif
    wxInt32             GetValue() const override;
    void                SetValue( wxInt32 v ) override;
    wxBitmap            GetBitmap() const override;
    void                SetBitmap( const wxBitmapBundle& bitmap ) override;
    void                SetBitmapPosition( wxDirection dir ) override;
    void                SetupTabs( const wxNotebook &notebook ) override;
    void                GetBestRect( wxRect *r ) const override;
    bool                IsEnabled() const override;
    void                Enable( bool enable ) override;
    bool                ButtonClickDidStateChange() override { return true; }
    void                SetMinimum( wxInt32 v ) override;
    void                SetMaximum( wxInt32 v ) override;
    void                SetIncrement(int value) override;
    wxInt32             GetMinimum() const override;
    wxInt32             GetMaximum() const override;
    int                 GetIncrement() const override;
    void                PulseGauge() override;
    void                SetScrollThumb( wxInt32 value, wxInt32 thumbSize ) override;

    void                SetFont(const wxFont & font) override;
    void                SetToolTip( wxToolTip* tooltip ) override;

    void                InstallEventHandler( WXWidget control = nullptr ) override;
    bool                EnableTouchEvents(int eventsMask) override;

    virtual bool        ShouldHandleKeyNavigation(const wxKeyEvent &event) const;
    bool                DoHandleKeyNavigation(const wxKeyEvent &event);
    virtual bool        DoHandleMouseEvent(NSEvent *event);
    virtual bool        DoHandleKeyEvent(NSEvent *event);
    virtual bool        DoHandleCharEvent(NSEvent *event, NSString *text);
    virtual void        DoNotifyFocusSet();
    virtual void        DoNotifyFocusLost();
    virtual void        DoNotifyFocusEvent(bool receivedFocus, wxWidgetImpl* otherWindow);

    virtual void        SetupKeyEvent(wxKeyEvent &wxevent, NSEvent * nsEvent, NSString* charString = nullptr);
    virtual void        SetupMouseEvent(wxMouseEvent &wxevent, NSEvent * nsEvent);
    void                SetupCoordinates(wxCoord &x, wxCoord &y, NSEvent *nsEvent);
    virtual bool        SetupCursor(NSEvent* event);

    virtual void        PanGestureEvent(NSPanGestureRecognizer *panGestureRecognizer);
    virtual void        ZoomGestureEvent(NSMagnificationGestureRecognizer *magnificationGestureRecognizer);
    virtual void        RotateGestureEvent(NSRotationGestureRecognizer *rotationGestureRecognizer);
    virtual void        LongPressEvent(NSPressGestureRecognizer *pressGestureRecognizer);
    virtual void        TouchesBegan(NSEvent *event);
    virtual void        TouchesMoved(NSEvent *event);
    virtual void        TouchesEnded(NSEvent *event);

#if !wxOSX_USE_NATIVE_FLIPPED
    void                SetFlipped(bool flipped);
    virtual bool        IsFlipped() const { return m_isFlipped; }
#endif

    virtual double      GetContentScaleFactor() const override;

    // cocoa thunk connected calls

#if wxUSE_DRAG_AND_DROP
    virtual unsigned int        draggingEntered(void* sender, WXWidget slf, void* _cmd);
    virtual void                draggingExited(void* sender, WXWidget slf, void* _cmd);
    virtual unsigned int        draggingUpdated(void* sender, WXWidget slf, void* _cmd);
    virtual bool                performDragOperation(void* sender, WXWidget slf, void* _cmd);
#endif
    virtual void                mouseEvent(WX_NSEvent event, WXWidget slf, void* _cmd);
    virtual void                cursorUpdate(WX_NSEvent event, WXWidget slf, void* _cmd);
    virtual void                keyEvent(WX_NSEvent event, WXWidget slf, void* _cmd);
    virtual void                insertText(NSString* text, WXWidget slf, void* _cmd);
    // Returns true if the event was processed by a user-defined event handler.
    virtual bool                doCommandBySelector(void* sel, WXWidget slf, void* _cmd);
    virtual bool                acceptsFirstResponder(WXWidget slf, void* _cmd);
    virtual bool                becomeFirstResponder(WXWidget slf, void* _cmd);
    virtual bool                resignFirstResponder(WXWidget slf, void* _cmd);
#if !wxOSX_USE_NATIVE_FLIPPED
    virtual bool                isFlipped(WXWidget slf, void* _cmd);
#endif
    virtual void                drawRect(void* rect, WXWidget slf, void* _cmd);

    virtual void                controlAction(WXWidget slf, void* _cmd, void* sender);
    virtual void                controlDoubleAction(WXWidget slf, void* _cmd, void *sender);

    // for wxTextCtrl-derived classes, put here since they don't all derive
    // from the same pimpl class.
    virtual void                controlTextDidChange();

protected:
    WXWidget m_osxView;
    
    // begins processing of native key down event, storing the native event for later wx event generation
    void BeginNativeKeyDownEvent( NSEvent* event );
    // done with the current native key down event
    void EndNativeKeyDownEvent();
    // allow executing text changes without triggering key down events

    // is currently processing a native key down event
    bool IsInNativeKeyDown() const;
    // the native key event
    NSEvent* GetLastNativeKeyDownEvent();
    // did send the wx event for the current native key down event
    void SetKeyDownSent();
    // was the wx event for the current native key down event sent
    bool WasKeyDownSent() const;


    // Return the view to apply the font/colour to.
    NSView* GetViewWithText() const;

    NSEvent* m_lastKeyDownEvent;
    bool m_lastKeyDownWXSent;
#if !wxOSX_USE_NATIVE_FLIPPED
    bool m_isFlipped;
#endif
    // if it the control has an editor, that editor will already send some
    // events, don't resend them
    bool m_hasEditor;

    friend class wxWidgetCocoaNativeKeyDownSuspender;
    
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxWidgetCocoaImpl);
};

DECLARE_WXCOCOA_OBJC_CLASS( wxNSWindow );

class wxNonOwnedWindowCocoaImpl : public wxNonOwnedWindowImpl
{
public :
    wxNonOwnedWindowCocoaImpl( wxNonOwnedWindow* nonownedwnd) ;
    wxNonOwnedWindowCocoaImpl();

    virtual ~wxNonOwnedWindowCocoaImpl();

    virtual void WillBeDestroyed() override;
    void Create( wxWindow* parent, const wxPoint& pos, const wxSize& size,
    long style, long extraStyle, const wxString& name ) override;
    void Create( wxWindow* parent, WXWindow nativeWindow );

    WXWindow GetWXWindow() const override;
    void Raise() override;
    void Lower() override;
    bool Show(bool show) override;

    virtual bool ShowWithEffect(bool show,
                                wxShowEffect effect,
                                unsigned timeout) override;

    void Update() override;
    bool SetTransparent(wxByte alpha) override;
    bool SetBackgroundColour(const wxColour& col ) override;
    void SetExtraStyle( long exStyle ) override;
    void SetWindowStyleFlag( long style ) override;
    bool SetBackgroundStyle(wxBackgroundStyle style) override;
    bool CanSetTransparent() override;

    void MoveWindow(int x, int y, int width, int height) override;
    void GetPosition( int &x, int &y ) const override;
    void GetSize( int &width, int &height ) const override;

    void GetContentArea( int &left, int &top, int &width, int &height ) const override;
    bool SetShape(const wxRegion& region) override;

    virtual void SetTitle( const wxString& title, wxFontEncoding encoding ) override;

    virtual wxContentProtection GetContentProtection() const override;
    virtual bool SetContentProtection(wxContentProtection contentProtection) override;

    virtual bool EnableCloseButton(bool enable) override;
    virtual bool EnableMaximizeButton(bool enable) override;
    virtual bool EnableMinimizeButton(bool enable) override;

    virtual bool IsMaximized() const override;

    virtual bool IsIconized() const override;

    virtual void Iconize( bool iconize ) override;

    virtual void Maximize(bool maximize) override;

    virtual bool IsFullScreen() const override;

    bool EnableFullScreenView(bool enable, long style) override;

    virtual bool ShowFullScreen(bool show, long style) override;

    virtual void ShowWithoutActivating() override;

    virtual void RequestUserAttention(int flags) override;

    virtual void ScreenToWindow( int *x, int *y ) override;

    virtual void WindowToScreen( int *x, int *y ) override;

    virtual bool IsActive() override;

    virtual void SetModified(bool modified) override;
    virtual bool IsModified() const override;

    virtual void SetRepresentedFilename(const wxString& filename) override;

    virtual void SetBottomBorderThickness(int thickness) override;

    wxNonOwnedWindow*   GetWXPeer() { return m_wxPeer; }

    CGWindowLevel   GetWindowLevel() const override { return m_macWindowLevel; }
    void            RestoreWindowLevel() override;

    bool m_macIgnoreNextFullscreenChange = false;
    long m_macFullscreenStyle = wxFULLSCREEN_ALL;

    static WX_NSResponder GetNextFirstResponder() ;
    static WX_NSResponder GetFormerFirstResponder() ;
protected :
    CGWindowLevel   m_macWindowLevel;
    WXWindow        m_macWindow;
    void *          m_macFullScreenData ;

private:
    void SetUpForModalParent();

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxNonOwnedWindowCocoaImpl);
};

DECLARE_WXCOCOA_OBJC_CLASS( wxNSButton );

class wxButtonCocoaImpl : public wxWidgetCocoaImpl, public wxButtonImpl
{
public:
    wxButtonCocoaImpl(wxWindowMac *wxpeer, wxNSButton *v);
    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;
#if wxUSE_MARKUP
    virtual void SetLabelMarkup(const wxString& markup) override;
#endif // wxUSE_MARKUP

    void SetPressedBitmap( const wxBitmapBundle& bitmap ) override;
    void SetAcceleratorFromLabel(const wxString& label);

    NSButton *GetNSButton() const;
};

#ifdef __OBJC__

    typedef NSRect WXRect;
    typedef void (*wxOSX_TextEventHandlerPtr)(NSView* self, SEL _cmd, NSString *event);
    typedef void (*wxOSX_EventHandlerPtr)(NSView* self, SEL _cmd, NSEvent *event);
    typedef BOOL (*wxOSX_PerformKeyEventHandlerPtr)(NSView* self, SEL _cmd, NSEvent *event);
    typedef void (*wxOSX_DoCommandBySelectorPtr)(NSView* self, SEL _cmd, SEL _sel);
    typedef BOOL (*wxOSX_FocusHandlerPtr)(NSView* self, SEL _cmd);
    typedef void (*wxOSX_DoCommandBySelectorPtr)(NSView* self, SEL _cmd, SEL _sel);
    typedef NSDragOperation (*wxOSX_DraggingEnteredOrUpdatedHandlerPtr)(NSView *self, SEL _cmd, void *sender);
    typedef void (*wxOSX_DraggingExitedHandlerPtr)(NSView *self, SEL _cmd, void *sender);
    typedef BOOL (*wxOSX_PerformDragOperationHandlerPtr)(NSView *self, SEL _cmd, void *sender);

    WXDLLIMPEXP_CORE NSScreen* wxOSXGetMenuScreen();
    WXDLLIMPEXP_CORE NSRect wxToNSRect( NSView* parent, const wxRect& r );
    WXDLLIMPEXP_CORE wxRect wxFromNSRect( NSView* parent, const NSRect& rect );
    WXDLLIMPEXP_CORE NSPoint wxToNSPoint( NSView* parent, const wxPoint& p );
    WXDLLIMPEXP_CORE wxPoint wxFromNSPoint( NSView* parent, const NSPoint& p );

    NSRect WXDLLIMPEXP_CORE wxOSXGetFrameForControl( wxWindowMac* window , const wxPoint& pos , const wxSize &size ,
        bool adjustForOrigin = true );

    WXDLLIMPEXP_CORE NSView* wxOSXGetViewFromResponder( NSResponder* responder );

    // used for many wxControls

    @interface wxNSButton : NSButton
    {
        NSTrackingRectTag rectTag;
    }

    @end

    @interface wxNSBox : NSBox
    {
    }

    @end

    @interface wxNSTextFieldEditor : NSTextView
    {
        NSEvent* lastKeyDownEvent;
        NSTextField* textField;
    }

    - (void) setTextField:(NSTextField*) field;
    @end

    @interface wxNSTextField : NSTextField <NSTextFieldDelegate>
    {
    }

    // Note that the name WXFieldEditor is special and is checked by
    // windowWillReturnFieldEditor: in wxNonOwnedWindowController
    // implementation, see there.
    @property (retain) wxNSTextFieldEditor* WXFieldEditor;

    @end

    @interface wxNSSecureTextField : NSSecureTextField <NSTextFieldDelegate>
    {
    }

    @end


    @interface wxNSTextView : NSTextView <NSTextViewDelegate>
    {
    }

    - (void)textDidChange:(NSNotification *)aNotification;
    - (void)changeColor:(id)sender;

    @property (retain) NSUndoManager* undoManager;

    @end

    @interface wxNSSearchField : NSSearchField
    {
        BOOL m_withinTextDidChange;
    }

    @property (retain) wxNSTextFieldEditor* WXFieldEditor;

    @end

    @interface wxNSComboBox : NSComboBox
    {
    }

    @property (retain) wxNSTextFieldEditor* WXFieldEditor;

    @end



    @interface wxNSMenu : NSMenu
    {
       wxMenuImpl* impl;
    }

    - (void) setImplementation:(wxMenuImpl*) item;
    - (wxMenuImpl*) implementation;

    @end

    @interface wxNSMenuItem : NSMenuItem
    {
       wxMenuItemImpl* impl;
    }

    - (void) setImplementation:(wxMenuItemImpl*) item;
    - (wxMenuItemImpl*) implementation;

    - (void)clickedAction:(id)sender;
    - (BOOL)validateMenuItem:(NSMenuItem *)menuItem;

    @end

    // this enum declares which methods should not be overridden in the native view classes
    enum wxOSXSkipOverrides {
        wxOSXSKIP_NONE = 0x0,
        wxOSXSKIP_DRAW = 0x1,
        wxOSXSKIP_DND = 0x2
    };

    void WXDLLIMPEXP_CORE wxOSXCocoaClassAddWXMethods(Class c, wxOSXSkipOverrides skipFlags = wxOSXSKIP_NONE);

    /*
    We need this for ShowModal, as the sheet just disables the parent window and
    returns control to the app, whereas we don't want to return from ShowModal
    until the sheet has been dismissed.
    */
    @interface ModalDialogDelegate : NSObject
    {
        BOOL sheetFinished;
        int resultCode;
        wxDialog* impl;
    }

    - (void)setImplementation: (wxDialog *)dialog;
    - (BOOL)finished;
    - (int)code;
    - (void)waitForSheetToFinish;
    - (void)sheetDidEnd:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo;
    @end

    WXEXPORT
    @interface wxNSAppController : NSObject <NSApplicationDelegate>
    {
    }

    @end

#endif // __OBJC__

// NSCursor

WX_NSCursor wxMacCocoaCreateStockCursor( int cursor_type );
WX_NSCursor  wxMacCocoaCreateCursorFromCGImage( CGImageRef cgImageRef, float hotSpotX, float hotSpotY );
void  wxMacCocoaSetCursor( WX_NSCursor cursor );
void  wxMacCocoaHideCursor();
void  wxMacCocoaShowCursor();

typedef struct tagClassicCursor
{
    wxUint16 bits[16];
    wxUint16 mask[16];
    wxInt16 hotspot[2];
}ClassicCursor;

const short kwxCursorBullseye = 0;
const short kwxCursorBlank = 1;
const short kwxCursorPencil = 2;
const short kwxCursorMagnifier = 3;
const short kwxCursorNoEntry = 4;
const short kwxCursorPaintBrush = 5;
const short kwxCursorPointRight = 6;
const short kwxCursorPointLeft = 7;
const short kwxCursorQuestionArrow = 8;
const short kwxCursorRightArrow = 9;
const short kwxCursorSizeNS = 10;
const short kwxCursorSize = 11;
const short kwxCursorSizeNESW = 12;
const short kwxCursorSizeNWSE = 13;
const short kwxCursorRoller = 14;
const short kwxCursorWatch = 15;
const short kwxCursorLast = kwxCursorWatch;

// exposing our fallback cursor map

extern ClassicCursor gMacCursors[];

extern NSLayoutManager* gNSLayoutManager;

// helper class for setting the current appearance to the
// effective appearance and restore when exiting scope

class WXDLLIMPEXP_CORE wxOSXEffectiveAppearanceSetter
{
public:
    wxOSXEffectiveAppearanceSetter();
    ~wxOSXEffectiveAppearanceSetter();
private:
    void * formerAppearance;
};

#endif // wxUSE_GUI

#endif
    // _WX_PRIVATE_COCOA_H_
