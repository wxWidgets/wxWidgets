/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dataview.cpp
// Purpose:     wxDataViewCtrl GTK+2 implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef wxUSE_GENERICDATAVIEWCTRL

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcclient.h"
    #include "wx/sizer.h"
#endif

#include "wx/stockitem.h"
#include "wx/calctrl.h"
#include "wx/popupwin.h"
#include "wx/icon.h"


#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

#include <gobject/gvaluecollector.h>
#include <gtk/gtktreemodel.h>
#include <gtk/gtktreednd.h>

#include <gdk/gdkkeysyms.h>

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDataViewCtrl;

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
  GObjectClass list_parent_class;
};

static GtkWxListStore *wxgtk_list_store_new          (void);
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

static GObjectClass *list_parent_class = NULL;

GType
gtk_wx_list_store_get_type (void)
{
    static GType list_store_type = 0;

    if (!list_store_type)
    {
        static const GTypeInfo list_store_info =
        {
            sizeof (GtkWxListStoreClass),
            NULL,   /* base_init */
            NULL,   /* base_finalize */
            (GClassInitFunc) wxgtk_list_store_class_init,
            NULL,   /* class_finalize */
            NULL,   /* class_data */
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
wxgtk_list_store_new(void)
{
    GtkWxListStore *retval = (GtkWxListStore *) g_object_new (GTK_TYPE_WX_LIST_STORE, NULL);
    return retval;
}

static void
wxgtk_list_store_class_init (GtkWxListStoreClass *klass)
{
    list_parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    GObjectClass *object_class = (GObjectClass*) klass;
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
    (* list_parent_class->finalize) (object);
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

    wxString wxtype = list_store->model->GetColType( (unsigned int) index );

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

    unsigned int i = (unsigned int)gtk_tree_path_get_indices (path)[0];

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
    int i = (wxUIntPtr) iter->user_data;
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
    wxString mtype = model->GetColType( (unsigned int) column );
    if (mtype == wxT("string"))
    {
        wxVariant variant;
        g_value_init( value, G_TYPE_STRING );
        model->GetValue( variant, (unsigned int) column, (unsigned int) iter->user_data );
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

    int n = (wxUIntPtr) iter->user_data;

    if (n == -1)
        return FALSE;

    if (n >= (int) list_store->model->GetNumberOfRows()-1)
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

//-----------------------------------------------------------------------------
// define new GTK+ class wxGtkCellRenderer
//-----------------------------------------------------------------------------

extern "C" {

#define GTK_TYPE_WX_CELL_RENDERER               (gtk_wx_cell_renderer_get_type ())
#define GTK_WX_CELL_RENDERER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_WX_CELL_RENDERER, GtkWxCellRenderer))
#define GTK_WX_CELL_RENDERER_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_WX_CELL_RENDERER, GtkWxCellRendererClass))
#define GTK_IS_WX_CELL_RENDERER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_WX_CELL_RENDERER))
#define GTK_IS_WX_CELL_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_WX_CELL_RENDERER))
#define GTK_WX_CELL_RENDERER_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_WX_CELL_RENDERER, GtkWxCellRendererClass))

GType            gtk_wx_cell_renderer_get_type (void);

typedef struct _GtkWxCellRenderer GtkWxCellRenderer;
typedef struct _GtkWxCellRendererClass GtkWxCellRendererClass;

struct _GtkWxCellRenderer
{
  GtkCellRenderer parent;

  /*< private >*/
  wxDataViewCustomCell *cell;
  guint32 last_click;
};

struct _GtkWxCellRendererClass
{
  GtkCellRendererClass cell_parent_class;
};


static GtkCellRenderer *gtk_wx_cell_renderer_new   (void);
static void gtk_wx_cell_renderer_init (
                        GtkWxCellRenderer      *cell );
static void gtk_wx_cell_renderer_class_init(
                        GtkWxCellRendererClass *klass );
static void gtk_wx_cell_renderer_finalize (
                        GObject                *object );
static void gtk_wx_cell_renderer_get_size (
                        GtkCellRenderer         *cell,
                        GtkWidget               *widget,
                        GdkRectangle            *rectangle,
                        gint                    *x_offset,
                        gint                    *y_offset,
                        gint                    *width,
                        gint                    *height );
static void gtk_wx_cell_renderer_render (
                        GtkCellRenderer         *cell,
                        GdkWindow               *window,
                        GtkWidget               *widget,
                        GdkRectangle            *background_area,
                        GdkRectangle            *cell_area,
                        GdkRectangle            *expose_area,
                        GtkCellRendererState     flags );
static gboolean gtk_wx_cell_renderer_activate(
                        GtkCellRenderer         *cell,
                        GdkEvent                *event,
                        GtkWidget               *widget,
                        const gchar             *path,
                        GdkRectangle            *background_area,
                        GdkRectangle            *cell_area,
                        GtkCellRendererState     flags );

static GObjectClass *cell_parent_class = NULL;

}  // extern "C"

GType
gtk_wx_cell_renderer_get_type (void)
{
    static GType cell_wx_type = 0;

    if (!cell_wx_type)
    {
        static const GTypeInfo cell_wx_info =
        {
            sizeof (GtkWxCellRendererClass),
            NULL, /* base_init */
            NULL, /* base_finalize */
            (GClassInitFunc) gtk_wx_cell_renderer_class_init,
            NULL, /* class_finalize */
            NULL, /* class_data */
            sizeof (GtkWxCellRenderer),
            0,          /* n_preallocs */
            (GInstanceInitFunc) gtk_wx_cell_renderer_init,
        };

        cell_wx_type = g_type_register_static( GTK_TYPE_CELL_RENDERER,
            "GtkWxCellRenderer", &cell_wx_info, (GTypeFlags)0 );
    }

    return cell_wx_type;
}

static void
gtk_wx_cell_renderer_init (GtkWxCellRenderer *cell)
{
    cell->cell = NULL;
    cell->last_click = 0;
}

static void
gtk_wx_cell_renderer_class_init (GtkWxCellRendererClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

    cell_parent_class = (GObjectClass*) g_type_class_peek_parent (klass);

    object_class->finalize = gtk_wx_cell_renderer_finalize;

    cell_class->get_size = gtk_wx_cell_renderer_get_size;
    cell_class->render = gtk_wx_cell_renderer_render;
    cell_class->activate = gtk_wx_cell_renderer_activate;
}

static void
gtk_wx_cell_renderer_finalize (GObject *object)
{
    /* must chain up */
    (* G_OBJECT_CLASS (cell_parent_class)->finalize) (object);
}

GtkCellRenderer*
gtk_wx_cell_renderer_new (void)
{
    return (GtkCellRenderer*) g_object_new (GTK_TYPE_WX_CELL_RENDERER, NULL);
}

static void
gtk_wx_cell_renderer_get_size (GtkCellRenderer *renderer,
                               GtkWidget       *widget,
                               GdkRectangle    *cell_area,
                               gint            *x_offset,
                               gint            *y_offset,
                               gint            *width,
                               gint            *height)
{
    GtkWxCellRenderer *wxrenderer = (GtkWxCellRenderer *) renderer;
    wxDataViewCustomCell *cell = wxrenderer->cell;

    wxSize size = cell->GetSize();

    gint calc_width  = (gint) renderer->xpad * 2 + size.x;
    gint calc_height = (gint) renderer->ypad * 2 + size.y;

    if (x_offset)
        *x_offset = 0;
    if (y_offset)
        *y_offset = 0;

    if (cell_area && size.x > 0 && size.y > 0)
    {
        if (x_offset)
        {
            *x_offset = (gint)((renderer->xalign *
                               (cell_area->width - calc_width - 2 * renderer->xpad)));
            *x_offset = MAX (*x_offset, 0) + renderer->xpad;
        }
        if (y_offset)
        {
            *y_offset = (gint)((renderer->yalign *
                               (cell_area->height - calc_height - 2 * renderer->ypad)));
            *y_offset = MAX (*y_offset, 0) + renderer->ypad;
        }
    }

    if (width)
        *width = calc_width;

    if (height)
        *height = calc_height;
}

static void
gtk_wx_cell_renderer_render (GtkCellRenderer      *renderer,
                             GdkWindow            *window,
                             GtkWidget            *widget,
                             GdkRectangle         *background_area,
                             GdkRectangle         *cell_area,
                             GdkRectangle         *expose_area,
                             GtkCellRendererState  flags)

{
    GtkWxCellRenderer *wxrenderer = (GtkWxCellRenderer *) renderer;
    wxDataViewCustomCell *cell = wxrenderer->cell;

    GdkRectangle rect;
    gtk_wx_cell_renderer_get_size (renderer, widget, cell_area,
                                   &rect.x,
                                   &rect.y,
                                   &rect.width,
                                   &rect.height);

    rect.x += cell_area->x;
    rect.y += cell_area->y;
    rect.width  -= renderer->xpad * 2;
    rect.height -= renderer->ypad * 2;

    GdkRectangle dummy;
    if (gdk_rectangle_intersect (expose_area, &rect, &dummy))
    {
        wxRect renderrect( rect.x, rect.y, rect.width, rect.height );
        wxWindowDC* dc = (wxWindowDC*) cell->GetDC();
        if (dc->m_window == NULL)
        {
            dc->m_window = window;
            dc->SetUpDC();
        }

        int state = 0;
        if (flags & GTK_CELL_RENDERER_SELECTED)
            state |= wxDATAVIEW_CELL_SELECTED;
        if (flags & GTK_CELL_RENDERER_PRELIT)
            state |= wxDATAVIEW_CELL_PRELIT;
        if (flags & GTK_CELL_RENDERER_INSENSITIVE)
            state |= wxDATAVIEW_CELL_INSENSITIVE;
        if (flags & GTK_CELL_RENDERER_INSENSITIVE)
            state |= wxDATAVIEW_CELL_INSENSITIVE;
        if (flags & GTK_CELL_RENDERER_FOCUSED)
            state |= wxDATAVIEW_CELL_FOCUSED;
        cell->Render( renderrect, dc, state );
    }
}

static gboolean
gtk_wx_cell_renderer_activate(
                        GtkCellRenderer         *renderer,
                        GdkEvent                *event,
                        GtkWidget               *widget,
                        const gchar             *path,
                        GdkRectangle            *background_area,
                        GdkRectangle            *cell_area,
                        GtkCellRendererState     flags )
{
    GtkWxCellRenderer *wxrenderer = (GtkWxCellRenderer *) renderer;
    wxDataViewCustomCell *cell = wxrenderer->cell;

    GdkRectangle rect;
    gtk_wx_cell_renderer_get_size (renderer, widget, cell_area,
                                   &rect.x,
                                   &rect.y,
                                   &rect.width,
                                   &rect.height);

    rect.x += cell_area->x;
    rect.y += cell_area->y;
    rect.width  -= renderer->xpad * 2;
    rect.height -= renderer->ypad * 2;

    wxRect renderrect( rect.x, rect.y, rect.width, rect.height );

    wxDataViewListModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *treepath = gtk_tree_path_new_from_string( path );
    unsigned int model_row = (unsigned int)gtk_tree_path_get_indices (treepath)[0];
    gtk_tree_path_free( treepath );

    unsigned int model_col = cell->GetOwner()->GetModelColumn();

    if (event->type == GDK_BUTTON_PRESS)
    {
        GdkEventButton *button_event = (GdkEventButton*) event;
        wxPoint pt( ((int) button_event->x) - renderrect.x,
                    ((int) button_event->y) - renderrect.y );

        bool ret = false;
        if (button_event->button == 1)
        {
            if (cell->LeftClick( pt, renderrect, model, model_col, model_row ))
                ret = true;
            // TODO: query system double-click time
            if (button_event->time - wxrenderer->last_click < 400)
                if (cell->Activate( renderrect, model, model_col, model_row ))
                    ret = true;
        }
        if (button_event->button == 3)
        {
            if (cell->RightClick( pt, renderrect, model, model_col, model_row ))
                ret = true;
        }

        wxrenderer->last_click = button_event->time;

        return ret;
    }

    return false;
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
    virtual bool RowInserted( unsigned int before );
    virtual bool RowDeleted( unsigned int row );
    virtual bool RowChanged( unsigned int row );
    virtual bool ValueChanged( unsigned int col, unsigned int row );
    virtual bool RowsReordered( unsigned int *new_order );
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
    unsigned int pos = m_wx_model->GetNumberOfRows()-1;

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
    GtkTreeIter iter;
    iter.stamp = m_gtk_store->stamp;
    iter.user_data = (gpointer) 0;

    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_path_append_index (path, (gint) 0);
    gtk_tree_model_row_inserted (GTK_TREE_MODEL (m_gtk_store), path, &iter);
    gtk_tree_path_free (path);

    return true;
}

bool wxGtkDataViewListModelNotifier::RowInserted( unsigned int before )
{
    GtkTreeIter iter;
    iter.stamp = m_gtk_store->stamp;
    iter.user_data = (gpointer) before;

    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_path_append_index (path, (gint) before);
    gtk_tree_model_row_inserted (GTK_TREE_MODEL (m_gtk_store), path, &iter);
    gtk_tree_path_free (path);

    return true;
}

bool wxGtkDataViewListModelNotifier::RowDeleted( unsigned int row )
{
    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_path_append_index (path, (gint) row);
    gtk_tree_model_row_deleted (GTK_TREE_MODEL (m_gtk_store), path);
    gtk_tree_path_free (path);

    return true;
}

bool wxGtkDataViewListModelNotifier::RowChanged( unsigned int row )
{
    GtkTreeIter iter;
    iter.stamp = m_gtk_store->stamp;
    iter.user_data = (gpointer) row;
    GtkTreePath *path = gtk_tree_model_get_path (GTK_TREE_MODEL (m_gtk_store), &iter);
    gtk_tree_model_row_changed (GTK_TREE_MODEL (m_gtk_store), path, &iter);
    gtk_tree_path_free (path);

    return true;
}

bool wxGtkDataViewListModelNotifier::ValueChanged( unsigned int model_col, unsigned int model_row )
{
    // This adds GTK+'s missing MVC logic for ValueChanged
    wxNode *node = GetOwner()->m_viewingColumns.GetFirst();
    while (node)
    {
        wxDataViewViewingColumn* viewing_column = (wxDataViewViewingColumn*) node->GetData();
        if (viewing_column->m_modelColumn == model_col)
        {
            GtkTreeView *widget = GTK_TREE_VIEW(viewing_column->m_viewColumn->GetOwner()->m_treeview);
            GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(viewing_column->m_viewColumn->GetGtkHandle());

            // Get cell area
            GtkTreePath *path = gtk_tree_path_new();
            gtk_tree_path_append_index( path, model_row );
            GdkRectangle cell_area;
            gtk_tree_view_get_cell_area( widget, path, column, &cell_area );
            gtk_tree_path_free( path );

            int ydiff = column->button->allocation.height;
            // Redraw
            gtk_widget_queue_draw_area( GTK_WIDGET(widget),
                cell_area.x, ydiff + cell_area.y, cell_area.width, cell_area.height );
        }

        node = node->GetNext();
    }

    return true;
}

bool wxGtkDataViewListModelNotifier::RowsReordered( unsigned int *new_order )
{
    // Assume sizeof(unsigned int)= == sizeof(gint)

    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_model_rows_reordered (GTK_TREE_MODEL (m_gtk_store), path, NULL, (gint*)new_order);
    gtk_tree_path_free (path);

    // This adds GTK+'s missing MVC logic for RowsReordered
    wxNode *node = GetOwner()->m_viewingColumns.GetFirst();
    while (node)
    {
        wxDataViewViewingColumn* viewing_column = (wxDataViewViewingColumn*) node->GetData();
        GtkTreeView *widget = GTK_TREE_VIEW(viewing_column->m_viewColumn->GetOwner()->m_treeview);
        // Doesn't work yet...
        gtk_widget_queue_draw( GTK_WIDGET(widget) );

        node = node->GetNext();
    }

    return true;
}

bool wxGtkDataViewListModelNotifier::Cleared()
{
    return false;
}

// ---------------------------------------------------------
// wxDataViewCell
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCell, wxDataViewCellBase)

wxDataViewCell::wxDataViewCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCellBase( varianttype, mode )
{
    m_renderer = NULL;
}

// ---------------------------------------------------------
// wxDataViewTextCell
// ---------------------------------------------------------

extern "C" {
static void wxGtkTextRendererEditedCallback( GtkCellRendererText *renderer,
    gchar *arg1, gchar *arg2, gpointer user_data );
}

static void wxGtkTextRendererEditedCallback( GtkCellRendererText *renderer,
    gchar *arg1, gchar *arg2, gpointer user_data )
{
    wxDataViewTextCell *cell = (wxDataViewTextCell*) user_data;

    wxString tmp = wxGTK_CONV_BACK( arg2 );
    wxVariant value = tmp;
    if (!cell->Validate( value ))
        return;

    wxDataViewListModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *path = gtk_tree_path_new_from_string( arg1 );
    unsigned int model_row = (unsigned int)gtk_tree_path_get_indices (path)[0];
    gtk_tree_path_free( path );

    unsigned int model_col = cell->GetOwner()->GetModelColumn();

    model->SetValue( value, model_col, model_row );
    model->ValueChanged( model_col, model_row );
}

IMPLEMENT_CLASS(wxDataViewTextCell, wxDataViewCell)

wxDataViewTextCell::wxDataViewTextCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
    m_renderer = (void*) gtk_cell_renderer_text_new();

    if (m_mode & wxDATAVIEW_CELL_EDITABLE)
    {
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_BOOLEAN );
        g_value_set_boolean( &gvalue, true );
        g_object_set_property( G_OBJECT(m_renderer), "editable", &gvalue );
        g_value_unset( &gvalue );

        g_signal_connect_after( m_renderer, "edited", G_CALLBACK(wxGtkTextRendererEditedCallback), this );
    }
}

bool wxDataViewTextCell::SetValue( const wxVariant &value )
{
    wxString tmp = value;

    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_STRING );
    g_value_set_string( &gvalue, wxGTK_CONV( tmp ) );
    g_object_set_property( G_OBJECT(m_renderer), "text", &gvalue );
    g_value_unset( &gvalue );

    return true;
}

bool wxDataViewTextCell::GetValue( wxVariant &value )
{
    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_STRING );
    g_object_get_property( G_OBJECT(m_renderer), "text", &gvalue );
    wxString tmp = wxGTK_CONV_BACK( g_value_get_string( &gvalue ) );
    g_value_unset( &gvalue );

    value = tmp;

    return true;
}

