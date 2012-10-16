#####################################################################
# relay_readme.txt file

#####################################################################
# aRelay Application Notes

The aRelay is a utility application to bridge serial based devices
such as the BrainStem to a TCP/IP socket interface. 

Usage information can be found in the Techology Software section. 
Either type "aRelay" into the find box on any Acroname webpage 
or navigate to:

  http://www.acroname.com/software/aRelay.html

This directory is named "acroname" and within this directory is an
aBinary sub-directory.  That is where the executable and support files
reside.

Notice the additional configuration file named "relay.config".  This
file contains additional parameters that you can use based on your
platform and desired use.  It is a plain text file that can be edited
with your favorite editor.

#####################################################################
# Acroname Support

This download is a work in progress.  If you would like to see
additional features or changes, please create a software request
change at:

  https://www.easierrobotics.com/cgi-bin/login

Alternatively, support can be found by emailing or calling at:

  support@acroname.com

    or

  720-564-0373

We also will be hosting updates to this software in our download
center at accessible from your home page:

  https://www.easierrobotics.com/cgi-bin/login

Keep up with the latest version for your platform there.

We hope you enjoy using the aRelay application to bridge serial 
devices to a TCP/IP socket service.

#####################################################################
# Platform Notes:

Linux

  L-1. You may need to change the permissions for your serial port
       to allow writing.  Something like this as root:

          # chmod a+rw /dev/ttyS0

       would do the trick.

Windows

  W-1. If you are using a USB to serial adapter, you may need to find
       out what COM port the adapter's driver is masquerading the 
       serial port as.  This may be a property in the devices section
       and will be found in the documentation for your adapter.
  W-2. If you make a config file or edit one, be CERTAIN there is not
       a .txt extension added to the file.  Windows hides this because
       it things you aren't smart enough to understand it so you need
       to check the properties of the file to be certain.  If there
       is a .txt extension, the configurations will not take effect.

MacOS X

  M-1. When using a USB to serial adapter, the driver names the serial port
       for you.  You need to find this name to put in the "relay.config"
       file.  There are detailed notes on this at:

         http://www.acroname.com/MacOS/usb.html

#####################################################################
                Thank You for using Acroname Robotics!
#####################################################################
