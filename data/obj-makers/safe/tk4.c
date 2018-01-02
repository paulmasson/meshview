/* Object Maker for torus knots on the flat 4-torus
   cc -o tk4 tk4.c -lm
   tknot 2 3 128
*/

#include <stdio.h>
#include <math.h>
#define WID 5

int n, m, nsegs;
double scale = M_PI *0.025;

main(int argc, char * argv[]) {
  FILE *out;
  char filename[256];
  int i, j;
  double dtheta, dn, dm, dth,  thn ,thm;
  double x, y, z, w;
  double tmp, deg;

  if(argc != 4) {
    printf("Usage : tknot <n> <m> <segments>\n");
    exit(0);
  }
  n = atoi(argv[1]);
  m = atoi(argv[2]);
  nsegs = atoi(argv[3]);

  dtheta = 2.0*M_PI/(double)nsegs;
  dn = (double) n;
  dm = (double) m;

  sprintf(filename, "./4tknot%02d-%02d.4mesh", n, m);

  out = fopen(filename, "w");

  fprintf(out, "4uMESH\n%d %d\n", nsegs, WID);

  /* deg = M_PI / 4.0; */
  deg = 0.0;

  for(j=0; j<WID ; j++){
    dth = scale * (double)j;
    for(i=0; i<nsegs; i++) {
      thn = dn*dtheta * (double)i;
      thm = dm*dtheta * (double)i;
      x = cos(thn + dth);
      y = sin(thn + dth);
      z = cos(thm);
      w = sin(thm);
      tmp = x * cos(deg) - w * sin(deg);
      w = x * sin(deg) + w * cos(deg);
      x = tmp; 
      fprintf(out, "%f %f %f %f\n", x,y,z,w);
    }
  }
}
