#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Construct
Construct is a library for runtime data types, bound together like a struct

The so called buffers are opaque to the end-user in addition to a lot of error-checks to make sure that this library is as memory safe as possible!
(The error checks must be enabled first by defining "ERROR_CHECKING" during compilation of the library)
*/
	
/* Unsigned integer- and byte-type I use for basically everything */
#include <inttypes.h>
typedef uint32_t uint;
typedef uint8_t byte;

/* Typedef to a void-pointer in order to make the buffer data-type opaque
(Because I don't trust myself having access to the struct in my applications) */
typedef void* buffer;

/* Enum with the supported types ("VOID" actually means void pointer and can also be used for nested buffers) */
enum type {UINT,INT,FLOAT,CHAR,UCHAR,VOID};

/* Pushes a type from the type-enum onto an internal stack */
void push_type(enum type type);
/* Pushes the types of the specified buffer for use in another buffer onto an internal stack */
void repush_buffer_types(buffer target);
/* Pushes the types of the currently bound buffer for use in another buffer onto an internal stack */
void repush_types();
/* Iterates over all items of a buffer starting at index of the iterator */
uint iterate_over(buffer target);

/* Returns an initialised buffer with the currently pushed types and the specified length (and clears the stack for the types) */
buffer init_buffer(uint num_elements);
/* Deinitialises the specified buffer by freeing the internal variables */
void deinit_buffer(buffer target);
/* Binds the specified buffer at the specified index */
void bind_buffer_at(buffer target, uint index);

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

/* Replaces the currently bound buffer at the given index, with the given element */
void replace_at(uint index, buffer element);
/* Removes an element from the currently bound buffer at the given index */
void remove_at(uint index);
/* Resizes the currently bound buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize(uint num_elements);

/* Replaces the specified buffer at the given index, with the given element */
void replace_buffer_at(buffer target, uint index, buffer element);
/* Removes an element from the specified buffer at the given index */
void remove_buffer_at(buffer target, uint index);
/* Resizes the specified buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize_buffer(buffer target, uint num_elements);

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

/* Returns a buffer with a single element laid out according to the currently bound buffer */
buffer create_single_element();
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

/* Returns a buffer with a single element laid out according to the specified buffer */
buffer create_single_buffer_element(buffer target);
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

/* Set-functions for the different supported types for bound buffer elements */
void set_fieldui(uint field, 	uint 			data);
void set_fieldi(uint field,  	int 			data);
void set_fieldf(uint field,  	float 			data);
void set_fieldc(uint field,  	char 			data);
void set_fielduc(uint field, 	unsigned char 	data);
void set_fieldv(uint field,		void* 			data);
/* Get-functions for the different supported types for bound buffer elements */
uint			get_fieldui(uint field);
int 			get_fieldi(uint field);
float 			get_fieldf(uint field);
char 			get_fieldc(uint field);
unsigned char 	get_fielduc(uint field);
void* 			get_fieldv(uint field);

/* Set-functions for the different supported types for elements in specified buffers */
void set_buffer_fieldui(buffer target, uint element, uint field,	uint 			data);
void set_buffer_fieldi(buffer target, uint element, uint field,		int 			data);
void set_buffer_fieldf(buffer target, uint element, uint field,		float 			data);
void set_buffer_fieldc(buffer target, uint element, uint field,		char 			data);
void set_buffer_fielduc(buffer target, uint element, uint field,	unsigned char 	data);
void set_buffer_fieldv(buffer target, uint element, uint field,		void* 			data);
/* Get-functions for the different supported types for elements in specified buffers */
uint			get_buffer_fieldui(buffer target, uint element, uint field);
int 			get_buffer_fieldi(buffer target, uint element, uint field);
float 			get_buffer_fieldf(buffer target, uint element, uint field);
char 			get_buffer_fieldc(buffer target, uint element, uint field);
unsigned char 	get_buffer_fielduc(buffer target, uint element, uint field);
void* 			get_buffer_fieldv(buffer target, uint element, uint field);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONSTRUCT_H */