// --------------------------------------------------------- 
// wxDataViewBitmapCell
// --------------------------------------------------------- 

IMPLEMENT_CLASS(wxDataViewBitmapCell, wxDataViewCell)

wxDataViewBitmapCell::wxDataViewBitmapCell( const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
    m_renderer = (void*) gtk_cell_renderer_pixbuf_new();
}

bool wxDataViewBitmapCell::SetValue( const wxVariant &value )
{
    if (value.GetType() == wxT("wxBitmap"))
    {
        wxBitmap bitmap;
        bitmap << value;
        
        // This may create a Pixbuf representation in the
        // wxBitmap object (and it will stay there)
        GdkPixbuf *pixbuf = bitmap.GetPixbuf();
        
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_OBJECT );
        g_value_set_object( &gvalue, pixbuf );
        g_object_set_property( G_OBJECT(m_renderer), "pixbuf", &gvalue );
        g_value_unset( &gvalue );
        
        return true;
    }
    
    if (value.GetType() == wxT("wxIcon"))
    {
        wxIcon bitmap;
        bitmap << value;
        
        // This may create a Pixbuf representation in the
        // wxBitmap object (and it will stay there)
        GdkPixbuf *pixbuf = bitmap.GetPixbuf();
        
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_OBJECT );
        g_value_set_object( &gvalue, pixbuf );
        g_object_set_property( G_OBJECT(m_renderer), "pixbuf", &gvalue );
        g_value_unset( &gvalue );
        
        return true;
    }
    
    return false;
}

