#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));

    finalColor = texelColor;
}
