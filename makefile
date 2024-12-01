d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		main.c \
		-lSDL2 \

debug:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		-DDEBUG \
		\
		main.c \
		-lSDL2 \

run: a.out
	./a.out
clean:
	rm a.out



