#define MOUSE3D_C
#include "all.h"

#include "sgidrvr.h"

int mouse3d_open(char *dev, char *reason)
{
  mouse3d = logitech_open (dev, reason);
  if (mouse3d < 0) return -1;

  cu_mouse_mode (mouse3d);
  cu_euler_mode (mouse3d);
  cu_demand_reporting (mouse3d);
  return 0;
}

void mouse3d_close() 
{
  logitech_close(mouse3d);
}

void mouse3d_interact() 
{
  double min_degree = 1.0;
  double min_dist = 0.01;
  double dist_scale = 0.3;
  double degree_scale = 15.0;

  MouseRecordType record;
  static int button_flag;
  unsigned char buttons;
  static float oldx, oldy, oldz, oldpitch, oldyaw, oldroll;
  float newx, newy, newz, newpitch, newyaw, newroll;
  double dx, dy, dz, dpitch, dyaw, droll;
  
  if(get_record (mouse3d, &record) == -1) return;

  newx = record.x;
  newy = record.y;
  newz = record.z;
  newpitch = record.pitch;
  newyaw = record.yaw;
  newroll = record.roll;

  if(mouse3d_reset_flag == OFF) {

    buttons = record.buttons;
    if(record.buttons & (1<<2))
      button_flag = _3DBUTTONLEFT;
    else if(record.buttons & (1<<1))
      button_flag = _3DBUTTONMIDDLE;
    else if(record.buttons & 1)
      button_flag = _3DBUTTONRIGHT;
    else if(record.buttons & (1<<3))
      button_flag = _3DBUTTONSIDE;

    dx = newx - oldx;
    dy = newy - oldy;
    dz = newz - oldz;
    dpitch = newpitch - oldpitch;
    dyaw = newyaw - oldyaw;
    droll = newroll - oldroll;

    /*
      printf("dx dy dz = %f %f %f\n", dx, dy, dz);
      printf("dpitch, dyaw, droll = %f %f %f\n", dpitch, dyaw, droll);
      printf("button_flag = %d\n", button_flag);
    */

    switch(button_flag)
      {
      case _3DBUTTONRIGHT:
	if(dx*dx+dy*dy+dz*dz > min_dist) {
	  MakeRot4(degree_scale*dx, degree_scale*dy, degree_scale*dz);
	  mat4 = Matrix4TimesMatrix4(&d_mat4, &mat4);
	  From4DTo3D();
	  AutoSmooth();
	  redraw_flag = ON;
	}
	break;

      case _3DBUTTONLEFT:
	glLoadMatrixd(rot_mat.d);
	if(fabs(dpitch) > min_degree)
	  glRotated(dpitch, 1.0, 0.0, 0.0);
	if(fabs(dyaw) > min_degree)
	  glRotated(dyaw, 0.0, 1.0, 0.0);
	if(fabs(droll) > min_degree)
	  glRotated(droll, 0.0, 0.0, 1.0);
	glGetDoublev(GL_MODELVIEW_MATRIX, rot_mat.d);      
	glLoadMatrixd(trans_mat.d);
	glMultMatrixd(rot_mat.d);
	break;

      case _3DBUTTONMIDDLE:
	if(dx*dx+dy*dy+dz*dz > min_dist) {
	  glLoadMatrixd(trans_mat.d);
	  glTranslated(dist_scale*dx, dist_scale*dy, dist_scale*dz);
	  glGetDoublev(GL_MODELVIEW_MATRIX, trans_mat.d);
	  glMultMatrixd(rot_mat.d);
	}
	break;
      }
    draw_all();  

  } else  {
    button_flag = 0;
    mouse3d_reset_flag = OFF;
  }
  oldx = newx;
  oldy = newy;
  oldz = newz;
  oldpitch = newpitch;
  oldyaw = newyaw;
  oldroll = newroll;
}

