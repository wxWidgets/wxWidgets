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
        const GTypeInfo list_store_info =
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

    return list_store->model->GetColumnCount();
}

static GType
wxgtk_list_store_get_column_type (GtkTreeModel *tree_model,
                                  gint          index)
{
    GtkWxListStore *list_store = (GtkWxListStore *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_LIST_STORE (tree_model), G_TYPE_INVALID);

    GType gtype = G_TYPE_INVALID;

    wxString wxtype = list_store->model->GetColumnType( (unsigned int) index );

    if (wxtype == wxT("string"))
        gtype = G_TYPE_STRING;
    else
    {
        wxFAIL_MSG( _T("non-string columns not supported yet") );
    }

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

    if (i >= list_store->model->GetRowCount())
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
    wxString mtype = model->GetColumnType( (unsigned int) column );
    if (mtype == wxT("string"))
    {
        wxVariant variant;
        g_value_init( value, G_TYPE_STRING );
        model->GetValue( variant,
                         (unsigned int) column,
                         (unsigned int) iter->user_data );

        // FIXME: we should support different encodings here
        g_value_set_string( value, wxGTK_CONV_SYS(variant.GetString()) );
    }
    else
    {
        wxFAIL_MSG( _T("non-string columns not supported yet") );
    }
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

    if (n >= (int) list_store->model->GetRowCount()-1)
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
        return (gint) list_store->model->GetRowCount();

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

    if (n >= (gint) list_store->model->GetRowCount())
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
// define new GTK+ class wxGtkRendererRenderer
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
  wxDataViewCustomRenderer *cell;
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
static GtkCellEditable *gtk_wx_cell_renderer_start_editing(
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
        const GTypeInfo cell_wx_info =
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
    cell_class->start_editing = gtk_wx_cell_renderer_start_editing;
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



static GtkCellEditable *gtk_wx_cell_renderer_start_editing(
                        GtkCellRenderer         *renderer,
                        GdkEvent                *event,
                        GtkWidget               *widget,
                        const gchar             *path,
                        GdkRectangle            *background_area,
                        GdkRectangle            *cell_area,
                        GtkCellRendererState     flags )
{
    GtkWxCellRenderer *wxrenderer = (GtkWxCellRenderer *) renderer;
    wxDataViewCustomRenderer *cell = wxrenderer->cell;
    if (!cell->HasEditorCtrl())
        return NULL;
        
    GdkRectangle rect;
    gtk_wx_cell_renderer_get_size (renderer, widget, cell_area,
                                   &rect.x,
                                   &rect.y,
                                   &rect.width,
                                   &rect.height);

    rect.x += cell_area->x;
    rect.y += cell_area->y;
//    rect.width  -= renderer->xpad * 2;
//    rect.height -= renderer->ypad * 2;

//    wxRect renderrect( rect.x, rect.y, rect.width, rect.height );
    wxRect renderrect( cell_area->x, cell_area->y, cell_area->width, cell_area->height );

    wxDataViewListModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *treepath = gtk_tree_path_new_from_string( path );
    unsigned int model_row = (unsigned int)gtk_tree_path_get_indices (treepath)[0];
    gtk_tree_path_free( treepath );

    cell->StartEditing( model_row, renderrect );
    
    return NULL;
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
    wxDataViewCustomRenderer *cell = wxrenderer->cell;

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
    wxDataViewCustomRenderer *cell = wxrenderer->cell;

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
    wxDataViewCustomRenderer *cell = wxrenderer->cell;

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

    if (!event)
    {
        bool ret = false;
        
        // activated by <ENTER>
        if (cell->Activate( renderrect, model, model_col, model_row ))
                    ret = true;
        
        return ret;
    }
    else if (event->type == GDK_BUTTON_PRESS)
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

    virtual bool Freed()
    {
        m_wx_model = NULL;
        m_gtk_store = NULL;
        return wxDataViewListModelNotifier::Freed();
    }

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
    unsigned int pos = m_wx_model->GetRowCount()-1;

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
    wxObjectList::compatibility_iterator
        node = GetOwner()->m_viewingColumns.GetFirst();
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
    wxObjectList::compatibility_iterator
        node = GetOwner()->m_viewingColumns.GetFirst();
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
// wxDataViewRenderer
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewRenderer, wxDataViewRendererBase)

wxDataViewRenderer::wxDataViewRenderer( const wxString &varianttype, wxDataViewCellMode mode,
                                        int align ) :
    wxDataViewRendererBase( varianttype, mode, align )
{
    m_renderer = NULL;

    // NOTE: SetMode() and SetAlignment() needs to be called in the renderer's ctor,
    //       after the m_renderer pointer has been initialized
}

void wxDataViewRenderer::SetMode( wxDataViewCellMode mode )
{
    GtkCellRendererMode gtkMode;
    switch (mode)
    {
    case wxDATAVIEW_CELL_INERT:
        gtkMode = GTK_CELL_RENDERER_MODE_INERT;
        break;
    case wxDATAVIEW_CELL_ACTIVATABLE:
        gtkMode = GTK_CELL_RENDERER_MODE_ACTIVATABLE;
        break;
    case wxDATAVIEW_CELL_EDITABLE:
        gtkMode = GTK_CELL_RENDERER_MODE_EDITABLE;
        break;
    }

    GValue gvalue = { 0, };
    g_value_init( &gvalue, gtk_cell_renderer_mode_get_type() );
    g_value_set_enum( &gvalue, gtkMode );
    g_object_set_property( G_OBJECT(m_renderer), "mode", &gvalue );
    g_value_unset( &gvalue );
}

wxDataViewCellMode wxDataViewRenderer::GetMode() const
{
    wxDataViewCellMode ret;

    GValue gvalue;
    g_object_get( G_OBJECT(m_renderer), "mode", &gvalue, NULL);

    switch (g_value_get_enum(&gvalue))
    {
    case GTK_CELL_RENDERER_MODE_INERT:
        ret = wxDATAVIEW_CELL_INERT;
        break;
    case GTK_CELL_RENDERER_MODE_ACTIVATABLE:
        ret = wxDATAVIEW_CELL_ACTIVATABLE;
        break;
    case GTK_CELL_RENDERER_MODE_EDITABLE:
        ret = wxDATAVIEW_CELL_EDITABLE;
        break;
    }

    g_value_unset( &gvalue );

    return ret;
}

void wxDataViewRenderer::SetAlignment( int align )
{
    // horizontal alignment:

    gfloat xalign = 0.0;
    if (align & wxALIGN_RIGHT)
        xalign = 1.0;
    else if (align & wxALIGN_CENTER_HORIZONTAL)
        xalign = 0.5;

    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_FLOAT );
    g_value_set_float( &gvalue, xalign );
    g_object_set_property( G_OBJECT(m_renderer), "xalign", &gvalue );
    g_value_unset( &gvalue );

    // vertical alignment:

    gfloat yalign = 0.0;
    if (align & wxALIGN_BOTTOM)
        yalign = 1.0;
    else if (align & wxALIGN_CENTER_VERTICAL)
        yalign = 0.5;

    GValue gvalue2 = { 0, };
    g_value_init( &gvalue2, G_TYPE_FLOAT );
    g_value_set_float( &gvalue2, yalign );
    g_object_set_property( G_OBJECT(m_renderer), "yalign", &gvalue2 );
    g_value_unset( &gvalue2 );
}

int wxDataViewRenderer::GetAlignment() const
{
    int ret = 0;
    GValue gvalue;

    // horizontal alignment:

    g_object_get( G_OBJECT(m_renderer), "xalign", &gvalue, NULL );
    float xalign = g_value_get_float( &gvalue );
    if (xalign < 0.5)
        ret |= wxALIGN_LEFT;
    else if (xalign == 0.5)
        ret |= wxALIGN_CENTER_HORIZONTAL;
    else
        ret |= wxALIGN_RIGHT;
    g_value_unset( &gvalue );


    // vertical alignment:

    g_object_get( G_OBJECT(m_renderer), "yalign", &gvalue, NULL );
    float yalign = g_value_get_float( &gvalue );
    if (yalign < 0.5)
        ret |= wxALIGN_TOP;
    else if (yalign == 0.5)
        ret |= wxALIGN_CENTER_VERTICAL;
    else
        ret |= wxALIGN_BOTTOM;
    g_value_unset( &gvalue );

    return ret;
}



// ---------------------------------------------------------
// wxDataViewTextRenderer
// ---------------------------------------------------------

extern "C" {
static void wxGtkTextRendererEditedCallback( GtkCellRendererText *renderer,
    gchar *arg1, gchar *arg2, gpointer user_data );
}

static void wxGtkTextRendererEditedCallback( GtkCellRendererText *renderer,
    gchar *arg1, gchar *arg2, gpointer user_data )
{
    wxDataViewTextRenderer *cell = (wxDataViewTextRenderer*) user_data;

    wxString tmp = wxGTK_CONV_BACK_FONT(arg2, cell->GetOwner()->GetOwner()->GetFont());
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

IMPLEMENT_CLASS(wxDataViewTextRenderer, wxDataViewRenderer)

wxDataViewTextRenderer::wxDataViewTextRenderer( const wxString &varianttype, wxDataViewCellMode mode,
                                                int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_renderer = (GtkWidget*) gtk_cell_renderer_text_new();

    if (mode & wxDATAVIEW_CELL_EDITABLE)
    {
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_BOOLEAN );
        g_value_set_boolean( &gvalue, true );
        g_object_set_property( G_OBJECT(m_renderer), "editable", &gvalue );
        g_value_unset( &gvalue );

        g_signal_connect_after( m_renderer, "edited", G_CALLBACK(wxGtkTextRendererEditedCallback), this );
    }

    SetMode(mode);
    SetAlignment(align);
}

bool wxDataViewTextRenderer::SetValue( const wxVariant &value )
{
    wxString tmp = value;

    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_STRING );
    g_value_set_string( &gvalue, wxGTK_CONV_FONT( tmp, GetOwner()->GetOwner()->GetFont() ) );
    g_object_set_property( G_OBJECT(m_renderer), "text", &gvalue );
    g_value_unset( &gvalue );

    return true;
}

