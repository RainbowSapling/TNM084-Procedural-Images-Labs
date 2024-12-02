#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 exNormal;
uniform sampler2D tex;
in vec3 outPosition;



void main(void)
{
	//float height = (length(outPosition.y) + 1)/2;
	float height = outPosition.y;


	// Texture from disc
	vec4 t = texture(tex, texCoord);

	// Procedural texture
	t.r = sin(texCoord.s * 3.1416);
	t.g = sin(texCoord.t * 3.1416);
	t.b = sin((texCoord.s + texCoord.t) * 10.0);

	vec3 n = normalize(exNormal);
	float shade = n.y + n.z;
//	if (t.a < 0.01) discard;
//	else
		//outColor = t * shade * shade * height; // Over-emphasized fake light

		vec4 color1 = vec4(0.6, 0.0, 1.0, 1.0);
		vec4 color2 = vec4(1.0, 0.0, 0.3, 1.0);

		vec4 color3 = mix(color1, color2, clamp(height,0.0,1.0));

		outColor = color3 * shade * shade;


//	outColor = vec4(texCoord.s, texCoord.t, 0, 1);
//	outColor = vec4(n.x, n.y, n.z, 1);
//	outColor = vec4(1) * shade;
}
