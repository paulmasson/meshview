#define INIT_C
#include "all.h"

void load_palette() {
  int c[3];
  int i, j;
  double dc[3];
  int E0, DE, COLORSIZE;
  int C[6][3], D[5];

  COLORSIZE = 256;
  DE = COLORSIZE / 8;

  C[0][0] = 0; C[0][1] = 0; C[0][2] = 128; D[0] = DE;
  C[1][0] = 0; C[1][1] = 0; C[1][2] = 255; D[1] = 2*DE;
  C[2][0] = 0; C[2][1] = 255; C[2][2] = 255; D[2] = 2*DE;
  C[3][0] = 255; C[3][1] = 255; C[3][2] = 0; D[3] = 2*DE;
  C[4][0] = 255; C[4][1] = 0; C[4][2] = 0; D[4] = DE;
  C[5][0] = 128; C[5][1] = 0; C[5][2] = 0; 

  E0 = 0;
  for(j=0; j<5; j++) {
    dc[0] = (double) (C[j+1][0]-C[j][0]) / (double) D[j];
    dc[1] = (double) (C[j+1][1]-C[j][1]) / (double) D[j];
    dc[2] = (double) (C[j+1][2]-C[j][2]) / (double) D[j];
    for(i=E0; i<E0+D[j]; i++) {
      c[0] = C[j][0] + (i-E0) * dc[0];
      c[1] = C[j][1] + (i-E0) * dc[1];
      c[2] = C[j][2] + (i-E0) * dc[2];
      palette[i][0] = (float) c[0] / 255.0;
      palette[i][1] = (float) c[1] / 255.0;
      palette[i][2] = (float) c[2] / 255.0;
      palette[i][3] = 1.0;
    }
    E0 += D[j];
  }  
}

void globalVarInit() {
  int i;

  olist = (Obj **) malloc(sizeof(Obj *) * MAXOBJ);
  /* make identity matrix */
  for(i=0; i<16; i++) {
    if(i%5 == 0)
      IMat4.d[i] = 1.0;
    else 
      IMat4.d[i] = 0.0;
  }
  mat4 = IMat4;

  nobjs = 0;

  momentum = OFF;
  momentum_mode = ON;

  contextf_flag = ON;
  
  frames_transform_flag = OFF;
  frames_mode = OFF;

  picker_flag = OFF;

  adjust_size_mode = ON;

  stereo_mode = OFF;

  /* set default background */
  BGred = oldBGred = defaultBGred = 0.23; 
  BGgreen = oldBGgreen = defaultBGgreen = 0.5; 
  BGblue = oldBGblue = defaultBGblue = 0.65; 
  BGalpha = 1.0;

  /* set light matrices (ambient, diffuse, specular) */
  mata[0] = 0.2; mata[1] = 0.2; mata[2] = 0.2; mata[3] = 0.2;
  matd[0] = 0.0; matd[1] = 0.7; matd[2] = 0.3; matd[3] = 1.0;
  mats[0] = 1.0; mats[1] = 1.0; mats[2] = 1.0; mats[3] = 1.0;
  
  /* set light position and color */
  ltp[0] = 0.0; ltp[1] = 0.1; ltp[2] = 1.0; ltp[3] = 0.0;
  ltc[0] = 1.0; ltc[1] = 1.0; ltc[2] = 1.0; ltc[3] = 1.0;

  /* set default object color (front and back) */
  c_default_front[0] = 0.2;
  c_default_front[1] = 0.8;
  c_default_front[2] = 0.2;
  c_default_front[3] = 1.0;
  c_default_back[0] = 1.0 - c_default_front[0];
  c_default_back[1] = 1.0 - c_default_front[1];
  c_default_back[2] = 1.0 - c_default_front[2];
  c_default_back[3] = 1.0;

  /* make different colors */
  black[0] =   0.0; black[1] =   0.0; black[2] =   0.0; black[3] =   1.0;
  white[0] =   1.0; white[1] =   1.0; white[2] =   1.0; white[3] =   1.0;
  red[0] =     1.0; red[1] =     0.0; red[2] =     0.0; red[3] =     1.0;
  green[0] =   0.0; green[1] =   1.0; green[2] =   0.0; green[3] =   1.0;
  blue[0] =    0.0; blue[1] =    0.0; blue[2] =    1.0; blue[3] =    1.0;
  yellow[0] =  1.0; yellow[1] =  1.0; yellow[2] =  0.0; yellow[3] =  1.0;
  cyan[0] =    0.0; cyan[1] =    1.0; cyan[2] =    1.0; cyan[3] =    1.0;
  magenta[0] = 1.0; magenta[1] = 0.0; magenta[2] = 1.0; magenta[3] = 1.0;

  /* make identity GL matrix */
  for(i=0; i<16; i++) {
    if(i%5 == 0)
      IMatGL.d[i] = 1.0;
    else 
      IMatGL.d[i] = 0.0;
  }
  d_rot_mat = IMatGL;
  rot_mat = IMatGL;
  trans_mat = IMatGL;

  /* set initial condition and default values */
  mouse3d_flag = OFF;
  mouse3d_reset_flag = ON;

  draw_face_flag = ON;
  draw_normal_flag = OFF;
  draw_face_flat_flag = OFF;
  draw_edge_flag = OFF;
  draw_vertex_flag = OFF;
  draw_palette_flag = OFF;
  draw_lighting_flag = OFF;
  draw_axis_flag = OFF;

  draw_color_mode = SAME;
  draw_shade_mode = GL_SMOOTH;

  scr_door_mode = SD_OFF;

  proj3_mode = PERSPECTIVE;
  proj4_mode = W;
  proj4_type = ORTHOGONAL;
  redraw_flag = ON;

  near = oldnear = defaultnear = 2.0;
  far = oldfar = defaultfar = 15.0;
  fdist = oldfdist = defaultfdist = 10.0;
  eye = oldeye = defaulteye = 6.20;
  scale = oldscale = defaultscale = 1.0;
  specular = oldspecular = defaultspecular = 0.2;
  shininess = oldshininess = defaultshininess = 30.0;
  m_sense = oldm_sense = defaultm_sense = 2.0;
  m_speed = oldm_speed = defaultm_speed = 1.0;
  polard = oldpolard = defaultpolard = 0.1;
  vcubesize = oldvcubesize = defaultvcubesize = 1.0;
  scrdoorgap = oldscrdoorgap = defaultscrdoorgap = 0.35;
  img_scale = 1.3;
  cubesize = 1.0;
  sph_rows = oldsph_rows = defaultsph_rows = 16;
  sph_size = oldsph_size = defaultsph_size = 1.01;
  load_palette();
}

