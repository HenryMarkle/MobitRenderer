#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform float tint; // 0 - 1

out vec4 finalColor;

void main()
{
    if (fragTexCoord.x < 0.0 || fragTexCoord.x > 1.0 || fragTexCoord.y < 0.0 || fragTexCoord.y > 1.0) {
        finalColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {

        vec4 texelColor = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));

        if (texelColor.r == 1.0 && texelColor.g == 1.0 && texelColor.b == 1.0 && texelColor.a == 1.0) {
            discard;
        }

        texelColor.r = clamp(texelColor.r + tint, 0, 1);
        texelColor.g = clamp(texelColor.g + tint, 0, 1);
        texelColor.b = clamp(texelColor.b + tint, 0, 1);

        finalColor = texelColor;
    }
}
