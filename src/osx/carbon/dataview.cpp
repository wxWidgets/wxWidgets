/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/datavgen.cpp
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

#include "wx/osx/carbon/databrow.h"

#ifndef WX_PRECOMP
    #include "wx/timer.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/icon.h"
#endif

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
#if wxCHECK_VERSION(2,9,0)
        DataViewCtrlPtr->HandleWindowEvent(DataViewEvent);
#else
        DataViewCtrlPtr->GetEventHandler()->ProcessEvent(DataViewEvent);
#endif
        return ::CallNextEventHandler(handler,EventReference);
      } /* if */
      else
        return eventNotHandledErr;
  } /* switch */

  return eventNotHandledErr;
} /* wxMacDataViewCtrlEventHandler(EventHandlerCallRef, EventRef, void*) */

static DataBrowserItemID* CreateDataBrowserItemIDArray(size_t& noOfEntries, wxDataViewItemArray const& items) // returns a newly allocated pointer to valid data browser item IDs
{
  size_t const noOfItems = items.GetCount();

  DataBrowserItemID* itemIDs(new DataBrowserItemID[noOfItems]);
 

 // convert all valid data view items to data browser items
  noOfEntries = 0;
  for (size_t i=0; i<noOfItems; ++i)
    if (items[i].IsOk())
    {
      itemIDs[noOfEntries] = reinterpret_cast<DataBrowserItemID>(items[i].GetID());
      ++noOfEntries;
    } /* if */
 // done:
  return itemIDs;
} /* CreateDataBrowserItemIDArray(size_t&, wxDataViewItemArray const&) */

