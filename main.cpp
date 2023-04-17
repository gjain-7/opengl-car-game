#define GLFW_INCLUDE_NONE
#define _USE_MATH_DEFINES

#include "constants.h"
#include "renderers/RenderManager.h"
#include "GameTime.h"

#include "particles/ParticleManager.h"
#include "particles/ParticleSystem.h"
#include "Window.h"
#include <renderers/TextRenderer.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm_ext.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <thread>
#include <map>
// #include <irrKlang/include/irrKlang.h>
// using namespace irrklang;
using namespace glm;
using namespace std::chrono;

constexpr float SKYBOX_SIZE = 200.0f;
bool terminate = 1;

// ISoundEngine *SoundEngine = createIrrKlangDevice();

auto setProjection(int inputWinX, int inputWinY) {
    // float aspect = (float) winX / winY;
    // FOV angle is in radians
    return glm::perspective(constants::PI / 4.0, double(inputWinX) / double(inputWinY), 1.0, 800.0);
}

void thread_function(std::map<std::string, float> &m) {
    float x = 0.0, z = 0.0;
    // SoundEngine->play2D("audio/breakout.mp3", true);
    while(terminate) {
        // sleep for a second
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "fps: " << m["fps"] << "\n";
        float angle = atan2(m["z"], m["x"]) - atan2(z, x);
        // std::cout << "angle: " << angle << "\n";
        if(abs(angle) < 1) { // don't consider points where tan inverse blows up
            if(angle < 0) {
                std::cout << "Wrong Way!\n";
            }
        }
        x = m["x"];
        z = m["z"];
    }
}

