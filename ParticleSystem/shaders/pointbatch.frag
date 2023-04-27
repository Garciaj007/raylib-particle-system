#version 330 core

out vec4 color;

varying vec4 vColor;

float circle(in vec2 _st){
    vec2 dist = _st-vec2(0.5);
	return 1.-smoothstep(1.0-0.01,
                         1.0+0.01,
                         dot(dist,dist)*4.000);
}

void main() 
{
    float a = vColor.a * circle(gl_PointCoord);
    color = vec4(vColor.xyz, a);
}
