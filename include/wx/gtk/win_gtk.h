/////////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.h
// Purpose:     wxWindows's GTK base widget
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTK_MYFIXED_H__
#define __GTK_MYFIXED_H__


#include <gdk/gdk.h>
#include <gtk/gtkcontainer.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_MYFIXED(obj)          GTK_CHECK_CAST (obj, gtk_myfixed_get_type (), GtkMyFixed)
#define GTK_MYFIXED_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_myfixed_get_type (), GtkMyFixedClass)
#define GTK_IS_MYFIXED(obj)       GTK_CHECK_TYPE (obj, gtk_myfixed_get_type ())


typedef struct _GtkMyFixed        GtkMyFixed;
typedef struct _GtkMyFixedClass   GtkMyFixedClass;
typedef struct _GtkMyFixedChild   GtkMyFixedChild;

struct _GtkMyFixed
{
  GtkContainer container;

  GList *children;
  
  gint16  scroll_offset_x;
  gint16  scroll_offset_y;
};

struct _GtkMyFixedClass
{
  GtkContainerClass parent_class;
};

struct _GtkMyFixedChild
{
  GtkWidget *widget;
  gint16 x;
  gint16 y;
};

guint      gtk_myfixed_get_type        (void);
GtkWidget* gtk_myfixed_new             (void);
void       gtk_myfixed_set_offset      (GtkMyFixed     *myfixed,
                                        gint16         x,
                                        gint16         y);
void       gtk_myfixed_put             (GtkMyFixed     *myfixed,
                                        GtkWidget      *widget,
                                        gint16         x,
                                        gint16         y);
void       gtk_myfixed_move            (GtkMyFixed     *myfixed,
                                        GtkWidget      *widget,
                                        gint16         x,
                                        gint16         y);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_MYFIXED_H__ */
