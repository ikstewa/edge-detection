#define KERNEL_SIZE 9

uniform sampler2D imgColor;

uniform sampler2D depColor;
uniform sampler2D normColor;


uniform float width;
uniform float height;
uniform float threshold_d;
uniform float threshold_n;
uniform vec4 edgeColor;


float step_w = 1.0/width;
float step_h = 1.0/height;
vec2 offset[KERNEL_SIZE];

float depth[KERNEL_SIZE];
vec4 norm[KERNEL_SIZE];

float g_depth[KERNEL_SIZE];
float g_norm[KERNEL_SIZE];
float gdMax, gdMin;
float gnMax, gnMin;

vec3 rgb2lum = vec3(0.30, 0.59, 0.11);

void loadPixels(vec2 texCoord)
{
	vec4 c;
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		depth[i] = texture2D(depColor, texCoord + offset[i]).r;
		norm[i] = texture2D(normColor, texCoord + offset[i]);
	}
}

void findMaxMin()
{
	gdMax = 0.0;
	gdMin = 1.0;
	gnMax = 0.0;
	gnMin = 1.0;
	
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		if (g_depth[i] > gdMax)
			gdMax = g_depth[i];
		if (g_depth[i] < gdMin)
			gdMin = g_depth[i];

		if (g_norm[i] > gnMax)
			gnMax = g_norm[i];
		if (g_norm[i] < gnMin)
			gnMin = g_norm[i];
	}
}

void main()
{
	// set the offsets
	offset[0] = vec2(-step_w, -step_h);
	offset[1] = vec2(0.0, -step_h);
	offset[2] = vec2(step_w, -step_h);
	offset[3] = vec2(-step_w, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(step_w, 0.0);
	offset[6] = vec2(-step_w, step_h);
	offset[7] = vec2(0.0, step_h);
	offset[8] = vec2(step_w, step_h);
	
	// grab the surrounding pixels
	vec2 tCoord;
	int a, i;
	for (a = 0; a < KERNEL_SIZE; a++)
	{
		tCoord = gl_TexCoord[0].st + offset[a];
		
		loadPixels(tCoord);
	
		g_depth[a] = (1.0/8.0)*(abs(depth[0]-depth[4])
					+ 2.0*abs(depth[1]-depth[4])
					+ abs(depth[2]-depth[4])
					+ 2.0*abs(depth[3]-depth[4])
					+ 2.0*abs(depth[5]-depth[4])
					+ abs(depth[6]-depth[4])
					+ 2.0*abs(depth[7]-depth[4])
					+ abs(depth[8]-depth[4]));
		g_norm[a] = (1.0/8.0)*(length(norm[0]-norm[4])
			+ 2.0*length(norm[1]-norm[4])
			+ length(norm[2]-norm[4])
			+ 2.0*length(norm[3]-norm[4])
			+ 2.0*length(norm[5]-norm[4])
			+ length(norm[6]-norm[4])
			+ 2.0*length(norm[7]-norm[4])
			+ length(norm[8]-norm[4]));
	}
	
	findMaxMin();
	
	float td = (gdMax-gdMin)/threshold_d;
	float tn = (gnMax-gnMin)/threshold_n;
	
	float edge_d = min(td*td, 1.0);
	float edge_n = min(tn*tn, 1.0);
	


	if (edge_d == 1.0 || edge_n == 1.0)
		gl_FragColor = edgeColor;
	else
//		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		gl_FragColor = texture2D(imgColor, gl_TexCoord[0].st);


	
	
	//gl_FragColor = vec4(edge, edge, edge, 1.0);
	//gl_FragColor = vec4(g, 0.0, 0.0, 1.0);
	//if (g[0] <= threshold)
	//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	//else
	//	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}


