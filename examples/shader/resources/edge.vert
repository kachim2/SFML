#version 130

// Uniforms
uniform mat4 sf_ModelMatrix;
uniform mat4 sf_ViewMatrix;
uniform mat4 sf_ProjectionMatrix;
uniform mat4 sf_TextureMatrix;
uniform int sf_TextureEnabled;

// Vertex attributes
in vec3 sf_Vertex;
in vec4 sf_Color;
in vec2 sf_MultiTexCoord0;

// Vertex shader outputs
out vec4 sf_FrontColor;
out vec2 sf_TexCoord0;

void main()
{
    // Vertex position
    gl_Position = sf_ProjectionMatrix * sf_ViewMatrix * sf_ModelMatrix * vec4(sf_Vertex, 1.0);

    // Vertex color
    sf_FrontColor = sf_Color;

    // Texture data
    if (sf_TextureEnabled == 1)
        sf_TexCoord0 = (sf_TextureMatrix * vec4(sf_MultiTexCoord0, 0.0, 1.0)).st;
}