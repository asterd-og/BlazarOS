all: compile run

compile:
	gcc blazfs.c -o out/blazfs
	gcc blazfs_view.c -o out/blazfs_view

run:
	$(MAKE) -C ../exec/src
	cp ../exec/out/terminal .
	./out/blazfs theme.tga theme_blue.tga wpp.tga terminal

view:
	./out/blazfs_view out/blazfs.fs
