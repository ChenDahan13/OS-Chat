all: stnc

stnc: stnc.c
	gcc stnc.c -o stnc

clean:
	rm stnc