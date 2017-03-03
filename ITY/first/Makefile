NAME=proj1

all: $(NAME).tex
	pdflatex $(NAME).tex

clean:
	rm $(NAME).aux $(NAME).dvi $(NAME).log $(NAME).pdf

zip:
	zip xmarus06-fit.zip Makefile $(NAME).tex
