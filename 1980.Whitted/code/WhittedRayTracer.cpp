#include "WhittedRayTracer.h"

#include "Material.h"
#include "Scene.h"

constexpr float fMax = std::numeric_limits<float>::max();
constexpr int MAX_DEPTH = 4;

void WhittedRayTracer::_renderThread(MyScene::Ptr scene, MyCamera::Ptr camera) {
  mEyePos = camera->getEyePos();
  camera->getFocalPlane(mFocalPlaneH, mFocalPlaneV, mFocalPlaneLeftTop);

  MyScene* pScene = scene.get();
  for (int y = 0; y < mFrameHeight; y++)
    for (int x = 0; x < mFrameWidth; x++) {
      if (!mRuning) break;

      Ray viewRay = _generateViewRay(x, y);
      glm::vec3 color = _rayShading(viewRay, scene.get(), 0);
      _writePixel(x, y, glm::vec4(color, 1));
      mPixelCount++;
    }  // end of for(x)
}

glm::vec3 WhittedRayTracer::_rayShading(Ray ray, MyScene* pScene, int depth) {
  if (depth > MAX_DEPTH) return glm::vec3(0);

  HitRecord hitRec;
  bool bHit = pScene->hit(ray, 0.0001f, fMax, hitRec);
  if (!bHit) return glm::vec3(0);

  glm::vec3 color;
  Material* mtl = dynamic_cast<Material*>(hitRec.mtl);
  glm::vec3 albedo(1);
  if (hitRec.mtl) albedo = mtl->sampleAlbedo(hitRec.uv, hitRec.p);

  float Ia = 0;
  Scene* scene = dynamic_cast<Scene*>(pScene);
  const auto& lights = scene->getLights();
  for (const auto& light : lights) {
    Ray shadowRay = light->generateShadowRay(hitRec.p);
    HitRecord hitRecS;
    constexpr float SHADOW_E = 0.1f;
    bool bShadow = pScene->hit(shadowRay, SHADOW_E, fMax, hitRecS);

    if (!bShadow) {
      Ia += light->shadingIntensity(hitRec.p, hitRec.normal, ray.direction, mtl->Kd,
                             mtl->Ks, mtl->n);
    } else {
      Ia += light->ambient;
    }

  }  // end of for each light
  color = Ia * albedo;

  // recursive
  if (mtl) {
    if (mtl->Ks > 0) {
      Ray reflectionRay =
          _generateReflectionRay(ray.direction, hitRec.p, hitRec.normal);
      glm::vec3 rColor = _rayShading(reflectionRay, pScene, depth + 1);
      color += rColor * mtl->Ks;
    }

    // if (mtl->Kt > 0) {
    //   Ray transmissionRay;
    //   glm::vec3 tColor = _rayShading(transmissionRay, pScene, depth + 1);
    //   color += tColor * mtl->Ks;
    // }
  }  // end of if(mtl)

  return color;
}

Ray WhittedRayTracer::_generateViewRay(int x, int y) {
  float s = (x + 0.5f) / (float)(mFrameWidth);
  float t = (y + 0.5f) / (float)(mFrameHeight);

  glm::vec3 origin = mEyePos;
  return Ray(origin,
             mFocalPlaneLeftTop + s * mFocalPlaneH - t * mFocalPlaneV - origin);
}

Ray WhittedRayTracer::_generateReflectionRay(const glm::vec3& dir,
                                             const glm::vec3& point,
                                             const glm::vec3& normal) {
  glm::vec3 outDir = glm::reflect(dir, normal);
  return Ray(point, outDir);
}
