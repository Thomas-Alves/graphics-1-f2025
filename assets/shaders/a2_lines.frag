#version 430
out vec4 fragColor;

uniform vec3 u_color;

in vec3 color;

void main()
{
    fragColor = vec4(u_color * color, 1.0);
}
