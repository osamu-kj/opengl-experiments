#ifndef GENERAL_FUNCTIONS_H
#define GENERAL_FUNCTIONS_H

#include "data-structures.h"

/**
Split the string into substrings. 

<Longer description>
Given a string (char array) and a delimiter, split it into multiple
substrings (char arrays) and return a structure containing the substrings.

@param data: pointer to the input string
@param size: size of the input string 
@param delimiter: char that will serve as a delimiter for the split operation 
@param free_data: if 1 the function will free the memory for data at the end of the function, 0 for not

@return: structure containing the number of substrings, and a pointer to the array of substrings
*/
splitted_strings_arr split(char *data, size_t size, char delimiter, int free_data);

/**
Size of a file. 

<Longer description>
Return the size of a file located on the computer's filesystem (number of bytes - characters)

@param path: path to the file 

@return: number of bytes inside the file 
*/
long int sizeof_file(const char *path); 

/**
Read contents of a file. 

<Warning>
The function assumes the required memory is already allocated to where it will write the contents.

<Longer description>
Write contents of a file into a memory location.

@param path: path to the file 
@param file_contents: pointer to a memory location where the function will write the file contents.
*/
void read_file(const char *path, char *file_contents); 

/**
Load shader into the program.

<Long description>
The function will write the given shader file, compile the shader, check it, attach it,
link it to the specified program and free/remove all of the junk at the end of the function.

@param program: program to link the shader to 
@param path: path to the shader file 
@param type: shader type
*/
void load_shader(GLuint program, const char *path, GLenum type); 

/**
Load model into the program. 

<Supported file formats>
.obj

<Long description>
Read vertex data from a given file and write it to an output buffer
and return the output_buffer's location (inside a struct) alongside the number
of vertecies found.

@param path: path to the model file 

@return: structure containing a pointer to the buffer, and the number of vertecies inside the buffer (can be used to calculate the size - vertecies * 4 * sizeof(float))
*/
vertex_data load_model(const char *path); 

/**
Randomize color for each face

<Long description>
Given a number of faces, the function will generate a buffer of floats for each 
face having its own randomly generated color. The number of vertecies is calculated 
with the following formula -> faces * 3 (assuming a face is a triangle therefore has 
3 vertecies), size of the bufer -> faces * 3 * 3 * sizeof(float) (assuming a vertex 
has 3 dimensions - rgb and each is a float)

@param faces: number of faces 

@return: structure containing a pointer to the generated color buffer, and the number of vertecies inside the buffer (can be used to calculate the size - vertecies * 4 * sizeof(float))
*/
color_data randomize_color(const size_t faces);

/**
Load data into gpu memory

<Long description>
Copy data from the given memory location into the gpu memory, 
making it available for drawing operations etc

@param buffer: the buffer into which the data should get stored
@param target: target to which the buffer object will be bound during the operation 
@param size: number of bytes the function should copy from the given memory address
@param data: pointer to the memory location 
@param usage: expected usage pattern of the data
*/
void load_gpu_data(GLuint buffer, GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage); 

float* generate_instancing_offsets(int count); 

int collision(float* obj_one_collision_bound, float* obj_two_collision_bound, float* obj_one_offset, float* obj_two_offset);

int touching_borders(float* obj_collision_bound, float* obj_offset, float x_limit, float y_limit); 

#endif
