/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/combobox.mm
// Purpose:     wxComboBox
// Author:      Ryan Norton
// Modified by:
// Created:     2005/02/16
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

//
// Impl notes:
// There is no custom data source because doing so unnecessarily sacrifices
// some native autocompletion behavior (we would have to make our own -
// the SimpleComboBox sample does so in the developer folder that
// comes with OSX).  One reason you might want this would be to have
// only one array or be able to display numbers returned by an NSNumber
// from the methods.
//
// One problem though is that wxCB_SORT isn't implemented...
//
// Also, not sure if it is correctly getting text field events since
// I'm using SetNSComboBox instead of SetNSTextField
//
// doWxEvent is really hackish... but since there's only one event...
//
// Ideas for future improvement - other notes:
// Combox w/o wxCB_DROPDOWN doesn't seem to be implementable
//wxCB_READONLY  Same as wxCB_DROPDOWN but only the strings specified as the combobox choices can be selected, it is impossible to select (even from a program) a string which is not in the choices list.
//wxCB_SORT      is possible with data source
//
// setIntercellSpacing:/setItemHeight: to autoadjust to number of inserted items?
//
/*
    //example of autocompletion from SimpleComboBox Example
    // ==========================================================
// Combo box data source methods
// ==========================================================

- (int)numberOfItemsInComboBox:(NSComboBox *)aComboBox {
    return [genres count];
}
- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(int)index {
    return [genres objectAtIndex:index];
}
- (unsigned int)comboBox:(NSComboBox *)aComboBox indexOfItemWithStringValue:(NSString *)string {
    return [genres indexOfObject: string];
}

- (NSString *) firstGenreMatchingPrefix:(NSString *)prefix {
    NSString *string = nil;
    NSString *lowercasePrefix = [prefix lowercaseString];
    NSEnumerator *stringEnum = [genres objectEnumerator];
    while ((string = [stringEnum nextObject])) {
        if ([[string lowercaseString] hasPrefix: lowercasePrefix]) return string;
    }
    return nil;
}

- (NSString *)comboBox:(NSComboBox *)aComboBox completedString:(NSString *)inputString {
    // This method is received after each character typed by the user, because we have checked the "completes" flag for genreComboBox in IB.
    // Given the inputString the user has typed, see if we can find a genre with the prefix, and return it as the suggested complete string.
    NSString *candidate = [self firstGenreMatchingPrefix: inputString];
    return (candidate ? candidate : inputString);
}
*/

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif // WX_PRECOMP

#import <AppKit/NSComboBox.h>
#import <Foundation/NSNotification.h>
#import <Foundation/NSString.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------
WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSComboBox)

void wxCocoaNSComboBox::AssociateNSComboBox(WX_NSComboBox cocoaNSComboBox)
{
    if(cocoaNSComboBox)
    {
        sm_cocoaHash.insert(wxCocoaNSComboBoxHash::value_type(cocoaNSComboBox,this));

        [[NSNotificationCenter defaultCenter] addObserver:(id)cocoaNSComboBox selector:@selector(comboBoxSelectionDidChange:) name:@"NSComboBoxSelectionDidChangeNotification" object:cocoaNSComboBox];
        [[NSNotificationCenter defaultCenter] addObserver:(id)cocoaNSComboBox selector:@selector(comboBoxSelectionDidChange:) name:@"NSComboBoxSelectionIsChangingNotification" object:cocoaNSComboBox];
        [[NSNotificationCenter defaultCenter] addObserver:(id)cocoaNSComboBox selector:@selector(comboBoxSelectionDidChange:) name:@"NSComboBoxWillDismissNotification" object:cocoaNSComboBox];
        [[NSNotificationCenter defaultCenter] addObserver:(id)cocoaNSComboBox selector:@selector(comboBoxSelectionDidChange:) name:@"NSComboBoxWillPopUpNotification" object:cocoaNSComboBox];
    }
}

void wxCocoaNSComboBox::DisassociateNSComboBox(WX_NSComboBox cocoaNSComboBox)
{
    if(cocoaNSComboBox)
    {
        sm_cocoaHash.erase(cocoaNSComboBox);
        [[NSNotificationCenter defaultCenter] removeObserver:(id)cocoaNSComboBox name:@"NSComboBoxSelectionDidChangeNotification" object:cocoaNSComboBox];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)cocoaNSComboBox name:@"NSComboBoxSelectionIsChangingNotification" object:cocoaNSComboBox];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)cocoaNSComboBox name:@"NSComboBoxWillDismissNotification" object:cocoaNSComboBox];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)cocoaNSComboBox name:@"NSComboBoxWillPopUpNotification" object:cocoaNSComboBox];
    }
}

// ============================================================================
// @class wxPoserNSComboBox
// ============================================================================
@interface wxPoserNSComboBox : NSComboBox
{
}

- (void)comboBoxSelectionDidChange:(NSNotification *)notification;
- (void)comboBoxSelectionIsChanging:(NSNotification *)notification;
- (void)comboBoxWillDismiss:(NSNotification *)notification;
- (void)comboBoxWillPopUp:(NSNotification *)notification;
@end // wxPoserNSComboBox

