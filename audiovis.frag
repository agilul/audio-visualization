// Copyright (c) 2016 Yat Hin Wong

#ifdef GL_ES
precision mediump float;
#endif

#define ITERATIONS 32
#define EPSILON 0.0001
#define MAXDIST 10000.0

#define SPECULAR vec3(1.0, 1.0, 1.0)
#define SHININESS 10.0

#define CAMERAPOS vec3(0.0, 0.0, -10.0)
#define LIGHTPOS vec3(0.0, 0.0, -10.0)

uniform vec2 uRes;
uniform float uTime;
uniform sampler2D uSpectrum;

// sphere distance function
float sphere(vec3 p, float radius)
{
	return length(p) - radius;
}

// box distance function
float box(vec3 p, vec3 b)
{
	return length(max(abs(p) - b, 0.0));
}

// scene
float map(vec3 p)
{
	float c = cos(uTime);
	float s = sin(uTime);
	mat3 rot;
	rot[0] = vec3(c, 0.0, -s);
	rot[1] = vec3(0.0, 1.0, 0.0);
	rot[2] = vec3(s, 0.0, c);
	p = inverse(rot)*p;
	return sphere(p + texture2D(uSpectrum, vec2(0.0)).x*sin(3.0*p.x)*sin(3.0*p.y)*sin(3.0*p.z)*0.7, 5.0);
}

// normal
vec3 getNormal(vec3 p)
{
	vec3 e = vec3(0.001, 0.0, 0.0);
	float dx = map(p + e.xyy) - map(p - e.xyy);
	float dy = map(p + e.yxy) - map(p - e.yxy);
	float dz = map(p + e.yyx) - map(p - e.yyx);
	return normalize(vec3(dx, dy, dz));
}

// ray marching
float trace(vec3 origin, vec3 direction, out vec3 p)
{
	float distance = 0.0;
	for (int i = 0; i < ITERATIONS; ++i)
	{
		p = origin + direction * distance;
		float step = map(p);
		distance += step;
		if (step < EPSILON)
		{
			break;
		}
		if (distance >= MAXDIST)
		{
			distance = 0.0;
			break;
		}
	}
	return distance;
}

// lighting
vec3 calcLighting(vec3 p, vec3 normal, vec3 lightPos, vec3 cameraPos, vec3 color)
{
	vec3 lightDir = normalize(lightPos - p);

	float diffuse = max(dot(lightDir, normal), 0.0);
	
	vec3 cameraDir = normalize(cameraPos - p);
	vec3 reflect = normalize(reflect(-lightDir, normal));
	float specular = pow(max(dot(cameraDir, reflect), 0.0), SHININESS);
	
	specular = min(diffuse, specular);
	
	return diffuse * color + specular * SPECULAR;
}

void main()
{
	vec2 st = (gl_FragCoord.xy / uRes.xy);
	vec2 uv = st * 2.0 - 1.0;
	uv.x *= uRes.x / uRes.y;
	
	vec3 cameraDir = normalize(vec3(uv.x, uv.y, 1.0));
	vec3 point;
	float distance = trace(CAMERAPOS, cameraDir, point);

	// white bars
	vec3 color = texture2D(uSpectrum, st*0.5).rgb + texture2D(uSpectrum, (1.0-st)*0.5).rgb;
	
	if (distance > 0.0)
	{
		// sphere
		vec3 sphereColor = vec3(st, 0.5+0.5*sin(uTime));
		vec3 normal = getNormal(point);
		color = calcLighting(point, normal, LIGHTPOS + vec3(sin(uTime)*15.0, cos(uTime)*15.0, 0.0), CAMERAPOS, sphereColor);
	}
	
	gl_FragColor = vec4(color, 1.0);
}
