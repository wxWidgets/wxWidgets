/* Jackets for VMS only */

#include <stdarg.h>

#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

void GDK_BEEP (void)
{
   gdk_beep();
}

GdkBitmap* GDK_BITMAP_CREATE_FROM_DATA	(GdkWindow   *window,
					 const gchar *data,
					 gint	      width,
					 gint	      height)
{
   return gdk_bitmap_create_from_data	(window,
					 data,
					 width,
					 height);
}

void	   GDK_BITMAP_UNREF		(GdkBitmap  *pixmap)
{
   gdk_bitmap_unref		(pixmap);
}

gint	 GDK_CHAR_HEIGHT    (GdkFont        *font,
			     gchar           character)
{
   return gdk_char_height    (font,
			     character);
}

GdkColormap* GDK_COLORMAP_GET_SYSTEM	  (void)
{
   return gdk_colormap_get_system();
}

GdkVisual*   GDK_COLORMAP_GET_VISUAL      (GdkColormap    *colormap)
{
   return gdk_colormap_get_visual      (colormap);
}

GdkColormap* GDK_COLORMAP_NEW	          (GdkVisual      *visual,
					   gboolean        allocate)
{
   return gdk_colormap_new	          (visual,
					   allocate);
}

void	     GDK_COLORMAP_UNREF	          (GdkColormap    *cmap)
{
   gdk_colormap_unref	          (cmap);
}

gboolean GDK_COLOR_ALLOC  (GdkColormap	*colormap,
			   GdkColor	*color)
{
   return gdk_color_alloc  (colormap,
			   color);
}

gboolean     GDK_COLOR_PARSE	          (const gchar    *spec,
					   GdkColor       *color)
{
   return gdk_color_parse	          (spec,
					   color);
}

void	   GDK_CURSOR_DESTROY		 (GdkCursor	 *cursor)
{
   gdk_cursor_destroy		 (cursor);
}

GdkCursor* GDK_CURSOR_NEW		 (GdkCursorType	  cursor_type)
{
   return gdk_cursor_new		 (cursor_type);
}

void GDK_DRAW_ARC	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  gint		filled,
			  gint		x,
			  gint		y,
			  gint		width,
			  gint		height,
			  gint		angle1,
			  gint		angle2)
{
   gdk_draw_arc	 (drawable,
		gc,
		filled,
		x,
		y,
		width,
		height,
		angle1,
		angle2);
}

void GDK_DRAW_IMAGE	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  GdkImage     *image,
			  gint		xsrc,
			  gint		ysrc,
			  gint		xdest,
			  gint		ydest,
			  gint		width,
			  gint		height)
{
   gdk_draw_image	 (drawable,
			  gc,
			  image,
			  xsrc,
			  ysrc,
			  xdest,
			  ydest,
			  width,
			  height);
}

void GDK_DRAW_LINE	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  gint		x1,
			  gint		y1,
			  gint		x2,
			  gint		y2)
{
   gdk_draw_line	 (drawable,
			  gc,
			  x1,
			  y1,
			  x2,
			  y2);
}

void GDK_DRAW_PIXMAP	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  GdkDrawable  *src,
			  gint		xsrc,
			  gint		ysrc,
			  gint		xdest,
			  gint		ydest,
			  gint		width,
			  gint		height)
{
   gdk_draw_pixmap	 (drawable,
			  gc,
			  src,
			  xsrc,
			  ysrc,
			  xdest,
			  ydest,
			  width,
			  height);
}

void GDK_DRAW_POINT	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  gint		x,
			  gint		y)
{
   gdk_draw_point	 (drawable,
			  gc,
			  x,
			  y);
}

void GDK_DRAW_POLYGON	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  gint		filled,
			  GdkPoint     *points,
			  gint		npoints)
{
   gdk_draw_polygon	 (drawable,
			  gc,
			  filled,
			  points,
			  npoints);
}

void GDK_DRAW_RECTANGLE	 (GdkDrawable  *drawable,
			  GdkGC	       *gc,
			  gint		filled,
			  gint		x,
			  gint		y,
			  gint		width,
			  gint		height)
{
   gdk_draw_rectangle	 (drawable,
			  gc,
			  filled,
			  x,
			  y,
			  width,
			  height);
}

void GDK_DRAW_STRING	 (GdkDrawable  *drawable,
			  GdkFont      *font,
			  GdkGC	       *gc,
			  gint		x,
			  gint		y,
			  const gchar  *string)
{
   gdk_draw_string	 (drawable,
			  font,
			  gc,
			  x,
			  y,
			  string);
}

void GDK_FLUSH (void)
{
   gdk_flush ();
}

GdkFont* GDK_FONT_LOAD	    (const gchar    *font_name)
{
   return gdk_font_load	    (font_name);
}

GdkFont* GDK_FONT_REF	    (GdkFont        *font)
{
   return gdk_font_ref	    (font);
}

void	 GDK_FONT_UNREF	    (GdkFont        *font)
{
   gdk_font_unref	    (font);
}

void   GDK_GC_DESTROY		  (GdkGC	    *gc)
{
   gdk_gc_destroy		  (gc);
}

GdkGC* GDK_GC_NEW		  (GdkWindow	    *window)
{
   return gdk_gc_new		  (window);
}

void   GDK_GC_SET_BACKGROUND	  (GdkGC	    *gc,
				   GdkColor	    *color)
{
   gdk_gc_set_background	  (gc,
				   color);
}

void   GDK_GC_SET_CLIP_MASK	  (GdkGC	    *gc,
				   GdkBitmap	    *mask)
{
   gdk_gc_set_clip_mask	  (gc,
			mask);
}

void   GDK_GC_SET_CLIP_ORIGIN	  (GdkGC	    *gc,
				   gint		     x,
				   gint		     y)
{
   gdk_gc_set_clip_origin	  (gc,
				   x,
				   y);
}

void   GDK_GC_SET_CLIP_RECTANGLE  (GdkGC	    *gc,
				   GdkRectangle	    *rectangle)
{
   gdk_gc_set_clip_rectangle  (gc,
			rectangle);
}

void   GDK_GC_SET_CLIP_REGION	  (GdkGC	    *gc,
				   GdkRegion	    *region)
{
   gdk_gc_set_clip_region	  (gc,
				   region);
}

void   GDK_GC_SET_DASHES          (GdkGC            *gc,
				   gint	             dash_offset,
				   gint8             dash_list[],
				   gint              n)
{
   gdk_gc_set_dashes          (gc,
			dash_offset,
			dash_list,
			n);
}

void   GDK_GC_SET_EXPOSURES	  (GdkGC	    *gc,
				   gboolean          exposures)
{
   gdk_gc_set_exposures	  (gc,
			exposures);
}

void   GDK_GC_SET_FILL		  (GdkGC	    *gc,
				   GdkFill	     fill)
{
   gdk_gc_set_fill		  (gc,
				   fill);
}

void   GDK_GC_SET_FOREGROUND	  (GdkGC	    *gc,
				   GdkColor	    *color)
{
   gdk_gc_set_foreground	  (gc,
				   color);
}

void   GDK_GC_SET_FUNCTION	  (GdkGC	    *gc,
				   GdkFunction	     function)
{
   gdk_gc_set_function	  (gc,
			function);
}

void   GDK_GC_SET_LINE_ATTRIBUTES (GdkGC	    *gc,
				   gint		     line_width,
				   GdkLineStyle	     line_style,
				   GdkCapStyle	     cap_style,
				   GdkJoinStyle	     join_style)
{
   gdk_gc_set_line_attributes (gc,
			line_width,
			line_style,
			cap_style,
			join_style);
}

void   GDK_GC_SET_STIPPLE	  (GdkGC	    *gc,
				   GdkPixmap	    *stipple)
{
   gdk_gc_set_stipple	  (gc,
			stipple);
}

void   GDK_GC_SET_SUBWINDOW	  (GdkGC	    *gc,
				   GdkSubwindowMode  mode)
{
   gdk_gc_set_subwindow	  (gc,
			mode);
}

void   GDK_GC_SET_TILE		  (GdkGC	    *gc,
				   GdkPixmap	    *tile)
{
   gdk_gc_set_tile		  (gc,
				   tile);
}

