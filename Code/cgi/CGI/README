################################################################################
# README                                                                       #
#                                                                              #
# Description: This file serves as a README and documentation for the example  #
#              15-441 CGI code and demo Python scripts.                        #
#                                                                              #
# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          #
#          Charles Rang <rang@cs.cmu.edu>,                                     #
#          Wolf Richter <wolf@cs.cmu.edu>                                      #
#                                                                              #
################################################################################




[TOC-1] Table of Contents
--------------------------------------------------------------------------------

        [TOC-1] Table of Contents
        [DES-2] Description of Files
        [RUN-3] How to Run




[DES-2] Description of Files
--------------------------------------------------------------------------------

Here is a listing of all files associated with Recitation 4 and what their
purpose is:

                    .../README                     - Current document 
                    .../Makefile                   - for make convenience;
                                                     builds an cgi executable
                    .../cgi_example.c              - Example server source code
                    .../cgi_script.py              - Minimal Python CGI example
                    .../cgi_dumper.py              - Non-CGI conformant,
                                                     dumping script
                    .../example.GET                - Valid HTTP Request forming
                                                     the basis for hard-coded
                                                     values in cgi_example.c


***** NOTE: The code is not feature complete, doesn't use select etc.
            It is meant more to demonstrate proper CGI setup and teardown.

            You will need to:

                    (1) Integrate the fd's of the pipes into your select() loop
                    (2) Collect all necessary headers to pass as environment
                        variables to the CGI executable
                    (3) Send data coming back via stdout to a client socket




[RUN-3] How to Run
--------------------------------------------------------------------------------

Do this in one console to run the echo SSL server:

                    make
                    ./cgi

By default, the ./cgi executable is hard-coded to run the cgi_script.py.  It
prints the stdout of the executable it runs to your stdout.

You may also try this out with the cgi_dumper.py executable to see
different/simpler output.

This demo does not really show off sending data via stdin to the spawned
process, but you will have to do that in the event that the HTTP/1.1 Request
has content.
