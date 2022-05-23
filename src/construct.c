#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#ifndef uint
#define uint uint32_t
#endif

#ifndef byte
#define byte uint8_t
#endif

enum type
{
    UINT,INT,FLOAT,CHAR,UCHAR,VOID
};

enum ERRORS
{
    ERROR_INVALID_FIELD = 100,
    ERROR_NO_PUSHED_TYPES,
    ERROR_INVALID_DATA,
    ERROR_BAD_BUFFER,
    ERROR_INVALID_TYPE,
    ERROR_OUT_OF_BOUNDS_INDEX
};

struct buffer
{
    uint iterator,num_types,num_elements;
    void* data_buffer;
    enum type* types;
};

typedef struct buffer* buffer;

#define cast_to(type) *(type*)

buffer current_buffer = NULL;
void* current_data = NULL;

enum type* current_types = NULL;
uint current_num_types = 0;

uint sizes[6] = {sizeof(uint),sizeof(int),sizeof(float),sizeof(char),sizeof(unsigned char),sizeof(void*)};

void test_error(bool expression, uint error, const char* function)
{
    if (!expression)
    {
        printf("%s errored with code %u: ",function,error);
        switch(error)
        {
            case ERROR_INVALID_FIELD:
                puts("ERROR_INVALID_FIELD");
                break;
            case ERROR_NO_PUSHED_TYPES:
                puts("ERROR_NO_PUSHED_TYPES");
                break;
            case ERROR_INVALID_DATA:
                puts("ERROR_INVALID_DATA");
                break;
            case ERROR_BAD_BUFFER:
                puts("ERROR_BAD_BUFFER");
                break;
            case ERROR_INVALID_TYPE:
                puts("ERROR_INVALID_TYPE");
                break;
            case ERROR_OUT_OF_BOUNDS_INDEX:
                puts("ERROR_OUT_OF_BOUNDS_INDEX");
            default:
                break;
        }
        exit(error);
    }
}

#define test_error(expression,error)     test_error(expression,error,__FUNCTION__);

uint util_get_size(buffer buffer)
{
    uint i, size = 0;
    for (i = 0; i < buffer->num_types; i++)
        size += sizes[buffer->types[i]];
    return size;
}

void util_swap(void* src1, void* src2, uint size)
{
    byte temp[size];
    memcpy(temp,src2,size);
    memcpy(src2, src1,size);
    memcpy(src1, temp,size);
}

void util_copy(void* src, void* dest, uint size)
{
    memcpy(dest,src,size);
}

void push_type(enum type t)
{
    if (current_types == NULL)
    {
        current_types = malloc(0);
        current_num_types = 0;
    }
    current_types = realloc(current_types,(current_num_types + 1) * sizeof(enum type));
    current_types[current_num_types] = t;
    current_num_types++;
}


void bind_buffer_at(buffer buffer, uint index)
{
#ifdef ERROR_CHECKING
	test_error(buffer != NULL,ERROR_BAD_BUFFER);
#endif
	current_buffer = buffer;
    current_buffer->iterator = index;
}

buffer init_buffer(uint num_elements)
{
    buffer buffer;
    buffer = malloc(sizeof(struct buffer));
    buffer->num_types = current_num_types;

    buffer->iterator = -1;

    #ifdef ERROR_CHECKING
    test_error(current_types != NULL,ERROR_NO_PUSHED_TYPES);
    #endif

    buffer->types = current_types;

    
    uint size = util_get_size(buffer);

    buffer->data_buffer = malloc(num_elements * size);
    buffer->num_elements = num_elements;

    current_types = NULL;
    return buffer;
}

void deinit_buffer(buffer buffer)
{
    free(buffer->data_buffer);
    free(buffer->types);
    free(buffer);
}

void* get_data_buffer()
{
    return current_buffer->data_buffer;
}

uint iterate_over(buffer buffer)
{   
    if (buffer->iterator == buffer->num_elements - 1)
    {
        buffer->iterator = -1;
        return 0;
    }
    else
    {
        buffer->iterator++;
		bind_buffer_at(buffer,buffer->iterator);
        return 1;
    }
}

void swap_at(uint idx1, uint idx2)
{
    test_error(idx1 < current_buffer->num_elements && idx2 < current_buffer->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);

    uint size = util_get_size(current_buffer);

    util_swap(current_buffer->data_buffer + size * idx1,current_buffer->data_buffer + size * idx2,size);
}

void insert_at(uint index, void* data)
{
    uint i,size = util_get_size(current_buffer);
    current_buffer->num_elements++;
    current_buffer->data_buffer = realloc(current_buffer->data_buffer,current_buffer->num_elements * size);

    for (i = current_buffer->num_elements - 2; i > index; i--)
        util_swap(current_buffer->data_buffer + size * (i + 1), current_buffer->data_buffer + size * i, size);

    util_copy(data,current_buffer->data_buffer + size * index,size);
}

void remove_at(uint index)
{
    uint i,size = util_get_size(current_buffer);
    current_buffer->num_elements--;
    for (i = index; i < current_buffer->num_elements; i++)
        util_copy(current_buffer->data_buffer + size * (i + 1),current_buffer->data_buffer + size * i, size);
    current_buffer->data_buffer = realloc(current_buffer->data_buffer,current_buffer->num_elements * size);
}

void resize(uint num_elements)
{
    uint size = util_get_size(current_buffer);
    current_buffer->data_buffer = realloc(current_buffer->data_buffer,num_elements * size);
}

uint get_element_size()
{
    uint size = util_get_size(current_buffer);
    return size;
}

void set_iterator(uint iterator)
{
    current_buffer->iterator = iterator - 1;
}

