/* ///////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.c
// Purpose:     Native GTK+ widget for wxWindows, based on GtkLayout and
//              GtkFixed. It makes use of the gravity window property and
//              therefore does not work with GTK 1.0.
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////// */

#include "wx/gtk/win_gtk.h"
#include "gtk/gtksignal.h"
#include "gtk/gtkprivate.h"
#include "gdk/gdkx.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IS_ONSCREEN(x,y) ((x >= G_MINSHORT) && (x <= G_MAXSHORT) && \
                          (y >= G_MINSHORT) && (y <= G_MAXSHORT))

typedef struct _GtkMyFixedAdjData  GtkMyFixedAdjData;
typedef struct _GtkMyFixedChild    GtkMyFixedChild;

struct _GtkMyFixedAdjData 
{
    gint dx;
    gint dy;
};

struct _GtkMyFixedChild
{
    GtkWidget *widget;
    gint x;
    gint y;
    gint width;
    gint height;
};

static void gtk_myfixed_class_init      (GtkMyFixedClass  *klass);
static void gtk_myfixed_init            (GtkMyFixed       *myfixed);

static void gtk_myfixed_realize         (GtkWidget        *widget);
static void gtk_myfixed_unrealize       (GtkWidget        *widget);

static void gtk_myfixed_map             (GtkWidget        *widget);

static void gtk_myfixed_size_request  (GtkWidget        *widget,
				     GtkRequisition   *requisition);
static void gtk_myfixed_size_allocate (GtkWidget        *widget,
				     GtkAllocation    *allocation);
static void gtk_myfixed_draw          (GtkWidget        *widget,
				     GdkRectangle     *area);
static gint gtk_myfixed_expose        (GtkWidget        *widget,
				     GdkEventExpose   *event);
