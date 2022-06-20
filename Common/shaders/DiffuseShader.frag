#version 410 core

in VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;

} fs_in;

out vec3 fragColor;
uniform vec3 cameraPos;
uniform bool renderBoundingVolume;
uniform vec3 renderColour;

void main()
{
    vec3 lightVector = cameraPos;
	if (renderBoundingVolume)
        fragColor = renderColour;
    else
    {
        float N_dot_L = max( dot( normalize(fs_in.fragNormal), normalize(lightVector)), 0.0f );
	    fragColor = fs_in.fragDiffuse * N_dot_L;
    }
    
}
