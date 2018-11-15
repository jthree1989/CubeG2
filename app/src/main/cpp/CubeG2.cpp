#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <math.h>
#include <stdlib.h>

#include "Cube.h"
#include "matrix.h"
#include "mylog.h"

void naInitGL20(JNIEnv* env, jclass clazz, jstring vertexShaderStr, jstring fragmentShaderStr);
void naDrawGraphics(JNIEnv* env, jclass clazz, float pAngleX, float pAngleY);
void naSurfaceChanged(JNIEnv* env, jclass clazz, int width, int height);

jint JNI_OnLoad(JavaVM* pVm, void* reserved)
{
	JNIEnv* env;
	if (pVm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
		 return -1;
	}
	JNINativeMethod nm[4];
	nm[0].name = "naInitGL20";
	nm[0].signature = "(Ljava/lang/String;Ljava/lang/String;)V";
	nm[0].fnPtr = (void*)naInitGL20;
	nm[1].name = "naDrawGraphics";
	nm[1].signature = "(FF)V";
	nm[1].fnPtr = (void*)naDrawGraphics;
	nm[2].name = "naSurfaceChanged";
	nm[2].signature = "(II)V";
	nm[2].fnPtr = (void*)naSurfaceChanged;
	jclass cls = env->FindClass("com/example/armyc/cubeg2/MyRenderer");
	env->RegisterNatives(cls, nm, 3);
	return JNI_VERSION_1_6;
}


GLfloat vertices[108] = {
		-1.0,-1.0,1.0,  1.0,1.0,1.0,   1.0,-1.0,1.0,    //465
		-1.0,-1.0,1.0,  -1.0,1.0,1.0,  1.0,1.0,1.0,     //476
		1.0,-1.0,-1.0,   -1.0,1.0,-1.0,   -1.0,-1.0,-1.0,//130
		1.0,-1.0,-1.0,   1.0,1.0,-1.0,    -1.0,1.0,-1.0, //123
		1.0,-1.0,1.0,   1.0,1.0,-1.0,   1.0,-1.0,-1.0,   //521
		1.0,-1.0,1.0,   1.0,1.0,1.0,    1.0,1.0,-1.0,    //562
		-1.0,-1.0,-1.0,  -1.0,1.0,1.0,  -1.0,-1.0,1.0,   //074
		-1.0,-1.0,-1.0,  -1.0,1.0,-1.0, -1.0,1.0,1.0,    //037
		-1.0,-1.0,-1.0,   1.0,-1.0,1.0,   1.0,-1.0,-1.0, //051
		-1.0,-1.0,-1.0,  -1.0,-1.0,1.0,   1.0,-1.0,1.0,  //045
		-1.0,1.0,1.0,    1.0,1.0,-1.0,  1.0,1.0,1.0,    //726
		-1.0,1.0,1.0,   -1.0,1.0,-1.0,  1.0,1.0,-1.0  //732
};
GLbyte indices[6] = {
	0, 1, 2, 3, 4, 5
};
Cube mCube(vertices, indices);

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE(1, "Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE(1, "Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint gProgram;

void setupShaders(const char *vetexShader, const char *fragmentShader) {
	gProgram = createProgram(vetexShader, fragmentShader);
	if (!gProgram) {
		LOGE(1, "Could not create program.");
		return;
	}
}
GLuint gvPositionHandle;
GLuint gmvP;
void naInitGL20(JNIEnv* env, jclass clazz, jstring vertexShaderStr, jstring fragmentShaderStr) {
	glDisable(GL_DITHER);	//disable dither to improve performance with reduced quality
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	//set clear value for color buffer as black
//	glEnable(GL_CULL_FACE);		//enabled for better performance
	glClearDepthf(1.0f);	//set clear value [0, 1] for depth buffer as farthest
	glEnable(GL_DEPTH_TEST);	//do depth comparison and update depth buffer
	glDepthFunc(GL_LEQUAL);		//type of depth test

    const char *vetexStr, *fragmentStr;
	vetexStr = env->GetStringUTFChars(vertexShaderStr, NULL);
	fragmentStr = env->GetStringUTFChars(fragmentShaderStr, NULL);
	setupShaders(vetexStr, fragmentStr);
	env->ReleaseStringUTFChars(vertexShaderStr, vetexStr);
	env->ReleaseStringUTFChars(fragmentShaderStr, fragmentStr);

	gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
	LOGI(1, "glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);

	gmvP = glGetUniformLocation(gProgram, "mvp");
	LOGI(1, "glGetUniformLocation(\"mvp\") = %d\n", gmvP);
}

int gWidth, gHeight;
void naSurfaceChanged(JNIEnv* env, jclass clazz, int width, int height) {
	glViewport(0, 0, width, height);
	gWidth = width;
	gHeight = height;
}

int iXangle = 0, iYangle = 0, iZangle = 0;
float aRotate[16], aScale[16], aTranslate[16], aModelView[16], aPerspective[16], aMVP[16];

void naDrawGraphics(JNIEnv* env, jclass clazz, float pAngleX, float pAngleY) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glUseProgram(gProgram);

//	GL1x: glRotatef(pAngleX, 0, 1, 0);	//rotate around y-axis
//	GL1x: glRotatef(pAngleY, 1, 0, 0);	//rotate around x-axis
	//rotate
	rotate_matrix(pAngleX, 0.0, 1.0, 0.0, aRotate);
	rotate_matrix(pAngleY, 1.0, 0.0, 0.0, aModelView);
	multiply_matrix(aRotate, aModelView, aModelView);

//	GL1x: glScalef(0.3f, 0.3f, 0.3f);      // Scale down
	scale_matrix(0.5, 0.5, 0.5, aScale);
	multiply_matrix(aScale, aModelView, aModelView);

// GL1x: glTranslate(0.0f, 0.0f, -3.5f);
	translate_matrix(0.0f, 0.0f, -3.5f, aTranslate);
	multiply_matrix(aTranslate, aModelView, aModelView);

//	gluPerspective(45, aspect, 0.1, 100);
	perspective_matrix(45.0, (float)gWidth/(float)gHeight, 0.1, 100.0, aPerspective);
	multiply_matrix(aPerspective, aModelView, aMVP);

	glUniformMatrix4fv(gmvP, 1, GL_FALSE, aMVP);

	mCube.draw(gvPositionHandle);
}



