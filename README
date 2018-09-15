LICENSE
-------
lcab is under the GPL.

PREREQUISITES
-------

ninja - a faster make.  At this point the build is a little crazy because there's a Makefile (std make) that generates more make files
via cmake using ninja as the build subsystem which you invoke with make.  Ultimately you invoke everything with just `make` though.

`brew install ninja`


COMPILE
-------

```
make
```



CREDITS / THANKS / BUGFIXES
---------------------------
Cabinet Files specs from Microsoft.
cabinet@Thrill.cis.fordham.edu : use 32K datablocks.
undefine@venus.wmid.amu.edu.pl : argument parsing.
Ben Kibbey (bjk@m-net.arbornet.org) : quiet-option, recursive dirs
John Johnsen (johnjohnsen@mindsync.com) : small bug in setting number of files
Jeremy Jongsma (jjongsma@tickhat.com) : it segfaulted; I searched and found out max. filename size was 50!!
function changepath: got rid of the pointer error
Juan Grigera (dario@dalam.com.ar) : return codes of main.c
Shaun Jackman (sjackman@pathwayconnect.com) : cabfile header bugfix, Debian packages, man page, autoconf
Ivan Brozovic <ivan.brozovic@ka.htnet.hr> : timestamp and attributes (especially the 'run-after-install' attribute *shudder*) bug fixed
Andreas Ropers (info@aropers.de): fixed the timestamp on Win32 platform, and fixed the build on Solaris

Enjoy,
Rien (rien@croonenborghs.net)
