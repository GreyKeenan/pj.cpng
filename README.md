GreyKeenan/pj.cpng
==================================================

<br>

<!-- INDEX -->
* Building and Running
* Code Structure, briefly

<br>


This is a C program which decodes a PNG
and prints it to the stdout using ANSI truecolor escape codes.
There are no dependencies in the actual PNG and Zlib/DEFLATE handling,
but the display code uses \<sys/ioctl.h\> and \<unistd.h\> if that's relevant to you.

> I'm not sure why you would want to use this, but it has been a good exercise for me.

* All critical types of PNG are supported.
* For now, images which are too wide for the stdout will be cut off.
* Technically, especially large PNGs will throw errors, where the width * height * bytesPerPixel max out a uint64.
  But, like, that would be insane? So who cares.
* For now, all ancillary chunks (including tRNS, gAMA, and cHRM) are discarded.
* Chunk CRCs are discarded.


Building and Running
--------------------------------------------------

The makefile is pretty simple.

`make` in the root of the repo will build the program to "a.out".
The makefile variable "out" can be set to change the output name.

`make run` will run the program.
The makefile variable "png" can be set to change the png.
Alternatively, you can of course run the program directly,
with the png filepath as the first and only argument.

`make runsuite` will run the program recursively on a dir containing the PNG Test Suite images.
The makefile variable "suite" can be set to change the target directory.

> Note that PNG Test Suite images named with the first letter 'x' are intended to fail.
> Because the program does not check CRCs, two negative tests will pass.


Code Structure, briefly
--------------------------------------------------

`Gunc` groups generalized utilities and interfaces which are not specific to the PNG code.

`Whine` handles the PNG wrapping around the Zlib data.

`Zoop` handles the Zlib header/adler and DEFLATE compression

`Shrub` splits out the huffman tree functionality.
It is not generalized to just any tree,
but splitting it out is useful for organization.

`Pork` has the code which calls to Whine and Zoop in order to combine them.
It is a little unnecessary to make it its own module, to be honest,
but it could become more involved in the future.