bool wxDataViewBitmapCell::GetValue( wxVariant &value )
{
    return false;
}
    
// ---------------------------------------------------------
// wxDataViewToggleCell
// ---------------------------------------------------------

extern "C" {
static void wxGtkToggleRendererToggledCallback( GtkCellRendererToggle *renderer,
    gchar *path, gpointer user_data );
}

static void wxGtkToggleRendererToggledCallback( GtkCellRendererToggle *renderer,
    gchar *path, gpointer user_data )
{
    wxDataViewToggleCell *cell = (wxDataViewToggleCell*) user_data;

    // get old value
    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_BOOLEAN );
    g_object_get_property( G_OBJECT(renderer), "active", &gvalue );
    bool tmp = g_value_get_boolean( &gvalue );
    g_value_unset( &gvalue );
    // invert it
    tmp = !tmp;

    wxVariant value = tmp;
    if (!cell->Validate( value ))
        return;

    wxDataViewListModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *gtk_path = gtk_tree_path_new_from_string( path );
    unsigned int model_row = (unsigned int)gtk_tree_path_get_indices (gtk_path)[0];
    gtk_tree_path_free( gtk_path );

    unsigned int model_col = cell->GetOwner()->GetModelColumn();

    model->SetValue( value, model_col, model_row );
    model->ValueChanged( model_col, model_row );
}

