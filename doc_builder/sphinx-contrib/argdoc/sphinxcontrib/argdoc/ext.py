#!/usr/bin/env python
"""Functions that constitute the :data:`sphinxcontrib.argdoc` extension for `Sphinx`_.

User functions
--------------
:func:`noargdoc`
    Function decorator that forces :data:`sphinxcontrib.argdoc` to skip a :term:`main-like function`
    it would normally process
    
Developer functions
-------------------

:func:`format_argparser_as_docstring`
    Extract tables of arguments from an :class:`~argparse.ArgumentParser`
    and from all of its subprograms, then format their descriptions and
    help text.

:func:`get_subcommand_tables`
    Extract tables from all subcommand
    :class:`ArgumentParsers <argparse.ArgumentParser>`
    contained by an enclosing :class:`~argparse.ArgumentParser`

:func:`post_process_automodule`
    Event handler that activates :data:`sphinxcontrib.argdoc` upon `autodoc-process-docstring`
    events

:func:`setup`
    Register :data:`sphinxcontrib.argdoc` with the running `Sphinx`_ instance
"""
import sys
import re
import shlex
import subprocess
import os
import codecs

import sphinx
from sphinx.errors import ConfigError

__author__  = "Joshua Griffin Dunn"

#===============================================================================
# INDEX: various constants
#===============================================================================
_REQUIRED = [
    'sphinx.ext.autodoc',
    'sphinx.ext.autosummary',
]
"""Other `Sphinx`_ extensions required by :data:`sphinxcontrib.argdoc`"""



_HEADERS = "=-~._\"'^;"
_INDENT_SIZE = 4


def safeunicode(inp):
    """Convert a string to unicode in a Python 2.7/3.x-safe way

    Parameters
    ----------
    inp : str
        Input string

    Returns
    -------
    unicode (Python 2.7) or string (Python 3.x)
        utf-8 encoded representation of `inp`
    """
    if sys.version_info[0] == 2 and isinstance(inp,str):
        return unicode(inp,"utf-8")
    else:
        return inp


_PLACEHOLDER_CONSTANT = "ARGDOCPOSITIONALARGUMENT "
_OTHER_HEADER_LINES = safeunicode("""Script contents
---------------""").split("\n")

_SEPARATOR = safeunicode("\n------------\n\n").split("\n")

#===============================================================================
# INDEX: helper functions for token parsing and text formatting
#===============================================================================

def get_patterns(prefix_chars="-"):
    """Retrieve a dictionary of regular expressions that separate argument names
    from their values and descriptions
    
    Parameters
    ----------
    prefix_chars : str, optional
        String of prefix characters that the :class:`~argparse.ArgumentParser`
        uses (Default: `'-'`)
    
    Returns
    -------
    dict
        Dictionary of regular expression patterns
    """
    all_patterns = {}
    esc_prefix_chars = prefix_chars
    for char in "-+*?[]{}()":
        esc_prefix_chars = esc_prefix_chars.replace(char,"\%s" % char)
    for char in prefix_chars:
        if char in "-+*?[]{}()":
            esc_char = "\%s" % char
        else:
            esc_char = char
        patterns = { "section_title"      : r"^(\w+.*):$",
                     "positional_arg"     : r"^  (?P<arg1>[^{}\sALL]+)(?:\s\s+(?P<desc>\w+.*))?$".replace("ALL",esc_prefix_chars),
                     "arg_only"           : r"^  (?P<arg1>-+[^\s,]+)(?:, (?P<arg2>--[^\s]+))?$".replace("-",esc_char),
                     "arg_plus_val"       : r"^  (?P<arg1>-+[^\s]+)(?P<val1>(?: [^ALL\s]+)+)(?:(?:, (?P<arg2>--[^\s]+))(?P<val2>(?: [^\s]+)+))?$".replace("-",esc_char).replace("ALL",esc_prefix_chars),
                     "arg_plus_desc"      : r"^  (?P<arg1>-+[^\s]+)(?:,\s(?P<arg2>--[^\s]+))?\s\s+(?P<desc>.*)".replace("-",esc_char),
                     "arg_plus_val_desc"  : r"^  (?P<arg1>-+[^\s]+)(?P<val1>(?: [^ALL\s]+)+)(?:(?:, (?P<arg2>--[^\s]+))(?P<val2>(?: [^\s]+)+))?  +(?P<desc>\w+.*)$".replace("-",esc_char).replace("ALL",esc_prefix_chars),
                     "continue_desc"      : r"^ {12,24}(.*)",
                     "section_desc"       : r"^  ((?:[^ALL\s][^\s]*)(?:\s[^\s]+)+)$".replace("ALL",esc_prefix_chars),
                     "subcommand_names"   : r"^  {((?:\w+)(?:(?:,(?:\w+))+)?)}$",
                     "subcommand_name"    : r"^    (?P<arg1>[^{}\sALL]+)(?:\s\s+(?P<desc>\w+.*))?$".replace("ALL",esc_prefix_chars), # same as positional arg, but with more leading space
                    }
        
        all_patterns[char] = { K : re.compile(V) for K,V in patterns.items() }

    return all_patterns

