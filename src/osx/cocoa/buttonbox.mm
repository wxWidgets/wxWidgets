/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/buttonbox.mm
// Purpose:     wxButtonBox implemented with NSSegmentedControl
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BUTTONBOX

#include "wx/buttonbox.h"
#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxButtonBox, wxControl);

// ----------------------------------------------------------------------------
// Objective-C view: wxWidgetCocoaImpl::InstallEventHandler() connects its
// target/action to controlAction:, which calls wxButtonBox::OSXHandleClicked()
// ----------------------------------------------------------------------------

@interface wxNSSegmentedControl : NSSegmentedControl
{
}
@end

@implementation wxNSSegmentedControl

+ (void)initialize
{
    static BOOL initialized = NO;
    if ( !initialized )
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods(self);
    }
}

@end

// ----------------------------------------------------------------------------
// peer
// ----------------------------------------------------------------------------

class wxButtonBoxCocoaImpl : public wxWidgetCocoaImpl, public wxButtonBoxImpl
{
public:
    wxButtonBoxCocoaImpl(wxWindowMac* peer, wxNSSegmentedControl* v)
        : wxWidgetCocoaImpl(peer, v) { }

    NSSegmentedControl* GetSegmentedControl() const
    {
        return (NSSegmentedControl*)GetWXWidget();
    }

    void SetMode(long style) override
    {
        NSSegmentSwitchTracking mode = NSSegmentSwitchTrackingSelectOne;
        if ( style & wxBB_SELECT_ANY )
            mode = NSSegmentSwitchTrackingSelectAny;
        else if ( style & wxBB_MOMENTARY )
            mode = NSSegmentSwitchTrackingMomentary;
        [GetSegmentedControl() setTrackingMode:mode];
    }

    size_t GetCount() const override
    {
        return (size_t)[GetSegmentedControl() segmentCount];
    }

    void Insert(size_t pos, const wxString& label,
                const wxBitmapBundle& bitmap) override
    {
        const NSInteger n = [GetSegmentedControl() segmentCount];
        [GetSegmentedControl() setSegmentCount:n + 1];
        for ( NSInteger i = n; i > (NSInteger)pos; --i )
            CopySegment(i - 1, i);

        ResetSegment(pos);
        SetItemLabel(pos, label);
        SetItemBitmap(pos, bitmap);
    }

    void Delete(size_t pos) override
    {
        const NSInteger n = [GetSegmentedControl() segmentCount];
        for ( NSInteger i = (NSInteger)pos; i < n - 1; ++i )
            CopySegment(i + 1, i);
        [GetSegmentedControl() setSegmentCount:n - 1];
    }

    int GetSelection() const override
    {
        return (int)[GetSegmentedControl() selectedSegment];
    }

    bool IsSelected(size_t n) const override
    {
        return [GetSegmentedControl() isSelectedForSegment:(NSInteger)n];
    }

    void SetSelected(size_t n, bool selected) override
    {
        [GetSegmentedControl() setSelected:selected forSegment:(NSInteger)n];
    }

    void ClearSelection() override
    {
        for ( NSInteger i = 0; i < [GetSegmentedControl() segmentCount]; ++i )
            [GetSegmentedControl() setSelected:NO forSegment:i];
    }

    void SetItemLabel(size_t n, const wxString& label) override
    {
        [GetSegmentedControl() setLabel:wxCFStringRef(label).AsNSString() forSegment:(NSInteger)n];
    }

    wxString GetItemLabel(size_t n) const override
    {
        NSString* s = [GetSegmentedControl() labelForSegment:(NSInteger)n];
        return s ? wxCFStringRef::AsString((CFStringRef)s) : wxString();
    }

    void SetItemBitmap(size_t n, const wxBitmapBundle& bitmap) override
    {
        NSImage* img = nil;
        if ( bitmap.IsOk() )
            img = bitmap.GetBitmapFor(GetWXPeer()).GetNSImage();
        [GetSegmentedControl() setImage:img forSegment:(NSInteger)n];
    }

