/////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/stc_osx.mm
// Purpose:     wxOSX implementation of wxStyledTextCtrl
// Author:      Maarten Bent
// Created:     2026-11-19
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STC

#include "wx/stc/stc.h"
#include "wx/osx/private.h"

#include <objc/runtime.h>

#include "Scintilla.h"
#import "ScintillaView.h"

// ----------------------------------------------------------------------------
// Objective-C delegate receiving Scintilla notifications
// ----------------------------------------------------------------------------

@interface wxScintillaNotificationDelegate : NSObject <ScintillaNotificationProtocol>
{
    wxStyledTextCtrl* m_stc;
}

- (id)initWithSTC:(wxStyledTextCtrl*)stc;
- (void)detach;
- (void)notification:(SCNotification*)notification;

@end

@implementation wxScintillaNotificationDelegate

- (id)initWithSTC:(wxStyledTextCtrl*)stc
{
    if ((self = [super init]))
    {
        m_stc = stc;
    }
    return self;
}

- (void)detach
{
    m_stc = nullptr;
}

- (void)notification:(SCNotification*)notification
{
    if (!m_stc || !notification)
        return;

    m_stc->NotifyParent(notification);

    // Scintilla Cocoa routes text modifications through notification: rather
    // than a separate WM_COMMAND/SCEN_CHANGE callback.
    if (notification->nmhdr.code == SCN_MODIFIED &&
        (notification->modificationType & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT)))
    {
        m_stc->NotifyChange();
    }
}

@end

// ----------------------------------------------------------------------------
// wxWidgetCocoaImpl wrapper around ScintillaView
// ----------------------------------------------------------------------------

namespace
{

class wxScintillaCocoaImpl : public wxWidgetCocoaImpl
{
public:
    wxScintillaCocoaImpl(wxWindowMac* peer, ScintillaView* view)
        : wxWidgetCocoaImpl(peer, view),
          m_delegate([[wxScintillaNotificationDelegate alloc]
                       initWithSTC:static_cast<wxStyledTextCtrl*>(peer)])
    {
        [view setDelegate:m_delegate];
    }

    virtual ~wxScintillaCocoaImpl()
    {
        ScintillaView* view = GetScintillaView();
        if (view)
        {
            [view setDelegate:nil];
        }
        [m_delegate detach];
        [m_delegate release];
    }

    ScintillaView* GetScintillaView() const
    {
        return static_cast<ScintillaView*>(GetWXWidget());
    }

    bool HandleKeyEvent(NSEvent* event)
    {
        return DoHandleKeyEvent(event);
    }

    virtual bool HasFocus() const override
    {
        NSView* view = GetWXWidget();
        if (!view)
            return false;

        NSWindow* window = [view window];
        if (!window)
            return false;

        NSResponder* firstResponder = [window firstResponder];
        if (!firstResponder)
            return false;

        // ScintillaView is a container; its inner SCIContentView becomes firstResponder
        if (firstResponder == view)
            return true;

        if ([firstResponder isKindOfClass:[NSView class]])
        {
            return [static_cast<NSView*>(firstResponder) isDescendantOf:view];
        }

        return false;
    }

