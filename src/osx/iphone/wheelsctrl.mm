/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/wheelsctrl.mm
// Purpose:     wxWheelsCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"

#include "wx/wheelsctrl.h"
#include "wx/osx/private.h"
#include "wx/osx/iphone/private.h"


#pragma mark -
#pragma mark Cocoa classes

#pragma mark wxWheelsUIView (holds either UIPickerView or UIDatePicker)

@interface wxWheelsUIView : UIView <UIPickerViewDataSource, UIPickerViewDelegate>
{
    UIPickerView *pickerView;
    UIDatePicker *datePickerView;
    
    @private
    
    wxWheelsCtrl* wheelsCtrl;
    wxWheelsDataSource* wxDataSource;
}

@property (nonatomic) wxWheelsDataSource *wxDataSource;

- (id)initWithWheelsCtrl:(wxWheelsCtrl *)initWheelsCtrl;

// Get selected row for component
- (NSInteger)selectedRowInComponent:(NSInteger)component;

// Select row in component
- (BOOL)selectRow:(NSInteger)row inComponent:(NSInteger)component;

// Reload component
- (BOOL)reloadComponent:(NSInteger)component;

// Reload all components
- (BOOL)reloadAllComponents;

// Reinitializes actual picker subview (either date or "normal")
- (void)reinitializePickerSubview;

// Target action for date picker
- (void)datePickerValueChanged:(id)sender;

// Passes common UIView getters to picker view
- (void)setFrame:(CGRect)frame;
- (CGSize)sizeThatFits:(CGSize)size;
- (void)sizeToFit;

@end

@implementation wxWheelsUIView

@synthesize wxDataSource;


- (id)init {
    if ((self = [super init])) {
        wheelsCtrl = NULL;
        wxDataSource = NULL;
    }
    
    return self;
}

- (id)initWithWheelsCtrl:(wxWheelsCtrl *)initWheelsCtrl {
    if ((self = [self init])) {
        wheelsCtrl = initWheelsCtrl;
    }
    
    return self;
}

- (void)dealloc {
    if (pickerView) {
        [pickerView release];
        pickerView = nil;
    }
    if (datePickerView) {
        [datePickerView release];
        datePickerView = nil;
    }
    
    [super dealloc];
}

- (void)setFrame:(CGRect)frame {
    if (pickerView) {
        // FIXME avoid setting size
        // (apparently, UIPickerView does not support custom width+height since iOS 4:
        // http://stackoverflow.com/questions/573979/how-to-change-uipickerview-height )
        CGRect currentFrame = pickerView.frame;
        currentFrame.size = frame.size;
        [pickerView setFrame:currentFrame];
    }
    if (datePickerView) {
        CGRect currentFrame = datePickerView.frame;
        currentFrame.size = frame.size;
        [datePickerView setFrame:currentFrame];
    }
    [super setFrame:frame];
}

- (CGSize)sizeThatFits:(CGSize)size {
    if (pickerView) {
        return [pickerView sizeThatFits:size];
    }
    if (datePickerView) {
        return [datePickerView sizeThatFits:size];
    }
}

- (void)sizeToFit {
    if (pickerView) {
        [pickerView sizeToFit];
    }
    if (datePickerView) {
        [datePickerView sizeToFit];
    }
    [super sizeToFit];
}

- (NSInteger)selectedRowInComponent:(NSInteger)component {
    if (pickerView) {
        return [pickerView selectedRowInComponent:component];
    } else if (datePickerView) {
        return -1;
    } else {
        return -1;
    }
}

- (BOOL)selectRow:(NSInteger)row inComponent:(NSInteger)component {
    if (pickerView) {
        [pickerView selectRow:row
                  inComponent:component
                     animated:NO];
        return true;
    } else if (datePickerView) {
        return false;
    } else {
        return false;
    }
}

- (BOOL)reloadComponent:(NSInteger)component {
    if (pickerView) {
        [pickerView reloadComponent:component];
        return true;
    } else if (datePickerView) {
        return false;
    } else {
        return false;
    }
}

