/* 
   The projective plane, embedded in 4D 

   Compile with "cc -o rp2 rp2.c -lm".
   It generates rp2.4mesh.

   Hui Ma, July 12, 1994 
*/
   
#include <stdio.h>
#include <math.h>

main(int argc, char *argv[]) {
  FILE *out;
  int i, j, nu, nv;
  double du, dv, u, v;
  double x, y, z, w;

  if(argc != 3) {
    printf("Usage : rp2 <nu> <nv>\n");
    exit(1);
  }

  nu = atoi(argv[1]);
  nv = atoi(argv[2]);
  printf("nu = %d, nv = %d\n", nu, nv);

  if ((out = fopen("rp2.4mesh", "w")) == NULL) {
    fprintf(stderr, "error: Could not open file.\n");
    exit(1);
  }
  
  fprintf(out, "4MESH\n%d %d\n",nu, nv);
  du = M_PI / (double) (nu-1.0);
  dv = M_PI / (double) (nv-1.0);

  v = 0.0;
  for (j=0; j<nv; j++) {
    u = 0.0;
    for (i=0; i<nu; i++) { 
      x = cos(u)*cos(u)*cos(v)*cos(v) - sin(u)*sin(u)*cos(v)*cos(v);
      y = sin(u)*cos(u)*cos(v)*cos(v);
      z = cos(u)*sin(v)*cos(v);
      w = sin(u)*sin(v)*cos(v);
      fprintf(out, "%f %f %f %f\n", x, y, z, w);
      u = u + du;
    }
    v = v + dv;
  }
}
