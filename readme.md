# RPi4 CMake project

## Development

### Your first connection (to the Pi)
To do anything with the Pi you'll need a way to execute commands on it.<br>
You can either hook up a bunch of peripherals and use it like a normal computer.<br>
Or you can `SSH` into the Pi over Ethernet *(I'm going to show you how)*

First connect your laptop's Ethernet port directly to the one on your Pi.<br>
*(Don't forget to give power to your Pi using a USB-C cable)*

Open a terminal on your host PC *(laptop)* and type connect over ssh using:
```sh
# "pi" is the default user, and "raspberrypi" is the default hostname.
$ ssh pi@raspberrypi
```
And boom, you now have a way to execute commands on your Pi.

### WiFi on the Pi (from CLI)
To setup wifi autoconnect for your hotspot you can use the following instructions:
1. Run `sudo raspi-config` on your Pi to open the system config.
2. Navigate to `1 System Options` and then `S1 Wireless LAN`.
3. At last simply enter your wifi hotspot name, and password.
Now the Pi will automatically connect the your hotspot when it's available.

## Cross-compilation
<sup>(optional)</sup>

### Installing (older) Arm cross-compiler
The reason we need an older compiler is because the RPi doesn't support newer versions of GLIBC.<br>
This is why we have to downgrade our version, to avoid a GLIBC required version that is too high.

First, make a `bin` directory under your user (or wherever you want :p)<br>
```sh
$ cd ${HOME}
$ mkdir bin
$ cd bin
```

Now, that we have somewhere to put our new compiler, let's download the tar ball.<br>
The available binaries can be found at [releases.linaro.org].<br>
Pick the version you want (I recommend 6.5 because it worked for my RPi4B)

Next, choose which compiler you want:
- ARM 32-bit : `arm-linux-gnueabihf`
- ARM 64-bit : `aarch64-linux-gnu`

Once inside the folder of the compiler which you need, copy the link to the `x86_64**.tar.xz` file.<br>
Now at last we can download the compiler binaries using `wget`.<br>
*(replace `<link>` with the link you copied)*

[releases.linaro.org]: https://releases.linaro.org/components/toolchain/binaries/

```sh
# IMPORTANT: make sure you're still inside your `${HOME}/bin/` directory you created.
$ wget <link>

# EXAMPLE link: https://releases.linaro.org/components/toolchain/binaries/6.5-2018.12/arm-linux-gnueabihf/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar.xz
```

Now simply unpack the tar ball using the following command:<br>
*(replace `<tarball>` with the name of the tar ball you downloaded)*
```sh
$ tar -xf <tarball>
```
*(you can delete the tar ball after unpacking :p)*

Finally we've come to the last part of the process, updating your `PATH` env variable.<br>
First find the `bin` directory inside the tar ball you just unpacked.<br>
Then run `pwd` to get the absolute directory to this `bin` folder.

After obtaining the directory where your new compiler binaries are stored.<br>
Open the `~/.profile` file using your favorite text editor.
```sh
$ sudo nano ~/.profile
```

Now look for this part of the file:
```sh
# set PATH so it includes user's private bin if it exists
if [ -d "$HOME/bin" ] ; then
    PATH="$HOME/bin:$PATH"
fi
```
And add your compiler directory in there like so:<br>
*(make sure to replace the path with I added as example with yours)*
```sh
# set PATH so it includes user's private bin if it exists
if [ -d "$HOME/bin" ] ; then
    PATH="$HOME/bin:$PATH"
    # add ARM cross compiler path
    PATH="$HOME/bin/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin:$PATH"
fi
```

That's it, now after a simple `reboot` you'll have access to your cross-compiler.<br>
Enjoy :D
