d:

test: ../testing
	gcc -std=c99 -Wpedantic \
		-o a.out \
		-iquote . \
		\
		main.c \
			../../all.c \
		-lm \

run:
	./a.out
