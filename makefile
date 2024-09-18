d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		-lSDL2 \
		\
		main.c \
			utils/*.c \
			mike/mike.c \
			mike/deflate.c \

run: a.out
	./a.out
clean:
	rm ./a.out
