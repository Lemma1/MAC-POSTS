.. wiki documentation master file, created by
   sphinx-quickstart on Thu Jul 14 17:01:20 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

MAC-POSTS
==========


Note For Users:
-----------------
This domain has been reserved by Mobility Data Analytics Center at Carnegie Mellon University for further development. Please check back later.

Introduction
------------
      MAC-POSTS is the abbreviation for *Mobility Data Analytics Center - Prediction, Optimization, and Simulation toolkit for Transportation Systems*. It is a mesoscopic traffic simulation library for dynamic network loading. This library is interface based and is friendly to new node/link/routing models.

      The library mainly consists of three parts. One part is called component, which is the separated model such as dynamic node/link model, real-time routing model and so on. Each component has clear interfaces with other components, so we can implement each components with different actual models. Here are the components that we already implemented:

      

      .. hlist::
         :columns: 1

         * Node
         * Link
         * Routing

      The second part is algorithm, the algorithms take the components as input and output corresponding components. The difference between component and algorithm is that usually component comsume memory and algorithm does not (when the algorithm finishes). Here are the algorithms that we already implemented:

      .. hlist::
         :columns: 1

         * All-to-one shortest path
         * Computing SO marginal cost



      By combining different components and algorithms, we can develop complex transportation models. The types of models may vary a lot, and here are the models we implemented:

      .. hlist::
         :columns: 1

         * Dynamic traffic loading
         * Real-time DTA     

Documentation
--------------------------

We discuss components, algorithms and models in detail, we also provide basic instrutions on how to install the package.

.. toctree::
   :maxdepth: 1

   start
   component
   algorithm
   model
   extension
   utls

People
--------------------------

.. glossary::
   Advisor:
      * Zhen (Sean) Qian

   Contributors:
      * Wei Ma
      * Xidong Pi





Indices and tables
==================

* `Home <index.html>`_
* :ref:`genindex`
* :ref:`search`
