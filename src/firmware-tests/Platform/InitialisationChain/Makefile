HEX_FILES=FirstInitialisationInChainTest.hex

OBJ_FILES=\
	$(PLATFORM_CORE_OBJ_FILES) \
	$(TEST_FIXTURE_OBJ_FILES) \
	$(ASSERT_NO_ISR_OBJ_FILE) \
	FirstInitialisationInChainTest.o \
	InitialiseAfterFirstInChainDummy.o \
	$(PLATFORM_DIR)/Clock.a

all: $(HEX_FILES)

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)

FirstInitialisationInChainTest.hex: $(OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES)

include $(MAKEFILE_RULES)
