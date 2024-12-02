#version 410 core

layout(triangles) in;
// Use line_strip for visualization and triangle_strip for solids
layout(triangle_strip, max_vertices = 3) out;
//layout(line_strip, max_vertices = 3) out;
in vec2 teTexCoord[3];
in vec3 teNormal[3];
out vec2 gsTexCoord;
out vec3 gsNormal;
uniform sampler2D tex;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

uniform float disp;
uniform int texon;

vec2 random2(vec2 st)
{
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

float noise(vec3);

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
float noise(vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    int x, y;
    vec3 m;
    float val;

    for (x = 0; x < 128; x++)
    for (y = 0; y < 128; y++)
    {
        m[0] = x / 32.0;
        m[1] = y / 32.0;
        m[2] = 0.0;

        val = noise(m) * 128.0;

        m[0] = x / 16.0;
        m[1] = y / 16.0;
        m[2] = 2.0;

        val += noise(m) * 64.0;

        m[0] = x / 8.0;
        m[1] = y / 8.0;
        m[2] = 3.0;

        val += noise(m) * 32.0;

        m[0] = x / 4.0;
        m[1] = y / 4.0;
        m[2] = 4.0;

        val += noise(m) * 16.0;

        m[0] = x / 2.0;
        m[1] = y / 2.0;
        m[2] = 5.0;

        val += noise(m) * 8.0;


        return val;


    }

    /*return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);*/
}

vec3 random3(vec3 st)
{
    st = vec3( dot(st,vec3(127.1,311.7, 543.21)),
              dot(st,vec3(269.5,183.3, 355.23)),
              dot(st,vec3(846.34,364.45, 123.65)) ); // Haphazard additional numbers by IR
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

// Gradient Noise by Inigo Quilez - iq/2013
// https://www.shadertoy.com/view/XdXGW8
// Trivially extended to 3D by Ingemar
float noise(vec3 st)
{
    vec3 i = floor(st);
    vec3 f = fract(st);

    vec3 u = f*f*(3.0-2.0*f);

    return mix(
    			mix( mix( dot( random3(i + vec3(0.0,0.0,0.0) ), f - vec3(0.0,0.0,0.0) ),
                     dot( random3(i + vec3(1.0,0.0,0.0) ), f - vec3(1.0,0.0,0.0) ), u.x),
                mix( dot( random3(i + vec3(0.0,1.0,0.0) ), f - vec3(0.0,1.0,0.0) ),
                     dot( random3(i + vec3(1.0,1.0,0.0) ), f - vec3(1.0,1.0,0.0) ), u.x), u.y),

    			mix( mix( dot( random3(i + vec3(0.0,0.0,1.0) ), f - vec3(0.0,0.0,1.0) ),
                     dot( random3(i + vec3(1.0,0.0,1.0) ), f - vec3(1.0,0.0,1.0) ), u.x),
                mix( dot( random3(i + vec3(0.0,1.0,1.0) ), f - vec3(0.0,1.0,1.0) ),
                     dot( random3(i + vec3(1.0,1.0,1.0) ), f - vec3(1.0,1.0,1.0) ), u.x), u.y), u.z

          	);
}

void computeVertex(int nr)
{
	vec3 p, v1, v2, v3, p1, p2, p3, s1, s2, n;

	p = vec3(gl_in[nr].gl_Position);
	// Add interesting code here
	p = normalize(p);

	float delta = 0.0001;

	vec3 line1 = normalize(cross(p, vec3(1,0,0)));

	if(line1 == vec3(0, 0, 0)) {
        line1 = normalize(cross(p, vec3(0, 1, 0)));
    }

	vec3 line2 = normalize(cross(p, line1));
	vec3 line3 = normalize(-line1-line2);

	vec3 point1 = line1 * delta + p;
	vec3 point2 = line2 * delta + p;
	vec3 point3 = line3 * delta + p;

	//point1 += noise(point1) * point1;
	//point2 += noise(point2) * point2;
	//point3 += noise(point3) * point3;

	vec3 ve1 = normalize(point2 - point1);
	vec3 ve2 = normalize(point3 - point1);



	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(p, 1.0) + noise(vec2(p.x, p.y)) / 3;

    gsTexCoord = teTexCoord[0];

	//n = teNormal[nr]; // This is not the normal you are looking for. Move along!
    gsNormal = mat3(camMatrix * mdlMatrix) * normalize(cross(ve1, ve2));

    //gsNormal = normalize(cross(ve1, ve2));

    EmitVertex();
}

void main()
{
	computeVertex(0);
	computeVertex(1);
	computeVertex(2);
}