def get_col1_text(matchdict):
    """Format argument name(s) and value(s) for column 1 of argument tables

    Parameters
    ----------
    matchdict : dict
        Dictionary of values

    Returns
    -------
    str (unicode if Python 2.7)
    """
    if "val1" in matchdict:
        tmpstr = "``%s %s``" % (matchdict["arg1"],matchdict["val1"])
        if matchdict.get("arg2") is not None:
            tmpstr += (", ``%s %s``" % (matchdict["arg2"],matchdict["val2"]))
    else:
        tmpstr = "``%s``" % matchdict["arg1"]
        if matchdict.get("arg2") is not None:
            tmpstr += (", ``%s``" % matchdict["arg2"])
        
    return safeunicode(tmpstr)

def get_col2_text(matchdict):
    """Format argument descriptions, if present, for column 2 of argument tables

    Parameters
    ----------
    matchdict : dict
        Dictionary of values

    Returns
    -------
    str (unicode if Python 2.7)
    """
    # line below looks weird- but coming out of regex matches,
    # often 'desc' *is* defined with value `None`
    tmpstr =  matchdict.get("desc","") if matchdict.get("desc") is not None else ""
    return safeunicode(tmpstr)

def make_rest_table(rows,title=False,indent=0):
    """Make a reStructuredText table from a list of rows of items

    Parameters
    ----------
    rows : list of tuples
        A row of text to put in the table, each tuple item a cell

    title : bool, optional
        If `True`, the first pair is assumed to contain column headings
        (Default: `False`)

    indent_size : int, optional
        Number of spaces prepend to each line of output (Default: `0`)

    Returns
    -------
    list
        List of strings, corresponding to multi-line `reStructuredText`_ table
    """
    columns = list(zip(*rows))
    lengths = [1 + max([len(X) for X in Y]) for Y in columns]
    if title == True:
        lengths = [X+4 for X in lengths]

    border   = []
    template = []
    for n, my_length in enumerate(lengths):
        border.append(safeunicode("="*my_length))
        template.append(safeunicode("{%s: <%ss}" % (n,my_length)))

    border   = safeunicode("    ").join(border)
    template = safeunicode("    ").join(template)

    lines = [border]
    n = 0
    if title == True:
        title_row = [safeunicode("**%s**") % X for X in rows[0]]
        lines.append(template.format(*tuple(title_row)))
        lines.append(border.replace("=","-"))
        n = 1

    for items in rows[n:]:
        lines.append(template.format(*items))

    lines.append(border)
    lines.append(safeunicode(""))
    if indent > 0:
        tmp = safeunicode(" "*indent)
        lines = [tmp+X if len(X) > 0 else X for X in lines]

    return lines

def format_warning(topline,details):
    """Format warning text clearly

    Parameters
    ----------
    topline : str
        One-line description of warning

    details : str
        Multiline, detailed description of warning (e.g. exception info)


    Returns
    -------
    str
        Multiline warning message, formatted
    """
    border = "-"*75 + "\n"
    out = border
    out += ("[argdoc] %s\n" % topline)
    out += details
    out += border
    return out


#===============================================================================
# INDEX: function decorator
#===============================================================================

def noargdoc(func):
    """Decorator that forces :data:`sphinxcontrib.argdoc` to skip processing of `func` 
    
    Parameters
    ----------
    func : function
        :term:`main-like function` of a script

    
    Returns
    -------
    func
        wrapped function
    """
    func.__dict__["noargdoc"] = True
    return func

#===============================================================================
# INDEX: documentation generation functions
#===============================================================================

