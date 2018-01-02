#define GEOM_C
#include "all.h"

Vector3 Cross3(Vector3 *v1, Vector3 *v2) {
  Vector3 tmp;

  tmp.d[0] = v1->d[1] * v2->d[2] - v2->d[1] * v1->d[2];
  tmp.d[1] = v2->d[0] * v1->d[2] - v1->d[0] * v2->d[2];
  tmp.d[2] = v1->d[0] * v2->d[1] - v2->d[0] * v1->d[1];

  return tmp;
}

double Dot3(Vector3 *v1, Vector3 *v2) {
  return (  v1->d[0] * v2->d[0] 
	  + v1->d[1] * v2->d[1] 
	  + v1->d[2] * v2->d[2] );
}  

double Dot4(Vector4 *v1, Vector4 *v2) {
  return (  v1->d[0] * v2->d[0] 
	  + v1->d[1] * v2->d[1] 
	  + v1->d[2] * v2->d[2]
	  + v1->d[3] * v2->d[3] );
}  

void Normalize3(Vector3 *v) {
  double d, d0, d1, d2;

  d0 = v->d[0];
  d1 = v->d[1];
  d2 = v->d[2];
  d = sqrt(d0*d0+d1*d1+d2*d2);

  if(d < TINYEPSILON) {
/*    printf("Warning : try to normalize 3d zero vector.\n"); */
  }
  else {
    d = 1.0 / d;
    v->d[0] = d0*d;
    v->d[1] = d1*d;
    v->d[2] = d2*d;
  }
}

Vector3 Normalize3R(Vector3 *v) {
  Vector3 tmp;
  double d0, d1, d2, d;

  d0 = v->d[0];
  d1 = v->d[1];
  d2 = v->d[2];
  d = sqrt(d0*d0+d1*d1+d2*d2);

  if(d < TINYEPSILON) {
/*    printf("Warning : try to normalize 3d zero vector\n"); */
    return *v;
  }
  else {
    d = 1.0 / d;
    tmp.d[0] = d0 * d;
    tmp.d[1] = d1 * d;
    tmp.d[2] = d2 * d;
    return tmp;
  }
}

Vector3 PlaneNormal(Point3 *p0, Point3 *p1, Point3 *p2) {
  Vector3 v0, v1, v;

  v0.d[0] = p1->d[0] - p0->d[0];
  v0.d[1] = p1->d[1] - p0->d[1];
  v0.d[2] = p1->d[2] - p0->d[2];

  v1.d[0] = p2->d[0] - p0->d[0];
  v1.d[1] = p2->d[1] - p0->d[1];
  v1.d[2] = p2->d[2] - p0->d[2];

  v = Cross3(&v0, &v1);
  return(Normalize3R(&v));
}

void Vector3TimesDouble(Vector3 *v, double s) {
  v->d[0] *= s;
  v->d[1] *= s;
  v->d[2] *= s;
}

Vector3 Vector3TimesDoubleR(Vector3 *v, double s) {
  Vector3 tmpv;

  tmpv.d[0] = v->d[0] * s;
  tmpv.d[1] = v->d[1] * s;
  tmpv.d[2] = v->d[2] * s;
  return tmpv;
}

void Vector4TimesDouble(Vector4 *v, double s) {
  v->d[0] *= s;
  v->d[1] *= s;
  v->d[2] *= s;
  v->d[3] *= s;
}

Vector4 Vector4TimesDoubleR(Vector4 *v, double s) {
  Vector4 tmpv;

  tmpv.d[0] = v->d[0] * s;
  tmpv.d[1] = v->d[1] * s;
  tmpv.d[2] = v->d[2] * s;
  tmpv.d[3] = v->d[3] * s;
  return tmpv;
}

void PointMinusPoint3(Point3 *p1, Point3 *p2, Vector3 *v) {
  v->d[0] = p1->d[0] - p2->d[0];
  v->d[1] = p1->d[1] - p2->d[1];
  v->d[2] = p1->d[2] - p2->d[2];
}

Vector3 PointMinusPoint3R(Point3 *p1, Point3 *p2) {
  Vector3 tmp;

  tmp.d[0] = p1->d[0] - p2->d[0];
  tmp.d[1] = p1->d[1] - p2->d[1];
  tmp.d[2] = p1->d[2] - p2->d[2];
  return tmp;
}

