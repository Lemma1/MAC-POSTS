#!/usr/bin/env python
"""Subcommands with main program arguments

In this test case, we test a parser that has its own arguments as well as
multiple subcommands, for which individual help sections should be generated.
"""
import argparse
import sys

foo_help = "Run the ``foo`` subprogram"
foo_desc = """This is a long description of what a ``foo`` program might do.
It spans multiple lines, so that we can test things reasonably.
"""

bar_help = "Take output from ``foo`` subprogram and run it through the ``bar`` subprogram"
bar_desc = """This is the long description for the ``bar`` subprogram."""

def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser()
    parser.add_argument("mainarg1")
    parser.add_argument("mainarg2",help="main positional argument #2")
    subparsers = parser.add_subparsers(title="subcommands",
                                       description="choose one of the following:",
                                       dest="program")
    fooparser = subparsers.add_parser("foo",
                                      help=foo_help,
                                      description=foo_desc)
    barparser = subparsers.add_parser("bar",
                                      help=bar_help,
                                      description=bar_desc)
    
    fooparser.add_argument("fooarg1",help="foo argument 1")
    fooparser.add_argument("fooarg2",help="foo argument 2")
    fooparser.add_argument("-f",help="short foo argument",type=str)
    fooparser.add_argument("--fookwarg",help="foo keyword argument",type=str)
    fooparser.add_argument("-v","--verbose",help="foo verbosely")

    barparser.add_argument("bararg",help="bar argument")
    barparser.add_argument("--choice",choices=("option1","option2","option3"),
                           help="A keyword that requries a choice")
    
    bgroup = barparser.add_argument_group(title="An argument group",
                                          description="A special goup of arguments in the ``bar`` subparser")
    bgroup.add_argument("--b1")
    bgroup.add_argument("--b2",help="Argument 2 has help (bar argument 1 did not have help)")
    bgroup.add_argument("-k",nargs=2,metavar="N",help="Some other argument")

    args = parser.parse_args(argv)
 
if __name__ == "__main__":
    main()
