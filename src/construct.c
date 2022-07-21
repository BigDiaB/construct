
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
    ERROR_INVALID_INDEX,
    ERROR_BAD_TYPES,
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
    "ERROR_NO_BOUND_BUFFER",
    "ERROR_BAD_TYPES",
    "ERROR_INVALID_INDEX"
};

#define cast_to(type) *(type*)
#define NULL ((void*)0)

struct buffer
{
    unsigned int iterator,num_types,num_elements;
    void* data_buffer;
    enum construct_types* types;
};

typedef struct buffer* buffer;

buffer CURRENT_BUFFER = NULL;
enum construct_types* CURRENT_TYPES = NULL;
unsigned int CURRENT_NUM_TYPES = 0;
static const unsigned int sizes[6] = {sizeof(unsigned int),sizeof(int),sizeof(float),sizeof(char),sizeof(unsigned char),sizeof(void*)};

void error_if(int failure, enum ERRORS error, const char* function);
unsigned int util_get_size(buffer target);
void swap(void* src1, void* src2, unsigned int size);

#define CONSTRUCT_IMPLEMENTATION
#include "construct.h"

void* memset(void* dest, int val, size_t len)
{
    unsigned char *ptr = dest;
    while (len-- > 0)
        *ptr++ = val;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t len)
{
    char *d = dest;
    const char *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

void error_if(int failure, unsigned int error, const char* function)
{
    if (failure)
    {
        printf("%s errored with code %u: %s\n",function,error,ERROR_MESSAGES[error]);
        exit(error + 100);
    }
}

#define error_if(failure,error)     error_if(failure,error,__FUNCTION__);

unsigned int util_get_size(buffer target)
{
    unsigned int i, size = 0;
    for (i = 0; i < target->num_types; i++)
        size += sizes[target->types[i]];
    return size;
}

unsigned int util_get_size_until(buffer target, unsigned int num_fields)
{
    unsigned int i, size = 0;
    for (i = 0; i < num_fields; i++)
        size += sizes[target->types[i]];
    return size;
}

void swap(void* src1, void* src2, unsigned int size)
{
    unsigned char temp[size];
    memcpy(temp,src2,size);
    memcpy(src2, src1,size);
    memcpy(src1, temp,size);
}

void push_type(enum construct_types t)
{
    if (CURRENT_TYPES == NULL)
    {
        CURRENT_TYPES = malloc(0);
        CURRENT_NUM_TYPES = 0;
    }
    CURRENT_TYPES = realloc(CURRENT_TYPES,(CURRENT_NUM_TYPES + 1) * sizeof(enum construct_types));
    CURRENT_TYPES[CURRENT_NUM_TYPES] = t;
    CURRENT_NUM_TYPES++;
}

void bind_buffer_at(buffer target, unsigned int index)
{
    #ifdef ERROR_CHECKING
	error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
	CURRENT_BUFFER = target;
    CURRENT_BUFFER->iterator = index;
}

unsigned int get_buffer_size(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return util_get_size(target) * target->num_elements;
}

unsigned int get_size()
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

buffer init_buffer(unsigned int num_elements)
{
    buffer target;
    target = malloc(sizeof(struct buffer));
    target->num_types = CURRENT_NUM_TYPES;

    target->iterator = -1;

    #ifdef ERROR_CHECKING
    error_if(CURRENT_TYPES == NULL,ERROR_NO_PUSHED_TYPES);
    #endif

    target->types = CURRENT_TYPES;

    
    unsigned int size = util_get_size(target);

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

unsigned int iterate_over(buffer target)
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

void swap_at(unsigned int idx1, unsigned int idx2)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(idx1 >= CURRENT_BUFFER->num_elements || idx2 >= CURRENT_BUFFER->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif
    unsigned int size = util_get_size(CURRENT_BUFFER);

    swap(CURRENT_BUFFER->data_buffer + size * idx1,CURRENT_BUFFER->data_buffer + size * idx2,size);
}

void replace_at(unsigned int index, buffer data)
{
    #ifdef ERROR_CHECKING
    error_if(data == NULL,ERROR_INVALID_DATA);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int size = util_get_size(CURRENT_BUFFER);
    memcpy(CURRENT_BUFFER->data_buffer + size * index,data->data_buffer,size);
}

void remove_at(unsigned int index)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int i,size = util_get_size(CURRENT_BUFFER);
    CURRENT_BUFFER->num_elements--;
    for (i = index; i < CURRENT_BUFFER->num_elements; i++)
        memcpy(CURRENT_BUFFER->data_buffer + size * i,CURRENT_BUFFER->data_buffer + size * (i + 1), size);
    CURRENT_BUFFER->data_buffer = realloc(CURRENT_BUFFER->data_buffer,CURRENT_BUFFER->num_elements * size);
    CURRENT_BUFFER->iterator--;
}

void resize(unsigned int num_elements)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int size = util_get_size(CURRENT_BUFFER);
    CURRENT_BUFFER->num_elements = num_elements;
    CURRENT_BUFFER->data_buffer = realloc(CURRENT_BUFFER->data_buffer,num_elements * size);
}

