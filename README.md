# LinuxShell

This project aims to emulate the commands of a linux based shell like bash. 
<br>
Compile: ```make```<br>
Run: ```./shell``` <br>


Currently the features that this supports includes: <br>
Normal Commands (ls, grep, etc.)<br>
Changing Directories <br>
I/O redirection with < and > <br>
Piping <br>
Background Processes <br>

Normal commands are run using execvp while piping, directory changes, background processing, and
I/O redirection is handled by the program. <br>

Test cases can be found in TESTS.md
