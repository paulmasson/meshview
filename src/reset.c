#define RESET_C
#include "all.h"

void Reset() 
{
  mat4 = IMat4;
  From4DTo3D();
  AutoSmooth();
  proj4_mode = W;
  momentum = OFF;
  quat_momentum = OFF;
  d_rot_mat = IMatGL;
  rot_mat = IMatGL;
  trans_mat = IMatGL;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  ltp[0] = 0.0; ltp[1] = 0.1; ltp[2] = 1.0; ltp[3] = 0.0; 
  glDisable(GL_LIGHT0);
  glLightfv(GL_LIGHT0,GL_POSITION,ltp);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,ltc);
  glEnable(GL_LIGHT0); /* enable the lighting with identity matrix */
  if(picker_flag == ON) resetPicker();
}
