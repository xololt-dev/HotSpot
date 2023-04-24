#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Uniform attributes (ones we give, don't change with every fs iteration)
uniform vec2 resolution;
uniform vec3 cameraPosition;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

void main()
{
    Ray ray;
    ray.origin = cameraPosition;

    vec3 rayTarget = vec3(fragTexCoord * 2.0f - 1.0f, 1.0f);

    // calculate a normalized vector for the ray direction.
    // it's pointing from the ray position to the ray target.
    ray.direction = normalize(rayTarget - ray.origin);
    
	// show the ray direction
    // finalColor = vec4(ray.direction, 1.0f);
    // finalColor = vec4(fragTexCoord, 0.0f, 1.0f);

	// Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code

    finalColor = texelColor*colDiffuse*vec4(fragTexCoord, 0.0f, 1.0f);
}