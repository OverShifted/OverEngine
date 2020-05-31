env = Environment()

env.Append( CPPPATH=[
    '.',
    'OverEngine/vendor/Glad/include',
    'OverEngine/vendor/GLFW/include',
    'OverEngine/vendor/glm',
    'OverEngine/vendor/imgui',
    'OverEngine/vendor/spdlog/include',
    'OverEngine/src'
])
env.Append(CPPDEFINES = ['OE_PLATFORM_WINDOWS']) 

BuildEditor = False

OverEngineFiles = [
    Glob('OverEngine/src/**/*.cpp'),
    Glob('OverEngine/src/**/*.h')
]
env.StaticLibrary('OverEngine', OverEngineFiles)

OverPlayerExecFiles = [
    Glob('OverPlayerExec/src/**/*.cpp'),
    Glob('OverPlayerExec/src/**/*.h')
]
env.Program('OverPlayerExec', OverPlayerExecFiles, LIBS=['OverEngine'], LIBPATH='.')


if BuildEditor:
    OverEditorFiles = [
        Glob('OverEditor/src/**/*.cpp'),
        Glob('OverEditor/src/**/*.h')
    ]
    env.StaticLibrary('OverEditor', OverEditorFiles)

    OverEditodExecFiles = [
        Glob('OverEditodExec/src/**/*.cpp'),
        Glob('OverEditodExec/src/**/*.h')
    ]
    env.Program('OverEditodExec', OverEditodExecFiles)