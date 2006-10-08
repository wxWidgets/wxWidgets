///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
//
// new DataBrowser-based version


#include "wx/wxprec.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"

#ifndef WX_PRECOMP
    #include "wx/arrstr.h"
#endif

#include "wx/mac/uma.h"

#ifndef __DARWIN__
#include <Appearance.h>
#endif

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
END_EVENT_TABLE()

class wxMacDataBrowserCheckListControl : public wxMacDataBrowserListControl , public wxMacCheckListControl
{
public:
    wxMacDataBrowserCheckListControl( wxListBox *peer, const wxPoint& pos, const wxSize& size, long style );
    virtual ~wxMacDataBrowserCheckListControl();

    virtual wxMacDataItem* CreateItem();

    virtual bool            MacIsChecked(unsigned int n) const;
    virtual void            MacCheck(unsigned int n, bool bCheck = true);
};

void wxCheckListBox::Init()
{
}

bool wxCheckListBox::Create(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    const wxArrayString& choices,
    long style,
    const wxValidator& validator,
    const wxString &name )
{
    wxCArrayString chs( choices );

    return Create( parent, id, pos, size, chs.GetCount(), chs.GetStrings(), style, validator, name );
}

bool wxCheckListBox::Create(
   wxWindow *parent,
   wxWindowID id,
   const wxPoint& pos,
   const wxSize& size,
   int n,
   const wxString choices[],
   long style,
   const wxValidator& validator,
   const wxString& name )
{
    m_macIsUserPane = false;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  wxT("only one of listbox selection modes can be specified") );

    if ( !wxListBoxBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    // this will be increased by our Append command
    wxMacDataBrowserCheckListControl* control = new wxMacDataBrowserCheckListControl( this, pos, size, style );
    control->SetClientDataType( m_clientDataItemsType );
    m_peer = control;

    MacPostControlCreate(pos,size);

    InsertItems( n , choices , 0 );

    // Needed because it is a wxControlWithItems
    SetBestSize( size );

    return true;
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(unsigned int item) const
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid index in wxCheckListBox::IsChecked") );

    return GetPeer()->MacIsChecked( item );
}

void wxCheckListBox::Check(unsigned int item, bool check)
{
    wxCHECK_RET( IsValid(item),
                 wxT("invalid index in wxCheckListBox::Check") );

    bool isChecked = GetPeer()->MacIsChecked( item );
    if ( check != isChecked )
    {
        GetPeer()->MacCheck( item , check );
    }
}

wxMacCheckListControl* wxCheckListBox::GetPeer() const
{
    return dynamic_cast<wxMacCheckListControl*>(m_peer);
}

const short kCheckboxColumnId = 1026;

wxMacDataBrowserCheckListControl::wxMacDataBrowserCheckListControl( wxListBox *peer, const wxPoint& pos, const wxSize& size, long style)
    : wxMacDataBrowserListControl( peer, pos, size, style )
{
    OSStatus err = noErr;

    DataBrowserListViewColumnDesc columnDesc;
    columnDesc.headerBtnDesc.titleOffset = 0;
    columnDesc.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;

    columnDesc.headerBtnDesc.btnFontStyle.flags =
        kControlUseFontMask | kControlUseJustMask;

    columnDesc.headerBtnDesc.btnContentInfo.contentType = kControlNoContent;
    columnDesc.headerBtnDesc.btnFontStyle.just = teFlushDefault;
    columnDesc.headerBtnDesc.btnFontStyle.font = kControlFontViewSystemFont;
    columnDesc.headerBtnDesc.btnFontStyle.style = normal;
    columnDesc.headerBtnDesc.titleString = NULL;

    columnDesc.headerBtnDesc.minimumWidth = 30;
    columnDesc.headerBtnDesc.maximumWidth = 30;

    columnDesc.propertyDesc.propertyID = kCheckboxColumnId;
    columnDesc.propertyDesc.propertyType = kDataBrowserCheckboxType;
    columnDesc.propertyDesc.propertyFlags =
        kDataBrowserPropertyIsMutable
        | kDataBrowserTableViewSelectionColumn
        | kDataBrowserDefaultPropertyFlags;

    err = AddColumn( &columnDesc, 0 );
    verify_noerr( err );
}

wxMacDataBrowserCheckListControl::~wxMacDataBrowserCheckListControl()
{

}

class wxMacCheckListBoxItem : public wxMacListBoxItem
{
public :
    wxMacCheckListBoxItem()
    {
        m_isChecked = false;
    }

    virtual ~wxMacCheckListBoxItem()
    {
    }

    virtual OSStatus GetSetData( wxMacDataItemBrowserControl *owner ,
        DataBrowserPropertyID property,
        DataBrowserItemDataRef itemData,
        bool changeValue )
    {
        OSStatus err = errDataBrowserPropertyNotSupported;

        wxCheckListBox *checklist = wxDynamicCast( owner->GetPeer() , wxCheckListBox );
        wxCHECK_MSG( checklist != NULL , errDataBrowserPropertyNotSupported , wxT("wxCheckListBox expected"));

        if ( !changeValue )
        {
            switch (property)
            {
                case kCheckboxColumnId:
                    verify_noerr(SetDataBrowserItemDataButtonValue( itemData, m_isChecked ? kThemeButtonOn : kThemeButtonOff ));
                    err = noErr;
                    break;

                case kDataBrowserItemIsEditableProperty:
                    verify_noerr(SetDataBrowserItemDataBooleanValue( itemData, true ));
                    err = noErr;
                    break;

                default:
                    break;
            }
        }
        else
        {
            switch (property)
            {
            case kCheckboxColumnId:
                {
                    // we have to change this behind the back, since Check() would be triggering another update round
                    bool newVal = !m_isChecked;
                    verify_noerr(SetDataBrowserItemDataButtonValue( itemData, newVal ? kThemeButtonOn : kThemeButtonOff ));
                    m_isChecked = newVal;
                    err = noErr;

                    wxCommandEvent event( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, checklist->GetId() );
                    event.SetInt( owner->GetLineFromItem( this ) );
                    event.SetEventObject( checklist );
                    checklist->GetEventHandler()->ProcessEvent( event );
                }
                break;

            default:
                break;
            }
        }

        if ( err == errDataBrowserPropertyNotSupported )
            err = wxMacListBoxItem::GetSetData( owner , property, itemData , changeValue);

        return err;
    }

    void Check( bool check )
    {
        m_isChecked = check;
    }
    bool IsChecked() const
    {
         return m_isChecked;
    }

protected :
    bool        m_isChecked;
};

wxMacDataItem* wxMacDataBrowserCheckListControl::CreateItem()
{
    return new wxMacCheckListBoxItem();
}

void wxMacDataBrowserCheckListControl::MacCheck( unsigned int n, bool bCheck)
{
    wxMacCheckListBoxItem* item = dynamic_cast<wxMacCheckListBoxItem*>( GetItemFromLine( n) );
    item->Check( bCheck);
    UpdateItem(wxMacDataBrowserRootContainer, item , kCheckboxColumnId);
}

bool wxMacDataBrowserCheckListControl::MacIsChecked( unsigned int n) const
{
    wxMacCheckListBoxItem * item = dynamic_cast<wxMacCheckListBoxItem*>( GetItemFromLine( n ) );
    return item->IsChecked();
}



#endif // wxUSE_CHECKLISTBOX
