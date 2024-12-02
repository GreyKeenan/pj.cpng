
png = assets/uncompressed.png

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
	./a.out $(png)
runlog: a.out
	./a.out $(png) 2> proglog.txt
	cat proglog.txt
clean:
	rm a.out
