#version 130

// Uniforms
uniform int sf_TextureEnabled;
uniform sampler2D sf_Texture0;
uniform float blur_radius;

// Fragment shader inputs
in vec4 sf_FrontColor;
in vec2 sf_TexCoord0;

// Fragment shader outputs
out vec4 sf_FragColor;

void main()
{
    vec2 offx = vec2(blur_radius, 0.0);
    vec2 offy = vec2(0.0, blur_radius);

    vec4 pixel = texture2D(sf_Texture0, sf_TexCoord0)               * 4.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx)        * 2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx)        * 2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offy)        * 2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offy)        * 2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx - offy) * 1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx + offy) * 1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx - offy) * 1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx + offy) * 1.0;

    if (sf_TextureEnabled == 1)
        sf_FragColor = sf_FrontColor * (pixel / 16.0);
    else
        sf_FragColor = sf_FrontColor;
}
