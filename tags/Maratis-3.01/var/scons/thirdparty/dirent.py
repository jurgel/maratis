# define the dirent library include flags for the various platforms
# 2011-02-06, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-02-12 01:43:50 +0100 (sam., 12 févr. 2011) $
# $LastChangedBy: dbacchet $


import sys, os

def getDirentParameters():
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

    # platform specific
    if sys.platform=='win32':
        # Dirent settings for win32
        params['includePath'].append('dirent/include')
        pass
    elif sys.platform=='darwin':
        # Dirent settings for osx
        pass
    elif sys.platform=='linux2':
        # Dirent settings for linux
        pass
    
    return params


def addDirentToEnv(env):
    """ add Dirent parameters to the build environment"""
    # retrieve params
    params = getDirentParameters()
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
    params = getDirentParameters()
    print '  --- Values ---'
    print 'Dirent Defines:      ' + str(params['defines'])
    print 'Dirent Objs:         ' + str(params['objs'])
    print 'Dirent Include Path: ' + str(params['includePath'])
    print 'Dirent Lib Path:     ' + str(params['libPath'])
    print 'Dirent Libs:         ' + str(params['libs'])

