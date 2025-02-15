#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void reverse_elem(void *place, void *data)
{
	array_t *new_list = (array_t *)place;
	new_list->len--;
	memcpy(new_list->data + new_list->len * new_list->elem_size, data,
		   new_list->elem_size);
}

array_t reverse(array_t list) {
	array_t new_list;
	new_list.destructor = list.destructor;
	new_list.elem_size = list.elem_size;
	new_list.len = list.len;
	new_list.data = malloc(new_list.len * new_list.elem_size);

	reduce(reverse_elem, &new_list, list);
	new_list.len = list.len;

	return new_list;
}

void convert_to_number(void *result, void **args) {
	int *integer_part = (int *)args[0];
	int *fractional_part = (int *)args[1];
	number_t *number = (number_t *)result;

	number->string = (char *)malloc(15 * sizeof(char));
	sprintf(number->string, "%d.%d", *integer_part, *fractional_part);

	number->integer_part = *integer_part;
	number->fractional_part = *fractional_part;
}

void destroy_number(void *ptr) {
	number_t *number = (number_t *)ptr;
	free(number->string);
}

array_t create_number_array(array_t integer_part, array_t fractional_part) {
	return map_multiple(convert_to_number, sizeof(number_t),
					    destroy_number, 2, integer_part, fractional_part);
}

boolean passing_grade(void *item) {
	student_t *student = (student_t *)item;
	return student->grade >= 5.0;
}

void extract_name(void *result, void *item) {
	student_t *student = (student_t *)item;
	char **name = (char **)result;
	*name = malloc(100);
	strcpy(*name, student->name);
}

void free_name(void *item) {
	char *name = *(char **)item;
	free(name);
}

array_t get_passing_students_names(array_t list) {
	array_t passing_students = filter(passing_grade, list);
	array_t names = map(extract_name, sizeof(char *),
					    free_name, passing_students);
	return names;
}

array_t check_bigger_sum(array_t list_list, array_t int_list) {
	(void)list_list;
	(void)int_list;
	return (array_t){0};
}

void add_string_to_even_index(void *place, void *data) {
	array_t *new_list = (array_t *)place;
	int index = new_list->len;
	if (index % 2 == 0) {
		memcpy(new_list->data + index / 2 * new_list->elem_size,
			   data, sizeof(new_list->data));
	} else {
		new_list->destructor(data);
	}
	new_list->len++;
}

array_t get_even_indexed_strings(array_t list) {
	array_t even_strings;
	even_strings.elem_size = list.elem_size;
	even_strings.destructor = list.destructor;
	even_strings.len = (list.len + 1) / 2;
	even_strings.data = malloc(even_strings.len * list.elem_size);
	even_strings.len = 0;
	reduce(add_string_to_even_index, &even_strings, list);
	even_strings.len = (list.len + 1) / 2;
	free(list.data);

	return even_strings;
}

array_t generate_square_matrix(int n) {
	(void)n;
	return (array_t){0};
}
