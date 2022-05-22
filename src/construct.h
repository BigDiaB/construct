#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Construct
Construct is a library for runtime data types, bound together like a struct

The so called buffers are opaque to the end-user in addition to a lot of error-checks to make sure that this library is as memory safe as possible!
(The error checks must be enabled first by defining "ERROR_CHECKING" during compilation)
*/
	
/* Unsigned integer type I use for basically everything */
#include <inttypes.h>
typedef uint32_t uint;

/* Typedef to a void-pointer in order to make the buffer data-type opaque
(Because I don't trust myself having access to the struct) */
typedef void* buffer;

/* Enum with the supported types (and #define to add "BUFFER" as a type for code clarity) */
#define BUFFER VOID
enum type {UINT,INT,FLOAT,CHAR,UCHAR,VOID /* ("VOID" actually means void pointer) */};

/* Pushes a type from the type-enum onto an internal stack */
void push_type(enum type type);
/* Iterates over all items of a buffer starting at index zero */
uint iterate_over(buffer buffer);

/* Returns an initialised buffer with the currently pushed types and the specified length (and clears the stack for the types) */
buffer init_buffer(uint num_elements);
/* Deinitialises the specified buffer by freeing the internal variables */
void deinit_buffer(buffer buffer);
/* Binds the specified buffer at the specified index */
void bind_buffer_at(buffer buffer, uint index);


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
/* Swaps the elements at the given indices in the currently bound buffer */
void swap_at(uint idx1, uint idx2);
/* Inserts the contents of data, which are expected to be the size of an element, into the currently bound buffer at a given index */
void insert_at(uint index, void* data);
/* Removes an element from the currently bound buffer at the given index */
void remove_at(uint index);
/* Resizes the currently bound buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize(uint num_elements);


/* Returns the raw data buffer of the specified buffer */
void* get_buffer_data_buffer(buffer buffer);
/* Returns the iterator of the specified buffer */
uint get_buffer_iterator(buffer buffer);
/* Sets the iterator of the specified buffer to the specified value -1 because the iterator increments before binding */
void set_buffer_iterator(buffer buffer, uint iterator);
/* Returns the number of elements that was given when initialising the specified buffer */
uint get_buffer_length(buffer buffer);
/* Returns the number of bytes each element in the specified buffer takes */
uint get_buffer_element_size(buffer buffer);
/* Swaps the elements at the given indices in the specified buffer */
void swap_buffer_at(buffer buffer,uint idx1, uint idx2);
/* Inserts the contents of data, which are expected to be the size of an element, into the specified buffer at the given index */
void insert_buffer_at(buffer buffer, uint index, void* data);
/* Removes an element from the specified buffer at the given index */
void remove_buffer_at(buffer buffer, uint index);
/* Resizes the specified buffer to have the given number of elements. When shrinking the buffer, the last elements will be removed, when enlarging, the new elements won't be initialised */
void resize_buffer(buffer buffer, uint num_elements);

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
void set_buffer_fieldui(buffer buffer, uint element, uint field,	uint 			data);
void set_buffer_fieldi(buffer buffer, uint element, uint field,		int 			data);
void set_buffer_fieldf(buffer buffer, uint element, uint field,		float 			data);
void set_buffer_fieldc(buffer buffer, uint element, uint field,		char 			data);
void set_buffer_fielduc(buffer buffer, uint element, uint field,	unsigned char 	data);
void set_buffer_fieldv(buffer buffer, uint element, uint field,		void* 			data);
/* Get-functions for the different supported types for elements in specified buffers */
uint			get_buffer_fieldui(buffer buffer, uint element, uint field);
int 			get_buffer_fieldi(buffer buffer, uint element, uint field);
float 			get_buffer_fieldf(buffer buffer, uint element, uint field);
char 			get_buffer_fieldc(buffer buffer, uint element, uint field);
unsigned char 	get_buffer_fielduc(buffer buffer, uint element, uint field);
void* 			get_buffer_fieldv(buffer buffer, uint element, uint field);


#ifdef __cplusplus
}
#endif

#endif /* CONSTRUCT_H */