void   GDK_GC_SET_TS_ORIGIN	  (GdkGC	    *gc,
				   gint		     x,
				   gint		     y)
{
   gdk_gc_set_ts_origin	  (gc,
			x,
			y);
}

void   GDK_GC_UNREF		  (GdkGC	    *gc)
{
   gdk_gc_unref		  (gc);
}

void	   GDK_IMAGE_DESTROY   (GdkImage     *image)
{
   gdk_image_destroy   (image);
}

GdkImage*  GDK_IMAGE_GET       (GdkWindow    *window,
				gint	      x,
				gint	      y,
				gint	      width,
				gint	      height)
{
   return gdk_image_get       (window,
			x,
			y,
			width,
			height);
}

guint32	   GDK_IMAGE_GET_PIXEL (GdkImage     *image,
				gint	      x,
				gint	      y)
{
   return gdk_image_get_pixel (image,
			x,
			y);
}

GdkImage*  GDK_IMAGE_NEW       (GdkImageType  type,
				GdkVisual    *visual,
				gint	      width,
				gint	      height)
{
   return gdk_image_new       (type,
			visual,
			width,
			height);
}

GdkImage* GDK_IMAGE_NEW_BITMAP(GdkVisual     *visual,
				gpointer      data,
				gint          width,
				gint          height)
{
   return gdk_image_new_bitmap(visual,
			data,
			width,
			height);
}

void	   GDK_IMAGE_PUT_PIXEL (GdkImage     *image,
				gint	      x,
				gint	      y,
				guint32	      pixel)
{
   gdk_image_put_pixel (image,
			x,
			y,
			pixel);

}

gint GDK_INPUT_ADD_FULL	  (gint		     source,
			   GdkInputCondition condition,
			   GdkInputFunction  function,
			   gpointer	     data,
			   GdkDestroyNotify  destroy)
{
   return gdk_input_add_full	  (source,
			   condition,
			   function,
			   data,
			   destroy);
}

void GDK_INPUT_REMOVE	  (gint		     tag)
{
   gdk_input_remove	  (tag);
}

guint    GDK_KEYVAL_TO_UPPER		  (guint	keyval)
{
   return gdk_keyval_to_upper		  (keyval);
}

GdkPixmap* GDK_PIXMAP_CREATE_FROM_XPM	(GdkWindow  *window,
					 GdkBitmap **mask,
					 GdkColor   *transparent_color,
					 const gchar *filename)
{
   return gdk_pixmap_create_from_xpm	(window,
					 mask,
					 transparent_color,
					 filename);
}

GdkPixmap* GDK_PIXMAP_CREATE_FROM_XPM_D (GdkWindow  *window,
					 GdkBitmap **mask,
					 GdkColor   *transparent_color,
					 gchar	   **data)
{
   return gdk_pixmap_create_from_xpm_d (window,
					mask,
					transparent_color,
					data);
}

GdkPixmap* GDK_PIXMAP_NEW		(GdkWindow  *window,
					 gint	     width,
					 gint	     height,
					 gint	     depth)
{
   return gdk_pixmap_new		(window,
					 width,
					 height,
					 depth);
}

void	   GDK_PIXMAP_UNREF		(GdkPixmap  *pixmap)
{
   gdk_pixmap_unref		(pixmap);
}

gint     GDK_POINTER_GRAB       (GdkWindow    *window,
                                 gint          owner_events,
                                 GdkEventMask  event_mask,
                                 GdkWindow    *confine_to,
                                 GdkCursor    *cursor,
                                 guint32       time)
{
   return gdk_pointer_grab       (window,
                                 owner_events,
                                 event_mask,
                                 confine_to,
                                 cursor,
                                 time);
}

void     GDK_POINTER_UNGRAB     (guint32       time)
{
   gdk_pointer_ungrab     (time);
}

GdkRegion*    GDK_REGIONS_INTERSECT	  (GdkRegion	  *source1,
					   GdkRegion	  *source2)
{
   return gdk_regions_intersect	  (source1,
				source2);

}

GdkRegion*    GDK_REGIONS_SUBTRACT	  (GdkRegion	  *source1,
					   GdkRegion	  *source2)
{
   return gdk_regions_subtract	  (source1,
				source2);
}

GdkRegion*    GDK_REGIONS_UNION		  (GdkRegion	  *source1,
					   GdkRegion	  *source2)
{
   return gdk_regions_union		  (source1,
					   source2);
}

GdkRegion*    GDK_REGIONS_XOR		  (GdkRegion	  *source1,
					   GdkRegion	  *source2)
{
   return gdk_regions_xor		  (source1,
					   source2);
}

void	       GDK_REGION_DESTROY   (GdkRegion	   *region)
{
   gdk_region_destroy   (region);
}

gboolean       GDK_REGION_EMPTY	    (GdkRegion	   *region)
{
   return gdk_region_empty	    (region);
}

void	       GDK_REGION_GET_CLIPBOX(GdkRegion    *region,
				      GdkRectangle *rectangle)
{
   gdk_region_get_clipbox(region,
			rectangle);
}

GdkRegion*     GDK_REGION_NEW	    (void)
{
   return gdk_region_new	    ();
}

gboolean       GDK_REGION_POINT_IN  (GdkRegion	   *region,
				     int		   x,
				     int		   y)
{
   return gdk_region_point_in  (region,
				x,
				y);
}

GdkOverlapType GDK_REGION_RECT_IN   (GdkRegion	   *region,
				     GdkRectangle  *rect)
{
   return gdk_region_rect_in   (region,
				rect);
}

GdkRegion*    GDK_REGION_UNION_WITH_RECT  (GdkRegion	  *region,
					   GdkRectangle	  *rect)
{
   return gdk_region_union_with_rect  (region,
				rect);
}

gint GDK_SCREEN_HEIGHT (void)
{
   return gdk_screen_height ();
}

gint GDK_SCREEN_WIDTH  (void)
{
   return gdk_screen_width  ();
}

gint	 GDK_STRING_WIDTH   (GdkFont        *font,
			     const gchar    *string)
{
   return gdk_string_width   (font,
			     string);
}

void     gdk_threads_enter1                (void)
{
   gdk_threads_enter                ();
}

void     gdk_threads_leave1                (void)
{
   gdk_threads_leave                ();
}

GdkVisual*    GDK_VISUAL_GET_BEST	     (void)
{
   return gdk_visual_get_best	     ();
}

GdkVisual*    GDK_VISUAL_GET_SYSTEM	     (void)
{
   return gdk_visual_get_system	     ();
}

void	      GDK_WINDOW_ADD_FILTER	(GdkWindow     *window,
					 GdkFilterFunc	function,
					 gpointer	data)
{
   gdk_window_add_filter	(window,
				function,
				data);
}

void	      GDK_WINDOW_CLEAR	     (GdkWindow	   *window)
{
   gdk_window_clear	     (window);
}

void	      GDK_WINDOW_CLEAR_AREA  (GdkWindow	   *window,
				      gint	    x,
				      gint	    y,
				      gint	    width,
				      gint	    height)
{
   gdk_window_clear_area  (window,
			x,
			y,
			width,
			height);
}

void	      GDK_WINDOW_COPY_AREA   (GdkWindow	   *window,
				      GdkGC	   *gc,
				      gint	    x,
				      gint	    y,
				      GdkWindow	   *source_window,
				      gint	    source_x,
				      gint	    source_y,
				      gint	    width,
				      gint	    height)
{
   gdk_window_copy_area   (window,
			gc,
			x,
			y,
			source_window,
			source_x,
			source_y,
			width,
			height);
}

void	      GDK_WINDOW_DESTROY     (GdkWindow	    *window)
{
   gdk_window_destroy     (window);
}

GdkColormap*  GDK_WINDOW_GET_COLORMAP	 (GdkWindow	  *window)
{
   return gdk_window_get_colormap	 (window);
}

gint	      GDK_WINDOW_GET_ORIGIN	 (GdkWindow	  *window,
					  gint		  *x,
					  gint		  *y)
{
   return gdk_window_get_origin	 (window,
				x,
				y);
}

GdkWindow*    GDK_WINDOW_GET_POINTER	 (GdkWindow	  *window,
					  gint		  *x,
					  gint		  *y,
					  GdkModifierType *mask)
{
   return gdk_window_get_pointer	 (window,
					  x,
					  y,
					  mask);
}

