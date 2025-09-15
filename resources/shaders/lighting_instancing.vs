#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

in mat4 instanceTransform;

// model-view-projection matrix
uniform mat4 mvp;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

void main()
{
    // fragPosition = vec3(instanceTransform*vec4(vertexPosition, 1.0));
    // fragTexCoord = vertexTexCoord;
    // fragColor = vec4(1.0);
    // fragNormal = normalize(vec3(instanceTransform * vec4(vertexNormal, 0.0)));

    gl_Position = mvp*instanceTransform*vec4(vertexPosition, 1.0);
}