int main(int argc, char** argv) {
    InputState input;
    glm::mat4 projection;
    Player* player;
    auto* headlight = new Light();
    Window window;
    // Vector to hold all of the world entities.
    std::vector<Entity*> entities;
    // Vector to hold lights
    std::vector<Light*> lights;
    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " basic|physics" << std::endl;
        exit(1);
    }
    // Check if desired controls are basic or physics
    bool basic_controls = strcmp(argv[1], "basic") == 0;

    if (basic_controls) {
        std::cout << "Controls: \n\tw - forward\n\ts - backwards\n\ta/d - turn left/right" << std::endl;
    } else {
        std::cout << "Controls: \n\tw - throttle\n\ts - brake\n\ta/d - steer left/right\n\tspace - handbrake\n\tt - "
                     "top view\n\ti - front view\n\tj - left view\n\tl - right view\n\tk - back view\n\tr - rest to "
                     "original view\n\th - toggle headlight"
                  << std::endl;
    }
    
    window.set_key_callback([&](GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
        // Terminate program if escape is pressed
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        // on/off headlights
        else if (key == GLFW_KEY_H && action == GLFW_PRESS) {
            if (lights[1]->radius == 0.0f) {
                lights[1]->radius = 15.0f;
            } else {
                lights[1]->radius = 0.0f;
            }
            std::cout << "Player Position: " << glm::to_string(player->getPosition()) << std::endl;
            std::cout << "Penalty: " << player->penalty << "\n";

        } else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
            if (lights[0]->diffuse == vec3(0.7f, 0.7f, 0.7f)) {
                lights[0]->diffuse = vec3(0.1f, 0.1f, 0.1f);
                lights[1]->radius = 15.0f;
            } else {
                lights[0]->diffuse = vec3(0.7f, 0.7f, 0.7f);
                lights[1]->radius = 0.0f;
            }
        }

        // top view
        else if (key == GLFW_KEY_T) {
            if (action == GLFW_PRESS)
                input.tKeyPressed = true;
            else if (action == GLFW_RELEASE)
                input.tKeyPressed = false;
        }

        // left view
        else if (key == GLFW_KEY_J) {
            if (action == GLFW_PRESS)
                input.jKeyPressed = true;
            else if (action == GLFW_RELEASE)
                input.jKeyPressed = false;
        }

        // right view
        else if (key == GLFW_KEY_L) {
            if (action == GLFW_PRESS)
                input.lKeyPressed = true;
            else if (action == GLFW_RELEASE)
                input.lKeyPressed = false;
        }

        // back view
        else if (key == GLFW_KEY_K) {
            if (action == GLFW_PRESS)
                input.kKeyPressed = true;
            else if (action == GLFW_RELEASE)
                input.kKeyPressed = false;
        }

        // front view
        else if (key == GLFW_KEY_I) {
            if (action == GLFW_PRESS)
                input.iKeyPressed = true;
            else if (action == GLFW_RELEASE)
                input.iKeyPressed = false;
        }

        // reset to original view
        else if (key == GLFW_KEY_R) {
            if (action == GLFW_PRESS)
                input.rKeyPressed = true;
            else if (action == GLFW_RELEASE)
                input.rKeyPressed = false;
        } else {
            player->handleKeyboardEvents(window, key, action);
        }
    });

    window.set_mouse_position_callback(
        [&](GLFWwindow* /*window*/, double x, double y) { input.update((float)x, (float)y); });

    window.set_mouse_scroll_callback([&](GLFWwindow* /*window*/, double xoffset, double yoffset) {
        input.updateScroll((float)xoffset, (float)yoffset);
    });

    window.set_mouse_button_callback([&](GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            input.rMousePressed = true;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            input.rMousePressed = false;
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            input.lMousePressed = true;
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            input.lMousePressed = false;
        }
    });

    window.set_window_reshape_callback([&](GLFWwindow* /*window*/, int x, int y) {
        projection = setProjection(x, y);
        glViewport(0, 0, x, y);
    });

    srand(static_cast<unsigned int>(time(nullptr)));
    // clang-format off
    // Define skybox textures
    // The current skybox is unfortunately quite low res and has compression issues.
    // However, I wanted a skybox that had no terrain AND a visible sun which was surprisingly hard to find.
    std::vector<std::string> skyboxTextures = {
         "res/sky/sky_right.tga",
         "res/sky/sky_left.tga",
         "res/sky/sky_top.tga",
         "res/sky/sky_bottom.tga",
         "res/sky/sky_back.tga",
         "res/sky/sky_front.tga"
    };
    // clang-format on
    // Load all of the requires models from disk.
    Model playerModel = Loader::getLoader()->loadModel("res/car/car-n.obj");
    Model fenceModel = Loader::getLoader()->loadModel("res/fence/fence.obj");
    Model coneModel = Loader::getLoader()->loadModel("res/cone/cone2_obj.obj");
    Model treeModel = Loader::getLoader()->loadModel("res/tree/PineTree03.obj");

    // Load text
    int err = LoadFont("res/fonts/Antonio-Bold.ttf");
    if (err != 0)
        return -1;
    Shader shader("shaders/text.vs", "shaders/text.fs");
    LoadShader(shader, window.get_width(), window.get_height());

    // Create the skybox with the textures defined.
    SkyboxRenderer skybox(skyboxTextures, SKYBOX_SIZE);
    RenderManager manager;

    // Create Terrain using blend map, height map and all of the remaining texture components.
    std::vector<std::string> terrainImages = {"res/terrain/blend_map.png", "res/terrain/grass.jpg",
        "res/terrain/road.jpg", "res/terrain/dirt.png", "res/terrain/mud.jpg"};
    Terrain* terrain = Terrain::loadTerrain(terrainImages, "res/terrain/height_map.png");
    // Moves the terrain model to be centered about the origin.
    terrain->setPosition(vec3(-Terrain::TERRAIN_SIZE / 2, 0.0f, -Terrain::TERRAIN_SIZE / 2));

    // Load dust texture and create particle system.
    GLuint dust_texture = Loader::getLoader()->loadTexture("./res/dust_single.png");
    ParticleSystem particleSystem(30.0f, 3.0f, 0.5f, 0.5f, dust_texture);

    // Create the player object, scaling for the model, and setting its position in the world to somewhere interesting.
    player = new Player(&playerModel, terrain, basic_controls);
    player->setScale(vec3(0.4f, 0.4f, 0.4f));
    player->setPosition(terrain->getPositionFromPixel(505, 130));
    player->setRotationY((float)4.f * constants::PI / 8.f);
    entities.push_back(player);

    // Initialisation of camera, projection matrix
    projection = setProjection(window.get_width(), window.get_height());
    Camera* cam = new PlayerCamera(player);

    // Create light sources
    auto* sun = new Light();
    sun->position =
        vec4(-1.25 * SKYBOX_SIZE / 10, 2.5 * SKYBOX_SIZE / 10, 3 * SKYBOX_SIZE / 10, 0.0f);  // w = 0 - directional
    sun->specular = vec3(1.0f, 1.0f, 1.0f);
    sun->diffuse = vec3(0.7f, 0.7f, 0.7f);
    sun->ambient = vec3(0.1f, 0.1f, 0.1f);
    lights.push_back(sun);

    headlight->position = vec4(2.0f, 8.0f, 0.0f, 1.0f);
    headlight->specular = vec3(1.0f, 1.0f, 1.0f);
    headlight->diffuse = vec3(2.0f, 2.0f, 2.0f);
    headlight->coneDirection = vec3(0.0f, -1.0f, 0.0f);
    headlight->coneAngle = constants::PI / 8.f;
    headlight->radius = 0.0f;
    lights.push_back(headlight);

    // Adds entities to random positions on the map
    const size_t RAND_ENTITIES = 100;
    for (size_t i = 0; i < RAND_ENTITIES; i += 2) {
        Entity* ent;
        ent = new Entity(&treeModel);
        ent->setPosition(terrain->getPositionFromPixel(rand() % 1024, rand() % 1024));
        entities.push_back(ent);
    }

    // // Set of pre calculated cone positions on corners of the track
    // // clang-format off
    // std::vector<int> conePositions = {
    //     263, 262, 226, 250, 209, 273,
    //     213, 299, 342, 717, 329, 734,
    //     326, 751, 354, 755, 372, 754,
    //     750, 400, 765, 396, 748, 381,
    //     828, 480, 842, 476, 854, 478,
    //     852, 500, 852, 521, 842, 547,
    //     772, 402
    // };
    // // clang-format on
    // // Creates cones from the positions and adds them.
    // for (size_t i = 0; i < conePositions.size(); i += 2) {
    //     auto* cone = new Entity(&coneModel);
    //     cone->setPosition(terrain->getPositionFromPixel(conePositions[i], conePositions[i + 1]));
    //     cone->setScale(vec3(0.01f, 0.01f, 0.01f));  // The cone model was MASSIVE
    //     entities.push_back(cone);
    // }

    // Add the bordering fences to the map.
    float fenceSize = fenceModel.getRangeInDim(0).second - fenceModel.getRangeInDim(0).first;

    float conePlacement = 25.0f;
    float coneSize = 5.0f;
    for (float x = -Terrain::TERRAIN_SIZE / 2 + conePlacement; x < Terrain::TERRAIN_SIZE / 2 - conePlacement;
         x += coneSize) {
        auto* cone = new Entity(&coneModel);
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        cone->setPosition(vec3(x, 0.0f, Terrain::TERRAIN_SIZE / 2 - conePlacement));
        entities.push_back(cone);

        cone = new Entity(&coneModel);
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        cone->setPosition(vec3(x, 0.0f, -Terrain::TERRAIN_SIZE / 2 + conePlacement));
        entities.push_back(cone);
    }

    for (float z = -Terrain::TERRAIN_SIZE / 2 + conePlacement; z < Terrain::TERRAIN_SIZE / 2 - conePlacement;
         z += coneSize) {
        auto* cone = new Entity(&coneModel);
        cone->setPosition(vec3(Terrain::TERRAIN_SIZE / 2 - conePlacement, 0.0f, z));
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        entities.push_back(cone);

        cone = new Entity(&coneModel);
        cone->setPosition(vec3(-Terrain::TERRAIN_SIZE / 2 + conePlacement, 0.0f, z));
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        entities.push_back(cone);
    }

    conePlacement = 54.0f;

    for (float x = -Terrain::TERRAIN_SIZE / 2 + conePlacement; x < Terrain::TERRAIN_SIZE / 2 - conePlacement;
         x += coneSize) {
        auto* cone = new Entity(&coneModel);
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        cone->setPosition(vec3(x, 0.0f, Terrain::TERRAIN_SIZE / 2 - conePlacement));
        entities.push_back(cone);

        cone = new Entity(&coneModel);
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        cone->setPosition(vec3(x, 0.0f, -Terrain::TERRAIN_SIZE / 2 + conePlacement));
        entities.push_back(cone);
    }

    for (float z = -Terrain::TERRAIN_SIZE / 2 + conePlacement; z < Terrain::TERRAIN_SIZE / 2 - conePlacement;
         z += coneSize) {
        auto* cone = new Entity(&coneModel);
        cone->setPosition(vec3(Terrain::TERRAIN_SIZE / 2 - conePlacement, 0.0f, z));
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        entities.push_back(cone);

        cone = new Entity(&coneModel);
        cone->setPosition(vec3(-Terrain::TERRAIN_SIZE / 2 + conePlacement, 0.0f, z));
        cone->setScale(vec3(0.01f, 0.01f, 0.01f));
        entities.push_back(cone);
    }

    for (float x = -Terrain::TERRAIN_SIZE / 2; x < Terrain::TERRAIN_SIZE / 2; x += fenceSize) {
        auto* fence = new Entity(&fenceModel);
        fence->setPosition(vec3(x, 0.0f, Terrain::TERRAIN_SIZE / 2 - 1.0f));
        entities.push_back(fence);

        fence = new Entity(&fenceModel);
        fence->setPosition(vec3(x, 0.0f, -Terrain::TERRAIN_SIZE / 2 + 1.0f));
        entities.push_back(fence);
    }

    for (float z = -Terrain::TERRAIN_SIZE / 2; z < Terrain::TERRAIN_SIZE / 2; z += fenceSize) {
        auto* fence = new Entity(&fenceModel);
        fence->setPosition(vec3(Terrain::TERRAIN_SIZE / 2 - 1.0f, 0.0f, z));
        fence->rotateY(-constants::PI / 2);
        entities.push_back(fence);

        fence = new Entity(&fenceModel);
        fence->setPosition(vec3(-Terrain::TERRAIN_SIZE / 2 + 1.0f, 0.0f, z));
        fence->rotateY(-constants::PI / 2);
        entities.push_back(fence);
    }

    // Goes through each entity and aligns its bottom edge with the terrain at that position.
    for (auto& entity : entities) {
        entity->placeBottomEdge(terrain->getHeight(entity->getPosition().x, entity->getPosition().z));
    }

    // Create the large lake
    auto* water = new Entity();
    // water->setScale(vec3(100.0f, 1.0f, 50.0f));
    // water->setPosition(terrain->getPositionFromPixel(650, 826));
    // water->setPosition(glm::vec3(water->getPosition().x, 0.4f, water->getPosition().z));

    // Create the object for handling rendering to texture for shadows.
    ShadowMap shadowMap(player, lights[0], 4096);

    bool check[4] = {0};
    int checkpoints[4][2] = {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}};
    auto start = high_resolution_clock::now();
    std::map<std::string, float> m;
    std::thread t(&thread_function, std::ref(m));

    // Main logic/render loop.
    while (!glfwWindowShouldClose(window.get_window())) {
        m["fps"] = GameTime::getGameTime()->getFPS();
        GameTime::getGameTime()->update();
        // std::cout << "FPS: " << GameTime::getGameTime()->getFPS() << std::endl;
        cam->update(input);

        // Render entire scene
        manager.render(entities, lights, terrain, water, skybox, shadowMap, cam, projection, window.get_width(),
            window.get_height(), shader);

        // Updates all particles and entities.
        ParticleManager::getParticleManager()->update();
        for (auto* entity : entities) {
            entity->update();
        }

        // Generate dust particles at the players positions if the car is going past enough or moving
        if (player->absVel > 5.0f || player->getThrottle() > 0.1f || (basic_controls && player->getBrake() > 0.1f)) {
            particleSystem.generateParticles(player->getPosition() - player->calculateDirectionVector());
        }

        // Update the position of the car headlights
        headlight->position = vec4(player->getPosition() + vec3(0.0f, 0.1f, 0.0f), 1.0f);
        headlight->coneDirection = player->calculateDirectionVector();

        glFlush();

        glfwSwapBuffers(window.get_window());
        glfwPollEvents();
        float x = player->getPosition().x;
        float z = player->getPosition().z;
        m["x"] = x;
        m["z"] = z;
        int out = 1;
        if (abs(x) < 125 && abs(z) < 125) {
            out = 0;
        }
        if (abs(x) < 98 && abs(z) < 98) {
            out = 1;
        }
        player->penalty += out;

        for (int i = 0; i < 4; i++) {
            if (check[i] == 0) {
                int x1 = 105 * checkpoints[i][0];
                int z1 = 105 * checkpoints[i][1];
                int dist = abs(x - x1) + abs(z - z1);
                if (dist < 20) {
                    check[i] = 1;
                    std::cout << "Checkpoint " << i + 1 << " reached!" << std::endl;
                }
            }
        }
        if (check[0] && check[1] && check[2] && check[3]) {
            if (x > 0 && abs(z + 112) < 12) {
                std::cout << "Lap Complete!" << std::endl;
                std::cout << "Penalty: " << player->penalty << std::endl;
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<seconds>(stop - start);
                std::cout << "Time Taken: " << duration.count() << std::endl;
                for (int i = 0; i < 4; i++) {
                    check[i] = 0;
                }
                int score = 100000 / duration.count() - player->penalty;
                score = max(0, score);
                std::cout << "Score: " << score << std::endl;
                player->penalty = 0;
                start = high_resolution_clock::now();
            }
        }
    }
    terminate = 0;
    // Cleanup program, delete all the dynamic entities.
    t.join();
    delete player;
    delete water;
    for (auto* entity : entities) {
        delete entity;
    }

    glfwDestroyWindow(window.get_window());
    glfwTerminate();

    return 0;
}
