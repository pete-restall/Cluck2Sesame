export SUT_OBJ_FILE=$(PLATFORM_DIR)/TemperatureSensor.a
export OBJ_FILES=\
	$(SUT_OBJ_FILE) \
	$(PLATFORM_CORE_OBJ_FILES) \
	$(TEST_FIXTURE_OBJ_FILES) \
	$(ASSERT_NO_ISR_OBJ_FILE) \
	$(CURDIR)/InitialiseAfterTemperatureSensorMock.o \
	$(CURDIR)/PollAfterTemperatureSensorMock.o

all: InitialiseAfterTemperatureSensorMock.o PollAfterTemperatureSensorMock.o
	$(MAKE) -C Initialisation all
	# $(MAKE) -C EnableDisable all
	$(MAKE) -C Poll all

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)
	$(MAKE) -C Initialisation clean
	# $(MAKE) -C EnableDisable clean
	$(MAKE) -C Poll clean

include $(MAKEFILE_RULES)
