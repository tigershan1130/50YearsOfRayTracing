#include <iostream>

#include "DemoScene.h"
#include "DirectLightingRenderer.h"
#include "framework/MyApp.h"
#include "framework/ThinLensCamera.h"

using namespace RayTracingHistory;

const char* const APP_NAME = "Path Tracing -  Kajiya 1986";
const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

int main(void) {
  MyApp app;
  try {
    app.init();
    app.createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME);
    auto renderer = app.createRenderer<DirectLightingRenderer>();

    auto scene = std::make_shared<DemoScene>();
    scene->init();

    glm::vec3 eyePos(0, 1.25f, -8);
    glm::vec3 lookAt(0, 1, 0);
    auto camera = std::make_shared<ThinLensCamera>();
    camera->setAperture(0.01f)
        .setFocusDist(glm::distance(eyePos, lookAt))
        .setAspect((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT)
        .setView(eyePos, lookAt, glm::vec3(0, 1, 0))
        .setFOV(45);

    renderer->renderScene(scene, camera, glm::vec4(1, 1, 1, 1));

    app.mainLoop();

  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  app.shutdown();
  return 0;
}