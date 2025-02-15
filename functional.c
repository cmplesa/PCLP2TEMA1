#include "functional.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void for_each(void (*func)(void *), array_t list)
{
	char *ptr = (char *)list.data;
	for (int i = 0; i < list.len; i++)
		func(ptr + (i * list.elem_size));
}

array_t map(void (*func)(void *, void *),
			int new_list_elem_size,
			void (*new_list_destructor)(void *),
			array_t list)
{
	array_t newlist;
	newlist.len = list.len;
	newlist.elem_size = new_list_elem_size;
	newlist.data = malloc(new_list_elem_size * list.len);
	newlist.destructor = new_list_destructor;

	for (int i = 0; i < list.len; ++i) {
		func(newlist.data + i * new_list_elem_size,
			 list.data + i * list.elem_size);
	}

	if (list.destructor) {
		for (int i = 0; i < list.len; ++i)
			list.destructor(list.data + i * list.elem_size);
	}
	free(list.data);
	return newlist;
}

array_t filter(boolean(*func)(void *), array_t list)
{
	array_t new_list;
	new_list.elem_size = list.elem_size;
	new_list.len = 0;
	new_list.data = malloc(list.elem_size * list.len);
	new_list.destructor = list.destructor;

	for (int i = 0; i < list.len; ++i) {
		if (func(list.data + i * list.elem_size)) {
			memcpy(new_list.data + new_list.len * new_list.elem_size,
				   list.data + i * list.elem_size,
				   new_list.elem_size);
			new_list.len++;
		}
	}
	free(list.data);
	return new_list;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list)
{
	char *current_element = (char *)list.data;
	for (int i = 0; i < list.len; ++i) {
		func(acc, current_element);
		current_element += list.elem_size;
	}
	return acc;
}

void for_each_multiple(void(*func)(void **), int varg_c, ...)
{
	va_list args;
	va_start(args, varg_c);
	array_t *arrays = malloc(varg_c * sizeof(array_t));
	int min_len = 10000;
	for (int i = 0; i < varg_c; ++i) {
		arrays[i] = va_arg(args, array_t);
		if (arrays[i].len < min_len)
			min_len = arrays[i].len;
	}

	void **current_elements = (void **)calloc(varg_c, sizeof(void *));
	for (int i = 0; i < varg_c; i++)
		current_elements[i] = calloc(1, arrays[0].elem_size);

	for (int i = 0; i < min_len; ++i) {
		for (int j = 0; j < varg_c; ++j) {
			memcpy(current_elements[j], arrays[j].data +
				   (i * arrays[j].elem_size), arrays[j].elem_size);
		}
		func(current_elements);
	}

	for (int i = 0; i < varg_c; i++)
		free(current_elements[i]);
	free(current_elements);
	free(arrays);
}

array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...)
{
	va_list args;
	va_start(args, varg_c);

	array_t *arrays = malloc(varg_c * sizeof(array_t));
	int min_len = 1000000;
	for (int i = 0; i < varg_c; i++) {
		arrays[i] = va_arg(args, array_t);
		if (arrays[i].len < min_len)
			min_len = arrays[i].len;
	}

	array_t new_list;
	new_list.data = malloc(new_list_elem_size * min_len);
	new_list.elem_size = new_list_elem_size;
	new_list.len = min_len;
	new_list.destructor = new_list_destructor;

	void **elements = malloc(varg_c * sizeof(void *));
	for (int i = 0; i < varg_c; i++)
		elements[i] = malloc(arrays[i].elem_size);

	for (int i = 0; i < min_len; i++) {
		for (int j = 0; j < varg_c; j++) {
			memcpy(elements[j], arrays[j].data +
				   (i * arrays[j].elem_size), arrays[j].elem_size);
		}
		func(new_list.data + i * new_list_elem_size, elements);
	}

	for (int i = 0; i < varg_c; i++) {
		if (arrays[i].destructor)
			for_each(arrays[i].destructor, arrays[i]);
		free(arrays[i].data);
		free(elements[i]);
	}
	free(elements);
	free(arrays);
	return new_list;
}

void *reduce_multiple(void(*func)(void *, void **), void *acc, int varg_c, ...)
{
	va_list args;
	va_start(args, varg_c);
	array_t *arrays = malloc(varg_c * sizeof(array_t));
	int min_len = 100000;
	for (int i = 0; i < varg_c; i++) {
		arrays[i] = va_arg(args, array_t);
		if (arrays[i].len < min_len)
			min_len = arrays[i].len;
	}

	void **data = malloc(varg_c * sizeof(void *));
	for (int i = 0; i < varg_c; i++)
		data[i] = malloc(arrays[0].elem_size);

	for (int i = 0; i < min_len; i++) {
		for (int j = 0; j < varg_c; j++) {
			memcpy(data[j], arrays[j].data +
				   (i * arrays[j].elem_size), arrays[j].elem_size);
		}
		func(acc, data);
	}

	for (int i = 0; i < varg_c; i++)
		free(data[i]);
	free(data);
	free(arrays);
	return acc;
}
