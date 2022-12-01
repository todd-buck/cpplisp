#Cite the source of this (kanaka/mal and seven1m/mal)

# Usage/help
all help:
	@echo
	@echo 'USAGE:'
	@echo
	@echo 'Rules/Targets:'
	@echo
	@echo 'make cpp                       	# builds cpplisp'
	@echo
	@echo 'make test                        # builds cpplisp and runs testing suite'
	@echo
	@echo 'make repl                        # builds cpplisp and runs in repl environment'
	@echo
	@echo 'make clean                       # removes main executable from project directory'
	@echo
	@echo 'Options/Settings (should be deprecated):'
	@echo
	@echo 'make TEST_OPTS="--opt ..."        # options to pass to runtest.py'
	@echo

#
# Command line settings


IMPLS = cpp 

# Map of step (e.g. "step8") to executable file for that step
cpp_executable_path =           impls/cpp/$($(1))

#
# General settings and utility functions
#

TEST_FILE = impls/tests/tests_master.txt

# Takes impl
# Returns nothing if DOCKERIZE is not set, otherwise returns the
# docker prefix necessary to run make within the docker environment
# for this impl
get_build_command = $(strip $(foreach mode,$(1)_MODE, \
    $(if $(strip $(DOCKERIZE)),\
      docker run \
      -it --rm -u $(shell id -u) \
      -v $(dir $(abspath $(lastword $(MAKEFILE_LIST)))):/mal \
      -w /mal/impls/$(1) \
      $(if $(strip $($(mode))),-e $(mode)=$($(mode)),) \
      $(if $(filter factor,$(1)),-e FACTOR_ROOTS=$(FACTOR_ROOTS),) \
      $(call impl_to_image,$(1)) \
      $(MAKE) $(if $(strip $($(mode))),$(mode)=$($(mode)),) \
      ,\
      $(MAKE) $(if $(strip $($(mode))),$(mode)=$($(mode)),) -C impls/$(impl))))

# Derived lists
# STEPS = $(sort $(filter-out %_EXCLUDES,$(filter step%,$(.VARIABLES))))
STEPS = main
DO_IMPLS = $(filter-out $(SKIP_IMPLS),$(IMPLS))
IMPL_TESTS = $(foreach impl,$(DO_IMPLS),test^$(impl))
STEP_TESTS = $(foreach step,$(STEPS),test^$(step))
ALL_TESTS = $(filter-out $(foreach e,$(step5_EXCLUDES),test^$(e)^step5),\
              $(strip $(sort \
                $(foreach impl,$(DO_IMPLS),\
					test^cpp^main))))

ALL_BUILDS = $(strip $(sort \
               $(foreach impl,$(DO_IMPLS),\
                 $(foreach step,$(STEPS),build^$(impl)^$(step)))))

IMPL_REPL = $(foreach impl,$(DO_IMPLS),repl^$(impl))
ALL_REPL = $(strip $(sort \
             $(foreach impl,$(DO_IMPLS),\
               $(foreach step,$(STEPS),repl^$(impl)^$(step)))))


#
# Build rules
#

# Enable secondary expansion for all rules
.SECONDEXPANSION:

# Build a program in an implementation directory
# Make sure we always try and build first because the dependencies are
# encoded in the implementation Makefile not here
.PHONY: $(foreach i,$(DO_IMPLS),$(foreach s,$(STEPS),$(call $(i)_executable_path,$(s))))
$(foreach i,$(DO_IMPLS),$(foreach s,$(STEPS),$(call $(i)_executable_path,$(s)))):
	$(foreach impl,$(word 2,$(subst /, ,$(@))),\
	  $(if $(DOCKERIZE), \
	    $(call get_build_command,$(impl)) $(patsubst impls/$(impl)/%,%,$(@)), \
	    $(call get_build_command,$(impl)) $(subst impls/$(impl)/,,$(@))))

# # Allow make cpp
$(DO_IMPLS): $$(foreach s,$$(STEPS),$$(call $$(@)_executable_path,$$(s)))

#
# Test rules
#

$(ALL_TESTS): $$(call $$(word 2,$$(subst ^, ,$$(@)))_executable_path,$$(word 3,$$(subst ^, ,$$(@))))
	@$(foreach impl,$(word 2,$(subst ^, ,$(@))),\
	  $(foreach step,$(word 3,$(subst ^, ,$(@))),\
	    echo "(call TEST_FILE,$(impl),$(step)): $(call TEST_FILE,$(impl),$(step))" && \
	    cd impls/cpp && \
	    $(foreach test,$(patsubst impls/%,%,$(call TEST_FILE,$(impl),$(step))),\
	      echo '----------------------------------------------' && \
	      echo 'Testing $@; step file: $+, test file: $(test)' && \
	      echo 'Running: ../../runtest.py ../$(test) -- ../$(impl)/run' && \
	      ../../runtest.py ../$(test) -- ../$(impl)/run && \
	      $(if $(filter tests/$(argv_STEP)$(EXTENSION),$(test)),\
	        echo '----------------------------------------------' && \
	        echo 'Testing ARGV of $@; step file: $+' && \
	        echo 'Running: $(call get_argvtest_cmd,$(impl),$(step)) ../$(impl)/run ' && \
	        $(call get_argvtest_cmd,$(impl),$(step)) ../$(impl)/run  && ,\
		true && ))\
	    true))

# Allow test
test: $(ALL_TESTS)

#
# REPL invocation rules
#

$(ALL_REPL): $$(call $$(word 2,$$(subst ^, ,$$(@)))_executable_path,$$(word 3,$$(subst ^, ,$$(@))))
	@$(foreach impl,$(word 2,$(subst ^, ,$(@))),\
	  $(foreach step,$(word 3,$(subst ^, ,$(@))),\
	    cd impls/cpp; \
	    echo 'REPL implementation $(impl), step file: $+'; \
	    echo 'Running: ../$(impl)/run $(RUN_ARGS)'; \
	    ../$(impl)/run $(RUN_ARGS);))

repl: $(ALL_REPL)



#
# Utility functions
#
print-%:
	@echo "$($(*))"

#
# Recursive rules (call make FOO in each subdirectory)
#

define recur_template
.PHONY: $(1)
$(1): $(2)
$(2):
	@echo "----------------------------------------------"; \
	$$(foreach impl,$$(word 2,$$(subst ^, ,$$(@))),\
	  echo "Running: $$(call get_build_command,$$(impl)) --no-print-directory $(1)"; \
	  $$(call get_build_command,$$(impl)) --no-print-directory $(1))
endef

recur_impls_ = $(filter-out $(foreach impl,$($(1)_EXCLUDES),$(1)^$(impl)),$(foreach impl,$(IMPLS),$(1)^$(impl)))

# recursive clean
$(eval $(call recur_template,clean,$(call recur_impls_,clean)))