static void gtk_myfixed_add           (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_myfixed_remove        (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_myfixed_forall       (GtkContainer     *container,
				     gboolean	      include_internals,
				     GtkCallback      callback,
				     gpointer         callback_data);

static void     gtk_myfixed_position_child     (GtkMyFixed      *myfixed,
					       GtkMyFixedChild *child);
static void     gtk_myfixed_allocate_child     (GtkMyFixed      *myfixed,
					       GtkMyFixedChild *child);
static void     gtk_myfixed_position_children  (GtkMyFixed      *myfixed);

static void     gtk_myfixed_adjust_allocations_recurse (GtkWidget *widget,
						       gpointer   cb_data);
static void     gtk_myfixed_adjust_allocations         (GtkMyFixed *myfixed,
					               gint       dx,
						       gint       dy);


static void     gtk_myfixed_expose_area        (GtkMyFixed      *myfixed,
					       gint            x, 
					       gint            y, 
					       gint            width, 
					       gint            height);
static void     gtk_myfixed_adjustment_changed (GtkAdjustment  *adjustment,
					       GtkMyFixed      *myfixed);
static GdkFilterReturn gtk_myfixed_filter      (GdkXEvent      *gdk_xevent,
					       GdkEvent       *event,
					       gpointer        data);
static GdkFilterReturn gtk_myfixed_main_filter (GdkXEvent      *gdk_xevent,
					       GdkEvent       *event,
					       gpointer        data);


static GtkType gtk_myfixed_child_type (GtkContainer     *container);

static void  gtk_myfixed_scroll_set_adjustments    (GtkMyFixed   *myfixed,
					       GtkAdjustment *hadj,
					       GtkAdjustment *vadj);


static GtkContainerClass *parent_class = NULL;
static gboolean gravity_works;

guint
gtk_myfixed_get_type ()
{
    static guint myfixed_type = 0;

    if (!myfixed_type)
    {
        GtkTypeInfo myfixed_info =
        {
	   "GtkMyFixed",
	   sizeof (GtkMyFixed),
	   sizeof (GtkMyFixedClass),
	   (GtkClassInitFunc) gtk_myfixed_class_init,
	   (GtkObjectInitFunc) gtk_myfixed_init,
	   /* reserved_1 */ NULL,
           /* reserved_2 */ NULL,
           (GtkClassInitFunc) NULL,
        };
        myfixed_type = gtk_type_unique (gtk_container_get_type (), &myfixed_info);
    }
    
    return myfixed_type;
}

static void
gtk_myfixed_class_init (GtkMyFixedClass *klass)
{
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    object_class = (GtkObjectClass*) klass;
    widget_class = (GtkWidgetClass*) klass;
    container_class = (GtkContainerClass*) klass;
    parent_class = gtk_type_class (GTK_TYPE_CONTAINER);

    widget_class->map = gtk_myfixed_map;
    widget_class->realize = gtk_myfixed_realize;
    widget_class->unrealize = gtk_myfixed_unrealize;
    widget_class->size_request = gtk_myfixed_size_request;
    widget_class->size_allocate = gtk_myfixed_size_allocate;
    widget_class->draw = gtk_myfixed_draw;
    widget_class->expose_event = gtk_myfixed_expose;

    container_class->add = gtk_myfixed_add;
    container_class->remove = gtk_myfixed_remove;
    container_class->forall = gtk_myfixed_forall;

    container_class->child_type = gtk_myfixed_child_type;

    klass->set_scroll_adjustments = gtk_myfixed_scroll_set_adjustments;

    widget_class->set_scroll_adjustments_signal =
    gtk_signal_new ("set_scroll_adjustments",
		    GTK_RUN_LAST,
		    object_class->type,
		    GTK_SIGNAL_OFFSET (GtkMyFixedClass, set_scroll_adjustments),
		    gtk_marshal_NONE__POINTER_POINTER,
		    GTK_TYPE_NONE, 2, GTK_TYPE_ADJUSTMENT, GTK_TYPE_ADJUSTMENT);
}

static GtkType
gtk_myfixed_child_type (GtkContainer     *container)
{
    return GTK_TYPE_WIDGET;
}

static void
gtk_myfixed_init (GtkMyFixed *myfixed)
{
    GTK_WIDGET_UNSET_FLAGS (myfixed, GTK_NO_WINDOW);
  
    myfixed->shadow_type = GTK_MYSHADOW_NONE;

    myfixed->children = NULL;
    
    myfixed->width = 20;
    myfixed->height = 20;

    myfixed->bin_window = NULL;

    myfixed->configure_serial = 0;
    myfixed->scroll_x = 0;
    myfixed->scroll_y = 0;
    myfixed->visibility = GDK_VISIBILITY_PARTIAL;
}

GtkWidget*
gtk_myfixed_new ()
{
    GtkMyFixed *myfixed;

    myfixed = gtk_type_new (gtk_myfixed_get_type ());
  
    return GTK_WIDGET (myfixed);
}

void  
gtk_myfixed_scroll_set_adjustments (GtkMyFixed     *myfixed,
				    GtkAdjustment  *hadj,
				    GtkAdjustment  *vadj)
{
   /* We handle scrolling in the wxScrolledWindow, not here. */
}

void 
gtk_myfixed_set_shadow_type (GtkMyFixed      *myfixed,
			     GtkMyShadowType  type)
{
    g_return_if_fail (myfixed != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (myfixed));

    if ((GtkMyShadowType) myfixed->shadow_type != type)
    {
        myfixed->shadow_type = type;

        if (GTK_WIDGET_VISIBLE (myfixed))
	{
	    gtk_widget_size_allocate (GTK_WIDGET (myfixed), &(GTK_WIDGET (myfixed)->allocation));
	    gtk_widget_queue_draw (GTK_WIDGET (myfixed));
	}
    }
}

void
gtk_myfixed_put (GtkMyFixed    *myfixed,
                 GtkWidget     *widget,
                 gint         x,
                 gint         y,
	         gint         width,
	         gint         height)
{
    GtkMyFixedChild *child_info;

    g_return_if_fail (myfixed != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (myfixed));
    g_return_if_fail (widget != NULL);

    child_info = g_new (GtkMyFixedChild, 1);
    
    child_info->widget = widget;
    child_info->x = x;
    child_info->y = y;
    child_info->width = width;
    child_info->height = height;
  
    myfixed->children = g_list_append (myfixed->children, child_info); 

    gtk_widget_set_parent (widget, GTK_WIDGET (myfixed));

    if (GTK_WIDGET_REALIZED (myfixed))
      gtk_widget_set_parent_window (widget, myfixed->bin_window);
    
    if (!IS_ONSCREEN (x, y))
       GTK_PRIVATE_SET_FLAG (widget, GTK_IS_OFFSCREEN);

    if (GTK_WIDGET_REALIZED (myfixed))
        gtk_widget_realize (widget);

    gtk_widget_set_usize (widget, width, height);
    
    if (GTK_WIDGET_VISIBLE (myfixed) && GTK_WIDGET_VISIBLE (widget))
    {
        if (GTK_WIDGET_MAPPED (myfixed))
	    gtk_widget_map (widget);
      
        gtk_widget_queue_resize (widget);
    }
}

void
gtk_myfixed_move (GtkMyFixed    *myfixed,
                  GtkWidget     *widget,
                  gint         x,
                  gint         y)
{
    GtkMyFixedChild *child;
    GList *children;

    g_return_if_fail (myfixed != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (myfixed));
    g_return_if_fail (widget != NULL);

    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;
      
        if (child->widget == widget)
        {
	    if ((child->x == x) && (child->y == y))
	        break;
	    
            child->x = x;
            child->y = y;
	    
	    if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (myfixed))
	        gtk_widget_queue_resize (widget);
	    break;
	}
    }
}

