#
# Makefile for densityplot program
#
# $Id: Makefile,v 1.1.1.1 1994/12/06 05:26:59 jak Exp $
#
# History:
# $Log: Makefile,v $
# Revision 1.1.1.1  1994/12/06 05:26:59  jak
# Initial import of DensityPlot code. -jak
#
#

CC=cc
CFLAGS=-ggdb3 -O
LIBS=

all: densityplot

densityplot: densityplot.cc
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

install: densityplot
	cp densityplot $(HOME)/Apps

