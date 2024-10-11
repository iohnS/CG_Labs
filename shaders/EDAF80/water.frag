#version 410

uniform float t;
uniform sampler2D normal_map;
uniform samplerCube cubemap;
uniform mat4 normal_model_to_world;

in VS_OUT {
	vec2 tex_coord;
    vec3 vertex;
    vec3 fV;
    vec3 fN;
    vec3 fL; 
    vec3 T;
    vec3 B;
    vec3 N;
} fs_in;

out vec4 fColor;

void main(){

    vec3 T = normalize(fs_in.T);
    vec3 B = normalize(fs_in.B);
    vec3 N = normalize(fs_in.N);

    mat3 TBN = mat3(T, B, N);

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

    vec3 n = (TBN * n_bump);

    vec3 V = normalize(fs_in.fV);
    N = normalize(n);
    vec3 L = normalize(fs_in.fL);
    vec3 R = reflect(-V,N);
    


    vec4 deep_color = vec4(0.0, 0.0, 0.1, 1.0);
    vec4 shallow_color = vec4(0.0, 0.5, 0.5, 1.0);
    float facing = 1.0 - max(dot(V, N), 0.0);
    vec4 colorwater = mix(deep_color, shallow_color, facing);
    
    vec4 reflection = vec4(texture(cubemap , R));

    float fresnel = 0.02 + 0.98 * pow(1.0 - dot(V, normalize(fs_in.fN)), 5.0);
    
    vec3 refraction = normalize(refract(-V, N, 1.0/1.33));
    vec4 refraction_color = texture(cubemap, refraction);

    fColor = colorwater + reflection * fresnel + refraction_color * (1.0 - fresnel);
    //fColor = refraction_color;
    }