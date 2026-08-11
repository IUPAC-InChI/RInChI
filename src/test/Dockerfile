FROM debian:bookworm-slim AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential ca-certificates git libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY src /workspace/RInChI/src
RUN git clone --depth 1 https://github.com/IUPAC-InChI/InChI.git /workspace/InChI

WORKDIR /workspace/RInChI/src/test
# Compile InChI C sources with gcc (C compiler) to avoid C/C++ linkage conflicts,
# then compile RInChI C++ sources and link with g++.
RUN set -e; \
    IBSRC=/workspace/InChI/INCHI-1-SRC/INCHI_BASE/src; \
    ILSRC=/workspace/InChI/INCHI-1-SRC/INCHI_API/libinchi/src; \
    RINCHI=/workspace/RInChI/src; \
    mkdir /tmp/objs && cd /tmp/objs && \
    gcc -O2 -DTARGET_API_LIB -DCOMPILE_ANSI_ONLY \
        -I"${IBSRC}" -I"${ILSRC}" \
        -c \
        "${IBSRC}"/*.c \
        "${ILSRC}/ichilnct.c" \
        "${ILSRC}/inchi_dll.c" \
        "${ILSRC}/inchi_dll_a.c" \
        "${ILSRC}/inchi_dll_a2.c" \
        "${ILSRC}/inchi_dll_b.c" && \
    g++ -std=c++17 -O2 -s -DTARGET_API_LIB \
        -I"${RINCHI}/lib" \
        -I"${RINCHI}/rinchi" \
        -I"${RINCHI}/parsers" \
        -I"${RINCHI}/writers" \
        -I"${RINCHI}/rinchi_lib" \
        -I"${IBSRC}" \
        -I"${ILSRC}" \
        /workspace/RInChI/src/test/rinchi_regression_test.cpp \
        "${RINCHI}"/lib/*.cpp \
        "${RINCHI}"/rinchi/*.cpp \
        "${RINCHI}"/parsers/*.cpp \
        "${RINCHI}"/writers/*.cpp \
        "${RINCHI}/rinchi_lib/rinchi_lib.cpp" \
        /tmp/objs/*.o \
        -lboost_system -lstdc++fs \
        -o /tmp/rinchi-regression-test

FROM debian:bookworm-slim AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates libboost-system1.81.0 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /tmp/rinchi-regression-test /usr/local/bin/rinchi-regression-test
RUN chmod +x /usr/local/bin/rinchi-regression-test

ENTRYPOINT ["/usr/local/bin/rinchi-regression-test"]
