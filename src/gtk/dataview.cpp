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
#include "wx/list.h"
#include "wx/listimpl.cpp"


#include "wx/gtk/private.h"
#include "wx/gtk/win_gtk.h"

#include <gobject/gvaluecollector.h>
#include <gtk/gtktreemodel.h>
#include <gtk/gtktreesortable.h>
#include <gtk/gtktreednd.h>

#include <gdk/gdkkeysyms.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

wxDataViewModel *g_model = NULL;

class wxGtkTreeModelNode;

extern "C" {
typedef struct _GtkWxTreeModel       GtkWxTreeModel;
}

//-----------------------------------------------------------------------------
// wxDataViewCtrlInternal
//-----------------------------------------------------------------------------

WX_DECLARE_LIST(wxDataViewItem, ItemList);
WX_DEFINE_LIST(ItemList);

class wxDataViewCtrlInternal
{
public:
    wxDataViewCtrlInternal( wxDataViewCtrl *owner, wxDataViewModel *wx_model, GtkWxTreeModel *owner );
    ~wxDataViewCtrlInternal();
    
    gboolean get_iter( GtkTreeIter *iter, GtkTreePath *path );
    GtkTreePath *get_path( GtkTreeIter *iter);
    GtkTreePath *get_path_safe( GtkTreeIter *iter);
    gboolean iter_next( GtkTreeIter *iter );
    gboolean iter_children( GtkTreeIter *iter, GtkTreeIter *parent);
    gboolean iter_has_child( GtkTreeIter *iter );
    gint iter_n_children( GtkTreeIter *iter );
    gboolean iter_nth_child( GtkTreeIter *iter, GtkTreeIter *parent, gint n );
    gboolean iter_parent( GtkTreeIter *iter, GtkTreeIter *child );
    
    wxDataViewModel* GetDataViewModel() { return m_wx_model; }
    wxDataViewCtrl* GetOwner()          { return m_owner; }
    GtkWxTreeModel* GetGtkModel()       { return m_gtk_model; }

    bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item );
    bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item );
    bool ItemChanged( const wxDataViewItem &item );
    bool ValueChanged( const wxDataViewItem &item, unsigned int col );
    bool Cleared();
    void Resort();
    
protected:
    void InitTree();
    wxGtkTreeModelNode *FindNode( const wxDataViewItem &item );
    wxGtkTreeModelNode *FindNode( GtkTreeIter *iter );
    wxGtkTreeModelNode *FindParentNode( const wxDataViewItem &item );
    wxGtkTreeModelNode *FindParentNode( GtkTreeIter *iter );
    void BuildBranch( wxGtkTreeModelNode *branch );
    
private:
    wxGtkTreeModelNode   *m_root;
    wxDataViewModel      *m_wx_model;
    GtkWxTreeModel       *m_gtk_model;
    wxDataViewCtrl       *m_owner;
};


//-----------------------------------------------------------------------------
// wxGtkTreeModelNode
//-----------------------------------------------------------------------------

int LINKAGEMODE wxGtkTreeModelNodeCmp( void *id1, void *id2 );

WX_DEFINE_ARRAY_PTR( wxGtkTreeModelNode*, wxGtkTreeModelNodes );
WX_DEFINE_SORTED_ARRAY( void* , wxGtkTreeModelChildren );

class wxGtkTreeModelNode
{
public:
    wxGtkTreeModelNode( wxGtkTreeModelNode* parent, const wxDataViewItem &item,
      wxDataViewCtrlInternal *internal )
    { 
        m_parent = parent; 
        m_item = item;
        m_internal = internal;
        g_model = internal->GetDataViewModel();
        m_children = new wxGtkTreeModelChildren( wxGtkTreeModelNodeCmp );
    }
    
    ~wxGtkTreeModelNode()
    { 
        g_model = m_internal->GetDataViewModel();
        size_t count = m_children->GetCount();
        size_t i;
        for (i = 0; i < count; i++)
        {
            wxGtkTreeModelNode *child = m_nodes.Item( i );
            delete child;
        }
        delete m_children;
    }

    unsigned int AddNode( wxGtkTreeModelNode* child ) 
        { 
            g_model = m_internal->GetDataViewModel();
            m_nodes.Add( child );
            return m_children->Add( child->GetItem().GetID() );
        }
        
    unsigned int AddLeave( void* id )
        {
            g_model = m_internal->GetDataViewModel();
            return m_children->Add( id );
        }
        
    void DeleteChild( void* id )
        {
            g_model = m_internal->GetDataViewModel();
            size_t pos;
            size_t count = m_children->GetCount();
            for (pos = 0; pos < count; pos++)
            {
                if (m_children->Item( pos ) == id)
                {
                    m_children->RemoveAt( pos );
                    break;
                }
            }
            count = m_nodes.GetCount();
            for (pos = 0; pos < count; pos++)
            {   
                wxGtkTreeModelNode *node = m_nodes.Item( pos );
                if (node->GetItem().GetID() == id)
                {
                    m_nodes.RemoveAt( pos );
                    delete node;
                    break;
                }
            }
            
        }
        
    wxGtkTreeModelNode* GetParent() 
        { return m_parent; }
    wxGtkTreeModelNodes &GetNodes() 
        { return m_nodes; }
    wxGtkTreeModelChildren &GetChildren() 
        { return *m_children; }
        
    unsigned int GetChildCount() { return m_children->GetCount(); }
    unsigned int GetNodesCount() { return m_nodes.GetCount(); }

    wxDataViewItem &GetItem() { return m_item; }
    wxDataViewCtrlInternal *GetInternal() { return m_internal; }
    
    void Resort();
    
private:
    wxGtkTreeModelNode         *m_parent;
    wxGtkTreeModelNodes         m_nodes;
    wxGtkTreeModelChildren     *m_children;
    wxDataViewItem              m_item; 
    wxDataViewCtrlInternal     *m_internal;
};


int LINKAGEMODE wxGtkTreeModelNodeCmp( void* id1, void* id2 )
{
    return g_model->Compare( id1, id2 );
}

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern bool   g_blockEventsOnDrag;

//-----------------------------------------------------------------------------
// define new GTK+ class wxGtkTreeModel
//-----------------------------------------------------------------------------

