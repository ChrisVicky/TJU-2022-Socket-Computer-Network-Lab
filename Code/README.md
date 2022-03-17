# project-1

This repository contains the starter code for ***CMU 15-441/641 Networking and the Internet Project 1: A Web Server Called Liso***.

## 1. Files
- `DockerFile`: Script to build the docker image for the project's environment.
- `Makefile`: Contains rules for `make`.
- `README.md`: Current document.
- `cp1`: CP1 scripts and examples.
- `cp2`: CP2 scripts and examples.
- `cp3`: CP3 scripts and examples.
- `src/`: Source code for the project.
    - `src/echo_client.c`: Simple echo network client.
    - `src/echo_server.c`: Simple echo network server
    - `src/example.c`: Example driver for parsing.
    - `src/lexer.l`: Lex/Yacc related logic.
    - `src/parser.y`
    - `src/parse.c`
- `include/parse.h`

## 2. Environment Setup
1. Install docker: https://www.docker.com
2. Open a terminal and navigate to the directory containing this `README.md` file.
3. Build the docker image: `docker build -t 15-441/641-project-1:latest -f ./DockerFile .`
4. Run the docker container: ``docker run -it -v `pwd`:/home/project-1/ --name <name for your container> 15-441/641-project-1 /bin/bash``
5. The starter code for the project is available at `/home/project-1/` in the container and `.` on your local machine. To make development easier, a mapping is established between these two folders. Modiying the code in one location will also effect the other one. This means that you can use an IDE to write code on your local machine and then seamlessly test it in the container.
6. To test your server using a web browser, you need to configure port mapping for the docker container. Simply add the argument `-p 8888:15441` to the `docker run` command to establish a mapping from `127.0.0.1:15441` in the container to `127.0.0.1:8888` on your local machine. Then you can test your server by using a web browser (e.g., Chrome) on your local machine to navigate to the URL `127.0.0.1:8888`.
