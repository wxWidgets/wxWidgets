/////////////////////////////////////////////////////////////////////////////
// Name:        dataview.cpp
// Purpose:     wxDataViewCtrl GTK+2 implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"
#include "wx/stockitem.h"

#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

#include <gobject/gvaluecollector.h>
#include <gtk/gtktreemodel.h>
#include <gtk/gtktreednd.h>

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDataViewCtrl;

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// define new GTK+ class wxGtkListStore
//-----------------------------------------------------------------------------

extern "C" {

#define GTK_TYPE_WX_LIST_STORE               (gtk_wx_list_store_get_type ())
#define GTK_WX_LIST_STORE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_WX_LIST_STORE, GtkWxListStore))
#define GTK_WX_LIST_STORE_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_WX_LIST_STORE, GtkWxListStoreClass))
#define GTK_IS_WX_LIST_STORE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_WX_LIST_STORE))
#define GTK_IS_WX_LIST_STORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_WX_LIST_STORE))
#define GTK_WX_LIST_STORE_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_WX_LIST_STORE, GtkWxListStoreClass))

GType         gtk_wx_list_store_get_type         (void);

typedef struct _GtkWxListStore       GtkWxListStore;
typedef struct _GtkWxListStoreClass  GtkWxListStoreClass;

struct _GtkWxListStore
{
  GObject parent;

  /*< private >*/
  gint stamp;
  wxDataViewListModel *model;
};

struct _GtkWxListStoreClass
{
  GObjectClass parent_class;
  
};

static GtkWxListStore *wxgtk_list_store_new          ();
static void         wxgtk_list_store_init            (GtkWxListStore      *list_store);
static void         wxgtk_list_store_class_init      (GtkWxListStoreClass *klass);
static void         wxgtk_list_store_tree_model_init (GtkTreeModelIface *iface);
static void         wxgtk_list_store_finalize        (GObject           *object);
static GtkTreeModelFlags wxgtk_list_store_get_flags  (GtkTreeModel      *tree_model);
static gint         wxgtk_list_store_get_n_columns   (GtkTreeModel      *tree_model);
static GType        wxgtk_list_store_get_column_type (GtkTreeModel      *tree_model,
						    gint               index);
static gboolean     wxgtk_list_store_get_iter        (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreePath       *path);
static GtkTreePath *wxgtk_list_store_get_path        (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter);
static void         wxgtk_list_store_get_value       (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    gint               column,
						    GValue            *value);
static gboolean     wxgtk_list_store_iter_next       (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter);
static gboolean     wxgtk_list_store_iter_children   (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreeIter       *parent);
static gboolean     wxgtk_list_store_iter_has_child  (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter);
static gint         wxgtk_list_store_iter_n_children (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter);
static gboolean     wxgtk_list_store_iter_nth_child  (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreeIter       *parent,
						    gint               n);
static gboolean     wxgtk_list_store_iter_parent     (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreeIter       *child);

static GObjectClass *parent_class = NULL;

GType
gtk_wx_list_store_get_type (void)
{
  static GType list_store_type = 0;

  if (!list_store_type)
    {
      static const GTypeInfo list_store_info =
      {
	sizeof (GtkWxListStoreClass),
	NULL,		/* base_init */
	NULL,		/* base_finalize */
        (GClassInitFunc) wxgtk_list_store_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
        sizeof (GtkWxListStore),
	0,
        (GInstanceInitFunc) wxgtk_list_store_init,
      };

      static const GInterfaceInfo tree_model_info =
      {
	(GInterfaceInitFunc) wxgtk_list_store_tree_model_init,
	NULL,
	NULL
      };

      list_store_type = g_type_register_static (G_TYPE_OBJECT, "GtkWxListStore",
						&list_store_info, (GTypeFlags)0 );

      g_type_add_interface_static (list_store_type,
				   GTK_TYPE_TREE_MODEL,
				   &tree_model_info);
    }

  return list_store_type;
}

static GtkWxListStore *
wxgtk_list_store_new()
{
  GtkWxListStore *retval = (GtkWxListStore *) g_object_new (GTK_TYPE_WX_LIST_STORE, NULL);
  return retval;
}

static void
wxgtk_list_store_class_init (GtkWxListStoreClass *klass)
{
    GObjectClass *object_class;
    parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    object_class = (GObjectClass*) klass;
    object_class->finalize = wxgtk_list_store_finalize;
}

static void
wxgtk_list_store_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags = wxgtk_list_store_get_flags;
    iface->get_n_columns = wxgtk_list_store_get_n_columns;
    iface->get_column_type = wxgtk_list_store_get_column_type;
    iface->get_iter = wxgtk_list_store_get_iter;
    iface->get_path = wxgtk_list_store_get_path;
    iface->get_value = wxgtk_list_store_get_value;
    iface->iter_next = wxgtk_list_store_iter_next;
    iface->iter_children = wxgtk_list_store_iter_children;
    iface->iter_has_child = wxgtk_list_store_iter_has_child;
    iface->iter_n_children = wxgtk_list_store_iter_n_children;
    iface->iter_nth_child = wxgtk_list_store_iter_nth_child;
    iface->iter_parent = wxgtk_list_store_iter_parent;
}

