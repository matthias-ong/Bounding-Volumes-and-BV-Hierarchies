#version 410 core

// Input from the rasterizer
// Conceptually equivalent to
// "WritePixel( x, y, rasterColor )"

uniform vec3 meshColour;

// Output data
out vec3 color;

void main()
{
	color = meshColour;
}
