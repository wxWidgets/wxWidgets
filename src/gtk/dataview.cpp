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
#include "gtktreemodel.h"
#include "gtktreedatalist.h"
#include "gtktreednd.h"

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

static void         wxgtk_list_store_init            (GtkWxListStore      *list_store);
static void         wxgtk_list_store_class_init      (GtkWxListStoreClass *class);
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

static void gtk_list_store_set_n_columns   (GtkWxListStore *list_store,
					    gint          n_columns);
static void gtk_list_store_set_column_type (GtkWxListStore *list_store,
					    gint          column,
					    GType         type);
                        
static GObjectClass *parent_class = NULL;

GType
wxgtk_list_store_get_type (void)
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
						&list_store_info, 0);

      g_type_add_interface_static (list_store_type,
				   GTK_TYPE_TREE_MODEL,
				   &tree_model_info);
    }

  return list_store_type;
}

static void
wxgtk_list_store_class_init (GtkWxListStoreClass *class)
{
    GObjectClass *object_class;
    parent_class = g_type_class_peek_parent (class);
    object_class = (GObjectClass*) class;
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
    list_store->stamp = g_random_init();
}

static void
wxgtk_list_store_finalize (GObject *object)
{
    GtkWxListStore *list_store = GTK_LIST_STORE (object);

    /* we need to sort out, which class deletes what */
    delete model;

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
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), 0);

    return GTK_TREE_MODEL_ITERS_PERSIST | GTK_TREE_MODEL_LIST_ONLY;
}

static gint
wxgtk_list_store_get_n_columns (GtkTreeModel *tree_model)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), 0);

    return list_store->model->GetColumns();
}

static GType
wxgtk_list_store_get_column_type (GtkTreeModel *tree_model,
				gint          index)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), G_TYPE_INVALID);

    GType gtype = G_TYPE_INVALID;
#if 0
    list_store->model->GetColumnType( index );
    // convert wxtype to GType
    gtype = ..
#endif

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

#if 0
  i = gtk_tree_path_get_indices (path)[0];

  if (i >= list_store->length)
    return FALSE;

  list = g_slist_nth (G_SLIST (list_store->root), i);

  /* If this fails, list_store->length has gotten mangled. */
  g_assert (list);

  iter->stamp = list_store->stamp;
  iter->user_data = list;
#endif

    return TRUE;
}

static GtkTreePath *
wxgtk_list_store_get_path (GtkTreeModel *tree_model,
			 GtkTreeIter  *iter)
{
    GtkWxListStore *list_store = (GtkListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), NULL);
  
#if 0
  g_return_val_if_fail (iter->stamp == GTK_WX_LIST_STORE (tree_model)->stamp, NULL);
  
  GtkTreePath *retval;
  GSList *list;
  gint i = 0;

  if (G_SLIST (iter->user_data) == G_SLIST (GTK_LIST_STORE (tree_model)->tail))
    {
      retval = gtk_tree_path_new ();
      gtk_tree_path_append_index (retval, GTK_LIST_STORE (tree_model)->length - 1);
      return retval;
    }

  for (list = G_SLIST (GTK_LIST_STORE (tree_model)->root); list; list = list->next)
    {
      if (list == G_SLIST (iter->user_data))
	break;
      i++;
    }
  if (list == NULL)
    return NULL;

  retval = gtk_tree_path_new ();
  gtk_tree_path_append_index (retval, i);
  return retval;
#endif 

    return NULL;
}

static void
wxgtk_list_store_get_value (GtkTreeModel *tree_model,
			  GtkTreeIter  *iter,
			  gint          column,
			  GValue       *value)
{
    GtkWxListStore *list_store = (GtkListStore *) tree_model;
    g_return_if_fail (GTK_IS_WX_LIST_STORE (tree_model) );
    
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
    GtkWxListStore *list_store = (GtkListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), FALSE);
    
#if 0
  g_return_val_if_fail (GTK_LIST_STORE (tree_model)->stamp == iter->stamp, FALSE);

  iter->user_data = G_SLIST (iter->user_data)->next;

  return (iter->user_data != NULL);
#endif

    return NULL;
}

static gboolean
wxgtk_list_store_iter_children (GtkTreeModel *tree_model,
			      GtkTreeIter  *iter,
			      GtkTreeIter  *parent)
{
    /* this is a list, nodes have no children */
    if (parent)
        return FALSE;

    /* but if parent == NULL we return the list itself */
    if (GTK_WX_LIST_STORE (tree_model)->root)
    {
        iter->stamp = GTK_WX_LIST_STORE (tree_model)->stamp;
        iter->user_data = GTK_WX_LIST_STORE (tree_model)->root;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
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
    
    if (iter == NULL)
        return GTK_WX_LIST_STORE (tree_model)->model->GetLength();

    g_return_val_if_fail (GTK_WX_LIST_STORE (tree_model)->stamp == iter->stamp, -1);
  
    return 0;
}

static gboolean
wxgtk_list_store_iter_nth_child (GtkTreeModel *tree_model,
			       GtkTreeIter  *iter,
			       GtkTreeIter  *parent,
			       gint          n)
{
#if 0
  GSList *child;

  g_return_val_if_fail (GTK_IS_LIST_STORE (tree_model), FALSE);

  if (parent)
    return FALSE;

  child = g_slist_nth (G_SLIST (GTK_LIST_STORE (tree_model)->root), n);

  if (child)
    {
      iter->stamp = GTK_LIST_STORE (tree_model)->stamp;
      iter->user_data = child;
      return TRUE;
    }
  else
#endif
    return FALSE;
}

static gboolean
wxgtk_list_store_iter_parent (GtkTreeModel *tree_model,
			    GtkTreeIter  *iter,
			    GtkTreeIter  *child)
{
    return FALSE;
}


//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl,wxControl)


#endif // wxUSE_DATAVIEWCTRL