static void
wxgtk_list_store_init (GtkWxListStore *list_store)
{
    list_store->model = NULL;
    list_store->stamp = g_random_int();
}

static void
wxgtk_list_store_finalize (GObject *object)
{
    /* GtkWxListStore *list_store = GTK_WX_LIST_STORE (object); */

    /* we need to sort out, which class deletes what */
    /* delete list_store->model; */

    /* must chain up */
    (* parent_class->finalize) (object);
}
                        
} // extern "C"

//-----------------------------------------------------------------------------
// implement callbacks from wxGtkListStore class by letting
// them call the methods of wxWidgets' wxDataViewListModel
//-----------------------------------------------------------------------------

static GtkTreeModelFlags
wxgtk_list_store_get_flags (GtkTreeModel *tree_model)
{
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), (GtkTreeModelFlags)0 );

    // GTK+ list store uses a linked list for storing the
    // items and a pointer to a child is used as the member
    // field of a GtkTreeIter. This means that the iter is
    // valid in the GtkListStore as long as the child exists.
    // We use the index of the row and since the index of a
    // specific row will change if a row above is deleted,
    // the iter does not persist
    return /* GTK_TREE_MODEL_ITERS_PERSIST | */ GTK_TREE_MODEL_LIST_ONLY;
}

static gint
wxgtk_list_store_get_n_columns (GtkTreeModel *tree_model)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), 0);

    return list_store->model->GetNumberOfCols();
}

static GType
wxgtk_list_store_get_column_type (GtkTreeModel *tree_model,
				gint          index)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), G_TYPE_INVALID);

    GType gtype = G_TYPE_INVALID;
    
    wxString wxtype = list_store->model->GetColType( (size_t) index );
    
    if (wxtype == wxT("string"))
        gtype = G_TYPE_STRING;

    return gtype;
}

static gboolean
wxgtk_list_store_get_iter (GtkTreeModel *tree_model,
			 GtkTreeIter  *iter,
			 GtkTreePath  *path)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), FALSE);
    g_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

    size_t i = (size_t)gtk_tree_path_get_indices (path)[0];

    if (i >= list_store->model->GetNumberOfRows())
        return FALSE;

    iter->stamp = list_store->stamp;
    // user_data is just the index
    iter->user_data = (gpointer) i;

    return TRUE;
}

static GtkTreePath *
wxgtk_list_store_get_path (GtkTreeModel *tree_model,
			 GtkTreeIter  *iter)
{
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), NULL);
    g_return_val_if_fail (iter->stamp == GTK_WX_LIST_STORE (tree_model)->stamp, NULL);
  
    GtkTreePath *retval = gtk_tree_path_new ();
    // user_data is just the index
    int i = (int) iter->user_data;
    gtk_tree_path_append_index (retval, i);
    return retval;
}

