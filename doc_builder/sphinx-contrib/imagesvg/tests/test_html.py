# -*- coding: utf-8 -*-

from sphinx_testing import with_app

@with_app(srcdir='docs', buildername='html')
def test_build_image(app, status, warning):
    app.build()
    html = (app.outdir / 'index.html').read_text(encoding='utf-8')
    print(html)
    assert '<img alt="regular inclusion" src="_images/test.svg" />' in html
    assert '<object alt="tagtype absent" data="_images/test.svg" type="image/svg+xml">' in html
    assert '<object alt="tagtype object" data="_images/test.svg" type="image/svg+xml">' in html
    assert '<iframe alt="tagtype iframe" src="_images/test.svg">' in html

