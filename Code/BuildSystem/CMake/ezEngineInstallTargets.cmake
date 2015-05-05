# Guard against mutliple inclusions during the same run.
# ezEngineInstallTargets_INCLUDED = GLOBAL::ezEngineInstallTargets_INCLUDED
get_property(ezEngineInstallTargets_INCLUDED GLOBAL PROPERTY ezEngineInstallTargets_INCLUDED)
if(ezEngineInstallTargets_INCLUDED)
  return()
endif(ezEngineInstallTargets_INCLUDED)
# ezEngineInstallTargets_INCLUDED = TRUE
set(ezEngineInstallTargets_INCLUDED TRUE)
# GLOBAL::ezEngineInstallTargets_INCLUDED = ezEngineInstallTargets_INCLUDED
set_property(GLOBAL PROPERTY ezEngineInstallTargets_INCLUDED "${ezEngineInstallTargets_INCLUDED}")

######################################################################
### During the configure process, this file will be copied to the  ###
### CMAKE_BINRARY_DIR and add_* lines will be appended.            ###
######################################################################
