/////////////////////////////////////////////////////////////////////////////
// Name:        sizer.h
// Purpose:     provide wxSizer class for layouting
// Author:      Robert Roebling and Robin Dunn
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) Robin Dunn, Dirk Holtwick and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXSIZER_H__
#define __WXSIZER_H__

#ifdef __GNUG__
#pragma interface "sizer.h"
#endif

#include "wx/defs.h"

#include "wx/window.h"
#include "wx/frame.h"
#include "wx/dialog.h"

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

class wxStaticBox;
class wxNotebook;

class wxSizerItem;
class wxSizer;
class wxBoxSizer;
class wxStaticBoxSizer;

//---------------------------------------------------------------------------
// wxSizerItem
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizerItem: public wxObject
{
public:
    // spacer
    wxSizerItem( int width, int height, int option, int flag, int border, wxObject* userData);

    // window
    wxSizerItem( wxWindow *window, int option, int flag, int border, wxObject* userData );

    // subsizer
    wxSizerItem( wxSizer *sizer, int option, int flag, int border, wxObject* userData );

    ~wxSizerItem();

    virtual wxSize GetSize();
    virtual wxSize CalcMin();
    virtual void SetDimension( wxPoint pos, wxSize size );

    wxSize GetMinSize()
        { return m_minSize; }

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
  
    void SetInitSize( int x, int y )
        { m_minSize.x = x; m_minSize.y = y; }
    void SetOption( int option )
        { m_option = option; }
    void SetFlag( int flag )
        { m_flag = flag; }
    void SetBorder( int border )
        { m_border = border; }

    wxWindow *GetWindow() const
        { return m_window; }
    void SetWindow( wxWindow *window )
        { m_window = window; }
    wxSizer *GetSizer() const
        { return m_sizer; }
    void SetSizer( wxSizer *sizer )
        { m_sizer = sizer; }
    int GetOption() const
        { return m_option; }
    int GetFlag() const
        { return m_flag; }
    int GetBorder() const
        { return m_border; }
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
    int          m_option;
    int          m_border;
    int          m_flag;
    // als: aspect ratio can always be calculated from m_size,
    //      but this would cause precision loss when the window
    //      is shrinked.  it is safer to preserve initial value.
    float        m_ratio;
    wxObject    *m_userData;
  
private:
    DECLARE_CLASS(wxSizerItem);
};

//---------------------------------------------------------------------------
// wxSizer
//---------------------------------------------------------------------------

class WXDLLEXPORT wxSizer: public wxObject
{
public:
    wxSizer();
    ~wxSizer();

    /* These should be called Append() really. */
    virtual void Add( wxWindow *window, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
    virtual void Add( wxSizer *sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
    virtual void Add( int width, int height, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );

    virtual void Insert( int before, wxWindow *window, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
    virtual void Insert( int before, wxSizer *sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
    virtual void Insert( int before, int width, int height, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );

    virtual void Prepend( wxWindow *window, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
    virtual void Prepend( wxSizer *sizer, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );
    virtual void Prepend( int width, int height, int option = 0, int flag = 0, int border = 0, wxObject* userData = NULL );

    virtual bool Remove( wxWindow *window );
    virtual bool Remove( wxSizer *sizer );
    virtual bool Remove( int pos );

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
        
    bool SetItemMinSize( int pos, int width, int height )
        { return DoSetItemMinSize( pos, width, height ); }
    bool SetItemMinSize( int pos, wxSize size )
        { return DoSetItemMinSize( pos, size.x, size.y ); }
        
    wxSize GetSize()
        { return m_size; }
    wxPoint GetPosition()
        { return m_position; }
        
    /* Calculate the minimal size or return m_minSize if bigger. */
    wxSize GetMinSize();

    virtual void RecalcSizes() = 0;
    virtual wxSize CalcMin() = 0;

    virtual void Layout();

    void Fit( wxWindow *window );
    void SetSizeHints( wxWindow *window );

    wxList& GetChildren()
        { return m_children; }

    void SetDimension( int x, int y, int width, int height );

protected:
    wxSize  m_size;
    wxSize  m_minSize;
    wxPoint m_position;
    wxList  m_children;

    wxSize GetMinWindowSize( wxWindow *window );
    
    virtual void DoSetMinSize( int width, int height );
    virtual bool DoSetItemMinSize( wxWindow *window, int width, int height );
    virtual bool DoSetItemMinSize( wxSizer *sizer, int width, int height );
    virtual bool DoSetItemMinSize( int pos, int width, int height );
        
private:
    DECLARE_CLASS(wxSizer);
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
    
    void SetItemBounds( wxSizerItem *item, int x, int y, int w, int h );
    
private:
    DECLARE_CLASS(wxGridSizer);
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
    
private:
    DECLARE_CLASS(wxFlexGridSizer);
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

protected:
    int m_orient;
    int m_stretchable;
    int m_minWidth;
    int m_minHeight;
    int m_fixedWidth;
    int m_fixedHeight;
    
private:
    DECLARE_CLASS(wxBoxSizer);
};

//---------------------------------------------------------------------------
// wxStaticBoxSizer
//---------------------------------------------------------------------------

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
    
private:
    DECLARE_CLASS(wxStaticBoxSizer);
};

//---------------------------------------------------------------------------
// wxNotebookSizer
//---------------------------------------------------------------------------

#if wxUSE_NOTEBOOK

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
   
private:
    DECLARE_CLASS(wxNotebookSizer);
};

#endif


#endif
  // __WXSIZER_H__
