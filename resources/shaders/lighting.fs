#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// uniform sampler2D albedoTexture;
uniform vec3 viewPos;
uniform vec4 ambient;
uniform float deltaTime;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 target;
    vec3 color;
    float attenuation;
};
uniform Light light;


void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);//vec4(ambient);
}