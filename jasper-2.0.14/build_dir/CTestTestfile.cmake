# CMake generated Testfile for 
# Source directory: /home/cloud/work/jasper-2.0.14
# Build directory: /home/cloud/work/jasper-2.0.14/build_dir
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(run_test_1 "/bin/bash" "/home/cloud/work/jasper-2.0.14/build_dir/test/bin/wrapper" "/home/cloud/work/jasper-2.0.14/test/bin/run_test_1")
add_test(run_test_2 "/bin/bash" "/home/cloud/work/jasper-2.0.14/build_dir/test/bin/wrapper" "/home/cloud/work/jasper-2.0.14/test/bin/run_test_2")
add_test(run_test_3 "/bin/bash" "/home/cloud/work/jasper-2.0.14/build_dir/test/bin/wrapper" "/home/cloud/work/jasper-2.0.14/test/bin/run_test_3")
add_test(run_test_4 "/bin/bash" "/home/cloud/work/jasper-2.0.14/build_dir/test/bin/wrapper" "/home/cloud/work/jasper-2.0.14/test/bin/run_test_4")
subdirs("src/libjasper")
subdirs("src/appl")
subdirs("doc")