    void SetItemWidth(size_t n, int width) override
    {
        [GetSegmentedControl() setWidth:(width > 0 ? width : 0) forSegment:(NSInteger)n];
    }

    int GetItemWidth(size_t n) const override
    {
        CGFloat w = [GetSegmentedControl() widthForSegment:(NSInteger)n];
        return w > 0 ? (int)w : wxDefaultCoord;
    }

    void EnableItem(size_t n, bool enable) override
    {
        [GetSegmentedControl() setEnabled:enable forSegment:(NSInteger)n];
    }

    bool IsItemEnabled(size_t n) const override
    {
        return [GetSegmentedControl() isEnabledForSegment:(NSInteger)n];
    }

    // Per-segment tooltips are only supported by macOS 10.13 and later.
    void SetItemToolTip(size_t n, const wxString& tip) override
    {
        if ( WX_IS_MACOS_AVAILABLE(10, 13) )
        {
            [GetSegmentedControl() setToolTip:(tip.empty() ? nil : wxCFStringRef(tip).AsNSString())
                                   forSegment:(NSInteger)n];
        }
    }

    wxString GetItemToolTip(size_t n) const override
    {
        if ( WX_IS_MACOS_AVAILABLE(10, 13) )
        {
            NSString* s = [GetSegmentedControl() toolTipForSegment:(NSInteger)n];
            if ( s )
                return wxCFStringRef::AsString((CFStringRef)s);
        }

        return wxString();
    }

private:
    void ResetSegment(size_t n)
    {
        NSInteger i = (NSInteger)n;
        [GetSegmentedControl() setLabel:@"" forSegment:i];
        [GetSegmentedControl() setImage:nil forSegment:i];
        [GetSegmentedControl() setWidth:0 forSegment:i];
        [GetSegmentedControl() setEnabled:YES forSegment:i];
        if ( WX_IS_MACOS_AVAILABLE(10, 13) )
            [GetSegmentedControl() setToolTip:nil forSegment:i];
        [GetSegmentedControl() setSelected:NO forSegment:i];
    }

    void CopySegment(NSInteger from, NSInteger to)
    {
        NSSegmentedControl* s = GetSegmentedControl();
        [s setLabel:[s labelForSegment:from] ?: @"" forSegment:to];
        [s setImage:[s imageForSegment:from] forSegment:to];
        [s setWidth:[s widthForSegment:from] forSegment:to];
        [s setEnabled:[s isEnabledForSegment:from] forSegment:to];
        if ( WX_IS_MACOS_AVAILABLE(10, 13) )
            [s setToolTip:[s toolTipForSegment:from] forSegment:to];
        [s setSelected:[s isSelectedForSegment:from] forSegment:to];
    }
};

static wxWidgetImplType* CreateButtonBoxImpl(wxWindowMac* wxpeer,
                                             wxWindowMac* /* parent */,
                                             wxWindowID /* id */,
                                             const wxPoint& pos,
                                             const wxSize& size,
                                             long style)
{
    NSRect r = wxOSXGetFrameForControl(wxpeer, pos, size);
    wxNSSegmentedControl* v = [[wxNSSegmentedControl alloc] initWithFrame:r];
    [v setSegmentCount:0];

    wxButtonBoxCocoaImpl* c = new wxButtonBoxCocoaImpl(wxpeer, v);
    c->SetMode(style);
    return c;
}

// ----------------------------------------------------------------------------
// wxButtonBox
// ----------------------------------------------------------------------------

bool wxButtonBox::Create(wxWindow* parent, wxWindowID id,
                         const wxPoint& pos, const wxSize& size,
                         long style, const wxString& name)
{
    DontCreatePeer();

    if ( !wxControl::Create(parent, id, pos, size, style,
                            wxDefaultValidator, name) )
        return false;

    SetPeer(CreateButtonBoxImpl(this, parent, id, pos, size, style));
    MacPostControlCreate(pos, size);
    return true;
}

wxButtonBoxImpl* wxButtonBox::GetButtonBoxPeer() const
{
    return dynamic_cast<wxButtonBoxImpl*>(GetPeer());
}

