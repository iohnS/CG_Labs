layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
// layout (location = 2) in vec2 tex_coord;
// layout (location = 3) in vec3 tangent;
// layout (location = 4) in vec3 bitangent;

uniform mat4 vertex_model_to_world;  //World
uniform mat4 normal_model_to_world;  //WorldIT
uniform mat4 vertex_world_to_clip;   //ModelViewProj

uniform vec3 light_position; 
uniform vec3 camera_position;  
uniform bool use_normal_mapping;


in vec3 vPos; // From application
in vec3 vNormal; // defined in model space

out vec3 fN; // Send to Pixel shader
out vec3 fV;
out vec3 fL;

void main()
{
    vec3 worldPos = vec3(vertex_model_to_world * vec4(vertex, 1.0));
	fN = vec3(normal_model_to_world * vec4(normal, 0.0));
    fV = camera_position - worldPos;
    fL = light_position - worldPos;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);

}

