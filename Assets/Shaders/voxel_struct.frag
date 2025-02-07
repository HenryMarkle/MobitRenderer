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
		float depthTint = clamp(l + depthOffset, 0, 29) * depth;
		float currentHeight = fragTexCoord.y * height + (l * height);

		if (currentHeight < 0 || currentHeight > 1.0) continue;
		
		vec2 newFragTexCoord = vec2(totalWidth, currentHeight);
	
		vec4 c = texture(texture0, newFragTexCoord);
		if (c.r == 1.0 && c.g == 1.0 && c.b == 1.0) continue;
		
		newColor = c;
        
        newColor = vec4(newColor.r + depthTint, newColor.g + depthTint, newColor.b + depthTint, newColor.a);
	}

    if (newColor.a <= 0.0) discard;

	FragColor = newColor;
}