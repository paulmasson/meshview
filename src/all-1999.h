/* header file for all *.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <math.h>

#include <Xm/XmAll.h>

#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <X11/Xos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#ifdef MESA /*  This really depends on where you installed the
		extra GLwMDrawA.h include file. */
#include <GL/GLwMDrawA.h>
#else
#include <X11/GLw/GLwMDrawA.h>
#endif

#include "defines.h"
#include "structs.h"

extern int errno;

#ifdef ACTION_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN int gl_version_1_1;
EXTERN int glw_width, glw_height;
EXTERN Widget topLevel, glw, dawpo;
EXTERN XtAppContext app_context;
EXTERN XtWorkProcId workprocid; 
EXTERN GLXContext glx_context;
EXTERN Window main_win, po_win; 

EXTERN void reset(Widget, XEvent *, String *, int *);
EXTERN void center(Widget, XEvent *, String *, int *);
EXTERN void stop(Widget, XEvent *, String *, int *);
EXTERN void motion(Widget, XEvent *, String *, int *);

EXTERN void initCB (Widget, XtPointer, XtPointer);
EXTERN void exposeCB (Widget, XtPointer, XtPointer);
EXTERN void resizeCB (Widget, XtPointer, XtPointer);

#undef EXTERN
#ifdef CALLBACK_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void bg_color(Widget, XtPointer, XtPointer);
EXTERN void new_proj(Widget, XtPointer, XtPointer);
EXTERN void new_scale(Widget, XtPointer, XtPointer);
EXTERN void new_lighting(Widget, XtPointer, XtPointer);
EXTERN void new_sense(Widget, XtPointer, XtPointer);
EXTERN void new_speed(Widget, XtPointer, XtPointer);
EXTERN void new_polard(Widget, XtPointer, XtPointer);
EXTERN void changeScrDoorGap(Widget, XtPointer, XtPointer);
EXTERN void new_vcube_size(Widget, XtPointer, XtPointer);
EXTERN void setting1_ok(Widget, XtPointer, XtPointer);
EXTERN void setting1_cancel(Widget, XtPointer, XtPointer);
EXTERN void setting1_default(Widget, XtPointer, XtPointer);
EXTERN void setting2_ok(Widget, XtPointer, XtPointer);
EXTERN void setting2_cancel(Widget, XtPointer, XtPointer);
EXTERN void setting2_default(Widget, XtPointer, XtPointer);
EXTERN void colorSelect_ok(Widget, XtPointer, XtPointer);
EXTERN void colorSelect_cancel(Widget, XtPointer, XtPointer);
EXTERN void colorSelect_default(Widget, XtPointer, XtPointer);
EXTERN void changeSphRows(Widget, XtPointer, XtPointer);
EXTERN void changeSphSize(Widget, XtPointer, XtPointer);
EXTERN void setting_all();
EXTERN void color_all();

#undef EXTERN
#ifdef DRAW_C
#define EXTERN
#else 
#define EXTERN extern
#endif
/*  display_lists for each object */
EXTERN int FACE_LIST;
EXTERN int EDGE_LIST;
EXTERN int NORMAL_LIST;
EXTERN int VERTEX_LIST;
EXTERN int VERTEX_CUBE_LIST;
EXTERN int UNIT_SPHERE_LIST;
EXTERN int PO_CUBE_LIST;
EXTERN void create_cube_dlist(double, int);
EXTERN void draw_all(); 