bool wxDataViewTextRenderer::GetValue( wxVariant &value ) const
{
    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_STRING );
    g_object_get_property( G_OBJECT(m_renderer), "text", &gvalue );
    wxString tmp = wxGTK_CONV_BACK_FONT( g_value_get_string( &gvalue ),
        wx_const_cast(wxDataViewTextRenderer*, this)->GetOwner()->GetOwner()->GetFont() );
    g_value_unset( &gvalue );

    value = tmp;

    return true;
}

void wxDataViewTextRenderer::SetAlignment( int align )
{
    wxDataViewRenderer::SetAlignment(align);

    if (gtk_check_version(2,10,0))
        return;

    // horizontal alignment:
    PangoAlignment pangoAlign = PANGO_ALIGN_LEFT;
    if (align & wxALIGN_RIGHT)
        pangoAlign = PANGO_ALIGN_RIGHT;
    else if (align & wxALIGN_CENTER_HORIZONTAL)
        pangoAlign = PANGO_ALIGN_CENTER;

    GValue gvalue = { 0, };
    g_value_init( &gvalue, gtk_cell_renderer_mode_get_type() );
    g_value_set_enum( &gvalue, pangoAlign );
    g_object_set_property( G_OBJECT(m_renderer), "alignment", &gvalue );
    g_value_unset( &gvalue );
}