IMPLEMENT_CLASS(wxDataViewToggleCell, wxDataViewCell)

wxDataViewToggleCell::wxDataViewToggleCell( const wxString &varianttype,
                        wxDataViewCellMode mode ) :
    wxDataViewCell( varianttype, mode )
{
    m_renderer = (void*) gtk_cell_renderer_toggle_new();

    if (m_mode & wxDATAVIEW_CELL_ACTIVATABLE)
    {
        g_signal_connect_after( m_renderer, "toggled", G_CALLBACK(wxGtkToggleRendererToggledCallback), this );
    }
    else
    {

        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_BOOLEAN );
        g_value_set_boolean( &gvalue, false );
        g_object_set_property( G_OBJECT(m_renderer), "activatable", &gvalue );
        g_value_unset( &gvalue );

        GValue gvalue2 = { 0, };
        g_value_init( &gvalue2, gtk_cell_renderer_mode_get_type() );
        g_value_set_enum( &gvalue2, GTK_CELL_RENDERER_MODE_INERT );
        g_object_set_property( G_OBJECT(m_renderer), "mode", &gvalue2 );
        g_value_unset( &gvalue2 );

    }
}

bool wxDataViewToggleCell::SetValue( const wxVariant &value )
{
    bool tmp = value;

    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_BOOLEAN );
    g_value_set_boolean( &gvalue, tmp );
    g_object_set_property( G_OBJECT(m_renderer), "active", &gvalue );
    g_value_unset( &gvalue );

    return true;
}

