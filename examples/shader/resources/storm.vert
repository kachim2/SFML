#version 130

// Uniforms
uniform mat4 sf_ModelMatrix;
uniform mat4 sf_ViewMatrix;
uniform mat4 sf_ProjectionMatrix;
uniform mat4 sf_TextureMatrix;
uniform int sf_TextureEnabled;
uniform vec2 storm_position;
uniform float storm_total_radius;
uniform float storm_inner_radius;
uniform sampler1D random_texture;

// Vertex attributes
in vec3 sf_Vertex;
in vec4 sf_Color;
in vec2 sf_MultiTexCoord0;

// Vertex shader outputs
out vec4 sf_FrontColor;
out vec2 sf_TexCoord0;

float rand(float index)
{
    return dot(texture1D(random_texture, index), vec4(1.0 / 16777216.0, 1.0 / 65536.0, 1.0 / 256.0, 1.0));
}

void main()
{
    vec4 vertex = sf_ModelMatrix * vec4(sf_Vertex, 1.0);
    vec2 offset = vertex.xy - storm_position;

    float angle = 0.0;
    if (offset.x != 0.0)
        angle = atan(offset.y, offset.x);
    else
        angle = sign(offset.y) * -1.570796327;
    angle = (angle + 3.141592654) / 6.283185308;
    float random_number = rand(angle);

    float len = length(offset);
    if (len < storm_total_radius * (0.8 + random_number / 6.0))
    {
        float push_distance = storm_inner_radius + len / storm_total_radius * (storm_total_radius - storm_inner_radius);
        vertex.xy = storm_position + normalize(offset) * push_distance * (0.8 + random_number * 0.2 * len / storm_total_radius);
    }
    // Vertex position
    gl_Position = sf_ProjectionMatrix * sf_ViewMatrix * vertex;

    // Vertex color
    sf_FrontColor = sf_Color;

    // Texture data
    if (sf_TextureEnabled == 1)
        sf_TexCoord0 = (sf_TextureMatrix * vec4(sf_MultiTexCoord0, 0.0, 1.0)).st;
}
