HEX_FILES=\
	InitialisationChainTest.hex

INITIALISATION_CHAIN_OBJ_FILES=\
	InitialisationChainTest.o \
	../InitialiseAfterIsrMock.o \
	$(PLATFORM_TESTS_DIR)/PowerManagement/InitialiseAfterPowerManagementDummy.o \
	$(PLATFORM_TESTS_DIR)/PowerManagement/PollAfterPowerManagementDummy.o

all: $(HEX_FILES)

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)

InitialisationChainTest.hex: $(OBJ_FILES) $(INITIALISATION_CHAIN_OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES) $(INITIALISATION_CHAIN_OBJ_FILES)

include $(MAKEFILE_RULES)
