# coding: utf-8
# define the Windows library include/obj/link flags for the various platforms
# 2011-02-06, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-09-14 17:12:57 +0700 (Wed, 14 Sep 2011) $
# $LastChangedBy: anael.seghezzi@gmail.com $


import sys

def getWindowsParameters():
    """get platform dependent parameters"""
    # variables
    params = dict()
    params['defines']     = []
    params['objs']        = []
    params['libs']        = []
    params['includePath'] = []
    params['libPath']     = []
    params['frameworks']  = []
    
    # fill needed params for each supported platform
    # common defs

    # platform specific
    if sys.platform=='win32':
        # Windows settings for win32
        params['libs'] = 'User32 Shell32 Gdi32 Comdlg32 Winmm'.split()
        pass
    elif sys.platform=='darwin':
        # Windows settings for osx
        pass
    elif sys.platform=='linux2' or sys.platform=='linux3':
        # Windows settings for linux
        pass
    
    return params


def addWindowsToEnv(env):
    """ add Windows parameters to the build environment"""
    # retrieve params
    params = getWindowsParameters()
    # append Windows specific params to the build environment
    env.AppendUnique(CPPPATH = params['includePath'])
    env.AppendUnique(LIBPATH = params['libPath'])
    env.AppendUnique(LIBS = params['libs'])
    env.AppendUnique(FRAMEWORKS = params['frameworks'])
    pass


if __name__ == '__main__':
    params = getWindowsParameters()
    print '  --- Values ---'
    print 'Windows Defines:      ' + str(params['defines'])
    print 'Windows Objs:         ' + str(params['objs'])
    print 'Windows Include Path: ' + str(params['includePath'])
    print 'Windows Lib Path:     ' + str(params['libPath'])
    print 'Windows Libs:         ' + str(params['libs'])
    print 'Windows Frameworks:   ' + str(params['frameworks'])