size_t wxButtonBox::GetCount() const
{
    return GetButtonBoxPeer()->GetCount();
}

void wxButtonBox::Insert(size_t pos, const wxString& label,
                         const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( pos <= GetCount(), "invalid segment position" );
    m_bitmaps.insert(m_bitmaps.begin() + pos, bitmap);
    GetButtonBoxPeer()->Insert(pos, label, bitmap);
    InvalidateBestSize();
}

void wxButtonBox::Delete(size_t pos)
{
    wxCHECK_RET( pos < GetCount(), "invalid segment index" );
    m_bitmaps.erase(m_bitmaps.begin() + pos);
    GetButtonBoxPeer()->Delete(pos);
    InvalidateBestSize();
}

void wxButtonBox::Clear()
{
    while ( GetCount() )
        Delete(GetCount() - 1);
}

int wxButtonBox::GetSelection() const
{
    return HasFlag(wxBB_MOMENTARY) ? wxNOT_FOUND : GetButtonBoxPeer()->GetSelection();
}

void wxButtonBox::SetSelection(int n)
{
    if ( HasFlag(wxBB_MOMENTARY) )
        return;

    if ( n == wxNOT_FOUND )
        GetButtonBoxPeer()->ClearSelection();
    else
        SetSelected((size_t)n, true);
}

bool wxButtonBox::IsSelected(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid segment index" );
    return GetButtonBoxPeer()->IsSelected(n);
}

void wxButtonBox::SetSelected(size_t n, bool selected)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    if ( HasFlag(wxBB_MOMENTARY) )
        return;
    GetButtonBoxPeer()->SetSelected(n, selected);
}

void wxButtonBox::SetItemLabel(size_t n, const wxString& label)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->SetItemLabel(n, label);
    InvalidateBestSize();
}

wxString wxButtonBox::GetItemLabel(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "invalid segment index" );
    return GetButtonBoxPeer()->GetItemLabel(n);
}

void wxButtonBox::SetItemBitmap(size_t n, const wxBitmapBundle& bitmap)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    m_bitmaps[n] = bitmap;
    GetButtonBoxPeer()->SetItemBitmap(n, bitmap);
    InvalidateBestSize();
}

wxBitmapBundle wxButtonBox::GetItemBitmap(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxBitmapBundle(), "invalid segment index" );
    return m_bitmaps[n];
}

void wxButtonBox::SetItemWidth(size_t n, int width)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->SetItemWidth(n, width);
    InvalidateBestSize();
}

int wxButtonBox::GetItemWidth(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxDefaultCoord, "invalid segment index" );
    return GetButtonBoxPeer()->GetItemWidth(n);
}

void wxButtonBox::EnableItem(size_t n, bool enable)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->EnableItem(n, enable);
}

bool wxButtonBox::IsItemEnabled(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid segment index" );
    return GetButtonBoxPeer()->IsItemEnabled(n);
}

void wxButtonBox::SetItemToolTip(size_t n, const wxString& tip)
{
    wxCHECK_RET( n < GetCount(), "invalid segment index" );
    GetButtonBoxPeer()->SetItemToolTip(n, tip);
}

wxString wxButtonBox::GetItemToolTip(size_t n) const
{
    wxCHECK_MSG( n < GetCount(), wxString(), "invalid segment index" );
    return GetButtonBoxPeer()->GetItemToolTip(n);
}

// For all three tracking modes NSSegmentedControl's selectedSegment is the
// segment that was just clicked, which is what the event reports.
bool wxButtonBox::OSXHandleClicked(double /* timestampsec */)
{
    wxButtonBoxImpl* const peer = GetButtonBoxPeer();

    const int n = peer->GetSelection();
    if ( n < 0 )
        return false;

    const bool selected = HasFlag(wxBB_MOMENTARY) ? true
                                                  : peer->IsSelected((size_t)n);
    SendButtonBoxEvent(n, selected);
    return true;
}

#endif // wxUSE_BUTTONBOX
