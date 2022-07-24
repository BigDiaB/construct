#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Construct
Construct is a library for runtime data types, bound together like a struct

The so called buffers are opaque to the end-user in addition to a lot of error-checks to make sure that this library is as memory safe as possible!
(The error checks must be enabled first by defining "ERROR_CHECKING" during compilation of the library)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* Typedef to a void-pointer in order to make the buffer data-type opaque
(Because I don't trust myself having access to the struct in my applications) 
 Don't define "CONSTRUCT_IMPLEMENTATION" in your projects, or it won't compile.
 This is only intended for the source of the implementation, hence the otherwise opaque data-type */
#ifndef CONSTRUCT_IMPLEMENTATION
typedef void* buffer;
#endif

/* Enum with the supported types ("VOID" actually means void pointer and can also be used for nested buffers) */
enum construct_types {UINT,INT,FLOAT,CHAR,UCHAR,VOID};

/* <Todo> */
void scramble_buffer(buffer target);
void scramble();

/* WIP: */
/* Returns a generic void pointer to the given field of the currently bound buffer */
/* Returns a generic void pointer to the given field of the given element of the specified buffer */

/* <\Todo> */

/* Flushes the type-stack */
void flush_types();
/* Pops the given number of types from the type-stack */
void pop_types(unsigned int num_types);
/* Pushes a type from the type-enum onto the type-stack */
void push_type(enum construct_types type);
/* Pushes the types of the specified buffer for use in another buffer onto the type-stack */
void repush_buffer_types(buffer target);
/* Pushes the types of the currently bound buffer for use in another buffer onto the type-stack */
void repush_types();
/* Iterates over all items of a buffer starting at index of the iterator */
unsigned int iterate_over(buffer target);

/* Returns an initialised buffer with the currently pushed types and the specified length (and clears the stack for the types) */
buffer init_buffer(unsigned int num_elements);
/* Returns an initialised buffer with the variadicly given types and the specified length (doesn't clear the stack for the types) */
buffer init_bufferva(unsigned int num_elements, unsigned int num_types, ...);
/* Returns an initialised buffer with the given types and the specified length (doesn't clear the stack for the types) */
buffer init_bufferve(unsigned int num_elements, unsigned int num_types, enum construct_types* types);
/* Deinitialises the specified buffer by freeing the internal variables */
void deinit_buffer(buffer target);
/* Binds the specified buffer at the specified index */
void bind_buffer_at(buffer target, unsigned int index);
/* Returns the currently bound buffer */
buffer get_current_buffer();

/* Sorts the specified buffer in ascending or descending order by the specified field */
void sort_buffer_by_field(buffer target,unsigned int less,unsigned int field, enum construct_types type);
/* Sorts the currently bound buffer in ascending or descending order by the specified field */
void sort_by_field(unsigned int more,unsigned int field, enum construct_types type);

/* Reverses the sequence of elements in the specified buffer */
void reverse_buffer(buffer target);
/* Reverses the sequence of elements in the currently bound buffer */
void reverse();

/* Resizes the specified buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize_buffer(buffer target, unsigned int num_elements);
/* Resizes the currently bound buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize(unsigned int num_elements);

/* Returns a buffer, initialised with a partition of the currently bound buffer within the given indices */
buffer copy_partial(unsigned int startidx, unsigned int endidx);
/* Returns a buffer, initialised with a partition of the specified buffer within the given indices */
buffer copy_partial_buffer(buffer target, unsigned int startidx, unsigned int endidx);
/* Returns a buffer with a single element laid out according to the specified buffer */
buffer create_single_buffer_element(buffer target);
/* Returns a buffer with a single element laid out according to the currently bound buffer */
buffer create_single_element();
/* Returns a buffer with the same type-layout as the specified buffer */
buffer recreate_buffer(buffer target);
/* Returns a buffer with the same type-layout as the currently bound buffer */
buffer recreate();
/* Returns a preinitialised copy of the specified buffer */
buffer copy_buffer(buffer src);
/* Copys the contents of the currenty bound buffer into the specified buffer */
void copy_to_buffer(buffer dest);
/* Copys the contents of the specified buffer into the currenty bound buffer */
void copy_from_buffer(buffer src);
/* Copys the contents of the specified buffer into another specified buffer */
void copy_buffer_to_buffer(buffer src,buffer dest);

/* Swaps the elements at the given indices in the currently bound buffer */
void swap_at(unsigned int idx1, unsigned int idx2);
/* Swaps the elements at the given indices in the specified buffer */
void swap_buffer_at(buffer target,unsigned int idx1, unsigned int idx2);
/* Swaps the elements of two specified buffers at the given indecies */
void swap_buffer_at_buffer(buffer src, unsigned int idxsrc, buffer dest, unsigned int idxdest);

/* Replaces the currently bound buffer at the given index, with the given element */
void replace_at(unsigned int index, buffer element);
/* Replaces the specified buffer at the given index, with the given element */
void replace_buffer_at(buffer target, unsigned int index, buffer element);
/* Replaces the specified buffer at the given index, with the element of another specified buffer at another given index */
void replace_buffer_at_buffer(buffer src, unsigned int idxsrc, buffer dest, unsigned int idxdest);

/* Replaces the element in specified buffer at the destination index with the element at the source index */
void replace_inside_buffer(buffer target, unsigned int idxsrc, unsigned int idxdest);
/* Replaces the element in the currently bound buffer at the destination index with the element at the source index */
void replace_inside(unsigned int idxsrc, unsigned int idxdest);

/* Removes an element from the specified buffer at the given index */
void remove_buffer_at(buffer target, unsigned int index);
/* Removes an element from the currently bound buffer at the given index */
void remove_at(unsigned int index);

/* Appends the contents of the specified buffer to the currently bound buffer */
void append_at(buffer src);
/* Appends the contents of the currently bound buffer to the specified buffer */
void append_to(buffer dest);
/* Appends the contents of the specified buffer to another specified buffer buffer */
void append_buffer_at(buffer src, buffer dest);

/* Appends one element at the given index of the specified buffer to the currently bound buffer */
void append_element_at(buffer src, unsigned int index);
/* Appends one element at the given index of the currently bound buffer to the specified buffer */
void append_element_to(buffer dest, unsigned int index);
/* Appends one element at the given index of the specified buffer to another specified buffer */
void append_buffer_element_at(buffer src, unsigned int index, buffer dest);

/* Returns an already malloc'ed pointer to a copy of the data buffer of the currently bound buffer and populates size with the length of the data buffer in bytes */
void* dump_binary(unsigned int* size);
/* Copies bin_data into the data buffer of the currently bound buffer and resizes it if the given size doesn't match the current size of the currently bound buffer */
void load_binary(void* bin_data, unsigned int size);
/* Returns an already malloc'ed pointer to a copy of the data buffer of the specified buffer and populates size with the length of the data buffer in bytes */
void* dump_buffer_binary(buffer target, unsigned int* size);
/* Copies bin_data into the data buffer of the specified buffer and resizes it if the given size doesn't match the current size of the specified buffer */
void load_buffer_binary(buffer target, void* bin_data, unsigned int size);

/* sets every single byte in the data-buffer of the currently bound buffer to zero */
void zero_out();
/* Returns the size of the currently bound buffer's data-buffer in bytes */
unsigned int get_size(buffer target);
/* Returns the raw data buffer of the currently bound buffer */
void* get_data_buffer();
/* Returns the iterator of the currently bound buffer */
unsigned int get_iterator();
/* Sets the iterator of the currently bound buffer to the specified value -1 because the iterator increments before binding */
void set_iterator(unsigned int iterator);
/* Returns the number of elements that was given when initialising the currently bound buffer */
unsigned int get_length();
/* Returns the number of bytes each element in the currently bound buffer takes */
unsigned int get_element_size();
/* Returns the offset in bytes of the element at the given index in the currently bound buffer */
unsigned int get_element_data_offset(unsigned int index);

/* sets every single byte in the data-buffer of the specified buffer to zero */
void zero_buffer_out(buffer target);
/* Returns the size of the specified buffer's data-buffer in bytes */
unsigned int get_buffer_size(buffer target);
/* Returns the raw data buffer of the specified buffer */
void* get_buffer_data_buffer(buffer target);
/* Returns the iterator of the specified buffer */
unsigned int get_buffer_iterator(buffer target);
/* Sets the iterator of the specified buffer to the specified value -1 because the iterator increments before binding */
void set_buffer_iterator(buffer target, unsigned int iterator);
/* Returns the number of elements that was given when initialising the specified buffer */
unsigned int get_buffer_length(buffer target);
/* Returns the number of bytes each element in the specified buffer takes */
unsigned int get_buffer_element_size(buffer target);
/* Returns the offset in bytes of the element at the given index in the specified buffer */
unsigned int get_buffer_element_data_offset(buffer target, unsigned int index);

/* Assigns the given field of the currently bound buffer to the specified data */
void set_fieldui(unsigned int field, 	unsigned int 	data);
void set_fieldi(unsigned int field,  	int 			data);
void set_fieldf(unsigned int field,  	float 			data);
void set_fieldc(unsigned int field,  	char 			data);
void set_fielduc(unsigned int field, 	unsigned char 	data);
void set_fieldv(unsigned int field,		void* 			data);
/* Returns the given field of the currently bound buffer */
unsigned int	get_fieldui(unsigned int field);
int 			get_fieldi(unsigned int field);
float 			get_fieldf(unsigned int field);
char 			get_fieldc(unsigned int field);
unsigned char 	get_fielduc(unsigned int field);
void* 			get_fieldv(unsigned int field);
/* Returns a pointer to the given field of the currently bound buffer (If a buffer gets resized, it invalidates all previously obtained pointers to it!) */
void*           get_pointer(unsigned int field);
unsigned int* 	get_pointerui(unsigned int field);
int* 			get_pointeri(unsigned int field);
float* 			get_pointerf(unsigned int field);
char* 			get_pointerc(unsigned int field);
unsigned char* 	get_pointeruc(unsigned int field);
void**          get_pointerv(unsigned int field);

/* Assigns the given field of the specified buffer to the specified data */
void set_buffer_fieldui(buffer target, unsigned int element, unsigned int field,	unsigned int 	data);
void set_buffer_fieldi(buffer target, unsigned int element, unsigned int field,		int 			data);
void set_buffer_fieldf(buffer target, unsigned int element, unsigned int field,		float 			data);
void set_buffer_fieldc(buffer target, unsigned int element, unsigned int field,		char 			data);
void set_buffer_fielduc(buffer target, unsigned int element, unsigned int field,	unsigned char 	data);
void set_buffer_fieldv(buffer target, unsigned int element, unsigned int field,		void* 			data);
/* Returns the given field of the specified buffer */
unsigned int	get_buffer_fieldui(buffer target, unsigned int element, unsigned int field);
int 			get_buffer_fieldi(buffer target, unsigned int element, unsigned int field);
float 			get_buffer_fieldf(buffer target, unsigned int element, unsigned int field);
char 			get_buffer_fieldc(buffer target, unsigned int element, unsigned int field);
unsigned char 	get_buffer_fielduc(buffer target, unsigned int element, unsigned int field);
void* 			get_buffer_fieldv(buffer target, unsigned int element, unsigned int field);
/* Returns a pointer to the given field of the given element of the specified buffer (If a buffer gets resized, it invalidates all previously obtained pointers to it!) */
void*           get_buffer_pointer(buffer target, unsigned int element, unsigned int field);
unsigned int* 	get_buffer_pointerui(buffer target, unsigned int element, unsigned int field);
int* 			get_buffer_pointeri(buffer target, unsigned int element, unsigned int field);
float* 			get_buffer_pointerf(buffer target, unsigned int element, unsigned int field);
char* 			get_buffer_pointerc(buffer target, unsigned int element, unsigned int field);
unsigned char* 	get_buffer_pointeruc(buffer target, unsigned int element, unsigned int field);
void**          get_buffer_pointerv(buffer target, unsigned int element, unsigned int field);

#ifdef __cplusplus
}
#endif

#endif /* CONSTRUCT_H */