void
gtk_myfixed_resize (GtkMyFixed     *myfixed,
                    GtkWidget      *widget,
		    gint         width,
		    gint         height)
{
    GtkMyFixedChild *child;
    GList *children;

    g_return_if_fail (myfixed != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (myfixed));
    g_return_if_fail (widget != NULL);

    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;
      
        if (child->widget == widget)
        {
	    if ((child->width == width) && (child->height == height))
	        break;
	    
            child->width = width;
            child->height = height;
	    
	    gtk_widget_set_usize (widget, width, height);
	    
            if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (myfixed))
	        gtk_widget_queue_resize (widget);
	    break;
	}
    }
}

void
gtk_myfixed_set_size (GtkMyFixed    *myfixed,
                      GtkWidget     *widget,
                      gint        x,
                      gint         y,
		      gint         width,
		      gint         height)
{
    GtkMyFixedChild *child;
    GList *children;
    GtkAllocation child_allocation;

    g_return_if_fail (myfixed != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (myfixed));
    g_return_if_fail (widget != NULL);

    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        if (child->widget == widget)
        {
	    if ((child->x == x) && 
	        (child->y == y) &&
	        (child->width == width) && 
	        (child->height == height)) return;
	  
            child->x = x;
            child->y = y;
            child->width = width;
            child->height = height;
	    
	    gtk_widget_set_usize (widget, width, height);
	    
            if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (myfixed))
	        gtk_widget_queue_resize (widget);
		    
            break;
        }
    }
}

static void
gtk_myfixed_map (GtkWidget *widget)
{
    GtkMyFixed *myfixed;
    GtkMyFixedChild *child;
    GList *children;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));

    GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
    myfixed = GTK_MYFIXED (widget);

    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        if ( GTK_WIDGET_VISIBLE (child->widget) && 
	    !GTK_WIDGET_MAPPED (child->widget) &&
	    !GTK_WIDGET_IS_OFFSCREEN (child->widget))
	{
	    gtk_widget_map (child->widget);
	}
    }
    
    gdk_window_show (widget->window);
    gdk_window_show (myfixed->bin_window);
}

