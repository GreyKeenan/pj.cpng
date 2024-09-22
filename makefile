d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		main.c \
			utils/autophagicSequence.c \
				utils/expandingWriter.c \
			mike/mike.c \
				mike/decompress/decompress.c \
				mike/dechunk/dechunk.c \
				mike/defilter/defilter.c \
			sdaubler/display.c \
		-lSDL2 \

run: a.out
	./a.out
clean:
	rm ./a.out