bool wxDataViewToggleCell::GetValue( wxVariant &value )
{
    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_BOOLEAN );
    g_object_get_property( G_OBJECT(m_renderer), "active", &gvalue );
    bool tmp = g_value_get_boolean( &gvalue );
    g_value_unset( &gvalue );

    value = tmp;

    return true;
}

// ---------------------------------------------------------
// wxDataViewCustomCell
// ---------------------------------------------------------

class wxDataViewCtrlDC: public wxWindowDC
{
public:
    wxDataViewCtrlDC( wxDataViewCtrl *window )
    {
        GtkWidget *widget = window->m_treeview;
        // Set later
        m_window = NULL;

        m_context = window->GtkGetPangoDefaultContext();
        m_layout = pango_layout_new( m_context );
        m_fontdesc = pango_font_description_copy( widget->style->font_desc );

        m_cmap = gtk_widget_get_colormap( widget ? widget : window->m_widget );

        // Set m_window later
        // SetUpDC();
        // m_owner = window;
    }
};

// ---------------------------------------------------------
// wxDataViewCustomCell
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewCustomCell, wxDataViewCell)

wxDataViewCustomCell::wxDataViewCustomCell( const wxString &varianttype,
                          wxDataViewCellMode mode, bool no_init ) :
    wxDataViewCell( varianttype, mode )
{
    m_dc = NULL;

    if (no_init)
        m_renderer = NULL;
    else
        Init();
}

