/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/dataview.h
// Purpose:     wxDataViewCtrl native implementation header
// Author:      
// Id:          $Id$
// Copyright:   (c) 2007
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __MACCARBONDATAVIEWCTRL_H__
#define __MACCARBONDATAVIEWCTRL_H__

#include <Carbon/carbon.h>

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/scrolwin.h"
#include "wx/icon.h"

// ---------------------------------------------------------
// classes
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCtrl;

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewRenderer: public wxDataViewRendererBase
{
public:
//
// constructors / destructor
//
  wxDataViewRenderer(wxString const& varianttype, wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited methods from wxDataViewRendererBase
//
  virtual int GetAlignment(void) const
  {
    return this->m_alignment;
  }
  virtual wxDataViewCellMode GetMode(void) const
  {
    return this->m_mode;
  }
  virtual bool GetValue(wxVariant& value) const
  {
    value = this->m_value;
    return true;
  }

  virtual void SetAlignment(int WXUNUSED(align)) // is always identical to the header alignment
  {
  }
  virtual void SetMode(wxDataViewCellMode mode);
  virtual bool SetValue(wxVariant const& newValue)
  {
    this->m_value = newValue;
    return true;
  }

//
// implementation
//
  DataBrowserItemDataRef GetDataReference(void) const
  {
    return this->m_dataReference;
  }
  wxVariant const& GetValue(void) const
  {
    return this->m_value;
  }

  virtual DataBrowserPropertyType GetPropertyType(void) const = 0;

  virtual bool Render(void) = 0; // a call to the appropriate data browser function filling the data reference with the stored datum;
                                 // returns 'true' if the data value could be rendered, 'false' otherwise

  void SetDataReference(DataBrowserItemDataRef const& newDataReference)
  {
    this->m_dataReference = newDataReference;
  }

private:
//
// variables
//
  DataBrowserItemDataRef m_dataReference; // data reference of the data browser; the data will be assigned to this reference during rendering

  int m_alignment; // contains the alignment flags

  wxDataViewCellMode m_mode; // storing the mode that determines how the cell is going to be shown

  wxVariant m_value; // value that is going to be rendered

//
// wxWidget internal stuff
//
  DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewRenderer)
};

// ---------------------------------------------------------
// wxDataViewCustomRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCustomRenderer: public wxDataViewRenderer
{
public:
//
// constructors / destructor
//
  wxDataViewCustomRenderer(wxString const& varianttype=wxT("string"), wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);
  
  virtual ~wxDataViewCustomRenderer(void);

//
// methods handling render space
//
  virtual wxSize GetSize() const = 0;

//
// methods handling user actions
// 
  virtual bool Render(wxRect cell, wxDC* dc, int state) = 0;

  virtual bool Activate(wxRect WXUNUSED(cell), wxDataViewListModel *WXUNUSED(model), unsigned int WXUNUSED(col), unsigned int WXUNUSED(row))
  {
    return false;
  }

  virtual bool LeftClick(wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell), wxDataViewListModel *WXUNUSED(model), unsigned int WXUNUSED(col), unsigned int WXUNUSED(row))
  {
    return false;
  }

  virtual bool RightClick(wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell), wxDataViewListModel *WXUNUSED(model), unsigned int WXUNUSED(col), unsigned int WXUNUSED(row))
  {
    return false;
  }

  virtual bool StartDrag(wxPoint WXUNUSED(cursor), wxRect WXUNUSED(cell), wxDataViewListModel *WXUNUSED(model), unsigned int WXUNUSED(col), unsigned int WXUNUSED(row))
  {
    return false;
  }

//
// in-place editing
//
  virtual void CancelEditing()
  {
  }

  virtual wxControl* CreateEditorCtrl(wxWindow * WXUNUSED(parent), wxRect WXUNUSED(labelRect), const wxVariant& WXUNUSED(value))
  {
    return NULL;
  }

  virtual bool FinishEditing()
  {
    return false;
  }

  wxControl* GetEditorCtrl(void) const
  {
    return this->m_editorCtrlPtr;
  }
  virtual bool GetValueFromEditorCtrl(wxControl* WXUNUSED(editor), wxVariant& WXUNUSED(value))
  {
    return false;
  }

  virtual bool HasEditorCtrl(void)
  {
    return false;
  }

  virtual bool StartEditing(unsigned int WXUNUSED(row), wxRect WXUNUSED(labelRect)) 
  {
    return false;
  }
  
