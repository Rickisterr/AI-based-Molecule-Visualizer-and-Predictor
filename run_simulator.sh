#!/bin/bash

filename=visualizer
build=$1

if [ -e "src/$filename.cpp" ]
then
    case $build in
    "build")
        if [ ! -e "cpp/build" ]
        then
            mkdir "cpp/build"
        fi
        g++ cpp/src/$filename.cpp cpp/src/glad.c cpp/src/test_axes.cpp -o cpp/build/$filename.exe -I cpp/include -L cpp/lib -lglew32 -lglfw3 -lopengl32 -lgdi32
        ;;

    "run")
        ./cpp/build/$filename.exe
        ;;

    *)
        echo "ValueError: Argument should only be 'build' or 'run'"
        ;;
    esac
else
    echo "NameError: File does not exist"
fi