Point3 PointAddVector3(Point3 *p, Vector3 *v) {
  Point3 tmp;
  tmp.d[0] = p->d[0] + v->d[0];
  tmp.d[1] = p->d[1] + v->d[1];
  tmp.d[2] = p->d[2] + v->d[2];
  return tmp;
}

void PointAddVector3R(Point3 *p, Vector3 *v) {
  p->d[0] += v->d[0];
  p->d[1] += v->d[1];
  p->d[2] += v->d[2];
}

Point4 PointAddVector4(Point4 *p, Vector4 *v) {
  Point4 tmp;
  tmp.d[0] = p->d[0] + v->d[0];
  tmp.d[1] = p->d[1] + v->d[1];
  tmp.d[2] = p->d[2] + v->d[2];
  tmp.d[3] = p->d[3] + v->d[3];
  return tmp;
}

void PointAddVector4R(Point4 *p, Vector4 *v) {
  p->d[0] += v->d[0];
  p->d[1] += v->d[1];
  p->d[2] += v->d[2];
  p->d[3] += v->d[3];
}

Vector3 Point3MinusPoint3(Point3 *p, Point3 *q) {
  Vector3 v;

  v.d[0] = p->d[0] - q->d[0];
  v.d[1] = p->d[1] - q->d[1];
  v.d[2] = p->d[2] - q->d[2];
  return v;
}

Vector4 Point4MinusPoint4(Point4 *p, Point4 *q) {
  Vector4 v;

  v.d[0] = p->d[0] - q->d[0];
  v.d[1] = p->d[1] - q->d[1];
  v.d[2] = p->d[2] - q->d[2];
  v.d[3] = p->d[3] - q->d[3];
  return v;
}

Vector3 PointMinusVertex3(Point3 *p, Vertex *q) {
  Vector3 v;

  v.d[0] = p->d[0] - q->p3.d[0];
  v.d[1] = p->d[1] - q->p3.d[1];
  v.d[2] = p->d[2] - q->p3.d[2];
  return v;
}

Vector3 VertexMinusVertex3(Vertex *p, Vertex *q) {
  Vector3 v;

  v.d[0] = p->p3.d[0] - q->p3.d[0];
  v.d[1] = p->p3.d[1] - q->p3.d[1];
  v.d[2] = p->p3.d[2] - q->p3.d[2];
  return v;
}

Vector4 VertexMinusVertex4(Vertex *p, Vertex *q) {
  Vector4 v;

  v.d[0] = p->p4.d[0] - q->p4.d[0];
  v.d[1] = p->p4.d[1] - q->p4.d[1];
  v.d[2] = p->p4.d[2] - q->p4.d[2];
  v.d[3] = p->p4.d[3] - q->p4.d[3];
  return v;
}

Point4 Matrix4TimesPoint4(Matrix4 *mat, Point4 *p) {
  Point4 tmp;
    
  tmp.d[0] = mat->d[0]  * p->d[0] + mat->d[4]  * p->d[1]
           + mat->d[8]  * p->d[2] + mat->d[12] * p->d[3];
  tmp.d[1] = mat->d[1]  * p->d[0] + mat->d[5]  * p->d[1]
           + mat->d[9]  * p->d[2] + mat->d[13] * p->d[3];
  tmp.d[2] = mat->d[2]  * p->d[0] + mat->d[6]  * p->d[1]
           + mat->d[10] * p->d[2] + mat->d[14] * p->d[3];
  tmp.d[3] = mat->d[3]  * p->d[0] + mat->d[7]  * p->d[1]
           + mat->d[11] * p->d[2] + mat->d[15] * p->d[3];
  return tmp;
}
  
Point3 Matrix3TimesPoint3(MatrixGL *mat, Point3 *p) {
  Point3 tmp;
    
  tmp.d[0] = mat->d[0]  * p->d[0] + mat->d[4]  * p->d[1]
           + mat->d[8]  * p->d[2];
  tmp.d[1] = mat->d[1]  * p->d[0] + mat->d[5]  * p->d[1]
           + mat->d[9]  * p->d[2];
  tmp.d[2] = mat->d[2]  * p->d[0] + mat->d[6]  * p->d[1]
           + mat->d[10] * p->d[2];
  return tmp;
}
  
