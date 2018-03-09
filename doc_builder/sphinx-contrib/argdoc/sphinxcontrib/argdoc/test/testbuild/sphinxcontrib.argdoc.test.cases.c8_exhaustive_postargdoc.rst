Exhaustively test all argument types

The following argument attributes are tested in all combinations to ensure
they are all properly parsed and rendered:

  - arguments with short or long names
  - positional arguments
  - keyword arguments starting with '-', '--', or both
  - arguments taking 0, 1, 2, (0 or more), (1 or more), or (0 or 1) arguments
  - arguments taking choices of 1 or more items
  - arguments with no help text
  - arguments with short help text, which tends to be displayed on one line
    by :mod:`argparse`
  - arguments with long help text, which tends to appear on multiple lines
  - arguments including or excluding unicode characters in their names

--------------

Here is a table, to show that we can have rich formatting in the module 
docstring, without :obj:`sphinxcontrib.argdoc` inadvertently introducing problems:

    =============  ======================================================
    **Column 1**   **Column 2**
    -------------  ------------------------------------------------------
     Some item     Some other item.

     Table row 2.  Table row 2 column 2.

     Another row.  Row with a link to `Python <https://www.python.org>`_
    =============  ======================================================
 

See also
--------
A definition list
    The purpose of this `See also` section is just to show that we can use
    a number of reStructuredText structures in the module docstring, and
    still have the argument descriptions appended below.

Here is another item
    To show that our test works


------------


Command-line arguments
----------------------

Positional arguments
~~~~~~~~~~~~~~~~~~~~

    ================================================================================    ================================================================================================================================================================
    **Argument**                                                                        **Description**                                                                                                                                                 
    --------------------------------------------------------------------------------    ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    ``shortpos1``                                                                                                                                                                                                                                       
    ``shortpos2``                                                                       one-line help text                                                                                                                                              
    ``shortpos3``                                                                       this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument1``                                                                                                                                                                         
    ``reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument2``          one-line help text                                                                                                                                             
    ``reallyreallyreallyreallyreallyreallyreallyreallylongpositionalargument3``          this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ================================================================================    ================================================================================================================================================================


