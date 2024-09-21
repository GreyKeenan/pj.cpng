d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		-lSDL2 \
		\
		main.c \
			utils/autophagicSequence.c \
			mike/mike.c \
				mike/decompress/decompress.c \
				mike/dechunk/dechunk.c \
				mike/defilter/defilter.c

run: a.out
	./a.out
clean:
	rm ./a.out
