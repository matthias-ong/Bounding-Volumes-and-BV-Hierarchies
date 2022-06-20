
#version 410 core

uniform mat4 vertexTransform;

in layout(location = 0) vec3 modelPosition;
in layout(location = 1) vec3 vertexNormal;
//in layout(location = 2) vec3 diffuseColor;

vec3 diffuseColor = vec3( 0.8f, 0.8f, 0.8f );

out VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;

} vs_out;

void main()
{
    vs_out.fragDiffuse = diffuseColor;
    vs_out.fragNormal = vertexNormal;

    gl_Position = vertexTransform * vec4( modelPosition, 1.0f );
}
