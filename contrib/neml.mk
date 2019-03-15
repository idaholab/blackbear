neml_DIR            := $(BLACKBEAR_DIR)/contrib/neml

ifneq ($(wildcard $(neml_DIR)/CMakeLists.txt),)

#Exclude all the files with _wrap in the name because they are drivers for testing
#that are not needed and have additional dependencies.
neml_srcfiles       := $(shell find $(neml_DIR)/src -name "*.cxx" | grep -v _wrap)
neml_objects        += $(patsubst %.cxx,%.$(obj-suffix),$(neml_srcfiles))
neml_LIB            := $(neml_DIR)/libneml-$(METHOD).la
neml_includes       := $(neml_DIR)/src
neml_dep_includes   := -I$(neml_DIR)/rapidxml

$(APPLICATION_DIR)/lib/libblackbear-$(METHOD).la: $(neml_LIB)

$(neml_LIB): $(neml_objects)
	@echo "Linking Library "$@"..."
	@$(libmesh_LIBTOOL) --tag=CC $(LIBTOOLFLAGS) --mode=link --quiet \
	  $(libmesh_CC) $(libmesh_CFLAGS) -o $@ $(neml_objects) $(libmesh_LDFLAGS) $(EXTERNAL_FLAGS) -rpath $(neml_DIR)
	@$(libmesh_LIBTOOL) --mode=install --quiet install -c $(neml_LIB) $(neml_DIR)

$(neml_DIR)/src/%.$(obj-suffix) : $(neml_DIR)/src/%.cxx
	@echo "Compiling C++ (in "$(METHOD)" mode) "$<"..."
	@$(libmesh_LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=compile --quiet \
	  $(libmesh_CXX) $(libmesh_CPPFLAGS) $(ADDITIONAL_CPPFLAGS) $(libmesh_CXXFLAGS) $(neml_dep_includes) $(app_INCLUDES) $(libmesh_INCLUDE) -w -DHAVE_CONFIG_H -MMD -MP -MF $@.d -MT $@ -c $< -o $@

ADDITIONAL_INCLUDES  += -I$(neml_includes)
ADDITIONAL_LIBS      += -L$(neml_DIR) -lneml-$(METHOD)
ADDITIONAL_CPPFLAGS  += -DNEML_ENABLED

else
$(info WARNING: Not building with NEML because contrib/neml submodule is not present)
$(info See https://github.com/Argonne-National-Laboratory/neml/blob/dev/INSTALL.md)
endif
