#version 110

varying vec3 normal;

void main(void)
{
	vec3 n;

	n = vec3(normal.x/2.0+1, normal.y/2.0+1, normal.z/2.0+1);
	n = normalize(n);

	float depth = clamp(gl_FragCoord.z, 0.0, 1.0);

	gl_FragData[0] = gl_Color;

//	if (depth == 0)
//		gl_FragData[1] = vec4( 1.0, 0.0, 0.0, 1.0 );
//	else
		gl_FragData[1] = vec4(depth, depth, depth, 1.0);

	gl_FragData[2] = vec4(n, 1.0);
}