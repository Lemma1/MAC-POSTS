Subcommands with main program arguments

In this test case, we test a parser that has its own arguments as well as
multiple subcommands, for which individual help sections should be generated.


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


Subcommands
~~~~~~~~~~~
  choose one of the following:

    =============    ==================================================================================
    **Argument**     **Description**                                                                   
    -------------    ----------------------------------------------------------------------------------
    ``foo``          Run the ``foo`` subprogram                                                        
    ``bar``          Take output from ``foo`` subprogram and run it through the ``bar`` subprogram     
    =============    ==================================================================================


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


------------


``bar`` subcommand
__________________
This is the long description for the ``bar`` subprogram.


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

    =================    ===========================================================
    **Argument**         **Description**                                            
    -----------------    -----------------------------------------------------------
    ``--b1  B1``                                                                    
    ``--b2  B2``         Argument 2 has help (bar argument 1 did not have help)     
    ``-k  N N``          Some other argument                                        
    =================    ===========================================================


------------


Script contents
---------------