void	      GDK_WINDOW_GET_ROOT_ORIGIN (GdkWindow	  *window,
					  gint		  *x,
					  gint		  *y)
{
   gdk_window_get_root_origin (window,
			x,
			y);
}

void	      GDK_WINDOW_GET_SIZE	 (GdkWindow	  *window,
					  gint		  *width,
					  gint		  *height)
{
   gdk_window_get_size	 (window,
			width,
			height);
}

void	      GDK_WINDOW_GET_USER_DATA	 (GdkWindow	  *window,
					  gpointer	  *data)
{
   gdk_window_get_user_data	 (window,
				data);
}

GdkVisual*    GDK_WINDOW_GET_VISUAL	 (GdkWindow	  *window)
{
   return gdk_window_get_visual	 (window);
}

void	      GDK_WINDOW_LOWER	     (GdkWindow	   *window)
{
   gdk_window_lower	     (window);
}

void	      GDK_WINDOW_MOVE	     (GdkWindow	   *window,
				      gint	    x,
				      gint	    y)
{
   gdk_window_move	     (window,
			x,
			y);
}

void	      GDK_WINDOW_MOVE_RESIZE (GdkWindow	   *window,
				      gint	    x,
				      gint	    y,
				      gint	    width,
				      gint	    height)
{
   gdk_window_move_resize (window,
			x,
			y,
			width,
			height);
}

GdkWindow*    GDK_WINDOW_NEW	     (GdkWindow	    *parent,
				      GdkWindowAttr *attributes,
				      gint	     attributes_mask)
{
   return gdk_window_new	     (parent,
				      attributes,
				      attributes_mask);
}

void	      GDK_WINDOW_RAISE	     (GdkWindow	   *window)
{
   gdk_window_raise	     (window);
}

GdkWindow*    GDK_WINDOW_REF	     (GdkWindow	    *window)
{
   return gdk_window_ref	     (window);
}

void	      GDK_WINDOW_RESIZE	     (GdkWindow	   *window,
				      gint	    width,
				      gint	    height)
{
   gdk_window_resize	     (window,
			width,
			height);
}

void	      GDK_WINDOW_SET_BACKGROUND	 (GdkWindow	  *window,
					  GdkColor	  *color)
{
   gdk_window_set_background	 (window,
				color);
}

void	      GDK_WINDOW_SET_CURSOR	 (GdkWindow	  *window,
					  GdkCursor	  *cursor)
{
   gdk_window_set_cursor	 (window,
				cursor);
}

void	      GDK_WINDOW_SET_DECORATIONS (GdkWindow	  *window,
					  GdkWMDecoration  decorations)
{
   gdk_window_set_decorations (window,
			decorations);
}

void	      GDK_WINDOW_SET_FUNCTIONS	 (GdkWindow	  *window,
					  GdkWMFunction	   functions)
{
   gdk_window_set_functions	 (window,
				functions);
}

void	      GDK_WINDOW_SET_HINTS	 (GdkWindow	  *window,
					  gint		   x,
					  gint		   y,
					  gint		   min_width,
					  gint		   min_height,
					  gint		   max_width,
					  gint		   max_height,
					  gint		   flags)
{
   gdk_window_set_hints	 (window,
			x,
			y,
			min_width,
			min_height,
			max_width,
			max_height,
			flags);
}

void	      GDK_WINDOW_SET_ICON	 (GdkWindow	  *window, 
					  GdkWindow	  *icon_window,
					  GdkPixmap	  *pixmap,
					  GdkBitmap	  *mask)
{
   gdk_window_set_icon	 (window, 
			icon_window,
			pixmap,
			mask);
}

gboolean GDK_WINDOW_SET_STATIC_GRAVITIES (GdkWindow *window,
					  gboolean   use_static)
{
   return gdk_window_set_static_gravities (window,
					  use_static);   
}

void	      GDK_WINDOW_SET_USER_DATA	 (GdkWindow	  *window,
					  gpointer	   user_data)
{
   gdk_window_set_user_data	 (window,
				user_data);
}

void	      GDK_WINDOW_SHOW	     (GdkWindow	   *window)
{
   gdk_window_show	     (window);
}

void	      GDK_WINDOW_UNREF	     (GdkWindow	    *window)
{
   gdk_window_unref	     (window);
}

void
GDK_DRAW_RGB_IMAGE (GdkDrawable *drawable,
		    GdkGC *gc,
		    gint x,
		    gint y,
		    gint width,
		    gint height,
		    GdkRgbDither dith,
		    guchar *rgb_buf,
		    gint rowstride)
{
gdk_draw_rgb_image (drawable,
		    gc,
		    x,
		    y,
		    width,
		    height,
		    dith,
		    rgb_buf,
		    rowstride);
}

void
GDK_RGB_INIT (void)
{
gdk_rgb_init ();
}

gpointer GDK_XID_TABLE_LOOKUP (XID	 xid)
{
   return gdk_xid_table_lookup (xid);
}


Display* GDK_DISPLAY0( void )
{
   return gdk_display;
}

gint GDK_INPUT_ADD	  (gint		     source,
			   GdkInputCondition condition,
			   GdkInputFunction  function,
			   gpointer	     data)
{
   return gdk_input_add	  (source,
			   condition,
			   function,
			   data);
}

GdkWindow* GDK_ROOT_PARENT0( void )
{
   return ((GdkWindow *)&gdk_root_parent);
}

void		GTK_ACCEL_GROUP_ATTACH		(GtkAccelGroup	*accel_group,
						 GtkObject	*object)
{
   gtk_accel_group_attach		(accel_group,
					object);
}

void		GTK_ACCEL_GROUP_DETACH		(GtkAccelGroup	*accel_group,
						 GtkObject	*object)
{
   gtk_accel_group_detach		(accel_group,
					object);
}

GtkAccelGroup*  GTK_ACCEL_GROUP_NEW	      	(void)
{
   return gtk_accel_group_new	      	();
}

GtkType	   GTK_ACCEL_LABEL_GET_TYPE	     (void)
{
   return gtk_accel_label_get_type	     ();
}

gboolean   GTK_ACCEL_LABEL_REFETCH           (GtkAccelLabel *accel_label)
{
   return gtk_accel_label_refetch           (accel_label);
}

GtkType	   GTK_ADJUSTMENT_GET_TYPE		(void)
{
   return gtk_adjustment_get_type		();
}

GtkType  GTK_BIN_GET_TYPE   (void)
{
   return gtk_bin_get_type   ();
}

GtkType	   GTK_BOX_GET_TYPE	       (void)
{
   return gtk_box_get_type	       ();
}

void	   GTK_BOX_PACK_START	       (GtkBox	     *box,
					GtkWidget    *child,
					gboolean      expand,
					gboolean      fill,
					guint	      padding)
{
   gtk_box_pack_start	       (box,
				child,
				expand,
				fill,
				padding);
}

GtkType        GTK_BUTTON_GET_TYPE       (void)
{
   return gtk_button_get_type       ();
}

GtkWidget*     GTK_BUTTON_NEW            (void)
{
   return gtk_button_new            ();
}

GtkWidget*     GTK_BUTTON_NEW_WITH_LABEL (const gchar *label)
{
   return gtk_button_new_with_label (label);
}

void           GTK_BUTTON_SET_RELIEF     (GtkButton *button,
					  GtkReliefStyle newstyle)
{
   gtk_button_set_relief     (button,
			newstyle);
}

GtkWidget* GTK_CHECK_BUTTON_NEW            (void)
{
   return gtk_check_button_new            ();
}

GtkWidget* GTK_CHECK_BUTTON_NEW_WITH_LABEL (const gchar *label)
{
   return gtk_check_button_new_with_label (label);
}

void	   GTK_CHECK_MENU_ITEM_SET_ACTIVE      (GtkCheckMenuItem *check_menu_item,
						gboolean	  is_active)
{
   gtk_check_menu_item_set_active      (check_menu_item,
					is_active);
}

guint      GTK_COMBO_GET_TYPE              (void)
{
   return gtk_combo_get_type              ();
}

GtkWidget *GTK_COMBO_NEW                   (void)
{
   return gtk_combo_new                   ();
}

