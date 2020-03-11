#ifndef SKYBOXSHADER_H
#define SKYBOXSHADER_H

#define _USE_MATH_DEFINES

#include "../entities/Light.h"
#include "../entities/Camera.h"
#include "../Model.h"
#include "ShaderProgram.h"

#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#pragma warning(push, 0)
#include <glm/ext.hpp>
#pragma warning(pop)
#include <GL/glew.h>

const std::string SKYBOX_VERTEX_SHADER = "shaders/skybox.vert";
const std::string SKYBOX_FRAGMENT_SHADER = "shaders/skybox.frag";

class SkyboxShader : public ShaderProgram {
private:
    GLuint location_projection;
    GLuint location_view;

public:
    SkyboxShader();

    virtual void bindUniformLocations();
    void loadMatrices(glm::mat4 camera, glm::mat4 projection);
};

#endif //SKYBOXSHADER_H