extern "C" {

#define GTK_TYPE_WX_TREE_MODEL               (gtk_wx_tree_model_get_type ())
#define GTK_WX_TREE_MODEL(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_WX_TREE_MODEL, GtkWxTreeModel))
#define GTK_WX_TREE_MODEL_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_WX_TREE_MODEL, GtkWxTreeModelClass))
#define GTK_IS_WX_TREE_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_WX_TREE_MODEL))
#define GTK_IS_WX_TREE_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_WX_TREE_MODEL))
#define GTK_WX_TREE_MODEL_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_WX_TREE_MODEL, GtkWxTreeModelClass))

GType         gtk_wx_tree_model_get_type         (void);

typedef struct _GtkWxTreeModelClass  GtkWxTreeModelClass;

struct _GtkWxTreeModel
{
  GObject parent;

  /*< private >*/
  gint stamp;
  wxDataViewCtrlInternal *internal;
};

struct _GtkWxTreeModelClass
{
  GObjectClass list_parent_class;
};

static GtkWxTreeModel *wxgtk_tree_model_new          (void);
static void         wxgtk_tree_model_init            (GtkWxTreeModel       *tree_model);
static void         wxgtk_tree_model_class_init      (GtkWxTreeModelClass  *klass);
static void         wxgtk_tree_model_tree_model_init (GtkTreeModelIface    *iface);
static void         wxgtk_tree_model_sortable_init   (GtkTreeSortableIface *iface);
static void         wxgtk_tree_model_finalize        (GObject           *object);
static GtkTreeModelFlags wxgtk_tree_model_get_flags  (GtkTreeModel      *tree_model);
static gint         wxgtk_tree_model_get_n_columns   (GtkTreeModel      *tree_model);
static GType        wxgtk_tree_model_get_column_type (GtkTreeModel      *tree_model,
                                                      gint               index);
static gboolean     wxgtk_tree_model_get_iter        (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreePath       *path);
static GtkTreePath *wxgtk_tree_model_get_path        (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);
static GtkTreePath *wxgtk_tree_model_get_path_safe   (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);
static void         wxgtk_tree_model_get_value       (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      gint               column,
                                                      GValue            *value);
static gboolean     wxgtk_tree_model_iter_next       (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);
static gboolean     wxgtk_tree_model_iter_children   (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreeIter       *parent);
static gboolean     wxgtk_tree_model_iter_has_child  (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);
static gint         wxgtk_tree_model_iter_n_children (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);
static gboolean     wxgtk_tree_model_iter_nth_child  (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreeIter       *parent,
                                                      gint               n);
static gboolean     wxgtk_tree_model_iter_parent     (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreeIter       *child);

/* sortable */
static gboolean wxgtk_tree_model_get_sort_column_id    (GtkTreeSortable       *sortable,
						      gint                     *sort_column_id,
						      GtkSortType              *order);
static void     wxgtk_tree_model_set_sort_column_id    (GtkTreeSortable       *sortable,
						      gint                      sort_column_id,
						      GtkSortType               order);
static void     wxgtk_tree_model_set_sort_func         (GtkTreeSortable       *sortable,
						      gint                      sort_column_id,
						      GtkTreeIterCompareFunc    func,
						      gpointer                  data,
						      GtkDestroyNotify          destroy);
static void     wxgtk_tree_model_set_default_sort_func (GtkTreeSortable       *sortable,
						      GtkTreeIterCompareFunc    func,
						      gpointer                  data,
						      GtkDestroyNotify          destroy);
static gboolean wxgtk_tree_model_has_default_sort_func (GtkTreeSortable       *sortable);



static GObjectClass *list_parent_class = NULL;

GType
gtk_wx_tree_model_get_type (void)
{
    static GType tree_model_type = 0;

    if (!tree_model_type)
    {
        const GTypeInfo tree_model_info =
        {
            sizeof (GtkWxTreeModelClass),
            NULL,   /* base_init */
            NULL,   /* base_finalize */
            (GClassInitFunc) wxgtk_tree_model_class_init,
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof (GtkWxTreeModel),
            0,
            (GInstanceInitFunc) wxgtk_tree_model_init,
        };

        static const GInterfaceInfo tree_model_iface_info =
        {
            (GInterfaceInitFunc) wxgtk_tree_model_tree_model_init,
            NULL,
            NULL
        };

        static const GInterfaceInfo sortable_iface_info =
        {
            (GInterfaceInitFunc) wxgtk_tree_model_sortable_init,
            NULL,
            NULL
        };

        tree_model_type = g_type_register_static (G_TYPE_OBJECT, "GtkWxTreeModel",
                                                &tree_model_info, (GTypeFlags)0 );

        g_type_add_interface_static (tree_model_type,
                                     GTK_TYPE_TREE_MODEL,
                                     &tree_model_iface_info);
        g_type_add_interface_static (tree_model_type,
                                     GTK_TYPE_TREE_SORTABLE,
                                     &sortable_iface_info);
    }

    return tree_model_type;
}

static GtkWxTreeModel *
wxgtk_tree_model_new(void)
{
    GtkWxTreeModel *retval = (GtkWxTreeModel *) g_object_new (GTK_TYPE_WX_TREE_MODEL, NULL);
    return retval;
}

static void
wxgtk_tree_model_class_init (GtkWxTreeModelClass *klass)
{
    list_parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
    GObjectClass *object_class = (GObjectClass*) klass;
    object_class->finalize = wxgtk_tree_model_finalize;
}

static void
wxgtk_tree_model_tree_model_init (GtkTreeModelIface *iface)
{
    iface->get_flags = wxgtk_tree_model_get_flags;
    iface->get_n_columns = wxgtk_tree_model_get_n_columns;
    iface->get_column_type = wxgtk_tree_model_get_column_type;
    iface->get_iter = wxgtk_tree_model_get_iter;
    iface->get_path = wxgtk_tree_model_get_path;
    iface->get_value = wxgtk_tree_model_get_value;
    iface->iter_next = wxgtk_tree_model_iter_next;
    iface->iter_children = wxgtk_tree_model_iter_children;
    iface->iter_has_child = wxgtk_tree_model_iter_has_child;
    iface->iter_n_children = wxgtk_tree_model_iter_n_children;
    iface->iter_nth_child = wxgtk_tree_model_iter_nth_child;
    iface->iter_parent = wxgtk_tree_model_iter_parent;
}

static void
wxgtk_tree_model_sortable_init (GtkTreeSortableIface *iface)
{
    iface->get_sort_column_id = wxgtk_tree_model_get_sort_column_id;
    iface->set_sort_column_id = wxgtk_tree_model_set_sort_column_id;
    iface->set_sort_func = wxgtk_tree_model_set_sort_func;
    iface->set_default_sort_func = wxgtk_tree_model_set_default_sort_func;
    iface->has_default_sort_func = wxgtk_tree_model_has_default_sort_func;
}

static void
wxgtk_tree_model_init (GtkWxTreeModel *tree_model)
{
    tree_model->internal = NULL;
    tree_model->stamp = g_random_int();
}

static void
wxgtk_tree_model_finalize (GObject *object)
{
    /* must chain up */
    (* list_parent_class->finalize) (object);
}

} // extern "C"

