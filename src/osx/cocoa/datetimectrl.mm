/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/datetimectrl.mm
// Purpose:     Implementation of wxDateTimePickerCtrl for Cocoa.
// Author:      Vadim Zeitlin
// Created:     2011-12-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_DATEPICKCTRL

#include "wx/datetimectrl.h"
#include "wx/datectrl.h"

#include "wx/osx/core/private/datetimectrl.h"
#include "wx/osx/cocoa/private/date.h"

using namespace wxOSXImpl;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Cocoa wrappers
// ----------------------------------------------------------------------------

@interface wxNSDatePicker : NSDatePicker
{
}

@end

@implementation wxNSDatePicker

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

@end

// ----------------------------------------------------------------------------
// Peer-specific subclass
// ----------------------------------------------------------------------------

namespace
{

class wxDateTimeWidgetCocoaImpl : public wxDateTimeWidgetImpl
{
public:
    wxDateTimeWidgetCocoaImpl(wxDateTimePickerCtrl* peer, wxNSDatePicker* w)
        : wxDateTimeWidgetImpl(peer, w)
    {
    }

    virtual void SetDateTime(const wxDateTime& dt) wxOVERRIDE
    {
        wxDateTime dtFrom, dtTo;
        
        if ( GetDateRange(&dtFrom,&dtTo) == false ||
            ( (!dtFrom.IsValid() || dtFrom <= dt) &&
             (!dtTo.IsValid() || dt <= dtTo ) ) )
            [View() setDateValue: NSDateFromWX(dt)];
    }

    virtual wxDateTime GetDateTime() const wxOVERRIDE
    {
        return NSDateToWX([View() dateValue]);
    }

    virtual void SetDateRange(const wxDateTime& dt1, const wxDateTime& dt2) wxOVERRIDE
    {
        // Note that passing nil is ok here so we don't need to test for the
        // dates validity.
        [View() setMinDate: NSDateFromWX(dt1)];
        [View() setMaxDate: NSDateFromWX(dt2)];
    }

    virtual bool GetDateRange(wxDateTime* dt1, wxDateTime* dt2) wxOVERRIDE
    {
        bool hasLimits = false;
        if ( dt1 )
        {
            *dt1 = NSDateToWX([View() minDate]);
            hasLimits = true;
        }

        if ( dt2 )
        {
            *dt2 = NSDateToWX([View() maxDate]);
            hasLimits = true;
        }

        return hasLimits;
    }

    virtual void controlAction(WXWidget WXUNUSED(slf),
                               void* WXUNUSED(cmd),
                               void* WXUNUSED(sender)) wxOVERRIDE
    {
        wxWindow* const wxpeer = GetWXPeer();
        if ( wxpeer )
        {
            static_cast<wxDateTimePickerCtrl*>(wxpeer)->
                OSXGenerateEvent(GetDateTime());
        }
    }

    virtual void Enable(bool enable = true) wxOVERRIDE
    {
        wxNSDatePicker* const nsdatePicker = View();

        [nsdatePicker setEnabled: enable];

        if ( enable )
        {
            wxWindow* const wxpeer = GetWXPeer();
            if ( wxpeer )
                [nsdatePicker setTextColor: wxpeer->GetForegroundColour().OSXGetNSColor()];
            else
                [nsdatePicker setTextColor: [NSColor controlTextColor]];
        }
        else
        {
            [nsdatePicker setTextColor: [NSColor disabledControlTextColor]];
        }
    }

private:
    wxNSDatePicker* View() const
    {
        return static_cast<wxNSDatePicker *>(m_osxView);
    }
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// CreateDateTimePicker() implementation
// ----------------------------------------------------------------------------

/* static */
wxDateTimeWidgetImpl*
wxDateTimeWidgetImpl::CreateDateTimePicker(wxDateTimePickerCtrl* wxpeer,
                                           const wxDateTime& dt,
                                           const wxPoint& pos,
                                           const wxSize& size,
                                           long WXUNUSED(style),
                                           wxDateTimeWidgetKind kind)
{
    NSRect r = wxOSXGetFrameForControl(wxpeer, pos, size);
    wxNSDatePicker* v = [[wxNSDatePicker alloc] initWithFrame:r];

    NSDatePickerElementFlags elements = 0;
    switch ( kind )
    {
        case wxDateTimeWidget_YearMonthDay:
            elements = NSYearMonthDayDatePickerElementFlag;
            break;

        case wxDateTimeWidget_HourMinuteSecond:
            elements = NSHourMinuteSecondDatePickerElementFlag;
            break;
    }

    wxASSERT_MSG( elements, "Unknown date time widget kind" );
    [v setDatePickerElements: elements];

    [v setDatePickerStyle: NSTextFieldAndStepperDatePickerStyle];

    // Avoid a disabled looking transparent background for the text cells.
    [v setDrawsBackground: YES];

    if ( dt.IsValid() )
    {
        [v setDateValue: NSDateFromWX(dt)];
    }
    else
    {
        [v setDateValue: [NSDate date]];
    }

    wxDateTimeWidgetImpl* c = new wxDateTimeWidgetCocoaImpl(wxpeer, v);
#if !wxOSX_USE_NATIVE_FLIPPED
    c->SetFlipped(false);
#endif
    return c;
}

#endif // wxUSE_DATEPICKCTRL
