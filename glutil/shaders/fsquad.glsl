#version 430 core

layout (location = 0) in vec2 vPosition;

out gl_PerVertex {
    vec4 gl_Position;
};

out vec2 fTexcoord;

void main (void)
{
	gl_Position = vec4 (vPosition, 0, 1);
	fTexcoord = 0.5 * vPosition + 0.5;
}
