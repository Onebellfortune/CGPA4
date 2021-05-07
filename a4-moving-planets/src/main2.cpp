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
static const char* earth_texture = "../bin/textures/earth.jpg";

uint	NUM_TESS = 72;

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 30, -100, 10 );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );
	
	float	fovy = PI/4.0f; // must be in radian
	float	aspect_ratio;
	float	dNear = 1.0f;
	float	dFar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4( 0.0f, 0.0f, 0.0f, 1.0f );   // directional light
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
bool	b_index_buffer = true;
GLuint	vertex_array = 0;
std::vector<vertex>	solar_system;
GLuint	EARTH;
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
	float scale	= 10.0f+float(cos(t*1.5f))*0.5f;
	mat4 model_matrix = mat4::rotate(vec3(0,0,1),t)*mat4::translate(50,0,0)*mat4::scale( scale, scale, scale );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	uloc = glGetUniformLocation( program, "model_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, model_matrix );

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
	glBindTexture( GL_TEXTURE_2D, EARTH );			
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
	//if(p_mesh&&p_mesh->vertex_array) glBindVertexArray( p_mesh->vertex_array );
	glBindVertexArray(vertex_array);
	// render vertices: trigger shader programs to process vertex data
	//glDrawElements( GL_TRIANGLES, p_mesh->index_list.size(), GL_UNSIGNED_INT, nullptr );
	glDrawElements(GL_TRIANGLES, NUM_TESS * (NUM_TESS) * 3, GL_UNSIGNED_INT, nullptr);
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

void update_vertex_buffer(const std::vector<vertex>& vertices, uint N)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if (b_index_buffer)
	{
		std::vector<uint> indices;
		uint k1, k2;
		for (uint k = 0; k <= N; k++)
		{
			k1 = k * (N + 1);  // k1 -- k1+1
							 // |	/		
			k2 = k1 + N + 1; // k2 -- k2+1   

			for (uint j = 0; j <= N; j++)
			{
				k1++;
				k2++;
				if (k != 0) {
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (k != N - 1) {
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}


			}
		}

		// generation of vertex buffer: use vertices as it is
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	}
	else
	{
		std::vector<vertex> v; // triangle vertices
		//printf("%lf\n", r);
		for (uint k = 0; k <= N * N / 2; k++)
		{
			v.push_back(vertices.front());	// the origin
			v.push_back(vertices[k + 1]);
			v.push_back(vertices[k + 2]);
			//printf("%lf\n", v[k].pos.x);
			printf("vector");
		}


		/*for (uint j = 0; j <= N / 2; j++) {
			t = PI * 1.0f * k / float(N / 2), ct = cos(t), st = sin(t);

		}*/

		//t = PI * 1.0f * k / float(N / 2), ct = cos(t), st = sin(t);//theta=latitude.
		//v.push_back({ vec3(r * st * cp,r * st * sp,r * ct), vec3(st * cp,st * sp,cp), vec2(p / (2.0f * PI),1.0f - (t / PI)) });


	// generation of vertex buffer: use triangle_vertices instead of vertices

		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * v.size(), &v[0], GL_STATIC_DRAW);
	}

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_circle_vertices(uint N)
{
	std::vector<vertex> v = { { vec3(0,0,0), vec3(0,0,-1.0f), vec2(0.5f) } }; // origin

	for (uint k = 0; k <= N / 2; k++)
	{
		float t = PI * 2.0f * k / float(N), cost = cos(t), sint = sin(t);
		for (uint j = 0; j <= N; j++) {
			float p = PI * 2.0f * j / float(N), cosp = cos(p), sinp = sin(p);//theta=latitude
			v.push_back({ vec3(sint * cosp,sint * sinp,cost), vec3(sint * cosp,sint * sinp,cost), vec2(p / (2 * PI),1.0f - t / PI) });
		}

	}
	return v;
}

GLuint create_texture(const char* image_path, bool mipmap = true, GLenum wrap = GL_CLAMP_TO_EDGE, GLenum filter = GL_LINEAR)
{
	// load image
	image* i = cg_load_image(image_path); if (!i) return 0; // return null texture; 0 is reserved as a null texture
	int		w = i->width, h = i->height, c = i->channels;

	// induce internal format and format from image
	GLint	internal_format = c == 1 ? GL_R8 : c == 2 ? GL_RG8 : c == 3 ? GL_RGB8 : GL_RGBA8;
	GLenum	format = c == 1 ? GL_RED : c == 2 ? GL_RG : c == 3 ? GL_RGB : GL_RGBA;

	// create a src texture (lena texture)
	GLuint texture;
	glGenTextures(1, &texture); if (texture == 0) { printf("%s(): failed in glGenTextures()\n", __func__); return 0; }
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, i->ptr);
	if (i) { delete i; i = nullptr; } // release image

	// build mipmap
	if (mipmap)
	{
		int mip_levels = 0; for (int k = w > h ? w : h; k; k >>= 1) mip_levels++;
		for (int l = 1; l < mip_levels; l++)
			glTexImage2D(GL_TEXTURE_2D, l, internal_format, (w >> l) == 0 ? 1 : (w >> l), (h >> l) == 0 ? 1 : (h >> l), 0, format, GL_UNSIGNED_BYTE, nullptr);
		if (glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, !mipmap ? filter : filter == GL_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

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


	solar_system = std::move(create_circle_vertices(NUM_TESS));
	EARTH = create_texture(earth_texture, true); if (!EARTH) return false;
	update_vertex_buffer(solar_system, NUM_TESS);
	// load mesh and texture from image
	//p_mesh = cg_load_mesh( mesh_vertex_path, mesh_index_path ); if(!p_mesh){ printf( "%s(): Unable to load mesh\n", __func__ ); return false; }
	//p_mesh->texture = cg_create_texture( mesh_texture_path, true ); if(!p_mesh->texture) return false;

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