// ---------------------------------------------------------
// wxDataViewBitmapRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewBitmapRenderer, wxDataViewRenderer)

wxDataViewBitmapRenderer::wxDataViewBitmapRenderer( const wxString &varianttype, wxDataViewCellMode mode,
                                                    int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_renderer = (GtkWidget*) gtk_cell_renderer_pixbuf_new();

    SetMode(mode);
    SetAlignment(align);
}

bool wxDataViewBitmapRenderer::SetValue( const wxVariant &value )
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

bool wxDataViewBitmapRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

// ---------------------------------------------------------
// wxDataViewToggleRenderer
// ---------------------------------------------------------

extern "C" {
static void wxGtkToggleRendererToggledCallback( GtkCellRendererToggle *renderer,
    gchar *path, gpointer user_data );
}

static void wxGtkToggleRendererToggledCallback( GtkCellRendererToggle *renderer,
    gchar *path, gpointer user_data )
{
    wxDataViewToggleRenderer *cell = (wxDataViewToggleRenderer*) user_data;

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

IMPLEMENT_CLASS(wxDataViewToggleRenderer, wxDataViewRenderer)

wxDataViewToggleRenderer::wxDataViewToggleRenderer( const wxString &varianttype,
                                                    wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_renderer = (GtkWidget*) gtk_cell_renderer_toggle_new();

    if (mode & wxDATAVIEW_CELL_ACTIVATABLE)
    {
        g_signal_connect_after( m_renderer, "toggled",
                                G_CALLBACK(wxGtkToggleRendererToggledCallback), this );
    }
    else
    {
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_BOOLEAN );
        g_value_set_boolean( &gvalue, false );
        g_object_set_property( G_OBJECT(m_renderer), "activatable", &gvalue );
        g_value_unset( &gvalue );
    }

    SetMode(mode);
    SetAlignment(align);
}