    virtual bool SetFocus() override
    {
        ScintillaView* view = GetScintillaView();
        if (!view || ![view window])
            return false;

        // Making ScintillaView firstResponder forwards focus to its inner SCIContentView
        return [[view window] makeFirstResponder:view];
    }

private:
    wxScintillaNotificationDelegate* m_delegate;
};

// ----------------------------------------------------------------------------
// Objective-C runtime hooks for SCIContentView key event forwarding
// ----------------------------------------------------------------------------

IMP s_origKeyDown = nullptr;
IMP s_origKeyUp = nullptr;
IMP s_origFlagsChanged = nullptr;

wxScintillaCocoaImpl* FindScintillaImplFromContentView(NSView* contentView)
{
    for (NSView* v = [contentView superview]; v != nil; v = [v superview])
    {
        if ([v isKindOfClass:[ScintillaView class]])
        {
            return static_cast<wxScintillaCocoaImpl*>(wxWidgetImpl::FindFromWXWidget(v));
        }
    }
    return nullptr;
}

bool ForwardKeyEventToWX(NSView* self, NSEvent* event)
{
    wxScintillaCocoaImpl* impl = FindScintillaImplFromContentView(self);
    if (!impl)
        return false;

    // Converts NSEvent* to wxKeyEvent and dispatches wxEVT_KEY_DOWN / wxEVT_CHAR / wxEVT_KEY_UP.
    // Returns true only if a user handler consumed the event without Skip().
    return impl->HandleKeyEvent(event);
}

void wxSCI_keyDown(id self, SEL _cmd, NSEvent* event)
{
    if (ForwardKeyEventToWX(static_cast<NSView*>(self), event))
        return;

    if (s_origKeyDown)
    {
        reinterpret_cast<void (*)(id, SEL, NSEvent*)>(s_origKeyDown)(self, _cmd, event);
    }
}

void wxSCI_keyUp(id self, SEL _cmd, NSEvent* event)
{
    if (ForwardKeyEventToWX(static_cast<NSView*>(self), event))
        return;

    if (s_origKeyUp)
    {
        reinterpret_cast<void (*)(id, SEL, NSEvent*)>(s_origKeyUp)(self, _cmd, event);
    }
}

void wxSCI_flagsChanged(id self, SEL _cmd, NSEvent* event)
{
    if (ForwardKeyEventToWX(static_cast<NSView*>(self), event))
        return;

    if (s_origFlagsChanged)
    {
        reinterpret_cast<void (*)(id, SEL, NSEvent*)>(s_origFlagsChanged)(self, _cmd, event);
    }
}

void HookSCIContentViewKeyEvents()
{
    static bool s_hooked = false;
    if (s_hooked)
        return;
    s_hooked = true;

    Class contentViewClass = objc_getClass("SCIContentView");
    if (!contentViewClass)
        return;

    Method mKeyDown = class_getInstanceMethod(contentViewClass, @selector(keyDown:));
    if (mKeyDown)
    {
        s_origKeyDown = method_setImplementation(mKeyDown, reinterpret_cast<IMP>(wxSCI_keyDown));
    }

    Method mKeyUp = class_getInstanceMethod(contentViewClass, @selector(keyUp:));
    if (mKeyUp)
    {
        s_origKeyUp = method_setImplementation(mKeyUp, reinterpret_cast<IMP>(wxSCI_keyUp));
    }

    Method mFlagsChanged = class_getInstanceMethod(contentViewClass, @selector(flagsChanged:));
    if (mFlagsChanged)
    {
        s_origFlagsChanged = method_setImplementation(mFlagsChanged, reinterpret_cast<IMP>(wxSCI_flagsChanged));
    }
    else
    {
        // SCIContentView may inherit flagsChanged: from NSResponder rather than implementing it directly
        Method superMethod = class_getInstanceMethod([NSResponder class], @selector(flagsChanged:));
        if (superMethod)
        {
            s_origFlagsChanged = method_getImplementation(superMethod);
            class_addMethod(contentViewClass,
                            @selector(flagsChanged:),
                            reinterpret_cast<IMP>(wxSCI_flagsChanged),
                            method_getTypeEncoding(superMethod));
        }
    }
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxStyledTextCtrl
// ----------------------------------------------------------------------------

bool wxStyledTextCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    HookSCIContentViewKeyEvents();

    DontCreatePeer();

    style &= ~(wxVSCROLL | wxHSCROLL);
    style |= wxWANTS_CHARS | wxCLIP_CHILDREN;

    if (!CreateControl(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    NSRect r = wxOSXGetFrameForControl(this, pos, size);
    ScintillaView* view = [[ScintillaView alloc] initWithFrame:r];
    if (!view)
        return false;

    SetPeer(new wxScintillaCocoaImpl(this, view));

    MacPostControlCreate(pos, size);

    m_sciFn = reinterpret_cast<void*>(
        [view message:SCI_GETDIRECTFUNCTION wParam:0 lParam:0]);
    m_sciPtr = [view message:SCI_GETDIRECTPOINTER wParam:0 lParam:0];

    SetDefaultAttributes();

    return true;
}

wxStyledTextCtrl::~wxStyledTextCtrl()
{
}

#endif // wxUSE_STC
