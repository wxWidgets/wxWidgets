/* ///////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.c
// Purpose:     native GTK+ widget for wxWindows
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////// */

#include "wx/gtk/win_gtk.h"
#include "gtk/gtksignal.h"
#include "gtk/gtknotebook.h"
#include "gtk/gtkscrolledwindow.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static void gtk_myfixed_class_init    (GtkMyFixedClass    *klass);
static void gtk_myfixed_init          (GtkMyFixed         *myfixed);
static void gtk_myfixed_map           (GtkWidget        *widget);
#if (GTK_MINOR_VERSION == 0)
static void gtk_myfixed_unmap         (GtkWidget        *widget);
#endif
static void gtk_myfixed_realize       (GtkWidget        *widget);
static void gtk_myfixed_size_request  (GtkWidget        *widget,
				     GtkRequisition   *requisition);
static void gtk_myfixed_size_allocate (GtkWidget        *widget,
				     GtkAllocation    *allocation);
static void gtk_myfixed_paint         (GtkWidget        *widget,
				     GdkRectangle     *area);
static void gtk_myfixed_draw          (GtkWidget        *widget,
				     GdkRectangle     *area);
static gint gtk_myfixed_expose        (GtkWidget        *widget,
				     GdkEventExpose   *event);
static void gtk_myfixed_add           (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_myfixed_remove        (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_myfixed_foreach       (GtkContainer     *container,
#if (GTK_MINOR_VERSION > 0)
				     gboolean	      include_internals,
#endif
				     GtkCallback      callback,
				     gpointer         callback_data);
#if (GTK_MINOR_VERSION > 0)
static GtkType gtk_myfixed_child_type (GtkContainer     *container);
#endif

#if (GTK_MINOR_VERSION > 0)
static void  gtk_myfixed_scroll_set_adjustments    (GtkMyFixed   *myfixed,
					       GtkAdjustment *hadj,
					       GtkAdjustment *vadj);
#endif



static GtkContainerClass *parent_class = NULL;

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
#if (GTK_MINOR_VERSION > 0)
	   /* reserved_1 */ NULL,
           /* reserved_2 */ NULL,
           (GtkClassInitFunc) NULL,
#else
           (GtkArgSetFunc) NULL,
           (GtkArgGetFunc) NULL,
#endif
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
  
#if (GTK_MINOR_VERSION > 0)
    parent_class = gtk_type_class (GTK_TYPE_CONTAINER);
#else
    parent_class = gtk_type_class (gtk_container_get_type ());
#endif

    widget_class->map = gtk_myfixed_map;
#if (GTK_MINOR_VERSION == 0)
    widget_class->unmap = gtk_myfixed_unmap;
#endif
    widget_class->realize = gtk_myfixed_realize;
    widget_class->size_request = gtk_myfixed_size_request;
    widget_class->size_allocate = gtk_myfixed_size_allocate;
    widget_class->draw = gtk_myfixed_draw;
    widget_class->expose_event = gtk_myfixed_expose;

    container_class->add = gtk_myfixed_add;
    container_class->remove = gtk_myfixed_remove;
#if (GTK_MINOR_VERSION > 0)
    container_class->forall = gtk_myfixed_foreach;
#else
    container_class->foreach = gtk_myfixed_foreach;
#endif

#if (GTK_MINOR_VERSION > 0)
    container_class->child_type = gtk_myfixed_child_type;
#endif

#if (GTK_MINOR_VERSION > 0)
    klass->set_scroll_adjustments = gtk_myfixed_scroll_set_adjustments;

    widget_class->set_scroll_adjustments_signal =
    gtk_signal_new ("set_scroll_adjustments",
		    GTK_RUN_LAST,
		    object_class->type,
		    GTK_SIGNAL_OFFSET (GtkMyFixedClass, set_scroll_adjustments),
		    gtk_marshal_NONE__POINTER_POINTER,
		    GTK_TYPE_NONE, 2, GTK_TYPE_ADJUSTMENT, GTK_TYPE_ADJUSTMENT);
#endif
}

#if (GTK_MINOR_VERSION > 0)
static GtkType
gtk_myfixed_child_type (GtkContainer     *container)
{
    return GTK_TYPE_WIDGET;
}
#endif

static void
gtk_myfixed_init (GtkMyFixed *myfixed)
{
    GTK_WIDGET_UNSET_FLAGS (myfixed, GTK_NO_WINDOW);
  
#if (GTK_MINOR_VERSION == 0)
    GTK_WIDGET_SET_FLAGS (myfixed, GTK_BASIC);
#endif

#if (GTK_MINOR_VERSION > 0)
    myfixed->shadow_type = GTK_MYSHADOW_NONE;
#endif

    myfixed->children = NULL;
}

GtkWidget*
gtk_myfixed_new ()
{
    GtkMyFixed *myfixed;

    myfixed = gtk_type_new (gtk_myfixed_get_type ());
  
    return GTK_WIDGET (myfixed);
}

#if (GTK_MINOR_VERSION > 0)
void  
gtk_myfixed_scroll_set_adjustments (GtkMyFixed     *myfixed,
				    GtkAdjustment  *hadj,
				    GtkAdjustment  *vadj)
{
   /* OK, this is embarassing, but this function has to be here */
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
#endif

void
gtk_myfixed_put (GtkMyFixed    *myfixed,
                 GtkWidget     *widget,
                 gint16         x,
                 gint16         y,
	         gint16         width,
	         gint16         height)
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
  
    gtk_widget_set_parent (widget, GTK_WIDGET (myfixed));

    myfixed->children = g_list_append (myfixed->children, child_info); 

    if (GTK_WIDGET_REALIZED (myfixed))
        gtk_widget_realize (widget);

    if (GTK_WIDGET_VISIBLE (myfixed) && GTK_WIDGET_VISIBLE (widget))
    {
        if (GTK_WIDGET_MAPPED (myfixed))
	    gtk_widget_map (widget);
      
        gtk_widget_queue_resize (GTK_WIDGET (myfixed));
    }
}

void
gtk_myfixed_move (GtkMyFixed    *myfixed,
                  GtkWidget     *widget,
                  gint16         x,
                  gint16         y)
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
            gtk_myfixed_set_size( myfixed, widget, x, y, child->width, child->height );
	    break;
	}
    }
}

