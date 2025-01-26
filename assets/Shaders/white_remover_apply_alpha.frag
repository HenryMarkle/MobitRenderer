#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform int alpha = 255;

out vec4 finalColor;

void main()
{
    if (alpha == 0) discard;

    vec4 texelColor = texture(texture0, fragTexCoord);

    if (texelColor.r == 1.0 && texelColor.g == 1.0 && texelColor.b == 1.0 && texelColor.a == 1.0) {
        discard;
    }

    texelColor.a = float(alpha) / 255.0;

    finalColor = texelColor;
}
