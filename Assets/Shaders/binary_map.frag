#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform int invert; // 0 - black on white; 1 - white on black
uniform int vflip; // 0 - 1

out vec4 finalColor;

void main()
{
    vec4 white = vec4(1, 1, 1, 1);
    vec4 black = vec4(0, 0, 0, 1);

    vec4 c;

    if (vflip == 0) {
        c = texture(texture0, fragTexCoord);
    } else {
        c = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));
    }

    // background
    if (c == white) {
        discard;
    } 
    // foreground
    else {
        if (invert == 0) {
            finalColor = black;
        } else {
            finalColor = white;
        }
    }
}
