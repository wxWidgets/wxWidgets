/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/dataview.h
// Purpose:     wxDataViewCtrl native implementation header
// Author:
// Id:          $Id$
// Copyright:   (c) 2007
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MACCARBONDATAVIEWCTRL_H_
#define _WX_MACCARBONDATAVIEWCTRL_H_

// --------------------------------------------------------
// Type definitions to mask native types
// --------------------------------------------------------

typedef void*         WXDataBrowserItemDataRef;
typedef unsigned long WXDataBrowserPropertyType;
typedef wxUint32      WXDataBrowserPropertyID;

// ---------------------------------------------------------
// wxDataViewRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewRenderer : public wxDataViewRendererBase
{
public:
//
// constructors / destructor
//
  wxDataViewRenderer(wxString const& varianttype, wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited methods from wxDataViewRendererBase
//
  virtual int GetAlignment() const
  {
    return this->m_alignment;
  }
  virtual wxDataViewCellMode GetMode() const
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
  WXDataBrowserItemDataRef GetDataReference() const
  {
    return this->m_dataReference;
  }
  wxVariant const& GetValue() const
  {
    return this->m_value;
  }

  virtual WXDataBrowserPropertyType GetPropertyType() const = 0;

  virtual bool Render() = 0; // a call to the appropriate data browser function filling the data reference with the stored datum;
                                 // returns 'true' if the data value could be rendered, 'false' otherwise

  void SetDataReference(WXDataBrowserItemDataRef const& newDataReference)
  {
    this->m_dataReference = newDataReference;
  }

private:
//
// variables
//
  WXDataBrowserItemDataRef m_dataReference; // data reference of the data browser; the data will be assigned to this reference during rendering

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

  virtual ~wxDataViewCustomRenderer();

  void RenderText( const wxString &text, int xoffset, wxRect cell, wxDC *dc, int state );

//
// methods handling render space
//
  virtual wxSize GetSize() const = 0;

//
// methods handling user actions
//
  virtual bool Render(wxRect cell, wxDC* dc, int state) = 0;

  virtual bool Activate( wxRect WXUNUSED(cell),
                         wxDataViewModel *WXUNUSED(model),
                         const wxDataViewItem & WXUNUSED(item),
                         unsigned int WXUNUSED(col) )
                         { return false; }

  virtual bool LeftClick( wxPoint WXUNUSED(cursor),
                          wxRect WXUNUSED(cell),
                          wxDataViewModel *WXUNUSED(model),
                          const wxDataViewItem & WXUNUSED(item),
                          unsigned int WXUNUSED(col) )
                          { return false; }

  virtual bool StartDrag( wxPoint WXUNUSED(cursor),
                          wxRect WXUNUSED(cell),
                          wxDataViewModel *WXUNUSED(model),
                          const wxDataViewItem & WXUNUSED(item),
                          unsigned int WXUNUSED(col) )
                          { return false; }

//
// device context handling
//
  virtual wxDC* GetDC(); // creates a device context and keeps it

//
// implementation
//
  virtual bool Render(); // declared in wxDataViewRenderer but will not be used here, therefore calling this function will
                             // return 'true' without having done anything

  virtual WXDataBrowserPropertyType GetPropertyType() const;

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
  virtual bool Render();

//
// implementation
//
  virtual WXDataBrowserPropertyType GetPropertyType() const;

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRenderer)
};

