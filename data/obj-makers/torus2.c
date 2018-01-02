#include <stdio.h>
#include <math.h>

/* Compile with "cc -o torus torus.c -lm".
   It generates torus.3mesh.

   Hui Ma, July 21, 1994
*/

main(int argc, char *argv[]) {
  int i, j, nu, nv;
  double theta, phi, dtheta, dphi;
  FILE *out;
  double x, y, z;
  double a, b;

  a = 1.0; b = 0.2;

  if(argc != 3) {
    printf("Usage : torus # #\n");
    exit(2);
  }

  out = fopen("torus.3mesh", "w");
  nu = atoi(argv[1]);
  nv = atoi(argv[2]);
  fprintf(out, "uvMESH\n%d %d\n", nu, nv);

  dtheta = 2.0*M_PI / (double)nu;
  dphi = 2.0*M_PI / (double)nv;

  phi = 0.0;
  for(j=0; j<nv; j++) {
    theta = 0.0;
    for(i=0; i<nu; i++) {
      x = (a + b * cos(theta)) * cos(phi);
      z = b * sin(theta);
      y = (a + b * cos(theta)) * sin(phi);
      fprintf(out, "%f %f %f\n", x, y, z);
      theta += dtheta;
    }
    phi += dphi;
  }
}
