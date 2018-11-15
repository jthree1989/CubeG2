#ifndef CUBE_H_
#define CUBE_H_

#include <GLES2/gl2.h>

class Cube {
public:
	GLfloat vertices[108];
	GLbyte indices[6];
public:
	Cube(GLfloat[], GLbyte[]);
	void draw(GLuint);
	virtual ~Cube();
};

#endif /* CUBE_H_ */