unsigned int get_element_size()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int size = util_get_size(CURRENT_BUFFER);
    return size;
}

void set_iterator(unsigned int iterator)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    CURRENT_BUFFER->iterator = iterator - 1;
}

unsigned int get_iterator()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return CURRENT_BUFFER->iterator;
}

unsigned int get_length()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return CURRENT_BUFFER->num_elements;
}

void* get_field(unsigned int field)
{
    unsigned int i,off = 0,size = util_get_size(CURRENT_BUFFER);
    for (i = 0; i < field; i++)
        off += sizes[CURRENT_BUFFER->types[i]];

    return CURRENT_BUFFER->data_buffer + off + size * CURRENT_BUFFER->iterator;
}


unsigned int get_fieldui(unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(unsigned int)get_field(field);
}

int get_fieldi(unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(INT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_field(field);
}

float get_fieldf(unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif


    return cast_to(float)get_field(field);
}

char get_fieldc(unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif


    return cast_to(char)get_field(field);
}

unsigned char get_fielduc(unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_field(field);
}

void* get_fieldv(unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_field(field);
}

void set_field(unsigned int field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(data == NULL,ERROR_INVALID_DATA);
    #endif
    memcpy(get_field(field),data,sizes[CURRENT_BUFFER->types[field]]);
}

void set_fieldui(unsigned int field, unsigned int data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldi(unsigned int field, int data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(INT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldf(unsigned int field, float data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif
    set_field(field,&data);
}

void set_fieldc(unsigned int field, char data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fielduc(unsigned int field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

void set_fieldv(unsigned int field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(field >= CURRENT_BUFFER->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != CURRENT_BUFFER->types[field],ERROR_INVALID_TYPE);
    #endif

    set_field(field,&data);
}

unsigned int get_element_data_offset(unsigned int index)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    return util_get_size(CURRENT_BUFFER) * index;
}

unsigned int get_buffer_element_data_offset(buffer target, unsigned int index)
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

void swap_buffer_at(buffer target, unsigned int idx1, unsigned int idx2)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(idx1 > target->num_elements || idx2 > target->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif
    unsigned int size = util_get_size(target);

    swap(target->data_buffer + get_buffer_element_data_offset(target,idx1),target->data_buffer + get_buffer_element_data_offset(target,idx2),size);
}

void swap_buffer_at_buffer(buffer src, unsigned int idxsrc, buffer dest, unsigned int idxdest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(idxsrc >= src->num_elements || idxdest >= dest->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    error_if(util_get_size(dest) != util_get_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(dest->num_elements < src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif

    unsigned int size = util_get_size(src);
    swap(src->data_buffer + size * idxsrc,dest->data_buffer + size * idxdest,size);
}

void replace_buffer_at_buffer(buffer src, unsigned int idxsrc, buffer dest, unsigned int idxdest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(idxsrc >= src->num_elements || idxdest >= dest->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    error_if(util_get_size(dest) != util_get_size(src),ERROR_UNEQUAL_ELEMENT_SIZE);
    error_if(dest->num_elements < src->num_elements, ERROR_SMALL_DEST_BUFFER);
    #endif

    unsigned int size = util_get_size(src);
    memcpy(dest->data_buffer + size * idxdest,src->data_buffer + size * idxsrc,size);
}

void replace_buffer_at(buffer target, unsigned int index, buffer element)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int size = util_get_size(target);
    memcpy(target->data_buffer + size * index,element->data_buffer,size);
}

void remove_buffer_at(buffer target,unsigned int index)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int i,size = util_get_size(target);
    target->num_elements--;
    for (i = index; i < target->num_elements; i++)
        memcpy(target->data_buffer + size * i,target->data_buffer + size * (i + 1), size);
    target->data_buffer = realloc(target->data_buffer,target->num_elements * size);
}

void resize_buffer(buffer target, unsigned int num_elements)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int size = util_get_size(target);
    target->num_elements = num_elements;
    target->data_buffer = realloc(target->data_buffer,num_elements * size);
}

unsigned int get_buffer_element_size(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int size = util_get_size(target);
    return size;
}

void set_buffer_iterator(buffer target,unsigned int iterator)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    target->iterator = iterator - 1;
}

unsigned int get_buffer_iterator(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return target->iterator;
}

unsigned int get_buffer_length(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    return target->num_elements;
}

void* get_buffer_field(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int i,off = 0,size = util_get_size(target);
    for (i = 0; i < field; i++)
        off += sizes[target->types[i]];

    return target->data_buffer + off + size * element;
}


unsigned int get_buffer_fieldui(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != target->types[field],ERROR_INVALID_TYPE);
    #endif
    
    return cast_to(unsigned int)get_buffer_field(target,element,field);
}

int get_buffer_fieldi(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(INT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(int)get_buffer_field(target,element,field);
}

float get_buffer_fieldf(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(float)get_buffer_field(target,element,field);
}

char get_buffer_fieldc(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(char)get_buffer_field(target,element,field);
}

unsigned char get_buffer_fielduc(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(unsigned char)get_buffer_field(target,element,field);
}

void* get_buffer_fieldv(buffer target, unsigned int element, unsigned int field)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(VOID != target->types[field],ERROR_INVALID_TYPE);
    #endif

    return cast_to(void*)get_buffer_field(target,element,field);
}

void set_buffer_field(buffer target, unsigned int element, unsigned int field, void* data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(element >= target->num_elements,ERROR_OUT_OF_BOUNDS_ELEMENT);
    error_if(data == NULL,ERROR_INVALID_DATA);
    #endif

    memcpy(get_buffer_field(target,element,field),data,sizes[target->types[field]]);
}

void set_buffer_fieldui(buffer target, unsigned int element, unsigned int field, unsigned int data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UINT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldi(buffer target, unsigned int element, unsigned int field, int data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(INT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldf(buffer target, unsigned int element, unsigned int field, float data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(FLOAT != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldc(buffer target, unsigned int element, unsigned int field, char data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(CHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fielduc(buffer target, unsigned int element, unsigned int field, unsigned char data)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(field >= target->num_types,ERROR_INVALID_FIELD);
    error_if(UCHAR != target->types[field],ERROR_INVALID_TYPE);
    #endif

    set_buffer_field(target,element, field,&data);
}

void set_buffer_fieldv(buffer target, unsigned int element, unsigned int field, void* data)
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
    unsigned int i;
    for (i = 0; i < target->num_types; i++)
        push_type(target->types[i]);
}

void repush_types()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int i;
    for (i = 0; i < CURRENT_BUFFER->num_types; i++)
        push_type(CURRENT_BUFFER->types[i]);
}

buffer create_single_buffer_element(buffer target)
{
    buffer element;
    element = malloc(sizeof(struct buffer));
    element->num_types = target->num_types;
    element->iterator = -1;
    element->types = malloc(sizeof(enum construct_types) * target->num_types);
    memcpy(element->types,target->types,sizeof(enum construct_types) * target->num_types);
    unsigned int size = util_get_size(target);
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
    element->types = malloc(sizeof(enum construct_types) * CURRENT_BUFFER->num_types);
    memcpy(element->types,CURRENT_BUFFER->types,sizeof(enum construct_types) * CURRENT_BUFFER->num_types);
    unsigned int size = util_get_size(CURRENT_BUFFER);
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

    unsigned int i, size = util_get_size(src);
    copy->num_types = src->num_types;
    copy->types = malloc(sizeof(enum construct_types) * copy->num_types);
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
    unsigned int size = util_get_size(CURRENT_BUFFER),old_num_element = CURRENT_BUFFER->num_elements;
    resize_buffer(CURRENT_BUFFER,CURRENT_BUFFER->num_elements + src->num_elements);
    memcpy(CURRENT_BUFFER->data_buffer + size * old_num_element,src->data_buffer,util_get_size(src) * src->num_elements);
}

void append_to(buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + CURRENT_BUFFER->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,CURRENT_BUFFER->data_buffer,util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements);
}

void append_buffer_at(buffer src, buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int size = util_get_size(dest),old_num_element = dest->num_elements;
    resize_buffer(dest,dest->num_elements + src->num_elements);
    memcpy(dest->data_buffer + size * old_num_element,src->data_buffer,util_get_size(src) * src->num_elements);
}

void append_element_at(buffer src, unsigned int index)
{
    #ifdef ERROR_CHECKING
    error_if(src == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int size = util_get_size(CURRENT_BUFFER),old_num_element = CURRENT_BUFFER->num_elements;
    resize_buffer(CURRENT_BUFFER,CURRENT_BUFFER->num_elements + src->num_elements);
    memcpy(CURRENT_BUFFER->data_buffer + size * old_num_element,src->data_buffer + get_buffer_element_data_offset(src,index),util_get_size(src));
}

void append_element_to(buffer dest, unsigned int index)
{
    #ifdef ERROR_CHECKING
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif
    unsigned int size = util_get_size(dest);
    resize_buffer(dest,dest->num_elements + 1);
    memcpy(dest->data_buffer + size * (dest->num_elements - 1),CURRENT_BUFFER->data_buffer + size * index,size);
}

void append_buffer_element_at(buffer src, unsigned int index, buffer dest)
{
    #ifdef ERROR_CHECKING
    error_if(dest == NULL,ERROR_BAD_BUFFER);
    error_if(src == NULL,ERROR_BAD_BUFFER);
    #endif
    unsigned int size = util_get_size(dest);
    resize_buffer(dest,dest->num_elements + 1);
    memcpy(dest->data_buffer + size * (dest->num_elements - 1),src->data_buffer + size * index,size);
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

void pop_types(unsigned int num_types)
{
    #ifdef ERROR_CHECKING
    error_if(num_types < num_types,ERROR_INVALID_NUM_TYPES);
    #endif
    CURRENT_NUM_TYPES -= num_types;
    CURRENT_TYPES = realloc(CURRENT_TYPES,CURRENT_NUM_TYPES * sizeof(enum construct_types));
}

void* dump_buffer_binary(buffer target, unsigned int* size)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif

    void* bin_data;

    if (size == NULL)
    {
        bin_data = malloc(util_get_size(target) * target->num_elements); 
        memcpy(bin_data,target->data_buffer,util_get_size(target) * target->num_elements);
    }
    else
    {
        *size = util_get_size(target) * target->num_elements;
        bin_data = malloc(*size); 
        memcpy(bin_data,target->data_buffer,*size);
    }
    
    return bin_data;
}

void load_buffer_binary(buffer target, void* bin_data, unsigned int size)
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

void* dump_binary(unsigned int* size)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif

    void* bin_data;

    if (size == NULL)
    {
        bin_data = malloc(util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements); 
        memcpy(bin_data,CURRENT_BUFFER->data_buffer,util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements);
    }
    else
    {
        *size = util_get_size(CURRENT_BUFFER) * CURRENT_BUFFER->num_elements;
        bin_data = malloc(*size); 
        memcpy(bin_data,CURRENT_BUFFER->data_buffer,*size);
    }

    return bin_data;
}

void load_binary(void* bin_data, unsigned int size)
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

buffer get_current_buffer()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif

    return CURRENT_BUFFER;
}

void reverse()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif

    unsigned int i, num_elements = CURRENT_BUFFER->num_elements;

    for (i = 0; i < num_elements; i++)
        swap_buffer_at(CURRENT_BUFFER,num_elements - i - 1, i);
}

void reverse_buffer(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif

    unsigned int i, num_elements = target->num_elements;
    for (i = 0; i < num_elements; i++)
        swap_buffer_at(target,num_elements - i - 1, i);
}

buffer recreate()
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif

    buffer recreation;
    recreation = malloc(sizeof(struct buffer));
    recreation->num_types = CURRENT_BUFFER->num_types;
    recreation->iterator = -1;
    recreation->types = malloc(sizeof(enum construct_types) * CURRENT_BUFFER->num_types);
    memcpy(recreation->types,CURRENT_BUFFER->types,sizeof(enum construct_types) * CURRENT_BUFFER->num_types);
    recreation->num_elements = 0;
    recreation->data_buffer = malloc(0);

    return recreation;
}


buffer recreate_buffer(buffer target)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif

    buffer recreation;
    recreation = malloc(sizeof(struct buffer));
    recreation->num_types = target->num_types;
    recreation->iterator = -1;
    recreation->types = malloc(sizeof(enum construct_types) * target->num_types);
    memcpy(recreation->types,target->types,sizeof(enum construct_types) * target->num_types);
    recreation->num_elements = 0;
    recreation->data_buffer = malloc(0);

    return recreation;
}

void sort_by_field(unsigned int more,unsigned int field, enum construct_types type)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    #endif

    int step,size = get_buffer_length(CURRENT_BUFFER);
    for (step = 0; step < size - 1; ++step)
    {
        int i,swapped = 0;
        for (i = 0; i < size - step - 1; ++i)
        {
            int condition = 0;

            if (more)
            {
                switch(type)
                {
                    case UINT:
                    condition = get_buffer_fieldui(CURRENT_BUFFER,i,field) > get_buffer_fieldui(CURRENT_BUFFER,i+1,field);
                    break;
                    case INT:
                    condition = get_buffer_fieldi(CURRENT_BUFFER,i,field) > get_buffer_fieldi(CURRENT_BUFFER,i+1,field);
                    break;
                    case CHAR:
                    condition = get_buffer_fieldc(CURRENT_BUFFER,i,field) > get_buffer_fieldc(CURRENT_BUFFER,i+1,field);
                    break;
                    case UCHAR:
                    condition = get_buffer_fielduc(CURRENT_BUFFER,i,field) > get_buffer_fielduc(CURRENT_BUFFER,i+1,field);
                    break;
                    case FLOAT:
                    condition = get_buffer_fieldf(CURRENT_BUFFER,i,field) > get_buffer_fieldf(CURRENT_BUFFER,i+1,field);
                    break;
                    case VOID:
                    condition = get_buffer_fieldv(CURRENT_BUFFER,i,field) > get_buffer_fieldv(CURRENT_BUFFER,i+1,field);
                    break;
                    default:
                    #ifdef ERROR_CHECKING
                    error_if(0,ERROR_INVALID_TYPE);
                    #endif
                    break;
                }
            }
            else
            {
                switch(type)
                {
                    case UINT:
                    condition = get_buffer_fieldui(CURRENT_BUFFER,i,field) < get_buffer_fieldui(CURRENT_BUFFER,i+1,field);
                    break;
                    case INT:
                    condition = get_buffer_fieldi(CURRENT_BUFFER,i,field) < get_buffer_fieldi(CURRENT_BUFFER,i+1,field);
                    break;
                    case CHAR:
                    condition = get_buffer_fieldc(CURRENT_BUFFER,i,field) < get_buffer_fieldc(CURRENT_BUFFER,i+1,field);
                    break;
                    case UCHAR:
                    condition = get_buffer_fielduc(CURRENT_BUFFER,i,field) < get_buffer_fielduc(CURRENT_BUFFER,i+1,field);
                    break;
                    case FLOAT:
                    condition = get_buffer_fieldf(CURRENT_BUFFER,i,field) < get_buffer_fieldf(CURRENT_BUFFER,i+1,field);
                    break;
                    case VOID:
                    condition = get_buffer_fieldv(CURRENT_BUFFER,i,field) < get_buffer_fieldv(CURRENT_BUFFER,i+1,field);
                    break;
                    default:
                    #ifdef ERROR_CHECKING
                    error_if(0,ERROR_INVALID_TYPE);
                    #endif
                    break;
                }
            }

            if (condition)
            {
                swap_buffer_at(CURRENT_BUFFER,i,i+1);
                swapped = 1;
            }
        }
        if (swapped == 0)
        {
            break;
        }
    }
}

void sort_buffer_by_field(buffer target,unsigned int more,unsigned int field,enum construct_types type)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    #endif

    int step,size = get_buffer_length(target);
    for (step = 0; step < size - 1; ++step)
    {
        int i,swapped = 0;
        for (i = 0; i < size - step - 1; ++i)
        {
            int condition = 0;

            if (more)
            {
                switch(type)
                {
                    case UINT:
                    condition = get_buffer_fieldui(target,i,field) > get_buffer_fieldui(target,i+1,field);
                    break;
                    case INT:
                    condition = get_buffer_fieldi(target,i,field) > get_buffer_fieldi(target,i+1,field);
                    break;
                    case CHAR:
                    condition = get_buffer_fieldc(target,i,field) > get_buffer_fieldc(target,i+1,field);
                    break;
                    case UCHAR:
                    condition = get_buffer_fielduc(target,i,field) > get_buffer_fielduc(target,i+1,field);
                    break;
                    case FLOAT:
                    condition = get_buffer_fieldf(target,i,field) > get_buffer_fieldf(target,i+1,field);
                    break;
                    case VOID:
                    condition = get_buffer_fieldv(target,i,field) > get_buffer_fieldv(target,i+1,field);
                    break;
                    default:
                    #ifdef ERROR_CHECKING
                    error_if(0,ERROR_INVALID_TYPE);
                    #endif
                    break;
                }
            }
            else
            {
                switch(type)
                {
                    case UINT:
                    condition = get_buffer_fieldui(target,i,field) < get_buffer_fieldui(target,i+1,field);
                    break;
                    case INT:
                    condition = get_buffer_fieldi(target,i,field) < get_buffer_fieldi(target,i+1,field);
                    break;
                    case CHAR:
                    condition = get_buffer_fieldc(target,i,field) < get_buffer_fieldc(target,i+1,field);
                    break;
                    case UCHAR:
                    condition = get_buffer_fielduc(target,i,field) < get_buffer_fielduc(target,i+1,field);
                    break;
                    case FLOAT:
                    condition = get_buffer_fieldf(target,i,field) < get_buffer_fieldf(target,i+1,field);
                    break;
                    case VOID:
                    condition = get_buffer_fieldv(target,i,field) < get_buffer_fieldv(target,i+1,field);
                    break;
                    default:
                    #ifdef ERROR_CHECKING
                    error_if(0,ERROR_INVALID_TYPE);
                    #endif
                    break;
                }
            }

            if (condition)
            {
                swap_buffer_at(target,i,i+1);
                swapped = 1;
            }
        }
        if (swapped == 0)
        {
            break;
        }
    }
}

buffer copy_partial(unsigned int startidx, unsigned int endidx)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(startidx > endidx,ERROR_INVALID_INDEX);
    error_if(endidx >= get_buffer_length(CURRENT_BUFFER),ERROR_OUT_OF_BOUNDS_INDEX);
    #endif

    buffer copy = malloc(sizeof(struct buffer));
    copy->iterator = CURRENT_BUFFER->iterator;

    unsigned int i, size = util_get_size(CURRENT_BUFFER);
    copy->num_types = CURRENT_BUFFER->num_types;
    copy->types = malloc(sizeof(enum construct_types) * copy->num_types);
    for (i = 0; i < CURRENT_BUFFER->num_types; i++)
        copy->types[i] = CURRENT_BUFFER->types[i];

    copy->data_buffer = malloc((endidx - startidx) * size);
    copy->num_elements = endidx - startidx;

    memcpy(copy->data_buffer,CURRENT_BUFFER->data_buffer + startidx * size,(endidx - startidx) * size);

    return copy;
}

