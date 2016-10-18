import sys

AddOption('--s', dest='src', type='string', nargs=1, action='store', help='src file to be build')

src=GetOption('src')
if src == '':
	print 'Error: no source file specified, exit...'
	sys.exit(-1)

env = Environment()

env['CXX'] = 'g++'
#env['CXX'] = 'clang++-3.5'
env['CPPFLAGS'] = [
    '-std=c++1y',
    '-g',
    '-O0',
    '-Wall',
    #'-static'
    ]
env['CPPPATH'] = [
    '/usr/include/',
    '/root/folly/include',
    '/root/gtest-1.7.0/include/',
    ]
env['LIBPATH'] = [
    '/root/gtest-1.7.0/lib/.libs/',
    '/root/folly/lib/',
    ]
env['LIBS'] = [
    'gtest',
    # Must put pthread after gtest under g++, clang++ not limited. See:
    #   http://stackoverflow.com/a/21116684/342348
    'pthread',
    'folly',
    ]

env.Program('a.out', [src, ])