//-----------------------------------------------------------------------------
// implement callbacks from wxGtkTreeModel class by letting
// them call the methods of wxWidgets' wxDataViewModel
//-----------------------------------------------------------------------------

static GtkTreeModelFlags
wxgtk_tree_model_get_flags (GtkTreeModel *tree_model)
{
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (tree_model), (GtkTreeModelFlags)0 );

    return GTK_TREE_MODEL_ITERS_PERSIST;
}

static gint
wxgtk_tree_model_get_n_columns (GtkTreeModel *tree_model)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), 0);

    return wxtree_model->internal->GetDataViewModel()->GetColumnCount();
}

static GType
wxgtk_tree_model_get_column_type (GtkTreeModel *tree_model,
                                  gint          index)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), G_TYPE_INVALID);

    GType gtype = G_TYPE_INVALID;

    wxString wxtype = wxtree_model->internal->GetDataViewModel()->GetColumnType( (unsigned int) index );

    if (wxtype == wxT("string"))
        gtype = G_TYPE_STRING;
    else
    {
        wxFAIL_MSG( _T("non-string columns not supported yet") );
    }

    return gtype;
}

static gboolean
wxgtk_tree_model_get_iter (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter,
                           GtkTreePath  *path)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);
    g_return_val_if_fail (gtk_tree_path_get_depth (path) > 0, FALSE);

    return wxtree_model->internal->get_iter( iter, path );
}

static GtkTreePath *
wxgtk_tree_model_get_path (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), NULL);
    g_return_val_if_fail (iter->stamp == GTK_WX_TREE_MODEL (wxtree_model)->stamp, NULL);

    return wxtree_model->internal->get_path( iter );
}

static GtkTreePath *
wxgtk_tree_model_get_path_safe (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), NULL);
    g_return_val_if_fail (iter->stamp == GTK_WX_TREE_MODEL (wxtree_model)->stamp, NULL);

    return wxtree_model->internal->get_path_safe( iter );
}

static void
wxgtk_tree_model_get_value (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter,
                            gint          column,
                            GValue       *value)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model) );

    wxDataViewModel *model = wxtree_model->internal->GetDataViewModel();
    wxString mtype = model->GetColumnType( (unsigned int) column );
    if (mtype == wxT("string"))
    {
        wxVariant variant;
        g_value_init( value, G_TYPE_STRING );
        wxDataViewItem item( (void*) iter->user_data );
        model->GetValue( variant, item, (unsigned int) column );

        g_value_set_string( value, variant.GetString().utf8_str() );
    }
    else
    {
        wxFAIL_MSG( _T("non-string columns not supported yet") );
    }
}

static gboolean
wxgtk_tree_model_iter_next (GtkTreeModel  *tree_model,
                            GtkTreeIter   *iter)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);
    g_return_val_if_fail (wxtree_model->stamp == iter->stamp, FALSE);

    return wxtree_model->internal->iter_next( iter );
}

static gboolean
wxgtk_tree_model_iter_children (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter,
                                GtkTreeIter  *parent)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);
    g_return_val_if_fail (wxtree_model->stamp == parent->stamp, FALSE);

    return wxtree_model->internal->iter_children( iter, parent );
}

static gboolean
wxgtk_tree_model_iter_has_child (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);
    g_return_val_if_fail (wxtree_model->stamp == iter->stamp, FALSE);

    return wxtree_model->internal->iter_has_child( iter );
}

static gint
wxgtk_tree_model_iter_n_children (GtkTreeModel *tree_model,
                                  GtkTreeIter  *iter)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);
    g_return_val_if_fail (wxtree_model->stamp == iter->stamp, 0);

    return wxtree_model->internal->iter_n_children( iter );
}

static gboolean
wxgtk_tree_model_iter_nth_child (GtkTreeModel *tree_model,
                                 GtkTreeIter  *iter,
                                 GtkTreeIter  *parent,
                                 gint          n)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);

    return wxtree_model->internal->iter_nth_child( iter, parent, n );
}

static gboolean
wxgtk_tree_model_iter_parent (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter,
                              GtkTreeIter  *child)
{
    GtkWxTreeModel *wxtree_model = (GtkWxTreeModel *) tree_model;
    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (wxtree_model), FALSE);
    g_return_val_if_fail (wxtree_model->stamp == child->stamp, FALSE);
    
    return wxtree_model->internal->iter_parent( iter, child );
}

/* sortable */
gboolean wxgtk_tree_model_get_sort_column_id    (GtkTreeSortable        *sortable,
						      gint                     *sort_column_id,
						      GtkSortType              *order)
{
    GtkWxTreeModel *tree_model = (GtkWxTreeModel *) sortable;

    g_return_val_if_fail (GTK_IS_WX_TREE_MODEL (sortable), FALSE);

    if (sort_column_id)
        *sort_column_id = tree_model->internal->GetDataViewModel()->GetSortingColumn();
        
    if (order)
    {
        bool ascending = tree_model->internal->GetDataViewModel()->GetSortOrderAscending();
        if (ascending)
            *order = GTK_SORT_ASCENDING;
        else
            *order = GTK_SORT_DESCENDING;
    }
        
    return TRUE;
}