buffer copy_partial_buffer(buffer target, unsigned int startidx, unsigned int endidx)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(startidx > endidx,ERROR_INVALID_INDEX);
    error_if(endidx >= get_buffer_length(target),ERROR_OUT_OF_BOUNDS_INDEX);
    #endif

    buffer copy = malloc(sizeof(struct buffer));
    copy->iterator = target->iterator;

    unsigned int i, size = util_get_size(target);
    copy->num_types = target->num_types;
    copy->types = malloc(sizeof(enum construct_types) * copy->num_types);
    for (i = 0; i < target->num_types; i++)
        copy->types[i] = target->types[i];

    copy->data_buffer = malloc((endidx - startidx) * size);
    copy->num_elements = endidx - startidx;

    memcpy(copy->data_buffer,target->data_buffer + startidx * size,(endidx - startidx) * size);

    return copy;
}

void replace_inside_buffer(buffer target, unsigned int idxsrc, unsigned int idxdest)
{
    #ifdef ERROR_CHECKING
    error_if(target == NULL,ERROR_BAD_BUFFER);
    error_if(idxsrc >= target->num_elements || idxdest >= target->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif

    unsigned int size = util_get_size(target);
    memcpy(target->data_buffer + size * idxdest,target->data_buffer + size * idxsrc,size);
}

void replace_inside(unsigned int idxsrc, unsigned int idxdest)
{
    #ifdef ERROR_CHECKING
    error_if(CURRENT_BUFFER == NULL,ERROR_NO_BOUND_BUFFER);
    error_if(idxsrc >= CURRENT_BUFFER->num_elements || idxdest >= CURRENT_BUFFER->num_elements,ERROR_OUT_OF_BOUNDS_INDEX);
    #endif

    unsigned int size = util_get_size(CURRENT_BUFFER);
    memcpy(CURRENT_BUFFER->data_buffer + size * idxdest,CURRENT_BUFFER->data_buffer + size * idxsrc,size);
}

buffer init_bufferve(unsigned int num_elements, unsigned int num_types, enum construct_types* types)
{
    #ifdef ERROR_CHECKING
    error_if(types == NULL,ERROR_BAD_TYPES);
    #endif

    buffer target;
    target = malloc(sizeof(struct buffer));
    target->num_types = num_types;
    target->iterator = -1;
    target->types = malloc(sizeof(enum construct_types) * num_types);
    memcpy(target->types,types,sizeof(enum construct_types) * num_types);

    unsigned int size = util_get_size(target);

    target->data_buffer = malloc(num_elements * size);
    target->num_elements = num_elements;

    return target;

}

buffer init_bufferva(unsigned int num_elements, unsigned int num_types, ...)
{
    va_list types;
    va_start(types,num_types);

    enum construct_types* buffer_types = malloc(sizeof(enum construct_types) * num_types);
    unsigned int i;
    for (i = 0; i < num_types; i++)
        buffer_types[i] = va_arg(types,enum construct_types);
    va_end(types);

    buffer target;
    target = malloc(sizeof(struct buffer));
    target->num_types = num_types;

    target->iterator = -1;

    #ifdef ERROR_CHECKING
    error_if(num_types == 0,ERROR_NO_PUSHED_TYPES);
    #endif

    target->types = buffer_types;

    
    unsigned int size = util_get_size(target);

    target->data_buffer = malloc(num_elements * size);
    target->num_elements = num_elements;

    return target;   
}

unsigned int* get_buffer_pointerui(buffer target, unsigned int element, unsigned int field)
{
    return (unsigned int*)(target->data_buffer + get_buffer_element_data_offset(target,element) + util_get_size_until(target,field));
}
unsigned int* get_pointerui(unsigned int field)
{
    return (unsigned int*)(CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,CURRENT_BUFFER->iterator) + util_get_size_until(CURRENT_BUFFER,field));
}

