
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
}

bool Pressed1 = false;
bool Pressed2 = false;
bool Pressed3 = false;








void myInitCode(void) {
	glViewport(0, 0, myRV::width, myRV::height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	float aux = 50.f;
	myRV::_projection = glm::ortho((float)-myRV::width / aux, (float)myRV::width / aux, (float)-myRV::height / aux, (float)myRV::height / aux, myRV::zNear, myRV::zFar);

	//myRV::_projection = glm::perspective(myRV::FOV, (float)myRV::width / (float)myRV::height, myRV::zNear, myRV::zFar);


	// Setup shaders & geometry
	myBox::setupCube();
	myAxis::setupAxis();
	//Cube::setupCube();

	//myCube::setupCube();
}

void myCleanupCode(void) {
	myBox::cleanupCube();
	myAxis::cleanupAxis();
	//Cube::cleanupCube();
	//MyFirstShader::myCleanupCode();
	//Cube::cleanupCube();
}

void myRenderCode(double currentTime) {
	//Nota: Cada vez que cambias de 1 a 2 o viceversa, empieza por donde lo dejó porque el currenTime va aumentando y uso currentTime. Hay que cambiarlo pero de momento voy tirando.

	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if (keyboardState[SDL_SCANCODE_1]) {
		myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
		Pressed1 = true;
		Pressed2 = false;
		Pressed3 = false;
	}
	else if (keyboardState[SDL_SCANCODE_2]) {
		Pressed1 = false;
		Pressed2 = true;
		Pressed3 = false;
	}
	else if (keyboardState[SDL_SCANCODE_3]) {
		std::cout << "3" << std::endl;
		Pressed1 = false;
		Pressed2 = false;
		Pressed3 = true;
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myRV::_modelView = glm::mat4(1.f);
	myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
	myRV::_modelView = glm::rotate(myRV::_modelView, myRV::rota[1], glm::vec3(1.f, 0.f, 0.f));
	myRV::_modelView = glm::rotate(myRV::_modelView, myRV::rota[0], glm::vec3(0.f, 1.f, 0.f));

	myRV::_MVP = myRV::_projection * myRV::_modelView;

	if (Pressed1) {//A lateral travelling with an orthonormal perspective.
		myRV::panv[2] = -15.f;
		float aux = 50.f;
		myRV::_projection = glm::ortho((float)-myRV::width / aux, (float)myRV::width / aux, (float)-myRV::height / aux, (float)myRV::height / aux, myRV::zNear, myRV::zFar);
		myRV::_modelView = glm::mat4(1.f);
		myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(currentTime, myRV::panv[1], myRV::panv[2]));

		myRV::_MVP = myRV::_projection * myRV::_modelView;
	}

	if (Pressed2) {
		myRV::_projection = glm::perspective(myRV::FOV, (float)myRV::width / (float)myRV::height, myRV::zNear, myRV::zFar);
		myRV::_modelView = glm::mat4(1.f);
		myRV::_modelView = glm::translate(myRV::_modelView, glm::vec3(myRV::panv[0], myRV::panv[1], myRV::panv[2]));
		//a) A close-up in perspective projection. The camera will approach objects in the middle of the scene, with some objects in background. 
		if (myRV::panv[2] <= -5) {
			myRV::panv[2] = -15.f+currentTime * 0.5f;
		}
		//b) An increase of the field of view of the camera, with the objects in the middle of the scene, and some objects in background.
		else {
			myRV::FOV = glm::radians(65.f + currentTime * 0.5f);
		}
		myRV::_MVP = myRV::_projection * myRV::_modelView;
	}

	// render code
	myBox::drawCube();
	myAxis::drawAxis();
	//Cube::drawCube();

	//MyFirstShader::myRenderCode(currentTime);

	//myCube::draw2Cubes(currentTime);


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