void     wxgtk_tree_model_set_sort_column_id    (GtkTreeSortable        *sortable,
						      gint                      sort_column_id,
						      GtkSortType               order)
{
    GtkWxTreeModel *tree_model = (GtkWxTreeModel *) sortable;
    g_return_if_fail (GTK_IS_WX_TREE_MODEL (sortable) );

    bool ascending = TRUE;
    if (order != GTK_SORT_ASCENDING)
        ascending = FALSE;

    if ((sort_column_id == (gint) tree_model->internal->GetDataViewModel()->GetSortingColumn()) &&
        (ascending == tree_model->internal->GetDataViewModel()->GetSortOrderAscending()))
        return;
    
    tree_model->internal->GetDataViewModel()->SetSortingColumn( sort_column_id );
    
    tree_model->internal->GetDataViewModel()->SetSortOrderAscending( ascending );
    
    gtk_tree_sortable_sort_column_changed (sortable);
    
    tree_model->internal->GetDataViewModel()->Resort();
}

void     wxgtk_tree_model_set_sort_func         (GtkTreeSortable        *sortable,
						      gint                      sort_column_id,
						      GtkTreeIterCompareFunc    func,
						      gpointer                  data,
						      GtkDestroyNotify          destroy)
{
    g_return_if_fail (GTK_IS_WX_TREE_MODEL (sortable) );
    g_return_if_fail (func != NULL);
}

void     wxgtk_tree_model_set_default_sort_func (GtkTreeSortable        *sortable,
						      GtkTreeIterCompareFunc    func,
						      gpointer                  data,
						      GtkDestroyNotify          destroy)
{
    g_return_if_fail (GTK_IS_WX_TREE_MODEL (sortable) );
    g_return_if_fail (func != NULL);

    wxPrintf( "wxgtk_tree_model_set_default_sort_func\n" );
}

gboolean wxgtk_tree_model_has_default_sort_func (GtkTreeSortable        *sortable)
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

    GtkTreePath *treepath = gtk_tree_path_new_from_string( path );
    GtkTreeIter iter;
    cell->GetOwner()->GetOwner()->GtkGetInternal()->get_iter( &iter, treepath );
    wxDataViewItem item( (void*) iter.user_data );
    gtk_tree_path_free( treepath );

    cell->StartEditing( item, renderrect );

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

    wxDataViewModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *treepath = gtk_tree_path_new_from_string( path );
    // TODO
    wxDataViewItem item;
    gtk_tree_path_free( treepath );

    unsigned int model_col = cell->GetOwner()->GetModelColumn();

    if (!event)
    {
        bool ret = false;

        // activated by <ENTER>
        if (cell->Activate( renderrect, model, item, model_col ))
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
            if (cell->LeftClick( pt, renderrect, model, item, model_col ))
                ret = true;
            // TODO: query system double-click time
            if (button_event->time - wxrenderer->last_click < 400)
                if (cell->Activate( renderrect, model, item, model_col ))
                    ret = true;
        }
        if (button_event->button == 3)
        {
            if (cell->RightClick( pt, renderrect, model, item, model_col ))
                ret = true;
        }

        wxrenderer->last_click = button_event->time;

        return ret;
    }

    return false;
}

// ---------------------------------------------------------
// wxGtkDataViewModelNotifier
// ---------------------------------------------------------

class wxGtkDataViewModelNotifier: public wxDataViewModelNotifier
{
public:
    wxGtkDataViewModelNotifier( GtkWxTreeModel  *wxgtk_model,
                                wxDataViewModel *wx_model,
                                wxDataViewCtrl  *ctrl );
    ~wxGtkDataViewModelNotifier();

    virtual bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item );
    virtual bool ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item );
    virtual bool ItemChanged( const wxDataViewItem &item );
    virtual bool ValueChanged( const wxDataViewItem &item, unsigned int col );
    virtual bool Cleared();
    virtual void Resort();

    GtkWxTreeModel      *m_wxgtk_model;
    wxDataViewModel     *m_wx_model;
    wxDataViewCtrl      *m_owner;
};

// ---------------------------------------------------------
// wxGtkDataViewListModelNotifier
// ---------------------------------------------------------

wxGtkDataViewModelNotifier::wxGtkDataViewModelNotifier(
    GtkWxTreeModel* wxgtk_model, wxDataViewModel *wx_model,
    wxDataViewCtrl *ctrl )
{
    m_wxgtk_model = wxgtk_model;
    m_wx_model = wx_model;
    m_owner = ctrl;
}

wxGtkDataViewModelNotifier::~wxGtkDataViewModelNotifier()
{
    m_wx_model = NULL;
    m_wxgtk_model = NULL;
}

bool wxGtkDataViewModelNotifier::ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    m_owner->GtkGetInternal()->ItemAdded( parent, item );

    GtkTreeIter iter;
    iter.stamp = m_wxgtk_model->stamp;
    iter.user_data = (gpointer) item.GetID();

    GtkTreePath *path = wxgtk_tree_model_get_path( 
        GTK_TREE_MODEL(m_wxgtk_model), &iter );
    gtk_tree_model_row_inserted( 
        GTK_TREE_MODEL(m_wxgtk_model), path, &iter);
    gtk_tree_path_free (path);

    return true;
}

bool wxGtkDataViewModelNotifier::ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    GtkTreeIter iter;
    iter.stamp = m_wxgtk_model->stamp;
    iter.user_data = (gpointer) item.GetID();

    GtkTreePath *path = wxgtk_tree_model_get_path_safe( 
        GTK_TREE_MODEL(m_wxgtk_model), &iter );
    gtk_tree_model_row_deleted(
        GTK_TREE_MODEL(m_wxgtk_model), path );
    gtk_tree_path_free (path);

    m_owner->GtkGetInternal()->ItemDeleted( parent, item );
    
    return true;
}

void wxGtkDataViewModelNotifier::Resort()
{
    m_owner->GtkGetInternal()->Resort();
}

bool wxGtkDataViewModelNotifier::ItemChanged( const wxDataViewItem &item )
{
    GtkTreeIter iter;
    iter.stamp = m_wxgtk_model->stamp;
    iter.user_data = (gpointer) item.GetID();

    GtkTreePath *path = wxgtk_tree_model_get_path( 
        GTK_TREE_MODEL(m_wxgtk_model), &iter );
    gtk_tree_model_row_changed(
        GTK_TREE_MODEL(m_wxgtk_model), path, &iter );
    gtk_tree_path_free (path);

    m_owner->GtkGetInternal()->ItemChanged( item );
    
    return true;
}

