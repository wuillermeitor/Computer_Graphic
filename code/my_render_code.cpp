
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL.h>

#include <cstdio>
#include <iostream>
#include <cassert>
#include <imgui/imgui.h>
#include <ctime>
#include "GL_framework.h"

enum key{u, d, t};

namespace myBox {
	void setupCube();
	void cleanupCube();
	void drawCube();
}
namespace myAxis {
	void setupAxis();
	void cleanupAxis();
	void drawAxis();
}

namespace myCube {
	void setupCube();
	void cleanupCube();
	void drawCubes(glm::vec3 posicion);
}

namespace myRV {
	float FOV = glm::radians(65.f);
	const float zNear = 1.f;
	const float zFar = 50.f;

	const int width = 800;
	const int height = 600;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	float panv[3] = { 0.f, -5.f, -15.f };
	float rota[2] = { 0.f, 0.f };

	float lastTime;
	float deltaTime;
	float timeAcum;
	key lastPressed;
}

bool Pressed1 = false;
bool Pressed2 = false;
bool Pressed3 = false;
bool keyUp = true;


void myInitCode(void) {
	glViewport(0, 0, myRV::width, myRV::height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	myRV::timeAcum = 0;

	float aux = 50.f;
	myRV::_projection = glm::ortho((float)-myRV::width / aux, (float)myRV::width / aux, (float)-myRV::height / aux, (float)myRV::height / aux, myRV::zNear, myRV::zFar);

	//myRV::_projection = glm::perspective(myRV::FOV, (float)myRV::width / (float)myRV::height, myRV::zNear, myRV::zFar);


	// Setup shaders & geometry
	myBox::setupCube();
	myAxis::setupAxis();
	myCube::setupCube();

	//myCube::setupCube();
}

void myCleanupCode(void) {
	myBox::cleanupCube();
	myAxis::cleanupAxis();
	myCube::cleanupCube();
	//MyFirstShader::myCleanupCode();
	//Cube::cleanupCube();
}

void myRenderCode(double currentTime) {
	myRV::deltaTime = currentTime - myRV::lastTime;
	//Nota: Cada vez que cambias de 1 a 2 o viceversa, empieza por donde lo dejó porque el currenTime va aumentando y uso currentTime. Hay que cambiarlo pero de momento voy tirando.

	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

	if (keyboardState[SDL_SCANCODE_1]) {
		if (keyUp) {
			myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
			Pressed1 = true;
			Pressed2 = false;
			Pressed3 = false;
			myRV::timeAcum = 0;
			keyUp = false;
		}
		else {
			myRV::timeAcum += myRV::deltaTime;
		}
	}
	else if (keyboardState[SDL_SCANCODE_2]) {
		if (keyUp) {
			Pressed1 = false;
			Pressed2 = true;
			Pressed3 = false;
			myRV::timeAcum = 0;
			keyUp = false;
		}
		else {
			myRV::timeAcum += myRV::deltaTime;
		}
	}
	else if (keyboardState[SDL_SCANCODE_3]) {
		if (keyUp) {
			Pressed1 = false;
			Pressed2 = false;
			Pressed3 = true;
			myRV::timeAcum = 0;
			keyUp = false;
		}
		else {
			myRV::timeAcum += myRV::deltaTime;
		}
	}
	else {
		myRV::timeAcum += myRV::deltaTime;
		keyUp = true;
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myRV::_modelView = glm::mat4(1.f);
	//myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
	myRV::_modelView = glm::rotate(myRV::_modelView, myRV::rota[1], glm::vec3(1.f, 0.f, 0.f));
	myRV::_modelView = glm::rotate(myRV::_modelView, myRV::rota[0], glm::vec3(0.f, 1.f, 0.f));

	myRV::_MVP = myRV::_projection * myRV::_modelView;

	if (Pressed1) {//A lateral travelling with an orthonormal perspective.
		myRV::panv[2] = -15.f;
		float aux = 50.f;
		myRV::_projection = glm::ortho((float)-myRV::width / aux, (float)myRV::width / aux, (float)-myRV::height / aux, (float)myRV::height / aux, myRV::zNear, myRV::zFar);
		myRV::_modelView = glm::mat4(1.f);
		myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::timeAcum, myRV::panv[1], myRV::panv[2]));

		myRV::_MVP = myRV::_projection * myRV::_modelView;

		myRV::lastPressed = key::u;
	}

	else if (Pressed2) {
		myRV::_projection = glm::perspective(myRV::FOV, (float)myRV::width / (float)myRV::height, myRV::zNear, myRV::zFar);
		myRV::_modelView = glm::mat4(1.f);
		myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
		//a) A close-up in perspective projection. The camera will approach objects in the middle of the scene, with some objects in background. 
		if (myRV::panv[2] <= -5) {
			myRV::panv[2] = -15.f + myRV::timeAcum * 0.5f;
		}
		//b) An increase of the field of view of the camera, with the objects in the middle of the scene, and some objects in background.
		else {
			myRV::FOV = glm::radians(65.f + myRV::timeAcum * 0.5f);
		}
		myRV::_MVP = myRV::_projection * myRV::_modelView;

		myRV::lastPressed = key::d;
	}

	else if (Pressed3) {
		myRV::_projection = glm::perspective(myRV::FOV, (float)myRV::width / (float)myRV::height, myRV::zNear, myRV::zFar);
		myRV::_modelView = glm::mat4(1.f);
		myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
		myRV::panv[2] = -15.f + myRV::timeAcum * 0.5f;
		myRV::FOV = glm::radians(65.f + myRV::timeAcum * 0.5f);
		myRV::_MVP = myRV::_projection * myRV::_modelView;

		myRV::lastPressed = key::t;
	}




	glm::vec3 posicion1(1.0f, 5.0f, 5.0f);
	glm::vec3 posicion2(2.0f, 6.0f, 6.0f);
	glm::vec3 posicion3(3.0f, 7.0f, 7.0f);
	glm::vec3 posicion4(-1.f, 4.0f, 4.0f);
	glm::vec3 posicion5(-2.0f, 3.0f, 3.0f);
	glm::vec3 posicion6(-3.0f, 2.0f, 2.0f);
	// render code
	myBox::drawCube();
	myAxis::drawAxis();
	myCube::drawCubes(posicion1);
	myCube::drawCubes(posicion2);
	myCube::drawCubes(posicion3);
	myCube::drawCubes(posicion4);
	myCube::drawCubes(posicion5);
	myCube::drawCubes(posicion6);

	//MyFirstShader::myRenderCode(currentTime);

	//myCube::draw2Cubes(currentTime);


	myRV::lastTime = currentTime;

	ImGui::Render();
}


