
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
runlog: a.out
	./a.out $(assetpath)$(png) 2> proglog.txt
	cat proglog.txt
clean:
	rm a.out
