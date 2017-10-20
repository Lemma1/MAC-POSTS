import unittest
import sphinxcontrib.py_directive as py_directive
from docutils.core import publish_parts

class TestPyDirective(unittest.TestCase):
    def setUp(self):
        pass

    def test_code2math_2y(self):
        res = py_directive.code2math('2*y')
        self.assertEqual(res, '2 y')

    def test_code2math_tau(self):
        res = py_directive.code2math('tau_a**2')
        self.assertEqual(res, '\\tau_{a}^{2}')
    
    def test_mathify_methodcall(self):
        res = py_directive.mathify("x = y.replace('a', 'b')")
        #expected =  'x &= y.replace('a', 'b')'#Need to find a solution.
        expected = 'x &= y'
        self.assertEqual(res, expected)
    
    def test_mathify_fucntioncall(self):
        res = py_directive.mathify('x = diff(T,t)')
        #expected = 'x &= \frac{d}{dt}T' #<- Need to find a solution.
        expected = 'x &= 0'              #   this is currently a feature request https://code.google.com/p/sympy/issues/detail?id=1966
        self.assertEqual(res, expected)

    def test_mathify_sympy_diff(self):
        exec 'from sympy.abc import a,x\nfrom sympy import diff' in py_directive.config['namespace']
        code = 'z=x\ny=diff(z,x)'
        res = '\n'.join(py_directive.codeblock2math(code.split('\n')))
        #expected = 'z &= x\n&= x\ny &= diff(z, x)\n&= 1'
        expected = 'z &= x\n&= x\ny &= 0\n&= 1'
        self.assertEqual(res, expected)

    def test_mathify_equality_test(self):
        res = py_directive.mathify('x = 1 == 1')
        #expected = 'x &= 1 == 1' # Need to find a solution.
        expected = 'x &= \\mathrm{True}'
        self.assertEqual(res, expected)
        res = py_directive.mathify('x = 1 == 1', eval_rhs=True)
        self.assertEqual(res, 'x &= \\mathrm{True}')

    def test_code2math_float(self):
        res = py_directive.code2math('1.2345')
        self.assertEqual(res, '1.2345')
        
    def test_mathify_float(self):
        res = py_directive.mathify('1.23451', prec=3, eval_rhs=True)
        self.assertEqual(res, '1.23')
        res = py_directive.mathify('1.2345123', prec=6, eval_rhs=True)
        self.assertEqual(res, '1.23451')

    def test_mathify_one_equation(self):
        res = py_directive.mathify('x = 1')
        self.assertEqual(res, 'x &= 1')

    def test_mathify_int(self):
        res = py_directive.mathify('1')
        self.assertEqual(res, '1')

    def test_mathify_onlyrhs(self):
        res = py_directive.mathify('x = 1', return_lhs=False)
        self.assertEqual(res, '&= 1')
        res2 = py_directive.mathify('x = 1', return_lhs=False, eval_rhs=True)
        self.assertEqual(res, res2)

    def text_codeblock2math_singleline_1_equation(self):
        res = py_directive.codeblock2math('x=1', return_source=False)
        self.assertEqual(res, 'x &= 1')

    def test_codeblock2math_singleline_2_equations(self):
        code = 'x=1\ny=2'
        res = '\n'.join(py_directive.codeblock2math(code.split('\n'), return_source=False))
        self.assertEqual(res,'x &= 1\ny &= 2')

    def test_codeblock2math_list(self):
        code = 'x=[1,2,3]'
        res = ''.join(py_directive.codeblock2math([code], return_source=False))
        self.assertEqual(res,'x &= \\begin{bmatrix}1, & 2, & 3\\end{bmatrix}')

    def test_codeblock2math_multiline_1_equation(self):
        code = 'x=[1,2,3,\n4,5,6]'
        res = '\n'.join(py_directive.codeblock2math(code.split('\n'), return_source=False))
        self.assertEqual(res,'x &= \\begin{bmatrix}1, & 2, & 3, & 4, & 5, & 6\\end{bmatrix}')

    def test_docutils_basic(self):  
        res = publish_parts('.. py::\n   x = 1', writer_name='latex')['body']
        self.assertEqual(res, '%\n\\begin{equation*}\nx = 1\n\\end{equation*}')

    def test_docutils_import(self):
        res = publish_parts('.. py::\n   :suppress:\n\n   import os', writer_name='latex')['body']
        self.assertEqual(res, '')


    def test_one_D_array(self):
        exec 'from numpy import array' in py_directive.config['namespace']
        code = 'y=array([1,2,3])'
        res = ''.join(py_directive.codeblock2math([code], return_source=False))
        self.assertEqual(res, 'y &= \\begin{bmatrix}{}1 & 2 & 3\\end{bmatrix}')


    def test_two_D_array(self):
        exec 'from numpy import array' in py_directive.config['namespace']        
        code = 'x=array([[1.0,2],[3,4]])'
        res = ''.join(py_directive.codeblock2math([code], return_source=False))
        self.assertEqual(res, 'x &= \\begin{bmatrix}{}1.0 & 2.0\\\\3.0 & 4.0\\end{bmatrix}')

        

        
if __name__ == '__main__':
    unittest.main()