bool wxDataViewToggleRenderer::SetValue( const wxVariant &value )
{
    bool tmp = value;

    GValue gvalue = { 0, };
    g_value_init( &gvalue, G_TYPE_BOOLEAN );
    g_value_set_boolean( &gvalue, tmp );
    g_object_set_property( G_OBJECT(m_renderer), "active", &gvalue );
    g_value_unset( &gvalue );

    return true;
}

bool wxDataViewToggleRenderer::GetValue( wxVariant &value ) const
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
// wxDataViewCustomRenderer
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
// wxDataViewCustomRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewCustomRenderer, wxDataViewRenderer)

wxDataViewCustomRenderer::wxDataViewCustomRenderer( const wxString &varianttype,
                                                    wxDataViewCellMode mode, int align,
                                                    bool no_init ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_dc = NULL;

    if (no_init)
        m_renderer = NULL;
    else
        Init(mode, align);
}

bool wxDataViewCustomRenderer::Init(wxDataViewCellMode mode, int align)
{
    GtkWxCellRenderer *renderer = (GtkWxCellRenderer *) gtk_wx_cell_renderer_new();
    renderer->cell = this;

    m_renderer = (GtkWidget*) renderer;

    SetMode(mode);
    SetAlignment(align);

    return true;
}

wxDataViewCustomRenderer::~wxDataViewCustomRenderer()
{
    if (m_dc)
        delete m_dc;
}

wxDC *wxDataViewCustomRenderer::GetDC()
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
// wxDataViewProgressRenderer
// ---------------------------------------------------------

IMPLEMENT_CLASS(wxDataViewProgressRenderer, wxDataViewCustomRenderer)

wxDataViewProgressRenderer::wxDataViewProgressRenderer( const wxString &label,
    const wxString &varianttype, wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align, true )
{
    m_label = label;
    m_value = 0;

#ifdef __WXGTK26__
    if (!gtk_check_version(2,6,0))
    {
        m_renderer = (GtkWidget*) gtk_cell_renderer_progress_new();

        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_STRING );

        // FIXME: font encoding support
        g_value_set_string( &gvalue, wxGTK_CONV_SYS(m_label) );
        g_object_set_property( G_OBJECT(m_renderer), "text", &gvalue );
        g_value_unset( &gvalue );

        SetMode(mode);
        SetAlignment(align);
    }
    else
#endif
    {
        // Use custom cell code
        wxDataViewCustomRenderer::Init(mode, align);
    }
}

wxDataViewProgressRenderer::~wxDataViewProgressRenderer()
{
}