void       GTK_COMBO_SET_USE_ARROWS_ALWAYS (GtkCombo*    combo, 
                                            gint         val)
{
   gtk_combo_set_use_arrows_always (combo, 
                                    val);
}

void    GTK_CONTAINER_ADD		 (GtkContainer	   *container,
					  GtkWidget	   *widget)
{
   gtk_container_add		 (container,
				widget);
}

gint   GTK_CONTAINER_FOCUS		   (GtkContainer     *container,
					    GtkDirectionType  direction)
{
   return gtk_container_focus		   (container,
					    direction);
}

void    GTK_CONTAINER_FORALL		     (GtkContainer *container,
					      GtkCallback   callback,
					      gpointer	    callback_data)
{
   gtk_container_forall		     (container,
				callback,
				callback_data);
}

GtkType GTK_CONTAINER_GET_TYPE		 (void)
{
   return gtk_container_get_type		 ();
}

void    GTK_CONTAINER_REMOVE		 (GtkContainer	   *container,
					  GtkWidget	   *widget)
{
   gtk_container_remove		 (container,
				widget);
}

void   GTK_CONTAINER_SET_FOCUS_VADJUST (GtkContainer     *container,
					    GtkAdjustment    *adjustment)
{
   gtk_container_set_focus_vadjustment (container,
					adjustment);
}

void GTK_DRAW_SHADOW  (GtkStyle	     *style,
		       GdkWindow     *window,
		       GtkStateType   state_type,
		       GtkShadowType  shadow_type,
		       gint	      x,
		       gint	      y,
		       gint	      width,
		       gint	      height)
{
   gtk_draw_shadow  (style,
		     window,
		     state_type,
		     shadow_type,
		     x,
		     y,
		     width,
		     height);
}

void       GTK_EDITABLE_COPY_CLIPBOARD (GtkEditable      *editable)
{
   gtk_editable_copy_clipboard (editable);
}

void       GTK_EDITABLE_CUT_CLIPBOARD  (GtkEditable      *editable)
{
   gtk_editable_cut_clipboard  (editable);
}

void       GTK_EDITABLE_DELETE_TEXT    (GtkEditable      *editable,
					gint              start_pos,
					gint              end_pos)
{
   gtk_editable_delete_text    (editable,
				start_pos,
				end_pos);
}

gchar*     GTK_EDITABLE_GET_CHARS      (GtkEditable      *editable,
					gint              start_pos,
					gint              end_pos)
{
   return gtk_editable_get_chars      (editable,
				start_pos,
				end_pos);
}

GtkType    GTK_EDITABLE_GET_TYPE       (void)
{
   return gtk_editable_get_type       ();
}

void       GTK_EDITABLE_INSERT_TEXT   (GtkEditable       *editable,
					const gchar      *new_text,
					gint              new_text_length,
					gint             *position)
{
   gtk_editable_insert_text   (editable,
			new_text,
			new_text_length,
			position);
}

void       GTK_EDITABLE_PASTE_CLIPBOARD (GtkEditable     *editable)
{
   gtk_editable_paste_clipboard (editable);
}

void       GTK_EDITABLE_SELECT_REGION  (GtkEditable      *editable,
					gint              start,
					gint              end)
{
   gtk_editable_select_region  (editable,
				start,
				end);
}

void       GTK_ENTRY_APPEND_TEXT    		(GtkEntry      *entry,
						 const gchar   *text)
{
   gtk_entry_append_text    		(entry,
					text);
}

gchar*     GTK_ENTRY_GET_TEXT       		(GtkEntry      *entry)
{
   return gtk_entry_get_text       		(entry);
}

GtkType    GTK_ENTRY_GET_TYPE       		(void)
{
   return gtk_entry_get_type       		();
}

GtkWidget* GTK_ENTRY_NEW            		(void)
{
   return gtk_entry_new            		();
}

void       GTK_ENTRY_SET_EDITABLE   		(GtkEntry      *entry,
						 gboolean       editable)
{
   gtk_entry_set_editable   		(entry,
					editable);
}

void       GTK_ENTRY_SET_POSITION   		(GtkEntry      *entry,
						 gint           position)
{
   gtk_entry_set_position   		(entry,
					position);
}

void       GTK_ENTRY_SET_TEXT       		(GtkEntry      *entry,
						 const gchar   *text)
{
   gtk_entry_set_text       		(entry,
					text);
}

void       GTK_ENTRY_SET_VISIBILITY 		(GtkEntry      *entry,
						 gboolean       visible)
{
   gtk_entry_set_visibility 		(entry,
					visible);
}

gint	   GTK_EVENTS_PENDING	 (void)
{
   return gtk_events_pending	 ();
}

gchar*     GTK_FILE_SELECTION_GET_FILENAME        (GtkFileSelection *filesel)
{
   return gtk_file_selection_get_filename        (filesel);
}

GtkType    GTK_FILE_SELECTION_GET_TYPE            (void)
{
   return gtk_file_selection_get_type            ();
}

void       GTK_FILE_SELECTION_HIDE_FILEOP_ (GtkFileSelection *filesel)
{
   gtk_file_selection_hide_fileop_buttons (filesel);
}

GtkWidget* GTK_FILE_SELECTION_NEW                 (const gchar      *title)
{
   return gtk_file_selection_new                 (title);
}

void       GTK_FILE_SELECTION_SET_FILENAME        (GtkFileSelection *filesel,
						   const gchar      *filename)
{
   gtk_file_selection_set_filename        (filesel,
					filename);
}

gchar*	 GTK_FONT_SELECTION_DIALOG_GETNF    (GtkFontSelectionDialog *fsd)
{
   return gtk_font_selection_dialog_get_font_name    (fsd);
}

GdkFont* GTK_FONT_SELECTION_DIALOG_GET_F	    (GtkFontSelectionDialog *fsd)
{
   return gtk_font_selection_dialog_get_font	    (fsd);
}

GtkType	   GTK_FONT_SELECTION_DIALOG_GET_T	(void)
{
   return gtk_font_selection_dialog_get_type	();
}

GtkWidget* GTK_FONT_SELECTION_DIALOG_NEW	(const gchar	  *title)
{
   return gtk_font_selection_dialog_new	(title);
}

GtkType    GTK_FRAME_GET_TYPE        (void)
{
   return gtk_frame_get_type        ();
}

GtkWidget* GTK_FRAME_NEW             (const gchar   *label)
{
   return gtk_frame_new             (label);
}

void       GTK_FRAME_SET_LABEL       (GtkFrame      *frame,
				      const gchar   *label)
{
   gtk_frame_set_label       (frame,
			label);
}

void	   GTK_GRAB_ADD		   (GtkWidget	       *widget)
{
   gtk_grab_add		   (widget);
}

void	   GTK_GRAB_REMOVE	   (GtkWidget	       *widget)
{
   gtk_grab_remove	   (widget);
}

GtkType     GTK_HANDLE_BOX_GET_TYPE             (void)
{
   return gtk_handle_box_get_type             ();
}

GtkWidget*  GTK_HANDLE_BOX_NEW                  (void)
{
   return gtk_handle_box_new                  ();
}

void        GTK_HANDLE_BOX_SET_SHADOW_TYPE      (GtkHandleBox    *handle_box,
                                                 GtkShadowType    type)
{
   gtk_handle_box_set_shadow_type      (handle_box,
                                        type);
}

GtkWidget* GTK_HBOX_NEW	     (gboolean homogeneous,
			      gint spacing)
{
   return gtk_hbox_new	     (homogeneous,
			      spacing);
}

GtkWidget* GTK_HSCALE_NEW      (GtkAdjustment *adjustment)
{
   return gtk_hscale_new      (adjustment);
}

GtkWidget* GTK_HSCROLLBAR_NEW      (GtkAdjustment *adjustment)
{
   return gtk_hscrollbar_new      (adjustment);
}

GtkWidget* GTK_HSEPARATOR_NEW      (void)
{
   return gtk_hseparator_new      ();
}

guint	   GTK_IDLE_ADD		   (GtkFunction	       function,
				    gpointer	       data)
{
   return gtk_idle_add		   (function,
				    data);
}

void	   GTK_IDLE_REMOVE	   (guint	       idle_handler_id)
{
   gtk_idle_remove	   (idle_handler_id);
}

