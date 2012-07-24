# define the Bullet library include/obj/link flags for the various platforms
# 2011-02-06, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-02-12 01:43:50 +0100 (sam., 12 févr. 2011) $
# $LastChangedBy: dbacchet $


import sys, os
import SCons
from SCons.Node.FS import Dir


def getBulletParameters():
    """get platforma dependent parameters"""
    # variables
    params = dict()
    params['defines']     = []
    params['objs']        = []
    params['libs']        = []
    params['includePath'] = []
    params['libPath']     = []
    
    # fill needed params for each supported platform
    # common defs
    params['includePath'].append('bullet')
    params['libPath'].append('3rdparty/bullet')
    params['libs'].append('bullet')

    # platform specific
    if sys.platform=='win32':
        # Bullet settings for win32
        pass
    elif sys.platform=='darwin':
        # Bullet settings for osx
        dir(SCons)
        pass
    elif sys.platform=='linux2':
        # Bullet settings for linux
        pass
    
    return params


def addBulletToEnv(env):
    """ add bullet parameters to the build environment"""
    # retrieve params
    params = getBulletParameters()
    # convert includepath to the complete 3rdparty dir
    vincludepath = []
    for pth in params['includePath']:
        vincludepath.append(os.path.join(env['thirdpartydir'],pth))
    # convert libpath to the corresponding variant dir
    vlibpath = []
    for pth in params['libPath']:
        buildpath = os.path.join(env['builddir'],pth)
        vlibpath.append(buildpath)
    # append specific params to the build environment
    env.AppendUnique(CPPPATH = vincludepath)
    env.AppendUnique(LIBPATH = vlibpath)
    env.AppendUnique(LIBS = params['libs'])
    pass


if __name__ == '__main__':
    params = getBulletParameters()
    print '  --- Values ---'
    print 'Bullet Defines:      ' + str(params['defines'])
    print 'Bullet Objs:         ' + str(params['objs'])
    print 'Bullet Include Path: ' + str(params['includePath'])
    print 'Bullet Lib Path:     ' + str(params['libPath'])
    print 'Bullet Libs:         ' + str(params['libs'])

