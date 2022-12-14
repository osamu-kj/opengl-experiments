#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "shaders-loader.h"

splitted_strings_arr split(char *data, size_t size, char delimiter, int free_data=1) {
	split_string_properties properties = {0,0,0};
	splitted_string *output_data = NULL;

	for (int i = 0; i < size; i++) {
		char current_char = *(data + i); 
		if (delimiter != current_char && size-1 > i) continue; 

		properties.found_counter++; 
		output_data = (splitted_string *) realloc(output_data, (size_t) ((properties.found_counter+1) * sizeof(splitted_string)));
		if (NULL == output_data) {
			fprintf(stderr, "error: failed to reallocate memory");
			return splitted_strings_arr { NULL, -1 }; 
		}

		properties.split_end_index = i; 
		if (delimiter != *(data+properties.split_end_index)) properties.split_end_index++;
		
		size_t split_size = properties.split_end_index - properties.split_start_index; 
		char *split_string = (char *) malloc(split_size+1); 
	
		strncpy(split_string, (const char*) (data + properties.split_start_index), split_size); 	
		*(split_string+split_size) = 0x0;
		
		*(output_data+properties.found_counter) = { split_size, split_string }; 
		if (size-1 >= i+1) properties.split_start_index = i+1; 
	}

	if (1 == free_data) free(data);

	return splitted_strings_arr { output_data, properties.found_counter }; 
}

long int sizeof_file(const char *path) {
	FILE *file_pointer = fopen(path, "r"); 

	if (NULL == file_pointer) {
		fprintf(stderr, "File can't be opened\n");
		return -1;
	}

	fseek(file_pointer, 0L, SEEK_END); 

	return (long int) ftell(file_pointer) / sizeof(char); 
}

void read_file(const char *path, char *file_contents) {
	FILE *file_pointer = fopen(path, "r");

	if (NULL == file_pointer) {
		fprintf(stderr, "File can't be opened\n"); 
		return;
	}

	int character_index = -1;
	char curr_char; 
	
	do {
		curr_char = fgetc(file_pointer); 
		character_index++; 

		if (-1 == curr_char) {
			file_contents[character_index] = 0x0; 
			break;	
		}

		file_contents[character_index] = curr_char; 

	} while (EOF != curr_char);
	
	fclose(file_pointer); 
}

void check_shader(GLuint shader, char *path) {
	GLint result = GL_FALSE; 
	int log_length; 

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result); 
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length); 

	char *shader_error = (char *) malloc((log_length > 1) ? log_length : 1);
	glGetShaderInfoLog(shader, log_length, NULL, &shader_error[0]);

	if (0x0 != shader_error[0]) {
		fprintf(stderr, "Shader '%s': %s\n", path, shader_error);
	}
}

void load_shader(GLuint program, const char *path, GLenum type) {
	GLuint shader = glCreateShader(type); 

	long int shader_size = sizeof_file(path);
	if (0 == shader_size) {
		fprintf(stdout, "info: '%s' file shader size is 0\n", path); 
		return; 
	}
	char* file_contents = (char *) malloc(shader_size); 

	read_file(path, file_contents);  

	// compile shader
	glShaderSource(shader, 1, &file_contents, NULL);
	glCompileShader(shader);

	// check the shader
	check_shader(shader, (char *) path);

	// attach shader & link shader
	glAttachShader(program, shader);
	glLinkProgram(program);

	// free memory and junk
	glDeleteShader(shader);
	free(file_contents);
}

vertex_data load_model(const char *path) {
	int vertex_counter = 0, face_vertex_counter = 0; 
	vertex *vertecies = NULL; 
	float *output_buffer = NULL;

	long int src_size = sizeof_file(path);

	if (0 == src_size) {
		fprintf(stdout, "info: model file is empty!");
		return (vertex_data) { };
	}

	char *model_src = (char *) malloc(src_size); 

	read_file(path, model_src);
	splitted_strings_arr lines = split(model_src, (size_t) src_size, '\n');

	for (int i = 1; i <= lines.elements; i++) {
		char *line = (lines.splitted_strings_arr+i)->string;
		size_t line_length = (lines.splitted_strings_arr+i)->size;
		
		splitted_strings_arr tokens = split(line, line_length, ' ', 0); 

		if (0 == strcmp("v", (tokens.splitted_strings_arr+1)->string)) {
			vertex_counter++; 

			vertecies = (vertex *) realloc(vertecies, (size_t) (vertex_counter * sizeof(vertex)));	
			if (NULL == vertecies) {
				fprintf(stderr, "error: failed to reallocate memory");
				return (vertex_data) {} ;
			}

			float x = atof( (tokens.splitted_strings_arr+2)->string), 
						y = atof( (tokens.splitted_strings_arr+3)->string), 
						z = atof( (tokens.splitted_strings_arr+4)->string), 
					w = 1.0f; 

			*(vertecies+vertex_counter-1) = { x, y, z, w } ;
		}
		else if (0 == strcmp("f", (tokens.splitted_strings_arr+1)->string)) {
			for (int v = 2; v < 5; v++) {
				face_vertex_counter++;
				char *vertex_value = (tokens.splitted_strings_arr+v)->string;
				size_t length = (tokens.splitted_strings_arr+v)->size; 
				
				splitted_strings_arr vertex_data = split(vertex_value, length, '/');
				int vertex_index = atoi( (vertex_data.splitted_strings_arr+1)->string);	
				
				float x = (vertecies+vertex_index-1)->x,
							y = (vertecies+vertex_index-1)->y,
							z = (vertecies+vertex_index-1)->z,
							w = (vertecies+vertex_index-1)->w;
				
				output_buffer = (float *) realloc(output_buffer, (size_t) (face_vertex_counter * sizeof(float) * 4));
				
				*(output_buffer+((face_vertex_counter-1)*4)) = x; 
				*(output_buffer+((face_vertex_counter-1)*4)+1) = y; 
				*(output_buffer+((face_vertex_counter-1)*4)+2) = z;
				*(output_buffer+((face_vertex_counter-1)*4)+3) = w; 	
			}
		}
	}
	
	vertex_data output_struct = { output_buffer, face_vertex_counter, };

	return output_struct; 
}

color_data randomize_color(const size_t faces) {
	size_t vertex_count = 0; 
	size_t required_size = faces * 3 * 4 * sizeof(float);
	printf("required size -> %d\n", required_size);
	float *color_buffer = (float *) malloc((size_t) required_size);

	srand(time(NULL)); // take random seed from time 	
	for (int face = 0; face < faces; face++) {

		float rgb[] = {
			(float) rand() / (float) RAND_MAX,
			(float) rand() / (float) RAND_MAX,
			(float) rand() / (float) RAND_MAX,
			1.0f, 
		};

		for (int vertex = 0; vertex < 3; vertex++) {
			vertex_count++; 
			for (int dimension = 0; dimension < 5; dimension++) {
				int location = (face * 3 * 4) + (vertex * 4) + dimension;
				*(color_buffer+location) = *(rgb+dimension);
			}
		}
	}

	color_data output_data = { color_buffer, vertex_count }; 
	return output_data;
}
