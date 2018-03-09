# ============
# py_directive
# ============
#
# py_directive is an extension to the reStructuredText markup providing
# a directive named py.
#
# The py directive is used to generate a professional (possibly even
# journal quality) ouput from a series of calculations.  This directive is
# easily used with Sphinx if included as an addon or can be imported
# into a Docutils front end.  
#
# Version Info
# ============
#
# ::

__version__ = 'dev'

# Standard Imports
# ================
#
# ::

import sys,traceback, numpy
from code import compile_command
from decimal import Decimal, getcontext

# Requirements
# ============
#
# The Docutils version needed is one that supports math nodes which are used in
# the py directive.
#
# ::

from docutils.parsers.rst import directives, Directive
from docutils.nodes import literal_block, math_block, Text
from docutils.parsers.rst.directives.images import Image, Figure

# Matplotlib for Plotting
# -----------------------
#
# Matplotlib contributed all of the plotting code.  Thanks.
# ::


import matplotlib._pylab_helpers
import matplotlib.sphinxext.plot_directive
from matplotlib.sphinxext.plot_directive import render_figures
from hashlib import md5
import re, os

# Sympy for Latex Printing
# =========================
# ::

from sympy.printing.latex import LatexPrinter as SympyLaTeXPrinter
from sympy import sympify, SympifyError, Matrix
from sympy.core.sympify import converter

converter[numpy.ndarray] = lambda x: Matrix(x)

# Py Directive Configuration
# ==========================
# ::

expr2math = sympify

config = {'namespace' : {},
          'pre_processor' : lambda lines : lines,
          'post_processor' : lambda lines : lines,
          'latex_find_replace_func' : lambda line : line,
          'expr2math' : expr2math,
          'math_node_class' : math_block,
          'literal_node_class' : literal_block,
          'prec' : -1,
          'math_printer_settings':{'mode':'plain','mat_str':'bmatrix', 'mat_delim':None},
          }

# Plotting
# ========
# ::

template = "   .. figure:: %(tmpdir)s/%(outname)s.pdf\n%(options)s\n\n"
template_content_indent = '      '
exception_template = "\n\n **Exception occured rendering plot** \n\n"
def plot(plot_code, options):
    # Since we don't have a filename, use a hash based on the content
    plot_path = md5(plot_code).hexdigest()[-10:]
    basedir, fname = os.path.split(plot_path)
    fname = os.path.basename(plot_path)
    basename, ext = os.path.splitext(fname)

    outdir = os.path.join('plot_directive', basedir)
    if not os.path.exists(outdir):
        os.mkdir(outdir)
    tmpdir = os.path.join('build', outdir)
    if not os.path.exists(tmpdir):
        os.mkdir(tmpdir)
    tmpdir = os.path.abspath(tmpdir)
    if not os.path.exists(tmpdir):
        os.mkdir(tmpdir)
    formats = [('png', 80), ('hires.png', 200), ('pdf', 50)]
    # Generate the figures, and return the number of them
    num_figs = render_figures(plot_path, None, plot_code, tmpdir,
                              outdir, formats, config["namespace"])
    # Now start generating the lines of output
    lines = []

    if num_figs > 0:
        options = ['%s:%s: %s' % (template_content_indent, key, val)
                   for key, val in options.items()]
        options = "\n".join(options)

        for i in range(num_figs):
            if num_figs == 1:
                outname = basename
            else:
                outname = "%s_%02d" % (basename, i) 
            lines.extend((template % locals()).split('\n'))
    else:
        lines.extend((exception_template % locals()).split('\n'))

    for fignum, manager in matplotlib._pylab_helpers.Gcf.figs.items():
        matplotlib._pylab_helpers.Gcf.destroy(fignum)
    return lines

# Code Execution
# ==============
#
# Execute code and evalate expressions while trying to give sufficient
# traceback information for debugging.
#
# ::

