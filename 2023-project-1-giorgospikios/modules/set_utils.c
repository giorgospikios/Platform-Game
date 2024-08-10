#include <stddef.h>
#include "set_utils.h"



// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μικρότερη τιμή του set που είναι μεγαλύτερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.

Pointer set_find_eq_or_greater(Set set, Pointer value)
{
	Pointer find = set_find(set, value);
	if(find != NULL)
		return find;
	else{
		set_insert(set, value);

		SetNode current_node = set_find_node(set, value);
		SetNode requested_node = set_next(set, current_node);

		if(requested_node != SET_EOF){
			find = set_node_value(set, requested_node);
			set_remove(set, value);
			return find;
		}
		else
			return NULL;
	}
}

// Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value,
// ή αν δεν υπάρχει, την μεγαλύτερη τιμή του set που είναι μικρότερη
// από value. Αν δεν υπάρχει καμία τότε επιστρέφει NULL.

Pointer set_find_eq_or_smaller(Set set, Pointer value)
{
	Pointer find = set_find(set, value);
	if(find != NULL)
		return find;
	else{
		set_insert(set, value);

		SetNode current_node = set_find_node(set, value);
		SetNode requested_node = set_previous(set, current_node);

		if(requested_node != SET_BOF){
			find = set_node_value(set, requested_node);
			set_remove(set, value);
			return find;
		}
		else
			return NULL;
	}
}
