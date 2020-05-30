BuildEditor = False

OverEngineFiles = [
    Glob('OverEngine/src/**.cpp'),
    Glob('OverEngine/src/**.h')
]
StaticLibrary('OverEngine', OverEngineFiles)

OverPlayerExecFiles = [
    Glob('OverPlayerExec/src/**.cpp'),
    Glob('OverPlayerExec/src/**.h')
]
Program('OverPlayerExec', OverPlayerExecFiles, LIBS=['OverEngine'], LIBPATH='.', INCPATH='OverEngine/vendor/spdlog/include')


if BuildEditor:
    OverEditorFiles = [
        Glob('OverEditor/src/**.cpp'),
        Glob('OverEditor/src/**.h')
    ]
    StaticLibrary('OverEditor', OverEditorFiles)

    OverEditodExecFiles = [
        Glob('OverEditodExec/src/**.cpp'),
        Glob('OverEditodExec/src/**.h')
    ]
    Program('OverEditodExec', OverEditodExecFiles)