bool wxGtkDataViewModelNotifier::ValueChanged( const wxDataViewItem &item, unsigned int model_col )
{
    // This adds GTK+'s missing MVC logic for ValueChanged
    unsigned int index;
    for (index = 0; index < m_owner->GetColumnCount(); index++)
    {
        wxDataViewColumn *column = m_owner->GetColumn( index );
        if (column->GetModelColumn() == model_col)
        {
            GtkTreeView *widget = GTK_TREE_VIEW(m_owner->m_treeview);
            GtkTreeViewColumn *gcolumn = GTK_TREE_VIEW_COLUMN(column->GetGtkHandle());

            // Get cell area
            GtkTreeIter iter;
            iter.stamp = m_wxgtk_model->stamp;
            iter.user_data = (gpointer) item.GetID();
            GtkTreePath *path = wxgtk_tree_model_get_path( 
                GTK_TREE_MODEL(m_wxgtk_model), &iter );
            GdkRectangle cell_area;
            gtk_tree_view_get_cell_area( widget, path, gcolumn, &cell_area );
            gtk_tree_path_free( path );

            GtkAdjustment* hadjust = gtk_tree_view_get_hadjustment( widget );
            double d = gtk_adjustment_get_value( hadjust );
            int xdiff = (int) d;

            int ydiff = gcolumn->button->allocation.height;
            // Redraw
            gtk_widget_queue_draw_area( GTK_WIDGET(widget),
                cell_area.x - xdiff, ydiff + cell_area.y, cell_area.width, cell_area.height );
                
            m_owner->GtkGetInternal()->ValueChanged( item, model_col );
            
            return true;
        }
    }

    return false;
}

bool wxGtkDataViewModelNotifier::Cleared()
{
    // TODO: delete everything

    m_owner->GtkGetInternal()->Cleared();
    
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

    wxDataViewModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *path = gtk_tree_path_new_from_string( arg1 );
    GtkTreeIter iter;
    cell->GetOwner()->GetOwner()->GtkGetInternal()->get_iter( &iter, path );
    wxDataViewItem item( (void*) iter.user_data );;
    gtk_tree_path_free( path );

    unsigned int model_col = cell->GetOwner()->GetModelColumn();

    model->SetValue( value, item, model_col );
    model->ValueChanged( item, model_col );
}

IMPLEMENT_CLASS(wxDataViewTextRenderer, wxDataViewRenderer)

wxDataViewTextRenderer::wxDataViewTextRenderer( const wxString &varianttype, wxDataViewCellMode mode,
                                                int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_renderer = (GtkCellRenderer*) gtk_cell_renderer_text_new();

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
    m_renderer = (GtkCellRenderer*) gtk_cell_renderer_pixbuf_new();

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

    wxDataViewModel *model = cell->GetOwner()->GetOwner()->GetModel();

    GtkTreePath *gtk_path = gtk_tree_path_new_from_string( path );
    GtkTreeIter iter;
    cell->GetOwner()->GetOwner()->GtkGetInternal()->get_iter( &iter, gtk_path );
    wxDataViewItem item( (void*) iter.user_data );;
    gtk_tree_path_free( gtk_path );

    unsigned int model_col = cell->GetOwner()->GetModelColumn();

    model->SetValue( value, item, model_col );
    model->ValueChanged( item, model_col );
}

IMPLEMENT_CLASS(wxDataViewToggleRenderer, wxDataViewRenderer)

wxDataViewToggleRenderer::wxDataViewToggleRenderer( const wxString &varianttype,
                                                    wxDataViewCellMode mode, int align ) :
    wxDataViewRenderer( varianttype, mode, align )
{
    m_renderer = (GtkCellRenderer*) gtk_cell_renderer_toggle_new();

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

    m_renderer = (GtkCellRenderer*) renderer;

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
        m_renderer = (GtkCellRenderer*) gtk_cell_renderer_progress_new();

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
        wxDataViewModel *model, const wxDataViewItem &item, unsigned int col ) :
        wxPopupTransientWindow( parent, wxBORDER_SIMPLE )
    {
        m_model = model;
        m_item = item;
        m_col = col;
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

    wxCalendarCtrl   *m_cal;
    wxDataViewModel  *m_model;
    wxDataViewItem    m_item;
    unsigned int      m_col;

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
    m_model->SetValue( value, m_item, m_col );
    m_model->ValueChanged( m_item, m_col );
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

bool wxDataViewDateRenderer::Activate( wxRect cell, wxDataViewModel *model,
                                       const wxDataViewItem &item, unsigned int col )
{
    wxVariant variant;
    model->GetValue( variant, item, col );
    wxDateTime value = variant.GetDateTime();

    wxDataViewDateRendererPopupTransient *popup = new wxDataViewDateRendererPopupTransient(
        GetOwner()->GetOwner()->GetParent(), &value, model, item, col );
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
        return FALSE;

    if (gdk_event->button == 1)
    {
        wxDataViewCtrl *dv = column->GetOwner();
        wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK, dv->GetId() );
        event.SetDataViewColumn( column );
        event.SetModel( dv->GetModel() );
        if (dv->GetEventHandler()->ProcessEvent( event ))
            return TRUE;
    }

    return FALSE;
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
    g_return_if_fail (GTK_IS_WX_TREE_MODEL (model));
    GtkWxTreeModel *tree_model = (GtkWxTreeModel *) model;

    wxDataViewRenderer *cell = (wxDataViewRenderer*) data;

    wxDataViewItem item( (void*) iter->user_data );

    wxVariant value;
    tree_model->internal->GetDataViewModel()->GetValue( value, item, cell->GetOwner()->GetModelColumn() );

    if (value.GetType() != cell->GetVariantType())
        wxLogError( wxT("Wrong type, required: %s but: %s"),
                    value.GetType().c_str(),
                    cell->GetVariantType().c_str() );

    cell->SetValue( value );

#if 0
    wxListItemAttr attr;
    tree_model->model->GetAttr( attr, cell->GetOwner()->GetModelColumn(), model_row );

    if (attr.HasBackgroundColour())
    {
        wxColour colour = attr.GetBackgroundColour();
        const GdkColor * const gcol = colour.GetColor();

        GValue gvalue = { 0, };
        g_value_init( &gvalue, GDK_TYPE_COLOR );
        g_value_set_boxed( &gvalue, gcol );
        g_object_set_property( G_OBJECT(renderer), "cell-background_gdk", &gvalue );
        g_value_unset( &gvalue );
    }
    else
    {
        GValue gvalue = { 0, };
        g_value_init( &gvalue, G_TYPE_BOOLEAN );
        g_value_set_boolean( &gvalue, FALSE );
        g_object_set_property( G_OBJECT(renderer), "cell-background-set", &gvalue );
        g_value_unset( &gvalue );
    }
#endif

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
    
    if (sortable)
        gtk_tree_view_column_set_sort_column_id( column, GetModelColumn() );
    else
        gtk_tree_view_column_set_sort_column_id( column, -1 );
}