static void
gtk_myfixed_realize (GtkWidget *widget)
{
    GtkMyFixed *myfixed;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkMyFixedChild *child;
    GList *children;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));

    myfixed = GTK_MYFIXED (widget);
  
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
  
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;

    if (myfixed->shadow_type == GTK_MYSHADOW_NONE)
    {
        /* no border, no changes to sizes */
    } else
    if (myfixed->shadow_type == GTK_MYSHADOW_THIN)
    {
        /* GTK_MYSHADOW_THIN == wxSIMPLE_BORDER */
        attributes.x += 1;
        attributes.y += 1;
        attributes.width -= 2;
        attributes.height -= 2;
    } else
    {
        /* GTK_MYSHADOW_IN == wxSUNKEN_BORDER */
        /* GTK_MYSHADOW_OUT == wxRAISED_BORDER */
        attributes.x += 2;
        attributes.y += 2;
        attributes.width -= 4;
        attributes.height -= 4;
    }
    
    /* minimal size */
    if (attributes.width < 2) attributes.width = 2;
    if (attributes.height < 2) attributes.height = 2;
    
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
    attributes.event_mask = 
       GDK_VISIBILITY_NOTIFY_MASK;
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    
    widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				     &attributes, attributes_mask);
    gdk_window_set_user_data (widget->window, widget);

    attributes.x = 0;
    attributes.y = 0;
    
    attributes.event_mask = gtk_widget_get_events (widget);
    attributes.event_mask |= 
       GDK_EXPOSURE_MASK	|
       GDK_POINTER_MOTION_MASK	|
       GDK_POINTER_MOTION_HINT_MASK  |
       GDK_BUTTON_MOTION_MASK	|
       GDK_BUTTON1_MOTION_MASK	|
       GDK_BUTTON2_MOTION_MASK	|
       GDK_BUTTON3_MOTION_MASK	|
       GDK_BUTTON_PRESS_MASK	|
       GDK_BUTTON_RELEASE_MASK	|
       GDK_KEY_PRESS_MASK	|
       GDK_KEY_RELEASE_MASK	|
       GDK_ENTER_NOTIFY_MASK	|
       GDK_LEAVE_NOTIFY_MASK	|
       GDK_FOCUS_CHANGE_MASK;

    myfixed->bin_window = gdk_window_new (widget->window,
					  &attributes, attributes_mask);
    gdk_window_set_user_data (myfixed->bin_window, widget);
    
    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
    gtk_style_set_background (widget->style, myfixed->bin_window, GTK_STATE_NORMAL);
    
    /* add filters for intercepting visibility and expose events */
    gdk_window_add_filter (widget->window, gtk_myfixed_main_filter, myfixed);
    gdk_window_add_filter (myfixed->bin_window, gtk_myfixed_filter, myfixed);

    /* we NEED gravity or we'll give up */
    gravity_works = gdk_window_set_static_gravities (myfixed->bin_window, TRUE);

    /* cannot be done before realisation */
    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        gtk_widget_set_parent_window (child->widget, myfixed->bin_window);
    }
}

static void 
gtk_myfixed_unrealize (GtkWidget *widget)
{
  GtkMyFixed *myfixed;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_MYFIXED (widget));

  myfixed = GTK_MYFIXED (widget);

  gdk_window_set_user_data (myfixed->bin_window, NULL);
  gdk_window_destroy (myfixed->bin_window);
  myfixed->bin_window = NULL;

  if (GTK_WIDGET_CLASS (parent_class)->unrealize)
    (* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
}

static void
gtk_myfixed_size_request (GtkWidget      *widget,
			  GtkRequisition *requisition)
{
    GtkMyFixed *myfixed;
    GtkMyFixedChild *child;
    GList *children;
    GtkRequisition child_requisition;
  
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));
    g_return_if_fail (requisition != NULL);

    myfixed = GTK_MYFIXED (widget);
  
    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;

        if (GTK_WIDGET_VISIBLE (child->widget))
	{
            gtk_widget_size_request (child->widget, &child_requisition);
	}
    }
    
    /* request very little, I'm not sure if requesting nothing
       will always have positive effects on stability... */
    requisition->width = 2;
    requisition->height = 2;
}

static void
gtk_myfixed_size_allocate (GtkWidget     *widget,
			   GtkAllocation *allocation)
{
    GtkMyFixed *myfixed;
    gint border;
    gint x,y,w,h;
    GtkMyFixedChild *child;
    GtkAllocation child_allocation; 
    GList *children;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED(widget));
    g_return_if_fail (allocation != NULL);

    myfixed = GTK_MYFIXED (widget);
  
    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;
 
        gtk_myfixed_position_child (myfixed, child);
        gtk_myfixed_allocate_child (myfixed, child);
    }
    
    widget->allocation = *allocation;
    
    if (myfixed->shadow_type == GTK_MYSHADOW_NONE)
        border = 0;
    else
    if (myfixed->shadow_type == GTK_MYSHADOW_THIN)
        border = 1;
    else
        border = 2;
	
    x = allocation->x + border;
    y = allocation->y + border;
    w = allocation->width - border*2;
    h = allocation->height - border*2;

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize( widget->window, x, y, w, h );
        gdk_window_move_resize( myfixed->bin_window, 0, 0, w, h );
    }
}