- (BOOL)reloadAllComponents {
    if (pickerView) {
        [pickerView reloadAllComponents];
        return true;
    } else if (datePickerView) {
        return false;
    } else {
        return false;
    }
}

- (void)setWxDataSource:(wxWheelsDataSource *)newWxDataSource {
    if (wxDataSource != newWxDataSource) {        
        wxDataSource = newWxDataSource;
        
        [self reinitializePickerSubview];
    }
}

- (void)reinitializePickerSubview {
    
    // Choose picker
    if (wxDataSource->IsKindOf(wxCLASSINFO(wxWheelsDateTimeDataSource))) {
        
        //
        // UIDatePicker
        //
        
        if (pickerView) {
            [pickerView removeFromSuperview];
            [pickerView release];
            pickerView = nil;
        }
        if (!datePickerView) {
            // initWithFrame: might not work: http://stackoverflow.com/questions/573979/how-to-change-uipickerview-height/6781755#6781755
            datePickerView = [[UIDatePicker alloc] init];
            [datePickerView addTarget:self
                               action:@selector(datePickerValueChanged:)
                     forControlEvents:UIControlEventValueChanged];
            [self addSubview:datePickerView];
        }
        
        wxWheelsDateTimeDataSource* dateDataSource = (wxWheelsDateTimeDataSource *)wxDataSource;
        wxASSERT_MSG(dateDataSource, "Date data source is not set");
        
        // Set up picker
        UIDatePickerMode cocoaMode;
        int mode = dateDataSource->GetPickerMode();
        switch (mode) {
            case wxWHEELS_DT_DATE_TIME: cocoaMode = UIDatePickerModeDateAndTime;    break;
            case wxWHEELS_DT_DATE:      cocoaMode = UIDatePickerModeDate;           break;
            case wxWHEELS_DT_TIME:      cocoaMode = UIDatePickerModeTime;           break;
            case wxWHEELS_DT_COUNTDOWN: cocoaMode = UIDatePickerModeCountDownTimer; break;
            default:                    cocoaMode = UIDatePickerModeDateAndTime;    break;
        };
        [datePickerView setDatePickerMode:cocoaMode];
        
        wxDateTime* initialDate = dateDataSource->GetInitialDate();
        NSDate *cocoaInitialDate = nil;
        if (initialDate) {
            cocoaInitialDate = [NSDate dateWithTimeIntervalSince1970:initialDate->GetTicks()];
        } else {
            cocoaInitialDate = [NSDate date];
        }
        [datePickerView setDate:cocoaInitialDate
                       animated:NO];
        
        wxDateTime* minDate = dateDataSource->GetMinimumPickerDate();
        NSDate *cocoaMinDate = nil;
        if (minDate) {
            cocoaMinDate = [NSDate dateWithTimeIntervalSince1970:minDate->GetTicks()];
        }
        [datePickerView setMinimumDate:cocoaMinDate];
        
        wxDateTime* maxDate = dateDataSource->GetMaximumPickerDate();
        NSDate *cocoaMaxDate = nil;
        if (maxDate) {
            cocoaMaxDate = [NSDate dateWithTimeIntervalSince1970:maxDate->GetTicks()];
        }
        [datePickerView setMaximumDate:cocoaMaxDate];
        
        int minuteInterval = dateDataSource->GetMinuteInterval();
        [datePickerView setMinuteInterval:minuteInterval];
        
        double countDownDuration = dateDataSource->GetCountDownDuration();
        [datePickerView setCountDownDuration:countDownDuration];
                
    } else {
        
        //
        // UIPickerView
        //
        
        if (datePickerView) {
            [datePickerView removeFromSuperview];
            [datePickerView release];
            datePickerView = nil;
        }
        if (!pickerView) {
            pickerView = [[UIPickerView alloc] initWithFrame:self.frame];
            [pickerView setDelegate:self];
            [pickerView setDataSource:self];
            [self addSubview:pickerView];
        }
    }
    
}