static void
wxgtk_list_store_get_value (GtkTreeModel *tree_model,
			  GtkTreeIter  *iter,
			  gint          column,
			  GValue       *value)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_if_fail (GTK_IS_WX_LIST_STORE (tree_model) );

    wxDataViewListModel *model = list_store->model;
    wxString mtype = model->GetColType( (size_t) column );
    if (mtype == wxT("string"))
    {
        g_value_init( value, G_TYPE_STRING );
        wxVariant variant = model->GetValue( (size_t) column, (size_t) iter->user_data );
        g_value_set_string( value, wxGTK_CONV(variant.GetString()) );
    }
    else
    {
    }
    
#if 0
  GtkTreeDataList *list;
  gint tmp_column = column;

  g_return_if_fail (column < GTK_LIST_STORE (tree_model)->n_columns);
  g_return_if_fail (GTK_LIST_STORE (tree_model)->stamp == iter->stamp);

  list = G_SLIST (iter->user_data)->data;

  while (tmp_column-- > 0 && list)
    list = list->next;

  if (list == NULL)
    g_value_init (value, GTK_LIST_STORE (tree_model)->column_headers[column]);
  else
    _gtk_tree_data_list_node_to_value (list,
				       GTK_LIST_STORE (tree_model)->column_headers[column],
				       value);
#endif

}

static gboolean
wxgtk_list_store_iter_next (GtkTreeModel  *tree_model,
			  GtkTreeIter   *iter)
{
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), FALSE);
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    
    g_return_val_if_fail (list_store->stamp == iter->stamp, FALSE);

    int n = (int) iter->user_data;
    
    if (n == -1)
        return FALSE;
        
    if (n >= (int) list_store->model->GetNumberOfRows())
        return FALSE;
        
    iter->user_data = (gpointer) ++n;

    return TRUE;
}

static gboolean
wxgtk_list_store_iter_children (GtkTreeModel *tree_model,
			      GtkTreeIter  *iter,
			      GtkTreeIter  *parent)
{
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), FALSE);
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    
    // this is a list, nodes have no children
    if (parent)
        return FALSE;

    iter->stamp = list_store->stamp;
    iter->user_data = (gpointer) -1;
    
    return TRUE;
}

static gboolean
wxgtk_list_store_iter_has_child (GtkTreeModel *tree_model,
			       GtkTreeIter  *iter)
{
    return FALSE;
}

static gint
wxgtk_list_store_iter_n_children (GtkTreeModel *tree_model,
				GtkTreeIter  *iter)
{
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), -1);
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    
    if (iter == NULL)
        return (gint) list_store->model->GetNumberOfRows();

    g_return_val_if_fail (list_store->stamp == iter->stamp, -1);
  
    return 0;
}

static gboolean
wxgtk_list_store_iter_nth_child (GtkTreeModel *tree_model,
			       GtkTreeIter  *iter,
			       GtkTreeIter  *parent,
			       gint          n)
{
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), FALSE);
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    
    if (parent)
        return FALSE;

    if (n < 0)
        return FALSE;
        
    if (n >= (gint) list_store->model->GetNumberOfRows())
        return FALSE;

    iter->stamp = list_store->stamp;
    iter->user_data = (gpointer) n;
    
    return TRUE;
}

static gboolean
wxgtk_list_store_iter_parent (GtkTreeModel *tree_model,
			    GtkTreeIter  *iter,
			    GtkTreeIter  *child)
{
    return FALSE;
}

// --------------------------------------------------------- 
// wxGtkDataViewListModelNotifier
// --------------------------------------------------------- 

class wxGtkDataViewListModelNotifier: public wxDataViewListModelNotifier
{
public:
    wxGtkDataViewListModelNotifier( GtkWxListStore* gtk_store, wxDataViewListModel *wx_model );
    
    virtual bool RowAppended();
    virtual bool RowPrepended();
    virtual bool RowInserted( size_t before );
    virtual bool RowDeleted( size_t row );
    virtual bool RowChanged( size_t row );
    virtual bool ValueChanged( size_t row, size_t col );
    virtual bool Cleared();
    
    GtkWxListStore      *m_gtk_store;
    wxDataViewListModel *m_wx_model;
};