static void
gtk_myfixed_draw (GtkWidget    *widget,
		  GdkRectangle *area)
{
    GtkMyFixed *myfixed;
    GtkMyFixedChild *child;
    GdkRectangle child_area;
    GList *children;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));

    myfixed = GTK_MYFIXED (widget);
      
    children = myfixed->children;
    if ( !(GTK_WIDGET_APP_PAINTABLE (widget)) )
    {
        gdk_window_clear_area( myfixed->bin_window,
			        area->x, area->y, area->width, area->height);
    }

    while (children)
    {
	child = children->data;
	children = children->next;

	if (gtk_widget_intersect (child->widget, area, &child_area))
	    gtk_widget_draw (child->widget, &child_area);
    }
}

/*
static void
gtk_myfixed_draw_border (GtkMyFixed *myfixed)
{
    GtkWidget *widget;
    
    widget = GTK_WIDGET(myfixed);

    if (myfixed->shadow_type == GTK_MYSHADOW_NONE)
        return;
	
    if (myfixed->shadow_type == GTK_MYSHADOW_OUT)
    {
        gtk_draw_shadow( widget->style,
                         widget->window,
                         GTK_STATE_NORMAL,
                         GTK_SHADOW_OUT,
                         0, 0,
			 widget->allocation.width,
			 widget->allocation.height );
        return;
    }

    if (myfixed->shadow_type == GTK_MYSHADOW_IN)
    {
        gtk_draw_shadow( widget->style,
                         widget->window,
                         GTK_STATE_NORMAL,
                         GTK_SHADOW_IN,
                         0, 0,
			 widget->allocation.width,
			 widget->allocation.height );
        return;
    }
    
    if (myfixed->shadow_type == GTK_MYSHADOW_THIN)
    {
        GdkGC *gc;
	gc = gdk_gc_new( widget->window );
        gdk_gc_set_foreground( gc, &widget->style->black );
        gdk_draw_rectangle( widget->window, gc, FALSE, 
                            0, 0, 
			    widget->allocation.width-1,
			    widget->allocation.height-1 );
        gdk_gc_unref( gc );
        return;
    }
}
*/

static gint
gtk_myfixed_expose (GtkWidget      *widget,
		    GdkEventExpose *event)
{
    GtkMyFixed *myfixed;
    GtkMyFixedChild *child;
    GdkEventExpose child_event;
    GList *children;

    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_MYFIXED (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    myfixed = GTK_MYFIXED (widget);

/*
    if (event->window == widget->window)
    {
        gtk_myfixed_draw_border( myfixed );
        return FALSE;
    }
*/

    if (event->window != myfixed->bin_window)
        return FALSE;

    children = myfixed->children;
    while (children)
    {
	child = children->data;
	children = children->next;

        child_event = *event;

	if (GTK_WIDGET_NO_WINDOW (child->widget) &&
	    GTK_WIDGET_DRAWABLE (child->widget) &&
	    gtk_widget_intersect (child->widget, &event->area, &child_event.area))
	{
	    gtk_widget_event (child->widget, (GdkEvent*) &child_event);
	}
    }

    return FALSE;
}

static void
gtk_myfixed_add (GtkContainer *container,
	       GtkWidget    *widget)
{
    g_return_if_fail (container != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (container));
    g_return_if_fail (widget != NULL);

    gtk_myfixed_put (GTK_MYFIXED (container), widget, 0, 0, 20, 20 );
}

static void
gtk_myfixed_remove (GtkContainer *container,
		    GtkWidget    *widget)
{
    GtkMyFixed *myfixed;
    GtkMyFixedChild *child;
    GList *children;

    g_return_if_fail (container != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (container));
    g_return_if_fail (widget != NULL);

    myfixed = GTK_MYFIXED (container);

    children = myfixed->children;
    while (children)
    {
        child = children->data;

        if (child->widget == widget)
	{
	    gtk_widget_unparent (widget);

            /* security checks */
            g_return_if_fail (GTK_IS_WIDGET (widget));
  
	    myfixed->children = g_list_remove_link (myfixed->children, children);
	    g_list_free (children);
	    g_free (child);

            /* security checks */
	    g_return_if_fail (GTK_IS_WIDGET (widget));
    
            GTK_PRIVATE_UNSET_FLAG (widget, GTK_IS_OFFSCREEN);
	    
	    break;
	}

        children = children->next;
    }
}

static void
gtk_myfixed_forall (GtkContainer *container,
		   gboolean	 include_internals,
		   GtkCallback   callback,
		   gpointer      callback_data)
{
  GtkMyFixed *myfixed;
  GtkMyFixedChild *child;
  GList *children;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_MYFIXED (container));
  g_return_if_fail (callback != NULL);

  myfixed = GTK_MYFIXED (container);

  children = myfixed->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      (* callback) (child->widget, callback_data);
    }
}


