#ifndef DICTIONARY_H
#define DICTIONARY_H

typedef struct {
	int row_number;
	char line[200];
}Element;

typedef struct {
	int number_of_elements;
	int max;
	Element* list;
}Dictionary;

Element* allocate();
void destroy(Element* e);
void init_element(Element* e, int row, char* label);

Dictionary* allocatedict();
void destroydict(Dictionary* dictionary);
void init_dictionary(Dictionary* dictionary, int size);
void add_element(Dictionary* dictionary, Element* element);
int get_row(Dictionary* dictionary, char* label);
void get_line(Dictionary* dictionary, int row_number, char* line);

#endif 