#if wxCHECK_VERSION(2,9,0)
static bool InitializeColumnDescription(DataBrowserListViewColumnDesc& columnDescription, wxDataViewColumn const* columnPtr, DataBrowserPropertyID columnPropertyID, wxCFStringRef const& title)
#else
static bool InitializeColumnDescription(DataBrowserListViewColumnDesc& columnDescription, wxDataViewColumn const* columnPtr, DataBrowserPropertyID columnPropertyID, wxMacCFStringHolder const& title)
#endif
{
 // set properties for the column:
  columnDescription.propertyDesc.propertyID    = columnPropertyID;
  columnDescription.propertyDesc.propertyType  = columnPtr->GetRenderer()->GetPropertyType();
  columnDescription.propertyDesc.propertyFlags = kDataBrowserListViewSelectionColumn; // make the column selectable
  if (columnPtr->IsReorderable())
    columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewMovableColumn;
  if (columnPtr->IsResizeable())
  {
    columnDescription.headerBtnDesc.minimumWidth = 0;
    columnDescription.headerBtnDesc.maximumWidth = 30000; // 32767 is the theoretical maximum though but 30000 looks nicer
  } /* if */
  else
  {
    columnDescription.headerBtnDesc.minimumWidth = columnPtr->GetWidth();
    columnDescription.headerBtnDesc.maximumWidth = columnPtr->GetWidth();
  } /* if */
  if (columnPtr->IsSortable())
    columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewSortableColumn;
  if (columnPtr->GetRenderer()->GetMode() == wxDATAVIEW_CELL_EDITABLE)
    columnDescription.propertyDesc.propertyFlags |= kDataBrowserPropertyIsEditable;
  if ((columnDescription.propertyDesc.propertyType == kDataBrowserCustomType) ||
      (columnDescription.propertyDesc.propertyType == kDataBrowserDateTimeType) ||
      (columnDescription.propertyDesc.propertyType == kDataBrowserIconAndTextType) ||
      (columnDescription.propertyDesc.propertyType == kDataBrowserTextType))
    columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewTypeSelectColumn; // enables generally the possibility to have user input for the mentioned types
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
  columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewNoGapForIconInHeaderButton;
#endif
 // set header's properties:
  columnDescription.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;
  columnDescription.headerBtnDesc.titleOffset = 0;
  columnDescription.headerBtnDesc.titleString = ::CFStringCreateCopy(kCFAllocatorDefault,title);
  columnDescription.headerBtnDesc.initialOrder       = kDataBrowserOrderIncreasing; // choose one of the orders as "undefined" is not supported anyway (s. ControlDefs.h in the HIToolbox framework)
  columnDescription.headerBtnDesc.btnFontStyle.flags = kControlUseFontMask | kControlUseJustMask;
  switch (columnPtr->GetAlignment())
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
  if (columnPtr->GetBitmap().IsOk())
  {
    columnDescription.headerBtnDesc.btnContentInfo.contentType = kControlContentIconRef;
#if wxCHECK_VERSION(2,9,0)
    columnDescription.headerBtnDesc.btnContentInfo.u.iconRef = columnPtr->GetBitmap().GetIconRef();
#else
    columnDescription.headerBtnDesc.btnContentInfo.u.iconRef = columnPtr->GetBitmap().GetBitmapData()->GetIconRef();
#endif
  }
  else
  {
    // not text only as we otherwise could not add a bitmap later
    // columnDescription.headerBtnDesc.btnContentInfo.contentType = kControlContentTextOnly;
    columnDescription.headerBtnDesc.btnContentInfo.contentType = kControlContentIconRef;
    columnDescription.headerBtnDesc.btnContentInfo.u.iconRef = NULL;
  }
    
 // done:
  return true;
} /* InitializeColumnDescription(DataBrowserListViewColumnDesc&, wxDataViewColumn const*, DataBrowserPropertyID, wxMacCFStringHolder const&) */

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
    bool noFailureFlag = (!(parent.IsOk()) && (this->m_dataViewControlPtr->AddItem(kDataBrowserNoItem,&itemID) == noErr) ||
                  parent.IsOk()  && (this->m_dataViewControlPtr->AddItem(reinterpret_cast<DataBrowserItemID>(parent.GetID()),&itemID) == noErr));
    
    wxDataViewCtrl *dvc = (wxDataViewCtrl*) this->m_dataViewControlPtr->GetWXPeer();
    if (dvc->GetWindowStyle() & wxDV_VARIABLE_LINE_HEIGHT)
    {
        wxDataViewModel *model = GetOwner();
    
        int height = 20; // TODO find out standard height
        unsigned int num = dvc->GetColumnCount();
        unsigned int col;
        for (col = 0; col < num; col++)
        {
            wxDataViewColumn *column = dvc->GetColumn( col );
            if (column->IsHidden())
                continue;
                
            wxDataViewCustomRenderer *renderer = wxDynamicCast( column->GetRenderer(), wxDataViewCustomRenderer );
            if (renderer)
            {
                wxVariant value;
                model->GetValue( value, item, column->GetModelColumn() );
                renderer->SetValue( value );
                height = wxMax( height, renderer->GetSize().y );
            }
            
        }
        
        if (height > 20)
            this->m_dataViewControlPtr->SetRowHeight( itemID, height );
    }
            
    return noFailureFlag;
  } /* ItemAdded(wxDataViewItem const&, wxDataViewItem const&) */

  virtual bool ItemsAdded(wxDataViewItem const& parent, wxDataViewItemArray const& items)
  {
    bool noFailureFlag;

    DataBrowserItemID* itemIDs;
    
    size_t noOfEntries;
    

   // convert all valid data view items to data browser items:
    itemIDs = ::CreateDataBrowserItemIDArray(noOfEntries,items);
   // insert all valid items into control:
    noFailureFlag = ((noOfEntries == 0) ||
                     !(parent.IsOk()) && (this->m_dataViewControlPtr->AddItems(kDataBrowserNoItem,noOfEntries,itemIDs,kDataBrowserItemNoProperty) == noErr) ||
                     parent.IsOk() && (this->m_dataViewControlPtr->AddItems(reinterpret_cast<DataBrowserItemID>(parent.GetID()),noOfEntries,itemIDs,kDataBrowserItemNoProperty) == noErr));
   // give allocated array space free again:
    delete[] itemIDs;

    wxDataViewCtrl *dvc = (wxDataViewCtrl*) this->m_dataViewControlPtr->GetWXPeer();
    if (dvc->GetWindowStyle() & wxDV_VARIABLE_LINE_HEIGHT)
    {
        wxDataViewModel *model = GetOwner();
        unsigned int colnum = dvc->GetColumnCount();
        
        size_t i;
        size_t count = items.GetCount();
        for (i = 0; i < count; i++)
        { 
            wxDataViewItem item = items[i];
            DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    
            int height = 20; // TODO find out standard height
            unsigned int col;
            for (col = 0; col < colnum; col++)
            {
                wxDataViewColumn *column = dvc->GetColumn( col );
                if (column->IsHidden())
                    continue;      // skip it!
                
                if ((col != 0) && model->IsContainer(item) && !model->HasContainerColumns(item))
                    continue;      // skip it!
            
                wxDataViewCustomRenderer *renderer = wxDynamicCast( column->GetRenderer(), wxDataViewCustomRenderer );
                if (renderer)
                {
                    wxVariant value;
                    model->GetValue( value, item, column->GetModelColumn() );
                    renderer->SetValue( value );
                    height = wxMax( height, renderer->GetSize().y );
                }
            }
        
            if (height > 20)
                this->m_dataViewControlPtr->SetRowHeight( itemID, height );
        }
   }    
    
   // done:
    return noFailureFlag;
  } /* ItemsAdded(wxDataViewItem const&, wxDataViewItemArray const&) */

  virtual bool ItemChanged(wxDataViewItem const& item)
  {
    DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    
    
    wxCHECK_MSG(item.IsOk(),false,_("Changed item is invalid."));
    if (this->m_dataViewControlPtr->UpdateItems(&itemID) == noErr)
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetWXPeer()));
      
     // sent the equivalent wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetItem(item);
     // sent the equivalent wxWidget event:
#if wxCHECK_VERSION(2,9,0)
      dataViewCtrlPtr->HandleWindowEvent(dataViewEvent);