void	   GTK_INIT		 (int	       *argc,
				  char	     ***argv)
{
   gtk_init		 (argc,
			argv);
}

void	GTK_ITEM_FACTORY_CREATE_ITEM	(GtkItemFactory		*ifactory,
					 GtkItemFactoryEntry	*entry,
					 gpointer		 callback_data,
					 guint			 callback_type)
{
   gtk_item_factory_create_item	(ifactory,
				entry,
				callback_data,
				callback_type);
}

GtkWidget*	GTK_ITEM_FACTORY_GET_ITEM	      (GtkItemFactory *ifactory,
						       const gchar    *path)
{
   return gtk_item_factory_get_item	      (ifactory,
					path);
}

GtkWidget*	GTK_ITEM_FACTORY_GET_WIDGET	      (GtkItemFactory *ifactory,
						       const gchar    *path)
{
   return gtk_item_factory_get_widget	      (ifactory,
					path);
}

GtkItemFactory*	GTK_ITEM_FACTORY_NEW	   (GtkType		 container_type,
					    const gchar		*path,
					    GtkAccelGroup       *accel_group)
{
   return gtk_item_factory_new	   (container_type,
				path,
				accel_group);
}

void       GTK_LABEL_GET           (GtkLabel          *label,
                                    gchar            **str)
{
   gtk_label_get           (label,
                            str);
}

GtkType    GTK_LABEL_GET_TYPE      (void)
{
   return gtk_label_get_type      ();
}

GtkWidget* GTK_LABEL_NEW           (const gchar       *str)
{
   return gtk_label_new           (str);
}

guint      GTK_LABEL_PARSE_ULINE    (GtkLabel         *label,
				     const gchar      *string)
{
   return gtk_label_parse_uline    (label,
				    string);
}

void       GTK_LABEL_SET_JUSTIFY   (GtkLabel          *label,
                                    GtkJustification   jtype)
{
   gtk_label_set_justify   (label,
                            jtype);
}

void	   GTK_LABEL_SET_LINE_WRAP (GtkLabel	      *label,
				    gboolean           wrap)
{
   gtk_label_set_line_wrap (label,
			wrap);
}

void       GTK_LABEL_SET_TEXT      (GtkLabel          *label,
                                    const gchar       *str)
{
   gtk_label_set_text      (label,
                            str);
}

void	   GTK_LIST_APPEND_ITEMS	  (GtkList	    *list,
					   GList	    *items)
{
   gtk_list_append_items	  (list,
				items);
}

void	   GTK_LIST_CLEAR_ITEMS		  (GtkList	    *list,
					   gint		     start,
					   gint		     end)
{
   gtk_list_clear_items		  (list,
				start,
				end);
}

GtkType	   GTK_LIST_GET_TYPE		  (void)
{
   return gtk_list_get_type		  ();
}

void	   GTK_LIST_INSERT_ITEMS	  (GtkList	    *list,
					   GList	    *items,
					   gint		     position)
{
   gtk_list_insert_items	  (list,
				items,
				position);
}

GtkWidget* GTK_LIST_ITEM_NEW_WITH_LABEL (const gchar      *label)
{
   return gtk_list_item_new_with_label (label);
}

GtkWidget* GTK_LIST_NEW			  (void)
{
   return gtk_list_new			  ();
}

void	   GTK_LIST_REMOVE_ITEMS	  (GtkList	    *list,
					   GList	    *items)
{
   gtk_list_remove_items	  (list,
				items);
}

void	   GTK_LIST_SELECT_ITEM		  (GtkList	    *list,
					   gint		     item)
{
   gtk_list_select_item		  (list,
				item);
}

void	   GTK_LIST_SET_SELECTION_MODE	  (GtkList	    *list,
					   GtkSelectionMode  mode)
{
   gtk_list_set_selection_mode	  (list,
				mode);
}

void	   GTK_LIST_UNSELECT_ITEM	  (GtkList	    *list,
					   gint		     item)
{
   gtk_list_unselect_item	  (list,
				item);
}

void	   GTK_MAIN		 (void)
{
   gtk_main		 ();
}

gint	   GTK_MAIN_ITERATION	 (void)
{
   return gtk_main_iteration	 ();
}

guint	   GTK_MAIN_LEVEL	 (void)
{
   return gtk_main_level	 ();
}

void	   GTK_MAIN_QUIT	 (void)
{
   gtk_main_quit	 ();
}

GdkEventMask  GDK_WINDOW_GET_EVENTS	 (GdkWindow	  *window)
{
   return gdk_window_get_events	 (window);
}

void	      GDK_WINDOW_SET_EVENTS	 (GdkWindow	  *window,
					  GdkEventMask	   event_mask)
{
   gdk_window_set_events	 (window,
				event_mask);
}

guint	   GTK_IDLE_ADD_PRIORITY   (gint	       priority,
				    GtkFunction	       function,
				    gpointer	       data)
{
   return gtk_idle_add_priority   (priority,
				   function,
				   data);
}

guint VMS_GTK_MAJOR_VERSION( void )
{
   return gtk_major_version;
}

guint VMS_GTK_MINOR_VERSION( void )
{
   return gtk_minor_version;
}

guint VMS_GTK_MICRO_VERSION( void )
{
   return gtk_micro_version;
}

GtkWidget*
gtk_item_factory_get_item (GtkItemFactory *ifactory,
			   const gchar    *path)
{
  GtkWidget *widget;

  g_return_val_if_fail (GTK_IS_ITEM_FACTORY (ifactory), NULL);
  g_return_val_if_fail (path != NULL, NULL);

  widget = gtk_item_factory_get_widget (ifactory, path);

  if (GTK_IS_MENU (widget))
    widget = gtk_menu_get_attach_widget (GTK_MENU (widget));

  return GTK_IS_ITEM (widget) ? widget : NULL;
}

void GTK_MARSHAL_NONE__POINTER_POINT (GtkObject * object,
					  GtkSignalFunc func,
					  gpointer func_data, GtkArg * args)
{
   gtk_marshal_NONE__POINTER_POINTER (object,
				func,
				func_data, args);
}

void	   GTK_MENU_APPEND		  (GtkMenu	       *menu,
					   GtkWidget	       *child)
{
   gtk_menu_append		  (menu,
				child);
}

GtkType    GTK_MENU_BAR_GET_TYPE        (void)
{
   return gtk_menu_bar_get_type        ();
}

GtkType	   GTK_MENU_GET_TYPE		  (void)
{
   return gtk_menu_get_type		  ();
}

void	   GTK_MENU_INSERT		  (GtkMenu	       *menu,
					   GtkWidget	       *child,
					   gint			position)
{
   gtk_menu_insert		  (menu,
					   child,
					   position);
}

GtkType	   GTK_MENU_ITEM_GET_TYPE	  (void)
{
   return gtk_menu_item_get_type	  ();
}

GtkWidget* GTK_MENU_ITEM_NEW_WITH_LABEL	  (const gchar	       *label)
{
   return gtk_menu_item_new_with_label	  (label);
}

void	   GTK_MENU_ITEM_SET_SUBMENU	  (GtkMenuItem	       *menu_item,
					   GtkWidget	       *submenu)
{
   gtk_menu_item_set_submenu	  (menu_item,
				submenu);
}

GtkWidget* GTK_MENU_NEW			  (void)
{
   return gtk_menu_new			  ();
}

void	   GTK_MENU_POPUP		  (GtkMenu	       *menu,
					   GtkWidget	       *parent_menu_shell,
					   GtkWidget	       *parent_menu_item,
					   GtkMenuPositionFunc	func,
					   gpointer		data,
					   guint		button,
					   guint32		activate_time)
{
   gtk_menu_popup		  (menu,
				parent_menu_shell,
				parent_menu_item,
				func,
				data,
				button,
				activate_time);
}

GtkType GTK_MENU_SHELL_GET_TYPE	  (void)
{
   return gtk_menu_shell_get_type	  ();
}

GtkType GTK_MISC_GET_TYPE      (void)
{
   return gtk_misc_get_type      ();
}

void	GTK_MISC_SET_ALIGNMENT (GtkMisc *misc,
				gfloat	 xalign,
				gfloat	 yalign)
{
   gtk_misc_set_alignment (misc,
			xalign,
			yalign);
}

