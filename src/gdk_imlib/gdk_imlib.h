
#ifndef __GDK_IMLIB_H__
#define __GDK_IMLIB_H__

/* we need this funny include path, because the working directory when     */
/* including this might actually be ../gdk/ instead of .                   */
#include "../gdk_imlib/gdk_imlib_types.h"

#ifdef __cplusplus
extern              "C"
{
#endif				/* __cplusplus */

   void                gdk_imlib_init(void);
   void                gdk_imlib_init_params(GdkImlibInitParams * p);
   gint                gdk_imlib_get_render_type(void);
   void                gdk_imlib_set_render_type(gint rend_type);
   gint                gdk_imlib_load_colors(char *file);
   GdkImlibImage      *gdk_imlib_load_image(char *file);
   gint                gdk_imlib_best_color_match(gint * r, gint * g, gint * b);
   gint                gdk_imlib_render(GdkImlibImage * image, gint width, gint height);
   GdkPixmap          *gdk_imlib_copy_image(GdkImlibImage * image);
   GdkBitmap          *gdk_imlib_copy_mask(GdkImlibImage * image);
   GdkPixmap          *gdk_imlib_move_image(GdkImlibImage * image);
   GdkBitmap          *gdk_imlib_move_mask(GdkImlibImage * image);
   void                gdk_imlib_destroy_image(GdkImlibImage * image);
   void                gdk_imlib_kill_image(GdkImlibImage * image);
   void                gdk_imlib_free_colors(void);
   void                gdk_imlib_free_pixmap(GdkPixmap * pixmap);
   void                gdk_imlib_free_bitmap(GdkBitmap * bitmap);
   void                gdk_imlib_get_image_border(GdkImlibImage * image, GdkImlibBorder * border);
   void                gdk_imlib_set_image_border(GdkImlibImage * image, GdkImlibBorder * border);
   void                gdk_imlib_get_image_shape(GdkImlibImage * image, GdkImlibColor * color);
   void                gdk_imlib_set_image_shape(GdkImlibImage * image, GdkImlibColor * color);
   gint                gdk_imlib_save_image_to_eim(GdkImlibImage * image, char *file);
   gint                gdk_imlib_add_image_to_eim(GdkImlibImage * image, char *file);
   gint                gdk_imlib_save_image_to_ppm(GdkImlibImage * image, char *file);
   gint                gdk_imlib_load_file_to_pixmap(char *filename, GdkPixmap ** pmap, GdkBitmap ** mask);
   void                gdk_imlib_set_image_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_set_image_red_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_set_image_green_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_set_image_blue_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_get_image_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_get_image_red_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_get_image_green_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_get_image_blue_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod);
   void                gdk_imlib_set_image_red_curve(GdkImlibImage * im, unsigned char *mod);
   void                gdk_imlib_set_image_green_curve(GdkImlibImage * im, unsigned char *mod);
   void                gdk_imlib_set_image_blue_curve(GdkImlibImage * im, unsigned char *mod);
   void                gdk_imlib_get_image_red_curve(GdkImlibImage * im, unsigned char *mod);
   void                gdk_imlib_get_image_green_curve(GdkImlibImage * im, unsigned char *mod);
   void                gdk_imlib_get_image_blue_curve(GdkImlibImage * im, unsigned char *mod);
   void                gdk_imlib_apply_modifiers_to_rgb(GdkImlibImage * im);
   void                gdk_imlib_changed_image(GdkImlibImage * im);
   void                gdk_imlib_apply_image(GdkImlibImage * im, GdkWindow * p);
   void                gdk_imlib_paste_image(GdkImlibImage * im, GdkWindow * p, gint x, gint y, gint w, gint h);
   void                gdk_imlib_paste_image_border(GdkImlibImage * im, GdkWindow * p, gint x, gint y, gint w, gint h);
   void                gdk_imlib_flip_image_horizontal(GdkImlibImage * im);
   void                gdk_imlib_flip_image_vertical(GdkImlibImage * im);
   void                gdk_imlib_rotate_image(GdkImlibImage * im, gint d);
   GdkImlibImage      *gdk_imlib_create_image_from_data(unsigned char *data, unsigned char *alpha, gint w, gint h);
   GdkImlibImage      *gdk_imlib_clone_image(GdkImlibImage * im);
   GdkImlibImage      *gdk_imlib_clone_scaled_image(GdkImlibImage * im, int w, int h);
   gint                gdk_imlib_get_fallback(void);
   void                gdk_imlib_set_fallback(gint fallback);
   GdkVisual          *gdk_imlib_get_visual(void);
   GdkColormap        *gdk_imlib_get_colormap(void);
   gchar              *gdk_imlib_get_sysconfig(void);
   GdkImlibImage      *gdk_imlib_create_image_from_xpm_data(char **data);
   gint                gdk_imlib_data_to_pixmap(char **data, GdkPixmap ** pmap, GdkBitmap ** mask);
   void                gdk_imlib_crop_image(GdkImlibImage * im, gint x, gint y, gint w, gint h);
   gint                gdk_imlib_save_image(GdkImlibImage * im, char *file, GdkImlibSaveInfo * info);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif
