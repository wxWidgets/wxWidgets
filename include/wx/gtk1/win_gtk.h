/* ///////////////////////////////////////////////////////////////////////////
// Name:        win_gtk.h
// Purpose:     wxWindows's GTK base widget
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////// */


#ifndef __GTK_MYFIXED_H__
#define __GTK_MYFIXED_H__

#include <gdk/gdk.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkfeatures.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_MYFIXED(obj)          GTK_CHECK_CAST (obj, gtk_myfixed_get_type (), GtkMyFixed)
#define GTK_MYFIXED_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_myfixed_get_type (), GtkMyFixedClass)
#define GTK_IS_MYFIXED(obj)       GTK_CHECK_TYPE (obj, gtk_myfixed_get_type ())

/* Shadow types */
typedef enum
{
  GTK_MYSHADOW_NONE,
  GTK_MYSHADOW_THIN,
  GTK_MYSHADOW_IN,
  GTK_MYSHADOW_OUT,
} GtkMyShadowType;

typedef struct _GtkMyFixed        GtkMyFixed;
typedef struct _GtkMyFixedClass   GtkMyFixedClass;

struct _GtkMyFixed
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
};

struct _GtkMyFixedClass
{
  GtkContainerClass parent_class;

  void  (*set_scroll_adjustments)   (GtkMyFixed     *myfixed,
				     GtkAdjustment  *hadjustment,
				     GtkAdjustment  *vadjustment);
};

guint      gtk_myfixed_get_type        (void);
GtkWidget* gtk_myfixed_new             (void);
void       gtk_myfixed_set_shadow_type (GtkMyFixed     *myfixed,
				        GtkMyShadowType  type);
					
void       gtk_myfixed_scroll          	(GtkMyFixed     *myfixed,	
                                         gint         dx,
                                         gint         dy);
					
void       gtk_myfixed_put             (GtkMyFixed     *myfixed,
                                        GtkWidget      *widget,
                                        gint         x,
                                        gint         y,
					gint         width,
					gint         height);

void       gtk_myfixed_move            (GtkMyFixed     *myfixed,
                                        GtkWidget      *widget,
                                        gint         x,
                                        gint         y );
					
void       gtk_myfixed_resize          (GtkMyFixed     *myfixed,
                                        GtkWidget      *widget,
                                        gint         width,
                                        gint         height );
					
void       gtk_myfixed_set_size        (GtkMyFixed     *myfixed,
                                        GtkWidget      *widget,
                                        gint         x,
                                        gint         y,
                                        gint         width,
                                        gint         height);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_MYFIXED_H__ */