void GTK_OBJECT_UNREF	  (GtkObject	    *object)
{
   gtk_object_unref	  (object);
}

GtkWidget* GTK_OPTION_MENU_GET_MENU    (GtkOptionMenu *option_menu)
{
   return gtk_option_menu_get_menu    (option_menu);
}

GtkType    GTK_OPTION_MENU_GET_TYPE    (void)
{
   return gtk_option_menu_get_type    ();
}

GtkWidget* GTK_OPTION_MENU_NEW         (void)
{
   return gtk_option_menu_new         ();
}

void       GTK_OPTION_MENU_REMOVE_MENU (GtkOptionMenu *option_menu)
{
   gtk_option_menu_remove_menu (option_menu);
}

void       GTK_OPTION_MENU_SET_HISTORY (GtkOptionMenu *option_menu,
					guint          index)
{
   gtk_option_menu_set_history (option_menu,
				index);
}

void       GTK_OPTION_MENU_SET_MENU    (GtkOptionMenu *option_menu,
					GtkWidget     *menu)
{
   gtk_option_menu_set_menu    (option_menu,
				menu);
}

GtkType	   GTK_PIXMAP_GET_TYPE	 (void)
{
   return gtk_pixmap_get_type	 ();
}

GtkWidget* GTK_PIXMAP_NEW	 (GdkPixmap  *pixmap,
				  GdkBitmap  *mask)
{
   return gtk_pixmap_new	 (pixmap,
				  mask);
}

void	   GTK_PIXMAP_SET	 (GtkPixmap  *pixmap,
				  GdkPixmap  *val,
				  GdkBitmap  *mask)
{
   gtk_pixmap_set	 (pixmap,
			val,
			mask);
}

void       GTK_PIXMAP_SET_BUILD_INSENSITIV (GtkPixmap *pixmap,
		                             guint build)
{
   gtk_pixmap_set_build_insensitive (pixmap,
		                     build);
}

GtkType    GTK_PROGRESS_BAR_GET_TYPE             (void)
{
   return gtk_progress_bar_get_type             ();
}

GtkWidget* GTK_PROGRESS_BAR_NEW                  (void)
{
   return gtk_progress_bar_new                  ();
}

void       GTK_PROGRESS_BAR_UPDATE               (GtkProgressBar *pbar,
						  gfloat          percentage)
{
   gtk_progress_bar_update               (pbar,
					percentage);
}

GtkType	    GTK_RADIO_BUTTON_GET_TYPE	     (void)
{
   return gtk_radio_button_get_type	     ();
}

GSList*	    GTK_RADIO_BUTTON_GROUP	     (GtkRadioButton *radio_button)
{
   return gtk_radio_button_group	     (radio_button);
}

GtkWidget*  GTK_RADIO_BUTTON_NEW_WITH_LABEL  (GSList	     *group,
					      const gchar    *label)
{
   return gtk_radio_button_new_with_label  (group,
					    label);
}

GtkAdjustment* GTK_RANGE_GET_ADJUSTMENT         (GtkRange      *range)
{
   return gtk_range_get_adjustment         (range);
}

GtkType        GTK_RANGE_GET_TYPE               (void)
{
   return gtk_range_get_type               ();
}

GtkStyle* GTK_RC_GET_STYLE		(GtkWidget   *widget)
{
   return gtk_rc_get_style		(widget);
}

GtkType GTK_SCALE_GET_TYPE        (void)
{
   return gtk_scale_get_type        ();
}

void    GTK_SCALE_SET_DIGITS      (GtkScale        *scale,
				   gint             digits)
{
   gtk_scale_set_digits      (scale,
				   digits);
}

void    GTK_SCALE_SET_DRAW_VALUE  (GtkScale        *scale,
				   gboolean         draw_value)
{
   gtk_scale_set_draw_value  (scale,
			draw_value);
}

void	       GTK_SCROLLED_WINDOW_ADD_WITH_VI (GtkScrolledWindow *scrolled_window,
						      GtkWidget		*child)
{
   gtk_scrolled_window_add_with_viewport (scrolled_window,
					child);
}

GtkType        GTK_SCROLLED_WINDOW_GET_TYPE          (void)
{
   return gtk_scrolled_window_get_type          ();
}

GtkAdjustment* GTK_SCROLLED_WINDOW_GET_VADJUST   (GtkScrolledWindow *scrolled_window)
{
   return gtk_scrolled_window_get_vadjustment   (scrolled_window);
}

GtkWidget*     GTK_SCROLLED_WINDOW_NEW               (GtkAdjustment     *hadjustment,
						      GtkAdjustment     *vadjustment)
{
   return gtk_scrolled_window_new               (hadjustment,
						 vadjustment);
}

void           GTK_SCROLLED_WINDOW_SET_POLICY        (GtkScrolledWindow *scrolled_window,
						      GtkPolicyType      hscrollbar_policy,
						      GtkPolicyType      vscrollbar_policy)
{
   gtk_scrolled_window_set_policy        (scrolled_window,
					hscrollbar_policy,
					vscrollbar_policy);
}

gchar*	   GTK_SET_LOCALE	 (void)
{
   return gtk_set_locale	 ();
}

guint  GTK_SIGNAL_CONNECT		  (GtkObject	       *object,
					   const gchar	       *name,
					   GtkSignalFunc	func,
					   gpointer		func_data)
{
   return gtk_signal_connect		  (object,
					   name,
					   func,
					   func_data);
}

guint  GTK_SIGNAL_CONNECT_AFTER		  (GtkObject	       *object,
					   const gchar	       *name,
					   GtkSignalFunc	func,
					   gpointer		func_data)
{
   return gtk_signal_connect_after		  (object,
					   name,
					   func,
					   func_data);
}

void   GTK_SIGNAL_DISCONNECT_BY_FUNC	  (GtkObject	       *object,
					   GtkSignalFunc	func,
					   gpointer		data)
{
   gtk_signal_disconnect_by_func	  (object,
					   func,
					   data);
}

void   GTK_SIGNAL_EMIT_BY_NAME		  (GtkObject	       *object,
					   const gchar	       *name
					   )
{
   gtk_signal_emit_by_name		  (object,
					   name
					  );

}

void   GTK_SIGNAL_EMIT_STOP_BY_NAME	  (GtkObject	       *object,
					   const gchar	       *name)
{
   gtk_signal_emit_stop_by_name	  (object,
				name);
}

#define MAX_SIGNAL_PARAMS		(31)

guint  GTK_SIGNAL_NEW			  (const gchar	       *name,
					   GtkSignalRunType	signal_flags,
					   GtkType		object_type,
					   guint		function_offset,
					   GtkSignalMarshaller	marshaller,
					   GtkType		return_val,
					   guint		nparams,
					   ...)
{
  GtkType *params;
  guint i;
  va_list args;
  guint signal_id;
  
  g_return_val_if_fail (nparams < MAX_SIGNAL_PARAMS, 0);
  
  if (nparams > 0)
    {
      params = g_new (GtkType, nparams);
      
      va_start (args, nparams);
      
      for (i = 0; i < nparams; i++)
	params[i] = va_arg (args, GtkType);
      
      va_end (args);
    }
  else
    params = NULL;
  
  signal_id = gtk_signal_newv (name,
			       signal_flags,
			       object_type,
			       function_offset,
			       marshaller,
			       return_val,
			       nparams,
			       params);
  
  g_free (params);
  
  return signal_id;
}

GtkStyle* GTK_STYLE_ATTACH		     (GtkStyle	    *style,
					      GdkWindow	    *window)
{
   return gtk_style_attach		     (style,
					      window);
}

GtkStyle* GTK_STYLE_COPY		     (GtkStyle	    *style)
{
   return gtk_style_copy		     (style);
}

void	  GTK_STYLE_SET_BACKGROUND	     (GtkStyle	   *style,
					      GdkWindow	   *window,
					      GtkStateType  state_type)
{
   gtk_style_set_background	     (style,
				window,
				state_type);
}

void	  GTK_STYLE_UNREF		     (GtkStyle	   *style)
{
   gtk_style_unref		     (style);
}

