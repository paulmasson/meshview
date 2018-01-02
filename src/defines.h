#ifndef DEFINES_H
#define DEFINES_H

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#define TRUE  1
#define FALSE 0

#define MAXOBJ 100 /* adjust this number if you want to load more objects */

#define min(x,y) ( ((x) < (y)) ? (x) : (y) ) 
#define max(x,y) ( ((x) > (y)) ? (x) : (y) ) 
#define flip(x)  ( ((x) == ON) ? OFF : ON )

/* for mesh->off */
#define myidx(i,j, nu, nv) ( (i)%(nu) + ( (j)%(nv) ) * (nu) ) 
#define myidx2(i,j, nu, nv) ( (i) + (j) * (nu) ) 
#define myfree(x) if(x!=NULL) free(x)

#define ON  TRUE
#define OFF FALSE
#define YES TRUE
#define NO  FALSE

#define TINYEPSILON 1.e-10
#define EPSILON     1.e-6

#define RAD_DEG 0.017453293

enum { FACE = 10, EDGE, VERTEX, NORMAL, UNIT_SPHERE, 
       PALETTE, LIGHTING, AXIS };
enum { X = 50, Y, Z, W };
enum { FORMAT_OFF = 100,  MESH, LIST, FRAMES, DOT, 
       LINE, LINE_STRIP, LINE_LOOP };

enum { FLAT = 150, SMOOTH };

/* for scale widgets */
enum settings { RED = 200, GREEN, BLUE, FDIST, NEAR, FAR, EYE, ZPS, 
		SPECULAR, SHININESS };
enum btnflg{ NONE = 250, SHIFT, CTRL };


/* for menubar choices */
enum { SAVE_PPM = 400, SAVE_SETTING, LOAD_DATA, LOAD_NEW_DATA, LOAD_SETTING,
       LOAD_PALETTE, LOAD_TEXTURE };

enum { NEW = 450, ADD };

enum { SAME = 500, DIFF, DEPTH, TEXTURE };

enum { SD_OFF = 520, SD_PLUS, SD_MINUS };

enum { COLORDFT = 550, COLOROBJ, COLORFACE, COLORVERT };

enum { PERSPECTIVE = 600, ORTHOGONAL, POLAR };

enum { NOCOLOR = 650, UNIFORM1, UNIFORM2 };

enum { SETTING1_DIALOG = 700, SETTING2_DIALOG, PICKER_DIALOG, FRAMES_DIALOG,
       HELP_DIALOG, ABOUT_DIALOG, MOUSE3D_DIALOG, QUATROT_DIALOG};

/* for 3D mouse */
enum { _3DBUTTONLEFT = 750, _3DBUTTONMIDDLE, _3DBUTTONRIGHT, _3DBUTTONSIDE };

/* for create_cube_dlist function */
enum { VERTEX_CUBE = 800, PICKER_CUBE };

enum { LEFT_EYE = 850, RIGHT_EYE };

#endif