/* Operations on children
 */

static void
gtk_myfixed_position_child (GtkMyFixed      *myfixed,
			   GtkMyFixedChild *child)
{
  gint x;
  gint y;

  x = child->x - myfixed->xoffset;
  y = child->y - myfixed->yoffset;

  if (IS_ONSCREEN (x,y))
    {
      if (GTK_WIDGET_MAPPED (myfixed) &&
	  GTK_WIDGET_VISIBLE (child->widget))
	{
	  if (!GTK_WIDGET_MAPPED (child->widget))
	    gtk_widget_map (child->widget);
	}

      if (GTK_WIDGET_IS_OFFSCREEN (child->widget))
	GTK_PRIVATE_UNSET_FLAG (child->widget, GTK_IS_OFFSCREEN);
    }
  else
    {
      if (!GTK_WIDGET_IS_OFFSCREEN (child->widget))
	GTK_PRIVATE_SET_FLAG (child->widget, GTK_IS_OFFSCREEN);

      if (GTK_WIDGET_MAPPED (child->widget))
	gtk_widget_unmap (child->widget);
    }
}

static void
gtk_myfixed_allocate_child (GtkMyFixed      *myfixed,
			   GtkMyFixedChild *child)
{
  GtkAllocation allocation;
  GtkRequisition requisition;

  allocation.x = child->x - myfixed->xoffset;
  allocation.y = child->y - myfixed->yoffset;
  gtk_widget_get_child_requisition (child->widget, &requisition);
  allocation.width = requisition.width;
  allocation.height = requisition.height;
  
  gtk_widget_size_allocate (child->widget, &allocation);
}

static void
gtk_myfixed_position_children (GtkMyFixed *myfixed)
{
  GList *tmp_list;

  tmp_list = myfixed->children;
  while (tmp_list)
    {
      GtkMyFixedChild *child = tmp_list->data;
      tmp_list = tmp_list->next;
      
      gtk_myfixed_position_child (myfixed, child);
    }
}

static void
gtk_myfixed_adjust_allocations_recurse (GtkWidget *widget,
				       gpointer   cb_data)
{
  GtkMyFixedAdjData *data = cb_data;

  widget->allocation.x += data->dx;
  widget->allocation.y += data->dy;

  if (GTK_WIDGET_NO_WINDOW (widget) &&
      GTK_IS_CONTAINER (widget))
    gtk_container_forall (GTK_CONTAINER (widget), 
			  gtk_myfixed_adjust_allocations_recurse,
			  cb_data);
}

static void
gtk_myfixed_adjust_allocations (GtkMyFixed *myfixed,
			       gint       dx,
			       gint       dy)
{
  GList *tmp_list;
  GtkMyFixedAdjData data;

  data.dx = dx;
  data.dy = dy;

  tmp_list = myfixed->children;
  while (tmp_list)
    {
      GtkMyFixedChild *child = tmp_list->data;
      tmp_list = tmp_list->next;
      
      child->widget->allocation.x += dx;
      child->widget->allocation.y += dy;

      if (GTK_WIDGET_NO_WINDOW (child->widget) &&
	  GTK_IS_CONTAINER (child->widget))
	gtk_container_forall (GTK_CONTAINER (child->widget), 
			      gtk_myfixed_adjust_allocations_recurse,
			      &data);
    }
}
  
/* Callbacks */

/* Send a synthetic expose event to the widget
 */
static void
gtk_myfixed_expose_area (GtkMyFixed    *myfixed,
			gint x, gint y, gint width, gint height)
{
  if (myfixed->visibility == GDK_VISIBILITY_UNOBSCURED)
    {
      GdkEventExpose event;
      
      event.type = GDK_EXPOSE;
      event.send_event = TRUE;
      event.window = myfixed->bin_window;
      event.count = 0;
      
      event.area.x = x;
      event.area.y = y;
      event.area.width = width;
      event.area.height = height;
      
      gdk_window_ref (event.window);
      gtk_widget_event (GTK_WIDGET (myfixed), (GdkEvent *)&event);
      gdk_window_unref (event.window);
    }
}

