
#include "application.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shader.hpp"

void programQuit([[maybe_unused]] int key, [[maybe_unused]] int action, Application *app) {
  app->close();
  LOG_S(INFO) << "Quiting...";
}
std::vector<glm::vec3> getCoordsForVertices(double xc, double yc, double size, int n) {
  std::vector<glm::vec3> vertices;
  auto xe = xc + size;
  auto ye = yc;
  vertices.emplace_back(xe, yc, ye);
  double alpha = 0;
  for (int i = 0; i < n - 1; i++) {
	alpha += 2 * M_PI / n;
	auto xr = xc + size * cos(alpha);
	auto yr = yc + size * sin(alpha);
	xe = xr;
	ye = yr;
	vertices.emplace_back(xe, yc, ye);
  }
  return vertices;
}
int main(int argc, char *argv[]) {
  Application app({640, 480}, argc, argv);
  Application::setOpenGLFlags();
  app.registerKeyCallback(GLFW_KEY_ESCAPE, programQuit);
  Shader shader("../shaders/basic_lighting_shader.glsl", true);
  Mesh mesh("../resources/models/Crate1.obj");
  mesh.setTexture("../resources/crate_1.png");
  Camera camera(app.getWindow()->getWindowSize());

  camera.moveTo({0, 0, 4});
  camera.lookAt({0, 0, 0.1});
  shader.setUniform3f("lightPos", {0,0,4});
  shader.setUniform3f("lightColor", {1, 1, 1});
  shader.setUniform3f("objectColor", {0.1,0.1,0.1});
  shader.bind();
  //shader.setUniform1i("u_Texture", 0);
  mesh.compile();
  std::vector<glm::vec3> cameraPositions = getCoordsForVertices(0, 0, 2, 100);/// координаты для точек гиперболойды
  int cameraPosition{0};
  while (!app.getShouldClose()) {
	camera.setModel(glm::rotate(camera.getModel(), 0.006f, {0, 1, 0}));
	shader.setUniformMat4f("model", camera.getModel());
	shader.setUniformMat4f("view", camera.getView());
	shader.setUniform3f("lightPos", cameraPositions[cameraPosition]);
	shader.setUniformMat4f("projection", camera.getProjection());
	shader.reload();
	Renderer::clear({0, 0, 0, 1});
	mesh.draw(&shader);
	cameraPosition++;
	if (cameraPosition >= cameraPositions.size()) {
	  cameraPosition = 0;
	}
	glCall(glfwSwapBuffers(app.getWindow()->getGLFWWindow()));
	glfwPollEvents();
  }
  glfwTerminate();
  exit(EXIT_SUCCESS);
}