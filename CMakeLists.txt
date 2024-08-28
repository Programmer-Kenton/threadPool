cmake_minimum_required(VERSION 3.30)
project(threadPool)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        threadPool main.cpp
        head/ThreadPool.h
        head/Any.h
        head/Semaphore.h
        head/Task.h
        head/MyTask.h
        source/ThreadPool.cpp
        source/Any.cpp
        source/Semaphore.cpp
        source/Task.cpp
        source/MyTask.cpp
)
