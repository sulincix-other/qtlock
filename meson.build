project('qlock', 'cpp', 'c')
# Source files
src = [
    'pam.c',
    'main.cpp'
]

# dependencies
deps = [
    dependency('Qt6Core'),
    dependency('Qt6Gui'),
    dependency('Qt6Widgets'),
    dependency('pam'),
]

add_project_arguments('-DOSK="'+get_option('OSK')+'"', language: 'cpp')

# -fPIC required
add_project_arguments('-fPIC', language: 'cpp')

# executable file
executable('main', src, dependencies: deps)
