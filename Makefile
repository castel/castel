BINARY    = castelc

CXX       = g++
MKDIR     = mkdir
RM        = rm

SRCS      = $(shell find sources -name '*.cc')
HDRS      = $(shell find includes -name '*.hh')
OBJS      = $(addprefix build/objects/,$(SRCS:.cc=.o))
DEPS      = $(addprefix build/dependencies/,$(SRCS:.cc=.d))

PURPLE    = $(shell printf "\033[35m")
CYAN      = $(shell printf "\033[36m")
GREEN     = $(shell printf "\033[32m")
BROWN     = $(shell printf "\033[33m")
EOS       = $(shell printf "\033[00m")

CXXFLAGS  = -fPIC -std=c++11 -I./libcastel/includes -I./includes -I.
CXXFLAGS += -g3 -O0 $(shell llvm-config --cxxflags) -fexceptions -g3 -O0

LDFLAGS  += $(shell llvm-config --ldflags) $(shell llvm-config --libs core jit native) -rdynamic -L./libcastel/build -lCastelEngine -lCastelParse -Wl,--whole-archive -lCastelRuntime -Wl,--no-whole-archive

all: $(BINARY)
	@printf "Compilation done, output is build/${BINARY}\n"

$(BINARY): build/$(BINARY)

-include $(DEPS)

build/$(BINARY): $(OBJS) libcastel/build/libCastelParse.a libcastel/build/libCastelEngine.a libcastel/build/libCastelRuntime.a
	@printf "%s# Linking final executable.%s\n" "${PURPLE}" "${EOS}"
	@${CXX} -o build/${BINARY} ${OBJS} ${LDFLAGS}

libcastel/build/libCastelParse.a libcastel/build/libCastelEngine.a libcastel/build/libCastelRuntime.a: build-libcastel ;

build-libcastel:
	@$(MAKE) -s -C libcastel libCastelParse.a libCastelEngine.a libCastelRuntime.a

$(DEPS): build/dependencies/%.d: %.cc | build-libcastel
	@printf "%s+ Generating dependency file for %s.%s\n" "${GREEN}" "${<}" "${EOS}"
	@${MKDIR} -p "$(dir ${@})"
	@${CXX} ${CXXFLAGS} -MM -MG -MT "$(patsubst build/dependencies/%,build/objects/%,$(@:.d=.o))" "${<}" > $(@)

$(OBJS): build/objects/%.o: %.cc
	@printf "%s+ Compiling %s.%s\n" "${GREEN}" "${<}" "${EOS}"
	@${MKDIR} -p "$(dir ${@})"
	@${CXX} ${CXXFLAGS} -c -o "${@}" "${<}"

clean:
	@printf "%s- Removing temporary files.%s\n" "${BROWN}" "${EOS}"
	@${RM} -rf build/generated
	@${RM} -rf build/objects

clean-dependencies:
	@printf "%s- Removing dependencies listing.%s\n" "${BROWN}" "${EOS}"
	@${RM} -rf build/dependencies

fclean: clean
	@printf "%s- Removing binary files.%s\n" "${BROWN}" "${EOS}"
	@${RM} -rf build/${BINARY}

re: clean-dependencies fclean
	@$(MAKE) --no-print-directory all

.PHONY: $(BINARY) all clean fclean re clean-depends build-libcastel
