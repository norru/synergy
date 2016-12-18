Synergy
=======

Share one mouse and keyboard between multiple computers.

Synergy and it's components are, unless otherwise
specified, licensed under the terms of the
GNU General Public License, Version 2 (GPLv2).

Synergy includes software developed by Eric Young (eay@cryptsoft.com)
and software developed by the OpenSSL Project for use in the
OpenSSL Toolkit (http://www.openssl.org/).
The OpenSSL toolkit is licensed under two BSD-style open-source
licenses: the OpenSSL License and the SSLeay License.
Full texts of both licenses can be found in the the
file "./ext/LICENSE (OpenSSL)"

A source tarball is provided as the file
"./ext/mDNSResponder-765.20.4.tar.gz" for building the
Bonjour|mDNSResponder|dnssd.dll project required for Windows.
The majority of that project (referred to as Bonjour herein)
is licensed under the Apache License, Version 2 available from:
  <http://www.apache.org/licenses/LICENSE-2.0>
To accommodate license compatibility with the widest possible range
of client code licenses, the shared library code, which is linked
at runtime into the same address space as the client using it, is
licensed under the terms of the "Three-Clause BSD License".
The LICENSE file of Bonjour is located within the tarball.
The tarball was retrieved from [here](https://opensource.apple.com/tarballs/mDNSResponder/).



A detailed building guide isn't provided yet so the best
plan is to view the [wiki entry for building](https://github.com/symless/synergy/wiki/Compiling) in the
[symless/synergy](https://github.com/symless/synergy) repository.

And here's an excerpt on build from a previous version of the README.md
Just use "hm conf" and "hm build" to compile (./hm.sh on Linux and Mac).



Changes in this branch
---
+ Free SSL.
+ Licensing crap removed.
+ With the latest commit for WIP Windows Unicode support I probably broke compatibility with previous protocol versions.
+ Print-Screen sends Print-Screen to Windows now and not Alt+Print-Screen. (Unknown if Alt+Print-Screen is possible to send to Windows)
+ Python 2 and 3 compatibility for the building scripts.
+ A few bugs fixed like the SSL fingerprint missing on the UI and OpenSSL compatibility with v1.1.x
+ (Untested) modern icons for OSX from @static inspired by @amandamcg
+ Windows binaries go into x64 and win32 folders.
+ <other stuff here like pull-requests that were merged>
