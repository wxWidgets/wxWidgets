/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.h
// Purpose:     provide wxSizer class for layouting
// Author:      Robert Roebling and Robin Dunn
// Modified by: Ron Lee
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) Robin Dunn, Dirk Holtwick and Robert Roebling
//              (c) 2003, Ron Lee
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSIZER_H__
#define __WXSIZER_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "sizer.h"
#endif

#include "wx/defs.h"

#include "wx/window.h"
#include "wx/frame.h"
#include "wx/dialog.h"

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

class wxSizerItem;
class wxSizer;
class wxBoxSizer;

//---------------------------------------------------------------------------
// wxSizerItem
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizerItem: public wxObject
{
public:
    // spacer
    wxSizerItem( int width,
                 int height,
                 int proportion,
                 int flag,
                 int border,
                 wxObject* userData);

    // window
    wxSizerItem( wxWindow *window,
                 int proportion,
                 int flag,
                 int border,
                 wxObject* userData );

    // subsizer
    wxSizerItem( wxSizer *sizer,
                 int proportion,
                 int flag,
                 int border,
                 wxObject* userData );

    ~wxSizerItem();
    
    virtual void DeleteWindows();

    // Enable deleting the SizerItem without destroying the contained sizer.
    void DetachSizer()
        { m_sizer = 0; }

    virtual wxSize GetSize();
    virtual wxSize CalcMin();
    virtual void SetDimension( wxPoint pos, wxSize size );

    wxSize GetMinSize()
        { return m_minSize; }
    void SetInitSize( int x, int y )
        { m_minSize.x = x; m_minSize.y = y; }

    void SetRatio( int width, int height )
        // if either of dimensions is zero, ratio is assumed to be 1
        // to avoid "divide by zero" errors
        { m_ratio = (width && height) ? ((float) width / (float) height) : 1; }
    void SetRatio( wxSize size )
        { m_ratio = (size.x && size.y) ? ((float) size.x / (float) size.y) : 1; }
    void SetRatio( float ratio )
        { m_ratio = ratio; }
    float GetRatio() const
        { return m_ratio; }

    bool IsWindow();
    bool IsSizer();
    bool IsSpacer();

    // Deprecated in 2.6, use {G,S}etProportion instead.
    wxDEPRECATED( void SetOption( int option ) );
    wxDEPRECATED( int GetOption() const );

    void SetProportion( int proportion )
        { m_proportion = proportion; }
    int GetProportion() const
        { return m_proportion; }
    void SetFlag( int flag )
        { m_flag = flag; }
    int GetFlag() const
        { return m_flag; }
    void SetBorder( int border )
        { m_border = border; }
    int GetBorder() const
        { return m_border; }

    wxWindow *GetWindow() const
        { return m_window; }
    void SetWindow( wxWindow *window )
        { m_window = window; }
    wxSizer *GetSizer() const
        { return m_sizer; }
    void SetSizer( wxSizer *sizer )
        { m_sizer = sizer; }
    const wxSize &GetSpacer() const
        { return m_size; }
    void SetSpacer( const wxSize &size )
        { m_size = size; m_minSize = size; }

    void Show ( bool show );
    bool IsShown() const
        { return m_show; }

    wxObject* GetUserData()
        { return m_userData; }
    wxPoint GetPosition()
        { return m_pos; }

protected:
    wxWindow    *m_window;
    wxSizer     *m_sizer;
    wxSize       m_size;
    wxPoint      m_pos;
    wxSize       m_minSize;
    int          m_proportion;
    int          m_border;
    int          m_flag;

    // If true, then this item is considered in the layout
    // calculation.  Otherwise, it is skipped over. 
    bool         m_show;

    // Aspect ratio can always be calculated from m_size,
    // but this would cause precision loss when the window
    // is shrunk.  It is safer to preserve the initial value.
    float        m_ratio;

    wxObject    *m_userData;

    DECLARE_DYNAMIC_CLASS(wxSizerItem);
    DECLARE_NO_COPY_CLASS(wxSizerItem)
};

WX_DECLARE_EXPORTED_LIST( wxSizerItem, wxSizerItemList );