bool wxDataViewColumn::IsSortable() const
{
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(m_column);
    return (gtk_tree_view_column_get_sort_column_id( column ) != -1);
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
// wxGtkTreeModelNode
//-----------------------------------------------------------------------------

void wxGtkTreeModelNode::Resort()
{
    g_model = m_internal->GetDataViewModel();
    
    size_t child_count = GetChildCount();
    if (child_count == 0)
        return;

    size_t node_count = GetNodesCount();

    if (child_count == 1)
    {
        if (node_count == 1)
        {
            wxGtkTreeModelNode *node = m_nodes.Item( 0 );
            node->Resort();
        }
        return;
    }

    wxGtkTreeModelChildren *new_array = new wxGtkTreeModelChildren( wxGtkTreeModelNodeCmp );

    size_t pos;
    for (pos = 0; pos < child_count; pos++)
        new_array->Add( m_children->Item( pos ) );

    gint *new_order = new gint[child_count];
    
    for (pos = 0; pos < child_count; pos++)
    {
        void *id = new_array->Item( pos );
        size_t old_pos;
        for (old_pos = 0; old_pos < child_count; old_pos++)
        {
            if (id == m_children->Item(old_pos))
            {
                new_order[pos] = old_pos;
                break;
            }
        }
    }
        
//    for (pos = 0; pos < count; pos++)
//        m_children->Clear();
    delete m_children;
    
    m_children = new_array;
    
    GtkTreeModel *gtk_tree_model = GTK_TREE_MODEL( m_internal->GetGtkModel() );

    GtkTreePath *path = gtk_tree_path_new ();
    wxGtkTreeModelNode *parent = GetParent();
    void *id = GetItem().GetID();

    while (parent)
    {
        int pos = parent->GetChildren().Index( id );
        gtk_tree_path_prepend_index( path, pos );
        id = parent->GetItem().GetID();
        parent = parent->GetParent();
    }
    
    GtkTreeIter iter;
    iter.user_data = id;
    iter.stamp = m_internal->GetGtkModel()->stamp;
    gtk_tree_model_rows_reordered( gtk_tree_model, path, &iter, new_order );
    
    gtk_tree_path_free (path);
    
    delete [] new_order;
    
    for (pos = 0; pos < node_count; pos++)
    {
        wxGtkTreeModelNode *node = m_nodes.Item( pos );
        node->Resort();
    }
}

//-----------------------------------------------------------------------------
// wxDataViewCtrlInternal
//-----------------------------------------------------------------------------

wxDataViewCtrlInternal::wxDataViewCtrlInternal( wxDataViewCtrl *owner, 
    wxDataViewModel *wx_model, GtkWxTreeModel *gtk_model )
{ 
    m_owner = owner;
    m_wx_model = wx_model; 
    m_gtk_model = gtk_model; 
    m_root = NULL; 
    InitTree();
}
    
wxDataViewCtrlInternal::~wxDataViewCtrlInternal()
{
    g_object_unref( m_gtk_model );
}
    
void wxDataViewCtrlInternal::InitTree()
{
    wxDataViewItem item;
    m_root = new wxGtkTreeModelNode( NULL, item, this );

    BuildBranch( m_root );
}

void wxDataViewCtrlInternal::BuildBranch( wxGtkTreeModelNode *node )
{
    if (node->GetChildCount() == 0)
    {
        wxDataViewItem child = m_wx_model->GetFirstChild( node->GetItem() );
        while (child.IsOk())
        {
            if (m_wx_model->IsContainer( child ))
                node->AddNode( new wxGtkTreeModelNode( node, child, this ) );
            else
                node->AddLeave( child.GetID() );
    
            // Don't send any events here
    
            child = m_wx_model->GetNextSibling( child );
        }
    }
}

void wxDataViewCtrlInternal::Resort()
{
    m_root->Resort();
}

bool wxDataViewCtrlInternal::ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    wxGtkTreeModelNode *parent_node = FindNode( parent );
    if (m_wx_model->IsContainer( item ))
        parent_node->AddNode( new wxGtkTreeModelNode( parent_node, item, this ) );
    else
        parent_node->AddLeave( item.GetID() );
        
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_ADDED, m_owner->GetId() );
    event.SetEventObject( m_owner );
    event.SetModel( m_owner->GetModel() );
    event.SetItem( item );
    m_owner->GetEventHandler()->ProcessEvent( event );

    return true;
}

bool wxDataViewCtrlInternal::ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    wxGtkTreeModelNode *parent_node = FindNode( parent );
    parent_node->DeleteChild( item.GetID() );
    
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_DELETED, m_owner->GetId() );
    event.SetEventObject( m_owner );
    event.SetModel( m_owner->GetModel() );
    event.SetItem( item );
    m_owner->GetEventHandler()->ProcessEvent( event );

    return true;
}

bool wxDataViewCtrlInternal::ItemChanged( const wxDataViewItem &item )
{
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_CHANGED, m_owner->GetId() );
    event.SetEventObject( m_owner );
    event.SetModel( m_owner->GetModel() );
    event.SetItem( item );
    m_owner->GetEventHandler()->ProcessEvent( event );
    
    return true;
}

bool wxDataViewCtrlInternal::ValueChanged( const wxDataViewItem &item, unsigned int col )
{
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED, m_owner->GetId() );
    event.SetEventObject( m_owner );
    event.SetModel( m_owner->GetModel() );
    event.SetColumn( col );
    event.SetItem( item );
    m_owner->GetEventHandler()->ProcessEvent( event );
    
    return true;
}

bool wxDataViewCtrlInternal::Cleared()
{
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED, m_owner->GetId() );
    event.SetEventObject( m_owner );
    event.SetModel( m_owner->GetModel() );
    m_owner->GetEventHandler()->ProcessEvent( event );
    
    return true;
}

