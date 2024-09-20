d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		-lSDL2 \
		\
		main.c \
			mike/mike.c \
				mike/decompress/decompress.c \

run: a.out
	./a.out
clean:
	rm ./a.out
