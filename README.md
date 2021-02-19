# LinuxShell

This project aims to emulate the commands of a linux based shell like bash. <br>

Currently the features that this supports includes: <br>
Normal Commands (ls)<br>
Changing Directories <br>
I/O redirection with < and > <br>
Piping <br>
Background Processes (adding &) <br>

Normal commands are run using execvp while piping, directory changes, background processing, and
I/O redirection is handled by the program.