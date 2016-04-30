# --- CONFIG
ifeq	(1,1)					# make these equal to enable POW code
	CFLAGS += -DPOW
	# OpenSSL
	# statically linking OpenSSL avoids runtime lib version incompatibility
	# statically linking OpenSSL may require you to distribute its license with bins
	ifeq (1,1)				# make these equal to statically link OpenSSL
		OPENSSL_MOJO = /usr/lib/i386-linux-gnu/libcrypto.a		# you might need to change this
	else
		OPENSSL_MOJO = -lcrypto
	endif
endif

FLAG_PUB = "\"not_the_flag{reckon you could leak the server's flag?}\""
FLAG_PRIV = "\"flag{I've_b33n_fr@m3d}\""
# --- END CONFIG

# enable all warnings
CFLAGS += -Wall

# x86 for disassembly in IDA Free
CFLAGS += -m32

# disable stack frame alignment
CFLAGS += -mpreferred-stack-boundary=2

# enable ASLR
CFLAGS += -pie

trashtalker: trashtalker.c
	@echo '[+]' Making $@ for public distribution with embedded flag $(FLAG_PUB)
	$(CC) $(CFLAGS)	-DFLAG=$(FLAG_PUB) $<	-o $@	$(OPENSSL_MOJO)

trashtalker_priv: trashtalker.c
	@echo '[+]' Making $@ NOT FOR PUBLIC DISTRIBUTION with embedded flag $(FLAG_PRIV)
	$(CC) $(CFLAGS)	-DFLAG=$(FLAG_PRIV) $<	-o $@	$(OPENSSL_MOJO)

all: trashtalker trashtalker_priv

clean:
	rm -f trashtalker trashtalker_priv
