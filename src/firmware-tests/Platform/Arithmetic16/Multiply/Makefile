HEX_FILES=\
	Mul16x16Test.hex

MUL16X16_OBJ_FILES=\
	Mul16x16Test.o

all: $(HEX_FILES)

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)

Mul16x16Test.hex: $(OBJ_FILES) $(MUL16X16_OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES) $(MUL16X16_OBJ_FILES)

include $(MAKEFILE_RULES)
