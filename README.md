# circuit-simulator

## Dependencies
### Ngspice Shared Library

The circuit simulation is handled by the ngspice shared library. In order to run the simulator you will need to download and install ngspice and the shared library. The shared library works ONLY if compiled from the ngspice git repo.

1. Clone the ngspice repo: `git clone http://git.code.sf.net/p/ngspice/ngspice`
2. Follow the install instructions in the file INSTALL in the repo directory (including the `./configure ... && make && sudo make install` step)
3. Run the command `./configure --enable-xspice --enable-cider --disable-debug=yes --with-ngshared && make && sudo make install`
4. Make sure that the file `/usr/local/include/ngspice/sharedspice.h` exists