def get_subcommand_tables(app,obj,help_lines,patterns,start_line,command_chain="",section_head=True,header_level=1):
    """Process help output from an :py:class:`~argparse.ArgumentParser`
    that includes one or more subcommands.  Called by :func:`format_argparser_as_docstring`
    
    Parameters
    ----------
    app
        Sphinx application instance
    
    obj : object
        Object (e.g. module, class, function) to document
            
    help_lines : list
        List of strings, each corresponding to a line of output from having
        passed ``--help`` as an argument to the :term:`main-like function`

    
    patterns : dict
        Dictionary names of line types in argparse output to regular expression
        patterns that process those line types

    start_line : int
        Line in argparse help output containing subcommand header

    section_head : bool, optional
        If `True`, a section header for "Command-line arguments" will be included
        in the output. (Default: `True`)

    pre_args : int, optional
        Number of arguments required to be supplied before subcommand help
        can be retrieved (Default: `0`)
        
    header_level : int, optional
        Level of header to use for `section_name`. Lower numbers are higher
        precedence. (Default: `1`)        
    
    Returns
    -------
    list
        List of strings encoding reStructuredText table of command-line
        arguments for all subprograms in the containing argparser
    """
    out_lines = []
    base = list(patterns.values())[0]
    for line in help_lines[start_line:]:
        match = base["subcommand_names"].search(line.strip("\n")) 
        if match is not None:
            subcommands = match.groups()[0].split(",")
            break
    
    app.debug("[argdoc] %s subcommands: %s" % (obj.__name__,", ".join(subcommands)))
    for subcommand in subcommands:
        try:
            newname = command_chain.replace(_PLACEHOLDER_CONSTANT,"").replace("  "," ").split()
            newname.append(subcommand)
            newname ="-".join(newname)
            callstr = "%s -m %s %s %s %s%shelp" % (sys.executable,
                                                   obj.__name__,
                                                   command_chain,
                                                   subcommand,
                                                   app.config.argdoc_prefix_chars[0],
                                                   app.config.argdoc_prefix_chars[0])
            app.debug("[argdoc] Parsing subcommand %s with as `%s`" % (subcommand,callstr))
            call = shlex.split(callstr)
            out = subprocess.check_output(call,env=os.environ.copy())
            if sys.version_info[0] == 2:
                out = unicode(out,"utf-8")
            elif sys.version_info[0] == 3:
                out = out.decode("utf-8")
            sub_help_lines = out.split("\n")
            new_command_chain = command_chain + (" %s " % subcommand)
            out_lines.extend(format_argparser_as_docstring(app,
                                                           obj,
                                                           sub_help_lines,
                                                           patterns,
                                                           section_head=section_head,
                                                           header_level=header_level+1,
                                                           section_name=safeunicode("``%s`` subcommand" % newname),
                                                           _is_subcommand=True,
                                                           command_chain=new_command_chain)) 
        except subprocess.CalledProcessError as e:
            note = "Could not call module %s as '%s'. Output:\n"% (obj.__name__, " ".join(e.cmd))
            msg = format_warning(note,e.output)
            app.warn(msg)

    return out_lines

