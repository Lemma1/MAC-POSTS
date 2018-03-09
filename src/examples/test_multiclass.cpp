#include "Snap.h"
#include "dlink.h"
#include "vehicle.h"
#include "dnode.h"
#include "od.h"
#include "factory.h"
#include "multiclass.h"

#include <deque>
#include <iostream>

int main()
{
	std::cout << "BEGIN multiclass TEST!\n";

	std::cout << "Add a new destination\n";
	MNM_Destination *d = new MNM_Destination(1);
	std::cout << "Add a new origin\n";
	MNM_Origin *o = new MNM_Origin(1, 1, 5, 1);
	d -> m_Dest_ID = 11;

	std::cout << "Add a new link\n";
	MNM_Dlink *l = new MNM_Dlink_Ctm_Multiclass(0, 2, 500, 0.125, 0.611, 0.125, 0.611, 13.33, 13.33, 5, 3, 1);
	l -> print_info();

	std::cout << "Add a new origin node and hook up origin\n";
	MNM_DMOND *o_node = new MNM_DMOND(1, 5);
	o_node -> hook_up_origin(o);
	o_node -> evolve(4);

	std::cout << "Add a new destination node and hook up destination\n";
	MNM_DMDND *d_node = new MNM_DMDND(1, 5);
	d_node -> hook_up_destination(d);
	d_node -> evolve(4);

	std::cout << "Loop build link\n";
	MNM_Veh *v;
	for (int i = 0; i < 4; ++i) {
		v = new MNM_Veh(i, 0);
		v -> set_current_link(l);
		v -> set_destination(d);
		l -> m_incoming_array.push_back(v);
	}

	std::cout << "Test ctm evolve\n";
	l -> clear_incoming_array();
	for (int i = 0 ; i < 2; ++i) {
		l -> evolve(i);
		l -> print_info();
		for (int j = 0; j < 4; ++j) {
			v = new MNM_Veh(j, i);
			v -> set_current_link(l);
			v -> set_destination(d);
			l -> m_incoming_array.push_back(v);
		}
		l -> clear_incoming_array();
	}
	std::cout << "End test\n";

	return 0;
}