// --------------------------------------------------------- 
// wxGtkDataViewListModelNotifier
// --------------------------------------------------------- 

wxGtkDataViewListModelNotifier::wxGtkDataViewListModelNotifier( 
    GtkWxListStore* gtk_store, wxDataViewListModel *wx_model )
{
    m_gtk_store = gtk_store;
    m_wx_model = wx_model;
}
    
bool wxGtkDataViewListModelNotifier::RowAppended()
{
    size_t pos = m_wx_model->GetNumberOfRows()-1;
    
    GtkTreeIter iter;
    iter.stamp = m_gtk_store->stamp;
    iter.user_data = (gpointer) pos;
    
    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_path_append_index (path, (gint) pos);
    gtk_tree_model_row_inserted (GTK_TREE_MODEL (m_gtk_store), path, &iter);
    gtk_tree_path_free (path);
    
    return true;
}

bool wxGtkDataViewListModelNotifier::RowPrepended()
{
    return false;
}

bool wxGtkDataViewListModelNotifier::RowInserted( size_t before )
{
    return false;
}

bool wxGtkDataViewListModelNotifier::RowDeleted( size_t row )
{
    return false;
}

bool wxGtkDataViewListModelNotifier::RowChanged( size_t row )
{
    return false;
}

bool wxGtkDataViewListModelNotifier::ValueChanged( size_t row, size_t col )
{
    return false;
}

bool wxGtkDataViewListModelNotifier::Cleared()
{
    return false;
}


// --------------------------------------------------------- 
// wxDataViewColumn
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumn, wxDataViewColumnBase)

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewCtrl *ctrl, 
            wxDataViewColumnType kind, int flags ) :
    wxDataViewColumnBase( title, ctrl, kind, flags )
{
    GtkCellRenderer *renderer = NULL;
    
    if (kind == wxDATAVIEW_COL_TEXT)
    {
        renderer = gtk_cell_renderer_text_new();
    } else
    if (kind == wxDATAVIEW_COL_CHECK)
    {
        renderer = gtk_cell_renderer_toggle_new();
    } else
    if (kind == wxDATAVIEW_COL_ICON)
    {
        renderer = gtk_cell_renderer_pixbuf_new();
    }
    else
        return;

    GtkTreeViewColumn *column = 
        gtk_tree_view_column_new_with_attributes( wxGTK_CONV(title), renderer, "text", 0, NULL );
       
    // bind to data here... not above.
    
    m_column = (void*) column;
}

wxDataViewColumn::~wxDataViewColumn()
{
}

void wxDataViewColumn::SetTitle( const wxString &title )
{
    wxDataViewColumnBase::SetTitle( title );
    
    GtkTreeViewColumn *column = (GtkTreeViewColumn *)m_column;
    gtk_tree_view_column_set_title( column, wxGTK_CONV(title) );
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase)

wxDataViewCtrl::~wxDataViewCtrl()
{
}

void wxDataViewCtrl::Init()
{
}

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size, 
           long style, const wxValidator& validator )
{
    Init();
    
    m_needParent = TRUE;
    m_acceptsFocus = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator ))
    {
        wxFAIL_MSG( wxT("wxDataViewCtrl creation failed") );
        return FALSE;
    }
    
    m_widget = gtk_tree_view_new();
    
    m_parent->DoAddChild( this );

    PostCreation(size);

    return true;
}

bool wxDataViewCtrl::AssociateModel( wxDataViewListModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    GtkWxListStore *gtk_store = wxgtk_list_store_new();
    gtk_store->model = model;

    wxGtkDataViewListModelNotifier *notifier = 
        new wxGtkDataViewListModelNotifier( gtk_store, model );

    model->SetNotifier( notifier );    

    gtk_tree_view_set_model( GTK_TREE_VIEW(m_widget), GTK_TREE_MODEL(gtk_store) );
    
    return true;
}

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;
        
    GtkTreeViewColumn *column = (GtkTreeViewColumn *)col->GetGtkHandle();

    gtk_tree_view_append_column( GTK_TREE_VIEW(m_widget), column );

    return true;
}


#endif // wxUSE_DATAVIEWCTRL

