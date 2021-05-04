#include "cgmath.h"		// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"		// slee's OpenGL utility

//*************************************
// global constants
static const char*	window_name			= "cgbase - texture";
static const char*	vert_shader_path	= "../bin/shaders/texture.vert";
static const char*	frag_shader_path	= "../bin/shaders/texture.frag";
static const char*	lena_image_path		= "../bin/images/lena.jpg";		// RGB image
static const char*	gray_image_path		= "../bin/images/gray.jpg";		// gray-scale image
static const char*	snow_image_path		= "../bin/images/snow.png";		// RGBA image

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_scale_by_dpi( 512, 512 ); // use ivec2(width,height) to ignore dpi-aware scale

//*************************************
// OpenGL objects
GLuint	program	= 0;		// ID holder for GPU program
GLuint  vertex_array = 0;	// ID holder for vertex array object
GLuint	LENA = 0;			// RGB texture object
GLuint	GRAY = 0;			// R texture object
GLuint	SNOW = 0;			// RGBA texture object

//*************************************
// global variables
int		frame = 0;			// index of rendering frames
uint	mode = 0;			// texture display mode: 0=texcoord, 1=RGB, 2=Gray, 3=Alpha

//*************************************
void update()
{
	glUseProgram( program );
	glUniform1i( glGetUniformLocation( program, "mode"), mode );
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// bind program
	glUseProgram( program );

	// bind textures
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, LENA );
	glUniform1i( glGetUniformLocation( program, "TEX0"), 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, GRAY );
	glUniform1i( glGetUniformLocation( program, "TEX1"), 1 );

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, SNOW );
	glUniform1i( glGetUniformLocation( program, "TEX2"), 2 );

	// bind vertex array object
	glBindVertexArray( vertex_array );

	// render quad vertices
	glDrawArrays( GL_TRIANGLES, 0, 6 );

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2( width, height );
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press 'd' to toggle display mode (0: texcoord, 1: RGB, 2: Gray, 3: Alpha\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_D)
		{
			mode = (mode+1)%4;
			printf( "using mode %d: %s\n", mode, mode==0?"texcoord":mode==1?"RGB":mode==2?"Gray":"Alpha" );
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
}

void motion( GLFWwindow* window, double x, double y )
{
}

// this function will be avaialble as cg_create_texture() in other samples
GLuint create_texture( const char* image_path, bool mipmap=true, GLenum wrap=GL_CLAMP_TO_EDGE, GLenum filter=GL_LINEAR )
{
	// load image
	image*	i = cg_load_image( image_path ); if(!i) return 0; // return null texture; 0 is reserved as a null texture
	int		w=i->width, h=i->height, c=i->channels;

	// induce internal format and format from image
	GLint	internal_format = c==1?GL_R8:c==2?GL_RG8:c==3?GL_RGB8:GL_RGBA8;
	GLenum	format = c==1?GL_RED:c==2?GL_RG:c==3?GL_RGB:GL_RGBA;

	// create a src texture (lena texture)
	GLuint texture;
	glGenTextures( 1, &texture ); if(texture==0){ printf("%s(): failed in glGenTextures()\n", __func__ ); return 0; }
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, i->ptr );
	if(i){ delete i; i=nullptr; } // release image

	// build mipmap
	if( mipmap )
	{
		int mip_levels=0; for( int k=w>h?w:h; k; k>>=1 ) mip_levels++;
		for( int l=1; l < mip_levels; l++ )
			glTexImage2D( GL_TEXTURE_2D, l, internal_format, (w>>l)==0?1:(w>>l), (h>>l)==0?1:(h>>l), 0, format, GL_UNSIGNED_BYTE, nullptr );
		if(glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, !mipmap?filter:filter==GL_LINEAR?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST_MIPMAP_NEAREST );

	return texture;
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );			// turn on backface culling
	glEnable( GL_DEPTH_TEST );			// turn on depth tests
	glEnable( GL_TEXTURE_2D );			// enable texturing
	glActiveTexture( GL_TEXTURE0 );		// notify GL the current texture slot is 0
	
	// create corners and vertices
	vertex corners[4];
	corners[0].pos=vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos=vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos=vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos=vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	// generation of vertex buffer is the same, but use vertices instead of corners
	GLuint vertex_buffer;
	glGenBuffers( 1, &vertex_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if(vertex_array) glDeleteVertexArrays(1,&vertex_array);
	vertex_array = cg_create_vertex_array( vertex_buffer );
	if(!vertex_array){ printf("%s(): failed to create vertex aray\n",__func__); return false; }

	// load the Lena image to a texture
	LENA = create_texture( lena_image_path, true ); if(!LENA) return false;
	GRAY = create_texture( gray_image_path, true ); if(!GRAY) return false;
	SNOW = create_texture( snow_image_path, true ); if(!SNOW) return false;
	
	// set default display mode to lena
	keyboard( window, GLFW_KEY_D, 0, GLFW_PRESS, 0 );

	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
