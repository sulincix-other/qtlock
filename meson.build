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

# -fPIC required
add_project_arguments('-fPIC', language: 'cpp')

# executable file
executable('main', src, dependencies: deps)
