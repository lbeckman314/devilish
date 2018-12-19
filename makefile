devilish: devilish.c
	gcc -o devilish.out devilish.c

clean:
	rm -f devilish

uninstall:
	rm -f devilish
	rm -f devilish.c
	rm -f README
	rm -f makefile
	rm -f devilish-beckmanl.zip

zip:
	zip devilish-beckmanl.zip devilish.c README makefile