bool wxDataViewProgressRenderer::SetValue( const wxVariant &value )
{
#ifdef __WXGTK26__
    if (!gtk_check_version(2,6,0))
    {
        gint tmp = (long) value;
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_INT );
        g_value_set_int( &gvalue, tmp );
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

bool wxDataViewProgressRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

bool wxDataViewProgressRenderer::Render( wxRect cell, wxDC *dc, int state )
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

wxSize wxDataViewProgressRenderer::GetSize() const
{
    return wxSize(40,12);
}

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

class wxDataViewDateRendererPopupTransient: public wxPopupTransientWindow
{
public:
    wxDataViewDateRendererPopupTransient( wxWindow* parent, wxDateTime *value,
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

BEGIN_EVENT_TABLE(wxDataViewDateRendererPopupTransient,wxPopupTransientWindow)
    EVT_CALENDAR( -1, wxDataViewDateRendererPopupTransient::OnCalendar )
END_EVENT_TABLE()

void wxDataViewDateRendererPopupTransient::OnCalendar( wxCalendarEvent &event )
{
    wxDateTime date = event.GetDate();
    wxVariant value = date;
    m_model->SetValue( value, m_col, m_row );
    m_model->ValueChanged( m_col, m_row );
    DismissAndNotify();
}

IMPLEMENT_CLASS(wxDataViewDateRenderer, wxDataViewCustomRenderer)

wxDataViewDateRenderer::wxDataViewDateRenderer( const wxString &varianttype,
                        wxDataViewCellMode mode, int align ) :
    wxDataViewCustomRenderer( varianttype, mode, align )
{
    SetMode(mode);
    SetAlignment(align);
}

bool wxDataViewDateRenderer::SetValue( const wxVariant &value )
{
    m_date = value.GetDateTime();

    return true;
}

bool wxDataViewDateRenderer::GetValue( wxVariant &value ) const
{
    return false;
}

bool wxDataViewDateRenderer::Render( wxRect cell, wxDC *dc, int state )
{
    dc->SetFont( GetOwner()->GetOwner()->GetFont() );
    wxString tmp = m_date.FormatDate();
    dc->DrawText( tmp, cell.x, cell.y );

    return true;
}

wxSize wxDataViewDateRenderer::GetSize() const
{
    wxString tmp = m_date.FormatDate();
    wxCoord x,y,d;
    GetView()->GetTextExtent( tmp, &x, &y, &d );
    return wxSize(x,y+d);
}

bool wxDataViewDateRenderer::Activate( wxRect cell, wxDataViewListModel *model,
                                       unsigned int col, unsigned int row )
{
    wxVariant variant;
    model->GetValue( variant, col, row );
    wxDateTime value = variant.GetDateTime();

    wxDataViewDateRendererPopupTransient *popup = new wxDataViewDateRendererPopupTransient(
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


static gboolean
gtk_dataview_header_button_press_callback( GtkWidget *widget,
                                           GdkEventButton *gdk_event,
                                           wxDataViewColumn *column )
{
    if (gdk_event->type != GDK_BUTTON_PRESS)
        return TRUE;

    if (gdk_event->button == 1)
    {
        wxDataViewCtrl *dv = column->GetOwner();
        wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, dv->GetId() );
        event.SetDataViewColumn( column );
        event.SetModel( dv->GetModel() );
        dv->GetEventHandler()->ProcessEvent( event );
    }

    return TRUE;
}

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

    wxDataViewRenderer *cell = (wxDataViewRenderer*) data;

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

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewRenderer *cell,
                                    unsigned int model_column, int width,
                                    wxAlignment align, int flags ) :
    wxDataViewColumnBase( title, cell, model_column, width, align, flags )
{
    Init( align, flags, width );

    gtk_tree_view_column_set_clickable( GTK_TREE_VIEW_COLUMN(m_column), TRUE );
    SetTitle( title );
}

wxDataViewColumn::wxDataViewColumn( const wxBitmap &bitmap, wxDataViewRenderer *cell,
                                    unsigned int model_column, int width,
                                    wxAlignment align, int flags ) :
    wxDataViewColumnBase( bitmap, cell, model_column, width, align, flags )
{
    Init( align, flags, width );

    SetBitmap( bitmap );
}

void wxDataViewColumn::Init(wxAlignment align, int flags, int width)
{
    m_isConnected = false;

    GtkCellRenderer *renderer = (GtkCellRenderer *) GetRenderer()->GetGtkHandle();
    GtkTreeViewColumn *column = gtk_tree_view_column_new();
    m_column = (GtkWidget*) column;

    SetFlags( flags );
    SetAlignment( align );

    // NOTE: we prefer not to call SetMinWidth(wxDVC_DEFAULT_MINWIDTH);
    //       as GTK+ is smart and unless explicitely told, will set the minimal
    //       width to the title's lenght, which is a better default

    // the GTK_TREE_VIEW_COLUMN_FIXED is required by the "fixed height" mode
    // that we use for the wxDataViewCtrl
    gtk_tree_view_column_set_fixed_width( column, width < 0 ? wxDVC_DEFAULT_WIDTH : width );
    gtk_tree_view_column_set_sizing( column, GTK_TREE_VIEW_COLUMN_FIXED );

    gtk_tree_view_column_pack_end( column, renderer, TRUE );

    gtk_tree_view_column_set_cell_data_func( column, renderer,
        wxGtkTreeCellDataFunc, (gpointer) GetRenderer(), NULL );
}

wxDataViewColumn::~wxDataViewColumn()
{
}

void wxDataViewColumn::OnInternalIdle()
{
    if (m_isConnected)
        return;

    if (GTK_WIDGET_REALIZED(GetOwner()->m_treeview))
    {
        GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
        if (column->button)
        {
            g_signal_connect(column->button, "button_press_event",
                      G_CALLBACK (gtk_dataview_header_button_press_callback), this);

            m_isConnected = true;
        }
    }
}

void wxDataViewColumn::SetOwner( wxDataViewCtrl *owner )
{
    wxDataViewColumnBase::SetOwner( owner );

    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);

    gtk_tree_view_column_set_title( column, wxGTK_CONV_FONT(GetTitle(), GetOwner()->GetFont() ) );
}

void wxDataViewColumn::SetTitle( const wxString &title )
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);

    if (m_isConnected)
    {
        // disconnect before column->button gets recreated
        g_signal_handlers_disconnect_by_func( column->button,
                      (GtkWidget*) gtk_dataview_header_button_press_callback, this);

        m_isConnected = false;
    }

    // FIXME: can it really happen that we don't have the owner here??
    wxDataViewCtrl *ctrl = GetOwner();
    gtk_tree_view_column_set_title( column, ctrl ? wxGTK_CONV_FONT(title, ctrl->GetFont())
                                                 : wxGTK_CONV_SYS(title) );

    gtk_tree_view_column_set_widget( column, NULL );
}

