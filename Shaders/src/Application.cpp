#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

// Enum to differentiate which Shader we have
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

// Attempt to read the shader file and parse the data
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

// Tries to take our Shader and compile it into openGL
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	// Create the shader program
	GLCall(unsigned int id = glCreateShader(type));
	// Get the source
	const char* src = source.c_str();
	// Specify the source of the shader, how many source codes are we specifying, pnter of source, length;
	GLCall(glShaderSource(id, 1, &src, nullptr));
	// Compile the shader
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	// If it failed to compile
	if (result == GL_FALSE)
	{
		// Get the shader
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*) alloca(length * sizeof(char));
		// Read the logs
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	// Return the compiled shader
	return id;
}

// Provides OpenGL with our shader src code, src text, link it together, then return an identifier
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	// Create the shader program
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Link the shaders into openGL
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));
	// Now that they are linked, we do not need our intermediates
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	// Return the compiled and linked program
	return program;
}


// To Create our shader
// #version - States we are using GLSL
// 330 - version 330
// core - do not use any depricated functions
// layout(location = 0) is the index of the attribute

// Run our application
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	// Create the context with the core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		std::cout << "Error initializing GLFW!" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Sync this window with our monitors refresh rate
	glfwSwapInterval(1);

	/* Initialize GLEW */
	if (glewInit() != GLEW_OK) 
	{
		std::cout << "Error initializing GLEW!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Display the GL version
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Scope to keep the OpenGL context
	{
		// triangle vertices
		float positions[] = {
			-0.5f, -0.5f, // 0
			 0.5f, -0.5f, // 1
			 0.5f,  0.5f, // 2
			-0.5f,  0.5f, // 3
		};
		// Index buffer that will tell openGL how to render this object without duplicates
		unsigned int indices[] = {
			0, 1, 2, // triangle 1
			2, 3, 0, // triangle 2
		};

		// In the core , we need a vertex array object
		unsigned int vao;
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		// Create an IndexBuffer
		IndexBuffer ib(indices, 6);

		// Shader source loaded from our res dir
		ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

		// Compile our shaders together
		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		// Bind our shader
		GLCall(glUseProgram(shader));

		// Retrieve the uniforms ID
		GLCall(int location = glGetUniformLocation(shader, "u_Color"));
		ASSERT(location != -1);
		// Pass our data to the shader uniform
		GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

		// Unbind everything
		GLCall(glBindVertexArray(0));
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			// Bind the shader program & Pass our data to the shader uniform
			GLCall(glUseProgram(shader));
			GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

			// Bind the index buffer & vertex array
			va.Bind();
			ib.Bind();

			// Draw our buffer
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			// Animate Red Channel
			if (r > 1.0f) increment = -0.05f;
			else if (r < 0.0f) increment = 0.05f;
			r += increment;



			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		GLCall(glDeleteProgram(shader));
	}

	glfwTerminate();
	return 0;
}