void	   GTK_TABLE_ATTACH	      (GtkTable	       *table,
				       GtkWidget       *child,
				       guint		left_attach,
				       guint		right_attach,
				       guint		top_attach,
				       guint		bottom_attach,
				       GtkAttachOptions xoptions,
				       GtkAttachOptions yoptions,
				       guint		xpadding,
				       guint		ypadding)
{
   gtk_table_attach	      (table,
			child,
			left_attach,
			right_attach,
			top_attach,
			bottom_attach,
			xoptions,
			yoptions,
			xpadding,
			ypadding);
}

GtkType	   GTK_TABLE_GET_TYPE	      (void)
{
   return gtk_table_get_type	      ();
}

GtkWidget* GTK_TABLE_NEW	      (guint		rows,
				       guint		columns,
				       gboolean		homogeneous)
{
   return gtk_table_new	      (rows,
			columns,
			homogeneous);
}

guint      GTK_TEXT_GET_LENGTH      (GtkText       *text)
{
   return gtk_text_get_length      (text);
}

guint      GTK_TEXT_GET_POINT       (GtkText       *text)
{
   return gtk_text_get_point       (text);
}

GtkType    GTK_TEXT_GET_TYPE        (void)
{
   return gtk_text_get_type        ();
}

void       GTK_TEXT_INSERT          (GtkText       *text,
				     GdkFont       *font,
				     GdkColor      *fore,
				     GdkColor      *back,
				     const char    *chars,
				     gint           length)
{
   gtk_text_insert          (text,
			font,
			fore,
			back,
			chars,
			length);
}

GtkWidget* GTK_TEXT_NEW             (GtkAdjustment *hadj,
				     GtkAdjustment *vadj)
{
   return gtk_text_new             (hadj,
				    vadj);
}

void       GTK_TEXT_SET_EDITABLE    (GtkText       *text,
				     gboolean       editable)
{
   gtk_text_set_editable    (text,
			editable);
}

void       GTK_TEXT_SET_WORD_WRAP   (GtkText       *text,
				     gint           word_wrap)
{
   gtk_text_set_word_wrap   (text,
			word_wrap);
}

guint	   GTK_TIMEOUT_ADD	   (guint32	       interval,
				    GtkFunction	       function,
				    gpointer	       data)
{
   return gtk_timeout_add	   (interval,
				    function,
				    data);
}

void	   GTK_TIMEOUT_REMOVE	   (guint	       timeout_handler_id)
{
   gtk_timeout_remove	   (timeout_handler_id);
}

GtkType    GTK_TOGGLE_BUTTON_GET_TYPE       (void)
{
   return gtk_toggle_button_get_type       ();
}

void       GTK_TOGGLE_BUTTON_SET_ACTIVE     (GtkToggleButton *toggle_button,
					     gboolean         is_active)
{
   gtk_toggle_button_set_active     (toggle_button,
				is_active);
}

void       GTK_TOOLBAR_APPEND_SPACE    (GtkToolbar      *toolbar)
{
   gtk_toolbar_append_space    (toolbar);
}

GtkType    GTK_TOOLBAR_GET_TYPE        (void)
{
   return gtk_toolbar_get_type        ();
}

GtkWidget* GTK_TOOLBAR_INSERT_ELEMENT  (GtkToolbar      *toolbar,
					GtkToolbarChildType type,
					GtkWidget       *widget,
					const char      *text,
					const char      *tooltip_text,
					const char      *tooltip_private_text,
					GtkWidget       *icon,
					GtkSignalFunc    callback,
					gpointer         user_data,
					gint             position)
{
   return gtk_toolbar_insert_element  (toolbar,
				type,
				widget,
				text,
				tooltip_text,
				tooltip_private_text,
				icon,
				callback,
				user_data,
				position);
}

void       GTK_TOOLBAR_INSERT_WIDGET   (GtkToolbar      *toolbar,
					GtkWidget       *widget,
					const char      *tooltip_text,
					const char      *tooltip_private_text,
					gint             position)
{
   gtk_toolbar_insert_widget   (toolbar,
				widget,
				tooltip_text,
				tooltip_private_text,
				position);


}

GtkWidget* GTK_TOOLBAR_NEW             (GtkOrientation   orientation,
					GtkToolbarStyle  style)
{
   return gtk_toolbar_new             (orientation,
				style);
}

void       GTK_TOOLBAR_SET_BUTTON_RELIEF     (GtkToolbar           *toolbar,
					      GtkReliefStyle        relief)
{
   gtk_toolbar_set_button_relief     (toolbar,
				relief);
}

void       GTK_TOOLBAR_SET_SPACE_SIZE        (GtkToolbar           *toolbar,
					      gint                  space_size)
{
   gtk_toolbar_set_space_size        (toolbar,
				space_size);
}

void       GTK_TOOLBAR_SET_TOOLTIPS          (GtkToolbar           *toolbar,
					      gint                  enable)
{
   gtk_toolbar_set_tooltips          (toolbar,
				enable);
}

void             GTK_TOOLTIPS_FORCE_WINDOW (GtkTooltips   *tooltips)
{
   gtk_tooltips_force_window (tooltips);
}

GtkTypeClass*	GTK_TYPE_CHECK_CLASS_CAST	(GtkTypeClass	*klass,
						 GtkType         cast_type)
{
   return gtk_type_check_class_cast	(klass,
					cast_type);
}

GtkTypeObject*	GTK_TYPE_CHECK_OBJECT_CAST	(GtkTypeObject	*type_object,
						 GtkType         cast_type)
{
   return gtk_type_check_object_cast	(type_object,
					cast_type);
}

gpointer	GTK_TYPE_CLASS			(GtkType	 type)
{
   return gtk_type_class			(type);
}

gboolean	GTK_TYPE_IS_A			(GtkType	 type,
						 GtkType	 is_a_type)
{
   return gtk_type_is_a			(type,
					is_a_type);
}

gpointer	GTK_TYPE_NEW			(GtkType	 type)
{
   return gtk_type_new			(type);
}

GtkType		GTK_TYPE_UNIQUE			(GtkType	   parent_type,
						 const GtkTypeInfo *type_info)
{
   return gtk_type_unique			(parent_type,
						 type_info);
}

GtkWidget* GTK_VSCALE_NEW      (GtkAdjustment *adjustment)
{
   return gtk_vscale_new      (adjustment);
}

GtkWidget* GTK_VSCROLLBAR_NEW      (GtkAdjustment *adjustment)
{
   return gtk_vscrollbar_new      (adjustment);
}

GtkWidget* GTK_VSEPARATOR_NEW      (void)
{
   return gtk_vseparator_new      ();
}

gboolean   GTK_WIDGET_ACTIVATE		     (GtkWidget	       *widget)
{
   return gtk_widget_activate		     (widget);
}

void	   GTK_WIDGET_DESTROY		  (GtkWidget	       *widget)
{
   gtk_widget_destroy		  (widget);
}

void	   GTK_WIDGET_DRAW		  (GtkWidget	       *widget,
					   GdkRectangle	       *area)
{
   gtk_widget_draw		  (widget,
				area);
}

gint	   GTK_WIDGET_EVENT		  (GtkWidget	       *widget,
					   GdkEvent	       *event)
{
   return gtk_widget_event		  (widget,
					   event);
}

void       GTK_WIDGET_GET_CHILD_REQUISITIO (GtkWidget	       *widget,
					     GtkRequisition    *requisition)
{
   gtk_widget_get_child_requisition (widget,
				requisition);
}

GdkColormap* GTK_WIDGET_GET_COLORMAP	(GtkWidget	*widget)
{
   return gtk_widget_get_colormap	(widget);
}

GdkColormap* GTK_WIDGET_GET_DEFAULT_COLORMAP (void)
{
   return gtk_widget_get_default_colormap ();
}

GtkStyle*    GTK_WIDGET_GET_DEFAULT_STYLE    (void)
{
   return gtk_widget_get_default_style    ();
}

gint	     GTK_WIDGET_GET_EVENTS	(GtkWidget	*widget)
{
   return gtk_widget_get_events	(widget);
}

GdkWindow *GTK_WIDGET_GET_PARENT_WINDOW	  (GtkWidget	       *widget)
{
   return gtk_widget_get_parent_window	  (widget);
}