/* This function is used to find events to process while scrolling
 */

static Bool 
gtk_myfixed_expose_predicate (Display *display, 
		  XEvent  *xevent, 
		  XPointer arg)
{
  if ((xevent->type == Expose) || 
      ((xevent->xany.window == *(Window *)arg) &&
       (xevent->type == ConfigureNotify)))
    return True;
  else
    return False;
}

/* This is the main routine to do the scrolling. Scrolling is
 * done by "Guffaw" scrolling, as in the Mozilla XFE, with
 * a few modifications.
 * 
 * The main improvement is that we keep track of whether we
 * are obscured or not. If not, we ignore the generated expose
 * events and instead do the exposes ourself, without having
 * to wait for a roundtrip to the server. This also provides
 * a limited form of expose-event compression, since we do
 * the affected area as one big chunk.
 */

void
gtk_myfixed_scroll (GtkMyFixed *myfixed, gint dx, gint dy)
{
  GtkWidget *widget;
  XEvent xevent;

  gint x,y,w,h,border;
  
  widget = GTK_WIDGET (myfixed);

  myfixed->xoffset += dx;
  myfixed->yoffset += dy;

  if (!GTK_WIDGET_MAPPED (myfixed))
    {
      gtk_myfixed_position_children (myfixed);
      return;
    }

  gtk_myfixed_adjust_allocations (myfixed, -dx, -dy);

  if (myfixed->shadow_type == GTK_MYSHADOW_NONE)
    border = 0;
  else
  if (myfixed->shadow_type == GTK_MYSHADOW_THIN)
    border = 1;
  else
    border = 2;
	
  x = 0;
  y = 0;
  w = widget->allocation.width - 2*border;
  h = widget->allocation.height - 2*border;
  
  if (dx > 0)
    {
      if (gravity_works)
	{
	  gdk_window_resize (myfixed->bin_window,
			     w + dx,
			     h);
	  gdk_window_move   (myfixed->bin_window, x-dx, y);
	  gdk_window_move_resize (myfixed->bin_window, x, y, w, h );
	}
      else
	{
	  /* FIXME */
	}

      gtk_myfixed_expose_area (myfixed, 
			      MAX ((gint)w - dx, 0),
			      0,
			      MIN (dx, w),
			      h);
    }
  else if (dx < 0)
    {
      if (gravity_works)
	{
	  gdk_window_move_resize (myfixed->bin_window,
				  x + dx, 
				  y,
				  w - dx,
				  h);
	  gdk_window_move   (myfixed->bin_window, x, y);
	  gdk_window_resize (myfixed->bin_window, w, h );
	}
      else
	{
	  /* FIXME */
	}

      gtk_myfixed_expose_area (myfixed,
			      0,
			      0,
			      MIN (-dx, w),
			      h);
    }

  if (dy > 0)
    {
      if (gravity_works)
	{
	  gdk_window_resize (myfixed->bin_window, w, h + dy);
	  gdk_window_move   (myfixed->bin_window, x, y-dy);
	  gdk_window_move_resize (myfixed->bin_window,
				  x, y, w, h );
	}
      else
	{
	  /* FIXME */
	}

      gtk_myfixed_expose_area (myfixed, 
			      0,
			      MAX ((gint)h - dy, 0),
			      w,
			      MIN (dy, h));
    }
  else if (dy < 0)
    {
      if (gravity_works)
	{
	  gdk_window_move_resize (myfixed->bin_window,
				  x, y+dy, w, h - dy );
	  gdk_window_move   (myfixed->bin_window, x, y);
	  gdk_window_resize (myfixed->bin_window, w, h );
	}
      else
	{
	  /* FIXME */
	}
      gtk_myfixed_expose_area (myfixed, 
			      0,
			      0,
			      w,
			      MIN (-dy, (gint)h));
    }

  gtk_myfixed_position_children (myfixed);

  /* We have to make sure that all exposes from this scroll get
   * processed before we scroll again, or the expose events will
   * have invalid coordinates.
   *
   * We also do expose events for other windows, since otherwise
   * their updating will fall behind the scrolling 
   *
   * This also avoids a problem in pre-1.0 GTK where filters don't
   * have access to configure events that were compressed.
   */

  gdk_flush();
  while (XCheckIfEvent(GDK_WINDOW_XDISPLAY (myfixed->bin_window),
		       &xevent,
		       gtk_myfixed_expose_predicate,
		       (XPointer)&GDK_WINDOW_XWINDOW (myfixed->bin_window)))
    {
      GdkEvent event;
      GtkWidget *event_widget;

      if ((xevent.xany.window == GDK_WINDOW_XWINDOW (myfixed->bin_window)) &&
	  (gtk_myfixed_filter (&xevent, &event, myfixed) == GDK_FILTER_REMOVE))
	continue;
      
      if (xevent.type == Expose)
	{
	  event.expose.window = gdk_window_lookup (xevent.xany.window);
	  gdk_window_get_user_data (event.expose.window, 
				    (gpointer *)&event_widget);

	  if (event_widget)
	    {
	      event.expose.type = GDK_EXPOSE;
	      event.expose.area.x = xevent.xexpose.x;
	      event.expose.area.y = xevent.xexpose.y;
	      event.expose.area.width = xevent.xexpose.width;
	      event.expose.area.height = xevent.xexpose.height;
	      event.expose.count = xevent.xexpose.count;
	      
	      gdk_window_ref (event.expose.window);
	      gtk_widget_event (event_widget, &event);
	      gdk_window_unref (event.expose.window);
	    }
	}
    }
}

