#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
typedef uint32_t uint;

#include <DBG/debug.h>

enum TYPE
{
    UINT,INT,FLOAT,CHAR,UCHAR,VOID
};

enum ERRORS
{
    ERROR_INVALID_FIELD,
    ERROR_NO_PUSHED_TYPES,
    ERROR_INVALID_DATA,
    ERROR_BAD_BUFFER,
    ERROR_INVALID_TYPE,
    ERROR_OUT_OF_BOUNDS_INDEX,
    ERROR_OUT_OF_BOUNDS_ELEMENT,
    ERROR_UNEQUAL_ELEMENT_SIZE,
    ERROR_SMALL_DEST_BUFFER,
    ERROR_INVALID_NUM_TYPES,
    ERROR_NO_BOUND_BUFFER,
    NUM_ERROR_MESSAGES
};

const char* ERROR_MESSAGES[NUM_ERROR_MESSAGES] = {
    "ERROR_INVALID_FIELD",
    "ERROR_NO_PUSHED_TYPES",
    "ERROR_INVALID_DATA",
    "ERROR_BAD_BUFFER",
    "ERROR_INVALID_TYPE",
    "ERROR_OUT_OF_BOUNDS_INDEX",
    "ERROR_OUT_OF_BOUNDS_ELEMENT",
    "ERROR_UNEQUAL_ELEMENT_SIZE",
    "ERROR_SMALL_DEST_BUFFER",
    "ERROR_INVALID_NUM_TYPES",
    "ERROR_NO_BOUND_BUFFER"
};

struct buffer
{
    uint iterator,num_types,num_elements;
    void* data_buffer;
    enum TYPE* types;
};

typedef struct buffer* buffer;

#define cast_to(type) *(type*)

buffer CURRENT_BUFFER = NULL;
enum TYPE* CURRENT_TYPES = NULL;
uint CURRENT_NUM_TYPES = 0;

const uint sizes[6] = {sizeof(uint),sizeof(int),sizeof(float),sizeof(char),sizeof(unsigned char),sizeof(void*)};

void error_if(int failure, uint error, const char* function)
{
    if (failure)
    {
        printf("%s errored with code %u: %s\n",function,error,ERROR_MESSAGES[error]);
        exit(error + 100);
    }
}

#define error_if(failure,error)     error_if(failure,error,__FUNCTION__);

uint util_get_size(buffer target)
{
    uint i, size = 0;
    for (i = 0; i < target->num_types; i++)
        size += sizes[target->types[i]];
    return size;
}

void swap(void* src1, void* src2, uint size)
{
    uint8_t temp[size];
    memcpy(temp,src2,size);
    memcpy(src2, src1,size);
    memcpy(src1, temp,size);
}

void push_type(enum TYPE t)
{
    if (CURRENT_TYPES == NULL)
    {
        CURRENT_TYPES = malloc(0);
        CURRENT_NUM_TYPES = 0;
    }
    CURRENT_TYPES = realloc(CURRENT_TYPES,(CURRENT_NUM_TYPES + 1) * sizeof(enum TYPE));
    CURRENT_TYPES[CURRENT_NUM_TYPES] = t;
    CURRENT_NUM_TYPES++;
}

void bind_buffer_at(buffer target, uint index)
{
#ifdef ERROR_CHECKING
	error_if(target == NULL,ERROR_BAD_BUFFER);
#endif
	CURRENT_BUFFER = target;
    CURRENT_BUFFER->iterator = index;
}

uint get_buffer_size(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return util_get_size(target) * target->num_elements;
}

uint get_size()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements;
}

void zero_buffer_out(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    memset(target->data_buffer,0,get_buffer_size(target));
}

void zero_out()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    memset(CURRENT_BUFFER->data_buffer,0,get_buffer_size(CURRENT_BUFFER));
}

buffer init_buffer(uint num_elements)
{
    buffer target;
    target = malloc(sizeof(struct buffer));
    target->num_types = CURRENT_NUM_TYPES;

    target->iterator = -1;

    #ifdef ERROR_CHECKING
    error_if(CURRENT_TYPES == NULL,ERROR_NO_PUSHED_TYPES);
    #endif

    target->types = CURRENT_TYPES;

    
    uint size = util_get_size(target);

    target->data_buffer = malloc(num_elements * size);
    target->num_elements = num_elements;

    CURRENT_TYPES = NULL;
    return target;
}

void deinit_buffer(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    if (target == CURRENT_BUFFER)
        CURRENT_BUFFER = NULL;

    free(target->data_buffer);
    free(target->types);
    free(target);
}

