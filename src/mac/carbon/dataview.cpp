/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/datavgen.cpp
// Purpose:     wxDataViewCtrl native mac implementation
// Author:      
// Id:          $Id$
// Copyright:   (c) 2007
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#if !defined(wxUSE_GENERICDATAVIEWCTRL) || (wxUSE_GENERICDATAVIEWCTRL == 0)

#include <limits>

#include "wx/mac/carbon/databrow.h"

#ifndef WX_PRECOMP
  #include "wx/timer.h"
#endif

#include "wx/icon.h"
#include "wx/renderer.h"

//-----------------------------------------------------------------------------
// local constants
//-----------------------------------------------------------------------------

// a list of all catchable events:
static EventTypeSpec const eventList[] =
{
  {kEventClassControl, kEventControlDraw},
  {kEventClassControl, kEventControlHit}
};

//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

static pascal OSStatus wxMacDataViewCtrlEventHandler(EventHandlerCallRef handler, EventRef EventReference, void* Data)
{
  wxDataViewCtrl* DataViewCtrlPtr((wxDataViewCtrl*) Data); // the 'Data' variable always contains a pointer to the data view control that installed the handler 

  wxMacCarbonEvent CarbonEvent(EventReference) ;


  switch (GetEventKind(EventReference))
  {
    case kEventControlDraw:
      {
        OSStatus status;

        DataViewCtrlPtr->MacSetDrawingContext(CarbonEvent.GetParameter<CGContextRef>(kEventParamCGContextRef,typeCGContextRef));
        status = ::CallNextEventHandler(handler,EventReference);
        DataViewCtrlPtr->MacSetDrawingContext(NULL);
        return status;
      } /* block */
    case kEventControlHit :
      if (CarbonEvent.GetParameter<ControlPartCode>(kEventParamControlPart,typeControlPartCode) == kControlButtonPart) // we only care about the header
      {
        ControlRef            controlReference;
        DataBrowserPropertyID columnPropertyID;
        unsigned long         columnIndex;
        OSStatus              status;
        wxDataViewEvent       DataViewEvent(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK,DataViewCtrlPtr->GetId());

        CarbonEvent.GetParameter(kEventParamDirectObject,&controlReference);
       // determine the column that triggered the event (this is the column that is responsible for sorting the data view):
        status = ::GetDataBrowserSortProperty(controlReference,&columnPropertyID);
        wxCHECK(status == noErr,status);
        status = ::GetDataBrowserTableViewColumnPosition(controlReference,columnPropertyID,&columnIndex);
        if (status == errDataBrowserPropertyNotFound) // user clicked into part of the header that does not have a property
          return ::CallNextEventHandler(handler,EventReference);
        wxCHECK(status == noErr,status);
       // initialize wxWidget event handler:
        DataViewEvent.SetEventObject(DataViewCtrlPtr);
        DataViewEvent.SetColumn(columnIndex);
        DataViewEvent.SetDataViewColumn(DataViewCtrlPtr->GetColumn(columnIndex));
       // finally sent the equivalent wxWidget event:
        DataViewCtrlPtr->GetEventHandler()->ProcessEvent(DataViewEvent);
        return ::CallNextEventHandler(handler,EventReference);
      } /* if */
      else
        return eventNotHandledErr;
  } /* switch */

  return eventNotHandledErr;
} /* wxMacDataViewCtrlEventHandler(EventHandlerCallRef, EventRef, void*) */

//-----------------------------------------------------------------------------
// local function pointers
//-----------------------------------------------------------------------------

DEFINE_ONE_SHOT_HANDLER_GETTER(wxMacDataViewCtrlEventHandler)

// ---------------------------------------------------------
// wxMacDataViewModelNotifier
// ---------------------------------------------------------
#pragma mark -
class wxMacDataViewModelNotifier : public wxDataViewModelNotifier
{
public:
  wxMacDataViewModelNotifier(wxMacDataViewDataBrowserListViewControl* initDataViewControlPtr) : m_dataViewControlPtr(initDataViewControlPtr)
  {
  }

