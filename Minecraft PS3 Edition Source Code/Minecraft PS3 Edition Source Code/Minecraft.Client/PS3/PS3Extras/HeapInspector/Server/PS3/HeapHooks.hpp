#ifndef _HEAPINSPECTORSERVER_PS3_HEAPHOOKS_HPP_
#define _HEAPINSPECTORSERVER_PS3_HEAPHOOKS_HPP_

#include <stdlib.h>
extern "C" void* __real_memalign(size_t a_Boundary, size_t a_Size);
extern "C" void* __wrap_memalign(size_t a_Boundary, size_t a_Size)
{
	HeapInspectorServer::Mutation mutation = HeapInspectorServer::BeginAlloc();
	void* result = __real_memalign(a_Boundary, a_Size);
	size_t usableSize = malloc_usable_size(result); 
	HeapInspectorServer::EndAlloc(mutation, 0, result, a_Size, usableSize);
	return result;
}

extern "C" void* __real_calloc(size_t a_NumElements, size_t a_SizePerElement);
extern "C" void* __wrap_calloc(size_t a_NumElements, size_t a_SizePerElement)
{
	HeapInspectorServer::Mutation mutation = HeapInspectorServer::BeginAlloc();
	void* result = __real_calloc(a_NumElements, a_SizePerElement);
	size_t usableSize = malloc_usable_size(result); 
	HeapInspectorServer::EndAlloc(mutation, 0, result, a_NumElements * a_SizePerElement, usableSize);
	return result;
}

extern "C" void* __real_malloc(size_t a_Size);
extern "C" void* __wrap_malloc(size_t a_Size) 
{
	HeapInspectorServer::Mutation mutation = HeapInspectorServer::BeginAlloc();
	void* result = __real_malloc(a_Size);
	size_t usableSize = malloc_usable_size(result); 
	HeapInspectorServer::EndAlloc(mutation, 0, result, a_Size, usableSize);
	return result;
}

extern "C" void __real_free(void* a_Address);
extern "C" void __wrap_free(void* a_Address) 
{
	HeapInspectorServer::Mutation mutation = HeapInspectorServer::BeginFree();
	__real_free(a_Address);
	HeapInspectorServer::EndFree(mutation, 0, a_Address);
}

extern "C" void *__real_realloc(void *a_Address, size_t a_Size);
extern "C" void *__wrap_realloc(void *a_Address, size_t a_Size)
{
	HeapInspectorServer::Mutation mutation = HeapInspectorServer::BeginReAlloc();
	void* result = __real_realloc(a_Address, a_Size);
	size_t usableSize = malloc_usable_size(result); 
	HeapInspectorServer::EndReAlloc(mutation, 0, a_Address, result, a_Size, usableSize);
	return result;
}

extern "C" void* __real_reallocalign(void *a_Address, size_t a_Size, size_t a_Boundary);
extern "C" void* __wrap_reallocalign(void *a_Address, size_t a_Size, size_t a_Boundary)
{
	HeapInspectorServer::Mutation mutation = HeapInspectorServer::BeginReAlloc();
	void* result = __real_reallocalign(a_Address, a_Size, a_Boundary);
	size_t usableSize = malloc_usable_size(result); 
	HeapInspectorServer::EndReAlloc(mutation, 0, a_Address, result, a_Size, usableSize);
	return result;
}

#endif //_HEAPINSPECTORSERVER_PS3_HEAPHOOKS_HPP_