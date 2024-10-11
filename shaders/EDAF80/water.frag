#version 410

uniform float t;
uniform sampler2D normal_map;
uniform samplerCube cubemap;
uniform mat4 normal_model_to_world;

in VS_OUT {
	vec2 tex_coord;
    vec3 vertex;
    mat4 TBN;
    vec3 fV;
    vec3 fN;
    vec3 fL; 
} fs_in;

out vec4 fColor;

void main(){

    vec2 texScale = vec2(8,4);
    float normalTime = mod(t, 100.0);
    vec2 normalSpeed = vec2(-0.05, 0.0);
	vec2 normalCoord0 = vec2(fs_in.tex_coord * texScale + normalTime * normalSpeed);
	vec2 normalCoord1 = vec2(fs_in.tex_coord * texScale * 2 + normalTime * normalSpeed * 4);
	vec2 normalCoord2 = vec2(fs_in.tex_coord * texScale * 4 + normalTime * normalSpeed * 8);
    vec3 n1 = vec3(texture(normal_map, normalCoord0) * 2 - 1);
	vec3 n2 = vec3(texture(normal_map, normalCoord1) * 2 - 1);
	vec3 n3 = vec3(texture(normal_map, normalCoord2) * 2 - 1);
    vec3 n_bump = normalize(n1+n2+n3);

    vec3 n = (normal_model_to_world * fs_in.TBN * vec4(n_bump, 1.0)).xyz;

    vec3 V = normalize(fs_in.fV);
    vec3 N = normalize(n);
    vec3 L = normalize(fs_in.fL);
    vec3 R = normalize(reflect(-V,N));


    vec4 deep_color = vec4(0.0, 0.0, 0.1, 1.0);
    vec4 shallow_color = vec4(0.0, 0.5, 0.5, 1.0);
    float facing = 1.0 - max(dot(V, N), 0.0);
    vec4 colorwater = mix(deep_color, shallow_color, facing);
    vec4 reflection = vec4(texture(cubemap , R));




    fColor = colorwater + reflection;
}