#ifndef NO_STEREO

#define STEREO_C
#include "all.h"
#include <X11/extensions/SGIStereo.h>

/* Standard screen diminsions */
#define XMAXSCREEN	1280
#define YMAXSCREEN	1024

#define YSTEREO		491		/* Subfield height in pixels */
#define YOFFSET_LEFT	532		/* YSTEREO + YBLANK */

void start_stereo() 
{
  int event, error;

  if (!XSGIStereoQueryExtension(dpy, &event, &error)) {
    fprintf(stderr, "Stereo not supported on this display!\n");
  }
  if (XSGIQueryStereoMode(dpy, main_win) == X_STEREO_UNSUPPORTED) {
    fprintf(stderr, "Stereo not supported on this window!\n");
  }
  if (system("/usr/gfx/setmon -n STR_TOP") != 0) {
    fprintf(stderr, "setmon attempt failed!\n");
    stop_stereo();
  }
}

void stop_stereo() 
{
  system("/usr/gfx/setmon -n 72HZ");
}

void stereo_left_buffer() 
{
  XSGISetStereoBuffer(dpy, main_win, STEREO_BUFFER_LEFT);
  XSync(dpy, False);
}

void stereo_right_buffer()
{
  XSGISetStereoBuffer(dpy, main_win, STEREO_BUFFER_RIGHT);
  XSync(dpy, False);
}

/* This routine performs the perspective projection for one eye's subfield.
   The projection is in the direction of the negative z axis.

   znear, zfar = the z-coordinate values of the clipping planes.

   fdist = the distance from the center of projection to the plane of zero
   parallax.

   eye = half the eye separation; positive for the right eye subfield,
   negative for the left eye subfield.
 */

void stereoproj(int flag)
{
  float top, bottom, left, right, n_over_d, eyedist;

  eyedist = eye/glw_width * 35;    /* keeps the eye distance constant for
				      any size of window. 15 is experimental
				      value for 19" monitor */

  if(flag == LEFT_EYE)  eyedist = -eyedist;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  switch(proj3_mode) 
    {
    case PERSPECTIVE:
      /* together with glTranslate keeps the center of the unit sphere in
	 the focal plane */
      n_over_d = near/fdist;
      if(aspect > 1.0) {
	top = n_over_d  * img_scale;
	bottom = -top;
	right = n_over_d * (img_scale*aspect - eyedist);
	left = n_over_d * (-img_scale*aspect - eyedist);
      } else {
	top = n_over_d  * img_scale / aspect;
	bottom = -top;
	right = n_over_d * (img_scale - eyedist);
	left = n_over_d * (-img_scale - eyedist);
      }
      glFrustum(left, right, bottom, top, near, far);
      break;
    case ORTHOGONAL:
      if(aspect > 1.0) {
	top = img_scale;
	bottom = -top;
	right = img_scale*aspect - eyedist;
	left = -img_scale*aspect - eyedist;
      } else {
	top = img_scale / aspect;
	bottom = -top;
	right = img_scale - eyedist;
	left = -img_scale - eyedist;
      }
      glOrtho(left, right, bottom, top, near, far);
      break;
    }
  glTranslated(-eyedist, 0.0, -fdist); /* placed here to allow objects to 
					  be in the same place when switching 
					  projection modes. */
}

#endif /* NO_STEREO */
