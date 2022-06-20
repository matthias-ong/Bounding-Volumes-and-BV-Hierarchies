#version 410 core

uniform sampler2D texSampler;

// Interpolating vertex attributes over the rasterizer
in VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV;       // interpolated vUV

} fs_in;

out vec3 fragColor;

void main()
{
    fragColor = texture( texSampler, fs_in.vertexUV ).rgb;
//    fragColor = vec3( fs_in.vertexUV.s, fs_in.vertexUV.t, 1.0f );
}
