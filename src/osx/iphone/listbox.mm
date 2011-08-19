///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/listbox.mm
// Purpose:     wxListBox
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-04
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"
#include "wx/dnd.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/arrstr.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private.h"


#pragma mark wxUIPickerView

class wxListWidgetIPhoneImpl;

@interface wxUIPickerView : UIPickerView <UIPickerViewDataSource, UIPickerViewDelegate>
{
@private
    wxListWidgetIPhoneImpl* implementation;
}

@property (nonatomic) wxListWidgetIPhoneImpl* implementation;

@end


#pragma mark wx

class wxListWidgetIPhoneImpl : public wxWidgetIPhoneImpl, public wxListWidgetImpl
{
    
public:
    wxListWidgetIPhoneImpl(wxWindowMac* peer, wxUIPickerView *w)  : wxWidgetIPhoneImpl(peer, w)
    {
        m_pickerView = w;
    }

    ~wxListWidgetIPhoneImpl()
    {
        
    }
    
    wxListWidgetColumn* InsertTextColumn(unsigned pos,
                                         const wxString& title,
                                         bool editable = false,
                                         wxAlignment just = wxALIGN_LEFT,
                                         int defaultWidth = -1)
    {
        return NULL;
    }
    
    wxListWidgetColumn* InsertCheckColumn(unsigned pos,
                                          const wxString& title,
                                          bool editable = false,
                                          wxAlignment just = wxALIGN_LEFT,
                                          int defaultWidth =  -1)
    {
        return NULL;
    }
    
    // add and remove
    void ListDelete( unsigned int n )
    {
        [m_pickerView reloadAllComponents];
    }
    
    void ListInsert( unsigned int n )
    {
        [m_pickerView reloadAllComponents];
    }
    
    void ListClear()
    {
        [m_pickerView reloadAllComponents];
    }
    
    // selecting
    void ListDeselectAll()
    {
        [m_pickerView selectRow:-1
                    inComponent:0
                       animated:NO];
    }
    
    void ListSetSelection( unsigned int n, bool select, bool multi )
    {
        if (select) {
            [m_pickerView selectRow:n
                        inComponent:0
                           animated:NO];
        } else {
            ListDeselectAll();
        }
    }
    
    int ListGetSelection() const
    {
        return [m_pickerView selectedRowInComponent:0];
    }
    
    int ListGetSelections( wxArrayInt& aSelections ) const
    {
        aSelections.Empty();
        
        int count = ListGetCount();
        int selectedRow = [m_pickerView selectedRowInComponent:0];
        
        for (int i = 0; i < count; ++i) {
            if (i == selectedRow) {
                aSelections.Add(i);
            }
        }
        
        return aSelections.Count();
    }
    
    bool ListIsSelected( unsigned int n ) const
    {
        return [m_pickerView selectedRowInComponent:0] == n;
    }
    
    // display
    void ListScrollTo( unsigned int n )
    {
        ListSetSelection(n, true, false);
    }
    
    // accessing content
    unsigned int ListGetCount() const
    {
        return [m_pickerView numberOfRowsInComponent:0];
    }
    
    int DoListHitTest( const wxPoint& inpoint ) const
    {
        return 0;
    }
    
    int ListGetColumnType( int col )
    {
        return col;
    }
    
    void UpdateLine( unsigned int n, wxListWidgetColumn* col = NULL )
    {
        [m_pickerView reloadAllComponents];
    }
    
    void UpdateLineToEnd( unsigned int n)
    {
        [m_pickerView reloadAllComponents];
    }
    
    void controlDoubleAction(WXWidget slf, void* _cmd, void* sender)
    {
        wxListBox* list = static_cast<wxListBox*> ( GetWXPeer());
        wxCHECK_RET( list != NULL , wxT("ListBox expected"));
        
        int sel = [m_pickerView selectedRowInComponent:0];
        if ((sel < 0) || (sel > (int) list->GetCount())) {  // OS X can select an item below the last item (why?)
            return;
        }
        
        list->HandleLineEvent( sel, true );        
    }
    
protected:
    wxUIPickerView*          m_pickerView ;
};


@implementation wxUIPickerView

@synthesize implementation;


- (id)init {
    if ((self = [super init])) {
        implementation = NULL;
    }
    
    return self;
}

#pragma mark UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    if (! implementation) {
        return 0;
    }
    
    wxListBox* lb = dynamic_cast<wxListBox*> ( implementation->GetWXPeer() );
    unsigned int count = lb->GetCount();
    
    return count;
}

#pragma mark UIPickerViewDelegate

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    if (! implementation) {
        return;
    }
    
    wxListBox* lb = dynamic_cast<wxListBox*> ( implementation->GetWXPeer() );
    
    if ((row < 0) || (row > (int) lb->GetCount())) {
        return;
    }
    
    wxCommandEvent event( wxEVT_COMMAND_LISTBOX_SELECTED, lb->GetId() );
    lb->GetEventHandler()->ProcessEvent(event);
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    
    if (! implementation) {
        return @"";
    }
    
    wxListBox* lb = dynamic_cast<wxListBox*> ( implementation->GetWXPeer() );
    wxString title = lb->GetString(row);
    
    if (! title) {
        return @"";
    }
    
    return [NSString stringWithString:wxCFStringRef(title).AsNSString()];
}

@end


#pragma mark Peer creation

wxWidgetImplType* wxWidgetImpl::CreateListBox(wxWindowMac* wxpeer,
                                              wxWindowMac* WXUNUSED(parent),
                                              wxWindowID WXUNUSED(id),
                                              const wxPoint& pos,
                                              const wxSize& size,
                                              long style,
                                              long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUIPickerView *v = [[wxUIPickerView alloc] init];
    [v setFrame:r];
    [v setDataSource:v];
    [v setDelegate:v];
    
    wxListWidgetIPhoneImpl* c = new wxListWidgetIPhoneImpl(wxpeer, v);
    [v setImplementation:c];

    return c;
}

#endif // wxUSE_LISTBOX
