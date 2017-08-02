Models
=========

Model is the highest level of the library, it can be totally different for different purpose. It is usually the combinations of components and algorithms, and it is usualy based papers or project.



Boston dynamic traffic assignment
-----------------------------------

In Boston DTA, the route choice of each traveler is based on the real time traffic infomation. The route of each vehicle is updated every one update interval, and each vehicle will follow the shortest path based on the current traffic network condition. The routing is not predictive, and the historical network condition can be determined differently. 


TO DO: we havn't implment the hybrid routing, but will do shortly.


.. note::

  `Hybrid DTA <http://link.springer.com/article/10.1007/s11067-012-9177-z>`_


Online dynamic traffic assignment
----------------------------------

The online DTA is due to the project we conduct. It takes the real-time speed feed and update the travelers' route choice. Also it will manage the Variable Message Signs (VMS) for the I-95 corridor.

.. note::

  `Dynamic Network Analysis and Real-time Traffic Management for Philadelphia Metropolitan Area <http://www.weima171.com/docs/WO004_final.pdf>`_


.. toctree::
   :maxdepth: 1


* `Home <index.html>`_
* :ref:`genindex`
* :ref:`search`