void
gtk_myfixed_resize (GtkMyFixed     *myfixed,
                    GtkWidget      *widget,
		    gint16         width,
		    gint16         height)
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
             gtk_myfixed_set_size( myfixed, widget, child->x, child->y, width, height );
	     break;
	}
    }
}

void
gtk_myfixed_set_size (GtkMyFixed    *myfixed,
                      GtkWidget     *widget,
                      gint16         x,
                      gint16         y,
		      gint16         width,
		      gint16         height)
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

            if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (myfixed))
	    {
                if ( (child->width > 1) && 
		     (child->height > 1) && 
		    !(GTK_WIDGET_REALIZED(widget) && GTK_IS_NOTEBOOK(widget)) )
	        {
                    child_allocation.x = child->x;
                    child_allocation.y = child->y;
                    child_allocation.width = MAX( child->width, 1 );
                    child_allocation.height = MAX( child->height, 1 );
		
		    /* work around for GTK bug when moving widgets outside
		       the X window -> do NOT move them entirely outside */
		    if (child_allocation.y + child_allocation.height < 0)
		        child_allocation.y = -child_allocation.height;
		    if (child_allocation.x + child_allocation.width < 0)
		        child_allocation.x = -child_allocation.width;
		
                    gtk_widget_size_allocate (widget, &child_allocation);
                } 
	        else 
	        {
                    gtk_widget_queue_resize (GTK_WIDGET (myfixed));
	        } 
	    }
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

        if (GTK_WIDGET_VISIBLE (child->widget) && !GTK_WIDGET_MAPPED (child->widget))
	    gtk_widget_map (child->widget);
    }
    
    gdk_window_show (widget->window);
}

#if (GTK_MINOR_VERSION == 0)
static void
gtk_myfixed_unmap (GtkWidget *widget)
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));

    GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);
}
#endif

static void
gtk_myfixed_realize (GtkWidget *widget)
{
    GtkMyFixed *myfixed;
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));

    myfixed = GTK_MYFIXED (widget);
  
    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

    attributes.window_type = GDK_WINDOW_CHILD;
  
