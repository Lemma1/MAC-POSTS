Subcommands with no main program arguments

:class:`~argparse.ArgumentParser` supports use of subcommands, 
each of which might have their own arguments and argument groups.
Here these are tested for a parser that has none of its own arguments,
except ``--help``.


------------


Command-line arguments
----------------------

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

    =============    ==============================================================================
    **Argument**     **Description**                                                               
    -------------    ------------------------------------------------------------------------------
    ``foo``          Run the foo subprogram                                                        
    ``bar``          Take output from foo subprogram and run it through the ``bar`` subprogram     
    =============    ==============================================================================


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


------------


Script contents
---------------