#undef EXTERN
#ifdef GEOM_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN Vector3 Cross3(Vector3 *, Vector3 *);
EXTERN double Dot3(Vector3 *, Vector3 *);
EXTERN double Dot4(Vector4 *, Vector4 *);
EXTERN void Normalize3(Vector3 *);
EXTERN Vector3 Normalize3R(Vector3 *);
EXTERN Vector3 PlaneNormal(Point3 *, Point3 *, Point3 *);
EXTERN void Vector3TimesDouble(Vector3 *, double);
EXTERN Vector3 Vector3TimesDoubleR(Vector3 *, double);
EXTERN void Vector4TimesDouble(Vector4 *, double);
EXTERN Vector4 Vector4TimesDoubleR(Vector4 *, double);
EXTERN Point3 PointAddVector3(Point3 *, Vector3 *);
EXTERN void PointMinusPoint3(Point3 *, Point3 *, Vector3 *);
EXTERN Vector3 PointMinusPoint3R(Point3 *, Point3 *);
EXTERN void PointAddVector3R(Point3 *, Vector3 *);
EXTERN Point4 PointAddVector4(Point4 *, Vector4 *);
EXTERN void PointAddVector4R(Point4 *, Vector4 *);
EXTERN Vector3 Point3MinusPoint3(Point3 *, Point3 *);
EXTERN Vector4 Point4MinusPoint4(Point4 *, Point4 *);
EXTERN Vector3 VertexMinusVertex3(Vertex *, Vertex *);
EXTERN Vector4 VertexMinusVertex4(Vertex *, Vertex *);
EXTERN Vector3 PointMinusVertex3(Point3 *, Vertex *);
EXTERN Point4 Matrix4TimesPoint4(Matrix4 *, Point4 *);
EXTERN Point3 Matrix3TimesPoint3(MatrixGL *, Point3 *);
EXTERN Matrix4 Matrix4TimesMatrix4(Matrix4 *, Matrix4 *);
EXTERN Matrix4 MatrixGLTimesMatrix4(MatrixGL *, Matrix4 *);
EXTERN Matrix4 MatrixRot4XY(double, double);
EXTERN Matrix4 MatrixRot4XZ(double, double);
EXTERN Matrix4 MatrixRot4XW(double, double);
EXTERN Matrix4 MatrixRot4YZ(double, double);
EXTERN Matrix4 MatrixRot4YW(double, double);
EXTERN Matrix4 MatrixRot4ZW(double, double);

#undef EXTERN
#ifdef INIT_C
#define EXTERN
#else 
#define EXTERN extern
#endif
/* polard, better greater than 0, is the distance between 
   the projection center and the north pole in polar projection */


EXTERN double aspect, img_scale;

EXTERN double near, oldnear, defaultnear;
EXTERN double far, oldfar, defaultfar; 
EXTERN double fdist, oldfdist, defaultfdist;
EXTERN double eye, oldeye, defaulteye;

EXTERN double scale, oldscale, defaultscale;
EXTERN double specular, oldspecular, defaultspecular;
EXTERN double shininess, oldshininess, defaultshininess;
EXTERN double m_sense, oldm_sense, defaultm_sense; 
EXTERN double m_speed, oldm_speed, defaultm_speed;
EXTERN double polard, oldpolard, defaultpolard;
EXTERN double vcubesize, oldvcubesize, defaultvcubesize;
EXTERN int sph_rows, oldsph_rows, defaultsph_rows;
EXTERN double sph_size, oldsph_size, defaultsph_size;
EXTERN double scrdoorgap, oldscrdoorgap, defaultscrdoorgap;
EXTERN float BGred, oldBGred, defaultBGred;
EXTERN float BGgreen, oldBGgreen, defaultBGgreen;
EXTERN float BGblue, oldBGblue, defaultBGblue;
EXTERN float BGalpha;
EXTERN float palette[256][4];
EXTERN int texImageHeight;
EXTERN int texImageWidth;
EXTERN GLubyte *texImage;

EXTERN GLfloat mata[4], matd[4], mats[4];
EXTERN GLfloat ltp[4], ltc[4];

EXTERN double back_z;
EXTERN double cubesize;

EXTERN Matrix4 IMat4;
EXTERN Matrix4 mat4, d_mat4;
EXTERN MatrixGL IMatGL;
EXTERN MatrixGL rot_mat, d_rot_mat, trans_mat;

EXTERN int mouse3d_flag, mouse3d_reset_flag;

