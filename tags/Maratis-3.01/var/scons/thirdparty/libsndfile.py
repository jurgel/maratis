# define the LibSndFile library include/obj/link flags for the various platforms
# 2011-02-06, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-02-12 01:43:50 +0100 (sam., 12 févr. 2011) $
# $LastChangedBy: dbacchet $


import sys, os

def getLibSndFileParameters():
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
    params['includePath'].append('libsndfile/include')

    # platform specific
    if sys.platform=='win32':
        # settings for win32
        params['libPath'].append('libsndfile/win32')
        params['libs'].append('libsndfile-1')
        pass
    elif sys.platform=='darwin':
        # settings for osx
        params['libPath'].append('libsndfile/osx')
        params['libs'].append('sndfile')
        pass
    elif sys.platform=='linux2':
        # settings for linux
        raise Exception('linux platform not supported yet')
        pass
    else:
        raise Exception('Unknown platform')
    
    return params


def addLibSndFileToEnv(env):
    """ add LibSndFile parameters to the build environment"""
    # retrieve params
    params = getLibSndFileParameters()
    # convert includepath to the complete 3rdparty dir
    vincludepath = []
    for pth in params['includePath']:
        vincludepath.append(os.path.join(env['thirdpartydir'],pth))
    # convert libpath to the corresponding dir
    vlibpath = []
    for pth in params['libPath']:
        buildpath = os.path.join(env['thirdpartydir'],pth)
        vlibpath.append(buildpath)
    # append specific params to the build environment
    env.AppendUnique(CPPPATH = vincludepath)
    env.AppendUnique(LIBPATH = vlibpath)
    env.AppendUnique(LIBS = params['libs'])
    pass


if __name__ == '__main__':
    params = getLibSndFileParameters()
    print '  --- Values ---'
    print 'LibSndFile Defines:      ' + str(params['defines'])
    print 'LibSndFile Objs:         ' + str(params['objs'])
    print 'LibSndFile Include Path: ' + str(params['includePath'])
    print 'LibSndFile Lib Path:     ' + str(params['libPath'])
    print 'LibSndFile Libs:         ' + str(params['libs'])

