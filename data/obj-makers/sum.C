/* 
   Surfaces of solutions to the equations (z1)^(n1) + (z2)^(n2) = 1, 
   where n1 and n2 are positive integers.

   compile with "CC -o sum sum.C -lcomplex -lm"

   Hui Ma, July 13, 1994 
*/
   
#include <stdio.h>
#include <complex.h>
#include <math.h>

#define PHI 1.0
/* #define PHI M_PI */

extern "C" {
  exit(int);
  int atoi(char *);
}

complex s(int k, int n) {
  complex tmp = complex(0.0, 2.0*M_PI*(double)k/(double)n);

  return exp(tmp);
}

complex u1(double theta, double phi) {
  complex c1 = complex(phi, theta);
  complex c2 = complex(-phi, -theta);

  return (exp(c1) + exp(c2)) / 2.0;
}

complex u2(double theta, double phi) {
  complex c1 = complex(phi, theta);
  complex c2 = complex(-phi, -theta);
  complex i2 = complex(0, 2);

  return (exp(c1) - exp(c2)) / i2;
}

int main(int argc, char *argv[]) {
  FILE *out1, *out2;
  char filename[100];
  int i, j, nu, nv, k1, k2;
  int n1, n2;
  double phi, dphi, theta, dtheta;
  double x, y, z, w;
  complex z1, z2, tmp, tmp2;
  double dr, dg, r0, g0, b0;

  if(argc != 5) {
    printf("Usage : sum <n1> <n2> <nu> <nv>\n");
    return(1);
  }

  n1 = atoi(argv[1]);
  n2 = atoi(argv[2]);
  nu = atoi(argv[3]);
  nv = atoi(argv[4]);
  printf("n1 = %d n2 = %d nu = %d, nv = %d\n", n1, n2, nu, nv);

  r0 = 0.2; g0 = 0.2; b0 = 0.3;
  dr = 0.0; dg = 0.0;
  if(n1>1)
    dr = (1.0-r0)/(double)(n1-1);
  if(n2>1)
    dg = (1.0-g0)/(double)(n2-1);

  sprintf(filename, "sum%1d%1d.list", n1, n2);
  if ((out1 = fopen(filename, "w")) == NULL) {
    fprintf(stderr, "error: Could not open file %s\n", filename);
    return(1);
  }
  fprintf(out1, "LIST\n");

  dtheta = 0.5*M_PI/(double) (nu-1);
  dphi = PHI*2.0/(double) (nv-1);

  for(k1=0; k1<n1; k1++) {
    for(k2=0; k2<n2; k2++) {
      sprintf(filename, "sum%1d%1d-%1d%1d.mesh", n1, n2, n1, n2, k1, k2);
      out2 = fopen(filename, "w");
      fprintf(out1, "%s\n", filename);
      fprintf(out2, "4MESHC\n");
      if(k1==0 && k2==0)
	fprintf(out2, "%f %f %f %f\n", 0.2, 0.2, 0.6, 1.0);
      else 
	fprintf(out2, "%f %f %f %f\n", r0+dr*k1, g0+dg*k2, b0, 1.0);
      fprintf(out2, "%d %d\n", nu, nv);
      phi = -PHI;
      for(j=0; j<nv; j++) {
	theta = 0.0;
	for(i=0; i<nu; i++) {
	  z1 = s(k1,n1) * pow(u1(theta, phi), 2.0/(double)n1);
	  z2 = s(k2,n2) * pow(u2(theta, phi), 2.0/(double)n2);
	  theta += dtheta;
	  x = real(z1);
	  y = imag(z1);
	  z = real(z2);
	  w = imag(z2);
	  fprintf(out2, "%f %f %f %f\n", x, y, z, w);
	}
	phi += dphi;
      }
      fclose(out2);
    }
  }
  fclose(out1);
  return 0;
}
