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
#include <string.h>
	
/* Unsigned integer-type I use for basically everything */
#include <inttypes.h>
typedef uint32_t uint;

/* Typedef to a void-pointer in order to make the buffer data-type opaque
(Because I don't trust myself having access to the struct in my applications) */
typedef void* buffer;

/* Enum with the supported types ("VOID" actually means void pointer and can also be used for nested buffers) */
#define BUFFER VOID
enum construct_types {UINT,INT,FLOAT,CHAR,UCHAR,VOID};

/* Flushes the type-stack */
void flush_types();
/* Pops the given number of types from the type-stack */
void pop_types(uint num_types);
/* Pushes a type from the type-enum onto the type-stack */
void push_type(enum construct_types type);
/* Pushes the types of the specified buffer for use in another buffer onto the type-stack */
void repush_buffer_types(buffer target);
/* Pushes the types of the currently bound buffer for use in another buffer onto the type-stack */
void repush_types();
/* Iterates over all items of a buffer starting at index of the iterator */
uint iterate_over(buffer target);

/* Returns an initialised buffer with the currently pushed types and the specified length (and clears the stack for the types) */
buffer init_buffer(uint num_elements);
/* Deinitialises the specified buffer by freeing the internal variables */
void deinit_buffer(buffer target);
/* Binds the specified buffer at the specified index */
void bind_buffer_at(buffer target, uint index);

/* Returns a buffer with a single element laid out according to the specified buffer */
buffer create_single_buffer_element(buffer target);
/* Returns a buffer with a single element laid out according to the currently bound buffer */
buffer create_single_element();
/* Returns a preinitialised copy of the specified buffer */
buffer copy_buffer(buffer src);
/* Copys the contents of the currenty bound buffer into the specified buffer */
void copy_to_buffer(buffer dest);
/* Copys the contents of the specified buffer into the currenty bound buffer */
void copy_from_buffer(buffer src);
/* Copys the contents of the specified buffer into another specified buffer */
void copy_buffer_to_buffer(buffer src,buffer dest);

/* Swaps the elements at the given indices in the currently bound buffer */
void swap_at(uint idx1, uint idx2);
/* Swaps the elements at the given indices in the specified buffer */
void swap_buffer_at(buffer target,uint idx1, uint idx2);
/* Swaps the elements of two specified buffers at the given indecies */
void swap_buffer_at_buffer(buffer src, uint idxsrc, buffer dest, uint idxdest);
/* Replaces the specified buffer at the given index, with the element of another specified buffer at another given index */
void replace_buffer_at_buffer(buffer src, uint idxsrc, buffer dest, uint idxdest);

/* Returns an already malloc'ed pointer to a copy of the data buffer of the currently bound buffer and populates size with the length of the data buffer in bytes */
void* dump_binary(uint* size);
/* Copies bin_data into the data buffer of the currently bound buffer and resizes it if the given size doesn't match the current size of the currently bound buffer */
void load_binary(void* bin_data, uint size);
/* Returns an already malloc'ed pointer to a copy of the data buffer of the specified buffer and populates size with the length of the data buffer in bytes */
void* dump_buffer_binary(buffer target, uint* size);
/* Copies bin_data into the data buffer of the specified buffer and resizes it if the given size doesn't match the current size of the specified buffer */
void load_buffer_binary(buffer target, void* bin_data, uint size);

/* Replaces the specified buffer at the given index, with the given element */
void replace_buffer_at(buffer target, uint index, buffer element);
/* Replaces the currently bound buffer at the given index, with the given element */
void replace_at(uint index, buffer element);

/* Resizes the specified buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize_buffer(buffer target, uint num_elements);
/* Resizes the currently bound buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize(uint num_elements);

/* Removes an element from the specified buffer at the given index */
void remove_buffer_at(buffer target, uint index);
/* Removes an element from the currently bound buffer at the given index */
void remove_at(uint index);

