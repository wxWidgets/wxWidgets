
/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dataview.h
// Purpose:     wxDataViewCtrl native implementation header for OSX
// Author:
// Id:          $Id$
// Copyright:   (c) 2009
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAVIEWCTRL_OSX_H_
#define _WX_DATAVIEWCTRL_OSX_H_

#ifdef __WXMAC_CLASSIC__
#  error "Native wxDataViewCtrl for classic environment not defined. Please use generic control."
#endif

// --------------------------------------------------------
// Class declarations to mask native types
// --------------------------------------------------------
class wxDataViewColumnNativeData;   // class storing environment dependent data for the native implementation
class wxDataViewRendererNativeData; // class storing environment dependent data for the native renderer
class wxDataViewWidgetImpl;         // class used as a common interface for carbon and cocoa implementation

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
  
  virtual ~wxDataViewRenderer(void);

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

  virtual void SetAlignment(int align); // carbon: is always identical to the header alignment;
                                        // cocoa: cell alignment is independent from header alignment
  virtual void SetMode(wxDataViewCellMode mode);
  virtual bool SetValue(wxVariant const& newValue)
  {
    this->m_value = newValue;
    return true;
  }

//
// implementation
//
  wxVariant const& GetValue() const
  {
    return this->m_value;
  }

  wxDataViewRendererNativeData* GetNativeData() const
  {
    return this->m_NativeDataPtr;
  }

  virtual bool Render() = 0; // a call to the native data browser function to render the data;
                             // returns true if the data value could be rendered, false otherwise

  void SetNativeData(wxDataViewRendererNativeData* newNativeDataPtr);

private:
//
// variables
//
  int m_alignment; // contains the alignment flags

  wxDataViewCellMode m_mode; // storing the mode that determines how the cell is going to be shown

  wxDataViewRendererNativeData* m_NativeDataPtr; // data used by implementation of the native renderer

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
  virtual bool Render();

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

protected:
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewBitmapRenderer)
};

#if !defined(wxUSE_GENERICDATAVIEWCTRL) && defined(__WXOSX_COCOA__)

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------
class WXDLLIMPEXP_ADV wxDataViewChoiceRenderer: public wxDataViewRenderer
{
public:
//
// constructors / destructor
//
  wxDataViewChoiceRenderer(wxArrayString const& choices,
                           wxDataViewCellMode mode = wxDATAVIEW_CELL_EDITABLE,
                           int alignment = wxDVR_DEFAULT_ALIGNMENT );

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render();

//
// implementation
//
  wxString GetChoice(size_t index) const
  {
    return this->m_Choices[index];
  }
  wxArrayString const& GetChoices(void) const
  {
    return this->m_Choices;
  }

private:
//
// variables
//
  wxArrayString m_Choices;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDataViewChoiceRenderer)
};

#endif

// ---------------------------------------------------------
// wxDataViewIconTextRenderer
// ---------------------------------------------------------
class WXDLLIMPEXP_ADV wxDataViewIconTextRenderer: public wxDataViewRenderer
{
public:
  wxDataViewIconTextRenderer(wxString const& varianttype = wxT("wxDataViewIconText"), wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT, int align=wxDVR_DEFAULT_ALIGNMENT);

//
// inherited functions from wxDataViewRenderer
//
  virtual bool Render();

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
                     int flags = wxDATAVIEW_COL_RESIZABLE);
    wxDataViewColumn(const wxBitmap& bitmap,
                     wxDataViewRenderer* renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE);
    virtual ~wxDataViewColumn(void);

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

    virtual void SetAlignment  (wxAlignment align);
    virtual void SetBitmap     (wxBitmap const& bitmap);
    virtual void SetFlags      (int flags) { SetIndividualFlags(flags); }
    virtual void SetHidden     (bool WXUNUSED(hidden)) { } // TODO
    virtual void SetMaxWidth   (int maxWidth);
    virtual void SetMinWidth   (int minWidth);
    virtual void SetReorderable(bool reorderable);
    virtual void SetResizeable (bool resizeable);
    virtual void SetSortable   (bool sortable);
    virtual void SetSortOrder  (bool ascending);
    virtual void SetTitle      (wxString const& title);
    virtual void SetWidth      (int  width);
    virtual void SetAsSortKey  (bool sort = true);

   // implementation only
    wxDataViewColumnNativeData* GetNativeData(void) const
    {
      return this->m_NativeDataPtr;
    }
    
    void SetNativeData(wxDataViewColumnNativeData* newNativeDataPtr); // class takes ownership of pointer
    void SetWidthVariable(int NewWidth)
    {
        m_width = NewWidth;
    }
    void SetSortOrderVariable(bool NewOrder)
    {
        m_ascending = NewOrder;
    }

