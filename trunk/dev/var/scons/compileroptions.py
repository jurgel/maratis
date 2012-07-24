# define the compilation flags for the various compiler/build types
# 2007-07-02, Davide Bacchet (davide.bacchet@gmail.com)
# Last Modified, 2007-07-19, dbs

import sys

class CompilationFlags:
    "define and get the compilation flags, depending on the compiler, build mode and current environment settings"

    # initializations
    def __init__(self, env):
        self.build_mode = env['build_mode']
        if (sys.platform=='win32'):
            # compilation flags for c++ (msvc compiler)
            self.CppCommonFlags  = '/EHsc /nologo /GR /FC'
            self.CppDebugFlags   = '/Z7 /W4 /Od /MDd'
            self.CppReleaseFlags = '/W1 /O2 /Ot /MD'
            # preprocessor definitions
            self.CppCommonDefines  = 'WIN32 _MBCS'
            self.CppDebugDefines   = '_DEBUG __Debug__'
            self.CppReleaseDefines = 'NDEBUG'
        elif (sys.platform=='darwin'):
            # compilation flags for c++
            self.CppCommonFlags  = '-arch i386 -arch x86_64 -isysroot /Developer/SDKs/MacOSX10.6.sdk'
            self.CppDebugFlags   = '-g'
            self.CppReleaseFlags = ''
            # preprocessor definitions
            self.CppCommonDefines  = 'OSX'
            self.CppDebugDefines   = '__Debug__'
            self.CppReleaseDefines = ''
        else:
            # compilation flags for c++
            self.CppCommonFlags  = '-fPIC'
            self.CppDebugFlags   = '-g'
            self.CppReleaseFlags = ''
            # preprocessor definitions
            self.CppCommonDefines  = 'LINUX'
            self.CppDebugDefines   = '__Debug__'
            self.CppReleaseDefines = ''
                      
        

    # get debug flags
    def getCppDebugFlags(self):
        " return cpp compiler debug flags "
        return (self.CppCommonFlags + ' ' + self.CppDebugFlags).split()
    
    # get debug defines
    def getCppDebugDefines(self):
        " get the preprocessor definitions for the debug build"
        return (self.CppCommonDefines + ' ' + self.CppDebugDefines).split()
    
    # get release flags
    def getCppReleaseFlags(self):
        " return cpp compiler release flags "
        return (self.CppCommonFlags + ' ' + self.CppReleaseFlags).split()
    
    # get release defines
    def getCppReleaseDefines(self):
        " get the preprocessor definitions for the release build"
        return (self.CppCommonDefines + ' ' + self.CppReleaseDefines).split()
    
    def getCppFlags(self):
        " get the compiler options using the current build mode "
        if (self.build_mode=='debug'):
            return self.getCppDebugFlags()
        else:
            return self.getCppReleaseFlags()
    
    def getCppDefines(self):
        " get the compiler options using the current build mode "
        if (self.build_mode=='debug'):
            return self.getCppDebugDefines()
        else:
            return self.getCppReleaseDefines()