/* The main event filter. Actually, we probably don't really need
 * to install this as a filter at all, since we are calling it
 * directly above in the expose-handling hack. But in case scrollbars
 * are fixed up in some manner...
 *
 * This routine identifies expose events that are generated when
 * we've temporarily moved the bin_window_origin, and translates
 * them or discards them, depending on whether we are obscured
 * or not.
 */
static GdkFilterReturn 
gtk_myfixed_filter (GdkXEvent *gdk_xevent,
		   GdkEvent  *event,
		   gpointer   data)
{
  XEvent *xevent;
  GtkMyFixed *myfixed;

  xevent = (XEvent *)gdk_xevent;
  myfixed = GTK_MYFIXED (data);

  switch (xevent->type)
    {
    case Expose:
      if (xevent->xexpose.serial == myfixed->configure_serial)
	{
	  if (myfixed->visibility == GDK_VISIBILITY_UNOBSCURED)
	    return GDK_FILTER_REMOVE;
	  else
	    {
	      xevent->xexpose.x += myfixed->scroll_x;
	      xevent->xexpose.y += myfixed->scroll_y;
	      
	      break;
	    }
	}
      break;
      
    case ConfigureNotify:
       if ((xevent->xconfigure.x != 0) || (xevent->xconfigure.y != 0))
	{
	  myfixed->configure_serial = xevent->xconfigure.serial;
	  myfixed->scroll_x = xevent->xconfigure.x;
	  myfixed->scroll_y = xevent->xconfigure.y;
	}
      break;
    }
  
  return GDK_FILTER_CONTINUE;
}

/* Although GDK does have a GDK_VISIBILITY_NOTIFY event,
 * there is no corresponding event in GTK, so we have
 * to get the events from a filter
 */
static GdkFilterReturn 
gtk_myfixed_main_filter (GdkXEvent *gdk_xevent,
			GdkEvent  *event,
			gpointer   data)
{
  XEvent *xevent;
  GtkMyFixed *myfixed;

  xevent = (XEvent *)gdk_xevent;
  myfixed = GTK_MYFIXED (data);

  if (xevent->type == VisibilityNotify)
    {
      switch (xevent->xvisibility.state)
	{
	case VisibilityFullyObscured:
	  myfixed->visibility = GDK_VISIBILITY_FULLY_OBSCURED;
	  break;

	case VisibilityPartiallyObscured:
	  myfixed->visibility = GDK_VISIBILITY_PARTIAL;
	  break;

	case VisibilityUnobscured:
	  myfixed->visibility = GDK_VISIBILITY_UNOBSCURED;
	  break;
	}

      return GDK_FILTER_REMOVE;
    }

  
  return GDK_FILTER_CONTINUE;
}




#ifdef __cplusplus
}
#endif /* __cplusplus */

