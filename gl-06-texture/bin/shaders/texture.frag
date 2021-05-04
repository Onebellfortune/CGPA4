#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform sampler2D	TEX0;
uniform sampler2D	TEX1;
uniform sampler2D	TEX2;
uniform int			mode;

void main()
{	
	if(mode==1)			fragColor = texture( TEX0, tc );
	else if(mode==2)	fragColor = texture( TEX1, tc ).rrrr;
	else if(mode==3)	fragColor = texture( TEX2, tc ).aaaa;
	else				fragColor = vec4(tc,0,0);
}