Matrix4 Matrix4TimesMatrix4(Matrix4 *m1, Matrix4 *m2) {
  Matrix4 tmp;
  int i;

  for(i=0; i<4; i++) {
      tmp.d[i]    = m1->d[i]   * m2->d[0]  + m1->d[i+4]  * m2->d[1]
	          + m1->d[i+8] * m2->d[2]  + m1->d[i+12] * m2->d[3];
      tmp.d[i+4]  = m1->d[i]   * m2->d[4]  + m1->d[i+4]  * m2->d[5]
	          + m1->d[i+8] * m2->d[6]  + m1->d[i+12] * m2->d[7];
      tmp.d[i+8]  = m1->d[i]   * m2->d[8]  + m1->d[i+4]  * m2->d[9]
	          + m1->d[i+8] * m2->d[10] + m1->d[i+12] * m2->d[11];
      tmp.d[i+12] = m1->d[i]   * m2->d[12] + m1->d[i+4]  * m2->d[13]
	          + m1->d[i+8] * m2->d[14] + m1->d[i+12] * m2->d[15];
    }
  return tmp;
}
  
Matrix4 MatrixGLTimesMatrix4(MatrixGL *m1, Matrix4 *m2) {
  Matrix4 tmp;
  int i;

  for(i=0; i<4; i++) {
      tmp.d[i]    = m1->d[i]   * m2->d[0]  + m1->d[i+4]  * m2->d[1]
	          + m1->d[i+8] * m2->d[2]  + m1->d[i+12] * m2->d[3];
      tmp.d[i+4]  = m1->d[i]   * m2->d[4]  + m1->d[i+4]  * m2->d[5]
	          + m1->d[i+8] * m2->d[6]  + m1->d[i+12] * m2->d[7];
      tmp.d[i+8]  = m1->d[i]   * m2->d[8]  + m1->d[i+4]  * m2->d[9]
	          + m1->d[i+8] * m2->d[10] + m1->d[i+12] * m2->d[11];
      tmp.d[i+12] = m1->d[i]   * m2->d[12] + m1->d[i+4]  * m2->d[13]
	          + m1->d[i+8] * m2->d[14] + m1->d[i+12] * m2->d[15];
    }
  return tmp;
}
  
Matrix4 MatrixRot4XY(double cos_theta, double sin_theta) {
  Matrix4 tmp;
  
  tmp = IMat4;
  tmp.d[0] = tmp.d[5] = cos_theta;
  tmp.d[1] = sin_theta;
  tmp.d[4] = - sin_theta;
  return tmp;
}

Matrix4 MatrixRot4XZ(double cos_theta, double sin_theta) {
  Matrix4 tmp;
  
  tmp = IMat4;
  tmp.d[0] = tmp.d[10] = cos_theta;
  tmp.d[2] = sin_theta;
  tmp.d[8] = - sin_theta;
  
  return tmp;
}

Matrix4 MatrixRot4XW(double cos_theta, double sin_theta) {
  Matrix4 tmp;
  
  tmp = IMat4;
  tmp.d[0]  = tmp.d[15] = cos_theta;
  tmp.d[3]  = sin_theta;
  tmp.d[12] = - sin_theta;
  
  return tmp;
}

Matrix4 MatrixRot4YZ(double cos_theta, double sin_theta) {
  Matrix4 tmp;
  
  tmp = IMat4;
  tmp.d[5] = tmp.d[10] = cos_theta;
  tmp.d[6] = sin_theta;
  tmp.d[9] = - sin_theta;
  
  return tmp;
}

Matrix4 MatrixRot4YW(double cos_theta, double sin_theta) {
  Matrix4 tmp;
  
  tmp = IMat4;
  tmp.d[5]  = tmp.d[15] = cos_theta;
  tmp.d[7]  = sin_theta;
  tmp.d[13] = - sin_theta;
  
  return tmp;
}

Matrix4 MatrixRot4ZW(double cos_theta, double sin_theta) {
  Matrix4 tmp;
  
  tmp = IMat4;
  tmp.d[10] = tmp.d[15] = cos_theta;
  tmp.d[11] = sin_theta;
  tmp.d[14] = - sin_theta;
  
  return tmp;
}
