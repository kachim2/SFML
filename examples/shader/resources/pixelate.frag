#version 130

// Uniforms
uniform int sf_TextureEnabled;
uniform sampler2D sf_Texture0;
uniform float pixel_threshold;

// Fragment shader inputs
in vec4 sf_FrontColor;
in vec2 sf_TexCoord0;

// Fragment shader outputs
out vec4 sf_FragColor;

void main()
{
    float factor = 1.0 / (pixel_threshold + 0.001);
    vec2 pos = floor(sf_TexCoord0 * factor + 0.5) / factor;

    if (sf_TextureEnabled == 1)
        sf_FragColor = texture2D(sf_Texture0, pos) * sf_FrontColor;
    else
        sf_FragColor = sf_FrontColor;
}
