HEX_FILES=\
	TimeNotElapsedTest.hex \
	TimeElapsedTest.hex

TEST_DOUBLE_OBJ_FILES=\
	../../PollAfterLcdMock.o \
	$(PLATFORM_TESTS_DIR)/ShiftRegister/ShiftRegisterDummies.o \
	../../AdcDummies.o \
	../../../Motor/MotorDummies.o

TIME_NOT_ELAPSED_OBJ_FILES=\
	TimeNotElapsedTest.o \
	$(TEST_DOUBLE_OBJ_FILES) \
	$(PLATFORM_DIR)/Arithmetic32.a

TIME_ELAPSED_OBJ_FILES=\
	TimeElapsedTest.o \
	$(TEST_DOUBLE_OBJ_FILES) \
	$(PLATFORM_DIR)/Arithmetic32.a

all: $(HEX_FILES)

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)

TimeNotElapsedTest.hex: $(OBJ_FILES) $(TIME_NOT_ELAPSED_OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES) $(TIME_NOT_ELAPSED_OBJ_FILES)

TimeElapsedTest.hex: $(OBJ_FILES) $(TIME_ELAPSED_OBJ_FILES)
	$(LD) $(LDARGS) -o $@ $(OBJ_FILES) $(TIME_ELAPSED_OBJ_FILES)

include $(MAKEFILE_RULES)
