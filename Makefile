# Makefile                                                                    

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

# Links all of the executables together                                      
all: project image ppm texture_synthesis

# Links together files needed to create project  executable
project: project.o image.o ppm.o texture_synthesis.o
	$(CC) -o project project.o image.o ppm.o texture_synthesis.o -lz -lm

# Links together files needed to create ppm executable                    
ppm: ppm.o project.o image.o texture_synthesis.o
	$(CC) -o ppm ppm.o project.o image.o texture_synthesis.o -lz -lm

# Links together files needed to create image executable
image: image.o project.o ppm.o texture_synthesis.o
	$(CC) -o image image.o project.o ppm.o texture_synthesis.o -lz -lm
                                                                             
# Links together files needed to create texture_synthesis executable
texture_synthesis: texture_synthesis.o image.o project.o ppm.o
	$(CC) -o texture_synthesis texture_synthesis.o image.o project.o ppm.o -lz -lm

# Creates object files                                                        
project.o: project.c image.h ppm.h texture_synthesis.h 
	$(CC) $(CFLAGS) -c project.c

ppm.o: ppm.c ppm.h
	$(CC) $(CLFAGS) -c ppm.c

pnglite.o: pnglite.c pnglite.h
	$(CC) $(CFLAGS) -c pnglite.c

image.o: image.h image.c 
	$(CC) $(CFLAGS) -c image.c

texture_synthesis.o: texture_synthesis.c texture_synthesis.h image.h
	$(CC) $(CFLAGS) -c texture_synthesis.c


# Removes all object files and the executables                                
# so we can start fresh                                                       
clean:
	rm -f *.o texture_synthesis project ppm image
