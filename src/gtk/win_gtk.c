/* ///////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.c
// Purpose:     native GTK+ widget for wxWindows
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////// */

#include "wx/gtk/win_gtk.h"
#include <gtk/gtkfeatures.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static void gtk_myfixed_class_init    (GtkMyFixedClass    *klass);
static void gtk_myfixed_init          (GtkMyFixed         *myfixed);
static void gtk_myfixed_map           (GtkWidget        *widget);
static void gtk_myfixed_unmap         (GtkWidget        *widget);
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
#if (GTK_MINOR_VERSION == 1)
				     gboolean	      include_internals,
#endif
				     GtkCallback      callback,
				     gpointer         callback_data);


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
	(GtkArgSetFunc) NULL,
        (GtkArgGetFunc) NULL,
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
  
  parent_class = gtk_type_class (gtk_container_get_type ());

  widget_class->map = gtk_myfixed_map;
  widget_class->unmap = gtk_myfixed_unmap;
  widget_class->realize = gtk_myfixed_realize;
  widget_class->size_request = gtk_myfixed_size_request;
  widget_class->size_allocate = gtk_myfixed_size_allocate;
  widget_class->draw = gtk_myfixed_draw;
  widget_class->expose_event = gtk_myfixed_expose;

  container_class->add = gtk_myfixed_add;
  container_class->remove = gtk_myfixed_remove;
#if (GTK_MINOR_VERSION == 1)
  container_class->forall = gtk_myfixed_foreach;
#else
  container_class->foreach = gtk_myfixed_foreach;
#endif
}

static void
gtk_myfixed_init (GtkMyFixed *myfixed)
{
  GTK_WIDGET_UNSET_FLAGS (myfixed, GTK_NO_WINDOW);
  GTK_WIDGET_SET_FLAGS (myfixed, GTK_BASIC);
  
#if (GTK_MINOR_VERSION == 1)
  gtk_container_set_resize_mode( GTK_CONTAINER(myfixed), GTK_RESIZE_PARENT );
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

void
gtk_myfixed_put (GtkMyFixed       *myfixed,
               GtkWidget      *widget,
               gint16         x,
               gint16         y)
{
  GtkMyFixedChild *child_info;

  g_return_if_fail (myfixed != NULL);
  g_return_if_fail (GTK_IS_MYFIXED (myfixed));
  g_return_if_fail (widget != NULL);

  child_info = g_new (GtkMyFixedChild, 1);
  child_info->widget = widget;
  child_info->x = x;
  child_info->y = y;
  
  gtk_widget_set_parent (widget, GTK_WIDGET (myfixed));

  myfixed->children = g_list_append (myfixed->children, child_info); 

  if (GTK_WIDGET_REALIZED (myfixed) && !GTK_WIDGET_REALIZED (widget))
    gtk_widget_realize (widget);

  if (GTK_WIDGET_MAPPED (myfixed) && !GTK_WIDGET_MAPPED (widget))
    gtk_widget_map (widget);

  if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (myfixed))
    gtk_widget_queue_resize (GTK_WIDGET (myfixed));
}

void
gtk_myfixed_move (GtkMyFixed       *myfixed,
                GtkWidget      *widget,
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
	  if ((child->x == x) && (child->y == y)) return;
	  
          child->x = x;
          child->y = y;

          if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (myfixed))
            gtk_widget_queue_resize (GTK_WIDGET (myfixed));

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

  gdk_window_show (widget->window);

  children = myfixed->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget) &&
	  !GTK_WIDGET_MAPPED (child->widget))
	gtk_widget_map (child->widget);
    }
}

static void
gtk_myfixed_unmap (GtkWidget *widget)
{
  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_MYFIXED (widget));

  GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);
}

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
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = 32000;
  attributes.height = 32000;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= 
  GDK_EXPOSURE_MASK	|
  GDK_POINTER_MOTION_MASK	|
  GDK_BUTTON_MOTION_MASK	|
  GDK_BUTTON1_MOTION_MASK	|
  GDK_BUTTON2_MOTION_MASK	|
  GDK_BUTTON3_MOTION_MASK	|
  GDK_BUTTON_PRESS_MASK		|
  GDK_BUTTON_RELEASE_MASK	|
  GDK_KEY_PRESS_MASK		|
  GDK_KEY_RELEASE_MASK		|
  GDK_ENTER_NOTIFY_MASK		|
  GDK_LEAVE_NOTIFY_MASK		|
  GDK_FOCUS_CHANGE_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, 
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
  
  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_MYFIXED (widget));
  g_return_if_fail (requisition != NULL);

  myfixed = GTK_MYFIXED (widget);
  
  requisition->width = 0;
  requisition->height = 0;

  children = myfixed->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
          gtk_widget_size_request (child->widget, &child->widget->requisition);
	}
    }
}

static void
gtk_myfixed_size_allocate (GtkWidget     *widget,
			 GtkAllocation *allocation)
{
  GtkMyFixed *myfixed;
  GtkMyFixedChild *child;
  GtkAllocation child_allocation;
  GList *children;
  guint16 border_width;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_MYFIXED(widget));
  g_return_if_fail (allocation != NULL);

  myfixed = GTK_MYFIXED (widget);

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window, allocation->x, allocation->y, 32000, 32000 );

  border_width = GTK_CONTAINER (myfixed)->border_width;
  
  children = myfixed->children;
  while (children)
    {
      child = children->data;
      children = children->next;
      
      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  child_allocation.x = child->x + border_width;
	  child_allocation.y = child->y + border_width;
	  child_allocation.width = child->widget->requisition.width;
	  child_allocation.height = child->widget->requisition.height;
	  gtk_widget_size_allocate (child->widget, &child_allocation);
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

  gtk_myfixed_put (GTK_MYFIXED (container), widget, 0, 0);
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

	  myfixed->children = g_list_remove_link (myfixed->children, children);
	  g_list_free (children);
	  g_free (child);

	  if (GTK_WIDGET_VISIBLE (widget) && GTK_WIDGET_VISIBLE (container))
	    gtk_widget_queue_resize (GTK_WIDGET (container));

	  break;
	}

      children = children->next;
    }
}

static void
gtk_myfixed_foreach (GtkContainer *container,
#if (GTK_MINOR_VERSION == 1)
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

