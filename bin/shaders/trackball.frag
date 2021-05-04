#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec2 tc;
in vec3 norm;
in vec4 epos;

// the only output variable

out vec4 fragColor;


uniform int			tc_xy;
uniform sampler2D	SUN;
uniform sampler2D	MERCURY;
uniform	sampler2D	VENUS;
uniform	sampler2D	EARTH;
uniform	sampler2D	MARS;
uniform	sampler2D	JUPITER;
uniform	sampler2D	SATURN;
uniform	sampler2D	NEPTUNE;
uniform	sampler2D	URANUS;


uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;		

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
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	//vec4 iKd;
	if(mode==0){
		fragColor=texture(SUN,tc);
	}else if(mode==1){
		vec4 iKd_mercury=texture(MERCURY,tc);
		fragColor=phong(l,n,h,iKd_mercury);
		//fragColor=texture(MERCURY,tc);
	}else if(mode==2){
		vec4 iKd_venus=texture(VENUS,tc);
		fragColor=phong(l,n,h,iKd_venus);
		//fragColor=texture(VENUS,tc);
	}else if(mode==3){
		vec4 iKd_earth=texture(EARTH,tc);
		fragColor=phong(l,n,h,iKd_earth);
		//fragColor=texture(EARTH,tc);
	}else if(mode==4){
		vec4 iKd_mars=texture(MARS,tc);
		fragColor=phong(l,n,h,iKd_mars);
		//fragColor=texture(MARS,tc);
	}else if(mode==5){
		vec4 iKd_jupiter=texture(JUPITER,tc);
		fragColor=phong(l,n,h,iKd_jupiter);
		//fragColor=texture(JUPITER,tc);
	}else if(mode==6){
		vec4 iKd_saturn=texture(SATURN,tc);
		fragColor=phong(l,n,h,iKd_saturn);
		//fragColor=texture(SATURN,tc);
	}else if(mode==7){
		vec4 iKd_neptune=texture(NEPTUNE,tc);
		fragColor=phong(l,n,h,iKd_neptune);
		//fragColor=texture(NEPTUNE,tc);
	}else if(mode==8){
		vec4 iKd_uranus=texture(URANUS,tc);
		fragColor=phong(l,n,h,iKd_uranus);
		//fragColor=texture(URANUS,tc);
	}else{
		vec4 iKd=vec4(tc.xy,0,0);
		fragColor=phong(l,n,h,iKd);
	}
		
	
}