//
// device context handling
//
  virtual wxDC* GetDC(void); // creates a device context and keeps it

//
// implementation
//
  virtual bool Render(void); // declared in wxDataViewRenderer but will not be used here, therefore calling this function will
                             // return 'false' without having done anything

  virtual DataBrowserPropertyType GetPropertyType(void) const
  {
    return kDataBrowserCustomType;
  }

  void SetDC(wxDC* newDCPtr); // this method takes ownership of the pointer

protected:
private:
//
// variables
//
  wxControl* m_editorCtrlPtr; // pointer to an in-place editor control

  wxDC* m_DCPtr;

//
// wxWidget internal stuff
//
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewCustomRenderer)
};

// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRenderer: public wxDataViewRenderer
{
public:
//
// constructors / destructor
//
    wxDataViewTextRenderer(wxString const& varianttype=wxT("string"), wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render(void);

//
// implementation
//
  virtual DataBrowserPropertyType GetPropertyType(void) const
  {
    return kDataBrowserTextType;
  }

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer)
};

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewBitmapRenderer: public wxDataViewRenderer
{
public:
//
// constructors / destructor
//
  wxDataViewBitmapRenderer(wxString const& varianttype=wxT("wxBitmap"), wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render(void);

//
// implementation
//
  virtual DataBrowserPropertyType GetPropertyType(void) const
  {
    return kDataBrowserIconType;
  }

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer)
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewToggleRenderer: public wxDataViewRenderer
{
public:
  wxDataViewToggleRenderer(wxString const& varianttype = wxT("bool"), wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render(void);

//
// implementation
//
  virtual DataBrowserPropertyType GetPropertyType(void) const
  {
    return kDataBrowserCheckboxType;
  }

protected:
private:
  DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewToggleRenderer)
};

// ---------------------------------------------------------
// wxDataViewProgressRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewProgressRenderer: public wxDataViewRenderer
{
public:
  wxDataViewProgressRenderer(wxString const& label = wxEmptyString, wxString const& varianttype=wxT("long"),
                             wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render(void);

//
// implementation
//
  virtual DataBrowserPropertyType GetPropertyType(void) const
  {
    return kDataBrowserProgressBarType;
  }

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewProgressRenderer)
};

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewDateRenderer: public wxDataViewRenderer
{
public:
  wxDataViewDateRenderer(wxString const& varianttype=wxT("datetime"), wxDataViewCellMode mode=wxDATAVIEW_CELL_ACTIVATABLE, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render(void);

//
// implementation
//
  virtual DataBrowserPropertyType GetPropertyType(void) const
  {
    return kDataBrowserDateTimeType;
  }

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewDateRenderer)
};

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewColumn: public wxDataViewColumnBase
{
public:
//
// constructors / destructor
//
  wxDataViewColumn(wxString const& title, wxDataViewRenderer* renderer, unsigned int model_column, int width=80, wxAlignment align=wxALIGN_CENTER,
                   int flags=wxDATAVIEW_COL_RESIZABLE);
  wxDataViewColumn(wxBitmap const& bitmap, wxDataViewRenderer* renderer, unsigned int model_column, int width=80, wxAlignment align=wxALIGN_CENTER,
                   int flags=wxDATAVIEW_COL_RESIZABLE);

//
// inherited methods from wxDataViewColumnBase
//
  virtual wxAlignment GetAlignment(void) const
  {
    return this->m_alignment;
  }
  virtual int GetFlags(void) const
  {
    return this->m_flags;
  }
  virtual int GetMaxWidth(void) const
  {
    return this->m_maxWidth;
  }
  virtual int GetMinWidth(void) const
  {
    return this->m_minWidth;
  }
  virtual wxString GetTitle(void) const
  {
    return this->m_title;
  }
  virtual int GetWidth(void) const
  {
    return this->m_width;
  }

  virtual bool IsHidden(void) const
  {
    return false; // not implemented
  }
  virtual bool IsResizeable(void) const
  {
    return ((this->m_flags & wxDATAVIEW_COL_RESIZABLE) != 0);
  }
  virtual bool IsSortable(void) const
  {
    return ((this->m_flags & wxDATAVIEW_COL_SORTABLE) != 0);
  }
  virtual bool IsSortOrderAscending(void) const
  {
    return this->m_ascending;
  }

  virtual void SetAlignment(wxAlignment align);
  virtual void SetBitmap   (wxBitmap const& bitmap);
  virtual void SetFlags    (int flags);
  virtual void SetHidden(bool WXUNUSED(hidden))
  {
  }
  virtual void SetMaxWidth  (int maxWidth);
  virtual void SetMinWidth  (int minWidth);
  virtual void SetResizeable(bool resizeable);
  virtual void SetSortable  (bool sortable);
  virtual void SetSortOrder (bool ascending);
  virtual void SetTitle     (wxString const& title);
  virtual void SetWidth     (int  width);

//
// implementation
//
  DataBrowserPropertyID GetPropertyID(void) const
  {
    return this->m_propertyID;
  }
  
  void SetPropertyID(DataBrowserPropertyID newID)
  {
    this->m_propertyID = newID;
  }

protected:
private:
//
// variables
//
  bool m_ascending; // sorting order

  DataBrowserPropertyID m_propertyID; // each column is identified by its unique property ID (NOT by the column index)
  
  int m_flags;    // flags for the column
  int m_maxWidth; // maximum width for the column
  int m_minWidth; // minimum width for the column
  int m_width;    // column width

  wxAlignment m_alignment; // column header alignment

  wxString m_title; // column title

 // wxWidget internal stuff:
  DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewColumn)
};