#else
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
#endif
     // done
      return true;
    } /* if */
    else
      return false;
  } /* ItemChanged(wxDataViewItem const&) */

  virtual bool ItemsChanged(wxDataViewItemArray const& items)
  {
    bool noFailureFlag;

    DataBrowserItemID* itemIDs;
    
    size_t noOfEntries;
    

   // convert all valid data view items to data browser items:
    itemIDs = ::CreateDataBrowserItemIDArray(noOfEntries,items);
   // change items (ATTENTION: ONLY ITEMS OF THE ROOT ARE CHANGED BECAUSE THE PARENT PARAMETER IS MISSING):
    noFailureFlag = (this->m_dataViewControlPtr->UpdateItems(kDataBrowserNoItem,noOfEntries,itemIDs,kDataBrowserItemNoProperty,kDataBrowserItemNoProperty) == noErr);
    if (noFailureFlag)
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetWXPeer()));
      
     // send for all changed items a wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      for (size_t i=0; i<noOfEntries; ++i)
      {
        dataViewEvent.SetItem(reinterpret_cast<void*>(itemIDs[i]));
#if wxCHECK_VERSION(2,9,0)
        dataViewCtrlPtr->HandleWindowEvent(dataViewEvent);
#else
        dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
#endif
      } /* for */
    } /* if */
   // release allocated array space:
    delete[] itemIDs;
   // done:
    return noFailureFlag;
  } /* ItemsChanged(wxDataViewItem const&) */

  virtual bool ItemDeleted(wxDataViewItem const& parent, wxDataViewItem const& item)
  {
    if (item.IsOk())
    {
     // variable definition and initialization:
      DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
      OSStatus          errorStatus;
      wxDataViewCtrl*   dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetWXPeer()));
      
     // when this method is called and currently an item is being edited this item may have already been deleted in the model (the passed item and the being edited item have
     // not to be identical because the being edited item might be below the passed item in the hierarchy);
     // to prevent the control trying to ask the model to update an already deleted item the control is informed that currently a deleting process
     // has been started and that variables can currently not be updated even when requested by the system:
      dataViewCtrlPtr->SetDeleting(true);
      errorStatus = this->m_dataViewControlPtr->RemoveItem(reinterpret_cast<DataBrowserItemID>(parent.GetID()),&itemID);
     // enable automatic updating again:
      dataViewCtrlPtr->SetDeleting(false);
      return (errorStatus == noErr);
    } /* if */
    else
      return false;
  } /* ItemDeleted(wxDataViewItem const&, wxDataViewItem const&) */

  virtual bool ItemsDeleted(wxDataViewItem const& parent, wxDataViewItemArray const& items)
  {
    bool noFailureFlag;

    DataBrowserItemID* itemIDs;
    
    wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetWXPeer()));

    size_t noOfEntries;
    

    wxCHECK_MSG(dataViewCtrlPtr != NULL,false,_("Data view control is not correctly initialized"));
   // convert all valid data view items to data browser items:
    itemIDs = ::CreateDataBrowserItemIDArray(noOfEntries,items);
   // when this method is called and currently an item is being edited this item may have already been deleted in the model (the passed item and the being edited item have
   // not to be identical because the being edited item might be below the passed item in the hierarchy);
   // to prevent the control trying to ask the model to update an already deleted item the control is informed that currently a deleting process
   // has been started and that variables can currently not be updated even when requested by the system:
    dataViewCtrlPtr->SetDeleting(true);
   // insert all valid items into control:
    noFailureFlag = ((noOfEntries == 0) ||
                     !(parent.IsOk()) && (this->m_dataViewControlPtr->RemoveItems(kDataBrowserNoItem,noOfEntries,itemIDs,kDataBrowserItemNoProperty) == noErr) ||
                     parent.IsOk() && (this->m_dataViewControlPtr->RemoveItems(reinterpret_cast<DataBrowserItemID>(parent.GetID()),noOfEntries,itemIDs,kDataBrowserItemNoProperty) == noErr));
   // enable automatic updating again:
    dataViewCtrlPtr->SetDeleting(false);
   // give allocated array space free again:
    delete[] itemIDs;
   // done:
    return noFailureFlag;
  } /* ItemsDeleted(wxDataViewItem const&, wxDataViewItemArray const&) */

  virtual bool ValueChanged(wxDataViewItem const& item, unsigned int col)
  {
    DataBrowserItemID itemID(reinterpret_cast<DataBrowserItemID>(item.GetID()));
    DataBrowserItemID parentID;
    
    DataBrowserPropertyID propertyID;

    wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetWXPeer()));


    wxCHECK_MSG(item.IsOk(),             false,_("Passed item is invalid."));
    wxCHECK_MSG(this->GetOwner() != NULL,false,_("Owner not initialized."));
    wxCHECK_MSG(dataViewCtrlPtr != NULL, false,_("Control is wrongly initialized."));
    parentID = reinterpret_cast<DataBrowserItemID>(this->GetOwner()->GetParent(item).GetID());
    if ((this->m_dataViewControlPtr->GetPropertyID(col,&propertyID) == noErr) &&
        (this->m_dataViewControlPtr->UpdateItems(parentID,1,&itemID,dataViewCtrlPtr->GetColumn(col)->GetPropertyID(),propertyID) == noErr))
    {
     // variable definition and initialization:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_ITEM_VALUE_CHANGED,dataViewCtrlPtr->GetId());

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetColumn(col);
      dataViewEvent.SetItem(item);
     // send the equivalent wxWidget event:
#if wxCHECK_VERSION(2,9,0)
      dataViewCtrlPtr->HandleWindowEvent(dataViewEvent);
#else
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
#endif
     // done
      return true;
    } /* if */
    else
      return false;
  } /* ValueChanged(wxDataViewItem const&, unsigned int) */

  virtual bool Cleared()
  {
    bool noFailureFlag = (this->m_dataViewControlPtr->RemoveItems() == noErr);
    wxDataViewItem item;
    wxDataViewItemArray array;
    GetOwner()->GetChildren( item, array );
    ItemsAdded( item, array );
    this->m_dataViewControlPtr->SetScrollPosition(0, 0);
    return noFailureFlag;
  } /* Cleared() */

  virtual void Resort()
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

