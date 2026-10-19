GCC = nspire-gcc
GXX = nspire-g++
LD = nspire-ld
GENZEHN = genzehn

GXXFLAGS = -Wall -Wextra -O2 -std=c++11
LDFLAGS = -lndls

OBJS = src/main.o
EXE = nspireai.elf
DIST = nspireai.tns

all: $(DIST)

src/%.o: src/%.cpp
	$(GXX) $(GXXFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

$(DIST): $(EXE)
	$(GENZEHN) --name "NspireAI" --input $< --output $@

clean:
	rm -f src/*.o $(EXE) $(DIST)
