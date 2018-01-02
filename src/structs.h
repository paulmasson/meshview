#ifndef STRUCTS_H
#define STRUCTS_H

#define NMAX 6

typedef struct { double d[3]; } Point3;
typedef struct { double d[4]; } Point4;
typedef struct { double d[5]; } Point5;
typedef struct { double d[6]; } Point6;
typedef struct { double d[NMAX]; } PointN;

typedef struct { double d[3]; } Vector3;
typedef struct { double d[4]; } Vector4;
typedef struct { double d[5]; } Vector5;
typedef struct { double d[6]; } Vector6;
typedef struct { double d[NMAX]; } VectorN;

typedef struct { float f[4]; } Color4; 
typedef struct { double d[4]; } HmgPoint3;
typedef struct { double d[4]; } HmgVector3;
typedef struct { double d[16]; } Matrix4;
typedef struct { double d[25]; } Matrix5;
typedef struct { double d[36]; } Matrix6;
typedef struct { double d[16]; } MatrixGL;

typedef struct { double d[3]; } Texture;

/* for BuildMenu*/
typedef struct _menu_item {
    char        *label;          /* the label for the item */
    WidgetClass *class;          /* pushbutton, label, separator... */
    char         mnemonic;       /* mnemonic; NULL if none */
    char        *accelerator;    /* accelerator; NULL if none */
    char        *accel_text;     /* to be converted to compound string */
    void       (*callback)();    /* routine to call; NULL if none */
    XtPointer   callback_data;   /* client_data for callback() */
    struct _menu_item *subitems; /* pullright menu items, if not NULL */
} MenuItem;

typedef struct Vertex Vertex;
typedef struct Face Face;
typedef struct Obj Obj;

struct Vertex{
  int index;            /* index of this vertex in current obj */
  Point3 p3;            /* 3d coordinates */
  int depth;            /* 4d depth between 0 and 255 */
  Point4 p4;            /* 4d coordinates */
  /*Point5 p5;             5d coordinates */
  /*Point6 p6;             6d coordinates */
  /*PointN pN;             Nd coordinates */
  Vector3 normal ;      /* 3d vertex normal */
  Color4 cfront, cback; /* RGBA colors */
  Texture tex;          /* Texture coordinates */
  int Nfaces;           /* number of neighbouring faces */
  Face ** faces;        /* list of pointers to neighbouring faces */
};

struct Face {
  int index;            /* index of this face in current obj */
  int nvert;            /* # of vertices in the face (4 in MESH) */
  Vertex ** vertices;   /* pointers to the vertices */  
  Vector3 normal;       /* 3d plane normal */
  Color4 cfront, cback; /* RGBA colors */
};

struct Obj {
  int gformat;       /* FORMAT_OFF, MESH, LIST, FRAMES, DOT, LINES etc.*/
  int cformat;       /* UNIFORM (0, 1 or 2 colors per obj) */ 
  int texture_flag;  /* Does user supply texture coordinates for vertices */
  int vcolor;        /* number of colors supplied for each vertex, 0, 1 or 2 */
  int fcolor;        /* number of colors supplied for each face, 0, 1 or 2 */
  int nvertices, nfaces;
  int nu, nv;        /* sizes of mesh, useful only when format_flag = MESH */
  int u_flag;        /* part of MESH format */
  int v_flag;        /* part of MESH format */
  int size_flag;     /* valid in POINTS/ LINES formats */
  float size;        

  Vertex *vlist;
  Face *flist;       /* in LINES format holds lines */
  double scale;      /* scale and center are used to fit object */
  Point4 center;     /* into identity sphere */
  Color4 cfront, cback;

  int nframes;       /* data for frame mode */
  Obj **frames;
};

#endif
