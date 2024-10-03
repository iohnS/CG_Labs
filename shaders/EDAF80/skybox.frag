#version 410

uniform vec3 light_position;
uniform samplerCube cubemap;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
} fs_in;

out vec4 frag_color;

void main()
{
	frag_color = texture(cubemap, fs_in.normal);
}
