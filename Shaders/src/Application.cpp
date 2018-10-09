#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

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

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	// Create the shader program
	unsigned int id = glCreateShader(type);
	// Get the source
	const char* src = source.c_str();
	// Specify the source of the shader, how many source codes are we specifying, pnter of source, length;
	glShaderSource(id, 1, &src, nullptr);
	// Compile the shader
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	// If it failed to compile
	if (result == GL_FALSE)
	{
		// Get the shader
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		// Read the logs
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	// Return the compiled shader
	return id;
}

// Provides OpenGL with our shader src code, src text, link it together, then return an identifier
static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	// Create the shader program
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Link the shaders into openGL
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	// Now that they are linked, we do not need our intermediates
	glDeleteShader(vs);
	glDeleteShader(fs);

	// Return the compiled and linked program
	return program;
}


// To Create our shader
// #version - States we are using GLSL
// 330 - version 330
// core - do not use any depricated functions
// layout(location = 0) is the index of the attribute


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

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

	/* Initialize GLEW */
	if (glewInit() != GLEW_OK) 
	{
		std::cout << "Error initializing GLEW!" << std::endl;
		glfwTerminate();
		return -1;
	}

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

	// Send the vertices to the GPU
	// 1 buffer
	// give pointer to unsigned int
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

	// Enable the vertex attribute
	glEnableVertexAttribArray(0);
	// Define the structure of our buffer input
	// First attribute, 2 components define this attribute, they are floats, not normalized, 2 float values define the size, next attribute offset
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	// Send the indices to the GPU
	// 1 buffer
	// give pointer to unsigned int
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), indices, GL_STATIC_DRAW);


	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::cout << "Vertex" << std::endl;
	std::cout << source.VertexSource << std::endl;
	std::cout << "Fragments" << std::endl;
	std::cout << source.FragmentSource << std::endl;


	// Compile our shaders together
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw our buffer
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}