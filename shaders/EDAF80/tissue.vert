#version 410
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform float t;

struct WaveConfig {
    vec2 direction;
    float amplitude;
    float frequency;
    float phase;
    float sharpness;
};

float alpha(vec2 direction, float frequency, float phase, float sharpness, float time){
    return pow(sin((vertex.x * direction.x + vertex.z * direction.y) * frequency + phase * time) * 0.5 + 0.5, sharpness);
}

vec3 calcWave(in WaveConfig wc){
    float amplitude = wc.amplitude;
    float frequency = wc.frequency;
    float phase = wc.phase;
    float sharpness = wc.sharpness;
    vec2 direction = wc.direction;

    float w = amplitude * alpha(direction, frequency, phase, sharpness, t);
    float displacement = amplitude * cos((vertex.x * direction.x + vertex.z * direction.y) * frequency + phase * t);
    float x = direction.x * displacement * alpha(direction, frequency, phase, sharpness, t-1);
    float z = direction.y * displacement * alpha(direction, frequency, phase, sharpness, t-1);
    return vec3(w, x, z);
}

vec3 animate(){
    WaveConfig config1 = WaveConfig(vec2(-1.0, 0.0), 1, 1.2, 0.5, 2.0);
    WaveConfig config2 = WaveConfig(vec2(-0.7, 0.7), 0.3, 0.4, 1.3, 2.0);

    vec3 res1 = calcWave(config1);
    vec3 res2 = calcWave(config2);

    float y = res1.x + res2.x;
    float Hx = res1.y + res2.y;
    float Hz = res1.z + res2.z;
    return vec3(Hx, y, Hz);
}


void main(){
    vec3 worldPos = vec3(vertex_model_to_world * vec4(vertex, 1.0));

    vec3 res = animate();
    float y = res.y;
    vec3 normal = vec3(-res.x, 1.0, -res.z);

    vec3 displaced_vertex = vertex;
    displaced_vertex.y += y;
    
    

    gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(displaced_vertex, 1.0);
}