wxDataViewCustomRenderer::~wxDataViewCustomRenderer()
{
    delete this->m_DCPtr;
} /* wxDataViewCustomRenderer::~wxDataViewCustomRenderer() */

void wxDataViewCustomRenderer::RenderText( const wxString &text, int xoffset, wxRect cell, wxDC *dc, int state )
{
    wxDataViewCtrl *view = GetOwner()->GetOwner();
//    wxColour col = (state & wxDATAVIEW_CELL_SELECTED) ? wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) : view->GetForegroundColour();
    wxColour col = (state & wxDATAVIEW_CELL_SELECTED) ? *wxWHITE : view->GetForegroundColour();
    dc->SetTextForeground(col);
    dc->DrawText( text, cell.x + xoffset, cell.y + ((cell.height - dc->GetCharHeight()) / 2));
}

wxDC* wxDataViewCustomRenderer::GetDC()
{
  if (this->m_DCPtr == NULL)
  {
    if ((GetOwner() == NULL) || (GetOwner()->GetOwner() == NULL))
      return NULL;
    this->m_DCPtr = new wxWindowDC(this->GetOwner()->GetOwner());
  } /* if */
  return this->m_DCPtr;
} /* wxDataViewCustomRenderer::GetDC() */

bool wxDataViewCustomRenderer::Render()
{
  return true;
} /* wxDataViewCustomRenderer::Render() */

void wxDataViewCustomRenderer::SetDC(wxDC* newDCPtr)
{
  delete this->m_DCPtr;
  this->m_DCPtr = newDCPtr;
} /* wxDataViewCustomRenderer::SetDC(wxDC*) */

WXDataBrowserPropertyType wxDataViewCustomRenderer::GetPropertyType() const
{
  return kDataBrowserCustomType;
} /* wxDataViewCustomRenderer::GetPropertyType() const */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewTextRenderer::wxDataViewTextRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                       :wxDataViewRenderer(varianttype,mode,align)
{
} /* wxDataViewTextRenderer::wxDataViewTextRenderer(wxString const&, wxDataViewCellMode, int) */