GtkStyle*  GTK_WIDGET_GET_STYLE		(GtkWidget	*widget)
{
   return gtk_widget_get_style		(widget);
}

GtkType	   GTK_WIDGET_GET_TYPE		  (void)
{
   return gtk_widget_get_type		  ();
}

GdkVisual*   GTK_WIDGET_GET_VISUAL	(GtkWidget	*widget)
{
   return gtk_widget_get_visual	(widget);
}

void	   GTK_WIDGET_GRAB_DEFAULT	  (GtkWidget	       *widget)
{
   gtk_widget_grab_default	  (widget);
}

void	   GTK_WIDGET_GRAB_FOCUS	  (GtkWidget	       *widget)
{
   gtk_widget_grab_focus	  (widget);
}

void	   GTK_WIDGET_HIDE		  (GtkWidget	       *widget)
{
   gtk_widget_hide		  (widget);
}

gint	   GTK_WIDGET_INTERSECT		  (GtkWidget	       *widget,
					   GdkRectangle	       *area,
					   GdkRectangle	       *intersection)
{
   return gtk_widget_intersect		  (widget,
					   area,
					   intersection);
}

void	   GTK_WIDGET_MAP		  (GtkWidget	       *widget)
{
   gtk_widget_map		  (widget);
}

void	   GTK_WIDGET_QUEUE_DRAW	  (GtkWidget	       *widget)
{
   gtk_widget_queue_draw	  (widget);
}

void	   GTK_WIDGET_QUEUE_RESIZE	  (GtkWidget	       *widget)
{
   gtk_widget_queue_resize	  (widget);
}

void	   GTK_WIDGET_REALIZE		  (GtkWidget	       *widget)
{
   gtk_widget_realize		  (widget);
}

void	   GTK_WIDGET_REF		  (GtkWidget	       *widget)
{
   gtk_widget_ref		  (widget);
}

void	   GTK_WIDGET_REPARENT		  (GtkWidget	       *widget,
					   GtkWidget	       *new_parent)
{
   gtk_widget_reparent		  (widget,
				new_parent);
}

void	     GTK_WIDGET_SET_DEFAULT_COLORMAP (GdkColormap *colormap)
{
   gtk_widget_set_default_colormap (colormap);
}

void	     GTK_WIDGET_SET_DEFAULT_VISUAL   (GdkVisual	  *visual)
{
   gtk_widget_set_default_visual   (visual);
}

void	   GTK_WIDGET_SET_PARENT	  (GtkWidget	       *widget,
					   GtkWidget	       *parent)
{
   gtk_widget_set_parent	  (widget,
				parent);
}

void	   GTK_WIDGET_SET_PARENT_WINDOW	  (GtkWidget	       *widget,
					   GdkWindow	       *parent_window)
{
   gtk_widget_set_parent_window	  (widget,
				parent_window);
}

void	   GTK_WIDGET_SET_SENSITIVE	  (GtkWidget	       *widget,
					   gboolean		sensitive)
{
   gtk_widget_set_sensitive	  (widget,
				sensitive);
}

void	   GTK_WIDGET_SET_STYLE		(GtkWidget	*widget,
					 GtkStyle	*style)
{
   gtk_widget_set_style		(widget,
				style);
}

void	   GTK_WIDGET_SET_UPOSITION	  (GtkWidget	       *widget,
					   gint			x,
					   gint			y)
{
   gtk_widget_set_uposition	  (widget,
				x,
				y);
}

void	   GTK_WIDGET_SET_USIZE		  (GtkWidget	       *widget,
					   gint			width,
					   gint			height)
{
   gtk_widget_set_usize		  (widget,
				width,
				height);
}

void	   GTK_WIDGET_SHOW		  (GtkWidget	       *widget)
{
   gtk_widget_show		  (widget);
}

void	   GTK_WIDGET_SIZE_ALLOCATE	  (GtkWidget	       *widget,
					   GtkAllocation       *allocation)
{
   gtk_widget_size_allocate	  (widget,
				allocation);
}

void	   GTK_WIDGET_SIZE_REQUEST	  (GtkWidget	       *widget,
					   GtkRequisition      *requisition)
{
   gtk_widget_size_request	  (widget,
				requisition);
}

void	   GTK_WIDGET_UNMAP		  (GtkWidget	       *widget)
{
   gtk_widget_unmap		  (widget);
}

void	   GTK_WIDGET_UNPARENT		  (GtkWidget	       *widget)
{
   gtk_widget_unparent		  (widget);
}

void	   GTK_WIDGET_UNREF		  (GtkWidget	       *widget)
{
   gtk_widget_unref		  (widget);
}

GtkType    GTK_WINDOW_GET_TYPE                 (void)
{
   return gtk_window_get_type                 ();
}

GtkWidget* GTK_WINDOW_NEW                      (GtkWindowType        type)
{
   return gtk_window_new                      (type);
}

void       GTK_WINDOW_SET_POLICY               (GtkWindow           *window,
						gint                 allow_shrink,
						gint                 allow_grow,
						gint                 auto_shrink)
{
   gtk_window_set_policy               (window,
					allow_shrink,
					allow_grow,
					auto_shrink);
}

void       GTK_WINDOW_SET_TITLE                (GtkWindow           *window,
						const gchar         *title)
{
   gtk_window_set_title                (window,
					title);
}

void       GTK_WINDOW_SET_TRANSIENT_FOR        (GtkWindow           *window, 
						GtkWindow           *parent)
{
   gtk_window_set_transient_for        (window, 
					parent);
}

void       GTK_WINDOW_SET_WMCLASS              (GtkWindow           *window,
						const gchar         *wmclass_name,
						const gchar         *wmclass_class)
{
   gtk_window_set_wmclass              (window,
					wmclass_name,
					wmclass_class);
}

void	 G_FREE	       (gpointer  mem)
{
   g_free	       (mem);
}

GList* G_LIST_ALLOC		(void)
{
   return g_list_alloc		();
}

GList* G_LIST_APPEND		(GList		*list,
				 gpointer	 data)
{
   return g_list_append		(list,
				 data);
}

void   G_LIST_FREE		(GList		*list)
{
   g_list_free		(list);
}

GList* G_LIST_INSERT		(GList		*list,
				 gpointer	 data,
				 gint		 position)
{
   return g_list_insert		(list,
				 data,
				 position);
}

GList* G_LIST_LAST		(GList		*list)
{
   return g_list_last		(list);
}

guint  G_LIST_LENGTH		(GList		*list)
{
   return g_list_length		(list);
}

GList* G_LIST_NTH		(GList		*list,
				 guint		 n)
{
   return g_list_nth		(list,
				 n);
}

GList* G_LIST_REMOVE		(GList		*list,
				 gpointer	 data)
{
   return g_list_remove		(list,
				 data);
}

GList* G_LIST_REMOVE_LINK	(GList		*list,
				 GList		*llink)
{
   return g_list_remove_link	(list,
				 llink);
}

void		G_LOG			(const gchar	*log_domain,
					 GLogLevelFlags	 log_level,
					 const gchar	*format,
					 ...)
{
  va_list args;
  
  va_start (args, format);
  g_logv (log_domain, log_level, format, args);
  va_end (args);
}

void		G_LOGV			(const gchar	*log_domain,
					 GLogLevelFlags	 log_level,
					 const gchar	*format,
					 va_list	 args)
{
   g_logv			(log_domain,
				log_level,
				format,
				args);
}

gpointer G_MALLOC      (gulong	  size)
{
   return g_malloc      (size);
}

GSList* G_SLIST_FIND		(GSList		*list,
				 gpointer	 data)
{
   return g_slist_find		(list,
				 data);
}

void       GTK_WINDOW_SET_GEOMETRY_HINTS       (GtkWindow           *window,
						GtkWidget           *geometry_widget,
						GdkGeometry         *geometry,
						GdkWindowHints       geom_mask)
{
   gtk_window_set_geometry_hints       (window,
					geometry_widget,
					geometry,
					geom_mask);
}

#undef GDK_THREADS_ENTER
void     GDK_THREADS_ENTER                (void)
{
   gdk_threads_enter1                ();
}

#undef GDK_THREADS_LEAVE
void     GDK_THREADS_LEAVE                (void)
{
   gdk_threads_leave1                ();
}
