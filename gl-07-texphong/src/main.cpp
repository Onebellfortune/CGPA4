#include "cgmath.h"		// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball

//*************************************
// global constants
static const char*	window_name = "cgbase - textured shading";
static const char*	vert_shader_path = "../bin/shaders/texphong.vert";
static const char*	frag_shader_path = "../bin/shaders/texphong.frag";
static const char*	mesh_vertex_path = "../bin/mesh/head.vertex.bin";	// http://graphics.cs.williams.edu/data/meshes.xml
static const char*	mesh_index_path	= "../bin/mesh/head.index.bin";		// http://graphics.cs.williams.edu/data/meshes.xml
static const char*	mesh_texture_path = "../bin/mesh/head.jpg";			// http://graphics.cs.williams.edu/data/meshes.xml

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 0, -100, 0 );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );
	
	float	fovy = PI/4.0f; // must be in radian
	float	aspect_ratio;
	float	dNear = 10.0f;
	float	dFar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4( 0.0f, -50.0f, 30.0f, 1.0f );   // directional light
    vec4	ambient  = vec4( 0.2f, 0.2f, 0.2f, 1.0f );
    vec4	diffuse  = vec4( 0.8f, 0.8f, 0.8f, 1.0f );
    vec4	specular = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
};

struct material_t
{
	vec4	ambient  = vec4( 0.2f, 0.2f, 0.2f, 1.0f );
    vec4	diffuse  = vec4( 0.8f, 0.8f, 0.8f, 1.0f );
    vec4	specular = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	float	shininess = 1000.0f;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;	// index of rendering frames
int		mode = 0;	// display mode

//*************************************
// scene objects
mesh*		p_mesh = nullptr;
camera		cam;
trackball	tb;
light_t		light;
material_t	material;

//*************************************
void update()
{
	// update projection matrix
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar );

	// build the model matrix for oscillating scale
	float t = float(glfwGetTime());
	//float scale	= 1.0f+float(cos(t*1.5f))*0.05f;
	float scale = 1.0f;
	mat4 model_matrix = mat4::scale( scale, scale, scale );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	//uloc = glGetUniformLocation( program, "model_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, model_matrix );

	// setup light properties
	glUniform4fv( glGetUniformLocation( program, "light_position" ), 1, light.position );
	glUniform4fv( glGetUniformLocation( program, "Ia" ), 1, light.ambient );
	glUniform4fv( glGetUniformLocation( program, "Id" ), 1, light.diffuse );
	glUniform4fv( glGetUniformLocation( program, "Is" ), 1, light.specular );

    // setup material properties
	glUniform4fv( glGetUniformLocation( program, "Ka" ), 1, material.ambient );
	glUniform4fv( glGetUniformLocation( program, "Kd" ), 1, material.diffuse );
	glUniform4fv( glGetUniformLocation( program, "Ks" ), 1, material.specular );
	glUniform1f( glGetUniformLocation( program, "shininess" ), material.shininess );

	// setup texture
	glActiveTexture( GL_TEXTURE0 );								// select the texture slot to bind
	glBindTexture( GL_TEXTURE_2D, p_mesh->texture );			
	glUniform1i( glGetUniformLocation( program, "TEX" ), 0 );	 // GL_TEXTURE0
	glUniform1i( glGetUniformLocation( program, "mode" ), mode );
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// notify GL that we use our own program
	glUseProgram(program);

	// bind vertex array object
	if(p_mesh&&p_mesh->vertex_array) glBindVertexArray( p_mesh->vertex_array );
	
	// render vertices: trigger shader programs to process vertex data
	glDrawElements( GL_TRIANGLES, p_mesh->index_list.size(), GL_UNSIGNED_INT, nullptr );

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf( "- press 'd' to toggle display mode\n" );
	printf( "  - 0: textured shading\n" );
	printf( "  - 1: shading only\n" );
	printf( "  - 2: texture only\n" );
	printf( "  - 3: texture coordinates\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_HOME)					cam=camera();
		else if(key==GLFW_KEY_D)					mode=(mode+1)%4;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if(action==GLFW_PRESS)			tb.begin( cam.view_matrix, npos );
		else if(action==GLFW_RELEASE)	tb.end();
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	cam.view_matrix = tb.update( npos );
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

	// load mesh and texture from image
	p_mesh = cg_load_mesh( mesh_vertex_path, mesh_index_path ); if(!p_mesh){ printf( "%s(): Unable to load mesh\n", __func__ ); return false; }
	p_mesh->texture = cg_create_texture( mesh_texture_path, true ); if(!p_mesh->texture) return false;

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

