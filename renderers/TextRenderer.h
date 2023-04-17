/* #pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <shaders/TextShader.h>

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
int LoadFont(const char* fontPath);
void LoadShader(Shader& shader, int SCR_WIDTH, int SCR_HEIGHT);

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;         // Size of glyph
    glm::ivec2 Bearing;      // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Horizontal offset to advance to next glyph
};

extern std::map<GLchar, Character> Characters;
extern unsigned int VAO, VBO; */

/************************************************************************************/
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <shaders/TextShader.h>

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;         // Size of glyph
    glm::ivec2 Bearing;      // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Horizontal offset to advance to next glyph
};

class TextRenderer {
  private:
    Shader shader;//("shaders/text.vs", "shaders/text.fs");
    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;

  public:
    TextRenderer(int SCR_WIDTH, int SCR_HEIGHT);

    void render(std::string text, float x, float y, float scale, glm::vec3 color);
    int LoadFont(const char* fontPath);
    void LoadShader(int SCR_WIDTH, int SCR_HEIGHT);
};