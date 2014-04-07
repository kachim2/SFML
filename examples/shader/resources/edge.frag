#version 130

// Uniforms
uniform int sf_TextureEnabled;
uniform sampler2D sf_Texture0;
uniform float edge_threshold;

// Fragment shader inputs
in vec4 sf_FrontColor;
in vec2 sf_TexCoord0;

// Fragment shader outputs
out vec4 sf_FragColor;

void main()
{
    const float offset = 1.0 / 512.0;
    vec2 offx = vec2(offset, 0.0);
    vec2 offy = vec2(0.0, offset);

    vec4 hEdge = texture2D(sf_Texture0, sf_TexCoord0 - offy)        * -2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offy)        *  2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx - offy) * -1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx + offy) *  1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx - offy) * -1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx + offy) *  1.0;

    vec4 vEdge = texture2D(sf_Texture0, sf_TexCoord0 - offx)        *  2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx)        * -2.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx - offy) *  1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 - offx + offy) * -1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx - offy) *  1.0 +
                 texture2D(sf_Texture0, sf_TexCoord0 + offx + offy) * -1.0;

    vec3 result = sqrt(hEdge.rgb * hEdge.rgb + vEdge.rgb * vEdge.rgb);
    float edge = length(result);

    vec4 pixel = sf_FrontColor * texture2D(sf_Texture0, sf_TexCoord0);
    if (edge > (edge_threshold * 8.0))
        pixel.rgb = vec3(0.0, 0.0, 0.0);
    else
        pixel.a = edge_threshold;

    if (sf_TextureEnabled == 1)
        sf_FragColor = pixel;
    else
        sf_FragColor = sf_FrontColor;
}
