NAME=proj3

all: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps

clean:
	rm $(NAME).aux $(NAME).dvi $(NAME).log $(NAME).pdf $(NAME).ps

zip:
	zip xmarus06-fit.zip Makefile $(NAME).tex