  virtual bool ItemAdded(const wxDataViewItem &parent, const wxDataViewItem &item)
  {
    DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));


    wxCHECK_MSG(item.IsOk(),false,_("Added item is invalid."));
    return (!(parent.IsOk()) && (this->m_dataViewControlPtr->AddItem(kDataBrowserNoItem,&itemID) == noErr) ||
            parent.IsOk()  && (this->m_dataViewControlPtr->AddItem(reinterpret_cast<DataBrowserItemID>(parent.GetID()),&itemID) == noErr));
  } /* ItemAdded(wxDataViewItem const&, wxDataViewItem const&) */

  virtual bool ItemChanged(wxDataViewItem const& item)
  {
    DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    
    
    wxCHECK_MSG(item.IsOk(),             false,_("Changed item is invalid."));
    wxCHECK_MSG(this->GetOwner() != NULL,false,_("Owner not initialized."));
    if (this->m_dataViewControlPtr->UpdateItems(&itemID) == noErr)
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      
     // sent the equivalent wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_CHANGED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetItem(item);
     // sent the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
  } /* ItemChanged(wxDataViewItem const&) */

  virtual bool ItemDeleted(wxDataViewItem const& parent, wxDataViewItem const& item)
  {
    if (item.IsOk())
    {
     // variable definition and initialization:
      DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
      OSStatus          errorStatus;
      wxDataViewCtrl*   dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      
     // when this method is called and currently an item is being edited this item may have already been deleted in the model (the passed item and the being edited item have
     // not to be identical because the being edited item might be below the passed item in the hierarchy); therefore, the control is informed that currently a deleting process
     // is started and that variables can currently not be updated even when requested by the system:
      dataViewCtrlPtr->SetDeleting(true);
      errorStatus = this->m_dataViewControlPtr->RemoveItem(reinterpret_cast<DataBrowserItemID>(parent.GetID()),&itemID);
      dataViewCtrlPtr->SetDeleting(false);
      return (errorStatus == noErr);
    } /* if */
    else
      return false;
  } /* ItemDeleted(wxDataViewItem const&, wxDataViewItem const&) */

  virtual bool ValueChanged(wxDataViewItem const& item, unsigned int col)
  {
    DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    DataBrowserItemID parentID;
    
    DataBrowserPropertyID propertyID;

    wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));


    wxCHECK_MSG(item.IsOk(),             false,_("Passed item is invalid."));
    wxCHECK_MSG(this->GetOwner() != NULL,false,_("Owner not initialized."));
    wxCHECK_MSG(dataViewCtrlPtr != NULL, false,_("Control is wrongly initialized."));
    parentID = reinterpret_cast<DataBrowserItemID>(this->GetOwner()->GetParent(item).GetID());
    if ((this->m_dataViewControlPtr->GetPropertyID(col,&propertyID) == noErr) &&
        (this->m_dataViewControlPtr->UpdateItems(parentID,1,&itemID,dataViewCtrlPtr->GetColumn(col)->GetPropertyID(),propertyID) == noErr))
    {
     // variable definition and initialization:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED,dataViewCtrlPtr->GetId());

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetColumn(col);
      dataViewEvent.SetItem(item);
     // send the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
  } /* ValueChanged(wxDataViewItem const&, unsigned int) */

  virtual bool Cleared(void)
  {
    if (this->m_dataViewControlPtr->RemoveItems() == noErr)
    {
     // variable definitions and initializations:
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      wxDataViewEvent dataViewEvent  (wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED,dataViewCtrlPtr->GetId());

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
     // send the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
  } /* Cleared(void) */

  virtual void Resort(void)
  {
    this->m_dataViewControlPtr->Resort();
  }

protected:
private:
//
// variables
//
  wxMacDataViewDataBrowserListViewControl* m_dataViewControlPtr;
};

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewRenderer::wxDataViewRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                   :wxDataViewRendererBase(varianttype,mode,align), m_alignment(align), m_mode(mode)
{
} /* wxDataViewRenderer::wxDataViewRenderer(wxString const&, wxDataViewCellMode) */

void wxDataViewRenderer::SetMode(wxDataViewCellMode mode)
{
  wxDataViewColumn* dataViewColumnPtr;


  this->m_mode = mode;
  dataViewColumnPtr = this->GetOwner();
  if (dataViewColumnPtr != NULL)
  {
   // variable definition and initialization:
    wxDataViewCtrl* dataViewCtrlPtr(dataViewColumnPtr->GetOwner());
    
    if (dataViewCtrlPtr != NULL)
    {
     // variable definition and initialization:
      wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
      
      if (macDataViewListCtrlPtr != NULL)
      {
       // variable definition and initialization:
        DataBrowserPropertyFlags flags;
        
        verify_noerr(macDataViewListCtrlPtr->GetPropertyFlags(dataViewColumnPtr->GetPropertyID(),&flags));
        if (mode == wxDATAVIEW_CELL_EDITABLE)
          flags |= kDataBrowserPropertyIsEditable;
        else
          flags &= ~kDataBrowserPropertyIsEditable;
        verify_noerr(macDataViewListCtrlPtr->SetPropertyFlags(dataViewColumnPtr->GetPropertyID(),flags));
      } /* if */
    } /* if */
  } /* if */
} /* wxDataViewRenderer::SetMode(wxDataViewCellMode) */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewRenderer,wxDataViewRendererBase)

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewCustomRenderer::wxDataViewCustomRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                         :wxDataViewRenderer(varianttype,mode,align), m_editorCtrlPtr(NULL), m_DCPtr(NULL)
{
} /* wxDataViewCustomRenderer::wxDataViewCustomRenderer(wxString const&, wxDataViewCellMode) */

wxDataViewCustomRenderer::~wxDataViewCustomRenderer(void)
{
  if (this->m_DCPtr != NULL)
    delete this->m_DCPtr;
} /* wxDataViewCustomRenderer::~wxDataViewCustomRenderer(void) */

wxDC* wxDataViewCustomRenderer::GetDC(void)
{
  if (this->m_DCPtr == NULL)
  {
    if ((GetOwner() == NULL) || (GetOwner()->GetOwner() == NULL))
      return NULL;
    this->m_DCPtr = new wxClientDC(this->GetOwner()->GetOwner());
  } /* if */
  return this->m_DCPtr;
} /* wxDataViewCustomRenderer::GetDC(void) */