//////////////////////////////////// COMPILE AND LINK
GLuint mycompileShader(const char* shaderStr, GLenum shaderType, const char* name = "") {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);
	glCompileShader(shader);
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
void mylinkProgram(GLuint program) {
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char *buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

void GLResize(int width, int height) {
	glViewport(0, 0, width, height);

	if (height != 0) /*myRV::_projection = glm::perspective(myRV::FOV, (float)width / (float)height, myRV::zNear, myRV::zFar);*/
		myRV::_projection = glm::ortho((float)-myRV::width / 50, (float)myRV::width / 50, (float)-myRV::height / 50, (float)myRV::height / 50, myRV::zNear, myRV::zFar);
	else myRV::_projection = glm::perspective(myRV::FOV, 0.f, myRV::zNear, myRV::zFar);
}

////////////////////////////////////////////////// BOX
namespace myBox {
	GLuint cubeVao;
	GLuint cubeVbo[2];
	GLuint cubeShaders[2];
	GLuint cubeProgram;

	float cubeVerts[] = {
		// -5,0,-5 -- 5, 10, 5
		-5.f,  0.f, -5.f,
		5.f,  0.f, -5.f,
		5.f,  0.f,  5.f,
		-5.f,  0.f,  5.f,
		-5.f, 10.f, -5.f,
		5.f, 10.f, -5.f,
		5.f, 10.f,  5.f,
		-5.f, 10.f,  5.f,
	};
	GLubyte cubeIdx[] = {
		1, 0, 2, 3, // Floor - TriangleStrip
		0, 1, 5, 4, // Wall - Lines
		1, 2, 6, 5, // Wall - Lines
		2, 3, 7, 6, // Wall - Lines
		3, 0, 4, 7  // Wall - Lines
	};

	const char* vertShader_xform =
		"#version 330\n\
in vec3 in_Position;\n\
uniform mat4 mvpMat;\n\
void main() {\n\
	gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
}";
	const char* fragShader_flatColor =
		"#version 330\n\
out vec4 out_Color;\n\
uniform vec4 color;\n\
void main() {\n\
	out_Color = color;\n\
}";

	void setupCube() {
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);
		glGenBuffers(2, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 20, cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShaders[0] = mycompileShader(vertShader_xform, GL_VERTEX_SHADER, "cubeVert");
		cubeShaders[1] = mycompileShader(fragShader_flatColor, GL_FRAGMENT_SHADER, "cubeFrag");

		cubeProgram = glCreateProgram();
		glAttachShader(cubeProgram, cubeShaders[0]);
		glAttachShader(cubeProgram, cubeShaders[1]);
		glBindAttribLocation(cubeProgram, 0, "in_Position");
		mylinkProgram(cubeProgram);
	}
	void cleanupCube() {
		glDeleteBuffers(2, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);

		glDeleteProgram(cubeProgram);
		glDeleteShader(cubeShaders[0]);
		glDeleteShader(cubeShaders[1]);
	}
	void drawCube() {
		glBindVertexArray(cubeVao);
		glUseProgram(cubeProgram);
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(myRV::_MVP));
		// FLOOR
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.6f, 0.6f, 0.6f, 1.f);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);
		// WALLS
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.f, 0.f, 0.f, 1.f);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 4));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 8));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 12));
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, (void*)(sizeof(GLubyte) * 16));

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// AXIS
namespace myAxis {
	GLuint AxisVao;
	GLuint AxisVbo[3];
	GLuint AxisShader[2];
	GLuint AxisProgram;

