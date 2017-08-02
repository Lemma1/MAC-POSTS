Components
===================


Introduction
------------

There are several components implemented, which form the essential parts of each traffic model in MAC-POSTS. The components usually have a base class, and the acutal implementation will inherit the base class, thus ensuring the uniformness of the interface. Here is the list we implemented:

.. hlist::
  :columns: 1

  * Vehicle
  * Node
  * Link
  * Routing



Vehicle
------------
.. cpp:class:: MNM_Veh 

Vehicle class represents the vehicle in real world, but it can be 1/100 of one actual vehicle, therefore it is able to simulate the continuous vehicle flow in flow theory.

Current the vehicle class has in inheritance since we only implement the standard vehicle, the truck/bus will be implemented shortly. We describe the member variables in vehicle class as follows:

.. cpp:member:: Vehicle_type m_type
.. cpp:member:: MNM_Dlink *m_current_link

 specify the current link of vehicle, initialized as NULL, will be the pointer to the dlink object.

.. cpp:member:: TInt m_start_time

specify the realse time of the vehicle.

.. cpp:member:: TInt m_finish_time

specify the finish time of the vehicle, if not finished then set it as -1.

.. cpp:member:: MNM_Dlink *m_next_link

determined by the routing model and used by the node model.

.. cpp:member:: MNM_Destination *m_dest

the destination of the vehicle, determined when the vehicle is released.

.. cpp:member:: MNM_Origin *m_origin

the destination of the vehicle, determined when the vehicle is released.


Node
----------
.. cpp:class:: MNM_Dnode 

The node model will manage the vehicle movement, the node model is only responsible for the vehicles' merging/diverging behaviors, the path of each vehicle is determined by the routing model.

.. cpp:member:: TInt m_node_ID

  node ID is used to identify the node.

.. cpp:member:: std::vector<MNM_Dlink *> m_out_link_array

    contains the pointers for the in links, determined when hooking up the node and link.

.. cpp:member:: std::vector<MNM_Dlink *> m_in_link_array

   contains the pointers for the out links, determined when hooking up the node and link.

.. cpp:member:: TFlt m_flow_scalar

  specify the quantity of one vehicle object, 2 means two :cpp:class:`MNM_Veh` objects equal one actual vehicle.



Link
----------
.. cpp:class:: MNM_Dlink


.. cpp:member:: TInt m_link_ID

  link ID is used to identify the link

.. cpp:member:: MNM_Dnode *m_from_node

  the tail of the directed link, determined when hooking up

.. cpp:member:: MNM_Dnode *m_to_node

  the head of the directed link, determined when hooking up

.. cpp:member:: TInt m_number_of_lane

  number of the lane in one direction

.. cpp:member:: TFlt m_length

  lenght of the link, unit: meter

.. cpp:member:: TFlt m_ffs

  free flow speed, unit: meter/second

.. cpp:member:: std::deque<MNM_Veh *> m_finished_array

  after the loading, the finished vehicle will be pushed in the the finished array, and node model will take care of putting them to next link.

.. cpp:member:: std::deque<MNM_Veh *> m_incoming_array

  before the loading, the incoming vehicles are stored here, and we need to clear the incoming array before loading.


If you want to see more detailed information for each implementation.

.. toctree::
   :maxdepth: 2

   comp_link

Routing
----------
.. cpp:class:: MNM_Routing

The routing class manages the paths of vehicles, it is the crutial part of the dynamic traffic assignment model. In our implementation, the routing model will take the network statistics as input and compute the routing for *all finished vehicles on each link*. Therefore the routing take place after the link loading but bofore the node loading.




.. toctree::
   :maxdepth: 1



* `Home <index.html>`_
* :ref:`genindex`
* :ref:`search`