bool wxDataViewCustomCell::Init()
{
    GtkWxCellRenderer *renderer = (GtkWxCellRenderer *) gtk_wx_cell_renderer_new();
    renderer->cell = this;

    m_renderer = (void*) renderer;

    if (m_mode & wxDATAVIEW_CELL_ACTIVATABLE)
    {
        GValue gvalue = { 0, };
        g_value_init( &gvalue, gtk_cell_renderer_mode_get_type() );
        g_value_set_enum( &gvalue, GTK_CELL_RENDERER_MODE_ACTIVATABLE );
        g_object_set_property( G_OBJECT(m_renderer), "mode", &gvalue );
        g_value_unset( &gvalue );
    }

    return true;
}

wxDataViewCustomCell::~wxDataViewCustomCell()
{
    if (m_dc)
        delete m_dc;
}

wxDC *wxDataViewCustomCell::GetDC()
{
    if (m_dc == NULL)
    {
        if (GetOwner() == NULL)
            return NULL;
        if (GetOwner()->GetOwner() == NULL)
            return NULL;
        m_dc = new wxDataViewCtrlDC( GetOwner()->GetOwner() );
    }

    return m_dc;
}

// ---------------------------------------------------------
// wxDataViewProgressCell
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewProgressCell, wxDataViewCustomCell)

wxDataViewProgressCell::wxDataViewProgressCell( const wxString &label,
    const wxString &varianttype, wxDataViewCellMode mode ) :
    wxDataViewCustomCell( varianttype, mode, true )
{
    m_label = label;
    m_value = 0;

#ifdef __WXGTK26__
    if (!gtk_check_version(2,6,0))
    {
        m_renderer = (void*) gtk_cell_renderer_progress_new();

        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_STRING );
        g_value_set_boolean( &gvalue, wxGTK_CONV(m_label) );
        g_object_set_property( G_OBJECT(m_renderer), "text", &gvalue );
        g_value_unset( &gvalue );
    }
    else
#endif
    {
        // Use custom cell code
        wxDataViewCustomCell::Init();
    }
}

wxDataViewProgressCell::~wxDataViewProgressCell()
{
}

bool wxDataViewProgressCell::SetValue( const wxVariant &value )
{
#ifdef __WXGTK26__
    if (!gtk_check_version(2,6,0))
    {
        gint tmp = (int) value;
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_INT );
        g_value_set_boolean( &gvalue, tmp );
        g_object_set_property( G_OBJECT(m_renderer), "value", &gvalue );
        g_value_unset( &gvalue );
    }
    else
