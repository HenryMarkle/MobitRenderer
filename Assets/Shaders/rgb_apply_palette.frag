#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform sampler2D palette;
uniform sampler2D lightmap;
uniform int depth; // 0 - 29
uniform int vflip; // 0 - 1

out vec4 finalColor;

void main()
{
    vec4 black = vec4(0, 0, 0, 0);
    vec4 white = vec4(1, 1, 1, 1);
    vec4 red = vec4(1, 0, 0, 1);
    vec4 green = vec4(0, 1, 0, 1);
    vec4 blue = vec4(0, 0, 1, 1);

    vec4 l;
    vec4 p;
    vec4 light;

    if (vflip == 0) {
        l = texture(texture0, fragTexCoord);
        light = texture(lightmap, fragTexCoord);
    } else {
        l = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));
        light = texture(lightmap, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));
    }

    if (l == white) { discard; }

    int r = depth + 1;
    int isLit = int(light == white)*3;

    if (l == red) {
        p = texture(palette, vec2(depth/32.0, (isLit + 4.0)/16.0));
    } 
    else if (l == green) {
        p = texture(palette, vec2(depth/32.0, (isLit + 3.0)/16.0));
        r += 30;
    } 
    else if (l == blue) {
        p = texture(palette, vec2(depth/32.0, (isLit + 2.0)/16.0));
        r += 60;
    }

    vec4 fogColor = texture(palette, vec2(1.0/32.0, 0));
    vec4 fogAmountColor = texture(palette, vec2(9.0/32.0, 0));
    float fogAmount = 0;

    if (fogAmountColor.r != 0) {
        fogAmount = 1 - fogAmountColor.r;
    }
    // 200%
    else if (fogAmountColor.r == 0 && fogAmountColor.g == 0) {
        fogAmount = 1 + fogAmountColor.b;
    }

    if (depth < 10) {
        p = clamp(mix( p, fogColor, (r/2.0) * fogAmount/30.0 ), 0, 1);
    } else {
        p = clamp(mix( p, fogColor, r       * fogAmount/30.0 ), 0, 1);
    }

    finalColor = p;
}
