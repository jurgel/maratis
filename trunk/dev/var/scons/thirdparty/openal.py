# coding: utf-8
# define the OpenAL library include/obj/link flags for the various platforms
# 2011-02-06, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-09-14 17:12:57 +0700 (Wed, 14 Sep 2011) $
# $LastChangedBy: anael.seghezzi@gmail.com $


import sys, os

def getOpenALParameters():
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
        # OpenAL settings for win32
        params['includePath'].append('openal/include')
        params['libPath'].append('openal/win32')
        params['libs'].append('OpenAL32')
        pass
    elif sys.platform=='darwin':
        # OpenAL settings for osx
        params['frameworks'] = 'OpenAL'
        pass
    elif sys.platform=='linux2' or sys.platform=='linux3':
        # OpenAL settings for linux
        params['includePath'].append('openal/include')
        params['libPath'].append('openal/linux')
        params['libs'].append('openal')
        pass
    
    return params


def addOpenALToEnv(env):
    """ add OpenAL parameters to the build environment"""
    # retrieve params
    params = getOpenALParameters()
    # convert includepath to the complete 3rdparty dir
    vincludepath = []
    for pth in params['includePath']:
        vincludepath.append(os.path.join(env['thirdpartydir'],pth))
    # convert libpath to the corresponding variant dir
    vlibpath = []
    for pth in params['libPath']:
        buildpath = os.path.join(env['thirdpartydir'],pth)
        vlibpath.append(buildpath)
    # append specific params to the build environment
    env.AppendUnique(CPPPATH = vincludepath)
    env.AppendUnique(LIBPATH = vlibpath)
    env.AppendUnique(LIBS = params['libs'])
    env.AppendUnique(FRAMEWORKS = params['frameworks'])
    pass


if __name__ == '__main__':
    params = getOpenALParameters()
    print '  --- Values ---'
    print 'OpenAL Defines:      ' + str(params['defines'])
    print 'OpenAL Objs:         ' + str(params['objs'])
    print 'OpenAL Include Path: ' + str(params['includePath'])
    print 'OpenAL Lib Path:     ' + str(params['libPath'])
    print 'OpenAL Libs:         ' + str(params['libs'])
    print 'OpenAL Frameworks:   ' + str(params['frameworks'])