#endif
    {
        m_value = (long) value;

        if (m_value < 0) m_value = 0;
        if (m_value > 100) m_value = 100;
    }

    return true;
}

bool wxDataViewProgressCell::Render( wxRect cell, wxDC *dc, int state )
{
    double pct = (double)m_value / 100.0;
    wxRect bar = cell;
    bar.width = (int)(cell.width * pct);
    dc->SetPen( *wxTRANSPARENT_PEN );
    dc->SetBrush( *wxBLUE_BRUSH );
    dc->DrawRectangle( bar );

    dc->SetBrush( *wxTRANSPARENT_BRUSH );
    dc->SetPen( *wxBLACK_PEN );
    dc->DrawRectangle( cell );

    return true;
}

wxSize wxDataViewProgressCell::GetSize()
{
    return wxSize(40,12);
}

// ---------------------------------------------------------
// wxDataViewDateCell
// ---------------------------------------------------------

class wxDataViewDateCellPopupTransient: public wxPopupTransientWindow
{
public:
    wxDataViewDateCellPopupTransient( wxWindow* parent, wxDateTime *value,
        wxDataViewListModel *model, unsigned int col, unsigned int row ) :
        wxPopupTransientWindow( parent, wxBORDER_SIMPLE )
    {
        m_model = model;
        m_col = col;
        m_row = row;
        m_cal = new wxCalendarCtrl( this, -1, *value );
        wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add( m_cal, 1, wxGROW );
        SetSizer( sizer );
        sizer->Fit( this );
    }

    virtual void OnDismiss()
    {
    }

    void OnCalendar( wxCalendarEvent &event );

    wxCalendarCtrl      *m_cal;
    wxDataViewListModel *m_model;
    unsigned int               m_col;
    unsigned int               m_row;

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxDataViewDateCellPopupTransient,wxPopupTransientWindow)
    EVT_CALENDAR( -1, wxDataViewDateCellPopupTransient::OnCalendar )
END_EVENT_TABLE()

void wxDataViewDateCellPopupTransient::OnCalendar( wxCalendarEvent &event )
{
    wxDateTime date = event.GetDate();
    wxVariant value = date;
    m_model->SetValue( value, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );
    DismissAndNotify();
}

IMPLEMENT_CLASS(wxDataViewDateCell, wxDataViewCustomCell)

wxDataViewDateCell::wxDataViewDateCell( const wxString &varianttype,
                        wxDataViewCellMode mode ) :
    wxDataViewCustomCell( varianttype, mode )
{
}

bool wxDataViewDateCell::SetValue( const wxVariant &value )
{
    m_date = value.GetDateTime();

    return true;
}

bool wxDataViewDateCell::Render( wxRect cell, wxDC *dc, int state )
{
    dc->SetFont( GetOwner()->GetOwner()->GetFont() );
    wxString tmp = m_date.FormatDate();
    dc->DrawText( tmp, cell.x, cell.y );

    return true;
}

wxSize wxDataViewDateCell::GetSize()
{
    wxDataViewCtrl* view = GetOwner()->GetOwner();
    wxString tmp = m_date.FormatDate();
    wxCoord x,y,d;
    view->GetTextExtent( tmp, &x, &y, &d );
    return wxSize(x,y+d);
}

bool wxDataViewDateCell::Activate( wxRect cell, wxDataViewListModel *model, unsigned int col, unsigned int row )
{
    wxVariant variant;
    model->GetValue( variant, col, row );
    wxDateTime value = variant.GetDateTime();

    wxDataViewDateCellPopupTransient *popup = new wxDataViewDateCellPopupTransient(
        GetOwner()->GetOwner()->GetParent(), &value, model, col, row );
    wxPoint pos = wxGetMousePosition();
    popup->Move( pos );
    popup->Layout();
    popup->Popup( popup->m_cal );

    return true;
}

// ---------------------------------------------------------
// wxDataViewColumn
// ---------------------------------------------------------

extern "C" {
static void wxGtkTreeCellDataFunc( GtkTreeViewColumn *column,
                            GtkCellRenderer *cell,
                            GtkTreeModel *model,
                            GtkTreeIter *iter,
                            gpointer data );
}


