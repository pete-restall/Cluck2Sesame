OBJ_FILES+=\
	$(ASSERT_NO_ISR_OBJ_FILE) \
	../../ResetFlagsStubs.o \
	../PollAfterClockMock.o

HEX_FILES=\
	PollWithNoTickedFlagTest.hex \
	PollWithTickedFlagTest.hex

POLL_WITH_NO_TICKED_FLAG_OBJ_FILES=\
	PollWithNoTickedFlagTest.o

POLL_WITH_TICKED_FLAG_OBJ_FILES=\
	PollWithTickedFlagTest.o

all: $(HEX_FILES)

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)

PollWithNoTickedFlagTest.hex: $(OBJ_FILES) $(POLL_WITH_NO_TICKED_FLAG_OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES) $(POLL_WITH_NO_TICKED_FLAG_OBJ_FILES)

PollWithTickedFlagTest.hex: $(OBJ_FILES) $(POLL_WITH_TICKED_FLAG_OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES) $(POLL_WITH_TICKED_FLAG_OBJ_FILES)

include $(MAKEFILE_RULES)