def format_argparser_as_docstring(app,obj,help_lines,patterns,
                                  section_head=True,section_name=safeunicode("Command-line arguments"),
                                  header_level=1,
                                  _is_subcommand=False,
                                  command_chain="",
                                  ):
    """Process help output from an :py:class:`argparse.ArgumentParser`.
    Called by :func:`post_process_automodule` and :func:`get_subcommand_tables`
    
    Parameters
    ----------
    app
        Sphinx application instance
    
    obj : object
        Object (e.g. module, class, function) to document
            
    help_lines : list
        List of strings, each corresponding to a line of output from having
        passed ``--help`` as an argument to the :term:`main-like function`

    patterns : dict
        Dictionary names of line types in argparse output to regular expression
        patterns that process those line types
    
    section_head : bool, optional
        If `True`, a section header for "Command-line arguments" will be included.
        This messes up parsing for function docstrings, but is fine for module
        docstrings (Default: `False`).
    
    section_name : str, optional
        A name or title for the current program or subcommand.
        (Default: `'Command-line arguments'`)
    
    header_level : int, optional
        Level of header to use for `section_name`. Lower numbers are higher
        precedence. (Default: `1`)
    
    _is_subcommand : bool, optional
        If `True`, include module docstring in output. Required for subcommands
        whose help won't be included by in the module docstring found by 
        autodoc. (Default: `False`) 
        
    
    Returns
    -------
    list
        List of strings encoding reStructuredText table of arguments
        for program or subprogram
    """
    base = list(patterns.values())[0]
    started          = False
    has_subcommands  = False
    subcommand_start = 0

    out_lines = []  # lines we will output
    positional_args = 0

    # markers for beginning and end of subcommand docstring descriptions
    desc_start = None
    desc_end   = None

    # the following are wiped & re-initialized for each section
    col1      = ["Argument"]  # holder for column 1 contents: argument names
    col2      = ["Description"]  # holder for column 2 contents: argument descriptions
    section_title = [] # title of current section
    section_desc  = [] # description of current section
    unmatched = []
    
    for n,line in enumerate(help_lines):
        line = line.rstrip()
        if _is_subcommand == True and desc_start is None:
            # subcommand descriptions are not automatically added by autodoc,
            # so we need to track them ourselves
            if line.strip() == "":
                desc_start = n+1
        if started == True:
            if len(line.strip()) == 0 and len(col1) > 1 and len(col2) > 1:
                # if current argument group is finished, format table of arguments for export
                # and append it to `out_lines`
                if len(col1) != len(col2):
                    app.warn("[argdoc] Column mismatch in section '%s'. col1 %s, col2 %s rows." % (section_title,len(col1),len(col2)))

                out_lines.append(safeunicode(""))
                out_lines.extend(section_title)
                out_lines.extend(section_desc)
                out_lines.append(safeunicode(""))
                out_lines.extend(make_rest_table(list(zip(col1,col2)),title=True,indent=_INDENT_SIZE))        
                out_lines.extend(unmatched)

                # reset section-specific variables
                section_title = []
                section_desc  = []
                col1 = ["Argument"]
                col2 = ["Description"]
                unmatched = []
            else:
                matchdict = None
                match = None
                for pat in ["section_title",
                            "section_desc",
                            "positional_arg",
                            "arg_only",
                            "arg_plus_val",
                            "continue_desc",
                            "arg_plus_desc",
                            "arg_plus_val_desc",
                            "subcommand_names",
                            "subcommand_name",
                            ]:
                    for char in patterns.keys():
                        if match is None:
                            match = patterns[char][pat].match(line)
                            if match is not None:
                                app.debug2("[argdoc] %s\n    %s\n" % (pat,line))
                                if pat == "section_title":
                                    match = base["section_title"].match(line)
                                    section_title = [match.groups()[0].capitalize(),
                                                     _HEADERS[header_level+1]*len(match.groups()[0]),
                                                    ]
                                    break
                                elif pat == "section_desc":
                                    section_desc.append(line)
                                    break
                                elif pat == "continue_desc":
                                    try:
                                        col2[-1] = safeunicode("%s %s" % (col2[-1],match.groups()[0].strip("\n")))
                                    except IndexError as e:
                                        app.warn("[argdoc] continuing description with no prior description on line %s: \n    %s" % (n,line))
                                        assert False
                                    break
                                elif pat == "positional_arg":
                                    matchdict = match.groupdict()
                                    col1.append(get_col1_text(matchdict))
                                    col2.append(get_col2_text(matchdict))
                                    positional_args += 1
                                    break
                                elif pat == "subcommand_names":
                                    has_subcommands = True
                                    subcommand_start = n
                                    break
                                elif pat == "subcommand_name":
                                    matchdict = match.groupdict()
                                    col1.append(get_col1_text(matchdict))
                                    col2.append(get_col2_text(matchdict))
                                    if has_subcommands == False:
                                        app.warn("[argdoc] found subcommand-like line but no subcommands at line %s:\n    %s" % (n,line))
                                    break
                                else:
                                    matchdict = match.groupdict()
                                    col1.append(get_col1_text(matchdict))
                                    col2.append(get_col2_text(matchdict))
                                    break
                if match is None:
                    # triggered if epilog, or if other unmatched lines
                    if len(line.strip()) > 0:
                        app.debug2("[argdoc] No match. Epilog?\n%s\n" % line.strip())
                        line = safeunicode(line)
                        out_lines.append(line)
                    else:
                        app.debug2("[argdoc] blank line")
        # FIXME: 
        # this is how we test where argument descriptions begin in ``--help`` text
        # at present we look for an explicit 'arguments:' token, which allows argdoc
        # to deal with lines of helptext that have trailing colons but which don't start
        # argument sections (which a regex would fail at)
        #
        # BUT, if an argument parser has no line that says "arguments:" in its helptext,
        # argdoc will fail
        #
        # we need a better test, which will be more portable
        elif line.endswith("arguments:"):
            # Found first argument section. Create command-line argument heading
            started = True
            desc_end = n
            if section_head == True:
                stmp1 = section_name
                stmp2 = _HEADERS[header_level]*len(section_name)
                out_lines.extend(_SEPARATOR)
                out_lines.append(safeunicode(stmp1))
                out_lines.append(safeunicode(stmp2))
                # if is a subcommand, put cached description under heading
                if _is_subcommand == True:
                    out_lines.extend(help_lines[desc_start:desc_end])
            
            # Create paragraph header for the argument section
            match = base["section_title"].match(line)
            section_title = [match.groups()[0].capitalize(),
                             _HEADERS[header_level+1]*len(match.groups()[0]),
                            ]
   
    if has_subcommands == True:
        # parse subcommand argparsers after main, and append output below
        command_chain = command_chain + (_PLACEHOLDER_CONSTANT*positional_args)
        new_lines = get_subcommand_tables(app,
                                          obj,
                                          help_lines,
                                          patterns,
                                          subcommand_start,
                                          section_head=section_head,
                                          header_level=header_level+2,
                                          command_chain=command_chain,
                                          )
        out_lines.extend(new_lines)

    return out_lines

