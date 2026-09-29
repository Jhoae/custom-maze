include config.make

ifndef OF_ROOT
$(error Set OF_ROOT to an openFrameworks 0.12.1 macOS SDK directory, or use ./build.sh)
endif

include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk
