#include <gdk/gdk.h>

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

