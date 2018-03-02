
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cstdio>
#include <cassert>

#include "GL_framework.h"

void myRenderCode(double currentTime) {



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

	glm::mat4 proj;
	proj = glm::perspective(glm::radians(65.f), (float)512 / (float)512, 0.1f, 100.f);
}