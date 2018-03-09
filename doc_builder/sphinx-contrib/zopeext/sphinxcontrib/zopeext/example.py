from zope.interface import Interface, Attribute

class IMyInterface(Interface):
    """This is an example of an interface."""
    x = Attribute("A required attribute of the interface")

    def f(self, x):
        r"""A required method of the interface."""
        return x*self.x


