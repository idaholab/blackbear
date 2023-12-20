include $(BLACKBEAR_DIR)/contrib/neml.mk
include $(BLACKBEAR_DIR)/contrib/neml2.mk

app_non_unity_dirs += $(shell find $(APPLICATION_DIR)/src/nonunity -type d -not -path '*/.libs*' 2> /dev/null)
app_non_unity_dirs += $(shell find $(APPLICATION_DIR)/test/src/nonunity -type d -not -path '*/.libs*' 2> /dev/null)
