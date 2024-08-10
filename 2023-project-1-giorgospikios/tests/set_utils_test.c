//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "set_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "state.h"

int compare_objs(Pointer a, Pointer b)
{
	Object obj_a = a;
	Object obj_b = b;
	
	float substruction_result = obj_a->rect.x - obj_b->rect.x;

	if(substruction_result > 0)
		return 1;
	else if(substruction_result < 0)
		return -1;
	else 
		return 0;
}

Object create_object(float value) {
	Object in_create_obj = malloc(sizeof(in_create_obj));
	in_create_obj->rect.x = value;

	return in_create_obj;
}

Set create_my_set()
{

	Set in_func_my_set = set_create(compare_objs, free);

	printf("\n");
	Object obj1 = create_object(1.5);
	set_insert(in_func_my_set, obj1);

	Object obj2 = create_object(3.5);
	set_insert(in_func_my_set, obj2);

	Object obj3 = create_object(6.55);
	set_insert(in_func_my_set, obj3);

	Object obj4 = create_object(7.15);
	set_insert(in_func_my_set, obj4);


	return in_func_my_set;

}

void test_set_utils()
{
	Set my_set = create_my_set();

	printf("\nset size = %d\n", set_size(my_set));



	Object obj_to_find = create_object(10.5);



	Object object_returned_from_func; //= set_find_eq_or_greater(my_set, obj_to_find);


	//printf("\n\n (set_find_eq_or_greater) object_returned_from_func->rect.x = %f\n\n", object_returned_from_func->rect.x);
	//TEST_ASSERT(object_returned_from_func->rect.x == 6.55);

	object_returned_from_func = set_find_eq_or_smaller(my_set, obj_to_find);
	printf("\n\n (set_find_eq_or_smaller) object_returned_from_func->rect.x = %f \n\n", object_returned_from_func->rect.x);


	TEST_ASSERT(object_returned_from_func->rect.x == 7.15);



}



// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_set_utils", test_set_utils },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};