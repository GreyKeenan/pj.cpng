
png = assets/uncompressed.png

cfiles = \
	main.c \
	pix_as_imt.c \
	\
	gunc/log.c \
	gunc/files.c \
	gunc/byteBalloon.c \
	gunc/sequence.c \
	gunc/bitStream.c \
	gunc/byteSeq64.c \
	\
	whine/all.c \
	\
	zoop/all.c \
	\
	sdaubler/convert.c \
	sdaubler/display.c \


d:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		\
		$(cfiles) \
		-lSDL2 \

debug:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		-DDEBUG \
		\
		$(cfiles) \
		-lSDL2 \

run: a.out
	./a.out $(png)
runlog: a.out
	./a.out $(png) 2> proglog.txt
	cat proglog.txt
clean:
	rm a.out
