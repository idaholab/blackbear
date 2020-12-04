NEML_DIR            ?= $(BLACKBEAR_DIR)/contrib/neml

ifneq ($(wildcard $(NEML_DIR)/CMakeLists.txt),)

#Exclude all the files with _wrap in the name because they are drivers for testing
#that are not needed and have additional dependencies.
neml_srcfiles       := $(shell find $(NEML_DIR)/src -name "*.cxx" | grep -v _wrap)
neml_objects        += $(patsubst %.cxx,%.$(obj-suffix),$(neml_srcfiles))
neml_LIB            := $(NEML_DIR)/libneml-$(METHOD).la
neml_includes       := $(NEML_DIR)/src
neml_dep_includes   := -I$(NEML_DIR)/rapidxml -I$(BLACKBEAR_DIR)/contrib/neml_extra_include

$(APPLICATION_DIR)/lib/libblackbear-$(METHOD).la: $(neml_LIB)

$(neml_LIB): $(neml_objects)
	@echo "Linking Library "$@"..."
	@$(libmesh_LIBTOOL) --tag=CC $(LIBTOOLFLAGS) --mode=link --quiet \
	  $(libmesh_CC) $(libmesh_CFLAGS) -o $@ $(neml_objects) $(libmesh_LDFLAGS) $(EXTERNAL_FLAGS) -rpath $(NEML_DIR)
	@$(libmesh_LIBTOOL) --mode=install --quiet install -c $(neml_LIB) $(NEML_DIR)

$(NEML_DIR)/src/%.$(obj-suffix) : $(NEML_DIR)/src/%.cxx
	@echo "Compiling C++ (in "$(METHOD)" mode) "$<"..."
	@$(libmesh_LIBTOOL) --tag=CXX $(LIBTOOLFLAGS) --mode=compile --quiet \
	  $(libmesh_CXX) $(libmesh_CPPFLAGS) $(ADDITIONAL_CPPFLAGS) $(libmesh_CXXFLAGS) $(neml_dep_includes) $(app_INCLUDES) $(libmesh_INCLUDE) -w -DHAVE_CONFIG_H -MMD -MP -MF $@.d -MT $@ -c $< -o $@

ADDITIONAL_INCLUDES  += -I$(neml_includes) $(neml_dep_includes)
ADDITIONAL_LIBS      += -L$(NEML_DIR) -lneml-$(METHOD)
ADDITIONAL_CPPFLAGS  += -DNEML_ENABLED -DNEML_STRAIN_RATE_LIMIT=1e10

else
$(info WARNING: Not building with NEML because contrib/neml submodule is not present and NEML_DIR was not set to a valid NEML checkout)
$(info See https://github.com/Argonne-National-Laboratory/neml/blob/dev/INSTALL.md)
endif
