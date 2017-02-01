Dynamic Link
===================


Introduction
------------

To really implement the dynamic link model, we need to clearly specify the interface between node and link. In the implementation we define the interface as follows:

  * Nodes pass the vehicles to the incoming_array, the size of incoming_array should always be smaller than the supply of the link. The pushing in of incoming_array is managed by node, but the popping up is due to link.
  * Link should clear incoming_array before real loading.
  * Loading behaviors are different from different models, we will specify separately.
  * The finished vehicle are stored in finished_array, note that the demand of the link is represent by the finishend_array, and node is in charge of extracting the information.

Here's a list of what we implemented.

  * PQ
  * CTM
  * LQ
  * LTM



Point Queue (PQ)
-----------------

Point queue is the most simplified model which has a bottle neck at the end of the link. It only controls that capacity of the link, but infinitely many vehicles can be stored in one link. Capacity is the only reason for PQ to have congestion, the density is not the reason, this is quite from typical LWR model.

The implementation scheme for PQ is: compute the maximum time stamp for each link, the time stamp represents the number of unit time required to passing by the link. Then at each time stamp, we check if the vehicles reach the time stamp, if yes then push it to the finished array, if not then add the time stamp by 1.

The supply is simply the capacity of the link, the link can hold infinitely many vehicles.

.. seealso::

  `Modelling network flow with and without link interactions: the cases of point queue, spatial queue and cell transmission model <http://www.tandfonline.com/doi/full/10.1080/21680566.2013.785921>`_

Cell Transmission Model
------------------------
Different from PQ, CTM is able to model the shock wave evolution.


.. seealso::

  `The cell transmission model: A dynamic representation of highway traffic consistent with the hydrodynamic theory <http://www.sciencedirect.com/science/article/pii/0191261594900027>`_

Link Queue Model
------------------------
.. seealso::
  
  `A link queue model of network traffic flow <https://arxiv.org/abs/1209.2361>`_

Link Transmission Model
-------------------------

.. seealso::
  
  `The Link Transmission Model for Dynamic Network Loading <http://www.mech.kuleuven.be/cib/verkeer/dwn/pub/P2007A.pdf>`_
  
.. toctree::
   :maxdepth: 1



* `Home <index.html>`_
* :ref:`genindex`
* :ref:`search`