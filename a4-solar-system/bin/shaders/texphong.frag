#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

// texture sampler
uniform sampler2D SUN;
uniform sampler2D MERCURY;
uniform sampler2D VENUS;
uniform sampler2D EARTH;
uniform sampler2D MARS;
uniform sampler2D JUPITER;
uniform sampler2D SATURN;
uniform sampler2D NEPTUNE;
uniform sampler2D URANUS;
uniform sampler2D SATURN_RING;
uniform sampler2D URANUS_RING;
uniform sampler2D SATURN_RING_ALPHA;
uniform sampler2D URANUS_RING_ALPHA;
uniform int mode;

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

void main()
{
	// light position in the eye space
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	vec4 iKd;	// Kd from image
	if(mode==0){
		iKd=texture(SUN,tc);
		fragColor=texture(SUN,tc);
	}else if(mode==1){
		iKd=texture(MERCURY,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==2){
		iKd=texture(VENUS,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==3){
		iKd=texture(EARTH,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==4){
		iKd=texture(MARS,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==5){
		iKd=texture(JUPITER,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==6){
		iKd=texture(SATURN,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==7){
		iKd=texture(URANUS,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==8){
		iKd=texture(NEPTUNE,tc);
		fragColor=phong(l,n,h,iKd);
	}else if(mode==21){
		iKd=texture(SATURN_RING,tc.yy);
		fragColor=phong(l,n,h,iKd);
		iKd=texture(SATURN_RING_ALPHA,tc.yy);
		fragColor.a=iKd.y;
		
	}else if(mode==22){
		iKd=texture(URANUS_RING,tc.yy);
		fragColor=phong(l,n,h,iKd);
		iKd=texture(URANUS_RING_ALPHA,tc.yy);
		fragColor.a=iKd.y;
	}else				
		fragColor = phong(l,n,h,Kd);
}