uint get_iterator()
{
    return current_buffer->iterator;
}

uint get_length()
{
    return current_buffer->num_elements;
}

void* get_field(uint field)
{
    uint i,off = 0,size = util_get_size(current_buffer);
    for (i = 0; i < field; i++)
        off += sizes[current_buffer->types[i]];

    return current_buffer->data_buffer + off + size * current_buffer->iterator;
}


uint get_fieldui(uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UINT == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(uint)get_field(field);
}
int get_fieldi(uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(INT == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_field(field);
}
float get_fieldf(uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(FLOAT == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif


    return cast_to(float)get_field(field);
}
char get_fieldc(uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(CHAR == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif


    return cast_to(char)get_field(field);
}
unsigned char get_fielduc(uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UCHAR == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_field(field);
}

void* get_fieldv(uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(VOID == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_field(field);
}

void set_field(uint field, void* data)
{
    #ifdef ERROR_CHECKING
    test_error(data != NULL,ERROR_INVALID_DATA);
    #endif
    memcpy(get_field(field),data,sizes[current_buffer->types[field]]);
}

void set_fieldui(uint field, uint data)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UINT == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldi(uint field, int data)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(INT == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldf(uint field, float data)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(FLOAT == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif
    set_field(field,&data);
}

void set_fieldc(uint field, char data)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(CHAR == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fielduc(uint field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UCHAR == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldv(uint field, void* data)
{
    #ifdef ERROR_CHECKING
    test_error(field < current_buffer->num_types,ERROR_INVALID_FIELD);
    test_error(VOID == current_buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}


void* get_buffer_data_buffer(buffer buffer)
{
    return buffer->data_buffer;
}

void swap_buffer_at(buffer buffer, uint idx1, uint idx2)
{
    test_error(idx1 < buffer->num_elements && idx2 < buffer->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);

    uint size = util_get_size(buffer);

    util_swap(buffer->data_buffer + size * idx1,buffer->data_buffer + size * idx2,size);
}

void insert_buffer_at(buffer buffer, uint index, void* data)
{
    uint i,size = util_get_size(buffer);
    buffer->num_elements++;
    buffer->data_buffer = realloc(buffer->data_buffer,buffer->num_elements * size);

    for (i = buffer->num_elements - 2; i > index; i--)
        util_swap(buffer->data_buffer + size * (i + 1), buffer->data_buffer + size * i, size);

    util_copy(data,buffer->data_buffer + size * index,size);
}

void remove_buffer_at(buffer buffer,uint index)
{
    uint i,size = util_get_size(buffer);
    buffer->num_elements--;
    for (i = index; i < buffer->num_elements; i++)
        util_copy(buffer->data_buffer + size * (i + 1),buffer->data_buffer + size * i, size);
    buffer->data_buffer = realloc(buffer->data_buffer,buffer->num_elements * size);
}

void resize_buffer(buffer buffer, uint num_elements)
{
    uint size = util_get_size(buffer);
    buffer->data_buffer = realloc(buffer->data_buffer,num_elements * size);
}

uint get_buffer_element_size(buffer buffer)
{
    uint size = util_get_size(buffer);
    return size;
}

void set_buffer_iterator(buffer buffer,uint iterator)
{
    buffer->iterator = iterator - 1;
}

uint get_buffer_iterator(buffer buffer)
{
    return buffer->iterator;
}

uint get_buffer_length(buffer buffer)
{
    return buffer->num_elements;
}

void* get_buffer_field(buffer buffer, uint element, uint field)
{
    uint i,off = 0,size = util_get_size(buffer);
    for (i = 0; i < field; i++)
        off += sizes[buffer->types[i]];

    return buffer->data_buffer + off + size * element;
}


uint get_buffer_fieldui(buffer buffer, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UINT == buffer->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(uint)get_buffer_field(buffer,element,field);
}
int get_buffer_fieldi(buffer buffer, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(INT == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_buffer_field(buffer,element,field);
}
float get_buffer_fieldf(buffer buffer, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(FLOAT == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(float)get_buffer_field(buffer,element,field);
}
char get_buffer_fieldc(buffer buffer, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(CHAR == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(char)get_buffer_field(buffer,element,field);
}
unsigned char get_buffer_fielduc(buffer buffer, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UCHAR == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_buffer_field(buffer,element,field);
}

void* get_buffer_fieldv(buffer buffer, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(VOID == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_buffer_field(buffer,element,field);
}

void set_buffer_field(buffer buffer, uint element, uint field, void* data)
{
    #ifdef ERROR_CHECKING
    test_error(data != NULL,ERROR_INVALID_DATA);
    #endif

    memcpy(get_buffer_field(buffer,element,field),data,sizes[buffer->types[field]]);
}

void set_buffer_fieldui(buffer buffer, uint element, uint field, uint data)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UINT == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(buffer,element, field,&data);
}

void set_buffer_fieldi(buffer buffer, uint element, uint field, int data)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(INT == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(buffer,element, field,&data);
}

void set_buffer_fieldf(buffer buffer, uint element, uint field, float data)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(FLOAT == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(buffer,element, field,&data);
}

void set_buffer_fieldc(buffer buffer, uint element, uint field, char data)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(CHAR == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(buffer,element, field,&data);
}

void set_buffer_fielduc(buffer buffer, uint element, uint field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(UCHAR == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(buffer,element, field,&data);
}

void set_buffer_fieldv(buffer buffer, uint element, uint field, void* data)
{
    #ifdef ERROR_CHECKING
    test_error(field < buffer->num_types,ERROR_INVALID_FIELD);
    test_error(VOID == buffer->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(buffer,element, field,&data);
}