void* get_data_buffer()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return CURRENT_BUFFER->data_buffer;
}

uint iterate_over(buffer target)
{   
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    target->iterator++;
    if (target->iterator == target->num_elements || target->num_elements == 0)
    {
        target->iterator = -1;
        return 0;
    }
    else
    {
		bind_buffer_at(target,target->iterator);
        return 1;
    }
}

void swap_at(uint idx1, uint idx2)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(idx1 >= CURRENT_BUFFER->num_elements || idx2 >= CURRENT_BUFFER->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif
    uint size = util_get_size(CURRENT_BUFFER);

    swap(CURRENT_BUFFER->data_buffer + size * idx1,CURRENT_BUFFER->data_buffer + size * idx2,size);
}

void replace_at(uint index, buffer data)
{
    #ifdef ERROR_CHECKING
    error_if(data == NULL,ERROR_INVALID_DATA);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(CURRENT_BUFFER);
    memcpy(CURRENT_BUFFER->data_buffer + size * index,data->data_buffer,size);
}

void remove_at(uint index)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint i,size = util_get_size(CURRENT_BUFFER);
    CURRENT_BUFFER->num_elements--;
    for (i = index; i < CURRENT_BUFFER->num_elements; i++)
        memcpy(CURRENT_BUFFER->data_buffer + size * i,CURRENT_BUFFER->data_buffer + size * (i + 1), size);
    CURRENT_BUFFER->data_buffer = realloc(CURRENT_BUFFER->data_buffer,CURRENT_BUFFER->num_elements * size);
}

void resize(uint num_elements)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(CURRENT_BUFFER);
    CURRENT_BUFFER->num_elements = num_elements;
    CURRENT_BUFFER->data_buffer = realloc(CURRENT_BUFFER->data_buffer,num_elements * size);
}

uint get_element_size()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(CURRENT_BUFFER);
    return size;
}

void set_iterator(uint iterator)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    CURRENT_BUFFER->iterator = iterator - 1;
}

uint get_iterator()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return CURRENT_BUFFER->iterator;
}

uint get_length()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return CURRENT_BUFFER->num_elements;
}

void* get_field(uint field)
{
    uint i,off = 0,size = util_get_size(CURRENT_BUFFER);
    for (i = 0; i < field; i++)
        off += sizes[CURRENT_BUFFER->types[i]];

    return CURRENT_BUFFER->data_buffer + off + size * CURRENT_BUFFER->iterator;
}


