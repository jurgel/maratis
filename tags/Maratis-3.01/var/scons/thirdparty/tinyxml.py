# define the TinyXml library include/obj/link flags for the various platforms
# 2011-02-09, Davide Bacchet (davide.bacchet@gmail.com)
# $LastChangedDate: 2011-02-12 01:43:50 +0100 (sam., 12 févr. 2011) $
# $LastChangedBy: dbacchet $


import sys, os

def getTinyXmlParameters():
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
    params['includePath'].append('tinyxml')
    params['libPath'].append('3rdparty/tinyxml')
    params['libs'].append('tinyxml')

    # platform specific
    if sys.platform=='win32':
        # TinyXml settings for win32
        pass
    elif sys.platform=='darwin':
        # TinyXml settings for osx
        pass
    elif sys.platform=='linux2':
        # TinyXml settings for linux
        pass
    
    return params


def addTinyXmlToEnv(env):
    """ add TinyXml parameters to the build environment"""
    # retrieve params
    params = getTinyXmlParameters()
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
    params = getTinyXmlParameters()
    print '  --- Values ---'
    print 'TinyXml Defines:      ' + str(params['defines'])
    print 'TinyXml Objs:         ' + str(params['objs'])
    print 'TinyXml Include Path: ' + str(params['includePath'])
    print 'TinyXml Lib Path:     ' + str(params['libPath'])
    print 'TinyXml Libs:         ' + str(params['libs'])

