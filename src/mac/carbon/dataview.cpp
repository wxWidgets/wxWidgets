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
// wxMacDataViewListModelNotifier
// ---------------------------------------------------------
#pragma mark -
class wxMacDataViewListModelNotifier : public wxDataViewListModelNotifier
{
public:
  wxMacDataViewListModelNotifier(wxMacDataViewDataBrowserListViewControl* initDataViewControlPtr) : m_dataViewControlPtr(initDataViewControlPtr)
  {
  }

  virtual bool RowAppended(void)
  {
    DataBrowserItemID newRowID;
    
    
    if ((this->m_dataViewControlPtr->GetFreeItemID(&newRowID) == noErr) &&
         this->m_dataViewControlPtr->InsertItemIDRowPair(newRowID,this->GetOwner()->GetRowCount()-1) &&
        (this->m_dataViewControlPtr->AddItem(kDataBrowserNoItem,&newRowID) == noErr))
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      
     // sent the equivalent wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ROW_APPENDED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetRow(this->GetOwner()->GetRowCount()-1);
     // sent the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
  }
  virtual bool RowChanged(unsigned int row)
  {
    DataBrowserItemID itemID;
    
    
    if (this->m_dataViewControlPtr->GetItemID(row,&itemID) == noErr)
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      
     // sent the equivalent wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ROW_CHANGED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetRow(row);
     // sent the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
  }
  virtual bool RowDeleted(unsigned int row)
  {
    DataBrowserItemID itemID;


    if (this->m_dataViewControlPtr->GetItemID(row,&itemID) == noErr)
    {
     // variable definition:
      unsigned long rowIndex;
      
      wxCHECK_MSG(this->m_dataViewControlPtr->GetRowIndex(rowIndex,itemID) && this->m_dataViewControlPtr->DeleteItemIDRowPair(itemID),false,_("Unable to delete row data."));
      this->m_dataViewControlPtr->RenumberItemIDRowIndicesDeletion(rowIndex);
      if (this->m_dataViewControlPtr->RemoveItem(kDataBrowserNoItem,&itemID) == noErr)
      {
        wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
        
       // sent the equivalent wxWidget event:
        wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ROW_DELETED,dataViewCtrlPtr->GetId()); // variable defintion

        dataViewEvent.SetEventObject(dataViewCtrlPtr);
        dataViewEvent.SetRow(row);
       // sent the equivalent wxWidget event:
        dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
       // done
        return true;
      } /* if */
      else
        return false;
    } /* if */
    else
      return false;
  }
  virtual bool RowInserted(unsigned int before)
  {
    DataBrowserItemID newRowID;


    if (this->m_dataViewControlPtr->GetFreeItemID(&newRowID) == noErr)
    {
      this->m_dataViewControlPtr->RenumberItemIDRowIndicesInsertion(before);
      if (this->m_dataViewControlPtr->InsertItemIDRowPair(newRowID,before) && (this->m_dataViewControlPtr->AddItem(kDataBrowserNoItem,&newRowID) == noErr))
      {
        wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
        
       // sent the equivalent wxWidget event:
        wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ROW_INSERTED,dataViewCtrlPtr->GetId()); // variable defintion

        dataViewEvent.SetEventObject(dataViewCtrlPtr);
        dataViewEvent.SetRow(before);
       // sent the equivalent wxWidget event:
        dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
       // done
        return true;
      } /* if */
      else
        return false;
    } /* if */
    else
      return false;
  }
  virtual bool RowPrepended(void)
  {
    DataBrowserItemID newRowID;


    if (this->m_dataViewControlPtr->GetFreeItemID(&newRowID) == noErr)
    {
      this->m_dataViewControlPtr->RenumberItemIDRowIndicesInsertion(0);
      if (this->m_dataViewControlPtr->InsertItemIDRowPair(newRowID,0) && (this->m_dataViewControlPtr->AddItem(kDataBrowserNoItem,&newRowID) == noErr))
      {
        wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
        
       // sent the equivalent wxWidget event:
        wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ROW_PREPENDED,dataViewCtrlPtr->GetId()); // variable defintion

        dataViewEvent.SetEventObject(dataViewCtrlPtr);
        dataViewEvent.SetRow(0);
       // sent the equivalent wxWidget event:
        dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
       // done
        return true;
      } /* if */
      else
        return false;
    } /* if */
    else
      return false;
  }
  virtual bool ValueChanged(unsigned int col, unsigned int row)
  {
    DataBrowserItemID itemID;
    
    DataBrowserPropertyID propertyID;

    
    wxDataViewCtrl*  dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
    
    if ((dataViewCtrlPtr != NULL) && (this->m_dataViewControlPtr->GetItemID(row,&itemID) == noErr) && (this->m_dataViewControlPtr->GetColumnID(col,&propertyID) == noErr) &&
        (this->m_dataViewControlPtr->UpdateItems(kDataBrowserNoItem,1,&itemID,dataViewCtrlPtr->GetColumn(col)->GetPropertyID(),propertyID) == noErr))
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      
     // sent the equivalent wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
      dataViewEvent.SetColumn(col);
      dataViewEvent.SetRow(row);
     // sent the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
  }
  virtual bool RowsReordered(unsigned int* new_order)
  {
    DataBrowserPropertyID propertyId;

        
    if ((new_order != NULL) && (this->m_dataViewControlPtr->GetSortProperty(&propertyId) == noErr) && (propertyId >= kMinPropertyID))
    {
      DataBrowserSortOrder sortOrder;
      unsigned long        column;
          
      if ((this->m_dataViewControlPtr->GetSortOrder(&sortOrder) == noErr) && (this->m_dataViewControlPtr->GetPropertyColumn(propertyId,&column) == noErr))
      {
        wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
        
        if (dataViewCtrlPtr->GetColumn(column)->IsSortOrderAscending())
          this->m_dataViewControlPtr->RenumberItemIDRowIndices(new_order);
        else // the sort order of the control is descending but the passed sort order is always ordered top - down
        {
          ItemCount itemCount;

          if (this->m_dataViewControlPtr->GetItemCount(&itemCount) != noErr)
            return false;
          unsigned int* reversedOrder(new unsigned int[itemCount]);
          
          for (ItemCount i=0; i<itemCount; ++i)
            reversedOrder[i] = itemCount-new_order[i];
          this->m_dataViewControlPtr->RenumberItemIDRowIndices(reversedOrder);
          delete[] reversedOrder;
        } /* if */
        if (this->m_dataViewControlPtr->UpdateItems())
        {
         // sent the equivalent wxWidget event:
          wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_ROWS_REORDERED,dataViewCtrlPtr->GetId()); // variable defintion

          dataViewEvent.SetEventObject(dataViewCtrlPtr);
         // sent the equivalent wxWidget event:
          dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
         // done
          return true;
        } /* if */
        else
          return false;
      } /* if */
      else
        return false;
    } /* if */
    else
      return false;
  }
  virtual bool Cleared(void)
  {
    this->m_dataViewControlPtr->ClearItemIDRowPairs();
    if (this->m_dataViewControlPtr->RemoveItems() == noErr)
    {
      wxDataViewCtrl* dataViewCtrlPtr(dynamic_cast<wxDataViewCtrl*>(this->m_dataViewControlPtr->GetPeer()));
      
     // sent the equivalent wxWidget event:
      wxDataViewEvent dataViewEvent(wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED,dataViewCtrlPtr->GetId()); // variable defintion

      dataViewEvent.SetEventObject(dataViewCtrlPtr);
     // sent the equivalent wxWidget event:
      dataViewCtrlPtr->GetEventHandler()->ProcessEvent(dataViewEvent);
     // done
      return true;
    } /* if */
    else
      return false;
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
                         :wxDataViewRenderer(varianttype,mode,align), m_editorCtrlPtr(NULL)
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
  return false;
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
      return false;
  } /* if */
  else
    return false;
} /* wxDataViewBitmapRenderer::Render(void) */

