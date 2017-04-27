include ./make/arch.mk

targets := test
midobjs	:= ./*.o ./*.d

include ./make/rules.mk

$(eval $(call LinkApp, $(targets), main.o))

#$(eval $(call LinkApp, $(targets), main.o))


xxx: clean 
	make ARCH=mt7688;
	make scp;

scp :
	scp -P 22 ./test root@192.168.10.101:/tmp