#pragma mark UIPickerViewDataSource / UIPickerViewDelegate

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    if (! wxDataSource) {
        return 0;
    }
    
    return wxDataSource->GetNumberOfComponents(wheelsCtrl);
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    if (! wxDataSource) {
        return 0;
    }
    
    return wxDataSource->GetNumberOfRowsForComponent(wheelsCtrl, component);
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    
    if (! wxDataSource) {
        return nil;
    }
    
    wxString title = wxDataSource->GetRowTitle(wheelsCtrl, component, row);
    if (! title) {
        return nil;
    }
    
    return [NSString stringWithString:wxCFStringRef(title).AsNSString()];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    
    if (! wxDataSource) {
        return;
    }
    
    if ((row < 0)
        ||
        (component > (int) wxDataSource->GetNumberOfComponents(wheelsCtrl))
        ||
        (row > (int) wxDataSource->GetNumberOfRowsForComponent(wheelsCtrl, component))) {
        
        return;
    }
    
    // Inform data source
    wxDataSource->OnSelectRow(wheelsCtrl, component, row);

    // Send event
    wxWheelsCtrlEvent event(wxEVT_COMMAND_WHEEL_SELECTED, wheelsCtrl->GetId(), component, row);
    wheelsCtrl->GetEventHandler()->ProcessEvent(event);
}

#pragma mark UIDatePicker target action

- (void)datePickerValueChanged:(id)sender {
    
    if (! wxDataSource) {
        return;
    }
    
    // Inform data source
    wxWheelsDateTimeDataSource* dateDataSource = (wxWheelsDateTimeDataSource *)wxDataSource;
    wxDateTime* date = new wxDateTime([[datePickerView date] timeIntervalSince1970]);
    dateDataSource->OnSelectDate(wheelsCtrl, date);

    // Send event
    wxWheelsCtrlEvent event(wxEVT_COMMAND_WHEEL_SELECTED, wheelsCtrl->GetId(), -1, -1);
    wheelsCtrl->GetEventHandler()->ProcessEvent(event);
}

@end


#pragma mark -
#pragma mark Peer implementation

class wxWheelsCtrlWidgetIPhoneImpl : public wxWidgetIPhoneImpl {
  
public:
    wxWheelsCtrlWidgetIPhoneImpl(wxWindowMac* peer, wxWheelsUIView *w)  : wxWidgetIPhoneImpl(peer, w) {
        m_wheelsUIView = w;
    }
    
    bool SetDataSource(wxWheelsDataSource* dataSource) {
        [m_wheelsUIView setWxDataSource:dataSource];
        return true;
    }
    
    int GetSelection(int component) {
        return [m_wheelsUIView selectedRowInComponent:component];
    }
    
    bool SetSelection(int component, int row) {
        return [m_wheelsUIView selectRow:row
                             inComponent:component];
    }
    
    bool ReloadComponent(int component) {
        return [m_wheelsUIView reloadComponent:component];
    }
    
    bool ReloadAllComponents() {
        return [m_wheelsUIView reloadAllComponents];
    }
    
private:
    wxWheelsUIView* m_wheelsUIView;
    
};

wxWidgetImplType* wxWidgetImpl::CreateWheelsCtrl(wxWindowMac* wxpeer,
                                                 wxWindowMac* WXUNUSED(parent),
                                                 wxWindowID WXUNUSED(id),
                                                 const wxPoint& pos,
                                                 const wxSize& size,
                                                 long style,
                                                 long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxWheelsUIView *v = [[wxWheelsUIView alloc] initWithWheelsCtrl:(wxWheelsCtrl *)wxpeer];
    [v setFrame:r];
    
    wxWheelsCtrlWidgetIPhoneImpl* c = new wxWheelsCtrlWidgetIPhoneImpl(wxpeer, v);
    
    return c;
}


