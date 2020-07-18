import platform, os, sys

SYSTEM_NONE    = 0
SYSTEM_LINUX   = 1
SYSTEM_WINDOWS = 2

def DetectOperatingSystem():
    systemInfo = platform.system()
    if systemInfo == 'Linux':
        return SYSTEM_LINUX
    if systemInfo == 'Windows':
        return SYSTEM_WINDOWS
    return SYSTEM_NONE

def HasFlag(flags):
    for flag in flags:
        if flag in sys.argv[1:]:
            return True
    return False

buildFlags = [
    "-b",
    "--b",
    "-build",
    "--build"
]
WannaBuild = HasFlag(buildFlags)

premakeCommand = "vendor/bin/premake/premake5"
buildCommand = ""
if DetectOperatingSystem() == SYSTEM_WINDOWS:
    premakeCommand = "vendor\\bin\\premake\\premake5.exe vs2019"
    buildCommand = "msbuild"
elif DetectOperatingSystem() == SYSTEM_LINUX:
    premakeCommand += " gmake2"
    buildCommand = "make"

print("Generating project files\n")
if os.system(premakeCommand) == 0:
    print("\nProject files generated")
    if WannaBuild:
        os.system(buildCommand)
else:
    print("\nProject files generation failed")
