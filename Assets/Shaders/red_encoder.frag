#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D lightmap;
uniform int depth; // 0 - 29
uniform int flipv; // 0 - 1

out vec4 finalColor;

void main()
{
    vec4 white = vec4(1, 1, 1, 1);
    vec4 black = vec4(0, 0, 0, 0);

    vec4 red = vec4(1, 0, 0, 1);
    vec4 green = vec4(0, 1, 0, 1);
    vec4 blue = vec4(0, 0, 1, 1);

    int r = depth + 1;

    vec4 c;
    vec4 l;

    if (flipv == 1) {
        c = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));
        l = texture(lightmap, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));
    } else {
        c = texture(texture0, fragTexCoord);
        l = texture(lightmap, fragTexCoord);
    }

    if (c == white) {
        //finalColor = white;
        //return;
        discard;
    } else if (c == black) {
        finalColor = black;
        return;
    }

    if (c == green) {
        r += 30;
    } else if (c == blue) {
        r += 60;
    }

    if (l == white) {
        r += 50;
    }

    finalColor = vec4(r/255.0, 0, 0, 1);
}