#pragma mark -
#pragma mark wxWheelsCtrl implementation

extern WXDLLEXPORT_DATA(const wxChar) wxWheelsCtrlNameStr[] = wxT("wheelsctrl");

IMPLEMENT_DYNAMIC_CLASS(wxWheelsCtrl, wxWheelsCtrlBase)

BEGIN_EVENT_TABLE(wxWheelsCtrl, wxWheelsCtrlBase)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

/// Default constructor.
wxWheelsCtrl::wxWheelsCtrl()
{
    Init();
}

/// Constructor.
wxWheelsCtrl::wxWheelsCtrl(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& WXUNUSED(size),    // Custom UIPickerView size is not allowed
                           long style,
                           const wxValidator& validator,
                           const wxString& name)
{
    Init();
    Create(parent, id, pos, wxDefaultSize, style, validator, name);
}

bool wxWheelsCtrl::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& WXUNUSED(size),     // Custom UIPickerView size is not allowed
                          long style,
                          const wxValidator& validator,
                          const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxControl::Create( parent, id, pos, wxDefaultSize, style, validator, name )) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateWheelsCtrl( this, parent, id, pos, wxDefaultSize, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, wxDefaultSize );
    
    return true;    
}

wxWheelsCtrl::~wxWheelsCtrl()
{
    if (m_dataSource && m_ownDataSource) {
        delete m_dataSource;
        m_dataSource = NULL;
    }
}

void wxWheelsCtrl::Init()
{
    m_showRowSelection = true;
    m_dataSource = NULL;
    m_ownDataSource = false;
}

// Reloads and displays all components.
bool wxWheelsCtrl::ReloadAllComponents()
{
    wxWheelsCtrlWidgetIPhoneImpl* peer = (wxWheelsCtrlWidgetIPhoneImpl *)GetPeer();
    peer->ReloadAllComponents();
}

// Reloads and displays the specified component.
bool wxWheelsCtrl::ReloadComponent(int component)
{
    wxWheelsCtrlWidgetIPhoneImpl* peer = (wxWheelsCtrlWidgetIPhoneImpl *)GetPeer();
    peer->ReloadComponent(component);
}

// Sets the selection (row) in the given component.
void wxWheelsCtrl::SetSelection(int component, int row)
{
    wxWheelsCtrlWidgetIPhoneImpl* peer = (wxWheelsCtrlWidgetIPhoneImpl *)GetPeer();
    peer->SetSelection(component, row);
}

// Gets the selection for the given component.
int wxWheelsCtrl::GetSelection(int component) const
{
    wxWheelsCtrlWidgetIPhoneImpl* peer = (wxWheelsCtrlWidgetIPhoneImpl *)GetPeer();
    return peer->GetSelection(component);
}

// Returns the number of rows in the given component.
int wxWheelsCtrl::GetComponentRowCount(int component) const
{
    if (GetDataSource()) {
        int rows = GetDataSource()->GetNumberOfRowsForComponent(const_cast<wxWheelsCtrl*>(this), component);
        return rows;
    } else {
        return 0;
    }
}

// Returns the number of components.
int wxWheelsCtrl::GetComponentCount() const
{
    if (GetDataSource()) {
        int n = GetDataSource()->GetNumberOfComponents(const_cast<wxWheelsCtrl*>(this));
        return n;
    } else {
        return 0;
    }
}

// Sets the data source object, recreating the components.
void wxWheelsCtrl::SetDataSource(wxWheelsDataSource* dataSource,
                                 bool ownsDataSource)
{
    if (m_dataSource && m_dataSource != dataSource && m_ownDataSource) {
        delete m_dataSource;
    }
    
    wxWheelsCtrlWidgetIPhoneImpl* peer = (wxWheelsCtrlWidgetIPhoneImpl *)GetPeer();
    peer->SetDataSource(dataSource);
    
    m_ownDataSource = ownsDataSource;
    m_dataSource = dataSource;
}
