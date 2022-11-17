#!/usr/bin/env python

import os

print('Populating NEML regression tests, Lagrangian system')
NEML_DIR = os.environ['NEML_DIR']
neml_rtest_path = os.path.relpath(NEML_DIR + '/test/test_regression')
dirs = []

for (dirpath, dirnames, filenames) in os.walk(neml_rtest_path):
  if '__pycache__' in dirnames:
    dirnames.remove("__pycache__")
  dirs.extend(dirnames)

try:
  os.mkdir('gold')
except FileExistsError:
  pass

for d in dirs:
  src = '../' + neml_rtest_path + '/' + d + '/data.csv'
  dst = 'gold/' + d + '_out.csv'
  try:
    os.symlink(src, dst)
  except FileExistsError:
    os.remove(dst)
    os.symlink(src, dst)

testsfile = open("tests", "w")

testsfile.write("[Tests]\n")

test_string = """  [{testname}]
    type = 'CSVDiff'
    input = 'neml_regression.i'
    cli_args = 'GlobalParams/data_file=../../../contrib/neml/test/test_regression/{testname}/data.csv Materials/stress/database=../../../contrib/neml/test/test_regression/{testname}/model.xml Outputs/csv/file_base={testname}_out'
    csvdiff = '{testname}_out.csv'
    rel_err = 1e-4
    abs_zero = {abs_zero}
    required_objects = CauchyStressFromNEML
    issues = '#312'
    design = 'CauchyStressFromNEML.md'
    requirement = 'BlackBear shall run {testname} of the NEML regression tests and obtain equivalent results to those from the NEML material driver'
{skip}{valgrind}  []
"""

abs_zero_overrides = {
  'test_substep':             1e-7,
  'test_basicchaboche':       1e-7,
  'test_chabochefull':        5e-7,
  'test_complexcreep':        1e-7,
  'test_complexcreepdamage':  1e-7,
  'test_walkerkremplswitch':  1e-7,
  'test_plasticcreep':        1e-7,
  'test_linearisokin':        1e-6,
  'test_cpmulti':             5e-7,
  'test_walkerbasic':         1e-7,
  'test_cpbcc':               1e-7,
  'test_cpsimple':            1e-7}

valgrind_overrides = {
  'test_substep':             '    valgrind = heavy\n'}

#No test currently need to be skipped, but listing them here as
#follows would skip them:
# 'test_name':  "    skip = 'Skip message'\n"
skip_overrides = {}

for d in dirs:
  abs_zero_d = abs_zero_overrides.get(d, 1e-8)
  skip_d = skip_overrides.get(d, "")
  valgrind_d = valgrind_overrides.get(d, "")

  testsfile.write(test_string.format(testname=d, abs_zero=abs_zero_d, skip=skip_d, valgrind=valgrind_d))

testsfile.write("[]")