IMPLEMENT_CLASS(wxDataViewBitmapRenderer,wxDataViewRenderer)

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
                 :wxDataViewColumnBase(title,cell,model_column,width,wxALIGN_CENTER,flags), m_alignment(align), m_ascending(true),
                  m_flags(flags & ~(wxDATAVIEW_COL_HIDDEN | wxDATAVIEW_COL_RESIZABLE)), m_minWidth(0), m_maxWidth(std::numeric_limits<int>::max()), m_width(width), m_title(title)
{
} /* wxDataViewColumn::wxDataViewColumn(wxString const &title, wxDataViewRenderer*, unsigned int, int, wxAlignment, int) */

wxDataViewColumn::wxDataViewColumn(wxBitmap const& bitmap, wxDataViewRenderer *cell, unsigned int model_column, int width, wxAlignment align, int flags)
                 :wxDataViewColumnBase(bitmap,cell,model_column,width,wxALIGN_CENTER,flags), m_ascending(true), m_alignment(align),
                  m_flags(flags & (wxDATAVIEW_COL_HIDDEN | wxDATAVIEW_COL_RESIZABLE)), m_minWidth(0), m_maxWidth(std::numeric_limits<int>::max()), m_width(width)
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
  this->m_macIsUserPane = false;
  this->m_NotifierPtr   = NULL;
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

  InstallControlEventHandler(this->m_peer->GetControlRef(),GetwxMacDataViewCtrlEventHandlerUPP(),GetEventTypeCount(eventList),eventList,this,NULL);

  return true;
} /* wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator) */

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

