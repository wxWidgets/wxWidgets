/* ///////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.h
// Purpose:     wxWindows's GTK base widget = GtkPizza
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////// */


#ifndef __GTK_PIZZA_H__
#define __GTK_PIZZA_H__

#include <gdk/gdk.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkfeatures.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_PIZZA(obj)          GTK_CHECK_CAST (obj, gtk_pizza_get_type (), GtkPizza)
#define GTK_PIZZA_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_pizza_get_type (), GtkPizzaClass)
#define GTK_IS_PIZZA(obj)       GTK_CHECK_TYPE (obj, gtk_pizza_get_type ())

/* Shadow types */
typedef enum
{
    GTK_MYSHADOW_NONE,
    GTK_MYSHADOW_THIN,
    GTK_MYSHADOW_IN,
    GTK_MYSHADOW_OUT
} GtkMyShadowType;

typedef struct _GtkPizzaChild    GtkPizzaChild;
typedef struct _GtkPizza        GtkPizza;
typedef struct _GtkPizzaClass   GtkPizzaClass;

struct _GtkPizzaChild
{
    GtkWidget *widget;
    gint x;
    gint y;
    gint width;
    gint height;
};

struct _GtkPizza
{
    GtkContainer container;
    GList *children;
    GtkMyShadowType shadow_type;
  
    guint width;
    guint height;

    guint xoffset;
    guint yoffset;

    GdkWindow *bin_window;

    GdkVisibilityState visibility;
    gulong configure_serial;
    gint scroll_x;
    gint scroll_y;
  
    gboolean clear_on_draw;
    gboolean use_filter;
    gboolean external_expose;
};

struct _GtkPizzaClass
{
  GtkContainerClass parent_class;

  void  (*set_scroll_adjustments)   (GtkPizza     *pizza,
				     GtkAdjustment  *hadjustment,
				     GtkAdjustment  *vadjustment);
};

guint      gtk_pizza_get_type        (void);
GtkWidget* gtk_pizza_new             (void);

void       gtk_pizza_set_shadow_type (GtkPizza          *pizza,
				                      GtkMyShadowType    type);
					
void       gtk_pizza_set_clear       (GtkPizza          *pizza,
                                      gboolean           clear);		
					
void       gtk_pizza_set_filter      (GtkPizza          *pizza,
                                      gboolean           use);		
					
void       gtk_pizza_set_external    (GtkPizza          *pizza,
                                      gboolean           expose);		
					
void       gtk_pizza_scroll          (GtkPizza          *pizza,	
                                      gint               dx,
                                      gint               dy);
					
void       gtk_pizza_put             (GtkPizza          *pizza,
                                      GtkWidget         *widget,
                                      gint               x,
                                      gint               y,
					                  gint               width,
					                  gint               height);

void       gtk_pizza_move            (GtkPizza          *pizza,
                                      GtkWidget         *widget,
                                      gint               x,
                                      gint               y );
					
void       gtk_pizza_resize          (GtkPizza          *pizza,
                                      GtkWidget         *widget,
                                      gint               width,
                                      gint               height );
					
void       gtk_pizza_set_size        (GtkPizza          *pizza,
                                      GtkWidget         *widget,
                                      gint               x,
                                      gint               y,
                                      gint               width,
                                      gint               height);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PIZZA_H__ */
