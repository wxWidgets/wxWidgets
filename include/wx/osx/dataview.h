/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dataview.h
// Purpose:     wxDataViewCtrl native implementation header for OSX
// Author:
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
class wxDataViewWidgetImpl;         // class used as a common interface for carbon and cocoa implementation

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
    wxDataViewColumn(const wxBitmapBundle& bitmap,
                     wxDataViewRenderer* renderer,
                     unsigned int model_column,
                     int width = wxDVC_DEFAULT_WIDTH,
                     wxAlignment align = wxALIGN_CENTER,
                     int flags = wxDATAVIEW_COL_RESIZABLE);
    virtual ~wxDataViewColumn();

    // implement wxHeaderColumnBase pure virtual methods
    virtual wxAlignment GetAlignment() const override { return m_alignment; }
    virtual int GetFlags() const override { return m_flags; }
    virtual int GetMaxWidth() const { return m_maxWidth; }
    virtual int GetMinWidth() const override { return m_minWidth; }
    virtual wxString GetTitle() const override { return m_title; }
    virtual int GetWidth() const override;
    virtual bool IsSortOrderAscending() const override { return m_ascending; }
    virtual bool IsSortKey() const override;
    virtual bool IsHidden() const override;

    virtual void SetAlignment  (wxAlignment align) override;
    virtual void SetBitmap     (wxBitmapBundle const& bitmap) override;
    virtual void SetFlags      (int flags) override { m_flags = flags; /*SetIndividualFlags(flags); */ }
    virtual void SetHidden     (bool hidden) override;
    virtual void SetMaxWidth   (int maxWidth);
    virtual void SetMinWidth   (int minWidth) override;
    virtual void SetReorderable(bool reorderable) override;
    virtual void SetResizeable (bool resizable) override;
    virtual void UnsetAsSortKey() override;
    virtual void SetSortable   (bool sortable) override;
    virtual void SetSortOrder  (bool ascending) override;
    virtual void SetTitle      (wxString const& title) override;
    virtual void SetWidth      (int  width) override;

   // implementation only
    wxDataViewColumnNativeData* GetNativeData() const
    {
      return m_NativeDataPtr;
    }

    void SetNativeData(wxDataViewColumnNativeData* newNativeDataPtr); // class takes ownership of pointer
    int GetWidthVariable() const
    {
        return m_width;
    }
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
        m_alignment = align;
        SetWidth(width);
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
    Init();
  }
  wxDataViewCtrl(wxWindow *parent,
                 wxWindowID winid,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr) )
  {
    Init();
    Create(parent, winid, pos, size, style, validator, name);
  }

  ~wxDataViewCtrl();

  bool Create(wxWindow *parent,
              wxWindowID winid,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxDataViewCtrlNameStr));

  virtual wxWindow* GetMainWindow() // not used for the native implementation
  {
    return this;
  }

 // inherited methods from wxDataViewCtrlBase:
  virtual bool AssociateModel(wxDataViewModel* model) override;

  virtual bool              AppendColumn     (wxDataViewColumn* columnPtr) override;
  virtual bool              ClearColumns     () override;
  virtual bool              DeleteColumn     (wxDataViewColumn* columnPtr) override;
  virtual wxDataViewColumn* GetColumn        (unsigned int pos) const override;
  virtual unsigned int      GetColumnCount   () const override;
  virtual int               GetColumnPosition(const wxDataViewColumn* columnPtr) const override;
  virtual wxDataViewColumn* GetSortingColumn () const override;
  virtual bool              InsertColumn     (unsigned int pos, wxDataViewColumn *col) override;
  virtual bool              PrependColumn    (wxDataViewColumn* columnPtr) override;

  virtual void Collapse( const wxDataViewItem& item) override;
  virtual void EnsureVisible(const wxDataViewItem& item, const wxDataViewColumn* columnPtr=nullptr) override;
  virtual bool IsExpanded(const wxDataViewItem & item) const override;

  virtual unsigned int GetCount() const;
  virtual int GetCountPerPage() const override;
  virtual wxRect GetItemRect(const wxDataViewItem& item,
                             const wxDataViewColumn* columnPtr = nullptr) const override;
  virtual int GetSelectedItemsCount() const override;
  virtual int GetSelections(wxDataViewItemArray& sel) const override;

  virtual wxDataViewItem GetTopItem() const override;

  virtual void HitTest(const wxPoint& point, wxDataViewItem& item, wxDataViewColumn*& columnPtr) const override;

  virtual bool SetRowHeight(int rowHeight) override;

  virtual bool IsSelected(const wxDataViewItem& item) const override;

  virtual void SelectAll() override;
  virtual void Select(const wxDataViewItem& item) override;
  virtual void SetSelections(const wxDataViewItemArray& sel) override;

  virtual void Unselect(const wxDataViewItem& item) override;
  virtual void UnselectAll() override;

