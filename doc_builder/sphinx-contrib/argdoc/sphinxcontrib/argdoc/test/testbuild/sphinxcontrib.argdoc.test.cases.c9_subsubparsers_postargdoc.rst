Subcommands of subcommands

In this test case, we test a parser that has its own arguments as well as
multiple subcommands, which have their own subcommands that must be
included as subsections within them. Here, the ``foo`` subcommand has
subcommands, but the ``bar`` subcommand does not.


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
    ``foo``          Run the ``foo`` subcommand                                                        
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


Subcommands of the ``foo`` subcommand
"""""""""""""""""""""""""""""""""""""
  this is indeed unusual

    ================    =============================================================
    **Argument**        **Description**                                              
    ----------------    -------------------------------------------------------------
    ``subfoo1``         Run the ``subfoo1`` subcommand of the ``foo`` subcommand     
    ``subfoo2``         Run the ``subfoo2`` subcommand of the ``foo`` subcommand     
    ``subfoo3``         Run the ``subfoo3`` subcommand of the ``foo`` subcommand     
    ================    =============================================================


------------


``foo-subfoo1`` subcommand
^^^^^^^^^^^^^^^^^^^^^^^^^^
Subcommands of subcommands are an unusual use case, but it seemed worth
testing


Optional arguments
;;;;;;;;;;;;;;;;;;

    =======================    ====================================
    **Argument**               **Description**                     
    -----------------------    ------------------------------------
    ``-h``, ``--help``         show this help message and exit     
    =======================    ====================================


------------


``foo-subfoo2`` subcommand
^^^^^^^^^^^^^^^^^^^^^^^^^^
This is a multi-line subcommand description intended to test whether or not
:data:`sphinxcontrib.argdoc` can correctly format these. If this text is
correctly formatted, it will end at the end of this sentence.


Optional arguments
;;;;;;;;;;;;;;;;;;

    =======================    ====================================
    **Argument**               **Description**                     
    -----------------------    ------------------------------------
    ``-h``, ``--help``         show this help message and exit     
    =======================    ====================================


------------


``foo-subfoo3`` subcommand
^^^^^^^^^^^^^^^^^^^^^^^^^^

Optional arguments
;;;;;;;;;;;;;;;;;;

    =======================    ====================================
    **Argument**               **Description**                     
    -----------------------    ------------------------------------
    ``-h``, ``--help``         show this help message and exit     
    =======================    ====================================


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
