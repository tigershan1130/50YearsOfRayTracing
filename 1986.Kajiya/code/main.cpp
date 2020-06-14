#include <iostream>

#include "CornellBoxScene.h"
#include "DemoScene.h"
#include "MonteCarloPathTracer.h"
#include "MoriKnobScene.h"
#include "framework/MyApp.h"
#include "framework/PinholeCamera.h"

using namespace RayTracingHistory;

// const char* const APP_NAME = "Path Tracing -  Kajiya 1986";
const char* const APP_NAME = "Monte Carlo Path Tracing";

const uint32_t WINDOW_WIDTH = 1280;
const uint32_t WINDOW_HEIGHT = 720;

enum class EMyTestScenes { Demo, CornellBox, MoriKnob };

int main(int argc, char* argv[]) {
  MyApp app;
  try {
    app.init();
    app.createWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME);
    auto renderer = app.createRenderer<MonteCarloPathTracer>();

    EMyTestScenes sceneType = EMyTestScenes::MoriKnob;
    MyScene::Ptr scene;
    glm::vec3 eyePos;
    glm::vec3 lookAt;

    switch (sceneType) {
      case EMyTestScenes::CornellBox:
        scene = std::make_shared<CornellBoxScene>();
        eyePos = glm::vec3(278, 278, -656);
        lookAt = glm::vec3(278, 278, 0);
        break;
      case EMyTestScenes::MoriKnob:
        scene = std::make_shared<MoriKnobScene>();
        eyePos = glm::vec3(0, 1, -2);
        lookAt = glm::vec3(0, 0, 0);
        break;
      case EMyTestScenes::Demo:
      default:
        scene = std::make_shared<DemoScene>();
        eyePos = glm::vec3(0, 250, -850);
        lookAt = glm::vec3(0, 250, 0);
        break;
    }  // end of scene
    scene->init();

    auto camera = std::make_shared<PinholeCamera>();
    camera->setZNear(0.1f)
        .setAspect((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT)
        .setView(eyePos, lookAt, glm::vec3(0, 1, 0))
        .setFOV(45);
    camera->init();

    renderer->renderScene(scene, camera, glm::vec4(1, 1, 1, 1));

    app.mainLoop();

  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  app.shutdown();
  return 0;
}