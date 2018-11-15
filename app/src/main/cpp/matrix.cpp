#include "matrix.h"

//all matrix are in column-major order
//the 4x4 matrix are stored in an array as shown below
//0-15 indicates the array index
//0	4	8	12
//1	5	9	13
//2	6	10	14
//3	7	11	15


void load_identity(float *M) {
	for (int i = 0; i < 16; ++i) {
		M[i] = 0.0f;
	}
	M[0] = 1.0f;
	M[5] = 1.0f;
	M[10] = 1.0f;
	M[15] = 1.0f;
}

//return translation matrix
//	1	0	0	tx
//	0	1	0	ty
//	0	0	1	tz
//	0	0	0	1
void translate_matrix(float tx, float ty, float tz, float *M) {
	load_identity(M);
    M[12] = tx;
    M[13] = ty;
    M[14] = tz;
}

//return scaling matrix
//	sx	0	0	0
//	0	sy	0	0
//	0	0	sz	0
//	0	0	0	1
void scale_matrix(float sx, float sy, float sz, float *M) {
	load_identity(M);
	M[0] *= sx;
	M[5] *= sy;
	M[10] *= sz;
}

//return rotation matrix
//R = Rx*Ry*Rz
//			1	0			0		0
//	Rx = 	0	cosx		-sinx	0
//			0	sinx		cosx	0
//			0	0			0		1

//		cosy		0		siny		0
//		0			1		0			0
//	Ry =-siny		0		cosy		0
//		0			0		0			1
//
//		cosz		-sinz	0		0
//	Rz=	sinz		cosz 	0		0
//		0			0		1		0
//		0			0		0		1
//refer to http://lignumcad.sourceforge.net/doc/en/HTML/SOHTML/TechnicalReference.html
//for detailed info

void rotate_matrix(float angle, float x, float y, float z, float *M) {
    double radians, c, s, c1, u[3], length;
    int i, j;

    radians = (angle * MYPI) / 180.0;
    c = cos(radians);
    s = sin(radians);
    c1 = 1.0 - cos(radians);
    length = sqrt(x * x + y * y + z * z);
    u[0] = x / length;
    u[1] = y / length;
    u[2] = z / length;

    for (i = 0; i < 16; i++) {
        M[i] = 0.0;
    }
    M[15] = 1.0;

    for (i = 0; i < 3; i++) {
        M[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
        M[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            M[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
        }
    }
}


/* simulate gluPerspectiveMatrix
 * //return perspective projection matrix
 *	cot(fovy/2) / aspect		0				0					0
 *	0						cot(fovy/2)			0					0
 *	0							0		(znear+zfar)/(znear-zfar)	2*znear*zfa/(znear-zfar)
 *	0							0				-1					0
 */
void perspective_matrix(float fovy, float aspect, float znear, float zfar, float *M) {
    int i;
    double f;

    load_identity(M);

    f = 1.0/tan(fovy * 0.5);

    M[0] = f / aspect;
    M[5] = f;
    M[10] = (znear + zfar) / (znear - zfar);
    M[11] = -1.0;
    M[14] = (2.0 * znear * zfar) / (znear - zfar);
    M[15] = 0.0;
}

//Multiplies A by B and output to C, a tmp buffer is used to support in-place multiplication
void multiply_matrix(float *A, float *B, float *C) {
	int i, j, k;
    float tmpM[16];

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
        	tmpM[j * 4 + i] = 0.0;
            for (k = 0; k < 4; k++) {
            	tmpM[j * 4 + i] += A[k * 4 + i] * B[j * 4 + k];
            }
        }
    }
    for (i = 0; i < 16; i++) {
        C[i] = tmpM[i];
    }
}
