FEAT_HTTP ?= 0
ifeq ($(FEAT_HTTP),1)


# Figure out the CMAKE toolchain.
CMAKE_TOOLCHAIN =
CMAKE_TOOLCHAIN_FILE ?=
ifneq ($(CMAKE_TOOLCHAIN_FILE), )
CMAKE_TOOLCHAIN = -DCMAKE_TOOLCHAIN_FILE=$(CMAKE_TOOLCHAIN_FILE)
endif  # $(CMAKE_TOOLCHAIN_FILE) != ''


# Rule to make the static onion.
features/build/onion/src/onion/libonion_static.a:
	[ -e features/onion ]
	mkdir -p features/build/onion
	cd features/build/onion; \
		cmake $(CMAKE_TOOLCHAIN) \
		-DONION_USE_SSL=false \
		-DONION_USE_PAM=false \
		-DONION_USE_PNG=false \
		-DONION_USE_JPEG=false \
		-DONION_USE_XML2=false \
		-DONION_USE_SYSTEMD=false \
		-DONION_USE_SQLITE3=false \
		-DONION_USE_REDIS=false \
		-DONION_USE_GC=false \
		-DONION_USE_TESTS=false \
		-DONION_EXAMPLES=false \
		-DONION_USE_BINDINGS_CPP=false \
		-DONION_USE_PTHREADS=true \
		../../onion
	cd features/build/onion; make onion_static
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
	$(GPP) $(LINK_FLAGS) $(LIBS) -o $@ $^


endif  # $(FEAT_HTTP) == 1
