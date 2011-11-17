#!/usr/bin/env python
from distutils.core import setup, Extension
import os

library_dirs = []
include_dirs = []
os.environ['CFLAGS'] = '-std=c99'

setup(name='mcs',
      version='1.0',
      packages=['mcs'],
	  ext_modules = [Extension('mcs._Cmcs', [
	  	'src/bipartite.c',
	  	'src/list.c',
	  	'src/utils.c',
	  	'src/vmaxs.c',
	  	'src/timed.c',
	  	'src/py_wrap.c',
	  ],
		  library_dirs=library_dirs,
		  libraries=['gc'],
		  include_dirs=include_dirs
	  )]
      )