EXTERN int dmousex, dmousey, button_flag, momentum;
EXTERN int contextf_flag, momentum_mode;
EXTERN int motion_flag, redraw_flag;
EXTERN int draw_face_flag, draw_edge_flag, draw_face_flat_flag;
EXTERN int draw_normal_flag, draw_vertex_flag, draw_palette_flag;
EXTERN int draw_unitsph_flag, draw_lighting_flag, picker_flag;
EXTERN int draw_axis_flag;
EXTERN int draw_color_mode, proj3_mode, proj4_mode, proj4_type;
EXTERN GLbitfield draw_shade_mode;
EXTERN int scr_door_mode;
EXTERN int frames_mode, frames_transform_flag;
EXTERN int stereo_mode;
EXTERN int adjust_size_mode;
EXTERN float c_default_front[4], c_default_back[4];
EXTERN float black[4], white[4], red[4], green[4], blue[4];
EXTERN float yellow[4], cyan[4], magenta[4];

EXTERN void globalVarInit();
EXTERN void setup();

#undef EXTERN
#ifdef LOAD_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void load_data(char *, int);
EXTERN void load_settings(char *);
EXTERN void load_pallete(char *);
EXTERN void load_texture(char *);

#undef EXTERN
#ifdef MAIN_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN Display *dpy;
EXTERN Screen *scr;
EXTERN Colormap cmap;
EXTERN Obj **olist;
EXTERN int nobjs;

#undef EXTERN
#ifdef MOUSE3D_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN int mouse3d_open(char *, char *);
EXTERN void mouse3d_close();
EXTERN void mouse3d_interact();
EXTERN int mouse3d;

#undef EXTERN
#ifdef RESET_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void Reset();

#undef EXTERN
#ifdef SAVE_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void save_settings(char *);
EXTERN void save_ppm(char *);

#undef EXTERN
#ifdef TRANSFORM_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void transform();
EXTERN void MakeRot3(double, double);
EXTERN void MakeRot3Z(double);
EXTERN void MakeRot4(double, double, double);
EXTERN void AutoSmooth();
EXTERN void CombineRot();
EXTERN void From4DTo3D();
EXTERN void myLoadProjection();

#undef EXTERN
#ifdef UI_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN Widget helpBox, helpList, aboutBox, errorBox;
EXTERN Widget colorwindow; /* the window the displays a solid color */
EXTERN XColor color;       /* the color in the colorwindow */

EXTERN Widget BuildMenu(Widget, int, char *, char, Boolean, MenuItem *);
EXTERN void build_ui();
EXTERN void ShowBox(Widget, XtPointer, XtPointer);

#undef EXTERN
#ifdef UI_INIT_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void ui_init();

#undef EXTERN
#ifdef UI_PICKER_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN Widget pickerDialog, pickerHelpList, objNumScale;
EXTERN Point4 pickerP4;
EXTERN Point3 pickerP3;

EXTERN void build_uiPicker(Widget);
EXTERN void setPicker();
EXTERN void resetPicker();
EXTERN void picker_close(Widget, XtPointer, XtPointer);

#undef EXTERN
#ifdef UI_SETTING_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN Widget nearScale, farScale, fdistScale, scaleScale, eyeScale;
EXTERN Widget specularScale;
EXTERN Widget shininessScale, redScale, greenScale, blueScale;
EXTERN Widget sensitiveScale, speedScale, polardScale, vcubesizeScale;
EXTERN Widget scrDoorScale;
EXTERN Widget sphRowsScale, sphSizeScale;
EXTERN Widget setting1Dialog, setting2Dialog;
EXTERN void build_uiSetting(Widget);

#undef EXTERN
#ifdef UI_FRAMES_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN Widget framesDialog, framesHelpList;

EXTERN void build_uiFrames(Widget);
EXTERN void frames_init();
EXTERN void frames_play();

#undef EXTERN

#undef EXTERN
#ifdef UI_QUATROT_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN int quat_momentum;
EXTERN Widget quatrotDialog; 
EXTERN void build_uiQuatrot(Widget);
EXTERN void quat_init();
EXTERN void transform_quat();
#undef EXTERN

#ifndef NO_STEREO

#ifdef STEREO_C
#define EXTERN
#else 
#define EXTERN extern
#endif
EXTERN void start_stereo();
EXTERN void stop_stereo();
EXTERN void stereo_left_buffer();
EXTERN void stereo_right_buffer();
EXTERN void stereoproj(int);
#undef EXTERN

#endif /* NO_STEREO */
