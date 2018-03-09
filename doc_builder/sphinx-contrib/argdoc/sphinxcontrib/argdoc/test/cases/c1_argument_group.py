#!/usr/bin/env python
"""Argument groups with short, long, or no descriptions

:class:`~argparse.ArgumentParser` allows organization of arguments into 
*argument groups*, which can make things more intelligible for users.
:obj:`sphinxcontrib.argdoc` styles argument groups as separate, paragraph-level sections,
with their descriptions (if present) appearing below the title, followed by
the arguments in each group, formatted as a table. Examples appear here below.
"""
import argparse
import sys

g1_title = "One group of arguments"
g1_description = """Sometimes it is useful to group arguments that relate to each other in
an argument group. This can make command-line help, documentation, and source
code more intelligible to others"""

g2_title = "A second group of arguments"
g2_description = "Description of second argument group"

g3_title = "A final group of arguments, with no description"

def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser()
    parser.add_argument("mainarg1")
    parser.add_argument("mainarg2",help="main positional argument #2")
        
    g1 = parser.add_argument_group(title=g1_title,description=g1_description)
    g2 = parser.add_argument_group(title=g2_title,description=g2_description)
    g3 = parser.add_argument_group(title=g3_title)

    g1.add_argument("fooarg1",help="foo argument 1")
    g1.add_argument("fooarg2",help="foo argument 2")
    g1.add_argument("-f",help="short foo argument",type=str)
    g1.add_argument("--fookwarg",help="foo keyword argument",type=str)
    g1.add_argument("-v","--verbose",help="foo verbosely")    

    g2.add_argument("bararg",help="bar argument")
    g2.add_argument("--choice",choices=("option1","option2","option3"),
                           help="A keyword that requries a choice")
    
    g3.add_argument("bazarg",help="baz argument")
    g3.add_argument("--numbers",metavar="M",nargs=3,help="numerical argument")
    g3.add_argument("-z","--zoom",help="zzzzzzzzzzzzzz")
     
    args   = parser.parse_args(argv)

if __name__ == "__main__":
    main()
