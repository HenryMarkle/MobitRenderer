#version 330

uniform sampler2D texture0;
uniform int layers;
uniform float height;
uniform float width;
uniform float depth;
uniform int depthOffset;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 FragColor;

void main() {
	vec4 newColor = vec4(0);
	float totalWidth = fragTexCoord.x * width;

    if (totalWidth < 0 || totalWidth > 1.0) discard;

	for (int l = layers - 1; l > -1; l--) {
		float depthTint = (l + depthOffset) * depth;
		float currentHeight = fragTexCoord.y * height + (l * height);

        if (currentHeight < 0 || currentHeight > 1.0) continue;
		
		vec2 newFragTexCoord = vec2(totalWidth, currentHeight);
	
		vec4 c = texture(texture0, newFragTexCoord);
		if (c.r == 1.0 && c.g == 1.0 && c.b == 1.0) continue;
		
		if (c.b == 1.0) {
            newColor = vec4(fragColor.r - 0.1, fragColor.g - 0.1, fragColor.b - 0.1, fragColor.a);
        }
        else if (c.g == 1.0) {
            newColor = vec4(fragColor.r - 0.3, fragColor.g - 0.3, fragColor.b - 0.3, fragColor.a);
        }
        else if (c.r == 1.0) {
            newColor = vec4(fragColor.r - 0.6, fragColor.g - 0.6, fragColor.b - 0.6, fragColor.a);
        } else {
            newColor = fragColor;
        }
        
        newColor = vec4(newColor.r + depthTint, newColor.g + depthTint, newColor.b + depthTint, newColor.a);
	}

    if (newColor.a <= 0.0) discard;

	FragColor = newColor;
}