from distutils.core import setup, Extension
from cPickle import load, dump
import os

module1 = Extension('pyobarr', sources = ['pyobarr.c', 'obarr.c', 'cgrid.c', 'red_black_tree.c', 'misc.c'])

buildno = 0
if os.path.exists('buildno'):
	buildno = load(open('buildno', 'rb'))
	buildno += 1
dump(buildno, open('buildno', 'wb'))

setup (name = 'py3dutil',
	version = '0.1.%.4d' % (buildno,),
	description = 'Accelerator library for 3d games',
	author = 'Bradley Lawrence',
	author_email = 'py3dutil@iambitter.org',
	long_description = '''
Does arrays, vectors, and collision. Does them fast. Oh my yes.
''',
	ext_modules = [module1])

