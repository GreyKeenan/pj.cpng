
assetpath = assets/
png = PNG_transparency_demonstration.png

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
	./a.out $(assetpath)$(png)
clean:
	rm a.out
