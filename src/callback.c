/* Motif callback functions */

#define CALLBACK_C
#include "all.h"

/* ARGSUSED */
void bg_color(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  int flag = (int) client_data;
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  switch (flag) {
  case RED :
    BGred = (float) cbs->value / 255.0;
    break;
  case GREEN :
    BGgreen = (float) cbs->value / 255.0;
    break;
  case BLUE :
    BGblue = (float) cbs->value / 255.0;
    break;
  }
  draw_all();
}

/* ARGSUSED */
void new_proj(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  int flag = (int) client_data;
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  switch (flag) {
  case FDIST:
    fdist =  (double) cbs->value * 0.1;
    if(fdist - near < 1.0) {
      near = max(0.1, fdist - 1.0);
      XtVaSetValues(nearScale, XmNvalue, (int) (near*10.0), NULL);
    } else if(far - fdist < 1.0) {
      far = min(40.0, fdist + 1.0);
      XtVaSetValues(farScale, XmNvalue, (int) (far*10.0), NULL);
    }
    break;
  case NEAR:
    near = (double) cbs->value * 0.1;
    if(far - near < 0.15) { /* to eliminate rounding in comparisson */
      far = near + 0.1;
      XtVaSetValues(farScale, XmNvalue, (int) (far*10.0), NULL);
    }
    break;
  case FAR:
    far = (double) cbs->value * 0.1;
    if(far - near < 0.15) {
      near = far - 0.1;
      XtVaSetValues(nearScale, XmNvalue, (int) (near*10.0), NULL);
    }
    break;
  case EYE:
    eye = (double) cbs->value * 0.1;
    break;
 }
  myLoadProjection();
  glMatrixMode(GL_MODELVIEW);
  draw_all();
}  

/* ARGSUSED */
void new_scale(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  double cur_scale;
  
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;
  cur_scale = cbs->value * 0.1;

  if(fabs(scale - cur_scale) > EPSILON) {
    scale = cur_scale;
    From4DTo3D();
    AutoSmooth();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(trans_mat.d);
    glMultMatrixd(rot_mat.d);
    redraw_flag = ON;
    draw_all();
  }
}

/* ARGSUSED */
void new_lighting(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  float s[4];
  int flag = (int) client_data;
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  switch (flag) {
  case SPECULAR :
    specular = (double) cbs->value * 0.01;
    s[0] = specular * mats[0];
    s[1] = specular * mats[1];
    s[2] = specular * mats[2];
    s[3] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,s);
    break;
  case SHININESS:
    shininess = (double) cbs->value;
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
    break;
  }
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void new_sense(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  m_sense = cbs->value;
}

/* ARGSUSED */
void new_speed(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  m_speed = (double) cbs->value * 0.1;
}

