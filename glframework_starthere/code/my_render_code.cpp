
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cstdio>
#include <cassert>
#include <imgui/imgui.h>
#include "GL_framework.h"

namespace myRV {
	const float FOV = glm::radians(65.f);
	const float zNear = 1.f;
	const float zFar = 50.f;
	const int width = 512;
	const int height = 512;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	float panv[3] = { 0.f, -5.f, -15.f };
	float rota[2] = { 0.f, 0.f };
}

void myRenderCode(double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myRV::_modelView = glm::mat4(1.f);
	myRV::_MVP = myRV::_projection * myRV::_modelView;


	ImGui::Render();
}
void myCleanupCode(void) {

}
void myInitCode(void) {
	glViewport(0, 0, 512, 512);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	myRV::_projection = glm::perspective(myRV::FOV, (float)myRV::width / (float)myRV::height, myRV::zNear, myRV::zFar);
}