#include "Snap.h"
#include "dlink.h"
#include "vehicle.h"
#include "dnode.h"
#include "od.h"
#include "factory.h"

#include <deque>
#include <iostream>

int main()
{
	std::cout << "BEGIN multiclass TEST!\n";

	MNM_Destination *d = new MNM_Destination(1);
	MNM_Origin *o = new MNM_Origin(1, 1, 5, 1);
	d -> m_Dest_ID = 11;

	MNM_Dlink *l = new MNM_Dlink_Ctm(0, 0.125, 0.611, 2, 500, 13.33, 5, 1);
	l -> print_info();

	MNM_DMOND *o_node = new MNM_DMOND(1, 5);
	o_node -> hook_up_origin(o);
	o_node -> evolve(4);

	MNM_DMDND *d_node = new MNM_DMDND(1, 5);
	d_node -> hook_up_destination(d);
	d_node -> evolve(4);

	MNM_Veh *v;
	for (int i = 0; i < 4; ++i) {
		v = new MNM_Veh(i, 0);
		v -> set_current_link(l);
		v -> set_destination(d);
		l -> m_incoming_array.push_back(v);
	}

	l -> clear_incoming_array();
	for (int i =0; i < 2; ++i) {
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

	return 0;
}