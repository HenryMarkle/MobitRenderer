#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform int variation;
uniform float width;
uniform float depth;

out vec4 finalColor;

void main()
{
    if (fragTexCoord.x < 0.0 || fragTexCoord.x > 1.0 || fragTexCoord.y < 0.0 || fragTexCoord.y > 1.0) {
        finalColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        float totalWidth = fragTexCoord.x * width;

        vec4 texelColor = texture(texture0, vec2(totalWidth + (totalWidth * variation), fragTexCoord.y));

        if (texelColor.r == 1.0 && texelColor.g == 1.0 && texelColor.b == 1.0 && texelColor.a == 1.0) {
            discard;
        }
        
        if (texelColor.r == 0.0 && texelColor.g == 0.0 && texelColor.b == 0.0 && texelColor.a == 1.0) {
            discard;
        }

        finalColor = texelColor + vec4(0, depth, 0, 0);
    }
}