#if (GTK_MINOR_VERSION > 0)
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
    
    if (attributes.width < 2) attributes.width = 2;
    if (attributes.height < 2) attributes.height = 2;
#else
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = 32000;
    attributes.height = 32000;
#endif
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);
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
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

    widget->window = gdk_window_new( gtk_widget_get_parent_window (widget), &attributes, 
				     attributes_mask);
    gdk_window_set_user_data (widget->window, widget);
    
    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
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
    GtkMyFixedChild *child;
    GtkAllocation child_allocation; 
    GList *children; 

    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED(widget));
    g_return_if_fail (allocation != NULL);

    myfixed = GTK_MYFIXED (widget);
  
    widget->allocation = *allocation;
#if (GTK_MINOR_VERSION > 0)
    if (myfixed->shadow_type == GTK_MYSHADOW_NONE)
        border = 0;
    else
    if (myfixed->shadow_type == GTK_MYSHADOW_THIN)
        border = 1;
    else
        border = 2;
#else
    border = 0;
#endif

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize( widget->window, 
                                allocation->x+border, allocation->y+border, 
#if (GTK_MINOR_VERSION > 0)
			        allocation->width-border*2, allocation->height-border*2
#else
			        32000, 32000
#endif
                              );
    }

    children = myfixed->children;
    while (children)
    {
        child = children->data;
        children = children->next;
 
        if (GTK_WIDGET_VISIBLE (child->widget))
        { 
/*          please look at the text in wxWindow::DoSetSize() on why the
            test GTK_WIDGET_REALIZED() has to be here   */
/*          if (GTK_IS_NOTEBOOK(child->widget) && !GTK_WIDGET_REALIZED(child->widget))
            {
	        gtk_widget_queue_resize( child->widget );
	    }
	    else  */
	    {
	        child_allocation.x = child->x;
	        child_allocation.y = child->y;
                child_allocation.width = MAX( child->width, 1 );
                child_allocation.height = MAX( child->height, 1 );
		
		/* work around for GTK bug when moving widgets outside
		   the X window -> do NOT move them entirely outside */
		if (child_allocation.y + child_allocation.height < 0)
		    child_allocation.y = -child_allocation.height;
		if (child_allocation.x + child_allocation.width < 0)
		    child_allocation.x = -child_allocation.width;
	  
	        gtk_widget_size_allocate (child->widget, &child_allocation);
 	    }
        }
    }
}

static void
gtk_myfixed_paint (GtkWidget    *widget,
		   GdkRectangle *area)
{
    g_return_if_fail (widget != NULL);
    g_return_if_fail (GTK_IS_MYFIXED (widget));
    g_return_if_fail (area != NULL);

    if (GTK_WIDGET_DRAWABLE (widget))
        gdk_window_clear_area (widget->window,
			       area->x, area->y,
			       area->width, area->height);
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

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      myfixed = GTK_MYFIXED (widget);
      gtk_myfixed_paint (widget, area);

      children = myfixed->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if (gtk_widget_intersect (child->widget, area, &child_area))
	    gtk_widget_draw (child->widget, &child_area);
	}
    }
}

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

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      myfixed = GTK_MYFIXED (widget);

      child_event = *event;

      children = myfixed->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if (GTK_WIDGET_NO_WINDOW (child->widget) &&
	      gtk_widget_intersect (child->widget, &event->area, 
				    &child_event.area))
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
	  gboolean was_visible = GTK_WIDGET_VISIBLE (widget);
	  
	  gtk_widget_unparent (widget);

          /* security checks */
          g_return_if_fail (GTK_IS_WIDGET (widget));
  
	  myfixed->children = g_list_remove_link (myfixed->children, children);
	  g_list_free (children);
	  g_free (child);

	  if (was_visible && GTK_WIDGET_VISIBLE (container))
	    gtk_widget_queue_resize (GTK_WIDGET (container));

          /* security checks */
	  g_return_if_fail (GTK_IS_WIDGET (widget));
	  
	  break;
	}

      children = children->next;
    }
}

static void
gtk_myfixed_foreach (GtkContainer *container,
#if (GTK_MINOR_VERSION > 0)
		   gboolean	 include_internals,
#endif
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


#ifdef __cplusplus
}
#endif /* __cplusplus */