uint get_fieldui(uint field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(uint)get_field(field);
}
int get_fieldi(uint field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(INT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_field(field);
}
float get_fieldf(uint field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif


    return cast_to(float)get_field(field);
}
char get_fieldc(uint field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif


    return cast_to(char)get_field(field);
}
unsigned char get_fielduc(uint field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_field(field);
}

void* get_fieldv(uint field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_field(field);
}

void set_field(uint field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(data == NULL,ERROR_INVALID_DATA);
    #endif
    memcpy(get_field(field),data,sizes[CURRENT_BUFFER->types[field]]);
}

void set_fieldui(uint field, uint data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldi(uint field, int data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(INT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldf(uint field, float data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif
    set_field(field,&data);
}

void set_fieldc(uint field, char data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fielduc(uint field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldv(uint field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

uint get_element_data_offset(uint index)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return util_get_size(CURRENT_BUFFER) * index;
}

uint get_buffer_element_data_offset(buffer target, uint index)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return util_get_size(target) * index;
}

void* get_buffer_data_buffer(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return target->data_buffer;
}

void swap_buffer_at(buffer target, uint idx1, uint idx2)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(idx1 > target->num_elements || idx2 > target->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif
    uint size = util_get_size(target);

    swap(target->data_buffer + get_buffer_element_data_offset(target,idx1),target->data_buffer + get_buffer_element_data_offset(target,idx2),size);
}

void swap_buffer_at_buffer(buffer src, uint idxsrc, buffer dest, uint idxdest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(idxsrc >= src->num_elements || idxdest >= dest->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    error_if(util_get_size(dest) != util_get_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(dest->num_elements < src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif

    uint size = util_get_size(src);
    swap(src->data_buffer + size * idxsrc,dest->data_buffer + size * idxdest,size);
}

void replace_buffer_at_buffer(buffer src, uint idxsrc, buffer dest, uint idxdest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(idxsrc >= src->num_elements || idxdest >= dest->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    error_if(util_get_size(dest) != util_get_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(dest->num_elements < src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif

    uint size = util_get_size(src);
    memcpy(dest->data_buffer + size * idxdest,src->data_buffer + size * idxsrc,size);
}

void replace_buffer_at(buffer target, uint index, buffer element)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    uint size = util_get_size(target);
    memcpy(target->data_buffer + size * index,element->data_buffer,size);
}

void remove_buffer_at(buffer target,uint index)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    uint i,size = util_get_size(target);
    target->num_elements--;
    for (i = index; i < target->num_elements; i++)
        memcpy(target->data_buffer + size * i,target->data_buffer + size * (i + 1), size);
    target->data_buffer = realloc(target->data_buffer,target->num_elements * size);
}

void resize_buffer(buffer target, uint num_elements)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    uint size = util_get_size(target);
    target->num_elements = num_elements;
    target->data_buffer = realloc(target->data_buffer,num_elements * size);
}

uint get_buffer_element_size(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    uint size = util_get_size(target);
    return size;
}

void set_buffer_iterator(buffer target,uint iterator)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    target->iterator = iterator - 1;
}

uint get_buffer_iterator(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return target->iterator;
}

uint get_buffer_length(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return target->num_elements;
}

void* get_buffer_field(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    uint i,off = 0,size = util_get_size(target);
    for (i = 0; i < field; i++)
        off += sizes[target->types[i]];

    return target->data_buffer + off + size * element;
}


uint get_buffer_fieldui(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != target->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(uint)get_buffer_field(target,element,field);
}
int get_buffer_fieldi(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(INT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_buffer_field(target,element,field);
}
float get_buffer_fieldf(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(float)get_buffer_field(target,element,field);
}
char get_buffer_fieldc(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(char)get_buffer_field(target,element,field);
}
unsigned char get_buffer_fielduc(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_buffer_field(target,element,field);
}

void* get_buffer_fieldv(buffer target, uint element, uint field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_buffer_field(target,element,field);
}

void set_buffer_field(buffer target, uint element, uint field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(element >= target->num_elements,ERROR_OUT_OF_BOUNDS_ELEMENT);
    error_if(data == NULL,ERROR_INVALID_DATA);
    #endif

    memcpy(get_buffer_field(target,element,field),data,sizes[target->types[field]]);
}

void set_buffer_fieldui(buffer target, uint element, uint field, uint data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldi(buffer target, uint element, uint field, int data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(INT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldf(buffer target, uint element, uint field, float data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldc(buffer target, uint element, uint field, char data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fielduc(buffer target, uint element, uint field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldv(buffer target, uint element, uint field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void repush_buffer_types(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    uint i;
    for (i = 0; i < target->num_types; i++)
        push_type(target->types[i]);
}
void repush_types()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint i;
    for (i = 0; i < CURRENT_BUFFER->num_types; i++)
        push_type(CURRENT_BUFFER->types[i]);
}

buffer create_single_buffer_element(buffer target)
{
    buffer element;
    element = malloc(sizeof(struct buffer));
    element->num_types = target->num_types;
    element->iterator = -1;
    element->types = malloc(sizeof(enum TYPE) * target->num_types);
    memcpy(element->types,target->types,sizeof(enum TYPE) * target->num_types);
    uint size = util_get_size(target);
    element->data_buffer = malloc(size);
    element->num_elements = 1;

    return element;
}

buffer create_single_element()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    buffer element;
    element = malloc(sizeof(struct buffer));
    element->num_types = CURRENT_BUFFER->num_types;
    element->iterator = -1;
    element->types = malloc(sizeof(enum TYPE) * CURRENT_BUFFER->num_types);
    memcpy(element->types,CURRENT_BUFFER->types,sizeof(enum TYPE) * CURRENT_BUFFER->num_types);
    uint size = util_get_size(CURRENT_BUFFER);
    element->data_buffer = malloc(size);
    element->num_elements = 1;

    return element;
}

void copy_to_buffer(buffer dest)
{

    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(get_buffer_element_size(dest) != get_buffer_element_size(CURRENT_BUFFER),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(dest->num_elements < CURRENT_BUFFER->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif
    memcpy(dest->data_buffer,CURRENT_BUFFER->data_buffer,dest->num_elements * get_buffer_element_size(dest));
}

void copy_from_buffer(buffer src)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(get_buffer_element_size(CURRENT_BUFFER) != get_buffer_element_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(CURRENT_BUFFER->num_elements < src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif
    memcpy(CURRENT_BUFFER->data_buffer,src->data_buffer,CURRENT_BUFFER->num_elements * get_buffer_element_size(CURRENT_BUFFER));
}

void copy_buffer_to_buffer(buffer src,buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(get_buffer_element_size(dest) != get_buffer_element_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(dest->num_elements < src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif
    memcpy(dest->data_buffer,src->data_buffer,dest->num_elements * get_buffer_element_size(dest));
}

buffer copy_buffer(buffer src)
{
    buffer copy = malloc(sizeof(struct buffer));

    copy->iterator = src->iterator;

    uint i, size = util_get_size(src);
    copy->num_types = src->num_types;
    copy->types = malloc(sizeof(enum TYPE) * copy->num_types);
    for (i = 0; i < src->num_types; i++)
        copy->types[i] = src->types[i];

    copy->data_buffer = malloc(src->num_elements * size);
    copy->num_elements = src->num_elements;

    copy_buffer_to_buffer(src,copy);

    return copy;
}



void append_at(buffer src)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(CURRENT_BUFFER),old_num_element = CURRENT_BUFFER->num_elements;
    resize_buffer(CURRENT_BUFFER,CURRENT_BUFFER->num_elements + src->num_elements);
    memcpy(CURRENT_BUFFER->data_buffer + size * old_num_element,src->data_buffer,util_get_size(src) * src->num_elements);
}

void append_to(buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + CURRENT_BUFFER->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,CURRENT_BUFFER->data_buffer,util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements);
}

void append_buffer_at(buffer src, buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    #endif
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + src->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,src->data_buffer,util_get_size(src) * src->num_elements);
}

void append_element_at(buffer src, uint index)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(CURRENT_BUFFER),old_num_element = CURRENT_BUFFER->num_elements;
    resize_buffer(CURRENT_BUFFER,CURRENT_BUFFER->num_elements + src->num_elements);
    memcpy(CURRENT_BUFFER->data_buffer + size * old_num_element,src->data_buffer + get_buffer_element_data_offset(src,index),util_get_size(src));
}

void append_element_to(buffer dest, uint index)
{
    #ifdef ERROR_CHECKING
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + CURRENT_BUFFER->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,index),util_get_size(CURRENT_BUFFER));
}

void append_buffer_element_at(buffer src, uint index, buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    #endif
    uint size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + src->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,src->data_buffer + get_buffer_element_data_offset(src,index),util_get_size(src));
}

void flush_types()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_TYPES == NULL,ERROR_NO_PUSHED_TYPES);
    #endif
    free(CURRENT_TYPES);
    CURRENT_TYPES = NULL;
    CURRENT_NUM_TYPES = 0;
}

void pop_types(uint num_types)
{
    #ifdef ERROR_CHECKING
    error_if(num_types < num_types,ERROR_INVALID_NUM_TYPES);
    #endif
    CURRENT_NUM_TYPES -= num_types;
    CURRENT_TYPES = realloc(CURRENT_TYPES,CURRENT_NUM_TYPES * sizeof(enum TYPE));
}

void* dump_buffer_binary(buffer target, uint* size)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(size == NULL,ERROR_INVALID_DATA);
    #endif
    *size = util_get_size(target) * target->num_elements;
    void* bin_data = malloc(*size); 
    memcpy(bin_data,target->data_buffer,*size);
    return bin_data;
}
void load_buffer_binary(buffer target, void* bin_data, uint size)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(bin_data == NULL,ERROR_INVALID_DATA);
    #endif
    if (util_get_size(target) * target->num_elements == size)
    {
        memcpy(target->data_buffer,bin_data,size);
    }
    else
    {   
        resize_buffer(target,size / util_get_size(target));
        memcpy(target->data_buffer,bin_data,size);
    }
}

void* dump_binary(uint* size)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(size == NULL,ERROR_INVALID_DATA);
    #endif
    *size = util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements;
    void* bin_data = malloc(*size); 
    memcpy(bin_data,CURRENT_BUFFER->data_buffer,*size);
    return bin_data;
}
void load_binary(void* bin_data, uint size)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(bin_data == NULL,ERROR_INVALID_DATA);
    #endif
    if (util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements == size)
    {
        memcpy(CURRENT_BUFFER->data_buffer,bin_data,size);
    }
    else
    {   
        resize_buffer(CURRENT_BUFFER,size / util_get_size(CURRENT_BUFFER));
        memcpy(CURRENT_BUFFER->data_buffer,bin_data,size);
    }
}