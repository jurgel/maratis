# build the plugin binaries
import os
import sys
import datetime,time


basepath = str(Dir('#').abspath)
sys.path.append(os.path.join(basepath,'var','scons'))

#from version_tag import *
import compileroptions
    
###############################################
#                ENVIRONMENT                  #
###############################################

# get additional build parameters from configuration file
opts = SConscript('#var/scons/EnvOptions')

# create the main environment
env = Environment( options = opts)

env.Append(BASEPATH=basepath)
env.Append(ENV = {'BASEPATH' : str(Dir('#').abspath)})

# configure the building environment
SConscript('#var/scons/EnvConfig', exports='env')

# update the help screen
Help(opts.GenerateHelpText(env))

# save an option file with the differences from the default values
fname = basepath+"/user_configuration"
opts.Save(fname,env)
if (os.path.getsize(fname) == 0):
    os.remove(fname)


###############################################
#                   BUILD                     #
###############################################
                        
# get build mode from environment
flags = compileroptions.CompilationFlags(env)

print "Build in", env['build_mode'], "mode"
builddir = os.path.join(env['build_area'], sys.platform, env['build_mode'])
env['builddir'] = builddir
env.AppendUnique( CCFLAGS = flags.getCppFlags())
env.AppendUnique( CPPDEFINES = flags.getCppDefines())

# build 3rdparty libraries
thirdpartydir = os.path.abspath(os.path.join('..','..','3rdparty'))
print 'Using 3rdparty libraries in folder:',thirdpartydir
env['thirdpartydir'] = thirdpartydir
SConscript( os.path.join(thirdpartydir,'SConscript'),
            variant_dir = os.path.join(builddir,'3rdparty'),
            duplicate=False,
            exports='env' )

# build maratis3d SDK
SConscript( 'MSDK/SConscript',
            variant_dir = os.path.join(builddir,'MSDK'),
            duplicate=False,
            exports='env' )

# build maratis3d
SConscript( 'Maratis/SConscript',
            variant_dir = os.path.join(builddir,'Maratis'),
            duplicate=False,
            exports='env' )

# build temp
SConscript( 'temp/SConscript',
            variant_dir = os.path.join(builddir,'temp'),
            duplicate=False,
            exports='env' )


