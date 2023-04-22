#pragma once

#include "EntityRenderer.h"
#include "TerrainRenderer.h"
#include "SkyboxRenderer.h"
#include "TextRenderer.h"
#include "../entities/Entity.h"
#include "../entities/Light.h"
#include "../entities/Camera.h"
#include "../entities/Terrain.h"
#include "../ShadowMap.h"

#include <vector>
#include <glm/glm.hpp>

class RenderManager {
  private:
    EntityRenderer renderer;
    TerrainRenderer terrainRenderer;
    TextRenderer textRenderer;

  public:
    RenderManager(int winX, int winY);

    // Renders the entire scene, including 4 passes (shadow, refraction, reflection, total)
    // Lots of parameters, some of these could be maybe moved to the constructor as they do not change.
    // Not sure what a better way to manage all of these parameters is
    void render(const std::vector<Entity*>& entities, const std::vector<Light*>& lights, Terrain* terrain,
        SkyboxRenderer& skybox, ShadowMap& shadowMap, Camera* cam, const glm::mat4& projection, int winX,
        int winY, std::map<std::string, float> &threadData);
    void renderMenu(const std::vector<Entity*>& entities, const std::vector<Light*>& lights, Terrain* terrain,
        SkyboxRenderer& skybox, ShadowMap& shadowMap, Camera* cam, const glm::mat4& projection, int winX,
        int winY);
};
