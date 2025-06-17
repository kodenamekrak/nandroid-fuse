project('nandroid', 'cpp', version: '0.0.1')

nandroid_shared_lib = static_library('nandroid_shared',
    sources: ['nandroidfs/nandroid_shared/path_utils.cpp', 'nandroidfs/nandroid_shared/requests.cpp', 'nandroidfs/nandroid_shared/responses.cpp', 'nandroidfs/nandroid_shared/serialization.cpp'],
    include_directories: ['nandroidfs'],
    cpp_args: ['-std=c++20']
)

nandroid_shared_dep = declare_dependency(
    link_with: nandroid_shared_lib,
    include_directories: ['nandroidfs']
)

nandroid_daemon_target = custom_target('nandroid-daemon',
    output: 'nandroid-daemon',
    command: ['build_daemon.sh', '@OUTPUT@'],
    depend_files: ['nandroidfs/nandroid_daemon/src/main.cpp', 'nandroidfs/nandroid_daemon/src/ClientHandler.cpp', 'nandroidfs/nandroid_daemon/src/UnixException.cpp'],
)

fuse_dep = dependency('fuse3')
boost_dep = dependency('boost', modules: ['filesystem'])

subdir('src')
executable('nandroid', 
    sources: [cpp_files, nandroid_daemon_target],
    cpp_args: ['-std=c++23', '-D_FILE_OFFSET_BITS=64', '-DFUSE_USE_VERSION=31'],
    dependencies: [nandroid_shared_dep, fuse_dep, boost_dep,],
)