/* ARGSUSED */
void new_vcube_size(Widget w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  vcubesize = (double) cbs->value * 0.01;
  create_cube_dlist(0.03*vcubesize, VERTEX_CUBE);
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void new_polard(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  polard = (double) cbs->value * 0.01;
  From4DTo3D();
  AutoSmooth();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void changeScrDoorGap(Widget w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  scrdoorgap = (double) cbs->value * 0.01;
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void changeSphRows(Widget w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  sph_rows = (double) cbs->value;
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void changeSphSize(Widget w, XtPointer client_data, XtPointer call_data) {
  XmScaleCallbackStruct *cbs = (XmScaleCallbackStruct *) call_data;

  sph_size = (double) cbs->value * 0.01;
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void setting1_ok(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  oldnear = near;
  oldfar = far;
  oldfdist = fdist;
  oldeye = eye;
  oldscale = scale;
  oldpolard = polard;
}

/* ARGSUSED */
void setting2_ok(Widget scale_w, XtPointer client_data, XtPointer call_data) {
  oldspecular = specular;
  oldshininess = shininess;
  oldm_sense = m_sense;
  oldm_speed = m_speed;
  oldvcubesize = vcubesize;
  oldscrdoorgap = scrdoorgap;
  oldsph_size = sph_size;
  oldsph_rows = sph_rows;
}

/* ARGSUSED */
void setting1_cancel(Widget scale_w, XtPointer client_data, 
		    XtPointer call_data) {
  near = oldnear;
  far = oldfar;
  fdist = oldfdist;
  eye = oldeye;
  scale = oldscale;
  polard = oldpolard;

  setting_all();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void setting2_cancel(Widget scale_w, XtPointer client_data, 
		    XtPointer call_data) {
  specular = oldspecular;
  shininess = oldshininess;
  m_sense = oldm_sense;
  m_speed = oldm_speed;
  vcubesize = oldvcubesize;
  scrdoorgap = oldscrdoorgap;
  sph_size = oldsph_size;
  sph_rows = oldsph_rows;

  setting_all();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void setting1_default(Widget scale_w, XtPointer client_data, 
		     XtPointer call_data) {
  near = defaultnear;
  far = defaultfar;
  fdist = defaultfdist;
  eye = defaulteye;
  scale = defaultscale;
  polard = defaultpolard;

  setting_all();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void setting2_default(Widget scale_w, XtPointer client_data, 
		     XtPointer call_data) {
  specular = defaultspecular;
  shininess = defaultshininess;
  m_sense = defaultm_sense;
  m_speed = defaultm_speed;
  vcubesize = defaultvcubesize;
  scrdoorgap = defaultscrdoorgap;
  sph_size = defaultsph_size;
  sph_rows = defaultsph_rows;

  setting_all();
  redraw_flag = ON;
  draw_all();
}

/* ARGSUSED */
void colorSelect_ok(Widget scale_w, XtPointer client_data, 
		    XtPointer call_data) {
  oldBGred = BGred;
  oldBGgreen = BGgreen;
  oldBGblue = BGblue;
}

/* ARGSUSED */
void colorSelect_cancel(Widget scale_w, XtPointer client_data, 
			XtPointer call_data) {
  BGred = oldBGred;
  BGgreen = oldBGgreen;
  BGblue = oldBGblue;

  color_all();
  draw_all();
}

/* ARGSUSED */
void colorSelect_default(Widget scale_w, XtPointer client_data, 
		    XtPointer call_data) {
  BGred = defaultBGred;
  BGgreen = defaultBGgreen;
  BGblue = defaultBGblue;

  color_all();
  draw_all();
}

void setting_all() {
  float s[4];

  From4DTo3D();
  AutoSmooth();
  
  create_cube_dlist(0.03*vcubesize, VERTEX_CUBE);

  myLoadProjection();
  glMatrixMode(GL_MODELVIEW);

  s[0] = specular * mats[0];
  s[1] = specular * mats[1];
  s[2] = specular * mats[2];
  s[3] = 1.0;
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,s);
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
  
  glLoadMatrixd(trans_mat.d);
  glMultMatrixd(rot_mat.d);

  XtVaSetValues(nearScale, XmNvalue, (int) (near*10.0),	NULL);
  XtVaSetValues(farScale, XmNvalue, (int) (far*10.0), NULL);
  XtVaSetValues(fdistScale, XmNvalue, (int) (fdist*10.0), NULL);
  XtVaSetValues(eyeScale, XmNvalue, (int) (eye*10.0), NULL);
  XtVaSetValues(scaleScale, XmNvalue, (int) (scale*10.0), NULL);
  XtVaSetValues(specularScale, XmNvalue, (int) (specular*100.0), NULL);
  XtVaSetValues(shininessScale, XmNvalue, (int) shininess, NULL);
  XtVaSetValues(sensitiveScale, XmNvalue, (int) m_sense, NULL);
  XtVaSetValues(speedScale, XmNvalue, (int) (m_speed*10.0), NULL);
  XtVaSetValues(polardScale, XmNvalue, (int) (polard*100.0), NULL);
  XtVaSetValues(vcubesizeScale, XmNvalue, (int) (vcubesize*100.0), NULL);
  XtVaSetValues(scrDoorScale, XmNvalue, (int) (scrdoorgap*100.0), NULL);
  XtVaSetValues(sphRowsScale, XmNvalue, sph_rows, NULL);
  XtVaSetValues(sphSizeScale, XmNvalue, (int) (sph_size*100.0), NULL);
}  

void color_all() {
  XtVaSetValues(redScale, XmNvalue, (int)(BGred*255.99), NULL);
  XtVaSetValues(greenScale, XmNvalue, (int)(BGgreen*255.99), NULL);
  XtVaSetValues(blueScale, XmNvalue, (int)(BGblue*255.99), NULL);
  color.red = (int) (BGred*255.99) << 8;
  color.green = (int) (BGgreen*255.99) << 8;
  color.blue = (int) (BGblue*255.99) << 8;
  /* reuse the same color again and again */
  XFreeColors (dpy, cmap, &color.pixel, 1, 0);
  if (!XAllocColor (dpy, cmap, &color)) {
    puts ("Couldn't XAllocColor!"); 
    exit(1);
  }
  XtVaSetValues (colorwindow, XmNbackground, color.pixel, NULL);
}