//---------------------------------------------------------------------------
// wxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizer: public wxObject, public wxClientDataContainer
{
public:
    wxSizer();
    ~wxSizer();

    /* These should be called Append() really. */
    virtual void Add( wxWindow *window,
                      int proportion = 0,
                      int flag = 0,
                      int border = 0,
                      wxObject* userData = NULL );
    virtual void Add( wxSizer *sizer,
                      int proportion = 0,
                      int flag = 0,
                      int border = 0,
                      wxObject* userData = NULL );
    virtual void Add( int width,
                      int height,
                      int proportion = 0,
                      int flag = 0,
                      int border = 0,
                      wxObject* userData = NULL );
    virtual void Add( wxSizerItem *item );

    virtual void Insert( size_t index,
                         wxWindow *window,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0,
                         wxObject* userData = NULL );
    virtual void Insert( size_t index,
                         wxSizer *sizer,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0,
                         wxObject* userData = NULL );
    virtual void Insert( size_t index,
                         int width,
                         int height,
                         int proportion = 0,
                         int flag = 0,
                         int border = 0,
                         wxObject* userData = NULL );
    virtual void Insert( size_t index,
                         wxSizerItem *item );

    virtual void Prepend( wxWindow *window,
                          int proportion = 0,
                          int flag = 0,
                          int border = 0,
                          wxObject* userData = NULL );
    virtual void Prepend( wxSizer *sizer,
                          int proportion = 0,
                          int flag = 0,
                          int border = 0,
                          wxObject* userData = NULL );
    virtual void Prepend( int width,
                          int height,
                          int proportion = 0,
                          int flag = 0,
                          int border = 0,
                          wxObject* userData = NULL );
    virtual void Prepend( wxSizerItem *item );

    // Deprecated in 2.6 since historically it does not delete the window,
    // use Detach instead.
    wxDEPRECATED( virtual bool Remove( wxWindow *window ) );
    virtual bool Remove( wxSizer *sizer );
    virtual bool Remove( size_t index );

    virtual bool Detach( wxWindow *window );
    virtual bool Detach( wxSizer *sizer );
    virtual bool Detach( size_t index );

    virtual void Clear( bool delete_windows=false );
    virtual void DeleteWindows();

    void SetMinSize( int width, int height )
        { DoSetMinSize( width, height ); }
    void SetMinSize( wxSize size )
        { DoSetMinSize( size.x, size.y ); }

    /* Searches recursively */
    bool SetItemMinSize( wxWindow *window, int width, int height )
        { return DoSetItemMinSize( window, width, height ); }
    bool SetItemMinSize( wxWindow *window, wxSize size )
        { return DoSetItemMinSize( window, size.x, size.y ); }

    /* Searches recursively */
    bool SetItemMinSize( wxSizer *sizer, int width, int height )
        { return DoSetItemMinSize( sizer, width, height ); }
    bool SetItemMinSize( wxSizer *sizer, wxSize size )
        { return DoSetItemMinSize( sizer, size.x, size.y ); }

    bool SetItemMinSize( size_t index, int width, int height )
        { return DoSetItemMinSize( index, width, height ); }
    bool SetItemMinSize( size_t index, wxSize size )
        { return DoSetItemMinSize( index, size.x, size.y ); }

    wxSize GetSize()
        { return m_size; }
    wxPoint GetPosition()
        { return m_position; }

    /* Calculate the minimal size or return m_minSize if bigger. */
    wxSize GetMinSize();

    virtual void RecalcSizes() = 0;
    virtual wxSize CalcMin() = 0;

    virtual void Layout();

    wxSize Fit( wxWindow *window );
    void FitInside( wxWindow *window );
    void SetSizeHints( wxWindow *window );
    void SetVirtualSizeHints( wxWindow *window );

    wxSizerItemList& GetChildren()
        { return m_children; }

    void SetDimension( int x, int y, int width, int height );

    // Manage whether individual scene items are considered
    // in the layout calculations or not.
    void Show( wxWindow *window, bool show = true );
    void Show( wxSizer *sizer, bool show = true );
    void Show( size_t index, bool show = true );

    void Hide( wxSizer *sizer )
        { Show( sizer, false ); }
    void Hide( wxWindow *window )
        { Show( window, false ); }
    void Hide( size_t index )
        { Show( index, false ); }

    bool IsShown( wxWindow *window );
    bool IsShown( wxSizer *sizer );
    bool IsShown( size_t index );
    
    // Recursively call wxWindow::Show () on all sizer items.
    void ShowItems (bool show);

protected:
    wxSize              m_size;
    wxSize              m_minSize;
    wxPoint             m_position;
    wxSizerItemList     m_children;

    wxSize GetMaxWindowSize( wxWindow *window );
    wxSize GetMinWindowSize( wxWindow *window );
    wxSize GetMaxClientSize( wxWindow *window );
    wxSize GetMinClientSize( wxWindow *window );
    wxSize FitSize( wxWindow *window );
    wxSize VirtualFitSize( wxWindow *window );

    virtual void DoSetMinSize( int width, int height );
    virtual bool DoSetItemMinSize( wxWindow *window, int width, int height );
    virtual bool DoSetItemMinSize( wxSizer *sizer, int width, int height );
    virtual bool DoSetItemMinSize( size_t index, int width, int height );

    DECLARE_DYNAMIC_CLASS(wxSizer);
};

