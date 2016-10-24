import sys
import os

AddOption('--s', dest='src', type='string', nargs=1, action='store', help='src file to be build')

src=GetOption('src')
if src == '':
	print 'Error: no source file specified, exit...'
	sys.exit(-1)

env = Environment()

env['CXX'] = 'g++'
#env['CXX'] = 'clang++-3.8'
env['CPPFLAGS'] = [
    '-std=c++1y',
    #'-g',
    '-O3',
    '-Wall',
    '-static',
    ]
env['CPPPATH'] = [
    '/usr/include/',
    '/usr/local/include/',
    '/root/gtest-1.7.0/include/',
    ]
env['LIBPATH'] = [
    '/root/gtest-1.7.0/lib/.libs/',
    '/usr/local/lib/',
    ]
env['LIBS'] = [
    'gtest',
    # Must put pthread after gtest under g++, clang++ not limited. See:
    #   http://stackoverflow.com/a/21116684/342348
    'pthread',
    'folly',
    'glog',
    ]

env.Program('a.out', [src, ])