/* Appends the contents of the specified buffer to the currently bound buffer */
void append_at(buffer src);
/* Appends the contents of the currently bound buffer to the specified buffer */
void append_to(buffer dest);
/* Appends the contents of the specified buffer to another specified buffer buffer */
void append_buffer_at(buffer src, buffer dest);

/* Appends one element at the given index of the specified buffer to the currently bound buffer */
void append_element_at(buffer src, uint index);
/* Appends one element at the given index of the currently bound buffer to the specified buffer */
void append_element_to(buffer dest, uint index);
/* Appends one element at the given index of the specified buffer to another specified buffer buffer */
void append_buffer_element_at(buffer src, uint index, buffer dest);

/* sets every single byte in the data-buffer of the currently bound buffer to zero */
void zero_out();
/* Returns the size of the currently bound buffer's data-buffer in bytes */
uint get_size(buffer target);
/* Returns the raw data buffer of the currently bound buffer */
void* get_data_buffer();
/* Returns the iterator of the currently bound buffer */
uint get_iterator();
/* Sets the iterator of the currently bound buffer to the specified value -1 because the iterator increments before binding */
void set_iterator(uint iterator);
/* Returns the number of elements that was given when initialising the currently bound buffer */
uint get_length();
/* Returns the number of bytes each element in the currently bound buffer takes */
uint get_element_size();
/* Returns the offset in bytes of the element at the given index in the currently bound buffer */
uint get_element_data_offset(uint index);

/* sets every single byte in the data-buffer of the specified buffer to zero */
void zero_buffer_out(buffer target);
/* Returns the size of the specified buffer's data-buffer in bytes */
uint get_buffer_size(buffer target);
/* Returns the raw data buffer of the specified buffer */
void* get_buffer_data_buffer(buffer target);
/* Returns the iterator of the specified buffer */
uint get_buffer_iterator(buffer target);
/* Sets the iterator of the specified buffer to the specified value -1 because the iterator increments before binding */
void set_buffer_iterator(buffer target, uint iterator);
/* Returns the number of elements that was given when initialising the specified buffer */
uint get_buffer_length(buffer target);
/* Returns the number of bytes each element in the specified buffer takes */
uint get_buffer_element_size(buffer target);
/* Returns the offset in bytes of the element at the given index in the specified buffer */
uint get_buffer_element_data_offset(buffer target, uint index);

/* Assigns the given field of the currently bound buffer to the specified data */
void set_fieldui(uint field, 	uint 			data);
void set_fieldi(uint field,  	int 			data);
void set_fieldf(uint field,  	float 			data);
void set_fieldc(uint field,  	char 			data);
void set_fielduc(uint field, 	unsigned char 	data);
void set_fieldv(uint field,		void* 			data);
/* Returns the given field of the currently bound buffer */
uint			get_fieldui(uint field);
int 			get_fieldi(uint field);
float 			get_fieldf(uint field);
char 			get_fieldc(uint field);
unsigned char 	get_fielduc(uint field);
void* 			get_fieldv(uint field);

/* Assigns the given field of the specified buffer to the specified data */
void set_buffer_fieldui(buffer target, uint element, uint field,	uint 			data);
void set_buffer_fieldi(buffer target, uint element, uint field,		int 			data);
void set_buffer_fieldf(buffer target, uint element, uint field,		float 			data);
void set_buffer_fieldc(buffer target, uint element, uint field,		char 			data);
void set_buffer_fielduc(buffer target, uint element, uint field,	unsigned char 	data);
void set_buffer_fieldv(buffer target, uint element, uint field,		void* 			data);
/* Returns the given field of the specified buffer */
uint			get_buffer_fieldui(buffer target, uint element, uint field);
int 			get_buffer_fieldi(buffer target, uint element, uint field);
float 			get_buffer_fieldf(buffer target, uint element, uint field);
char 			get_buffer_fieldc(buffer target, uint element, uint field);
unsigned char 	get_buffer_fielduc(buffer target, uint element, uint field);
void* 			get_buffer_fieldv(buffer target, uint element, uint field);

#ifdef __cplusplus
}
#endif

#endif /* CONSTRUCT_H */
