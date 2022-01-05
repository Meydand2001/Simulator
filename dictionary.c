#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Dictionary.h"


Element* allocate() {
	Element* e = (Element*)malloc(sizeof(Element));
	return e;
}

void destroy(Element* e) {
	free(e);
}

void init_element(Element* e, int num, char* label) {
	e->row_number = num;
	strcpy(e->line, label);
}

Dictionary* allocatedict() {
	Dictionary* d = (Dictionary*)malloc(sizeof(Dictionary));
	if (d == NULL) {
		return NULL;
	}
	return d;
}

void destroydict(Dictionary* dictionary) {
	free(dictionary->list);
	free(dictionary);
}
void init_dictionary(Dictionary* dictionary, int size) {
	dictionary->max = size;
	dictionary->number_of_elements = 0;
	dictionary->list = (Element*)malloc(size * sizeof(Element));
}

void add_element(Dictionary* dictionary, Element* element) {
	Element* list = dictionary->list;
	init_element(&list[dictionary->number_of_elements], element->row_number, element->line);
	dictionary->number_of_elements++;
}

int get_row(Dictionary* dictionary, char* label) {
	Element* list = dictionary->list;
	for (int i = 0; i < dictionary->number_of_elements; i++) {
		if (strcmp(list[i].line, label) == 0) {
			return list[i].row_number;
		}
	}
	return 0;
}

void get_line(Dictionary* dictionary, int row_number, char* line) {
	Element* list = dictionary->list;
	strcpy(line, list[row_number].line);
}
