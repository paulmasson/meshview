#include <stdio.h>
#include <math.h>

/* Compile with "cc -o torus torus.c -lm".
   It generates torus.3off.

   Hui Ma, July 21, 1994
*/

int ni, nj;

int idx(int i, int j) {
  return (i*nj+j);
}

main(int argc, char *argv[]) {
  int i, j;
  double theta, phi, dtheta, dphi;
  FILE *out;
  double x, y, z;
  double a, b;

  a = 1.0; b = 0.2;

  if(argc != 3) {
    printf("Usage : torus # #\n");
    exit(2);
  }

  out = fopen("torus.3off", "w");
  ni = atoi(argv[1]);
  nj = atoi(argv[2]);
  fprintf(out, "3OFF\n%d %d %d\n", ni*nj, 2*ni*nj, 1);

  dtheta = 2.0*M_PI / ni;
  dphi = 2.0*M_PI / nj;

  theta = 0.0;
  for(i=0; i<ni; i++) {
    phi = 0.0;
    for(j=0; j<nj; j++) {
      x = (a + b * cos(theta)) * cos(phi);
      z = b * sin(theta);
      y = (a + b * cos(theta)) * sin(phi);
      fprintf(out, "%f %f %f\n", x, y, z);
      phi += dphi;
    }
    theta += dtheta;
  }

  for(j=0; j<=nj-2; j++)
    for(i=0; i<=ni-2; i++) {
      fprintf(out, "3 %d %d %d\n", idx(i,j), idx(i,j+1), idx(i+1,j+1));
      fprintf(out, "3 %d %d %d\n", idx(i,j), idx(i+1,j+1), idx(i+1,j));
    }

  for(j=0; j<=nj-2; j++) {
    fprintf(out, "3 %d %d %d\n", idx(ni-1,j), idx(ni-1,j+1), idx(0,j+1));
    fprintf(out, "3 %d %d %d\n", idx(ni-1,j), idx(0,j+1), idx(0,j));
  }
  for(i=0; i<=ni-2; i++) {
    fprintf(out, "3 %d %d %d\n", idx(i,nj-1), idx(i,0), idx(i+1,0));
    fprintf(out, "3 %d %d %d\n", idx(i,nj-1), idx(i+1,0), idx(i+1,nj-1));
  }
  fprintf(out, "3 %d %d %d\n", idx(ni-1,nj-1), idx(ni-1,0), idx(0,0));
  fprintf(out, "3 %d %d %d\n", idx(ni-1,nj-1), idx(0,0), idx(0,nj-1));
}

