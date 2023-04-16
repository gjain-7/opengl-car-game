#include "RenderManager.h"

#include <glad/glad.h>
#include "../particles/Particle.h"
#include "../particles/ParticleManager.h"
#include "../particles/ParticleSystem.h"


const float WATER_PLANE_HEIGHT = 0.398918f;

RenderManager::RenderManager() : reflectionBuffer(640, 320), refractionBuffer(1280, 720) {
    reflectionBuffer.addColourTexture();
    reflectionBuffer.addDepthBuffer();

    refractionBuffer.addColourTexture();
    refractionBuffer.addDepthTexture();
}

void RenderManager::render(const std::vector<Entity*>& entities, const std::vector<Light*>& lights, Terrain* terrain,
    Entity* water, SkyboxRenderer& skybox, ShadowMap& shadowMap, Camera* cam, const glm::mat4& projection,
    int winX, int winY, Shader& shader) {
    // SHADOW PASS
    glDisable(GL_CLIP_DISTANCE0);
    shadowMap.bind();
    renderer.render(entities, lights, shadowMap.getView(), shadowMap.getProjection(), skybox.getSkyboxTexture(),
        glm::vec4(0, 1, 0, 10000));
    shadowMap.unbind();

    // REFRACTION PASS
    glEnable(GL_CLIP_DISTANCE0);
    refractionBuffer.bind();
    terrainRenderer.render(terrain, lights, cam->getViewMtx(), projection, shadowMap.getView(),
        shadowMap.getProjection(), shadowMap.getTextureID(), glm::vec4(0, -1, 0, water->getPosition().y));
    skybox.render(cam->getViewMtx(), projection);
    renderer.render(entities, lights, cam->getViewMtx(), projection, skybox.getSkyboxTexture(), shadowMap.getView(),
        shadowMap.getProjection(), shadowMap.getTextureID(), glm::vec4(0, -1, 0, water->getPosition().y));
    ParticleManager::getParticleManager()->render(cam->getViewMtx(), projection);
    refractionBuffer.unbind();

    // REFLECTION PASS
    glEnable(GL_CLIP_DISTANCE0);
    glm::mat4 invView = cam->getInverted(WATER_PLANE_HEIGHT);
    reflectionBuffer.bind();
    terrainRenderer.render(terrain, lights, invView, projection, shadowMap.getView(), shadowMap.getProjection(),
        shadowMap.getTextureID(), glm::vec4(0, 1, 0, -water->getPosition().y));
    skybox.render(invView, projection);
    renderer.render(entities, lights, invView, projection, skybox.getSkyboxTexture(), shadowMap.getView(),
        shadowMap.getProjection(), shadowMap.getTextureID(), glm::vec4(0, 1, 0, -water->getPosition().y));
    ParticleManager::getParticleManager()->render(invView, projection);
    reflectionBuffer.unbind();

    // NORMAL PASS
    glDisable(GL_CLIP_DISTANCE0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, winX, winY);

    terrainRenderer.render(terrain, lights, cam->getViewMtx(), projection, shadowMap.getView(),
        shadowMap.getProjection(), shadowMap.getTextureID(), glm::vec4(0, 1, 0, 10000));
    skybox.render(cam->getViewMtx(), projection);
    renderer.render(entities, lights, cam->getViewMtx(), projection, skybox.getSkyboxTexture(), shadowMap.getView(),
        shadowMap.getProjection(), shadowMap.getTextureID(), glm::vec4(0, 1, 0, 10000));
    ParticleManager::getParticleManager()->render(cam->getViewMtx(), projection);
    waterRenderer.render(water, cam->getViewMtx(), projection, refractionBuffer.getColourTexture(),
        reflectionBuffer.getColourTexture(), cam->getPosition(), lights[0]);

    RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    RenderText(shader, "(C) LearnOpenGL.com", 100.0f, 100.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

}
