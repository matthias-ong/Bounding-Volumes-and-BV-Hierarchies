#version 410 core

uniform mat4  vertexTransform;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3    vPosition;
layout(location = 1) in vec2    vUV;


const vec3 vertexNormal = vec3( 0.0, 0.0, 1.0 );

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
    vec4 vertexPosition; // interpolated vPosition
    vec2 vertexUV;       // interpolated vUV

} vs_out;

void main()
{
    vs_out.vertexPosition = vertexTransform *  vec4( vPosition, 1.0f );
    vs_out.vertexUV = vUV;

//    gl_Position = vs_out.vertexPosition;
}