//WX_IMPLEMENT_POSER(wxPoserNSComboBox);
@implementation wxPoserNSComboBox : NSComboBox

- (void)comboBoxSelectionDidChange:(NSNotification *)notification
{
    wxCocoaNSComboBox *win = wxCocoaNSComboBox::GetFromCocoa(self);
    win->doWxEvent(wxEVT_COMMAND_COMBOBOX_SELECTED);
}

- (void)comboBoxSelectionIsChanging:(NSNotification *)notification
{
    //...
}

- (void)comboBoxWillDismiss:(NSNotification *)notification
{
    //...
}

- (void)comboBoxWillPopUp:(NSNotification *)notification
{
    //...
}

@end // implementation wxPoserNSComboBox

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSComboBox.h>

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxTextCtrl)
BEGIN_EVENT_TABLE(wxComboBox, wxTextCtrl)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxComboBox,NSComboBox,NSTextField,NSView)

bool wxComboBox::Create(wxWindow *parent, wxWindowID winid,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, winid, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), style, validator, name);
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID winid,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    wxAutoNSAutoreleasePool pool;
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;

    m_cocoaNSView = NULL;
    SetNSComboBox([[wxPoserNSComboBox alloc] initWithFrame:MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    [GetNSTextField() setStringValue:wxNSStringWithWxString(value.c_str())];
    [GetNSControl() sizeToFit];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    for(int i = 0; i < n; ++i)
        wxComboBox::DoAppend(choices[i]);

    [GetNSComboBox() setCompletes:true]; //autocomplete :)

    return true;
}

wxComboBox::~wxComboBox()
{
    DisassociateNSComboBox(GetNSComboBox());
}

void wxComboBox::doWxEvent(int nEvent)
{
    wxCommandEvent event2(wxEVT_COMMAND_COMBOBOX_SELECTED, GetId() );
    event2.SetInt(GetSelection());
    event2.SetEventObject(this);
    event2.SetString(GetStringSelection());
    GetEventHandler()->ProcessEvent(event2);

    // For consistency with MSW and GTK, also send a text updated event
    // After all, the text is updated when a selection is made
    wxCommandEvent TextEvent( wxEVT_COMMAND_TEXT_UPDATED, GetId() );
    TextEvent.SetString( GetStringSelection() );
    TextEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent( TextEvent );
}


void wxComboBox::SetSelection(int nSelection)
{
    [GetNSComboBox() selectItemAtIndex:nSelection];
}

wxString wxComboBox::GetStringSelection()
{
    return wxStringWithNSString([GetNSComboBox() objectValueOfSelectedItem]);
}

void wxComboBox::Clear()
{
    [GetNSComboBox() removeAllItems];
    m_Datas.Clear();
}

void wxComboBox::Delete(unsigned int n)
{
    [GetNSComboBox() removeItemAtIndex:n];
    m_Datas.RemoveAt(n);
}

unsigned int wxComboBox::GetCount() const
{
    return (unsigned int)[GetNSComboBox() numberOfItems];
}

wxString wxComboBox::GetString(unsigned int nIndex) const
{
    return wxStringWithNSString([GetNSComboBox() itemObjectValueAtIndex:nIndex]);
}

void wxComboBox::SetString(unsigned int nIndex, const wxString& szString)
{
    wxAutoNSAutoreleasePool pool;
    //FIXME:  There appears to be no "set item data" method - maybe
    //an assignment would work?
    [GetNSComboBox() removeItemAtIndex:nIndex];
    [GetNSComboBox() insertItemWithObjectValue:wxNSStringWithWxString(szString) atIndex:nIndex];
}

int wxComboBox::FindString(const wxString& szItem, bool bCase) const
{
    // FIXME: use wxItemContainerImmutable::FindString for bCase parameter
    return [GetNSComboBox() indexOfItemWithObjectValue:wxNSStringWithWxString(szItem)];
}

int wxComboBox::GetSelection() const
{
    return [GetNSComboBox() indexOfSelectedItem];
}

int wxComboBox::DoAppend(const wxString& szItem)
{
    m_Datas.Add(NULL);
    wxAutoNSAutoreleasePool pool;
    [GetNSComboBox() addItemWithObjectValue:wxNSStringWithWxString(szItem)];
    return [GetNSComboBox() numberOfItems];
}

int wxComboBox::DoInsert(const wxString& szItem, unsigned int nIndex)
{
    m_Datas.Insert(NULL, nIndex);
    wxAutoNSAutoreleasePool pool;
    [GetNSComboBox() insertItemWithObjectValue:wxNSStringWithWxString(szItem) atIndex:nIndex];
    return (int)nIndex;
}

void wxComboBox::DoSetItemClientData(unsigned int nIndex, void* pData)
{
    m_Datas[nIndex] = pData;
}

void* wxComboBox::DoGetItemClientData(unsigned int nIndex) const
{
    return m_Datas[nIndex];
}

void wxComboBox::DoSetItemClientObject(unsigned int nIndex, wxClientData* pClientData)
{
    m_Datas[nIndex] = (void*) pClientData;
}

wxClientData* wxComboBox::DoGetItemClientObject(unsigned int nIndex) const
{
    return (wxClientData*) m_Datas[nIndex];
}

#endif //wxUSE_COMBOBOX
