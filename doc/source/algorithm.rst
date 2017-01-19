Algorithms
=============

The algorithms take several components as inputs and also output the components. They may allocate memory during the excution, but when the function finished, there should be no extra memory allocated. The algorithms are usually implemented as static functions.


All-to-one Shortest path
------------------------

We implement the all-to-one shortest path algorithm by FIFO/LIFO/Dijstra algorithm, it is also based on the label correction algorithm.

.. cpp:function:: int static all_to_one_FIFO(TInt dest_node_ID, PNEGraph graph, const std::unordered_map<TInt, TFlt>& cost_map, std::unordered_map<TInt, TInt> &output_map)

The all to one FIFO implemention takes the *graph* as the base graph, *dest_nonde_ID* as the destination and *cost_map* as the cost for each link, the hashmap is coded by <link_ID, cost>. Finally it will output the <link_ID, next_link_ID> pair which can be formulated as a tree.

.. seealso::

  `Hybrid DTA <http://link.springer.com/article/10.1007/s11067-012-9177-z>`_


.. toctree::
   :maxdepth: 1


* `Home <index.html>`_
* :ref:`genindex`
* :ref:`search`