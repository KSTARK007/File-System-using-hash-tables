pwd
gcc -c main_fun.c && gcc -Wall main_fun.o main.c `pkg-config fuse --cflags --libs` -o main
mkdir file
./main -f file