//---------------------------------------------------------------------------
// wxGridSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxGridSizer: public wxSizer
{
public:
    wxGridSizer( int rows, int cols, int vgap, int hgap );
    wxGridSizer( int cols, int vgap = 0, int hgap = 0 );

    void RecalcSizes();
    wxSize CalcMin();

    void SetCols( int cols )    { m_cols = cols; }
    void SetRows( int rows )    { m_rows = rows; }
    void SetVGap( int gap )     { m_vgap = gap; }
    void SetHGap( int gap )     { m_hgap = gap; }
    int GetCols()               { return m_cols; }
    int GetRows()               { return m_rows; }
    int GetVGap()               { return m_vgap; }
    int GetHGap()               { return m_hgap; }

protected:
    int    m_rows;
    int    m_cols;
    int    m_vgap;
    int    m_hgap;

    // return the number of total items and the number of columns and rows
    int CalcRowsCols(int& rows, int& cols) const;

    void SetItemBounds( wxSizerItem *item, int x, int y, int w, int h );

    DECLARE_DYNAMIC_CLASS(wxGridSizer);
};

//---------------------------------------------------------------------------
// wxFlexGridSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxFlexGridSizer: public wxGridSizer
{
public:
    wxFlexGridSizer( int rows, int cols, int vgap, int hgap );
    wxFlexGridSizer( int cols, int vgap = 0, int hgap = 0 );
    ~wxFlexGridSizer();

    void RecalcSizes();
    wxSize CalcMin();

    void AddGrowableRow( size_t idx );
    void RemoveGrowableRow( size_t idx );
    void AddGrowableCol( size_t idx );
    void RemoveGrowableCol( size_t idx );

protected:
    int         *m_rowHeights;
    int         *m_colWidths;
    wxArrayInt  m_growableRows;
    wxArrayInt  m_growableCols;

    void CreateArrays();

    DECLARE_DYNAMIC_CLASS(wxFlexGridSizer);
    DECLARE_NO_COPY_CLASS(wxFlexGridSizer)
};

//---------------------------------------------------------------------------
// wxBoxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxBoxSizer: public wxSizer
{
public:
    wxBoxSizer( int orient );

    void RecalcSizes();
    wxSize CalcMin();

    int GetOrientation()
        { return m_orient; }

    void SetOrientation(int orient)
        { m_orient = orient; }

protected:
    int m_orient;
    int m_stretchable;
    int m_minWidth;
    int m_minHeight;
    int m_fixedWidth;
    int m_fixedHeight;

    DECLARE_DYNAMIC_CLASS(wxBoxSizer);
};

//---------------------------------------------------------------------------
// wxStaticBoxSizer
//---------------------------------------------------------------------------

#if wxUSE_STATBOX

class WXDLLEXPORT wxStaticBox;

class WXDLLEXPORT wxStaticBoxSizer: public wxBoxSizer
{
public:
    wxStaticBoxSizer( wxStaticBox *box, int orient );

    void RecalcSizes();
    wxSize CalcMin();

    wxStaticBox *GetStaticBox()
        { return m_staticBox; }

protected:
    wxStaticBox   *m_staticBox;

    DECLARE_DYNAMIC_CLASS(wxStaticBoxSizer);
    DECLARE_NO_COPY_CLASS(wxStaticBoxSizer)
};

#endif // wxUSE_STATBOX

//---------------------------------------------------------------------------
// wxNotebookSizer
//---------------------------------------------------------------------------

#if wxUSE_NOTEBOOK

class WXDLLEXPORT wxNotebook;

class WXDLLEXPORT wxNotebookSizer: public wxSizer
{
public:
    wxNotebookSizer( wxNotebook *nb );

    void RecalcSizes();
    wxSize CalcMin();

    wxNotebook *GetNotebook()
        { return m_notebook; }

protected:
    wxNotebook   *m_notebook;

    DECLARE_DYNAMIC_CLASS(wxNotebookSizer);
    DECLARE_NO_COPY_CLASS(wxNotebookSizer)
};

#endif // wxUSE_NOTEBOOK


#endif
  // __WXSIZER_H__
