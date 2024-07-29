# Multi-Threaded Pacman

Welcome to Multi Threaded Pacman project!

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)

## Introduction
The project is a multi-threaded version of the classic Pac-Man game using synchronization techniques to manage interactions between Pac-Man and multiple computer simulated ghosts. The game handles data conflicts and ensure smooth gameplay with each entity operating on its own thread. It involves the use of Semaphores to synchronize different threads.

## Features
- Seperate threads for each entity
- Synchronizating scenarios based on present models
- Implementation of Lives system

## Requirements
- gcc compiler to compile files
- GLUT and SOIL Library

## Installation
### Installation of GLUT and SOIL Library
Use the following commands to download and install GLUT library
- sudo apt-get update
- sudo apt-get install build-essential
- sudo apt-get install freeglut3 freeglut3-dev

Use the following commands to download and install SOIL library
- sudo apt-get update
- sudo apt-get install libsoil-dev


### Compile the Project
Navigate to the project directory and use the following command to compile the project:
gcc -pthread -o exe main.c -lglut -lGLU -lGL -lSOIL

### Executing the Project
./exe
