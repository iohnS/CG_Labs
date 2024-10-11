#version 410

//uniform vec3 light_position;
uniform vec3 camera_position;
uniform float t;
uniform sampler2D water_map;
uniform samplerCube cubemap;

in VS_OUT {
	vec2 tex_coord;
    vec3 vertex;
	vec3 normal;
    mat3 TBN;
    vec3 fV;
} fs_in;

out vec4 fColor;

void main(){
    vec3 V = normalize(fs_in.fV);
    vec3 N = normalize(fs_in.normal);

    vec4 deep_color = vec4(0.0, 0.0, 0.1, 1.0);
    vec4 shallow_color = vec4(0.0, 0.5, 0.5, 1.0);
    float facing = 1.0 - max(dot(V, N), 0.0);
    vec4 colorwater = mix(deep_color, shallow_color, facing);
    fColor = colorwater;

    // vec2 texScale = vec2(8,4);
    // float normalTime = mod(t, 100.0);
    // vec2 normalSpeed = vec2(-0.05, 0.0);

	// vec2 normalCoord0 = vec2(fs_in.text_coord.xy * texScale + normalTime * normalSpeed);
	// vec2 normalCoord1 = vec2(fs_in.text_coord.xy * texScale * 2 + normalTime * normalSpeed * 4);
	// vec2 normalCoord2 = vec2(fs_in.text_coord.xy * texScale * 4 + normalTime * normalSpeed * 8);

    // vec3 normal_0 = vec3(texture(normal_mapping, normalCoord0) * 2 - 1);
	// vec3 normal_1 = vec3(texture(normal_mapping, normalCoord1) * 2 - 1);
	// vec3 normal_2 = vec3(texture(normal_mapping, normalCoord2) * 2 - 1);

    // vec3 n_bump = normalize(n1+n2+n3);
    
    // vec3 n = (normal_model_to_world * fs_in.TBN * vec4(n_bump, 1.0)).xyz;



    // vec3 worldPos = vec3(vertex_model_to_world * vec4(vertex, 1.0));


    




    // vec3 N = normalize(fs_in.TBN * normal);
    // vec3 L = normalize(fs_in.fL);
    // vec3 V = normalize(fs_in.fV);
    // vec3 R = normalize(reflect(-V,N));
    // vec4 reflection = ????;


    // float facing = 1.0 - max(dot(V, n), 0.0);
    
    // vec3 color = colorwater + reflection;

}