private:
    // common part of all ctors
    void InitCommon(int width, wxAlignment align, int flags)
    {
        m_ascending = true;
        m_flags = flags & ~wxDATAVIEW_COL_HIDDEN; // TODO
        m_maxWidth = 30000;
        m_minWidth = 0;
        m_width = width >= 0 ? width : wxDVC_DEFAULT_WIDTH;
        m_alignment = align;
    }

    bool m_ascending; // sorting order

    int m_flags;    // flags for the column
    int m_maxWidth; // maximum width for the column
    int m_minWidth; // minimum width for the column
    int m_width;    // column width

    wxAlignment m_alignment; // column header alignment

    wxDataViewColumnNativeData* m_NativeDataPtr; // storing environment dependent data for the native implementation

    wxString m_title; // column title
};

//
// type definitions related to wxDataViewColumn
//
WX_DEFINE_ARRAY(wxDataViewColumn*,wxDataViewColumnPtrArrayType);

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

  virtual wxControl* GetMainWindow() // not used for the native implementation
  {
    return this;
  }

 // inherited methods from wxDataViewCtrlBase:
  virtual bool AssociateModel(wxDataViewModel* model);

  virtual bool              AppendColumn     (wxDataViewColumn* columnPtr);
  virtual bool              ClearColumns     (void);
  virtual bool              DeleteColumn     (wxDataViewColumn* columnPtr);
  virtual wxDataViewColumn* GetColumn        (unsigned int pos) const;
  virtual unsigned int      GetColumnCount   (void) const;
  virtual int               GetColumnPosition(const wxDataViewColumn* columnPtr) const;
  virtual wxDataViewColumn* GetSortingColumn (void) const;
  virtual bool              InsertColumn     (unsigned int pos, wxDataViewColumn *col);
  virtual bool              PrependColumn    (wxDataViewColumn* columnPtr);

  virtual void Collapse( const wxDataViewItem& item);
  virtual void EnsureVisible(const wxDataViewItem& item, const wxDataViewColumn* columnPtr=NULL);
  virtual void Expand(const wxDataViewItem& item);
  virtual bool IsExpanded(const wxDataViewItem & item) const;


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
 // returns a pointer to the native implementation
  wxDataViewWidgetImpl* GetDataViewPeer(void) const;

 // adds all children of the passed parent to the control; if 'parentItem' is invalid the root(s) is/are added:
  void AddChildren(wxDataViewItem const& parentItem);

 // finishes editing of custom items; if no custom item is currently edited the method does nothing
  void FinishCustomItemEditing(void);

 // returns the n-th pointer to a column;
 // this method is different from GetColumn(unsigned int pos) because here 'n' is not a position in the control but the n-th
 // position in the internal list/array of column pointers
  wxDataViewColumn* GetColumnPtr(size_t n) const
  {
    return this->m_ColumnPtrs[n];
  }
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

 // checks if currently a delete process is running
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
 // inherited methods from wxDataViewCtrlBase
  virtual void DoSetExpanderColumn();
  virtual void DoSetIndent();

  virtual wxSize DoGetBestSize() const;

 // event handling
  void OnSize(wxSizeEvent &event);
  void OnMouse(wxMouseEvent &event);

private:
 // initializing of local variables:
  void Init();

 //
 // variables
 //
  bool m_Deleting; // flag indicating if a delete process is running; this flag is necessary because the notifier indicating an item deletion in the model may be called
                   // after the actual deletion of the item; then, native callback functions/delegates may try to update data of variables that are already deleted;
                   // if this flag is set all native variable update requests will be ignored

  void* m_cgContext; // pointer to core graphics context

  wxDataViewCustomRenderer* m_CustomRendererPtr; // pointer to a valid custom renderer while editing; this class does NOT own the pointer

  wxDataViewItem m_CustomRendererItem; // currently edited item by the customrenderer; it is invalid while not editing a custom item

  wxDataViewColumnPtrArrayType m_ColumnPtrs; // all column pointers are stored in an array

 // wxWidget internal stuff:
  DECLARE_DYNAMIC_CLASS(wxDataViewCtrl)
  DECLARE_NO_COPY_CLASS(wxDataViewCtrl)
  DECLARE_EVENT_TABLE()
};

#endif // _WX_DATAVIEWCTRL_OSX_H_