Optional arguments
~~~~~~~~~~~~~~~~~~

    ===============================================================================================================    ================================================================================================================================================================
    **Argument**                                                                                                       **Description**                                                                                                                                                 
    ---------------------------------------------------------------------------------------------------------------    ----------------------------------------------------------------------------------------------------------------------------------------------------------------
    ``-h``, ``--help``                                                                                                 show this help message and exit                                                                                                                                 
    ``-a  X``                                                                                                                                                                                                                                                                          
    ``-b  X``                                                                                                          one-line help text                                                                                                                                              
    ``-c  X``                                                                                                          this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--double1  X``                                                                                                                                                                                                                                                                   
    ``--double2  X``                                                                                                   one-line help text                                                                                                                                              
    ``--double3  X``                                                                                                   this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``-d  X``, ``--combo1  X``                                                                                                                                                                                                                                                         
    ``-e  X``, ``--combo2  X``                                                                                         one-line help text                                                                                                                                              
    ``-f  X``, ``--combo3  X``                                                                                         this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--reallyreallyreallyreallyreallyreallylong_double1  X``                                                                                                                                                                                                                          
    ``--reallyreallyreallyreallyreallyreallylong_double2  X``                                                           one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double3  X``                                                           this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-g  X``, ``--reallyreallyreallyreallyreallyreallylong_combo1  X``                                                                                                                                                                                                                
    ``-i  X``, ``--reallyreallyreallyreallyreallyreallylong_combo2  X``                                                 one-line help text                                                                                                                                             
    ``-j  X``, ``--reallyreallyreallyreallyreallyreallylong_combo3  X``                                                 this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-k  X X``                                                                                                                                                                                                                                                                        
    ``-l  X X``                                                                                                        one-line help text                                                                                                                                              
    ``-m  X X``                                                                                                        this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--double4  X X``                                                                                                                                                                                                                                                                 
    ``--double5  X X``                                                                                                 one-line help text                                                                                                                                              
    ``--double6  X X``                                                                                                 this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``-n  X X``, ``--combo4  X X``                                                                                                                                                                                                                                                     
    ``-o  X X``, ``--combo5  X X``                                                                                     one-line help text                                                                                                                                              
    ``-p  X X``, ``--combo6  X X``                                                                                     this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--reallyreallyreallyreallyreallyreallylong_double4  X X``                                                                                                                                                                                                                        
    ``--reallyreallyreallyreallyreallyreallylong_double5  X X``                                                         one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double6  X X``                                                         this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-q  X X``, ``--reallyreallyreallyreallyreallyreallylong_combo4  X X``                                                                                                                                                                                                            
    ``-r  X X``, ``--reallyreallyreallyreallyreallyreallylong_combo5  X X``                                             one-line help text                                                                                                                                             
    ``-s  X X``, ``--reallyreallyreallyreallyreallyreallylong_combo6  X X``                                             this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-t  X [X ...]``                                                                                                                                                                                                                                                                  
    ``-u  X [X ...]``                                                                                                  one-line help text                                                                                                                                              
    ``-v  X [X ...]``                                                                                                  this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--double7  X [X ...]``                                                                                                                                                                                                                                                           
    ``--double8  X [X ...]``                                                                                           one-line help text                                                                                                                                              
    ``--double9  X [X ...]``                                                                                           this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``-w  X [X ...]``, ``--combo7  X [X ...]``                                                                                                                                                                                                                                         
    ``-x  X [X ...]``, ``--combo8  X [X ...]``                                                                          one-line help text                                                                                                                                             
    ``-y  X [X ...]``, ``--combo9  X [X ...]``                                                                          this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``--reallyreallyreallyreallyreallyreallylong_double7  X [X ...]``                                                                                                                                                                                                                  
    ``--reallyreallyreallyreallyreallyreallylong_double8  X [X ...]``                                                   one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double9  X [X ...]``                                                   this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-z  X [X ...]``, ``--reallyreallyreallyreallyreallyreallylong_combo7  X [X ...]``                                                                                                                                                                                                
    ``-A  X [X ...]``, ``--reallyreallyreallyreallyreallyreallylong_combo8  X [X ...]``                                 one-line help text                                                                                                                                             
    ``-B  X [X ...]``, ``--reallyreallyreallyreallyreallyreallylong_combo9  X [X ...]``                                 this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-C  [X]``                                                                                                                                                                                                                                                                        
    ``-D  [X]``                                                                                                        one-line help text                                                                                                                                              
    ``-E  [X]``                                                                                                        this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--double10  [X]``                                                                                                                                                                                                                                                                
    ``--double11  [X]``                                                                                                one-line help text                                                                                                                                              
    ``--double12  [X]``                                                                                                this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``-F  [X]``, ``--combo10  [X]``                                                                                                                                                                                                                                                    
    ``-G  [X]``, ``--combo11  [X]``                                                                                     one-line help text                                                                                                                                             
    ``-H  [X]``, ``--combo12  [X]``                                                                                     this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``--reallyreallyreallyreallyreallyreallylong_double10  [X]``                                                                                                                                                                                                                       
    ``--reallyreallyreallyreallyreallyreallylong_double11  [X]``                                                        one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double12  [X]``                                                        this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-I  [X]``, ``--reallyreallyreallyreallyreallyreallylong_combo10  [X]``                                                                                                                                                                                                           
    ``-J  [X]``, ``--reallyreallyreallyreallyreallyreallylong_combo11  [X]``                                            one-line help text                                                                                                                                             
    ``-K  [X]``, ``--reallyreallyreallyreallyreallyreallylong_combo12  [X]``                                            this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-L  [X [X ...]]``                                                                                                                                                                                                                                                                
    ``-M  [X [X ...]]``                                                                                                one-line help text                                                                                                                                              
    ``-N  [X [X ...]]``                                                                                                this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--double13  [X [X ...]]``                                                                                                                                                                                                                                                        
    ``--double14  [X [X ...]]``                                                                                         one-line help text                                                                                                                                             
    ``--double15  [X [X ...]]``                                                                                         this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-O  [X [X ...]]``, ``--combo13  [X [X ...]]``                                                                                                                                                                                                                                    
    ``-P  [X [X ...]]``, ``--combo14  [X [X ...]]``                                                                     one-line help text                                                                                                                                             
    ``-Q  [X [X ...]]``, ``--combo15  [X [X ...]]``                                                                     this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``--reallyreallyreallyreallyreallyreallylong_double13  [X [X ...]]``                                                                                                                                                                                                               
    ``--reallyreallyreallyreallyreallyreallylong_double14  [X [X ...]]``                                                one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double15  [X [X ...]]``                                                this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-R  [X [X ...]]``, ``--reallyreallyreallyreallyreallyreallylong_combo13  [X [X ...]]``                                                                                                                                                                                           
    ``-S  [X [X ...]]``, ``--reallyreallyreallyreallyreallyreallylong_combo14  [X [X ...]]``                            one-line help text                                                                                                                                             
    ``-T  [X [X ...]]``, ``--reallyreallyreallyreallyreallyreallylong_combo15  [X [X ...]]``                            this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-U  {one_choice}``                                                                                                                                                                                                                                                               
    ``-V  {one_choice}``                                                                                               one-line help text                                                                                                                                              
    ``-W  {one_choice}``                                                                                               this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)      
    ``--double16  {one_choice}``                                                                                                                                                                                                                                                       
    ``--double17  {one_choice}``                                                                                        one-line help text                                                                                                                                             
    ``--double18  {one_choice}``                                                                                        this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-X  {one_choice}``, ``--combo16  {one_choice}``                                                                                                                                                                                                                                  
    ``-Y  {one_choice}``, ``--combo17  {one_choice}``                                                                   one-line help text                                                                                                                                             
    ``-Z  {one_choice}``, ``--combo18  {one_choice}``                                                                   this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``--reallyreallyreallyreallyreallyreallylong_double16  {one_choice}``                                                                                                                                                                                                              
    ``--reallyreallyreallyreallyreallyreallylong_double17  {one_choice}``                                               one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double18  {one_choice}``                                               this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-1  {one_choice}``, ``--reallyreallyreallyreallyreallyreallylong_combo16  {one_choice}``                                                                                                                                                                                         
    ``-2  {one_choice}``, ``--reallyreallyreallyreallyreallyreallylong_combo17  {one_choice}``                          one-line help text                                                                                                                                             
    ``-3  {one_choice}``, ``--reallyreallyreallyreallyreallyreallylong_combo18  {one_choice}``                          this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-4  {one,two,three,four}``                                                                                                                                                                                                                                                       
    ``-5  {one,two,three,four}``                                                                                        one-line help text                                                                                                                                             
    ``-6  {one,two,three,four}``                                                                                        this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``--double19  {one,two,three,four}``                                                                                                                                                                                                                                               
    ``--double20  {one,two,three,four}``                                                                                one-line help text                                                                                                                                             
    ``--double21  {one,two,three,four}``                                                                                this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-7  {one,two,three,four}``, ``--combo19  {one,two,three,four}``                                                                                                                                                                                                                  
    ``-8  {one,two,three,four}``, ``--combo20  {one,two,three,four}``                                                   one-line help text                                                                                                                                             
    ``-9  {one,two,three,four}``, ``--combo21  {one,two,three,four}``                                                   this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``--reallyreallyreallyreallyreallyreallylong_double19  {one,two,three,four}``                                                                                                                                                                                                      
    ``--reallyreallyreallyreallyreallyreallylong_double20  {one,two,three,four}``                                       one-line help text                                                                                                                                             
    ``--reallyreallyreallyreallyreallyreallylong_double21  {one,two,three,four}``                                       this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ``-Â  {one,two,three,four}``, ``--reallyreallyreallyreallyreallyreallylong_combo19  {one,two,three,four}``                                                                                                                                                                         
    ``-Ã  {one,two,three,four}``, ``--reallyreallyreallyreallyreallyreallylong_combo20  {one,two,three,four}``          one-line help text                                                                                                                                             
    ``-Ä  {one,two,three,four}``, ``--reallyreallyreallyreallyreallyreallylong_combo21  {one,two,three,four}``          this is very, very long help text which should span multiple lines and thus require special parsing. We'll also add `special` *chars* (default: 513251324)     
    ===============================================================================================================    ================================================================================================================================================================


------------


Script contents
---------------
