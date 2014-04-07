#version 130

// Uniforms
uniform int sf_TextureEnabled;
uniform sampler2D sf_Texture0;
uniform float blink_alpha;

// Fragment shader inputs
in vec4 sf_FrontColor;
in vec2 sf_TexCoord0;

// Fragment shader outputs
out vec4 sf_FragColor;

void main()
{
    vec4 pixel = sf_FrontColor;
    pixel.a = blink_alpha;

    if (sf_TextureEnabled == 1)
        sf_FragColor = texture2D(sf_Texture0, sf_TexCoord0) * pixel;
    else
        sf_FragColor = pixel;
}
