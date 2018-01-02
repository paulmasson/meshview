#define UI_INIT_C
#include "all.h"

/* Convert an array of string to an array of compound strings */
XmStringTable ArgvToXmStringTable(int argc, char **argv) {
  XmStringTable new =
    (XmStringTable) XtMalloc ((argc+1) * sizeof (XmString));
  
  if (!new)
    return (XmStringTable) NULL;

  new[argc] = 0;
  while (--argc >= 0)
    new[argc] = XmStringCreate (argv[argc], "defaultset");
  return new;
}

/* Free the table created by ArgvToXmStringTable() */
void XmStringFreeTable(XmStringTable argv) {
  register int i;
  
  if(!argv)
    return;
  for(i = 0; argv[i]; i++)
    XmStringFree (argv[i]);
  XtFree((char*) argv);
}

void ui_init() 
{
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

/* create help text */
  { 
    char *help[] = {
      " ",
      " MOUSE/KEYBOARD CONTROLS", 
      " ",
      " (Below, (u,v) denotes the right-handed Cartesian coordinates  ",
      "  of the mouse.) ",
      " -- 3D viewing controls:",  
      "     leftbutton   : 3D rotation (3D rolling ball) R3(u,v)",
      "     middlebutton : 3D translation in x-y plane   T3(u,v,0)",
      "     rightbutton  : 3D translation along z axis   T3(0,0,-v)",
      "     Shift+right  : 3D rotation around z axis     R2(u)",
      " ",
      " -- 3D lighting controls: (you can see the light ray by Ctrl+l)  ",
      "     Ctrl+left    : 3D rotation (3D rolling ball) R3(u,v)",
      "     Ctrl+middle  : 3D rotation around z axis     R2(u)",
      " ",
      " -- 4D controls: ",
      "     Shift+left   : xyw rotation (4D rolling ball) R4(u,v,0)",
      "     Shift+middle : xzw rotation (4D rolling ball) R4(u,0,-v)",
      " ",
      " -- <Key>r : (Reset)  It resets 4D and 3D matrices and 3D light  ", 
      "                      direction, and it stops momentum.",
      " -- <Key>c : (Center) It resets 3D rotation and translation", 
      "                      matrices, and it stops momentum.",
      " -- <Key>s : (Stop)   It Stops momentum.",
      " ",
      " -- <Key>F3: (3D mouse)   Toggles 3D mouse.",
      " ",
      " -- Keyboard short-cuts for menu items:",
      "     Ctrl+d :  load data (old data discarded)",
      "     Ctrl+a :  add data",
      "     Ctrl+t :  load texture",
      "     Ctrl+l :  load palette",
      "     Ctrl+e :  load setting",
      "     Ctrl+f :  save setting",
      "     Ctrl+i :  save as ppm",
      "     Esc    :  quit",
      " ",
      "     <Key>f :  toggle drawing of faces (default on)",  
      "     <Key>e :  toggle drawing of edges (default off)",
      "     <Key>v :  toggle drawing of vertices (default off)",   
      "     <Key>n :  toggle drawing of normals (default off)",     
      "     <Key>u :  toggle drawing of unit sphere (default off)",     
      "     <Key>i :  toggle drawing of 2D complex plane (default off)",
      "     <Key>p :  toggle drawing of palette (default off)",     
      "     <Key>l :  toggle drawing of light (default off)",      
      "     <Key>a :  toggle drawing of axes (default off)",      
      " ",
      "     <Key>1 :  draw two sides of face use same color (default)",
      "     <Key>2 :  draw two sides of face use different colors",
      "     <Key>3 :  4D depth color coding", 
      "     <Key>4 :  use texture",
      " ",
      "     <Key>5 :  screen_door off ",
      "     <Key>6 :  screen door positive",
      "     <Key>7 :  screen door negative",
      " ",
      "      Ctrl+g:  smooth shading (default)",
      "      Ctrl+h:  flat shading",
      " ",
      "     Ctrl+p :  3D perspective projection (default)",
      "     Ctrl+o :  3D orthogonal projection",
      " ",
      "     <Key>w :  4D projection along w-axis (default)",
      "     <Key>x :  4D projection along x-axis",
      "               (rotating the objects in x-w plane by 90 degrees  ",
      "                before apply other 4D rotations)",
      "     <Key>y :  4D-projection along y-axis",
      "               (rotating the objects in y-w plane by 90 degrees  ",
      "                before apply other 4D rotations)",
      "     <Key>z :  4D-projection along z-axis",
      "               (rotating the objects in z-w plane by 90 degrees  ",
      "                before apply other 4D rotations)",
      " ",
      "     Shift+o :  4D orthogonal projection (default)",
      "     Shift+p :  4D polar projection ",
      "                (use 'polar distance' scale on 'setting panel' ",
      "                 to adjust the parameter) ",
      " ",
      "     Ctrl+m :  toggle momentum (default on)",
      "     Ctrl+q :  toggle context-free (default on)",
      "     Ctrl+s :  toggle stereo mode (default off)",
      " ",
      "     Shift+v :  pop up view setting panel",
      "     Shift+s :  pop up setting panel",
      "     Shift+k :  pop up picker panel",
      "     Shift+q :  pop up quaternion rotation panel",
      "     Shift+f :  pop up key frame panel",
      " ",
      " NOTES:",
      " ",
      " See the README file for a short tutorial on the rolling ball ",
      " interface and the concept of context-free. ",
      " Transparency is not supported in this version, but by using ",
      " the screen door drawing modes or by adjusting the near ",
      " and far clipping plane sliders on the setting panel, ",
      " one can get some useful insights into surface interiors. ",
      " "};

    XmStringTable strs =
      ArgvToXmStringTable(XtNumber(help), help);

    XtVaSetValues(helpList, 
		  XmNitems, strs,
		  XmNitemCount, XtNumber(help),
		  NULL);
    XmStringFreeTable(strs);
  }
/* help text for pickerHelpList */
  {
    char *help[] = {
      " ",
      " This dialog creates a connection between the positions in ",
      " the parametric space of mesh to the actural 4D/3D positions ",
      " of the mesh. ",
      " ",
      " u and v are coordinates in the parametric space; ",
      " The 4D coordinates shown are the linear interpolation of ",
      " input values of four neighbouring vertices; ",
      " The 3D coordinates shown are the 3D position of the 4D point ",
      " after apply 4D rotation matrix, 4D->3D projection matrices ",
      " and 3D rotation matrix. ",
      " ",
      " You can adjust the size of the cube, and you can choose the ",
      " number of the object you want to do the parametric mapping ",
      " if more than one objects have been loaded. ",
      " "};
    XmStringTable strs =
      ArgvToXmStringTable(XtNumber(help), help);
    
    XtVaSetValues(pickerHelpList, 
		  XmNitems, strs,
		  XmNitemCount, XtNumber(help),
		  NULL);
    XmStringFreeTable(strs);
  }

/* help text for framesHelpList */
  {
    char *help[] = {
      " KEY FRAMES CONTROLS:",
      " ",
      " First row buttons:",
      "  <<   -  rewind to the beginning",
      "  ||<  -  make one step in reverse ",
      "  <    -  play in reverse direction",
      "  []   -  stop",
      "  >    -  play forward",
      "  >||  -  make one step forward ",
      "  >>   -  rewind to the end",
      " ",
      " Second row buttons:",
      "  --> -  play in one direction with a pause between runs",
      "  --> ",
      " ",
      "  --> -  play in both directions with a pause between change ", 
      "  <--    of direction",
      " ",
      "  -->  - play frames in a loop with interpolation between ",
      "  -<-    the last and the first frames", 
      " ",
      "  Linear/Cubic - type of interpolation",
      " ",
      " Indicator: ",
      "  Green line shows current position. It can be adjusted ",
      "    with the mouse (while holding the middle button) ",
      "    or with buttons below the indicator (in the middle)",
      " ",
      "  Black lines show position of the left and right limits",
      "    They can be adjusted with mouse (while holding the ",
      "    left button), or with buttons below the indicator",
      "    (on the left and right sides).",
      "    Note: in the LOOP mode limits can only be incremented or",
      "      decremented by a whole number of frames.",
      " ",
      " Rate of transformation:",
      "  The step of interpolation. 1/rate equals the number of steps",
      "  between two frames",
    };
    XmStringTable strs =
      ArgvToXmStringTable(XtNumber(help), help);
    
    XtVaSetValues(framesHelpList, 
		  XmNitems, strs,
		  XmNitemCount, XtNumber(help),
		  NULL);
    XmStringFreeTable(strs);
  }
  quat_init();
}
