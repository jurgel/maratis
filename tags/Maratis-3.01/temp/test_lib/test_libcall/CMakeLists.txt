# Maratis framework build Sconscript
# $LastChangedDate:  $
# $LastChangedBy: dbacc $


import os, sys, fnmatch
import pyutils
import thirdparty

Import('env')
env_local = env.Clone()

# include dirs
env_local.AppendUnique(CPPPATH = ['../shlib1',
                                  '../shlib2',
                                  '../stlib'] )
# lib dirs
env_local.AppendUnique(LIBPATH = ['../../MSDK/MaratisCore',
                                  '../../MSDK/MEngine',
                                  '../../MSDK/MCore',
                                  '../../MSDK/MGui',
                                  '../Common'] )
# needed libs
env_local.AppendUnique(LIBS = ['MaratisCommon',
                               'MaratisCore',
                               'MEngine',
                               'MCore',
                               'MGui'] )

# add 3rdparty libraries parameters
thirdparty.lua.addLuaToEnv(env_local)
thirdparty.bullet.addBulletToEnv(env_local)
thirdparty.openal.addOpenALToEnv(env_local)
thirdparty.opengl.addOpenGLToEnv(env_local)
thirdparty.glee.addGleeToEnv(env_local)
thirdparty.devil.addDevilToEnv(env_local)
thirdparty.tinyxml.addTinyXmlToEnv(env_local)
thirdparty.libpng.addLibPngToEnv(env_local)
thirdparty.libjpeg.addLibJpegToEnv(env_local)
thirdparty.zlib.addZlibToEnv(env_local)
thirdparty.freetype.addFreetypeToEnv(env_local)
thirdparty.libsndfile.addLibSndFileToEnv(env_local)
if sys.platform=='darwin':
    thirdparty.cocoa.addCocoaToEnv(env_local)
if sys.platform=='win32':
    thirdparty.windows.addWindowsToEnv(env_local)

# get list of files (recursive)
files = pyutils.getFilesRecursive(Dir('.').srcnode().abspath,'*.c*')

# build application
maratis_exe  = env_local.Program('Maratis',files,PDB='Maratis.pdb')
Return('maratis_exe')

