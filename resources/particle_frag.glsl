in vec2 UV;
in vec4 particle_color;
out vec4 frag_color;

uniform sampler2D textureSamp;
uniform float hasTexture;

void main() {
	if (hasTexture == 1.0f) {
		vec4 texture = texture2D(textureSamp, UV);
		if (texture.x < 0.25) {
			discard;
		}
		frag_color = texture(textureSamp, UV);
	}
	else {
		frag_color = particle_color;
	}
}