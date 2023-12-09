LinnStrument Firmware: skip-fretting Fork
============
I added an experimental feature to the Linnstrument firmware to support "skip-fretting" - each subsequent pad represents every other MIDI note, so note 0 2 4 6 8 ... instead of 0 1 2 3 4. 

The name is in reference to the microtonal [kite guitar](https://kiteguitar.com/), which obviously uses frets and not keys, but the Linnstrument's rows and columns work just like strings and frets, so you can acheive Kite fretting on the Linnstrument easily with this mod by setting your synth software to use 41 equal divisions of the octave instead of 12. This is easy to do in e.g. Surge XT with it's Tuning Editor.

Why would you want to skip half the notes? On the kite guitar, strings are 13 steps apart, which you can achieve on the Linnstrument by setting the Row Offset to a custom value of 13 (Global Settings -> Row Offset -> hold down "Octave" to get the secret menu -> swipe right). Thirteen is an odd number, so that means that if the first row represented the Even scale degrees (0 2 4 6 8...), the next row will represent the odd scale degrees (..13 15 17 19 21..)! Thus you actually have access to all the midi notes after all, with two neighboring strings filling each other's gaps. 

This is handy, as 41 notes per octave would not otherwise fit on a single row. But with half that, you get at least an octave per row on a 200-pad instrument. Luckily, you typically only want the odds or evens at the same time on the same string/row anyway, as explained on the kite guitar website.

Note that while you may be used to your Linnstrument lighting up multiple pads when you press a single one (the other pads are repeats of the same note), with the kite layout all the notes are actually slightly different, so pressing a pad will only light up that pad.

# Usage
Follow the [standard directions](https://www.rogerlinndesign.com/support/support-linnstrument-update-software), but replace the `.bin` file in the updater bundle with the one found in the Releases section of this project.

If you're not on Mac or Windows, you can still do it using `arduino-cli`, or by cloning this project, opening it in Arduino IDE (according to the Dev section below), and hitting "Upload" - no coding or command line required. Note that these methods do not let you preserve your Linnstruments' settings and calibration, for now.

# Caveats
- Because MIDI only supports 128 notes, the Linnstrument pads that go beyond that range are disabled. This won't be noticable on the smaller Linnstrument, but on the full-size device, 128 notes plus a 13 note row offset means 194 pads lit, falling just 6 pads shy of the full 200 pads. By default they'll all be in the top corner; you can shift everyhting around with transpose settings (or just use split mode!)
- Linnstrument pad color lighting is still based on the 12EDO assumption, so the lighting pattern is not very useful for 41edo. Kite-specific lighting is an area for future exploration, but it doesn't need to be at the firmware level - simply send MIDI to your instrument and it will light the pads accordingly. Pure Data might be a good software choice for this.
- The arpeggiator is currently unaware of the skipfretting patch and will exhibit undefined behavior for now.

# Development

These sources assume that you're using Arduino IDE v1.8.1 with SAM boards v1.6.11.
Different versions of this package might create unknown build and execution problems.
I use the flatpak version of Arduino IDE, which means the build environment is nicely sandboxed. This means you can just pick the versions as per the instructions and all other software will work the same.

Instructions from [https://www.rogerlinndesign.com/support/support-linnstrument-source-code](Linnstrument support page):
-    Visit the Ardunino downloads page and follow the instructions for downloading the latest Arduino IDE.
-    Launch the "Arduino IDE" app.
-    Connect your LinnStrument via USB, then turn on UPDATE OS mode (Global Settings > Actions column).
-    In the Arduino IDE, Click Tools menu > Board > "Boards Manager...", then Install "Arduino SAM Boards". Then click Tools menu > Board > "Arduino SAM boards (32 bit ARM Cortex M3)" > "Arduino Due (Programming Port)".
-    Go to Sketch > Include Library > "Manage Libraries...". Search for "DueFlashStorage" and install it.
-    Go to File menu > "Open..." and select the file "linnstrument-firmware.ino" in the "linnStrument-firmware" directory that you downloaded above.
-    Go to Tools menu > Port and select your LinnStrument port from there. (It starts with /dev/cu.usbmodem followed by a number.)
-    In the "linnstrument-firmware" window, click the top-left check mark icon (Verify) to compile the code. If you receive compilation errors or warnings, check the above steps.
    Note: You can simply click the Upload button (right-facing arrow) in the IDE to upload your code to LinnStrument, but this will reset LinnStrument's sequencer data, stored panel settings, and sensor calibration data to defaults, requiring you to perform a subsequent 'Global Settings > Calibration' after the upload. To avoid losing this data, do the following additional steps:
-    Compile your LinnStrument firmware as above by clicking the Arduino IDE's "Verify" button (the checkmark button in upper left corner).
-    In the IDE's Output pane, select and copy the file path that will appear similar to the picture at the bottom of this page, starting with "/var" and ending with ".bin":
-    On mac, open a finder window, press Shift + Command G, paste the line you just copied and press Enter. This will take you to the location of the newly-compiled OS file. On Windows, go to the directory indicated by the file path.
-    From the LinnStrument Support page, click the "Update OS" page and download the latest Updater app. Place your newly compiled OS file in the same directory as the Updater app. Now the Updater app will load your newly compiled OS file instead of the OS file inside the Updater app. By using this method to load your edited source code, your LinnStrument's settings and calibration data will be preserved.

# Support
For support with the official firmware, email Roger at support@rogerlinndesign.com.
For requests for this fork, open an new issue.