static void wxGtkTreeCellDataFunc( GtkTreeViewColumn *column,
                            GtkCellRenderer *renderer,
                            GtkTreeModel *model,
                            GtkTreeIter *iter,
                            gpointer data )
{
    g_return_if_fail (GTK_IS_WX_LIST_STORE (model));
    GtkWxListStore *list_store = (GtkWxListStore *) model;

    wxDataViewCell *cell = (wxDataViewCell*) data;

    unsigned int model_row = (unsigned int) iter->user_data;

    wxVariant value;
    list_store->model->GetValue( value, cell->GetOwner()->GetModelColumn(), model_row );

    if (value.GetType() != cell->GetVariantType())
        wxLogError( wxT("Wrong type, required: %s but: %s"), 
                    value.GetType().c_str(), 
                    cell->GetVariantType().c_str() );

    cell->SetValue( value );
}

IMPLEMENT_CLASS(wxDataViewColumn, wxDataViewColumnBase)

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewCell *cell, unsigned int model_column,
    int width, int flags ) :
    wxDataViewColumnBase( title, cell, model_column, width, flags )
{
    GtkCellRenderer *renderer = (GtkCellRenderer *) cell->GetGtkHandle();

    GtkTreeViewColumn *column = gtk_tree_view_column_new();

    gtk_tree_view_column_set_title( column, wxGTK_CONV(title) );

    if (flags & wxDATAVIEW_COL_RESIZABLE)
        gtk_tree_view_column_set_resizable( column, true );
    if (flags & wxDATAVIEW_COL_HIDDEN)
        gtk_tree_view_column_set_visible( column, false );
    if (flags & wxDATAVIEW_COL_SORTABLE)
        gtk_tree_view_column_set_sort_indicator( column, true );

    if (width > 0)
    {
        gtk_tree_view_column_set_fixed_width( column, width );
        gtk_tree_view_column_set_sizing( column, GTK_TREE_VIEW_COLUMN_FIXED );
    }

    gtk_tree_view_column_pack_end( column, renderer, FALSE );

    gtk_tree_view_column_set_cell_data_func( column, renderer,
        wxGtkTreeCellDataFunc, (gpointer) cell, NULL );

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

int wxDataViewColumn::GetWidth()
{
    return gtk_tree_view_column_get_width( (GtkTreeViewColumn *)m_column );
}

void wxDataViewColumn::SetFixedWidth( int width )
{
    gtk_tree_view_column_set_fixed_width( (GtkTreeViewColumn *)m_column, width );
}

int wxDataViewColumn::GetFixedWidth()
{
    return gtk_tree_view_column_get_fixed_width( (GtkTreeViewColumn *)m_column );
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase)

wxDataViewCtrl::~wxDataViewCtrl()
{
    if (m_notifier)
        GetModel()->RemoveNotifier( m_notifier );
}

void wxDataViewCtrl::Init()
{
    m_notifier = NULL;
}

bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style, const wxValidator& validator )
{
    Init();

    m_needParent = true;
    m_acceptsFocus = true;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator ))
    {
        wxFAIL_MSG( wxT("wxDataViewCtrl creation failed") );
        return false;
    }

    m_widget = gtk_scrolled_window_new (NULL, NULL);

    GtkScrolledWindowSetBorder(m_widget, style);

    m_treeview = gtk_tree_view_new();
    gtk_container_add (GTK_CONTAINER (m_widget), m_treeview);

    if (style & wxDV_MULTIPLE)
    {
        GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
        gtk_tree_selection_set_mode( selection, GTK_SELECTION_MULTIPLE );
    }

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_widget),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_show (m_treeview);

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

    m_notifier = new wxGtkDataViewListModelNotifier( gtk_store, model );

    model->AddNotifier( m_notifier );

    gtk_tree_view_set_model( GTK_TREE_VIEW(m_treeview), GTK_TREE_MODEL(gtk_store) );
    g_object_unref( gtk_store );

    return true;
}

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    if (!wxDataViewCtrlBase::AppendColumn(col))
        return false;

    GtkTreeViewColumn *column = (GtkTreeViewColumn *)col->GetGtkHandle();

    gtk_tree_view_append_column( GTK_TREE_VIEW(m_treeview), column );

    return true;
}

void wxDataViewCtrl::SetSelection( int row )
{
}

void wxDataViewCtrl::SetSelectionRange( unsigned int from, unsigned int to )
{
}

void wxDataViewCtrl::SetSelections( const wxArrayInt& aSelections)
{
}
    
bool wxDataViewCtrl::IsSelected( unsigned int row ) const
{
    return false;
}

int wxDataViewCtrl::GetSelection() const
{
    return -1;
}

int wxDataViewCtrl::GetSelections(wxArrayInt& aSelections) const
{
    return 0;
}


#endif
    // !wxUSE_GENERICDATAVIEWCTRL

#endif
    // wxUSE_DATAVIEWCTRL
