#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color; 

smooth out vec4 the_color; 

uniform float z_offset;
uniform float frustum_scale;
uniform float near; 
uniform float far;

void main() {

	vec4 camera_pos = position;
	camera_pos.z -= z_offset;

	vec4 clip_pos;
	clip_pos.xy = camera_pos.xy * frustum_scale;
	
	clip_pos.z = camera_pos.z * (near + far) / (near - far); 
	clip_pos.z += 2 * near * far / (near - far);
	clip_pos.w = -camera_pos.z;

	gl_Position = clip_pos;
	the_color = color;
}
