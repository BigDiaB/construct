#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
typedef uint32_t uint;
typedef uint8_t byte;

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
    ERROR_OUT_OF_BOUNDS_INDEX,
    ERROR_OUT_OF_BOUNDS_ELEMENT,
    ERROR_UNEQUAL_ELEMENT_SIZE,
    ERROR_SMALL_DEST_BUFFER
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
enum type* current_types = NULL;
uint current_num_types = 0;

const uint sizes[6] = {sizeof(uint),sizeof(int),sizeof(float),sizeof(char),sizeof(unsigned char),sizeof(void*)};

void test_error(int expression, uint error, const char* function)
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
                break;
            case ERROR_OUT_OF_BOUNDS_ELEMENT:
                puts("ERROR_OUT_OF_BOUNDS_ELEMENT");
                break;
            case ERROR_UNEQUAL_ELEMENT_SIZE:
                puts("ERROR_UNEQUAL_ELEMENT_SIZE");
                break;
            case ERROR_SMALL_DEST_BUFFER:
                puts("ERROR_SMALL_DEST_BUFFER");
                break;
            default:
                break;
        }
        exit(error);
    }
}

#define test_error(expression,error)     test_error(expression,error,__FUNCTION__);

uint util_get_size(buffer target)
{
    uint i, size = 0;
    for (i = 0; i < target->num_types; i++)
        size += sizes[target->types[i]];
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


void bind_buffer_at(buffer target, uint index)
{
#ifdef ERROR_CHECKING
	test_error(target != NULL,ERROR_BAD_BUFFER);
#endif
	current_buffer = target;
    current_buffer->iterator = index;
}

buffer init_buffer(uint num_elements)
{
    buffer target;
    target = malloc(sizeof(struct buffer));
    target->num_types = current_num_types;

    target->iterator = -1;

    #ifdef ERROR_CHECKING
    test_error(current_types != NULL,ERROR_NO_PUSHED_TYPES);
    #endif

    target->types = current_types;

    
    uint size = util_get_size(target);

    target->data_buffer = malloc(num_elements * size);
    target->num_elements = num_elements;

    current_types = NULL;
    return target;
}

void deinit_buffer(buffer target)
{
    free(target->data_buffer);
    free(target->types);
    free(target);
}

void* get_data_buffer()
{
    return current_buffer->data_buffer;
}

uint iterate_over(buffer target)
{   
    if (target->iterator == target->num_elements - 1)
    {
        target->iterator = 0;
        return 0;
    }
    else
    {
        target->iterator++;
		bind_buffer_at(target,target->iterator);
        return 1;
    }
}

void swap_at(uint idx1, uint idx2)
{
    #ifdef ERROR_CHECKING
    test_error(idx1 < current_buffer->num_elements && idx2 < current_buffer->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif
    uint size = util_get_size(current_buffer);

    util_swap(current_buffer->data_buffer + size * idx1,current_buffer->data_buffer + size * idx2,size);
}

void replace_at(uint index, buffer data)
{
    uint size = util_get_size(current_buffer);
    util_copy(data->data_buffer,current_buffer->data_buffer + size * index,size);
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
    current_buffer->num_elements = num_elements;
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

uint get_element_data_offset(uint index)
{
    return util_get_size(current_buffer) * index;
}

uint get_buffer_element_data_offset(buffer target, uint index)
{
    return util_get_size(target) * index;
}

void* get_buffer_data_buffer(buffer target)
{
    return target->data_buffer;
}

void swap_buffer_at(buffer target, uint idx1, uint idx2)
{
    #ifdef ERROR_CHECKING
    test_error(idx1 < target->num_elements && idx2 < target->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif
    uint size = util_get_size(target);

    util_swap(target->data_buffer + get_buffer_element_data_offset(target,idx1),target->data_buffer + get_buffer_element_data_offset(target,idx2),size);
}

void swap_buffer_at_buffer(buffer src, uint idxsrc, buffer dest, uint idxdest)
{
    #ifdef ERROR_CHECKING
    test_error(idxsrc < src->num_elements && idxdest < dest->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    test_error(util_get_size(dest) == util_get_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    test_error(dest->num_elements >= src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif

    uint size = util_get_size(src);
    util_swap(src->data_buffer + size * idxsrc,dest->data_buffer + size * idxdest,size);
}

void replace_buffer_at_buffer(buffer src, uint idxsrc, buffer dest, uint idxdest)
{
    #ifdef ERROR_CHECKING
    test_error(idxsrc < src->num_elements && idxdest < dest->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    test_error(util_get_size(dest) == util_get_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    test_error(dest->num_elements >= src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif

    uint size = util_get_size(src);
    util_copy(src->data_buffer + size * idxsrc,dest->data_buffer + size * idxdest,size);
}

void replace_buffer_at(buffer target, uint index, buffer element)
{
    uint size = util_get_size(target);
    util_copy(element->data_buffer,target->data_buffer + size * index,size);
}

void remove_buffer_at(buffer target,uint index)
{
    uint i,size = util_get_size(target);
    target->num_elements--;
    for (i = index; i < target->num_elements; i++)
        util_copy(target->data_buffer + size * (i + 1),target->data_buffer + size * i, size);
    target->data_buffer = realloc(target->data_buffer,target->num_elements * size);
}

void resize_buffer(buffer target, uint num_elements)
{
    uint size = util_get_size(target);
    target->num_elements = num_elements;
    target->data_buffer = realloc(target->data_buffer,num_elements * size);
}

uint get_buffer_element_size(buffer target)
{
    uint size = util_get_size(target);
    return size;
}

void set_buffer_iterator(buffer target,uint iterator)
{
    target->iterator = iterator - 1;
}

uint get_buffer_iterator(buffer target)
{
    return target->iterator;
}

uint get_buffer_length(buffer target)
{
    return target->num_elements;
}

void* get_buffer_field(buffer target, uint element, uint field)
{
    uint i,off = 0,size = util_get_size(target);
    for (i = 0; i < field; i++)
        off += sizes[target->types[i]];

    return target->data_buffer + off + size * element;
}


uint get_buffer_fieldui(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(UINT == target->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(uint)get_buffer_field(target,element,field);
}
int get_buffer_fieldi(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(INT == target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_buffer_field(target,element,field);
}
float get_buffer_fieldf(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(FLOAT == target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(float)get_buffer_field(target,element,field);
}
char get_buffer_fieldc(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(CHAR == target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(char)get_buffer_field(target,element,field);
}
unsigned char get_buffer_fielduc(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(UCHAR == target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_buffer_field(target,element,field);
}

void* get_buffer_fieldv(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(VOID == target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_buffer_field(target,element,field);
}

void set_buffer_field(buffer target, uint element, uint field, void* data)
{
    #ifdef ERROR_CHECKING
    test_error(element < target->num_elements,ERROR_OUT_OF_BOUNDS_ELEMENT);
    test_error(data != NULL,ERROR_INVALID_DATA);
    #endif

    memcpy(get_buffer_field(target,element,field),data,sizes[target->types[field]]);
}

void set_buffer_fieldui(buffer target, uint element, uint field, uint data)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(UINT == target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldi(buffer target, uint element, uint field, int data)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(INT == target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldf(buffer target, uint element, uint field, float data)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(FLOAT == target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldc(buffer target, uint element, uint field, char data)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(CHAR == target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fielduc(buffer target, uint element, uint field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(UCHAR == target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldv(buffer target, uint element, uint field, void* data)
{
    #ifdef ERROR_CHECKING
    test_error(field < target->num_types,ERROR_INVALID_FIELD);
    test_error(VOID == target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void repush_buffer_types(buffer target)
{
    uint i;
    for (i = 0; i < target->num_types; i++)
        push_type(target->types[i]);
}
void repush_types()
{
    uint i;
    for (i = 0; i < current_buffer->num_types; i++)
        push_type(current_buffer->types[i]);
}

buffer create_single_buffer_element(buffer target)
{
    buffer element;
    element = malloc(sizeof(struct buffer));
    element->num_types = target->num_types;
    element->iterator = -1;
    element->types = malloc(sizeof(enum type) * target->num_types);
    memcpy(element->types,target->types,sizeof(enum type) * target->num_types);
    uint size = util_get_size(target);
    element->data_buffer = malloc(size);
    element->num_elements = 1;

    return element;
}

buffer create_single_element()
{
    buffer element;
    element = malloc(sizeof(struct buffer));
    element->num_types = current_buffer->num_types;
    element->iterator = -1;
    element->types = malloc(sizeof(enum type) * current_buffer->num_types);
    memcpy(element->types,current_buffer->types,sizeof(enum type) * current_buffer->num_types);
    uint size = util_get_size(current_buffer);
    element->data_buffer = malloc(size);
    element->num_elements = 1;

    return element;
}

void copy_to_buffer(buffer dest)
{
    #ifdef ERROR_CHECKING
    test_error(get_buffer_element_size(dest) == get_buffer_element_size(current_buffer),ERROR_UNEQUAL_ELEMENT_SIZE);
    test_error(dest->num_elements >= current_buffer->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif
    util_copy(current_buffer->data_buffer,dest->data_buffer,dest->num_elements * get_buffer_element_size(dest));
}

void copy_from_buffer(buffer src)
{
    #ifdef ERROR_CHECKING
    test_error(get_buffer_element_size(current_buffer) == get_buffer_element_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    test_error(current_buffer->num_elements >= src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif
    util_copy(src->data_buffer,current_buffer->data_buffer,current_buffer->num_elements * get_buffer_element_size(current_buffer));
}

void copy_buffer_to_buffer(buffer src,buffer dest)
{
    #ifdef ERROR_CHECKING
    test_error(get_buffer_element_size(dest) == get_buffer_element_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    test_error(dest->num_elements >= src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif
    util_copy(src->data_buffer,dest->data_buffer,dest->num_elements * get_buffer_element_size(dest));
}

buffer copy_buffer(buffer src)
{
    buffer copy = malloc(sizeof(struct buffer));

    copy->iterator = src->iterator;

    uint i, size = util_get_size(src);
    copy->num_types = src->num_types;
    copy->types = malloc(sizeof(enum type) * copy->num_types);
    for (i = 0; i < src->num_types; i++)
        copy->types[i] = src->types[i];

    copy->data_buffer = malloc(src->num_elements * size);
    copy->num_elements = src->num_elements;

    copy_buffer_to_buffer(src,copy);

    return copy;
}



void append_at(buffer src)
{
    uint size = util_get_size(current_buffer),old_num_element = current_buffer->num_elements;
    resize_buffer(current_buffer,current_buffer->num_elements + src->num_elements);
    memcpy(current_buffer->data_buffer + size * old_num_element,src->data_buffer,util_get_size(src) * src->num_elements);
}

void append_to(buffer dest)
{
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + current_buffer->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,current_buffer->data_buffer,util_get_size(current_buffer) * current_buffer->num_elements);
}

void append_buffer_at(buffer src, buffer dest)
{
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + src->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,src->data_buffer,util_get_size(src) * src->num_elements);
}

void append_element_at(buffer src, uint index)
{
    uint size = util_get_size(current_buffer),old_num_element = current_buffer->num_elements;
    resize_buffer(current_buffer,current_buffer->num_elements + src->num_elements);
    memcpy(current_buffer->data_buffer + size * old_num_element,src->data_buffer + get_buffer_element_data_offset(src,index),util_get_size(src));
}

void append_element_to(buffer dest, uint index)
{
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + current_buffer->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,current_buffer->data_buffer + get_buffer_element_data_offset(current_buffer,index),util_get_size(current_buffer));
}

void append_buffer_element_at(buffer src, uint index, buffer dest)
{
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + src->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,src->data_buffer + get_buffer_element_data_offset(src,index),util_get_size(src));
}



















