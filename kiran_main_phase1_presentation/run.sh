pwd
gcc -c -w main_fun.c && gcc -w main_fun.o main.c `pkg-config fuse --cflags --libs` -o main
mkdir file
./main -f file
