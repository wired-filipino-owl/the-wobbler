# The Wobbler: Inspired by Sound Werkshop's "The Wiggler"

Project inspiration: https://www.youtube.com/watch?v=x4khM5Bjihg

My senior project at Oregon Institute of Technology, CSET Department.

----
### Software build setup
##### Install build tools
Follow [Daisy Wiki - Install the Toolchain](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment#1-install-the-toolchain)
##### (Linux) Add `udev` rule for Daisy Seed to be able to program without using `sudo`
https://forum.electro-smith.com/t/linux-system-wont-program-daisy-seed-solved/5381
##### Get libraries
Clone https://github.com/electro-smith/DaisySP/ and https://github.com/electro-smith/libDaisy/ into `$HOME/lib/Daisy/`.
I suggest using `git checkout` to get the versions that match the Daisy Examples repo, otherwise the code might not compile due to version mismatch.
Build is done via `make`. The demos are dependent on the libDaisy Makefile, so make sure to put libDaisy in the right place or be prepared to edit the Makefile with the correct path.

---- 

### Non-code artifacts
Schematics are in `/schem`

More to come soon!
