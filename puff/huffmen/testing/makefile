d:

test: ../testing
	gcc -std=c99 -Wpedantic \
		-o a.out \
		-iquote . \
		\
		main.c \
			../*.c \
		-lm \

debug: ../testing
	gcc -std=c99 -Wpedantic \
		-o a.out \
		-iquote . \
		\
		-DDEBUG \
		\
		main.c \
			../*.c \
		-lm \

run:
	./a.out
