# define the LibJpeg library include/obj/link flags for the various platforms
# 2011-02-06, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-02-12 01:43:50 +0100 (sam., 12 févr. 2011) $
# $LastChangedBy: dbacchet $


import sys, os

def getLibJpegParameters():
    """get platform dependent parameters"""
    # variables
    params = dict()
    params['defines']     = []
    params['objs']        = []
    params['libs']        = []
    params['includePath'] = []
    params['libPath']     = []
    
    # fill needed params for each supported platform
    # common defs
    params['includePath'].append('libjpeg')
    params['libPath'].append('3rdparty/libjpeg')
    params['libs'].append('jpeg')

    # platform specific
    if sys.platform=='win32':
        # LibJpeg settings for win32
        pass
    elif sys.platform=='darwin':
        # LibJpeg settings for osx
        pass
    elif sys.platform=='linux2':
        # LibJpeg settings for linux
        pass
    
    return params


def addLibJpegToEnv(env):
    """ add LibJpeg parameters to the build environment"""
    # retrieve params
    params = getLibJpegParameters()
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
    params = getLibJpegParameters()
    print '  --- Values ---'
    print 'LibJpeg Defines:      ' + str(params['defines'])
    print 'LibJpeg Objs:         ' + str(params['objs'])
    print 'LibJpeg Include Path: ' + str(params['includePath'])
    print 'LibJpeg Lib Path:     ' + str(params['libPath'])
    print 'LibJpeg Libs:         ' + str(params['libs'])