gboolean wxDataViewCtrlInternal::get_iter( GtkTreeIter *iter, GtkTreePath *path )
{
    g_model = m_wx_model;
    
    int depth = gtk_tree_path_get_depth( path );

    wxGtkTreeModelNode *node = m_root;

    int i;
    for (i = 0; i < depth; i++)    
    {
        BuildBranch( node );
        
        gint pos = gtk_tree_path_get_indices (path)[i];
        if (pos < 0) return FALSE;
        if ((size_t)pos >= node->GetChildCount()) return FALSE;
        
        void* id = node->GetChildren().Item( (size_t) pos );
        
        if (i == depth-1)
        {
            iter->stamp = m_gtk_model->stamp;
            iter->user_data = id;
            return TRUE;
        }

        size_t count = node->GetNodes().GetCount();
        size_t pos2;
        for (pos2 = 0; pos2 < count; pos2++)
        {
            wxGtkTreeModelNode *child_node = node->GetNodes().Item( pos2 );
            if (child_node->GetItem().GetID() == id)
            {
                node = child_node;
                break;
            }
        }        
    }

    return FALSE;
}

GtkTreePath *wxDataViewCtrlInternal::get_path( GtkTreeIter *iter )
{
    g_model = m_wx_model;
    
    GtkTreePath *retval = gtk_tree_path_new ();
    void *id = iter->user_data;    
    
    wxGtkTreeModelNode *node = FindParentNode( iter );
    while (node)
    {
        int pos = node->GetChildren().Index( id );
        gtk_tree_path_prepend_index( retval, pos );
        
        id = node->GetItem().GetID();
        node = node->GetParent();
    }

    return retval;
}

GtkTreePath *wxDataViewCtrlInternal::get_path_safe( GtkTreeIter *iter )
{
    g_model = m_wx_model;
    
    GtkTreePath *retval = gtk_tree_path_new ();
    void *id = iter->user_data;    
    
    wxGtkTreeModelNode *node = FindParentNode( iter );
    while (node)
    {
        size_t pos;
        for (pos = 0; pos < node->GetChildren().GetCount(); pos++)
        {
            if (id == node->GetChildren().Item( pos ))
            {
                gtk_tree_path_prepend_index( retval, (int) pos );
                continue;
            }
        }

        id = node->GetItem().GetID();
        node = node->GetParent();
    }

    return retval;
}

gboolean wxDataViewCtrlInternal::iter_next( GtkTreeIter *iter )
{
    g_model = m_wx_model;
    
    wxGtkTreeModelNode *parent = FindParentNode( iter );
    if( parent == NULL )
        return FALSE;

    unsigned int pos = parent->GetChildren().Index( iter->user_data );
    
    if (pos == parent->GetChildCount()-1)
        return FALSE;
        
    iter->stamp = m_gtk_model->stamp;
    iter->user_data = parent->GetChildren().Item( pos+1 );

    return TRUE;
}

gboolean wxDataViewCtrlInternal::iter_children( GtkTreeIter *iter, GtkTreeIter *parent )
{
    g_model = m_wx_model;
    
    wxDataViewItem item( (void*) parent->user_data );
    
    if (!m_wx_model->IsContainer( item ))
        return FALSE;
        
    wxGtkTreeModelNode *parent_node = FindNode( parent );
    BuildBranch( parent_node );
    
    if (parent_node->GetChildCount() == 0)
        return FALSE;
        
    iter->stamp = m_gtk_model->stamp;
    iter->user_data = (gpointer) parent_node->GetChildren().Item( 0 );

    return TRUE;
}

gboolean wxDataViewCtrlInternal::iter_has_child( GtkTreeIter *iter )
{
    g_model = m_wx_model;
    
    wxDataViewItem item( (void*) iter->user_data );
    bool is_container = m_wx_model->IsContainer( item );
    
    if (!is_container)
        return FALSE;
    
    wxGtkTreeModelNode *node = FindNode( iter );
    BuildBranch( node );
    
    return (node->GetChildCount() > 0);
}

gint wxDataViewCtrlInternal::iter_n_children( GtkTreeIter *iter )
{
    g_model = m_wx_model;
    
    wxDataViewItem item( (void*) iter->user_data );
    
    if (!m_wx_model->IsContainer( item ))
        return 0;
    
    wxGtkTreeModelNode *parent_node = FindNode( iter );
    BuildBranch( parent_node );

    // wxPrintf( "iter_n_children %d\n", parent_node->GetChildCount() );
    
    return parent_node->GetChildCount();
}

gboolean wxDataViewCtrlInternal::iter_nth_child( GtkTreeIter *iter, GtkTreeIter *parent, gint n )
{
    g_model = m_wx_model;
    
    void* id = NULL;
    if (parent) id = (void*) parent->user_data;
    wxDataViewItem item( id );
    
    if (!m_wx_model->IsContainer( item ))
        return FALSE;
    
    wxGtkTreeModelNode *parent_node = FindNode( parent );
    BuildBranch( parent_node );

    // wxPrintf( "iter_nth_child %d\n", n );
    
    iter->stamp = m_gtk_model->stamp;
    iter->user_data = parent_node->GetChildren().Item( n );

    return TRUE;
}

gboolean wxDataViewCtrlInternal::iter_parent( GtkTreeIter *iter, GtkTreeIter *child )
{
    g_model = m_wx_model;

    wxGtkTreeModelNode *node = FindParentNode( child );
    if (!node)
        return FALSE;
    
    iter->stamp = m_gtk_model->stamp;
    iter->user_data = (gpointer) node->GetItem().GetID();

    return TRUE;
}
    
static wxGtkTreeModelNode*
wxDataViewCtrlInternal_FindNode( wxDataViewModel * model, wxGtkTreeModelNode *treeNode, const wxDataViewItem &item )
{
    if( model == NULL )
        return NULL;

    ItemList list;
    list.DeleteContents( true );
    wxDataViewItem it( item );
    while( it.IsOk() )
    {
        wxDataViewItem * pItem = new wxDataViewItem( it );
        list.Insert( pItem );
        it = model->GetParent( it );
    }

    wxGtkTreeModelNode * node = treeNode;
    for( ItemList::compatibility_iterator n = list.GetFirst(); n; n = n->GetNext() )
    {
        if( node && node->GetNodes().GetCount() != 0 )
        {
            int len = node->GetNodes().GetCount();
            wxGtkTreeModelNodes nodes = node->GetNodes();
            int j = 0;
            for( ; j < len; j ++)
            {
                if( nodes[j]->GetItem() == *(n->GetData()))
                {
                    node = nodes[j];
                    break;
                }    
            }

            if( j == len )
            {
                return NULL;
            }
        }
        else
            return NULL;
    }
    return node;

}

