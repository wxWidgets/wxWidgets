/* ///////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.h
// Purpose:     wxWidgets's GTK base widget = GtkPizza
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////// */


#ifndef __GTK_PIZZA_H__
#define __GTK_PIZZA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtk/gtkcontainer.h>

#include "wx/dlimpexp.h"

#define GTK_PIZZA(obj)          GTK_CHECK_CAST (obj, gtk_pizza_get_type (), GtkPizza)
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

    guint m_xoffset;
    guint m_yoffset;
    
    gint  m_width;

    GdkWindow *bin_window;

    gboolean external_expose;
};

struct _GtkPizzaClass
{
  GtkContainerClass parent_class;

  void  (*set_scroll_adjustments)   (GtkPizza     *pizza,
                                     GtkAdjustment  *hadjustment,
                                     GtkAdjustment  *vadjustment);
};

WXDLLIMPEXP_CORE
GtkType    gtk_pizza_get_type        (void);
WXDLLIMPEXP_CORE
GtkWidget* gtk_pizza_new             (void);

/* accessors */

WXDLLIMPEXP_CORE
gint       gtk_pizza_get_xoffset     (GtkPizza          *pizza); 
WXDLLIMPEXP_CORE
gint       gtk_pizza_get_yoffset     (GtkPizza          *pizza); 
WXDLLIMPEXP_CORE
void       gtk_pizza_set_xoffset     (GtkPizza          *pizza, gint xoffset); 
WXDLLIMPEXP_CORE
void       gtk_pizza_set_yoffset     (GtkPizza          *pizza, gint yoffset); 


WXDLLIMPEXP_CORE
gint       gtk_pizza_get_rtl_offset  (GtkPizza          *pizza); 

WXDLLIMPEXP_CORE
void       gtk_pizza_set_shadow_type (GtkPizza          *pizza,
                                      GtkMyShadowType    type);

WXDLLIMPEXP_CORE
void       gtk_pizza_set_external    (GtkPizza          *pizza,
                                      gboolean           expose);

WXDLLIMPEXP_CORE
void       gtk_pizza_scroll          (GtkPizza          *pizza,
                                      gint               dx,
                                      gint               dy);

WXDLLIMPEXP_CORE
void       gtk_pizza_put             (GtkPizza          *pizza,
                                      GtkWidget         *widget,
                                      gint               x,
                                      gint               y,
                                      gint               width,
                                      gint               height);

WXDLLIMPEXP_CORE
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