// ---------------------------------------------------------
// wxDataViewCtrl
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewCtrl: public wxDataViewCtrlBase
{
public:
 // Constructors / destructor:
    wxDataViewCtrl()
    {
      this->Init();
    }
    wxDataViewCtrl(wxWindow *parent, wxWindowID id, wxPoint const& pos = wxDefaultPosition, wxSize const& size = wxDefaultSize, long style = 0,
                   wxValidator const& validator = wxDefaultValidator)
    {
      this->Init();
      this->Create(parent, id, pos, size, style, validator );
    }

    bool Create(wxWindow *parent, wxWindowID id, wxPoint const& pos=wxDefaultPosition, wxSize const& size=wxDefaultSize, long style=0,
                wxValidator const& validator=wxDefaultValidator);

    virtual bool AppendColumn(wxDataViewColumn* columnPtr);

    virtual bool AssociateModel(wxDataViewListModel* model);

    virtual void SetSelection( int row ); // -1 for unselect
    virtual void SetSelectionRange( unsigned int from, unsigned int to );
    virtual void SetSelections( const wxArrayInt& aSelections);
    virtual void Unselect( unsigned int row );
    
    virtual bool IsSelected( unsigned int row ) const;
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

    virtual wxControl* GetMainWindow(void) // should disappear as it is not of any use for the native implementation
    {
      return this;
    }

//
// implementation
//

 // with CG, we need to get the context from an kEventControlDraw event
 // unfortunately, the DataBrowser callbacks don't provide the context
 // and we need it, so we need to set/remove it before and after draw 
 // events so we can access it in the callbacks.
  void MacSetDrawingContext(void* context)
  {
    this->m_cgContext = context;
  }
  void* MacGetDrawingContext(void) const
  {
    return this->m_cgContext;
  }

 // reverts the sorting order of the hash map if wxDataViewColumn data and control data do not match;
 // wxDataViewColumn data will NOT be updated
  void ReverseSorting(DataBrowserPropertyID columnID);

protected:
 // event handling:
  void OnSize(wxSizeEvent &event);

private:
 // initializing of local variables:
  void Init(void);

///
// variables
//

  wxDataViewListModelNotifier* m_NotifierPtr; // the notifier is NOT owned by this class but by the associated model

  void* m_cgContext; // pointer to core graphics context

 // wxWidget internal stuff:
  DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
  DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
  DECLARE_EVENT_TABLE()
};


#endif // __MACDATAVIEWCTRL_H__