wxGtkTreeModelNode *wxDataViewCtrlInternal::FindNode( GtkTreeIter *iter )
{
    if (!iter)
        return m_root;

    wxDataViewItem item( (void*) iter->user_data );
    if (!item.IsOk())
        return m_root;
    
    wxGtkTreeModelNode *result = wxDataViewCtrlInternal_FindNode( m_wx_model, m_root, item );
    
    if (!result)
    {
        wxPrintf( "Not found %d\n", (int) iter->user_data );
        char *crash = NULL;
        *crash = 0;
    }
    
    return result;
}

wxGtkTreeModelNode *wxDataViewCtrlInternal::FindNode( const wxDataViewItem &item )
{
    if (!item.IsOk())
        return m_root;

    wxGtkTreeModelNode *result = wxDataViewCtrlInternal_FindNode( m_wx_model, m_root, item );
    
    if (!result)
    {
        wxPrintf( "Not found %d\n", (int) item.GetID() );
        char *crash = NULL;
        *crash = 0;
    }
    
    return result;
}

static wxGtkTreeModelNode*
wxDataViewCtrlInternal_FindParentNode( wxDataViewModel * model, wxGtkTreeModelNode *treeNode, const wxDataViewItem &item )
{
    if( model == NULL )
        return NULL;

    ItemList list;
    list.DeleteContents( true );
    if( !item.IsOk() )
        return NULL;

    wxDataViewItem it( model->GetParent( item ) );
    while( it.IsOk() )
    {
        wxDataViewItem * pItem = new wxDataViewItem( it );
        list.Insert( pItem );
        it = model->GetParent( it );
    }

    wxGtkTreeModelNode * node = treeNode;
    for( ItemList::compatibility_iterator n = list.GetFirst(); n; n = n->GetNext() )
    {
        if( node && node->GetNodes().GetCount() != 0 )
        {
            int len = node->GetNodes().GetCount();
            wxGtkTreeModelNodes nodes = node->GetNodes();
            int j = 0;
            for( ; j < len; j ++)
            {
                if( nodes[j]->GetItem() == *(n->GetData()))
                {
                    node = nodes[j];
                    break;
                }    
            }

            if( j == len )
            {
                return NULL;
            }
        }
        else
            return NULL;
    }
    //Examine whether the node is item's parent node
    int len = node->GetChildCount();
    for( int i = 0; i < len ; i ++ )
    {
        if( node->GetChildren().Item( i ) == item.GetID() )
            return node;
    }
    return NULL;
}

wxGtkTreeModelNode *wxDataViewCtrlInternal::FindParentNode( GtkTreeIter *iter )
{
    if (!iter)
        return NULL;
        
    wxDataViewItem item( (void*) iter->user_data );
    if (!item.IsOk())
        return NULL;

    return wxDataViewCtrlInternal_FindParentNode( m_wx_model, m_root, item );
}

wxGtkTreeModelNode *wxDataViewCtrlInternal::FindParentNode( const wxDataViewItem &item )
{
    if (!item.IsOk())
        return NULL;

    return wxDataViewCtrlInternal_FindParentNode( m_wx_model, m_root, item );
}

//-----------------------------------------------------------------------------
// wxDataViewCtrl signal callbacks
//-----------------------------------------------------------------------------

static void
wxdataview_selection_changed_callback( GtkTreeSelection* selection, wxDataViewCtrl *dv )
{
    if (!GTK_WIDGET_REALIZED(dv->m_widget))
        return;

    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_ITEM_SELECTED, dv->GetId() );
    // TODO: item
    event.SetModel( dv->GetModel() );
    dv->GetEventHandler()->ProcessEvent( event );
}

static void
wxdataview_row_activated_callback( GtkTreeView* treeview, GtkTreePath *path,
                                   GtkTreeViewColumn *column, wxDataViewCtrl *dv )
{
    wxDataViewEvent event( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, dv->GetId() );

    GtkTreeIter iter;
    dv->GtkGetInternal()->get_iter( &iter, path );
    wxDataViewItem item( (void*) iter.user_data );;
    event.SetItem( item );
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
    wxWindowList::compatibility_iterator node = win->GetChildren().GetFirst();
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
    
    delete m_internal;
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
    {
        gtk_tree_view_set_rules_hint( GTK_TREE_VIEW(m_treeview), (style & wxDV_HORIZ_RULES) != 0 );
    }

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_widget),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_widget_show (m_treeview);

    m_parent->DoAddChild( this );

    PostCreation(size);

    GtkEnableSelectionEvents();

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

bool wxDataViewCtrl::AssociateModel( wxDataViewModel *model )
{
    if (!wxDataViewCtrlBase::AssociateModel( model ))
        return false;

    GtkWxTreeModel *gtk_model = wxgtk_tree_model_new();
    m_internal = new wxDataViewCtrlInternal( this, model, gtk_model );
    gtk_model->internal = m_internal;

    m_notifier = new wxGtkDataViewModelNotifier( gtk_model, model, this );

    model->AddNotifier( m_notifier );

    gtk_tree_view_set_model( GTK_TREE_VIEW(m_treeview), GTK_TREE_MODEL(gtk_model) );
    
    // unref in wxDataViewCtrlInternal
    // g_object_unref( gtk_model );

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

wxDataViewItem wxDataViewCtrl::GetSelection()
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
    
    if (m_windowStyle & wxDV_MULTIPLE)
    {
    }
    else
    {
        GtkTreeIter iter;
        if (gtk_tree_selection_get_selected( selection, NULL, &iter ))
        {
            wxDataViewItem item( (void*) iter.user_data );
            return item;
        }
    }
    
    return wxDataViewItem(0);
}

void wxDataViewCtrl::DoSetExpanderColumn()
{
}

void wxDataViewCtrl::DoSetIndent()
{
}

void wxDataViewCtrl::GtkDisableSelectionEvents()
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
    g_signal_connect_after (selection, "changed",
                            G_CALLBACK (wxdataview_selection_changed_callback), this);
}

void wxDataViewCtrl::GtkEnableSelectionEvents()
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(m_treeview) );
    g_signal_handlers_disconnect_by_func( selection,
                            (gpointer) (wxdataview_selection_changed_callback), this);
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