bool wxDataViewTextRenderer::Render()
{
  wxCHECK_MSG(this->GetValue().GetType() == this->GetVariantType(),false,wxString(_("Text renderer cannot render value; value type: ")) << this->GetValue().GetType());

 // variable definition:
#if wxCHECK_VERSION(2,9,0)
  wxCFStringRef cfString(this->GetValue().GetString(),(this->GetView()->GetFont().Ok() ? this->GetView()->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
#else
  wxMacCFStringHolder cfString(this->GetValue().GetString(),(this->GetView()->GetFont().Ok() ? this->GetView()->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
#endif
  return (::SetDataBrowserItemDataText(this->GetDataReference(),cfString) == noErr);
} /* wxDataViewTextRenderer::Render() */

WXDataBrowserPropertyType wxDataViewTextRenderer::GetPropertyType() const
{
  return kDataBrowserTextType;
} /* wxDataViewTextRenderer::GetPropertyType() const */

IMPLEMENT_CLASS(wxDataViewTextRenderer,wxDataViewRenderer)

// --------------------------------------------------------- 
// wxDataViewTextRendererAttr
// --------------------------------------------------------- 
#pragma mark -
wxDataViewTextRendererAttr::wxDataViewTextRendererAttr(wxString const& varianttype, wxDataViewCellMode mode, int align)
                           :wxDataViewTextRenderer(varianttype,mode,align)
{
} /* wxDataViewTextRendererAttr::wxDataViewTextRendererAttr(wxString const&, wxDataViewCellMode, int) */
    
IMPLEMENT_CLASS(wxDataViewTextRendererAttr,wxDataViewTextRenderer)

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewBitmapRenderer::wxDataViewBitmapRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                         :wxDataViewRenderer(varianttype,mode,align)
{
}

bool wxDataViewBitmapRenderer::Render()
 // This method returns 'true' if
 //  - the passed bitmap is valid and it could be assigned to the native data browser;
 //  - the passed bitmap is invalid (or is not initialized); this case simulates a non-existing bitmap.
 // In all other cases the method returns 'false'.
{
  wxCHECK_MSG(this->GetValue().GetType() == this->GetVariantType(),false,wxString(_("Bitmap renderer cannot render value; value type: ")) << this->GetValue().GetType());

 // variable definition:
  wxBitmap bitmap;
  
  bitmap << this->GetValue();
  if (bitmap.Ok())
#if wxCHECK_VERSION(2,9,0)
    return (::SetDataBrowserItemDataIcon(this->GetDataReference(),bitmap.GetIconRef()) == noErr);
#else
    return (::SetDataBrowserItemDataIcon(this->GetDataReference(),bitmap.GetBitmapData()->GetIconRef()) == noErr);
#endif
  else
    return true;
} /* wxDataViewBitmapRenderer::Render() */

WXDataBrowserPropertyType wxDataViewBitmapRenderer::GetPropertyType() const
{
  return kDataBrowserIconType;
} /* wxDataViewBitmapRenderer::GetPropertyType() const */

IMPLEMENT_CLASS(wxDataViewBitmapRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewIconTextRenderer::wxDataViewIconTextRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                           :wxDataViewRenderer(varianttype,mode)
{
}

bool wxDataViewIconTextRenderer::Render()
{
  wxCHECK_MSG(this->GetValue().GetType() == this->GetVariantType(),false,wxString(_("Icon & text renderer cannot render value; value type: ")) << this->GetValue().GetType());

 // variable definition:
  wxDataViewIconText iconText;
  
  iconText << this->GetValue();

 // variable definition:
#if wxCHECK_VERSION(2,9,0)
  wxCFStringRef cfString(iconText.GetText(),(this->GetView()->GetFont().Ok() ? this->GetView()->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
#else
  wxMacCFStringHolder cfString(iconText.GetText(),(this->GetView()->GetFont().Ok() ? this->GetView()->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
#endif

  if (iconText.GetIcon().IsOk())
    if (::SetDataBrowserItemDataIcon(this->GetDataReference(),MAC_WXHICON(iconText.GetIcon().GetHICON())) != noErr)
      return false;
  return (::SetDataBrowserItemDataText(this->GetDataReference(),cfString) == noErr);
} /* wxDataViewIconTextRenderer::Render() */

WXDataBrowserPropertyType wxDataViewIconTextRenderer::GetPropertyType() const
{
  return kDataBrowserIconAndTextType;
} /* wxDataViewIconTextRenderer::GetPropertyType() const */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewIconTextRenderer,wxDataViewRenderer)


// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewToggleRenderer::wxDataViewToggleRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                         :wxDataViewRenderer(varianttype,mode)
{
}

bool wxDataViewToggleRenderer::Render()
{
  wxCHECK_MSG(this->GetValue().GetType() == this->GetVariantType(),false,wxString(_("Toggle renderer cannot render value; value type: ")) << this->GetValue().GetType());
  return (::SetDataBrowserItemDataButtonValue(this->GetDataReference(),this->GetValue().GetBool()) == noErr);
} /* wxDataViewToggleRenderer::Render() */

WXDataBrowserPropertyType wxDataViewToggleRenderer::GetPropertyType() const
{
  return kDataBrowserCheckboxType;
} /* wxDataViewToggleRenderer::GetPropertyType() const */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewToggleRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewProgressRenderer::wxDataViewProgressRenderer(wxString const& label, wxString const& varianttype, wxDataViewCellMode mode, int align)
                           :wxDataViewRenderer(varianttype,mode,align)
{
}

bool wxDataViewProgressRenderer::Render()
{
  wxCHECK_MSG(this->GetValue().GetType() == this->GetVariantType(),false,wxString(_("Progress renderer cannot render value type; value type: ")) << this->GetValue().GetType());
  return ((::SetDataBrowserItemDataMinimum(this->GetDataReference(),  0)                        == noErr) &&
          (::SetDataBrowserItemDataMaximum(this->GetDataReference(),100)                        == noErr) &&
          (::SetDataBrowserItemDataValue  (this->GetDataReference(),this->GetValue().GetLong()) == noErr));
} /* wxDataViewProgressRenderer::Render() */

WXDataBrowserPropertyType wxDataViewProgressRenderer::GetPropertyType() const
{
  return kDataBrowserProgressBarType;
} /* wxDataViewProgressRenderer::GetPropertyType() const */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewProgressRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------
#pragma mark -
wxDataViewDateRenderer::wxDataViewDateRenderer(wxString const& varianttype, wxDataViewCellMode mode, int align)
                       :wxDataViewRenderer(varianttype,mode,align)
{
}

bool wxDataViewDateRenderer::Render()
{
  wxCHECK_MSG(this->GetValue().GetType() == this->GetVariantType(),false,wxString(_("Date renderer cannot render value; value type: ")) << this->GetValue().GetType());
  return (::SetDataBrowserItemDataDateTime(this->GetDataReference(),this->GetValue().GetDateTime().Subtract(wxDateTime(1,wxDateTime::Jan,1904)).GetSeconds().GetLo()) == noErr);
} /* wxDataViewDateRenderer::Render() */

WXDataBrowserPropertyType wxDataViewDateRenderer::GetPropertyType() const
{
  return kDataBrowserDateTimeType;
} /* wxDataViewDateRenderer::GetPropertyType() const */

IMPLEMENT_ABSTRACT_CLASS(wxDataViewDateRenderer,wxDataViewRenderer)

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------
#pragma mark -
wxDataViewColumn::wxDataViewColumn(wxString const &title, wxDataViewRenderer *cell, unsigned int model_column, int width, wxAlignment align, int flags)
                 :wxDataViewColumnBase(title,cell,model_column,width,align,flags), m_ascending(true),
                  m_flags(flags & ~(wxDATAVIEW_COL_HIDDEN)), m_maxWidth(30000), m_minWidth(0), m_width(width >= 0 ? width : wxDVC_DEFAULT_WIDTH),
                  m_alignment(align), m_title(title)
{
} /* wxDataViewColumn::wxDataViewColumn(wxString const &title, wxDataViewRenderer*, unsigned int, int, wxAlignment, int) */

wxDataViewColumn::wxDataViewColumn(wxBitmap const& bitmap, wxDataViewRenderer *cell, unsigned int model_column, int width, wxAlignment align, int flags)
                 :wxDataViewColumnBase(bitmap,cell,model_column,width,align,flags), m_ascending(true),
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
#if wxCHECK_VERSION(2,9,0)
        headerDescription.btnContentInfo.u.iconRef = this->GetBitmap().GetIconRef();
#else
        headerDescription.btnContentInfo.u.iconRef = this->GetBitmap().GetBitmapData()->GetIconRef();
#endif
      else
        headerDescription.btnContentInfo.u.iconRef = NULL;
      wxCHECK_RET(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription) == noErr,_("Could not set icon."));
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetBitmap(wxBitmap const&) */

void wxDataViewColumn::SetFlags(int flags)
{
  this->SetHidden     ((flags & wxDATAVIEW_COL_HIDDEN)      != 0);
  this->SetReorderable((flags & wxDATAVIEW_COL_REORDERABLE) != 0);
  this->SetResizeable ((flags & wxDATAVIEW_COL_RESIZABLE)   != 0);
  this->SetSortable   ((flags & wxDATAVIEW_COL_SORTABLE)    != 0);
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

void wxDataViewColumn::SetReorderable(bool reorderable)
{
 // first set the internal flag of the column:
  if (reorderable)
    this->m_flags |= wxDATAVIEW_COL_REORDERABLE;
  else
    this->m_flags &= ~wxDATAVIEW_COL_REORDERABLE;
 // if the column is associated with a control change also immediately the flags of the control:
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner()); // variable definition and initialization

  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    DataBrowserPropertyFlags                       flags;
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    wxCHECK_RET(macDataViewListCtrlPtr != NULL,                                                 _("Valid pointer to native data view control does not exist"));
    wxCHECK_RET(macDataViewListCtrlPtr->GetPropertyFlags(this->GetPropertyID(),&flags) == noErr,_("Could not get property flags."));
    if (reorderable)
      flags |= kDataBrowserListViewMovableColumn;
    else
      flags &= ~kDataBrowserListViewMovableColumn;
    wxCHECK_RET(macDataViewListCtrlPtr->SetPropertyFlags(this->GetPropertyID(),flags) == noErr,_("Could not set property flags."));
  } /* if */
} /* wxDataViewColumn::SetReorderable(bool) */

void wxDataViewColumn::SetResizeable(bool resizeable)
{
 // first set the internal flag of the column:
  if (resizeable)
    this->m_flags |= wxDATAVIEW_COL_RESIZABLE;
  else
    this->m_flags &= ~wxDATAVIEW_COL_RESIZABLE;
 // if the column is associated with a control change also immediately the flags of the control:
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner()); // variable definition and initialization

  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    if (macDataViewListCtrlPtr != NULL)
    {
     // variable definition and initialization:
      DataBrowserListViewHeaderDesc headerDescription;
      
      verify_noerr(macDataViewListCtrlPtr->GetHeaderDesc(this->GetPropertyID(),&headerDescription));
      if (resizeable) {
        headerDescription.minimumWidth = 0;
        headerDescription.maximumWidth = 30000;
      }
      else {
        headerDescription.minimumWidth = this->m_width;
        headerDescription.maximumWidth = this->m_width;
      }
      verify_noerr(macDataViewListCtrlPtr->SetHeaderDesc(this->GetPropertyID(),&headerDescription));
      macDataViewListCtrlPtr->SetSortProperty(this->GetPropertyID());
    } /* if */
  } /* if */
} /* wxDataViewColumn::SetResizeable(bool) */

void wxDataViewColumn::SetSortable(bool sortable)
{
 // first set the internal flag of the column:
  if (sortable)
    this->m_flags |= wxDATAVIEW_COL_SORTABLE;
  else
    this->m_flags &= ~wxDATAVIEW_COL_SORTABLE;
 // if the column is associated with a control change also immediately the flags of the control:
  wxDataViewCtrl* dataViewCtrlPtr(this->GetOwner()); // variable definition and initialization

  if (dataViewCtrlPtr != NULL)
  {
   // variable definition and initialization:
    DataBrowserPropertyFlags                       flags;
    wxMacDataViewDataBrowserListViewControlPointer macDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(dataViewCtrlPtr->GetPeer()));
    
    wxCHECK_RET(macDataViewListCtrlPtr != NULL,                                                 _("Valid pointer to native data view control does not exist"));
    wxCHECK_RET(macDataViewListCtrlPtr->GetPropertyFlags(this->GetPropertyID(),&flags) == noErr,_("Could not get property flags."));
    if (sortable)
      flags |= kDataBrowserListViewSortableColumn;
    else
      flags &= ~kDataBrowserListViewSortableColumn;
    wxCHECK_RET(macDataViewListCtrlPtr->SetPropertyFlags(this->GetPropertyID(),flags) == noErr,_("Could not set property flags."));
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
      macDataViewListCtrlPtr->SetSortProperty(this->GetPropertyID());
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
#if wxCHECK_VERSION(2,9,0)
      wxCFStringRef           cfTitle(title,(dataViewCtrlPtr->GetFont().Ok() ? dataViewCtrlPtr->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
#else
      wxMacCFStringHolder           cfTitle(title,(dataViewCtrlPtr->GetFont().Ok() ? dataViewCtrlPtr->GetFont().GetEncoding() : wxLocale::GetSystemEncoding()));
#endif
      
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


wxDataViewCtrl::~wxDataViewCtrl()
{
   ClearColumns();
}

#pragma mark -
void wxDataViewCtrl::Init()
{
  this->m_CustomRendererPtr = NULL;
  this->m_Deleting          = false;
  this->m_macIsUserPane     = false;
  this->m_cgContext         = NULL;
} /* wxDataViewCtrl::Init() */

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator )
{
  if (!(this->wxControl::Create(parent,id,pos,size,style & ~(wxHSCROLL | wxVSCROLL),validator)))
    return false;

#ifdef __WXMAC__
  MacSetClipChildren(true) ;
#endif

  this->m_peer = new wxMacDataViewDataBrowserListViewControl(this,pos,size,style);
  
  if ( style & wxBORDER_NONE )
    this->m_peer->SetData( kControlNoPart, kControlDataBrowserIncludesFrameAndFocusTag, (Boolean) false ) ;

  this->MacPostControlCreate(pos,size);
  ::SetAutomaticControlDragTrackingEnabledForWindow(::GetControlOwner(this->m_peer->GetControlRef()),true);

  InstallControlEventHandler(this->m_peer->GetControlRef(),GetwxMacDataViewCtrlEventHandlerUPP(),GetEventTypeCount(eventList),eventList,this,NULL);

  ::SetDataBrowserTableViewHiliteStyle( this->m_peer->GetControlRef(), kDataBrowserTableViewFillHilite );
  
  return true;
} /* wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) */

/*static*/
wxVisualAttributes wxDataViewCtrl::GetClassDefaultAttributes(wxWindowVariant variant)
{
    wxVisualAttributes attr;

    attr.colFg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
    attr.colBg = wxSystemSettings::GetColour( wxSYS_COLOUR_LISTBOX );
    attr.font.MacCreateFromThemeFont(kThemeViewsFont);

    return attr;
}

bool wxDataViewCtrl::AssociateModel(wxDataViewModel* model)
{
  if (!wxDataViewCtrlBase::AssociateModel(model))
    return false;
  
  model->AddNotifier(new wxMacDataViewModelNotifier(dynamic_cast<wxMacDataViewDataBrowserListViewControl*>(this->m_peer)));

  return true;
}

bool wxDataViewCtrl::AppendColumn(wxDataViewColumn* columnPtr)
{
   return InsertColumn( kDataBrowserListViewAppendColumn, columnPtr );
} 

bool wxDataViewCtrl::PrependColumn(wxDataViewColumn* columnPtr)
{
   return InsertColumn( 0, columnPtr );
}

bool wxDataViewCtrl::InsertColumn(unsigned int pos, wxDataViewColumn* columnPtr)
{
  DataBrowserListViewColumnDesc columnDescription;

  DataBrowserPropertyID NewPropertyID;

  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

#if wxCHECK_VERSION(2,9,0)
  wxCFStringRef title(columnPtr->GetTitle(),this->m_font.Ok() ? this->GetFont().GetEncoding() : wxLocale::GetSystemEncoding());
#else
  wxMacCFStringHolder title(columnPtr->GetTitle(),this->m_font.Ok() ? this->GetFont().GetEncoding() : wxLocale::GetSystemEncoding());
#endif


 // first, some error checking:
  wxCHECK_MSG(MacDataViewListCtrlPtr != NULL,                                    false,_("m_peer is not or incorrectly initialized"));
  wxCHECK_MSG(columnPtr != NULL,                                                 false,_("Column pointer must not be NULL."));
  wxCHECK_MSG(columnPtr->GetRenderer() != NULL,                                  false,_("Column does not have a renderer."));
  wxCHECK_MSG(this->GetModel() != NULL,                                          false,_("No model associated with control."));
  wxCHECK_MSG((columnPtr->GetModelColumn() >= 0) &&
              (columnPtr->GetModelColumn() < this->GetModel()->GetColumnCount()),false,_("Column's model column has no equivalent in the associated model."));

 // try to get new ID for the column:
  wxCHECK_MSG(MacDataViewListCtrlPtr->GetFreePropertyID(&NewPropertyID) == noErr,false,_("Cannot create new column's ID. Probably max. number of columns reached."));
 // full column variable initialization:
  columnPtr->SetPropertyID(NewPropertyID);
 // add column to wxWidget's internal structure:
  wxCHECK_MSG(this->wxDataViewCtrlBase::AppendColumn(columnPtr) &&
              this->m_ColumnPointers.insert(ColumnPointerHashMapType::value_type(NewPropertyID,columnPtr)).second,false,_("Could not add column to internal structures."));
 // create a column description and add column to the native control:
  wxCHECK_MSG(::InitializeColumnDescription(columnDescription,columnPtr,NewPropertyID,title),                 false,_("Column description could not be initialized."));
  wxCHECK_MSG(MacDataViewListCtrlPtr->AddColumn(&columnDescription,pos) == noErr,false,_("Column could not be added."));

 // final adjustments for the layout:
  wxCHECK_MSG(MacDataViewListCtrlPtr->SetColumnWidth(NewPropertyID,columnPtr->GetWidth()) == noErr,false,_("Column width could not be set."));

 // make sure that the data is up-to-date...
 // if the newly appended column is the first column add the initial data to the control and mark the column as an expander column,
 // otherwise ask the control to 'update' the data in the newly appended column:
  if (this->GetColumnCount() == 1)
  {
    this->SetExpanderColumn(columnPtr);
    this->AddChildrenLevel(wxDataViewItem());
  } /* if */
  else
    MacDataViewListCtrlPtr->UpdateItems(kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty,NewPropertyID);
 // done:
  return true;
}

bool wxDataViewCtrl::ClearColumns()
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  while (this->m_ColumnPointers.begin() != this->m_ColumnPointers.end())
  {
    wxCHECK_MSG(MacDataViewListCtrlPtr->RemoveColumnByProperty(this->m_ColumnPointers.begin()->first) == noErr,false,_("Could not remove column."));
    delete this->m_ColumnPointers.begin()->second;
    this->m_ColumnPointers.erase(this->m_ColumnPointers.begin());
  } /* while */
  return true;
} 

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

unsigned int wxDataViewCtrl::GetColumnCount() const
{
  return this->m_ColumnPointers.size();
} /* wxDataViewCtrl::GetColumnCount() const */

int wxDataViewCtrl::GetColumnPosition(wxDataViewColumn const* columnPtr) const
{
  if (columnPtr != NULL)
  {
   // variable definition and initialization:
    DataBrowserTableViewColumnIndex                Position;
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
    
    wxCHECK_MSG(MacDataViewListCtrlPtr->GetColumnIndex(columnPtr->GetPropertyID(),&Position) == noErr,-1,_("Could not determine column's position"));
    return static_cast<int>(Position);
  } /* if */
  else
    return wxNOT_FOUND;
} /* wxDataViewCtrl::GetColumnPosition(wxDataViewColumn const*) const */

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

wxDataViewColumn* wxDataViewCtrl::GetSortingColumn() const
{
  DataBrowserPropertyID propertyID;

  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  
  
  if (MacDataViewListCtrlPtr->GetSortProperty(&propertyID) == noErr)
    return this->GetColumnPtr(propertyID);
  else
    return NULL;
} /* wxDataViewCtrl::GetSortingColumn() const */

unsigned int wxDataViewCtrl::GetCount() const
{
  ItemCount noOfItems;


  wxCHECK_MSG(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer)->GetItemCount(&noOfItems) == noErr,0,_("Could not determine number of items"));
  return noOfItems;
} /* wxDataViewCtrl::GetCount() const */

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

wxDataViewItem wxDataViewCtrl::GetSelection() const
{
  wxArrayDataBrowserItemID itemIDs;
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  if (MacDataViewListCtrlPtr->GetSelectedItemIDs(itemIDs) > 0)
    return wxDataViewItem(reinterpret_cast<void*>(itemIDs[0]));
  else
    return wxDataViewItem();
} /* wxDataViewCtrl::GetSelection() const */

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

void wxDataViewCtrl::SelectAll()
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
} /* wxDataViewCtrl::SelectAll() */

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

void wxDataViewCtrl::UnselectAll()
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
} /* wxDataViewCtrl::UnselectAll() */

// data handling:
void wxDataViewCtrl::AddChildrenLevel(wxDataViewItem const& parentItem)
{
  int NoOfChildren;

  wxDataViewItemArray items;
  
  
  wxCHECK_RET(this->GetModel() != NULL,_("Model pointer not initialized."));
  NoOfChildren = this->GetModel()->GetChildren(parentItem,items);
#if 0
  for (int i=0; i<NoOfChildren; ++i)
    (void) this->GetModel()->ItemAdded(parentItem,items[i]);
#else
  (void) this->GetModel()->ItemsAdded(parentItem,items);
#endif
} 

void wxDataViewCtrl::FinishCustomItemEditing()
{
  if (this->GetCustomRendererItem().IsOk())
  {
    this->GetCustomRendererPtr()->FinishEditing();
    this->SetCustomRendererItem(wxDataViewItem());
    this->SetCustomRendererPtr (NULL);
  }
} 

wxDataViewColumn* wxDataViewCtrl::GetColumnPtr(WXDataBrowserPropertyID propertyID) const
{
 // variable definition:
  ColumnPointerHashMapType::const_iterator Result(this->m_ColumnPointers.find(propertyID));
  
  if (Result != this->m_ColumnPointers.end())
    return Result->second;
  else
    return NULL;
} 

// inherited methods from wxDataViewCtrlBase
void wxDataViewCtrl::DoSetExpanderColumn()
{
  if (this->GetExpanderColumn() != NULL)
  {
   // variable definition and initialization:
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

    (void) MacDataViewListCtrlPtr->SetDisclosureColumn(this->GetExpanderColumn()->GetPropertyID(),false); // second parameter explicitely passed to ensure that arrow is centered
  } 
} 

void wxDataViewCtrl::DoSetIndent()
{
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));  

  (void) MacDataViewListCtrlPtr->SetIndent(static_cast<float>(this->GetIndent()));
} 

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
  
  wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));
  ControlRef ref = MacDataViewListCtrlPtr->GetControlRef();
  if (NoOfColumns == 1)
  {
     ::SetDataBrowserHasScrollBars( ref, false, true );
     ::AutoSizeDataBrowserListViewColumns( ref );
  }
  if (NoOfColumns > 1)
  {
     ::SetDataBrowserHasScrollBars( ref, true, true );
  }
  
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

