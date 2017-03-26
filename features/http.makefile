FEAT_HTTP ?= 0
ifeq ($(FEAT_HTTP),1)


# Rule to make the static onion.
features/build/onion/src/onion/libonion_static.a:
	[ -e features/onion ]
	mkdir -p features/build/onion
	cd features/build/onion; cmake ../../onion
	cd features/build/onion; make
	touch features/http.check

out/libonion_static.a: out/ features/build/onion/src/onion/libonion_static.a
	cp features/build/onion/src/onion/libonion_static.a out/libonion_static.a


# Add include path for onion headers.
INCLUDES += -Ifeatures/onion/src/


# Add ptheread to the libs list.
LIBS += -lpthread


# Add feature sources and the onion static.
BUIILD_DEPS += out/libonion_static.a
SRC_OBJS += src/features/http.o


# Add the HTTP example.
EXAMPLE_DEPS += out/http_example
out/http_example: examples/http.o out/libpromclient.a out/libonion_static.a
	g++ $(LINK_FLAGS) $(LIBS) -o $@ $^


endif  # $(FEAT_HTTP) == 1