bool wxDataViewCtrl::AssociateModel(wxDataViewListModel *model)
{
  if (!wxDataViewCtrlBase::AssociateModel(model))
    return false;
  
  this->m_NotifierPtr = new wxMacDataViewListModelNotifier(dynamic_cast<wxMacDataViewDataBrowserListViewControl*>(this->m_peer));
  model->AddNotifier(this->m_NotifierPtr);

  return true;
}

bool wxDataViewCtrl::AppendColumn(wxDataViewColumn* dataViewColumnPtr)
{
  wxCHECK_MSG(dataViewColumnPtr != NULL,               false,_("Column pointer must not be NULL."));
  wxCHECK_MSG(dataViewColumnPtr->GetRenderer() != NULL,false,_("Column does not have a renderer."));
  if (this->wxDataViewCtrlBase::AppendColumn(dataViewColumnPtr))
  {
   // variable definition:
    DataBrowserPropertyID                          FreeID;
    DataBrowserListViewColumnDesc                  columnDescription;
    wxMacCFStringHolder                            cfTitle(dataViewColumnPtr->GetTitle(),(this->m_font.Ok() ? this->m_font.GetEncoding() : wxLocale::GetSystemEncoding()));
    wxMacDataViewDataBrowserListViewControlPointer MacDataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

   // initialize column description:
    wxCHECK_MSG (MacDataViewListCtrlPtr != NULL,false,_("m_peer is not or incorrectly initialized"));
    verify_noerr(MacDataViewListCtrlPtr->GetFreePropertyID(&FreeID));
    dataViewColumnPtr->SetPropertyID(FreeID);
    columnDescription.propertyDesc.propertyID = FreeID;
    columnDescription.propertyDesc.propertyType = dataViewColumnPtr->GetRenderer()->GetPropertyType();
    columnDescription.propertyDesc.propertyFlags = kDataBrowserListViewSelectionColumn;
    if (dataViewColumnPtr->IsSortable())
      columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewSortableColumn;
    if (dataViewColumnPtr->IsResizeable())
      columnDescription.propertyDesc.propertyFlags |= kDataBrowserListViewMovableColumn;
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
    columnDescription.headerBtnDesc.minimumWidth = 0;
    columnDescription.headerBtnDesc.maximumWidth = 30000;
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
    verify_noerr(MacDataViewListCtrlPtr->AddColumn(&columnDescription,kDataBrowserListViewAppendColumn));
   // final adjustments for the layout:
    if (dataViewColumnPtr->GetWidth() <= 0)
    {
     // variable definition:
      UInt16 defaultColumnWidth;
      
      MacDataViewListCtrlPtr->GetDefaultColumnWidth(&defaultColumnWidth);
      dataViewColumnPtr->SetWidth(defaultColumnWidth);
    } /* if */
    verify_noerr(MacDataViewListCtrlPtr->SetColumnWidth(dataViewColumnPtr->GetPropertyID(),dataViewColumnPtr->GetWidth()));
    if (dataViewColumnPtr->IsSortable()) // if the current column is sortable and there is no active sortable column yet, the new column will become active
    {
     // variable definition:
      DataBrowserPropertyID sortedProperty;

      if ((MacDataViewListCtrlPtr->GetSortProperty(&sortedProperty) == noErr) && (sortedProperty < kMinPropertyID))
        MacDataViewListCtrlPtr->SetSortProperty(dataViewColumnPtr->GetPropertyID());
    } /* if */

   // now, make sure that data is shown in the newly appended column:
    if ((this->GetModel() != NULL) && (this->m_NotifierPtr != NULL) && (dataViewColumnPtr->GetModelColumn() >= 0) && (dataViewColumnPtr->GetModelColumn() < this->GetModel()->GetColumnCount()))
      if (this->GetColumnCount() == 1) // the newly appended column is the only one and this means that no data has been inserted yet, so do it now:
      {
       // variable definition:
        DataBrowserItemID newID;
        
        for (size_t i=0; i<this->GetModel()->GetRowCount(); ++i)
          if (!((MacDataViewListCtrlPtr->GetFreeItemID(&newID) == noErr) && MacDataViewListCtrlPtr->InsertItemIDRowPair(newID,i) && (MacDataViewListCtrlPtr->AddItem(kDataBrowserNoItem,&newID) == noErr)))
            return false;
      } /* if */
      else
        for (size_t i=0; i<this->GetModel()->GetRowCount(); ++i)
          (void) this->m_NotifierPtr->ValueChanged(dataViewColumnPtr->GetModelColumn(),i);
   // done:
    return true;
  } /* if */
  else
    return false;
} /* wxDataViewCtrl::AppendColumn(wxDataViewColumn*) */