int* get_buffer_pointeri(buffer target, unsigned int element, unsigned int field)
{
    return (int*)(target->data_buffer + get_buffer_element_data_offset(target,element) + util_get_size_until(target,field));
}
int* get_pointeri(unsigned int field)
{
    return (int*)(CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,CURRENT_BUFFER->iterator) + util_get_size_until(CURRENT_BUFFER,field));
}

float* get_buffer_pointerf(buffer target, unsigned int element, unsigned int field)
{
    return (float*)(target->data_buffer + get_buffer_element_data_offset(target,element) + util_get_size_until(target,field));
}
float* get_pointerf(unsigned int field)
{
    return (float*)(CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,CURRENT_BUFFER->iterator) + util_get_size_until(CURRENT_BUFFER,field));
}

char* get_buffer_pointerc(buffer target, unsigned int element, unsigned int field)
{
    return (char*)(target->data_buffer + get_buffer_element_data_offset(target,element) + util_get_size_until(target,field));
}
char* get_pointerc(unsigned int field)
{
    return (char*)(CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,CURRENT_BUFFER->iterator) + util_get_size_until(CURRENT_BUFFER,field));
}

unsigned char* get_buffer_pointeruc(buffer target, unsigned int element, unsigned int field)
{
    return (unsigned char*)(target->data_buffer + get_buffer_element_data_offset(target,element) + util_get_size_until(target,field));
}
unsigned char* get_pointeruc(unsigned int field)
{
    return (unsigned char*)(CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,CURRENT_BUFFER->iterator) + util_get_size_until(CURRENT_BUFFER,field));
}

void** get_buffer_pointerv(buffer target, unsigned int element, unsigned int field)
{
    return (void**)(target->data_buffer + get_buffer_element_data_offset(target,element) + util_get_size_until(target,field));
}
void** get_pointerv(unsigned int field)
{
    return (void**)(CURRENT_BUFFER->data_buffer + get_buffer_element_data_offset(CURRENT_BUFFER,CURRENT_BUFFER->iterator) + util_get_size_until(CURRENT_BUFFER,field));
}