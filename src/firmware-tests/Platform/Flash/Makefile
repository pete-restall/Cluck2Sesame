export SUT_OBJ_FILE=$(PLATFORM_DIR)/Flash.o
export OBJ_FILES=\
	$(SUT_OBJ_FILE) \
	$(PLATFORM_CORE_OBJ_FILES) \
	$(TEST_FIXTURE_OBJ_FILES) \
	$(ASSERT_NO_ISR_OBJ_FILE)

all:
	$(MAKE) -C ReadFlashWord all
	$(MAKE) -C ReadFlashWordAsPairOfSevenBitBytes all

clean:
	$(RM) -f $(BUILD_ARTEFACT_PATTERN)
	$(MAKE) -C ReadFlashWord clean
	$(MAKE) -C ReadFlashWordAsPairOfSevenBitBytes clean

include $(MAKEFILE_RULES)
