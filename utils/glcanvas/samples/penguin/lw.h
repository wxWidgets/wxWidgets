/*
 * Copyright (C) 1998 Janne Löf <jlof@mail.student.oulu.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef LW_H
#define LW_H

#include <GL/gl.h>

#define LW_MAX_POINTS   200
#define LW_MAX_NAME_LEN 500

typedef struct {
  char name[LW_MAX_NAME_LEN];
  GLfloat r,g,b;
} lwMaterial;

typedef struct {
  int material;         /* material of this face */
  int index_cnt;        /* number of vertices */
  int *index;           /* index to vertex */
  float *texcoord;      /* u,v texture coordinates */
} lwFace;

typedef struct {
  int face_cnt;
  lwFace *face;

  int material_cnt;
  lwMaterial *material;

  int vertex_cnt;
  GLfloat *vertex;

} lwObject;

#ifdef __cplusplus
extern "C" {
#endif

int       lw_is_lwobject(const char     *lw_file);
lwObject *lw_object_read(const char     *lw_file);
void      lw_object_free(      lwObject *lw_object);
void      lw_object_show(const lwObject *lw_object);

GLfloat   lw_object_radius(const lwObject *lw_object);
void      lw_object_scale (lwObject *lw_object, GLfloat scale);

#ifdef __cplusplus
}
#endif

#endif /* LW_H */