//
// implementation
//
 // returns a pointer to the native implementation
  wxDataViewWidgetImpl* GetDataViewPeer() const;

 // adds all children of the passed parent to the control; if 'parentItem' is invalid the root(s) is/are added:
  void AddChildren(wxDataViewItem const& parentItem);

 // finishes editing of custom items; if no custom item is currently edited the method does nothing
  void FinishCustomItemEditing();

  virtual void EditItem(const wxDataViewItem& item, const wxDataViewColumn *column) override;

#if wxUSE_DRAG_AND_DROP
  virtual bool DoEnableDropTarget( const wxVector<wxDataFormat>& formats ) override;
#endif // wxUSE_DRAG_AND_DROP

 // returns the n-th pointer to a column;
 // this method is different from GetColumn(unsigned int pos) because here 'n' is not a position in the control but the n-th
 // position in the internal list/array of column pointers
  wxDataViewColumn* GetColumnPtr(size_t n) const
  {
    return m_ColumnPtrs[n];
  }
 // returns the current being rendered item of the customized renderer (this item is only valid during editing)
  wxDataViewItem const& GetCustomRendererItem() const
  {
    return m_CustomRendererItem;
  }
 // returns a pointer to a customized renderer (this pointer is only valid during editing)
  wxDataViewCustomRenderer* GetCustomRendererPtr() const
  {
    return m_CustomRendererPtr;
  }

 // checks if a single item or all items are being deleted
  bool IsDeleting() const;
  bool IsClearing() const;

 // with CG, we need to get the context from an kEventControlDraw event
 // unfortunately, the DataBrowser callbacks don't provide the context
 // and we need it, so we need to set/remove it before and after draw
 // events so we can access it in the callbacks.
  void MacSetDrawingContext(void* context)
  {
    m_cgContext = context;
  }
  void* MacGetDrawingContext() const
  {
    return m_cgContext;
  }

 // sets the currently being edited item of the custom renderer
  void SetCustomRendererItem(wxDataViewItem const& NewItem)
  {
    m_CustomRendererItem = NewItem;
  }
 // sets the custom renderer
  void SetCustomRendererPtr(wxDataViewCustomRenderer* NewCustomRendererPtr)
  {
    m_CustomRendererPtr = NewCustomRendererPtr;
  }

  void AdjustAutosizedColumns() const;

  virtual wxDataViewColumn *GetCurrentColumn() const override;

  virtual wxVisualAttributes GetDefaultAttributes() const override
  {
      return GetClassDefaultAttributes(GetWindowVariant());
  }

  static wxVisualAttributes
  GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

protected:
 // inherited methods from wxDataViewCtrlBase
  virtual void DoSetExpanderColumn() override;
  virtual void DoSetIndent() override;

  virtual void DoExpand(const wxDataViewItem& item, bool expandChildren) override;

  virtual wxSize DoGetBestSize() const override;

 // event handling
  void OnSize(wxSizeEvent &event);

  virtual void DoThaw() override;

private:
 // initializing of local variables:
  void Init();

  virtual wxDataViewItem DoGetCurrentItem() const override;
  virtual void DoSetCurrentItem(const wxDataViewItem& item) override;

  void OnContextMenu(wxContextMenuEvent& event);

 //
 // variables
 //

  // If non-null, describes the item(s) being deleted. This is necessary to
  // allow avoiding referencing already deleted items from the native
  // callbacks/delegates.
  struct wxOSXDVCDeleting* m_Deleting;

  // This class can set (and reset) m_Deleting.
  friend class wxOSXDVCScopedDeleter;

  void* m_cgContext; // pointer to core graphics context

  wxDataViewCustomRenderer* m_CustomRendererPtr; // pointer to a valid custom renderer while editing; this class does NOT own the pointer

  wxDataViewItem m_CustomRendererItem; // currently edited item by the customrenderer; it is invalid while not editing a custom item

  wxDataViewColumnPtrArrayType m_ColumnPtrs; // all column pointers are stored in an array

  class wxOSXDataViewModelNotifier* m_ModelNotifier; // stores the model notifier for the control (does not own the notifier)

 // wxWidget internal stuff:
  wxDECLARE_DYNAMIC_CLASS(wxDataViewCtrl);
  wxDECLARE_NO_COPY_CLASS(wxDataViewCtrl);
  wxDECLARE_EVENT_TABLE();
};

#endif // _WX_DATAVIEWCTRL_OSX_H_

