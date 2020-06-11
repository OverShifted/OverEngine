import platform, os, sys

SYSTEM_LINUX   = 0
SYSTEM_WINDOWS = 1

systemInfo = platform.system()
if systemInfo == 'Linux':
    systemInfo = SYSTEM_LINUX
elif systemInfo == 'Windows':
    systemInfo = SYSTEM_WINDOWS
else:
    print("Unknown OS")
    quit()

WannaBuild = "-b" in sys.argv[1:] or "--b" in sys.argv[1:] or "--build" in sys.argv[1:] or "-build" in sys.argv[1:]

premakeCommand = "vendor/bin/premake/premake5"
buildCommand = ""
if systemInfo == SYSTEM_WINDOWS:
    premakeCommand += ".exe vs2019"
    buildCommand = "msbuild"
elif systemInfo == SYSTEM_LINUX:
    premakeCommand += " gmake2"
    buildCommand = "make"

print("Generating project files\n")
if os.system(premakeCommand) == 0:
    print("\nProject files generated")
    if WannaBuild:
        os.system(buildCommand)
else:
    print("\nProject files generation failed")