def _print_traceback(content):
    ''' 
    Print a traceback including the code that cause the error.
    '''
    for i,l in enumerate(content.split('\n')):
        print '%s: %s'%(i+1,l)
    print sys.path
    print os.getcwd()
    traceback.print_exc(file=sys.stdout)
    sys.exit(0)


def _exec(code):
    ''' 
    Execute a line of code. If it fails, try to give a reasonable amount
    of information to the user.
    '''
    try:
        exec code in config['namespace']
    except:
        _print_traceback(code)

def _eval(code,prec=-1):
    '''
    Evaluate an expression.  If prec > -1, the result will be trimmed to
    the given precision using Decimal.
    '''
    out = eval(code, config['namespace'])
    if prec > -1:
        getcontext().prec = prec
        try:
            out= Decimal(1)*Decimal(out)
        except:
            pass
    return out

# Math Printing
# =============
# ::

def row_to_latex(rowin):
    str_list = [code2math(item) for item in rowin]
    out_str = ' & '.join(str_list)
    return out_str


def two_D_array_to_latex(nested_list):
    """Basically pass each row list to row_to_latex and then append
    the latex line break to all rows except the last.  This is done by
    appending the latex line break before the row string for each line
    after the first one."""
    str_out = ''

    first = 1

    for row in nested_list:
        if first:
            first = 0
        else:
            str_out += ' \\\\ '#latex line break
        row_str = row_to_latex(row)
        str_out += row_str

    return str_out

class MathPrinter(SympyLaTeXPrinter):
    """
    The Sympy LaTeXEx printer provides nice type conversion to LaTeX
    along with supporting greek characters, sub and superscripts, and
    variable accents.
    """

    def _print_Quantity(self,expr):
        out_str = str(expr).replace('dimensionless','')
        return out_str


    def _print_ndarray(self, expr):
        fmt = '\\begin{bmatrix} %s \\end{bmatrix}'
        if len(expr.shape) == 1:
            #1D array
            out_str =  fmt % row_to_latex(expr.tolist())
        elif len(expr.shape) == 2:
            out_str = fmt % two_D_array_to_latex(expr.tolist())
        else:
            raise NotImplementedError, "Not sure how to print a matrix with more than 2 dimensions: " + \
                  "len(expr.shape) = " + str(len(expr.shape))
        return out_str

math_printer = MathPrinter(settings=config['math_printer_settings'])         
config['math_printer'] = math_printer

def code2math(code):
    ''' 
    Convert an expression into math.
    '''
    #print type(code)
    try:
        return config['math_printer'].doprint(config['expr2math'](code))
    except SympifyError:
    	return config['math_printer'].doprint(code)


def is_sage_eqn(expr):
    if hasattr(expr, 'is_relational'):
        return expr.is_relational()
    else:
        return False


def sageeqn2math(eqn):
    rhs = eqn.rhs()
    lhs = eqn.lhs()
    rhsstr = code2math(rhs)
    lhsstr = code2math(lhs)
    lineout = '%s = %s' % (lhsstr, rhsstr)
    return lineout


def mathify(line, prec=config['prec'],eval_rhs = False, return_lhs = True):
    '''
    Turn a line of code into a math equation.
    '''
    if not line.strip():
        #do nothing for a blank line or line of only whitespace
        return line
    
    equation = True
    if line.find('=') > -1:
        lhs, rhs = line.split('=',1)
        lhs_math = code2math(lhs)
    else:
        lhs = lhs_math = ''
        rhs = line
        equation = False
    if eval_rhs:
        rhs_math = _eval(rhs,prec=prec)
    else:
        rhs_math = rhs
    if is_sage_eqn(rhs_math):
            rhs_math = sageeqn2math(rhs_math)
            rhs_math = rhs_math
    else:
        rhs_math = code2math(rhs_math)

        
    if return_lhs and equation:
        out = '%s &= %s' % (lhs_math,rhs_math)
    elif return_lhs and not equation:
        out = '%s' % rhs_math
    else:
        out = '&= %s' % (rhs_math)
    out = config['latex_find_replace_func'](out)
    return out

