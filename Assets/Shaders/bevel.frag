#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 texSize;    // Texture size in pixels
uniform int edgeThickness; // Edge thickness control
uniform bool highlights; // Enable/disable blue edges (top/left)
uniform bool shadows;  // Enable/disable red edges (bottom/right)

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
    int c = classifyColor(texture(texture0, fragTexCoord).rgb); // Center pixel

    bool isEdge = false;
    float gx = 0.0;
    float gy = 0.0;

    // Expand edge detection based on thickness
    for (int x = -edgeThickness; x <= edgeThickness; x++) {
        for (int y = -edgeThickness; y <= edgeThickness; y++) {
            if (x == 0 && y == 0) continue; // Skip center pixel

            vec2 offset = texel * vec2(float(x), float(y));
            int neighbor = classifyColor(texture(texture0, fragTexCoord + offset).rgb);

            if (neighbor != c && c != 0) { // Detect edges (ignore white background)
                isEdge = true;
                gx += float(x);
                gy += float(y);
            }
        }
    }

    if (isEdge) {
        if ((gx < 0.0 || gy < 0.0) && highlights) {
            FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue for top/left edges
        } else if ((gx >= 0.0 || gy >= 0.0) && shadows) {
            FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red for bottom/right edges
        } else {
            FragColor = texture(texture0, fragTexCoord); // Keep original texture if no color is enabled
        }
    } else {
        FragColor = texture(texture0, fragTexCoord); // Keep original texture
    }
}
