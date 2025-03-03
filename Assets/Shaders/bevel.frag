#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 texSize;    // Texture size in pixels
uniform int edgeThickness; // Edge thickness control
uniform bool highlights; // Enable/disable blue edges (top/left)
uniform bool shadows;  // Enable/disable red edges (bottom/right)
uniform int vflip;

out vec4 finalColor;

// Function to classify the color in the texture
int classifyColor(vec3 color) {
    if (color.r > 0.9 && color.g > 0.9 && color.b > 0.9) return 0; // White
    if (color.r > 0.9 && color.g < 0.1 && color.b < 0.1) return 1; // Red
    if (color.r < 0.1 && color.g > 0.9 && color.b < 0.1) return 2; // Green
    if (color.r < 0.1 && color.g < 0.1 && color.b > 0.9) return 3; // Blue
    return -1; // Unknown color (shouldn't happen)
}

void main() {
    vec2 texel = 1.0 / texSize; // Texel size
    vec4 pixel;

    vec4 white = vec4(1, 1, 1, 1);
    vec4 red = vec4(1, 0, 0, 1);
    vec4 green = vec4(0, 1, 0, 1);
    vec4 blue = vec4(0, 0, 1, 1);

    if (vflip == 0) {
        pixel = texture(texture0, fragTexCoord);
    } else {
        pixel = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y));
    }

    if (pixel == white) { discard; }
    
    int c = classifyColor(pixel.rgb); // Center pixel

    bool isEdge = false;
    int gx = 0;
    int gy = 0;

    //int left = classifyColor(texture(texture0, fragTexCoord + texel * vec2(-edgeThickness, 0)).rgb);
    //int top  = classifyColor(texture(texture0, fragTexCoord + texel * vec2(0, -edgeThickness)).rgb);
    //int right  = classifyColor(texture(texture0, fragTexCoord + texel * vec2(edgeThickness, 0)).rgb);    
    //int right  = classifyColor(texture(texture0, fragTexCoord + texel * vec2(0, edgeThickness)).rgb); 

    // Expand edge detection based on thickness
    for (int x = -edgeThickness; x <= edgeThickness; x++) {
        for (int y = -edgeThickness; y <= edgeThickness; y++) {
            if (x == 0 && y == 0) continue; // Skip center pixel

            vec2 offset = texel * vec2(float(x), float(y));
            vec4 neighbor = texture(texture0, fragTexCoord + offset);

            if (neighbor == white) {
                isEdge = true;
                gx += x;
                gy += y;
            }
        }
    }

    if (isEdge) {
        if ((gx < 0.0 || gy < 0.0) && highlights) {
            finalColor = blue; // Blue for top/left edges
        } else if ((gx >= 0.0 || gy >= 0.0) && shadows) {
            finalColor = red; // Red for bottom/right edges
        } else {
            finalColor = pixel; // Keep original texture if no color is enabled
        }
    } else {
        finalColor = pixel; // Keep original texture
    }
}