bool wxDataViewCustomRenderer::Render(void)
{
  return true;
} /* wxDataViewCustomRenderer::Render(void) */

void wxDataViewCustomRenderer::SetDC(wxDC* newDCPtr)
{
  delete this->m_DCPtr;
  this->m_DCPtr = newDCPtr;
} /* wxDataViewCustomRenderer::SetDC(wxDC*) */


IMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewTextRenderer::wxDataViewTextRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                       :wxDataViewRenderer(varianttype,mode,align)
{
} /* wxDataViewTextRenderer::wxDataViewTextRenderer(wxString const&, wxDataViewCellMode, int) */

bool wxDataViewTextRenderer::Render(void)
{
  if (this->GetValue().GetType() == this->GetVariantType())
  {
   // variable definition:
    wxMacCFStringHolder cfString(this->GetValue().GetString(),(this->GetView()->GetFont().Ok() ? this->GetView()->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));

    return (::SetDataBrowserItemDataText(this->GetDataReference(),cfString) == noErr);
  } /* if */
  else
    return false;
} /* wxDataViewTextRenderer::Render(void) */

IMPLEMENT_CLASS(wxDataViewTextRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewBitmapRenderer::wxDataViewBitmapRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                         :wxDataViewRenderer(varianttype,mode,align)
{
}

bool wxDataViewBitmapRenderer::Render(void)
{
  if (this->GetValue().GetType() == this->GetVariantType())
  {
    wxBitmap bitmap;
    
    bitmap << this->GetValue();
    if (bitmap.Ok())
      return (::SetDataBrowserItemDataIcon(this->GetDataReference(),bitmap.GetBitmapData()->GetIconRef()) == noErr);
    else
      return true;
  } /* if */
  else
    return false;
} /* wxDataViewBitmapRenderer::Render(void) */

IMPLEMENT_CLASS(wxDataViewBitmapRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewIconTextRenderer::wxDataViewIconTextRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                           :wxDataViewRenderer(varianttype,mode)
{
}

bool wxDataViewIconTextRenderer::Render(void)
{
  if (this->GetValue().GetType() == this->GetVariantType())
  {
   // variable definition:
    wxDataViewIconText iconText;
    
    iconText << this->GetValue();

   // variable definition:
    wxMacCFStringHolder cfString(iconText.GetText(),(this->GetView()->GetFont().Ok() ? this->GetView()->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
    
    return ((::SetDataBrowserItemDataIcon(this->GetDataReference(),MAC_WXHICON(iconText.GetIcon().GetHICON())) == noErr) &&
            (::SetDataBrowserItemDataText(this->GetDataReference(),cfString) == noErr));
  } /* if */
  else
    return false;
} /* wxDataViewIconTextRenderer::Render(void) */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewIconTextRenderer,wxDataViewRenderer)


// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewToggleRenderer::wxDataViewToggleRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                         :wxDataViewRenderer(varianttype,mode)
{
}

bool wxDataViewToggleRenderer::Render(void)
{
  if (this->GetValue().GetType() == this->GetVariantType())
    return (::SetDataBrowserItemDataButtonValue(this->GetDataReference(),this->GetValue().GetBool()) == noErr);
  else
    return false;
} /* wxDataViewToggleRenderer::Render(void) */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewProgressRenderer::wxDataViewProgressRenderer(wxString const& label, wxString const& varianttype, wxDataViewCellMode mode, int align)
                           :wxDataViewRenderer(varianttype,mode,align)
{
}

bool wxDataViewProgressRenderer::Render(void)
{
  if (this->GetValue().GetType() == this->GetVariantType())
    return ((::SetDataBrowserItemDataMinimum(this->GetDataReference(),  0)                        == noErr) &&
            (::SetDataBrowserItemDataMaximum(this->GetDataReference(),100)                        == noErr) &&
            (::SetDataBrowserItemDataValue  (this->GetDataReference(),this->GetValue().GetLong()) == noErr));
  else
    return false;
} /* wxDataViewProgressRenderer::Render(void) */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewDateRenderer::wxDataViewDateRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                       :wxDataViewRenderer(varianttype,mode,align)
{
}

bool wxDataViewDateRenderer::Render(void)
{
  if (this->GetValue().GetType() == this->GetVariantType())
    return (::SetDataBrowserItemDataDateTime(this->GetDataReference(),this->GetValue().GetDateTime().Subtract(wxDateTime(1,wxDateTime::Jan,1904)).GetSeconds().GetLo()) == noErr);
  else
    return false;
} /* wxDataViewDateRenderer::Render(void) */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewDateRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------
#pragma mark -
wxDataViewColumn::wxDataViewColumn(wxString const &title, wxDataViewRenderer *cell, unsigned int model_column, int width, wxAlignment align, int flags)
                 :wxDataViewColumnBase(title,cell,model_column,width,wxALIGN_CENTER,flags), m_ascending(true),
                  m_flags(flags & ~(wxDATAVIEW_COL_HIDDEN)), m_maxWidth(30000), m_minWidth(0), m_width(width >= 0 ? width : wxDVC_DEFAULT_WIDTH),
                  m_alignment(align), m_title(title)
{
} /* wxDataViewColumn::wxDataViewColumn(wxString const &title, wxDataViewRenderer*, unsigned int, int, wxAlignment, int) */

wxDataViewColumn::wxDataViewColumn(wxBitmap const& bitmap, wxDataViewRenderer *cell, unsigned int model_column, int width, wxAlignment align, int flags)
                 :wxDataViewColumnBase(bitmap,cell,model_column,width,wxALIGN_CENTER,flags), m_ascending(true),
                  m_flags(flags & ~(wxDATAVIEW_COL_HIDDEN)), m_maxWidth(30000), m_minWidth(0), m_width(width >= 0 ? width : wxDVC_DEFAULT_WIDTH),
                  m_alignment(align)
{
} /* wxDataViewColumn::wxDataViewColumn(wxBitmap const&, wxDataViewRenderer*, unsigned int, int, wxAlignment, int) */

void wxDataViewColumn::SetAlignment(wxAlignment align)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());

  
  this->m_alignment = align;
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;

      wxCHECK_RET(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not get header description."));
      switch (align)
      {
        case wxALIGN_CENTER:
        case wxALIGN_CENTER_HORIZONTAL:
          headerDescription.btnFontStyle.just = teCenter;
          break;
        case wxALIGN_LEFT:
          headerDescription.btnFontStyle.just = teFlushLeft;
          break;
        case wxALIGN_RIGHT:
          headerDescription.btnFontStyle.just = teFlushRight;
          break;
        default:
          headerDescription.btnFontStyle.just = teFlushDefault;
      } /* switch */
      wxCHECK_RET(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not set alignment."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetAlignment(wxAlignment) */

void wxDataViewColumn::SetBitmap(wxBitmap const& bitmap)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());


  wxDataViewColumnBase::SetBitmap(bitmap);
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;

      wxCHECK_RET(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not get header description."));
      if (this->GetBitmap().Ok())
        headerDescription.btnContentInfo.u.iconRef = this->GetBitmap().GetBitmapData()->GetIconRef();
      else
        headerDescription.btnContentInfo.u.iconRef = NULL;
      wxCHECK_RET(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not set icon."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetBitmap(wxBitmap const&) */

void wxDataViewColumn::SetFlags(int flags)
{
  this->SetHidden    ((flags & wxDATAVIEW_COL_HIDDEN)    != 0);
  this->SetResizeable((flags & wxDATAVIEW_COL_RESIZABLE) != 0);
  this->SetSortable  ((flags & wxDATAVIEW_COL_SORTABLE)  != 0);
} /* wxDataViewColumn::SetFlags(int) */

void wxDataViewColumn::SetMaxWidth(int maxWidth)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());

  
  this->m_maxWidth = maxWidth;
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;
      
      wxCHECK_RET(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not get header description."));
      headerDescription.maximumWidth = static_cast<UInt16>(maxWidth);
      wxCHECK_RET(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not set maximum width."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetMaxWidth(int) */

void wxDataViewColumn::SetMinWidth(int minWidth)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());

  
  this->m_minWidth = minWidth;
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;
      
      wxCHECK_RET(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not get header description."));
      headerDescription.minimumWidth = static_cast<UInt16>(minWidth);
      wxCHECK_RET(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not set minimum width."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetMaxWidth(int) */

void wxDataViewColumn::SetResizeable(bool WXUNUSED(resizeable))
{
} /* wxDataViewColumn::SetResizeable(bool) */

void wxDataViewColumn::SetSortable(bool sortable)
{
 // variable definition and initialization:
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());
  
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserPropertyFlags flags;
      
      wxCHECK_RET(macDataViewListCtrlPtr->GetPropertyFlags(this->GetPropertyID(),&flags) == noErr,_("Could not get property flags."));
      if (sortable)
      {
        this->m_flags |= wxDATAVIEW_COL_SORTABLE;
        flags         |= kDataBrowserListViewSortableColumn;
      } /* if */
      else
      {
        this->m_flags &= ~wxDATAVIEW_COL_SORTABLE;
        flags         &= ~kDataBrowserPropertyIsEditable;
      } /* if */
      wxCHECK_RET(macDataViewListCtrlPtr->SetPropertyFlags(this->GetPropertyID(),flags) == noErr,_("Could not set property flags."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetSortable(bool) */

void wxDataViewColumn::SetSortOrder(bool ascending)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());

  
  this->m_ascending = ascending;
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;
      
      verify_noerr(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription));
      if (ascending)
        headerDescription.initialOrder = kDataBrowserOrderIncreasing;
      else
        headerDescription.initialOrder = kDataBrowserOrderDecreasing;
      verify_noerr(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetSortOrder(bool) */

void wxDataViewColumn::SetTitle(wxString const& title)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());

  
  this->m_title = title;
  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;
      wxMacCFStringHolder           cfTitle(title,(dataViewCtrlPtr->GetFont().Ok() ? dataViewCtrlPtr->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
      
      wxCHECK_RET(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not get header description."));
      headerDescription.titleString = cfTitle;
      wxCHECK_RET(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not set header description."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetTitle(wxString const&) */

void wxDataViewColumn::SetWidth(int width)
{
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner());

  
  if ((width >= this->m_minWidth) && (width <= this->m_maxWidth))
  {
    this->m_width = width;
    if (dataViewCtrlPtr != NULL)
    {
     // variable definition and initialization:
      wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
      
      if (macDataViewListCtrlPtr != NULL)
        wxCHECK_RET(macDataViewListCtrlPtr->SetColumnWidth(this->GetPropertyID(),static_cast<UInt16>(width)) == noErr,_("Could not set column width."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetWidth(int) */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumn,wxDataViewColumnBase)

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------
#pragma mark -
void wxDataViewCtrl::Init(void)
{
  this->m_Deleting      = false;
  this->m_macIsUserPane = false;
  this->m_cgContext     = NULL;
} /* wxDataViewCtrl::Init(void) */

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator )
{
  if (!(this->wxControl::Create(parent,id,pos,size,(style | wxSUNKEN_BORDER) & ~(wxHSCROLL | wxVSCROLL),validator)))
    return false;

#ifdef __WXMAC__
  MacSetClipChildren(true) ;
#endif

  this->m_peer = new wxMacDataViewDataBrowserListViewControl(this,pos,size,style);
  this->MacPostControlCreate(pos,size);
  ::SetAutomaticControlDragTrackingEnabledForWindow(::GetControlOwner(this->m_peer->GetControlRef()),true);

  InstallControlEventHandler(this->m_peer->GetControlRef(),GetwxMacDataViewCtrlEventHandlerUPP(),GetEventTypeCount(eventList),eventList,this,NULL);

  return true;
} /* wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) */

bool wxDataViewCtrl::AssociateModel(wxDataViewModel* model)
{
  if (!wxDataViewCtrlBase::AssociateModel(model))
    return false;
  
  model->AddNotifier(new wxMacDataViewModelNotifier(dynamic_cast<wxMacDataViewDataBrowserListViewControl*>(this->m_peer)));

  return true;
} /* wxDataViewCtrl::AssociateModel(wxDataViewModel*) */

bool wxDataViewCtrl::AppendColumn(wxDataViewColumn* dataViewColumnPtr)
{
  DataBrowserPropertyID NewPropertyID;

  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));


 // first, some error checking:
  wxCHECK_MSG(MacDataViewListCtrlPtr != NULL,                                            false,_("m_peer is not or incorrectly initialized"));
  wxCHECK_MSG(dataViewColumnPtr != NULL,                                                 false,_("Column pointer must not be NULL."));
  wxCHECK_MSG(dataViewColumnPtr->GetRenderer() != NULL,                                  false,_("Column does not have a renderer."));
  wxCHECK_MSG(this->GetModel() != NULL,                                                  false,_("No model associated with control."));
  wxCHECK_MSG((dataViewColumnPtr->GetModelColumn() >= 0) &&
              (dataViewColumnPtr->GetModelColumn() < this->GetModel()->GetColumnCount()),false,_("Column's model column has no equivalent in the associated model."));
  if ((MacDataViewListCtrlPtr->GetFreePropertyID(&NewPropertyID) == noErr) && this->wxDataViewCtrlBase::AppendColumn(dataViewColumnPtr))
  {
   // insert column into hash map:
    this->m_ColumnPointers.insert(ColumnPointerHashMapType::value_type(NewPropertyID,dataViewColumnPtr));

   // variable definitions:
    DataBrowserListViewColumnDesc columnDescription;
    wxMacCFStringHolder           cfTitle(dataViewColumnPtr->GetTitle(),(this->m_font.Ok() ? this->m_font.GetEncoding() : wxLocale::GetSystemEncoding()));

   // initialize column description:
    dataViewColumnPtr->SetPropertyID(NewPropertyID);
    columnDescription.propertyDesc.propertyID = NewPropertyID;
    columnDescription.propertyDesc.propertyType = dataViewColumnPtr->GetRenderer()->GetPropertyType();
    columnDescription.propertyDesc.propertyFlags = kDataBrowserListViewSelectionColumn; // make the column selectable
    if (dataViewColumnPtr->IsSortable())
      columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewSortableColumn;
#if 0
    if (dataViewColumnPtr->IsMovable())
      columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewMovableColumn;
#endif
    if (dataViewColumnPtr->GetRenderer()->GetMode() == wxDATAVIEW_CELL_EDITABLE)
      columnDescription.propertyDesc.propertyFlags |= kDataBrowserPropertyIsEditable;
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    if ((columnDescription.propertyDesc.propertyType == kDataBrowserTextType) ||
        (columnDescription.propertyDesc.propertyType == kDataBrowserIconAndTextType) ||
        (columnDescription.propertyDesc.propertyType == kDataBrowserDateTimeType))
      columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewTypeSelectColumn;
#endif
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
    columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewNoGapForIconInHeaderButton;
#endif
    columnDescription.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;
    if (dataViewColumnPtr->IsResizeable())
    {
      columnDescription.headerBtnDesc.minimumWidth = 0;
      columnDescription.headerBtnDesc.maximumWidth = 30000;
    } /* if */
    else
    {
      columnDescription.headerBtnDesc.minimumWidth = dataViewColumnPtr->GetWidth();
      columnDescription.headerBtnDesc.maximumWidth = dataViewColumnPtr->GetWidth();
    } /* if */
    columnDescription.headerBtnDesc.titleOffset = 0;
    columnDescription.headerBtnDesc.titleString = cfTitle; // we cannot directly use the wxMacCFStringHolder constructor call because then the CFStringRef is released
                                                           // having called 'AddColumn' where the title (CFStringRef) is going to be used
    columnDescription.headerBtnDesc.initialOrder = kDataBrowserOrderIncreasing;
    columnDescription.headerBtnDesc.btnFontStyle.flags = kControlUseFontMask | kControlUseJustMask;
    switch (dataViewColumnPtr->GetAlignment())
    {
      case wxALIGN_CENTER:
      case wxALIGN_CENTER_HORIZONTAL:
        columnDescription.headerBtnDesc.btnFontStyle.just = teCenter;
        break;
      case wxALIGN_LEFT:
        columnDescription.headerBtnDesc.btnFontStyle.just = teFlushLeft;
        break;
      case wxALIGN_RIGHT:
        columnDescription.headerBtnDesc.btnFontStyle.just = teFlushRight;
        break;
      default:
        columnDescription.headerBtnDesc.btnFontStyle.just = teFlushDefault;
    } /* switch */
    columnDescription.headerBtnDesc.btnFontStyle.font  = kControlFontViewSystemFont;
    columnDescription.headerBtnDesc.btnFontStyle.style = normal;
    columnDescription.headerBtnDesc.btnContentInfo.contentType = kControlContentIconRef;
    if (dataViewColumnPtr->GetBitmap().Ok())
      columnDescription.headerBtnDesc.btnContentInfo.u.iconRef = dataViewColumnPtr->GetBitmap().GetBitmapData()->GetIconRef();
   // add column:
    wxCHECK_MSG(MacDataViewListCtrlPtr->AddColumn(&columnDescription,kDataBrowserListViewAppendColumn) == noErr,false,_("Column could not be added."));

   // final adjustments for the layout:
    wxCHECK_MSG(MacDataViewListCtrlPtr->SetColumnWidth(NewPropertyID,dataViewColumnPtr->GetWidth()) == noErr,false,_("Column width could not be set."));

   // make sure that the data is up-to-date...
   // if the newly appended column is the first column add the initial data to the control and mark the column as an expander column,
   // otherwise ask the control to 'update' the data in the newly appended column:
    if (this->GetColumnCount() == 1)
    {
      this->SetExpanderColumn(dataViewColumnPtr);
      this->AddChildrenLevel(wxDataViewItem());
    } /* if */
    else
      MacDataViewListCtrlPtr->UpdateItems(kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty,NewPropertyID);
   // done:
    return true;
  } /* if */
  else
    return false;
} /* wxDataViewCtrl::AppendColumn(wxDataViewColumn*) */

bool wxDataViewCtrl::ClearColumns(void)
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  while (this->m_ColumnPointers.begin() != this->m_ColumnPointers.end())
  {
    wxCHECK_MSG(MacDataViewListCtrlPtr->RemoveColumnByProperty(this->m_ColumnPointers.begin()->first) == noErr,false,_("Could not remove column."));
    delete this->m_ColumnPointers.begin()->second;
    this->m_ColumnPointers.erase(this->m_ColumnPointers.begin());
  } /* while */
  return true;
} /* wxDataViewCtrl::ClearColumns(void) */

bool wxDataViewCtrl::DeleteColumn(wxDataViewColumn* columnPtr)
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  if ((MacDataViewListCtrlPtr->RemoveColumnByProperty(columnPtr->GetPropertyID()) == noErr) && (this->m_ColumnPointers.erase(columnPtr->GetPropertyID()) > 0))
  {
    delete columnPtr;
    return true;
  } /* if */
  else
    return false;
} /* wxDataViewCtrl::DeleteColumn(wxDataViewColumn*) */

wxDataViewColumn* wxDataViewCtrl::GetColumn(unsigned int pos) const
{
  DataBrowserPropertyID propertyID;
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  if (MacDataViewListCtrlPtr->GetPropertyID(pos,&propertyID) == noErr)
  {
   // variable definition:
    ColumnPointerHashMapType::const_iterator Result(this->m_ColumnPointers.find(propertyID));
    
    if (Result != this->m_ColumnPointers.end())
      return Result->second;
    else
      return NULL;
  } /* if */
  else
    return NULL;
} /* wxDataViewCtrl::GetColumn(unsigned int pos) const */

unsigned int wxDataViewCtrl::GetColumnCount(void) const
{
  return this->m_ColumnPointers.size();
} /* wxDataViewCtrl::GetColumnCount(void) const */

wxDataViewColumn *wxDataViewCtrl::GetSortingColumn() const
{
    return NULL;
}

void wxDataViewCtrl::Collapse(wxDataViewItem const& item)
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));


  MacDataViewListCtrlPtr->CloseContainer(reinterpret_cast<DataBrowserItemID>(item.GetID()));
} /* wxDataViewCtrl::Collapse(wxDataViewItem const&) */

void wxDataViewCtrl::EnsureVisible(wxDataViewItem const& item, wxDataViewColumn const* columnPtr)
{
  if (item.IsOk())
  {
   // variable definition and initialization:
    DataBrowserPropertyID propertyID;
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

    if (columnPtr != NULL)
      propertyID = columnPtr->GetPropertyID();
    else
      propertyID = kDataBrowserNoItem;
    MacDataViewListCtrlPtr->RevealItem(reinterpret_cast<DataBrowserItemID>(item.GetID()),propertyID,kDataBrowserRevealOnly);
  } /* if */
} /* wxDataViewCtrl::EnsureVisible(wxDataViewItem const&, wxDataViewColumn const*) */

void wxDataViewCtrl::Expand(wxDataViewItem const& item)
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  MacDataViewListCtrlPtr->OpenContainer(reinterpret_cast<DataBrowserItemID>(item.GetID()));
} /* wxDataViewCtrl::Expand(wxDataViewItem const&) */

unsigned int wxDataViewCtrl::GetCount(void) const
{
  ItemCount noOfItems;


  wxCHECK_MSG(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer)->GetItemCount(&noOfItems) == noErr,0,_("Could not determine number of items"));
  return noOfItems;
} /* wxDataViewCtrl::GetCount(void) const */

wxRect wxDataViewCtrl::GetItemRect(wxDataViewItem const& item, wxDataViewColumn const* columnPtr) const
{
  if (item.IsOk() && (columnPtr != NULL))
  {
   // variable definition:
    Rect                                           MacRectangle;
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
    
    if (MacDataViewListCtrlPtr->GetPartBounds(reinterpret_cast<DataBrowserItemID>(item.GetID()),columnPtr->GetPropertyID(),kDataBrowserPropertyContentPart,&MacRectangle) == noErr)
    {
     // variable definition:
      wxRect rectangle;
      
      ::wxMacNativeToRect(&MacRectangle,&rectangle);
      return rectangle;
    } /* if */
    else
      return wxRect();
  } /* if */
  else
    return wxRect();
} /* wxDataViewCtrl::GetItemRect(wxDataViewItem const&, unsigned int) const */

wxDataViewItem wxDataViewCtrl::GetSelection(void) const
{
  wxArrayDataBrowserItemID itemIDs;
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  if (MacDataViewListCtrlPtr->GetSelectedItemIDs(itemIDs) > 0)
    return wxDataViewItem(reinterpret_cast<void*>(itemIDs[0]));
  else
    return wxDataViewItem();
} /* wxDataViewCtrl::GetSelection(void) const */

int wxDataViewCtrl::GetSelections(wxDataViewItemArray& sel) const
{
  size_t NoOfSelectedItems;

  wxArrayDataBrowserItemID itemIDs;
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  NoOfSelectedItems = MacDataViewListCtrlPtr->GetSelectedItemIDs(itemIDs);
  sel.Empty();
  sel.SetCount(NoOfSelectedItems);
  for (size_t i=0; i<NoOfSelectedItems; ++i)
    sel[i] = wxDataViewItem(reinterpret_cast<void*>(itemIDs[i]));
  return static_cast<int>(NoOfSelectedItems);
} /* wxDataViewCtrl::GetSelections(wxDataViewItemArray&) const */

void wxDataViewCtrl::HitTest(wxPoint const& point, wxDataViewItem& item, wxDataViewColumn*& columnPtr) const
{
  item = wxDataViewItem();
  columnPtr = NULL;
} /* wxDataViewCtrl::HitTest(wxPoint const&, wxDataViewItem&, wxDataViewColumn*&) const */

bool wxDataViewCtrl::IsSelected(wxDataViewItem const& item) const
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  return MacDataViewListCtrlPtr->IsItemSelected(reinterpret_cast<DataBrowserItemID>(item.GetID()));
} /* wxDataViewCtrl::IsSelected(wxDataViewItem const&) const */

void wxDataViewCtrl::SelectAll(void)
{
  DataBrowserItemID* itemIDPtr;

  Handle handle(::NewHandle(0));
  
  size_t NoOfItems;
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  wxCHECK_RET(MacDataViewListCtrlPtr->GetItems(kDataBrowserNoItem,true,kDataBrowserItemAnyState,handle) == noErr,_("Could not get items."));
  NoOfItems = static_cast<size_t>(::GetHandleSize(handle)/sizeof(DataBrowserItemID));
  HLock(handle);
  itemIDPtr = (DataBrowserItemID*) (*handle);
  MacDataViewListCtrlPtr->SetSelectedItems(NoOfItems,itemIDPtr,kDataBrowserItemsAssign);
  HUnlock(handle);
  DisposeHandle(handle);
} /* wxDataViewCtrl::SelectAll(void) */

void wxDataViewCtrl::Select(wxDataViewItem const& item)
{
  if (item.IsOk())
  {
   // variable definition and initialization:
    DataBrowserItemID                              itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
    MacDataViewListCtrlPtr->SetSelectedItems(1,&itemID,kDataBrowserItemsAdd);
  } /* if */
} /* wxDataViewCtrl::Select(wxDataViewItem const&) */

void wxDataViewCtrl::SetSelections(wxDataViewItemArray const& sel)
{
  size_t const NoOfSelections = sel.GetCount();

  DataBrowserItemID* itemIDs;

  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  itemIDs = new DataBrowserItemID[NoOfSelections];
  for (size_t i=0; i<NoOfSelections; ++i)
    itemIDs[i] = reinterpret_cast<DataBrowserItemID>(sel[i].GetID());
  MacDataViewListCtrlPtr->SetSelectedItems(NoOfSelections,itemIDs,kDataBrowserItemsAssign);
  delete[] itemIDs;
} /* wxDataViewCtrl::SetSelections(wxDataViewItemArray const&) */

void wxDataViewCtrl::Unselect(wxDataViewItem const& item)
{
  if (item.IsOk())
  {
   // variable definition and initialization:
    DataBrowserItemID                              itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
    MacDataViewListCtrlPtr->SetSelectedItems(1,&itemID,kDataBrowserItemsRemove);
  } /* if */
} /* wxDataViewCtrl::Unselect(wxDataViewItem const&) */

void wxDataViewCtrl::UnselectAll(void)
{
  DataBrowserItemID* itemIDPtr;
  
  Handle handle(::NewHandle(0));
  
  size_t NoOfItems;
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  wxCHECK_RET(MacDataViewListCtrlPtr->GetItems(kDataBrowserNoItem,true,kDataBrowserItemAnyState,handle) == noErr,_("Could not get items."));
  NoOfItems = static_cast<size_t>(::GetHandleSize(handle)/sizeof(DataBrowserItemID));
  HLock(handle);
  itemIDPtr = (DataBrowserItemID*) (*handle);
  MacDataViewListCtrlPtr->SetSelectedItems(NoOfItems,itemIDPtr,kDataBrowserItemsRemove);
  HUnlock(handle);
  DisposeHandle(handle);
} /* wxDataViewCtrl::UnselectAll(void) */

// data handling:
void wxDataViewCtrl::AddChildrenLevel(wxDataViewItem const& parentItem)
{
  int NoOfChildren;

  wxDataViewItemArray items;
  
  
  wxCHECK_RET(this->GetModel() != NULL,_("Model pointer not initialized."));
  NoOfChildren = this->GetModel()->GetChildren(parentItem,items);
  for (int i=0; i<NoOfChildren; ++i)
    (void) this->GetModel()->ItemAdded(parentItem,items[i]);
} /* wxDataViewCtrl::AddChildrenLevel(wxDataViewItem const&) */

wxDataViewColumn* wxDataViewCtrl::GetColumnPtr(DataBrowserPropertyID propertyID) const
{
 // variable definition:
  ColumnPointerHashMapType::const_iterator Result(this->m_ColumnPointers.find(propertyID));
  
  if (Result != this->m_ColumnPointers.end())
    return Result->second;
  else
    return NULL;
} /* wxDataViewCtrl::GetColumnPtr(DataBrowserPropertyID) const */

// inherited methods from wxDataViewCtrlBase
void wxDataViewCtrl::DoSetExpanderColumn(void)
{
  if (this->GetExpanderColumn() != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

    (void) MacDataViewListCtrlPtr->SetDisclosureColumn(this->GetExpanderColumn()->GetPropertyID());
  } /* if */
} /* wxDataViewCtrl::DoSetExpanderColumn(void) */

void wxDataViewCtrl::DoSetIndent(void)
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));  


  (void) MacDataViewListCtrlPtr->SetIndent(static_cast<float>(this->GetIndent()));
} /* wxDataViewCtrl::DoSetIndent(void) */

// event handling:
void wxDataViewCtrl::OnSize(wxSizeEvent& event)
{
  unsigned int const NoOfColumns = this->GetColumnCount();


  for (unsigned int i=0; i<NoOfColumns; ++i)
  {
   // variable definition and initialization:
    wxDataViewColumn* dataViewColumnPtr(this->GetColumn(i));
    
    if (dataViewColumnPtr != NULL)
    {
     // variable definition and initialization:
      wxDataViewCustomRenderer* dataViewCustomRendererPtr(dynamic_cast<wxDataViewCustomRenderer*>(dataViewColumnPtr->GetRenderer()));
    
      if (dataViewCustomRendererPtr != NULL)
        dataViewCustomRendererPtr->SetDC(NULL); // reset DC because DC has changed
    } /* if */
  } /* for */
  event.Skip();
} /* wxDataViewCtrl::OnSize(wxSizeEvent&) */

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl,wxDataViewCtrlBase)

BEGIN_EVENT_TABLE(wxDataViewCtrl,wxDataViewCtrlBase)
  EVT_SIZE(wxDataViewCtrl::OnSize)
END_EVENT_TABLE()

#endif
    // !wxUSE_GENERICDATAVIEWCTRL

#endif
    // wxUSE_DATAVIEWCTRL

