#Digital Mars (was Symantec) C++ makefile 
all:
        make -f client.sc
        make -f server.sc

clean:
        make -f client.sc clean
        make -f server.sc clean