int wxDataViewCtrl::GetSelection(void) const
{
  wxArrayInt selectedRows;


  if (this->GetSelections(selectedRows) > 0)
    return selectedRows[0];
  else
    return wxNOT_FOUND;
} /* wxDataViewCtrl::GetSelection(void) const */

int wxDataViewCtrl::GetSelections(wxArrayInt& selectedRows) const
{
  size_t NoOfSelections;
 
  wxArrayDataBrowserItemID itemIDs;

  wxMacDataViewDataBrowserListViewControlPointer dataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));


  if (dataViewListCtrlPtr == NULL)
    NoOfSelections = 0;
  else
    NoOfSelections = dataViewListCtrlPtr->GetSelectedItemIDs(itemIDs);
  selectedRows.Empty();
  selectedRows.Alloc(NoOfSelections);
  for (size_t i=0; i<NoOfSelections; ++i)
  {
   // variable definition:
    DataBrowserTableViewRowIndex rowIndex;
    
    wxCHECK_MSG(dataViewListCtrlPtr->GetRowIndex(rowIndex,itemIDs[i]),0,_("Could not determine row index."));
    selectedRows[i] = static_cast<int>(rowIndex);
  } /* for */
  return static_cast<int>(NoOfSelections);
} /* wxDataViewCtrl::GetSelections(wxArrayInt&) const */

bool wxDataViewCtrl::IsSelected(unsigned int row) const
{
  DataBrowserItemID itemID;
  
  wxMacDataViewDataBrowserListViewControlPointer dataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

  
  return ((dataViewListCtrlPtr != NULL) && (dataViewListCtrlPtr->GetItemID(row,&itemID) == noErr) && dataViewListCtrlPtr->IsItemSelected(itemID));
} /* wxDataViewCtrl::IsSelected(unsigned int row) const */

void wxDataViewCtrl::SetSelection(int row)
{
  wxArrayDataBrowserItemID selectedItemIDs;
  
  size_t NoOfSelections;
  
  wxMacDataViewDataBrowserListViewControlPointer dataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));


  wxCHECK_RET(dataViewListCtrlPtr != NULL,_("Peer pointer not initialized."));  
  if (row == -1) // unselect all selected items
  {
    NoOfSelections = dataViewListCtrlPtr->GetSelectedItemIDs(selectedItemIDs);
    for (size_t i=0; i<NoOfSelections; ++i)
      (void) dataViewListCtrlPtr->SetSelectedItems(1,&(selectedItemIDs[i]),kDataBrowserItemsRemove);
  } /* if */
  else if (row >= 0) // select specified item
  {
   // variable definition:
    DataBrowserItemID itemID;
    
    verify_noerr(dataViewListCtrlPtr->GetItemID(row,&itemID));
    verify_noerr(dataViewListCtrlPtr->SetSelectedItems(1,&itemID,kDataBrowserItemsAdd)); // should also deselect automatically all other items
  } /* if */
} /* wxDataViewCtrl::SetSelection(int) */

