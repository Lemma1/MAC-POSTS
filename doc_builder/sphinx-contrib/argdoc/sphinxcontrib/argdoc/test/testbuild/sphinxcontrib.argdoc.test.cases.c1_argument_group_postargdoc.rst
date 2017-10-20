Argument groups with short, long, or no descriptions

:class:`~argparse.ArgumentParser` allows organization of arguments into 
*argument groups*, which can make things more intelligible for users.
:obj:`sphinxcontrib.argdoc` styles argument groups as separate, paragraph-level sections,
with their descriptions (if present) appearing below the title, followed by
the arguments in each group, formatted as a table. Examples appear here below.


------------


Command-line arguments
----------------------

Positional arguments
~~~~~~~~~~~~~~~~~~~~

    =================    ================================
    **Argument**         **Description**                 
    -----------------    --------------------------------
    ``mainarg1``                                         
    ``mainarg2``         main positional argument #2     
    =================    ================================


Optional arguments
~~~~~~~~~~~~~~~~~~

    =======================    ====================================
    **Argument**               **Description**                     
    -----------------------    ------------------------------------
    ``-h``, ``--help``         show this help message and exit     
    =======================    ====================================


One group of arguments
~~~~~~~~~~~~~~~~~~~~~~
  Sometimes it is useful to group arguments that relate to each other in an
  argument group. This can make command-line help, documentation, and source
  code more intelligible to others

    ============================================    =========================
    **Argument**                                    **Description**          
    --------------------------------------------    -------------------------
    ``fooarg1``                                     foo argument 1           
    ``fooarg2``                                     foo argument 2           
    ``-f  F``                                       short foo argument       
    ``--fookwarg  FOOKWARG``                        foo keyword argument     
    ``-v  VERBOSE``, ``--verbose  VERBOSE``          foo verbosely           
    ============================================    =========================


A second group of arguments
~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Description of second argument group

    ============================================    ======================================
    **Argument**                                    **Description**                       
    --------------------------------------------    --------------------------------------
    ``bararg``                                      bar argument                          
    ``--choice  {option1,option2,option3}``          A keyword that requries a choice     
    ============================================    ======================================


A final group of arguments, with no description
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    ===================================    =======================
    **Argument**                           **Description**        
    -----------------------------------    -----------------------
    ``bazarg``                             baz argument           
    ``--numbers  M M M``                   numerical argument     
    ``-z  ZOOM``, ``--zoom  ZOOM``         zzzzzzzzzzzzzz         
    ===================================    =======================


------------


Script contents
---------------
