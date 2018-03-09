# Makefile for sphinxcontrib-argdoc
date := $(shell date +%Y-%m-%d)

help:
	@echo "sphinxcontrib-argdoc make help"
	@echo " "
	@echo "Please use \`make <target>\`, choosing <target> from the following:"
	@echo "    dist        to make HTML documentation and eggs for distribution"
	@echo "    docs        to make HTML documentation"
	@echo "    python27    to make Python 2.7 egg distribution"
	@echo "    python3     to make Python 3.x egg distribution"
	@echo "    eggs        to make all egg distributions"
	@echo "    dev_egg     to make development release"
	@echo "    cleandoc    to remove previous generated documentation components"
	@echo "    clean       to remove everything previously built"
	@echo " "

docs/source/class_substitutions.txt :
	mkdir -p docs/source
	get_class_substitutions sphinxcontrib-argdoc sphinxcontrib-argdoc
	mv argdoc_substitutions.txt docs/source/class_substitutions.txt

docs/build/html : docs/source/class_substitutions.txt | docs/source/generated
	$(MAKE) html -C docs

docs/source/generated :
	sphinx-apidoc -M -e -o docs/source/generated argdoc

docs : | docs/build/html docs/source/generated

dev_egg :
	python setup.py egg_info -rbdev$(date) bdist_egg

python27 :
	python2.7 setup.py bdist_egg

python3 :
	python3 setup.py bdist_egg

eggs: python27 python3

dist: docs python27 python3

cleandoc : 
	rm -rf docs/build
	rm -rf docs/source/generated
	rm -rf docs/class_substitutions.txt

clean : cleandoc
	rm -rf dist
	rm -rf build
	
.PHONY : docs dist clean cleandoc dev_egg eggs help python27 python3
