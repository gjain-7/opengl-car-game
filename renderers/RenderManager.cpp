#include "RenderManager.h"

#include <glad/glad.h>
#include <iomanip>  // for std::setprecision
#include "../particles/Particle.h"
#include "../particles/ParticleManager.h"
#include "../particles/ParticleSystem.h"

RenderManager::RenderManager(int winX, int winY)
    : textRenderer(winX, winY) {}

void RenderManager::render(const std::vector<Entity*>& entities, const std::vector<Light*>& lights, Terrain* terrain,
    SkyboxRenderer& skybox, ShadowMap& shadowMap, Camera* cam, const glm::mat4& projection, int winX, int winY,
    std::map<std::string, float>& threadData) {
    // SHADOW PASS
    glDisable(GL_CLIP_DISTANCE0);
    shadowMap.bind();
    renderer.render(entities, lights, shadowMap.getView(), shadowMap.getProjection(), skybox.getSkyboxTexture(),
        glm::vec4(0, 1, 0, 10000));
    shadowMap.unbind();

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

    std::ostringstream out;
    Player* player = (Player*)entities[0];
    out << std::fixed << std::setprecision(1) << player->getSpeed();
    textRenderer.render(out.str() + " mph", 260.0f, 50.0f, 0.8f, glm::vec3(1, 1, 0));
    out.str("");
    textRenderer.render("Lap: " + std::to_string(player->lap), 540.0f, 450.0f, 0.4f, glm::vec3(0, 0, 0));
    out << std::fixed << std::setprecision(0) << threadData["time"];
    textRenderer.render("Lap Time: " + out.str(), 540.0f, 400.0f, 0.4f, glm::vec3(0, 0, 0));
    out.str("");
    out << std::fixed << std::setprecision(2) << threadData["fps"];
    textRenderer.render("FPS: " + out.str(), 540.0f, 350.0f, 0.4f, glm::vec3(0, 0, 0));
    out.str("");
    textRenderer.render("Score: " + std::to_string(player->score), 10.0f, 450.0f, 0.4f, glm::vec3(0, 0, 0));
    textRenderer.render("Penalty: " + std::to_string(player->penalty), 10.0f, 400.0f, 0.4f, glm::vec3(0, 0, 0));
    textRenderer.render(
        "Last Checkpoint: " + std::to_string(player->checkpoint), 10.0f, 350.0f, 0.4f, glm::vec3(0, 0, 0));
    if (threadData["wrong_way"] == 1) {
        textRenderer.render("Wrong Way!", 220.0f, 400.0f, 1.0f, glm::vec3(1, 0, 0));
    }
}

void RenderManager::renderMenu(const std::vector<Entity*>& entities, const std::vector<Light*>& lights,
    Terrain* terrain, SkyboxRenderer& skybox, ShadowMap& shadowMap, Camera* cam, const glm::mat4& projection, int winX,
    int winY) {
    // SHADOW PASS
    glDisable(GL_CLIP_DISTANCE0);
    shadowMap.bind();
    renderer.render(entities, lights, shadowMap.getView(), shadowMap.getProjection(), skybox.getSkyboxTexture(),
        glm::vec4(0, 1, 0, 10000));
    shadowMap.unbind();

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
    terrainRenderer.render(terrain, lights, cam->getViewMtx(), projection, shadowMap.getView(),
        shadowMap.getProjection(), shadowMap.getTextureID(), glm::vec4(0, 1, 0, 10000));
    skybox.render(cam->getViewMtx(), projection);
    ParticleManager::getParticleManager()->render(cam->getViewMtx(), projection);
    
    textRenderer.render("Welcome to New Formula Speed!", 30.0f, 410.0f, 1.0f, glm::vec3(0, 1, 0));
    textRenderer.render("Instructions", 30.0f, 360.0f, 0.7f, glm::vec3(1, 0, 0));
    textRenderer.render("W/A/S/D : Throttle/Left/Right/Brake", 30.0f, 310.0f, 0.4f, glm::vec3(1, 1, 0));
    textRenderer.render("Space : HandBrake", 30.0f, 270.0f, 0.4f, glm::vec3(1, 1, 0));
    textRenderer.render("I/J/K/L/T : Change Camera", 30.0f, 230.0f, 0.4f, glm::vec3(1, 1, 0));
    textRenderer.render("R : Reset Camera", 30.0f, 190.0f, 0.4f, glm::vec3(1, 1, 0));
    textRenderer.render("N : Night Mode", 30.0f, 150.0f, 0.4f, glm::vec3(1, 1, 0));
    textRenderer.render("H : Toggle Headlights", 30.0f, 110.0f, 0.4f, glm::vec3(1, 1, 0));
    textRenderer.render("Hit Enter to Start!", 200.0f, 50.0f, 0.8f, glm::vec3(1, 0, 0));
}