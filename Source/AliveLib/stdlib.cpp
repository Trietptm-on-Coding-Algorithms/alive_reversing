#include "stdafx.h"
#include "stdlib.hpp"
#include "Function.hpp"
#include "Error.hpp"

// stdlib proxys
EXPORT void CC free_521334(void* ptr)
{
    NOT_IMPLEMENTED();
    ::free(ptr);
}

EXPORT void* CC malloc_5212C0(size_t size)
{
    NOT_IMPLEMENTED();
    return ::malloc(size);
}

EXPORT void* CC realloc_522335(void* ptr, size_t size)
{
    NOT_IMPLEMENTED();
    return ::realloc(ptr, size);
}

// Game specific stdlib wrappers
EXPORT void* CC malloc_4F4E60(size_t size)
{
    if (size > 0)
    {
        return malloc_5212C0(size);
    }

    ALIVE_FATAL("0 bytes allocated");
}

EXPORT void* CC malloc_4954D0(size_t size) // Probably operator new
{
    return malloc_5212C0(size);
}

EXPORT void* CC malloc_non_zero_4954F0(size_t size)
{
    if (size == 0)
    {
        size = 1;
    }
    return malloc_5212C0(size);
}

EXPORT void CC mem_free_4F4EA0(void* ptr)
{
    if (ptr)
    {
        free_521334(ptr);
    }
    else
    {
        Error_NullPrint_4F28D0("mem_free: invalid pointer ( 0 )\n");
    }
}

EXPORT void CC Mem_Free_495540(void* ptr)
{
    free_521334(ptr);
}

EXPORT void CC Mem_Free_495560(void *ptr)
{
    free_521334(ptr);
}

EXPORT void* CC realloc_4F4E80(void* ptr, size_t size)
{
    return realloc_522335(ptr, size);
}
