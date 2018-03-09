Subcommands, each having its own epilog

An epilog should appear at the bottom of each command-group section


------------


Command-line arguments
----------------------

Positional arguments
~~~~~~~~~~~~~~~~~~~~

    =================    ================================================================================
    **Argument**         **Description**                                                                 
    -----------------    --------------------------------------------------------------------------------
    ``mainarg1``                                                                                         
    ``mainarg2``         main positional argument #2, which has a description (``mainarg1`` did not)     
    =================    ================================================================================


Optional arguments
~~~~~~~~~~~~~~~~~~

    =======================    ====================================
    **Argument**               **Description**                     
    -----------------------    ------------------------------------
    ``-h``, ``--help``         show this help message and exit     
    =======================    ====================================


Subcommands
~~~~~~~~~~~
  choose one of the following:

    =============    ==================================================================================
    **Argument**     **Description**                                                                   
    -------------    ----------------------------------------------------------------------------------
    ``foo``          Run the ``foo`` subprogram                                                        
    ``bar``          Take output from ``foo`` subprogram and run it through the ``bar`` subprogram     
    =============    ==================================================================================

This is a multi-line epilog which should appear at the bottom of the module
docstring and also follow all of the options, arguments, et cetera.

------------


``foo`` subcommand
__________________
This is a long description of what a ``foo`` program might do. It spans
multiple lines, so that we can test things reasonably.


Positional arguments
""""""""""""""""""""

    ================    ===================
    **Argument**        **Description**    
    ----------------    -------------------
    ``fooarg1``         foo argument 1     
    ``fooarg2``         foo argument 2     
    ================    ===================


Optional arguments
""""""""""""""""""

    ============================================    ====================================
    **Argument**                                    **Description**                     
    --------------------------------------------    ------------------------------------
    ``-h``, ``--help``                              show this help message and exit     
    ``-f  F``                                       short foo argument                  
    ``--fookwarg  FOOKWARG``                        foo keyword argument                
    ``-v  VERBOSE``, ``--verbose  VERBOSE``          foo verbosely                      
    ============================================    ====================================

This is the epilog for the ``foo`` subprogram. It should appear at the end of
the ``foo`` subcommand section.

------------


``bar`` subcommand
__________________
This is the long description for the ``bar`` subcommand.


Positional arguments
""""""""""""""""""""

    ===============    =================
    **Argument**       **Description**  
    ---------------    -----------------
    ``bararg``         bar argument     
    ===============    =================


Optional arguments
""""""""""""""""""

    ============================================    ======================================
    **Argument**                                    **Description**                       
    --------------------------------------------    --------------------------------------
    ``-h``, ``--help``                              show this help message and exit       
    ``--choice  {option1,option2,option3}``          A keyword that requries a choice     
    ============================================    ======================================


An argument group
"""""""""""""""""
  A special goup of arguments in the ``bar`` subparser

    =================    ========================
    **Argument**         **Description**         
    -----------------    ------------------------
    ``--b1  B1``                                 
    ``--b2  B2``         Argument 2 has help     
    ``-k  N N``          Some other argument     
    =================    ========================

This the epilog for the ``bar`` subcommand. It is short.

------------


Script contents
---------------
