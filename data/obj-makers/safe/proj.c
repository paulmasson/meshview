/* 
   The projective plane, embedded in 4D 
   July 12, 1994 
*/
   
#include <stdio.h>
#include <math.h>

main(int argc, char *argv[]) {
  FILE *out;
  int i, j, nu, nv;
  double du, dv, u, v;
  double x, y, z, w;

  if(argc != 3) {
    printf("Usage : proj <nu> <nv>\n");
    exit(1);
  }

  nu = atoi(argv[1]);
  nv = atoi(argv[2]);
  printf("nu = %d, nv = %d\n", nu, nv);

  if ((out = fopen("proj.4mesh", "w")) == NULL) {
    fprintf(stderr, "error: Could not open file.\n");
    exit(1);
  }
  
  fprintf(out, "4MESH\n%d %d\n",nu, nv);
  du = 2.0 * M_PI / (double) (nu-1.0);
  dv = 2.0 * M_PI / (double) (nv-1.0);

  v = 0.0;
  for (j=0; j<nv; j++) {
    u = 0.0;
    for (i=0; i<nu; i++) { 
      x = cos(u) * sin(v/2.0);
      y = sin(u) * sin(v/2.0);
      z = cos(2.0*u) * sin(v/4.0);
      w = -cos(v/2.0);
      fprintf(out, "%f %f %f %f\n", x, y, z, w);
      u = u + du;
    }
    v = v + dv;
  }
}