/* put the objects into the unit 3-sphere */
void setup() {
  int i, j, k, l,  nv;
  Obj *obj, *objf;
  Point4 p, min, max, center;
  double r2, scale;

  /* first find center */
  for(i=0; i<4; i++) {
    min.d[i] = 1.e10;
    max.d[i] = -1.e10;
  }
  for(k=0; k<nobjs; k++) {
    obj = olist[k];
    nv = obj->nvertices;
    for(j=0; j<nv; j++) {
      p = obj->vlist[j].p4;
      for(i=0; i<4; i++) {
	min.d[i] = min(min.d[i], p.d[i]);
	max.d[i] = max(max.d[i], p.d[i]);
      }
    }
      
    if(frames_mode == ON) {
      for(l=0; l<obj->nframes; l++) {
	objf = obj->frames[l];
	nv = objf->nvertices;
	for(j=0; j<nv; j++) {
	  p = objf->vlist[j].p4;
	  for(i=0; i<4; i++) {
	    min.d[i] = min(min.d[i], p.d[i]);
	    max.d[i] = max(max.d[i], p.d[i]);
	  }
	}
      }
    }
  }
  for(i=0; i<4; i++)
    center.d[i] = (max.d[i] + min.d[i]) * 0.5;

  /* set center for the object */
  for(k=0; k<nobjs; k++) {
    olist[k]->center = center;
    if(frames_mode == ON) {
      for(l=0; l<olist[k]->nframes; l++)
	olist[k]->frames[l]->center = center;
    }
  }
  
  /* find the radius of bounding sphere (3D or 4D) */
  r2 = 0.0;
  for(k=0; k<nobjs; k++) {
    obj = olist[k];
    nv = obj->nvertices;
    for(j=0; j<nv; j++) {
      p = obj->vlist[j].p4;
      p.d[0] -= center.d[0];      p.d[1] -= center.d[1];
      p.d[2] -= center.d[2];      p.d[3] -= center.d[3];
      r2 = max(r2, p.d[0]*p.d[0]+p.d[1]*p.d[1]+p.d[2]*p.d[2]+p.d[3]*p.d[3]);
    }
      
    if(frames_mode == ON) {
      for(l=0; l<obj->nframes; l++) {
	objf = obj->frames[l];
	nv = objf->nvertices;
	for(j=0; j<nv; j++) {
	  p = objf->vlist[j].p4;
	  p.d[0] -= center.d[0];      p.d[1] -= center.d[1];
	  p.d[2] -= center.d[2];      p.d[3] -= center.d[3];
	  r2 = max(r2, p.d[0]*p.d[0]+p.d[1]*p.d[1]+p.d[2]*p.d[2]+p.d[3]*p.d[3]);
	}
      }
    }
  }
  /* set scale for the object */
  scale = 1.0 / sqrt(r2);

  for(k=0; k<nobjs; k++) {
    olist[k]->scale = scale;
    if(frames_mode == ON) {
      for(l=0; l<olist[k]->nframes; l++)
	olist[k]->frames[l]->scale = scale;
    }
  }
  
  myLoadProjection();
  glMatrixMode(GL_MODELVIEW);
}