wxString wxDataViewColumn::GetTitle() const
{
    const gchar *str = gtk_tree_view_column_get_title( GTK_TREE_VIEW_COLUMN(m_column) );
    return wxConvFileName->cMB2WX(str);
}

void wxDataViewColumn::SetBitmap( const wxBitmap &bitmap )
{
    wxDataViewColumnBase::SetBitmap( bitmap );

    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
    if (bitmap.Ok())
    {
        GtkImage *gtk_image = GTK_IMAGE( gtk_image_new() );

        GdkBitmap *mask = (GdkBitmap *) NULL;
        if (bitmap.GetMask())
            mask = bitmap.GetMask()->GetBitmap();

        if (bitmap.HasPixbuf())
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(gtk_image),
                                      bitmap.GetPixbuf());
        }
        else
        {
            gtk_image_set_from_pixmap(GTK_IMAGE(gtk_image),
                                      bitmap.GetPixmap(), mask);
        }
        gtk_widget_show( GTK_WIDGET(gtk_image) );

        gtk_tree_view_column_set_widget( column, GTK_WIDGET(gtk_image) );
    }
    else
    {
        gtk_tree_view_column_set_widget( column, NULL );
    }
}

void wxDataViewColumn::SetHidden( bool hidden )
{
    gtk_tree_view_column_set_visible( GTK_TREE_VIEW_COLUMN(m_column), !hidden );
}

void wxDataViewColumn::SetResizeable( bool resizeable )
{
    gtk_tree_view_column_set_resizable( GTK_TREE_VIEW_COLUMN(m_column), resizeable );
}

void wxDataViewColumn::SetAlignment( wxAlignment align )
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);

    gfloat xalign = 0.0;
    if (align == wxALIGN_RIGHT)
        xalign = 1.0;
    if (align == wxALIGN_CENTER_HORIZONTAL ||
        align == wxALIGN_CENTER)
        xalign = 0.5;

    gtk_tree_view_column_set_alignment( column, xalign );
}

wxAlignment wxDataViewColumn::GetAlignment() const
{
    gfloat xalign = gtk_tree_view_column_get_alignment( GTK_TREE_VIEW_COLUMN(m_column) );

    if (xalign == 1.0)
        return wxALIGN_RIGHT;
    if (xalign == 0.5)
        return wxALIGN_CENTER_HORIZONTAL;

    return wxALIGN_LEFT;
}

void wxDataViewColumn::SetSortable( bool sortable )
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
    gtk_tree_view_column_set_sort_indicator( column, sortable );
}

bool wxDataViewColumn::IsSortable() const
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
    return gtk_tree_view_column_get_sort_indicator( column );
}

bool wxDataViewColumn::IsResizeable() const
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
    return gtk_tree_view_column_get_resizable( column );
}

bool wxDataViewColumn::IsHidden() const
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
    return !gtk_tree_view_column_get_visible( column );
}

void wxDataViewColumn::SetSortOrder( bool ascending )
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);

    if (ascending)
        gtk_tree_view_column_set_sort_order( column, GTK_SORT_ASCENDING );
    else
        gtk_tree_view_column_set_sort_order( column, GTK_SORT_DESCENDING );
}

bool wxDataViewColumn::IsSortOrderAscending() const
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);

    return (gtk_tree_view_column_get_sort_order( column ) != GTK_SORT_DESCENDING);
}

void wxDataViewColumn::SetMinWidth( int width )
{
    gtk_tree_view_column_set_min_width( GTK_TREE_VIEW_COLUMN(m_column), width );
}

int wxDataViewColumn::GetMinWidth() const
{
    return gtk_tree_view_column_get_min_width( GTK_TREE_VIEW_COLUMN(m_column) );
}

int wxDataViewColumn::GetWidth() const
{
    return gtk_tree_view_column_get_width( GTK_TREE_VIEW_COLUMN(m_column) );
}

void wxDataViewColumn::SetWidth( int width )
{
    gtk_tree_view_column_set_fixed_width( GTK_TREE_VIEW_COLUMN(m_column), width );
}