void wxDataViewCtrl::SetSelectionRange(unsigned int from, unsigned int to)
{
  wxCHECK_RET(from <= to,_("Invalid specified range ('from' has to be smaller or equal to 'to')."));
  
 // variable definition:
  wxArrayInt selectedRows;
  
  selectedRows.Alloc(to-from+1);
  for (unsigned int i=from; i<=to; ++i)
    selectedRows.Add(i);
  this->SetSelections(selectedRows);
} /* wxDataViewCtrl::SetSelectionRange(unsigned int, unsigned int) */

void wxDataViewCtrl::SetSelections(wxArrayInt const& selectedRows)
{
  wxMacDataViewDataBrowserListViewControlPointer dataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

 
  wxCHECK_RET(dataViewListCtrlPtr != NULL,_("Peer pointer not initialized."));  
 // unselect all selected items:
  this->SetSelection(-1);
  for (size_t i=0; i<selectedRows.GetCount(); ++i)
  {
   // variable definition:
    DataBrowserItemID itemID;
    
    verify_noerr(dataViewListCtrlPtr->GetItemID(selectedRows[i],&itemID));
    verify_noerr(dataViewListCtrlPtr->SetSelectedItems(1,&itemID,kDataBrowserItemsAssign));
  } /* if */
} /* wxDataViewCtrl::SetSelections(wxArrayInt const&) */

void wxDataViewCtrl::Unselect(unsigned int row)
{
  DataBrowserItemID itemID;

  wxMacDataViewDataBrowserListViewControlPointer dataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

 
  wxCHECK_RET( dataViewListCtrlPtr != NULL,                                                                                                                 _("Peer pointer not initialized."));  
  wxCHECK_RET((dataViewListCtrlPtr->GetItemID(row,&itemID) == noErr) && (dataViewListCtrlPtr->SetSelectedItems(1,&itemID,kDataBrowserItemsRemove) == noErr),_("Unselection impossible."));
} /* wxDataViewCtrl::Unselect(unsigned int) */

void wxDataViewCtrl::ReverseSorting(DataBrowserPropertyID columnID)
{

  if (columnID >= kMinPropertyID)
  {
   // variable definition and initialization:
    DataBrowserPropertyID                          sortPropertyID;
    wxMacDataViewDataBrowserListViewControlPointer dataViewListCtrlPtr(dynamic_cast<wxMacDataViewDataBrowserListViewControlPointer>(this->m_peer));

    wxCHECK_RET(dataViewListCtrlPtr != NULL,_("Peer pointer not initialized."));
    wxCHECK_RET(dataViewListCtrlPtr->GetSortProperty(&sortPropertyID) == noErr,_("Determination of sort property impossible."));
    if (sortPropertyID == columnID)
    {
     // variable defintion:
      DataBrowserSortOrder         sortOrderCtrl;
      DataBrowserTableViewColumnID columnIndex;
      wxDataViewColumn*            dataViewColumnPtr;

      wxCHECK_RET(dataViewListCtrlPtr->GetPropertyColumn(columnID,&columnIndex) == noErr,_("Determination of column index impossible"));
      wxCHECK_RET(dataViewListCtrlPtr->GetSortOrder(&sortOrderCtrl)             == noErr,_("Determination of sort order impossible"));
      dataViewColumnPtr = this->GetColumn(columnIndex);
      wxCHECK_RET(dataViewColumnPtr != NULL,_("Column could not be obtained."));
      if ((sortOrderCtrl == kDataBrowserOrderIncreasing) && !(dataViewColumnPtr->IsSortOrderAscending()) ||
          (sortOrderCtrl == kDataBrowserOrderDecreasing) &&   dataViewColumnPtr->IsSortOrderAscending())
        dataViewListCtrlPtr->ReverseRowIndexNumbering();
    } /* if */
  } /* if */
} /* wxDataViewCtrl::ReverseSorting(DataBrowserPropertyID columnID) */

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl,wxDataViewCtrlBase)

BEGIN_EVENT_TABLE(wxDataViewCtrl,wxDataViewCtrlBase)
  EVT_SIZE(wxDataViewCtrl::OnSize)
END_EVENT_TABLE()

#endif
    // !wxUSE_GENERICDATAVIEWCTRL

#endif
    // wxUSE_DATAVIEWCTRL

