#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);

    if (texelColor.r == 1.0 && texelColor.g == 1.0 && texelColor.b == 1.0 && texelColor.a == 1.0) {
        discard;
    }

    if (texelColor.b == 1.0) {
        finalColor = fragColor + vec4(0.1, 0.1, 0.1, 0);
    }
    if (texelColor.g == 1.0) {
        finalColor = fragColor;
    }
    if (texelColor.r == 1.0) {
        finalColor = fragColor - vec4(0.1, 0.1, 0.1, 0);
    }
}