//-----------------------------------------------------------------------------
// wxDataViewCtrl signal callbacks
//-----------------------------------------------------------------------------

static void
wxdataview_selection_changed_callback( GtkTreeSelection* selection, wxDataViewCtrl *dv )
{
    if (!GTK_WIDGET_REALIZED(dv->m_widget))
        return;

    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_ROW_SELECTED, dv->GetId() );
    event.SetRow( dv->GetSelection() );
    event.SetModel( dv->GetModel() );
    dv->GetEventHandler()->ProcessEvent( event );
}

static void
wxdataview_row_activated_callback( GtkTreeView* treeview, GtkTreePath *path,
                                   GtkTreeViewColumn *column, wxDataViewCtrl *dv )
{
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_ROW_ACTIVATED, dv->GetId() );
    unsigned int row = (unsigned int)gtk_tree_path_get_indices (path)[0];
    event.SetRow( row );
    event.SetModel( dv->GetModel() );
    dv->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// InsertChild for wxDataViewCtrl
//-----------------------------------------------------------------------------

static void wxInsertChildInDataViewCtrl( wxWindowGTK* parent, wxWindowGTK* child )
{
    wxDataViewCtrl * dvc = (wxDataViewCtrl*) parent;
    GtkWidget *treeview = dvc->GtkGetTreeView();

    // Insert widget in GtkTreeView
    if (GTK_WIDGET_REALIZED(treeview))
        gtk_widget_set_parent_window( child->m_widget, 
          gtk_tree_view_get_bin_window( GTK_TREE_VIEW(treeview) ) );
    gtk_widget_set_parent( child->m_widget, treeview );
}

static
void gtk_dataviewctrl_size_callback( GtkWidget *WXUNUSED(widget),
                                     GtkAllocation *alloc,
                                     wxDataViewCtrl *win )
{
    
    wxWindowList::Node *node = win->GetChildren().GetFirst();
    while (node)
    {
        wxWindow *child = node->GetData();
        
        GtkRequisition req;
        gtk_widget_size_request( child->m_widget, &req );
        
        GtkAllocation alloc;
        alloc.x = child->m_x;
        alloc.y = child->m_y;
        alloc.width = child->m_width;
        alloc.height = child->m_height;
        gtk_widget_size_allocate( child->m_widget, &alloc );
        
        node = node->GetNext();
    }
}



IMPLEMENT_DYNAMIC_CLASS(wxDataViewCtrl, wxDataViewCtrlBase)

wxDataViewCtrl::~wxDataViewCtrl()
{
    if (m_notifier)
        GetModel()->RemoveNotifier( m_notifier );

    // remove the model from the GtkTreeView before it gets destroyed by the
    // wxDataViewCtrlBase's dtor
    gtk_tree_view_set_model( GTK_TREE_VIEW(m_treeview), NULL );
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

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, validator ))
    {
        wxFAIL_MSG( wxT("wxDataViewCtrl creation failed") );
        return false;
    }

    m_insertCallback = wxInsertChildInDataViewCtrl;

    m_widget = gtk_scrolled_window_new (NULL, NULL);

    GtkScrolledWindowSetBorder(m_widget, style);

    m_treeview = gtk_tree_view_new();
    gtk_container_add (GTK_CONTAINER (m_widget), m_treeview);
    
    g_signal_connect (m_treeview, "size_allocate",
                     G_CALLBACK (gtk_dataviewctrl_size_callback), this);

#ifdef __WXGTK26__
    if (!gtk_check_version(2,6,0))
        gtk_tree_view_set_fixed_height_mode( GTK_TREE_VIEW(m_treeview), TRUE );
#endif

    if (style & wxDV_MULTIPLE)
    {
        GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
        gtk_tree_selection_set_mode( selection, GTK_SELECTION_MULTIPLE );
    }

    gtk_tree_view_set_headers_visible( GTK_TREE_VIEW(m_treeview), (style & wxDV_NO_HEADER) == 0 );

#ifdef __WXGTK210__
    if (!gtk_check_version(2,10,0))
    {
        GtkTreeViewGridLines grid = GTK_TREE_VIEW_GRID_LINES_NONE;

        if ((style & wxDV_HORIZ_RULES) != 0 &&
            (style & wxDV_VERT_RULES) != 0)
            grid = GTK_TREE_VIEW_GRID_LINES_BOTH;
        else if (style & wxDV_VERT_RULES)
            grid = GTK_TREE_VIEW_GRID_LINES_VERTICAL;
        else if (style & wxDV_HORIZ_RULES)
            grid = GTK_TREE_VIEW_GRID_LINES_HORIZONTAL;

        gtk_tree_view_set_grid_lines( GTK_TREE_VIEW(m_treeview), grid );
    }
    else