// ---------------------------------------------------------
// wxDataViewTextRendererAttr
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewTextRendererAttr: public wxDataViewTextRenderer
{
public:
//
// constructors / destructor
//
    wxDataViewTextRendererAttr(wxString const& varianttype=wxT("string"), wxDataViewCellMode mode=wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewTextRendererAttr)
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
  virtual bool Render();

//
// implementation
//
  virtual WXDataBrowserPropertyType GetPropertyType() const;

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer)
};

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewRenderer
{
public:
  wxDataViewIconTextRenderer(wxString const& varianttype = wxT("wxDataViewIconText"), wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render();

//
// implementation
//
  virtual WXDataBrowserPropertyType GetPropertyType() const;

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewIconTextRenderer)
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
  virtual bool Render();

//
// implementation
//
  virtual WXDataBrowserPropertyType GetPropertyType() const;

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
  virtual bool Render();

//
// implementation
//
  virtual WXDataBrowserPropertyType GetPropertyType() const;

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
  virtual bool Render();

//
// implementation
//
  virtual WXDataBrowserPropertyType GetPropertyType() const;

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
    // constructors / destructor
    wxDataViewColumn(const wxString& title,
                     wxDataViewRenderer* renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE)
        : wxDataViewColumnBase(renderer, model_column),
          m_title(title)
    {
        Init(width, align, flags);
    }

    wxDataViewColumn(const wxBitmap& bitmap,
                     wxDataViewRenderer* renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE)
        : wxDataViewColumnBase(bitmap, renderer, model_column)
    {
        Init(width, align, flags);
    }

    // implement wxHeaderColumnBase pure virtual methods
    virtual wxAlignment GetAlignment() const { return m_alignment; }
    virtual int GetFlags() const { return m_flags; }
    virtual int GetMaxWidth() const { return m_maxWidth; }
    virtual int GetMinWidth() const { return m_minWidth; }
    virtual wxString GetTitle() const { return m_title; }
    virtual int GetWidth() const { return m_width; }
    virtual bool IsHidden() const { return false; } // TODO
    virtual bool IsSortOrderAscending() const { return m_ascending; }
    virtual bool IsSortKey() const;

    virtual void SetAlignment(wxAlignment align);
    virtual void SetBitmap   (wxBitmap const& bitmap);
    virtual void SetFlags    (int flags) { SetIndividualFlags(flags); }
    virtual void SetHidden(bool WXUNUSED(hidden)) { } // TODO
    virtual void SetMaxWidth   (int maxWidth);
    virtual void SetMinWidth   (int minWidth);
    virtual void SetReorderable(bool reorderable);
    virtual void SetResizeable (bool resizeable);
    virtual void SetSortable   (bool sortable);
    virtual void SetSortOrder  (bool ascending);
    virtual void SetTitle      (wxString const& title);
    virtual void SetWidth      (int  width);
    virtual void SetAsSortKey(bool sort = true);

    // implementation only
    WXDataBrowserPropertyID GetPropertyID() const
    {
        return m_propertyID;
    }

    void SetPropertyID(WXDataBrowserPropertyID newID)
    {
        m_propertyID = newID;
    }
    void SetWidthVariable(int NewWidth)
    {
        m_width = NewWidth;
    }

private:
    // common part of all ctors
    void Init(int width, wxAlignment align, int flags)
    {
        m_ascending = true;
        m_flags = flags & ~wxDATAVIEW_COL_HIDDEN; // TODO
        m_maxWidth = 30000;
        m_minWidth = 0;
        m_width = width >= 0 ? width : wxDVC_DEFAULT_WIDTH;
        m_alignment = align;
    }

    bool m_ascending; // sorting order

    // each column is identified by its unique property ID (NOT by the column
    // index)
    WXDataBrowserPropertyID m_propertyID;

    int m_flags;    // flags for the column
    int m_maxWidth; // maximum width for the column
    int m_minWidth; // minimum width for the column
    int m_width;    // column width

    wxAlignment m_alignment; // column header alignment

    wxString m_title; // column title
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
  wxDataViewCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
                 const wxValidator& validator = wxDefaultValidator)
  {
    this->Init();
    this->Create(parent, id, pos, size, style, validator );
  }

  ~wxDataViewCtrl();

 // explicit control creation
  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0,
              const wxValidator& validator=wxDefaultValidator);

  virtual wxControl* GetMainWindow() // should disappear as it is not of any use for the native implementation
  {
    return this;
  }

 // inherited methods from 'wxDataViewCtrlBase':
  virtual bool AssociateModel(wxDataViewModel* model);

  virtual bool AppendColumn(wxDataViewColumn* columnPtr);
  virtual bool PrependColumn(wxDataViewColumn* columnPtr);
  virtual bool InsertColumn(unsigned int pos, wxDataViewColumn *col);

  virtual bool ClearColumns();
  virtual bool DeleteColumn(wxDataViewColumn* columnPtr);
  virtual wxDataViewColumn* GetColumn(unsigned int pos) const;
  virtual unsigned int GetColumnCount() const;
  virtual int GetColumnPosition(const wxDataViewColumn* columnPtr) const;

  virtual void Collapse( const wxDataViewItem& item);
  virtual void EnsureVisible(const wxDataViewItem& item, const wxDataViewColumn* columnPtr=NULL);
  virtual void Expand(const wxDataViewItem& item);
  virtual bool IsExpanded(const wxDataViewItem & item) const;

  virtual wxDataViewColumn* GetSortingColumn() const;

  virtual unsigned int GetCount() const;
  virtual wxRect GetItemRect(const wxDataViewItem& item, const wxDataViewColumn* columnPtr) const;
  virtual wxDataViewItem GetSelection() const;
  virtual int GetSelections(wxDataViewItemArray& sel) const;

  virtual void HitTest(const wxPoint& point, wxDataViewItem& item, wxDataViewColumn*& columnPtr) const;

  virtual bool IsSelected(const wxDataViewItem& item) const;

  virtual void SelectAll();
  virtual void Select(const wxDataViewItem& item);
  virtual void SetSelections(const wxDataViewItemArray& sel);

  virtual void Unselect(const wxDataViewItem& item);
  virtual void UnselectAll();