	float AxisVerts[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0
	};
	float AxisColors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0
	};
	GLubyte AxisIdx[] = {
		0, 1,
		2, 3,
		4, 5
	};
	const char* Axis_vertShader =
		"#version 330\n\
in vec3 in_Position;\n\
in vec4 in_Color;\n\
out vec4 vert_color;\n\
uniform mat4 mvpMat;\n\
void main() {\n\
	vert_color = in_Color;\n\
	gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
}";
	const char* Axis_fragShader =
		"#version 330\n\
in vec4 vert_color;\n\
out vec4 out_Color;\n\
void main() {\n\
	out_Color = vert_color;\n\
}";

	void setupAxis() {
		glGenVertexArrays(1, &AxisVao);
		glBindVertexArray(AxisVao);
		glGenBuffers(3, AxisVbo);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisColors, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AxisVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, AxisIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		AxisShader[0] = mycompileShader(Axis_vertShader, GL_VERTEX_SHADER, "AxisVert");
		AxisShader[1] = mycompileShader(Axis_fragShader, GL_FRAGMENT_SHADER, "AxisFrag");

		AxisProgram = glCreateProgram();
		glAttachShader(AxisProgram, AxisShader[0]);
		glAttachShader(AxisProgram, AxisShader[1]);
		glBindAttribLocation(AxisProgram, 0, "in_Position");
		glBindAttribLocation(AxisProgram, 1, "in_Color");
		mylinkProgram(AxisProgram);
	}
	void cleanupAxis() {
		glDeleteBuffers(3, AxisVbo);
		glDeleteVertexArrays(1, &AxisVao);

		glDeleteProgram(AxisProgram);
		glDeleteShader(AxisShader[0]);
		glDeleteShader(AxisShader[1]);
	}
	void drawAxis() {
		glBindVertexArray(AxisVao);
		glUseProgram(AxisProgram);
		glUniformMatrix4fv(glGetUniformLocation(AxisProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(myRV::_MVP));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// CUBE
namespace myCube {
	GLuint cubeVao;
	GLuint cubeVbo[3];
	GLuint cubeShaders[2];
	GLuint cubeProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	glm::vec4 myColor = { 0.0f, 0.5f, 1.0f, 1.0f };

	extern const float halfW = 0.5f;
	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART

						   //   4---------7
						   //  /|        /|
						   // / |       / |
						   //5---------6  |
						   //|  0------|--3
						   //| /       | /
						   //|/        |/
						   //1---------2
	glm::vec3 verts[] = {
		glm::vec3(-halfW, -halfW, -halfW),
		glm::vec3(-halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW, -halfW),
		glm::vec3(-halfW,  halfW, -halfW),
		glm::vec3(-halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW, -halfW)
	};
	glm::vec3 norms[] = {
		glm::vec3(0.f, -1.f,  0.f),
		glm::vec3(0.f,  1.f,  0.f),
		glm::vec3(-1.f,  0.f,  0.f),
		glm::vec3(1.f,  0.f,  0.f),
		glm::vec3(0.f,  0.f, -1.f),
		glm::vec3(0.f,  0.f,  1.f)
	};

	glm::vec3 cubeVerts[] = {
		verts[1], verts[0], verts[2], verts[3],
		verts[5], verts[6], verts[4], verts[7],
		verts[1], verts[5], verts[0], verts[4],
		verts[2], verts[3], verts[6], verts[7],
		verts[0], verts[4], verts[3], verts[7],
		verts[1], verts[2], verts[5], verts[6]
	};
	glm::vec3 cubeNorms[] = {
		norms[0], norms[0], norms[0], norms[0],
		norms[1], norms[1], norms[1], norms[1],
		norms[2], norms[2], norms[2], norms[2],
		norms[3], norms[3], norms[3], norms[3],
		norms[4], norms[4], norms[4], norms[4],
		norms[5], norms[5], norms[5], norms[5]
	};
	GLubyte cubeIdx[] = {
		0, 1, 2, 3, UCHAR_MAX,
		4, 5, 6, 7, UCHAR_MAX,
		8, 9, 10, 11, UCHAR_MAX,
		12, 13, 14, 15, UCHAR_MAX,
		16, 17, 18, 19, UCHAR_MAX,
		20, 21, 22, 23, UCHAR_MAX
	};




	const char* cube_vertShader =
		"#version 330\n\
	in vec3 in_Position;\n\
	in vec3 in_Normal;\n\
	out vec4 vert_Normal;\n\
	uniform mat4 objMat;\n\
	uniform mat4 mv_Mat;\n\
	uniform mat4 mvpMat;\n\
	void main() {\n\
		gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
		vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
	}";


	const char* cube_fragShader =
		"#version 330\n\
in vec4 vert_Normal;\n\
out vec4 out_Color;\n\
uniform mat4 mv_Mat;\n\
uniform vec4 color;\n\
void main() {\n\
	out_Color = vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );\n\
}";
	void setupCube() {
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);
		glGenBuffers(3, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glPrimitiveRestartIndex(UCHAR_MAX);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShaders[0] = mycompileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
		cubeShaders[1] = mycompileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		cubeProgram = glCreateProgram();
		glAttachShader(cubeProgram, cubeShaders[0]);
		glAttachShader(cubeProgram, cubeShaders[1]);
		glBindAttribLocation(cubeProgram, 0, "in_Position");
		glBindAttribLocation(cubeProgram, 1, "in_Normal");
		mylinkProgram(cubeProgram);
	}
	void cleanupCube() {
		glDeleteBuffers(3, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);

		glDeleteProgram(cubeProgram);
		glDeleteShader(cubeShaders[0]);
		glDeleteShader(cubeShaders[1]);
	}

	void drawCubes(glm::vec3 posicion) {

		glEnable(GL_PRIMITIVE_RESTART);
		glBindVertexArray(cubeVao);
		glUseProgram(cubeProgram);

		glm::mat4 t = glm::translate(glm::mat4(1.0f), posicion);

		objMat = t;

		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(myRV::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(myRV::_MVP));
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), myColor.r, myColor.g, myColor.b, myColor.a);
		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
		glDisable(GL_PRIMITIVE_RESTART);
	}

}