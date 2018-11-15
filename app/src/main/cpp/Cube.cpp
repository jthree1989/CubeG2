#include "Cube.h"
#include <string.h>

Cube::Cube(GLfloat pVertices[], GLbyte pIndices[]) {
	memcpy(vertices, pVertices, 108*sizeof(GLfloat));
	memcpy(indices, pIndices, 6*sizeof(GLbyte));
}
void Cube::draw(GLuint pvPositionHandle) {
	glVertexAttribPointer(pvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(pvPositionHandle);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

Cube::~Cube() {

}