//
// implementation
//

 // adds all children of the passed parent to the control; if 'parentItem' is invalid the root(s) is/are added:
  void AddChildrenLevel(wxDataViewItem const& parentItem);

 // finishes editing of custom items; if no custom item is currently edited the method does nothing
  void FinishCustomItemEditing();

 // returns a pointer to a column;
 // in case the pointer cannot be found NULL is returned:
  wxDataViewColumn* GetColumnPtr(WXDataBrowserPropertyID propertyID) const;
 // returns the current being rendered item of the customized renderer (this item is only valid during editing)
  wxDataViewItem const& GetCustomRendererItem() const
  {
    return this->m_CustomRendererItem;
  }
 // returns a pointer to a customized renderer (this pointer is only valid during editing)
  wxDataViewCustomRenderer* GetCustomRendererPtr() const
  {
    return this->m_CustomRendererPtr;
  }

 // checks if currently a delete process is running:
  bool IsDeleting() const
  {
    return this->m_Deleting;
  }

 // with CG, we need to get the context from an kEventControlDraw event
 // unfortunately, the DataBrowser callbacks don't provide the context
 // and we need it, so we need to set/remove it before and after draw
 // events so we can access it in the callbacks.
  void MacSetDrawingContext(void* context)
  {
    this->m_cgContext = context;
  }
  void* MacGetDrawingContext() const
  {
    return this->m_cgContext;
  }

 // sets the currently being edited item of the custom renderer
  void SetCustomRendererItem(wxDataViewItem const& NewItem)
  {
    this->m_CustomRendererItem = NewItem;
  }
 // sets the custom renderer
  void SetCustomRendererPtr(wxDataViewCustomRenderer* NewCustomRendererPtr)
  {
    this->m_CustomRendererPtr = NewCustomRendererPtr;
  }
 // sets the flag indicating a deletion process:
  void SetDeleting(bool deleting)
  {
    this->m_Deleting = deleting;
  }

  virtual wxVisualAttributes GetDefaultAttributes() const
  {
      return GetClassDefaultAttributes(GetWindowVariant());
  }

  static wxVisualAttributes
  GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

protected:
 // inherited methods from wxDataViewCtrlBase:
  virtual void DoSetExpanderColumn();
  virtual void DoSetIndent();

 // event handling:
  void OnSize(wxSizeEvent &event);
  void OnMouse(wxMouseEvent &event);

private:
 // type definitions:
  WX_DECLARE_HASH_MAP(WXDataBrowserPropertyID,wxDataViewColumn*,wxIntegerHash,wxIntegerEqual,ColumnPointerHashMapType);

 // initializing of local variables:
  void Init();

///
// variables
//

  bool m_Deleting; // flag indicating if a delete process is running; this flag is necessary because the notifier indicating an item deletion in the model may be called
                   // after the actual deletion of the item; then, the callback function "wxMacDataViewDataBrowserListViewControl::DataBrowserGetSetItemDataProc" may
                   // try to update data into variables that are already deleted; this flag will ignore all variable update requests during item deletion

  void* m_cgContext; // pointer to core graphics context

  wxDataViewCustomRenderer* m_CustomRendererPtr; // pointer to a valid custom renderer while editing; this class does NOT own the pointer

  wxDataViewItem m_CustomRendererItem; // currently edited item by the customerenderer; it is invalid while not editing

  ColumnPointerHashMapType m_ColumnPointers; // all column pointers are stored in a hash map with the property ID as a key

 // wxWidget internal stuff:
  DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
  DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
  DECLARE_EVENT_TABLE()
};


#endif // _WX_MACCARBONDATAVIEWCTRL_H_
