d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		main.c \
			utils/autophagicSequence.c \
				utils/expandingWriter.c \
			glass/all.c \
			xylb/all.c \
			puff/all.c \
			sdaubler/display.c \
				sdaubler/convert.c \
		-lm \
		-lSDL2 \

run: a.out
	./a.out
clean:
	rm ./a.out