def codeblock2math(code,prec=config['prec'],return_source=True, return_output=True):
    '''
    Turn a block of code into equations.
    '''
    verified_code = []
    cur_cmd = ''
    for line in code:
        if cur_cmd != '':
            cur_cmd += '\n'+line
        else:
            cur_cmd = line
        if compile_command(cur_cmd):
            verified_code.append(cur_cmd)
            cur_cmd = ''

    for line in verified_code:
        _exec(line)
        if return_source:
            yield mathify(line)
        if return_output:
            yield mathify(line,prec=prec,eval_rhs = True, return_lhs = not return_source)

# Py Directive
# ============
# ::

class py_directive(Directive):

    has_content = True
    option_spec = {
        'showsource': directives.flag,
        'prec' : directives.nonnegative_int,
        'suppress' : directives.flag,
        'context' : directives.unchanged,
        # Options needed in plot context.
        'caption' : directives.unchanged,
        'height': directives.length_or_unitless,
        'width': directives.length_or_percentage_or_unitless,
        'scale': directives.nonnegative_int,
        'align': Image.align,
    }

    def run(self):
        prec = config['prec']
        context = 'math'
        content = config['pre_processor'](self.content)
        if 'prec' in self.options:
            prec = self.options['prec']
        if 'context' in self.options:
            context = self.options['context'].lower()
        if context in ['block','code'] or 'showsource' not in self.options and 'suppress' in self.options:
            code_block = config['post_processor'](content)
            code = '\n'.join(code_block)
            _exec(code)

            if not 'suppress' in self.options:
                litnode = config['literal_node_class'](code,code)
                litnode['language'] = 'python'
                return [litnode]
            else:
                return []
        elif context == 'math':
            code_block = '\\\\\n'.join(codeblock2math(content,prec=prec, return_source='showsource' in self.options,return_output='suppress' not in self.options))
            if 'showsource' in self.options or not 'suppress' in self.options:
                code_block = config['post_processor'](code_block)
                math_node = config['math_node_class']()
                math_node['latex'] = code_block
                math_node['nowrap'] = False
                math_node['label'] = None
                if code_block.find('\n') == -1:
                    code_block = code_block.replace('&=','=')
                math_node.children.append(Text(code_block, code_block))

                #math_node['docname'] = self.state.document.settings.env.docname
                return [math_node]
            else:
                return []
        
        elif context == 'plot':
            # get rid of the context option or it will show up in the options for .. figure::
            del self.options["context"]
            rst = plot('\n'.join(content), self.options)
            self.state_machine.insert_input(
                rst, self.state_machine.input_lines.source(0))
            return []
        elif context == 'stdout':# Idea to implement.  Use state_machine.insert_input(stdout_captured).
            raise NotImplementedError, "%s not yet supported" % context
        else:
            raise NotImplementedError, "%s not yet supported" % context
            
# Sphinx Setup
# ============
# 
# The py directive can be enabled as a Sphinx extention.  Simply add
# "py_directive" to the extensions in conf.py.
#
# ::

def setup(app):
    ''' 
    Provide the setup function for Sphinx.
    '''
    from sphinx.ext.mathbase import displaymath
    config['math_node_class'] = displaymath

# Docutils Setup
# ==============
#
# ::

directives.register_directive('py', py_directive)
matplotlib.sphinxext.plot_directive.plot_context = config["namespace"]    

# When showing figures in plot context, a caption can be provided with
# the caption option to the py directive. 
# ::

Figure.option_spec["caption"] = directives.unchanged
from docutils.writers.latex2e import LaTeXTranslator

def depart_figure(obj, node):
    if node.children[0].has_key("caption"):
        obj.out.append("\\caption{%s}\n" % node.children[0]["caption"])
    obj.out.append("\\end{figure}\n")

LaTeXTranslator.depart_figure  = depart_figure
