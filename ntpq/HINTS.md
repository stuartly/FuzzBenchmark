## Test harness

The original vulnerability is fairly accessible to afl.

Rather than trying to have afl's output simulate a remote ntpd, just replace the main() function with code that calls cookedprint with `datatype`, `status`, and `data` all read in from stdin, and the output file as stdout. Whilst genuine responses from ntpd as seeds would help, they aren't neccessary.

This is a common pattern in testing network programs - target functions such as parsers can often be easily tested in isolation.

Compile with the classic `CC=/path/to/afl-clang-fast AFL_HARDEN=1 ./configure && make`


## Coverage

You may be able to find CVE-2009-0159 in a few minutes with no further work, especially if using persistent mode. You may not - it depends how lucky your run is. If you've found it, continue with these instructions using ntp-4.2.8p10; if you haven't, continue with these instructions on ntp-4.2.2.

After running afl-fuzz for a while, you can check what coverage of `cookedprint` you've got. There are various options, here is how to use clang's gcov-compatible coverage tool:
- Compile with `CC=clang CFLAGS="--coverage -g -O0" ./configure`  (run `make distclean` first to be safe)
- Run the instrumented ntpq on all of the files in the queue (these correspond to all of the inputs that triggered a new path):

    `$ for F in out/queue/id* ; do ./ntp-4.2.8p10/ntpq/ntpq < $F > /dev/null ; done`
    
- Compile all the coverage data into a gcov report: `cd ./ntp-4.2.8p10/ntpq/ && llvm-cov gcov ntpq.c`
- Open the report (`./ntp-4.2.8p10/ntpq/ntpq.c.gcov`) in a text editor, and look at what parts of the file, and cookedprint in particular, aren't covered.

afl has a feature that will allow you to easily reach these unexplored areas - answers in ANSWERS.md!
