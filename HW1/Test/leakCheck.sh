clang ../lab1.c -o lab1
valgrind --leak-check=full -v ./lab1 in.txt out.txt
rm lab1