#endif
        gtk_tree_view_set_rules_hint( GTK_TREE_VIEW(m_treeview), (style & wxDV_HORIZ_RULES) != 0 );

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_widget),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_show (m_treeview);

    m_parent->DoAddChild( this );

    PostCreation(size);

    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
    g_signal_connect_after (selection, "changed",
                            G_CALLBACK (wxdataview_selection_changed_callback), this);
    g_signal_connect_after (m_treeview, "row_activated",
                            G_CALLBACK (wxdataview_row_activated_callback), this);

    return true;
}

void wxDataViewCtrl::OnInternalIdle()
{
    wxWindow::OnInternalIdle();

    unsigned int cols = GetColumnCount();
    unsigned int i;
    for (i = 0; i < cols; i++)
    {
        wxDataViewColumn *col = GetColumn( i );
        col->OnInternalIdle();
    }
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
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );

    if (row < 0)
    {
        gtk_tree_selection_unselect_all( selection );
    }
    else
    {
        GtkTreePath *path = gtk_tree_path_new ();
        gtk_tree_path_append_index( path, row );

        gtk_tree_selection_select_path( selection, path );

        gtk_tree_path_free( path );
    }
}

void wxDataViewCtrl::Unselect( unsigned int row )
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );

    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_path_append_index( path, row );

    gtk_tree_selection_unselect_path( selection, path );

    gtk_tree_path_free( path );
}

void wxDataViewCtrl::SetSelectionRange( unsigned int from, unsigned int to )
{
}

void wxDataViewCtrl::SetSelections( const wxArrayInt& aSelections)
{
}

bool wxDataViewCtrl::IsSelected( unsigned int row ) const
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );

    GtkTreePath *path = gtk_tree_path_new ();
    gtk_tree_path_append_index( path, row );

    gboolean ret =  gtk_tree_selection_path_is_selected( selection, path );

    gtk_tree_path_free( path );

    return ret;
}

int wxDataViewCtrl::GetSelection() const
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
    if (HasFlag(wxDV_MULTIPLE))
    {
        GtkTreeModel *model;
        GList *list = gtk_tree_selection_get_selected_rows( selection, &model );

        // do something
        if (list)
        {
            // list = g_list_nth( list, 0 );  should be a noop
            GtkTreePath *path = (GtkTreePath*) list->data;

            unsigned int row = (unsigned int)gtk_tree_path_get_indices (path)[0];

            // delete list
            g_list_foreach( list, (GFunc) gtk_tree_path_free, NULL );
            g_list_free( list );

            return (int) row;
        }
    }
    else
    {

        GtkTreeModel *model;
        GtkTreeIter iter;
        gboolean has_selection = gtk_tree_selection_get_selected( selection, &model, &iter );
        if (has_selection)
        {
            unsigned int row = (wxUIntPtr) iter.user_data;
            return (int) row;
        }
    }

    return -1;
}

int wxDataViewCtrl::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Clear();

    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
    if (HasFlag(wxDV_MULTIPLE))
    {
        GtkTreeModel *model;
        GList *list = gtk_tree_selection_get_selected_rows( selection, &model );

        int count = 0;
        while (list)
        {

            // list = g_list_nth( list, 0 );  should be a noop
            GtkTreePath *path = (GtkTreePath*) list->data;

            unsigned int row = (unsigned int)gtk_tree_path_get_indices (path)[0];

            aSelections.Add( (int) row );

            list = g_list_next( list );
        }

        // delete list
        g_list_foreach( list, (GFunc) gtk_tree_path_free, NULL );
        g_list_free( list );

        return count;
    }
    else
    {
        GtkTreeModel *model;
        GtkTreeIter iter;
        gboolean has_selection = gtk_tree_selection_get_selected( selection, &model, &iter );
        if (has_selection)
        {
            unsigned int row = (wxUIntPtr) iter.user_data;
            aSelections.Add( (int) row );
            return 1;
        }
    }

    return 0;
}

// static
wxVisualAttributes
wxDataViewCtrl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    return GetDefaultAttributesFromGTKWidget(gtk_tree_view_new);
}


#endif
    // !wxUSE_GENERICDATAVIEWCTRL

#endif
    // wxUSE_DATAVIEWCTRL