def post_process_automodule(app,what,name,obj,options,lines):
    """Insert a table listing and describing an executable script's command-line
    arguments into its ``:automodule:`` documentation.
    
    Any :term:`main-like function` decorated with the :func:`noargdoc` decorator
    will be skipped. A function is determined to be a :term:`main-like function`
    if its name matches the name set in the configuration option
    `argdoc_main_func` inside ``conf.py``. The default value for
    `argdoc_main_func` is `main`.
    
    
    Notes
    -----
    Per the `autodoc`_ spec, this function modifies `lines` in place.
    
    
    Parameters
    ----------
    app
        Sphinx application instance
    
    what : str
        Type of object (e.g. "module", "function", "class")
    
    name : str
        Fully-qualified name of object
    
    obj : object
        Object (e.g. module, class, function) to document
    
    options : object
        Options given to the directive, whose boolean properties are set to `True`
        if their corresponding flag was given in the directive

    lines : list
        List of strings encoding the module docstrings after `Sphinx`_ processing

    Raises
    ------
    :class:`~sphinx.errors.ConfigError`
       If `argdoc_main_func` is defined in ``conf.py`` and is not a `str`
    """
    funcname     = app.config.argdoc_main_func
    prefix_chars = app.config.argdoc_prefix_chars
    patterns = get_patterns(prefix_chars)

    errmsg = ""
    if not isinstance(funcname,str):
        errmsg += "[argdoc] Incorrect type for `argdoc_main_func. Expected `str`, found, `%s` with value `%s`)\n" % (type(funcname),funcname)
    if len(prefix_chars) == 0:
        errmsg += "[argdoc] Expected at least one prefix character (e.g. '-'). Found empty string.\n"

    if len(errmsg) > 0:
        raise ConfigError(errmsg)

    if what == "module" and obj.__dict__.get(funcname,None) is not None:
        if obj.__dict__.get(funcname).__dict__.get("noargdoc",False) == False:
            app.debug2("[argdoc] Processing module '%s'" % obj.__name__)
            call = shlex.split("%s -m %s --help".replace("-",prefix_chars[0]) % (sys.executable,name))
            try:
                out = subprocess.check_output(call,env=os.environ.copy()) 
                if sys.version_info[0] == 2:
                    out = unicode(out,"utf-8")
                elif sys.version_info[0] == 3:
                    out = out.decode("utf-8")
                help_lines = out.split("\n")
                out_lines = format_argparser_as_docstring(app,obj,help_lines,section_head=True,header_level=1,patterns=patterns)
                out_lines += _SEPARATOR
                lines.extend(out_lines)
                lines.extend(_OTHER_HEADER_LINES)
            except subprocess.CalledProcessError as e:
                note = "Could not call module %s as '%s'. Output:\n"% (name," ".join(e.cmd))
                app.warn(format_warning(note,e.output))
            except IndexError as e:
                note = "Error processing argparser into docstring for module %s: \n%s" % (name,e.message)
                details = "\n\n%s\n\n%s" % (e.args,e)
                app.warn(format_warning(note,details))

        if app.config.argdoc_save_rst == True:
            filename = os.path.join(app.outdir,"%s_postargdoc.rst" % name)
            with codecs.open(filename,encoding="utf-8",mode="wb") as fout:
                for n,line in enumerate(lines):
                    try:
                        line = safeunicode(line)
                        fout.write(line)
                        fout.write(safeunicode("\n"))
                    except Exception as e:
                        app.warn("[argdoc] Could not write out line %s of file %s." % (n,name))
    
            fout.close()
                
        app.emit("argdoc-process-docstring",what,name,obj,options,lines)

