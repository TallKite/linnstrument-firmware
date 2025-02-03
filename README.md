/**************************************** MICROLINN ****************************************/


WARNING: unfinished, many features don't work yet. In particular, the microtonal features require a helper app, microLinn.jsfx.
But the sequence chaining and the scale +/- footswitches do work.



-------- INSTALLATION -------- 

WARNING: installing this fork deletes your Linnstrument's settings and calibration (for now, fix coming soon).

1) Go to https://www.rogerlinndesign.com/support/support-linnstrument-update-software and follow the "How to Check Your Software Version" instructions. If it's not 2.3.3, follow the "How to Update Your LinnStrument Software‍" instructions to update to 2.3.3.
2) Download the latest version of linnstrument-firmware.ino.bin from this github. Important: if on a mac, put the .bin file on your **desktop**. 
3) Follow the "How to Update Your LinnStrument Software‍" instructions, with one difference: after you download (and possibly unzip) the updater and before running it, put it in the same folder as the .bin file from step 2. Mac users: when you run the updater, if it asks for permission to read files from the desktop, say yes.
4) Check your OS version to confirm the update. It should be 234.072A. If not, reboot your computer and try again.
5) Important, read the next section about uninstalling!

-------- UPGRADING/UNINSTALLING -------- 

IMPORTANT: Later on when you update the OS to a different version, you'll be asked if you want to uninstall microLinn. Say no if you're updating to a new version of microLinn. This avoids deleting your microtonal user settings needlessly. Say yes otherwise (e.g. if you're going back to an official (non-microtonal) version of the firmware). This does delete your microtonal user settings, which is necessary in order to avoid deleting all your other user settings plus your calibration data.

-------- NON-MICROTONAL FEATURES -------- 

CHAINING SEQUENCES: hold the pad for sequence #2 and tap the pad for sequence #1. Now the two sequences are chained into a 
single sequence twice as long. This is indicated by sequence #1 being accented and sequence #2 blinking.
You can chain #3 and #4 together as well. Or chain all 4 together. The lights for the entire chain will blink when it's playing. 
When making a chain, to start at the beginning of the chain not the end, hold the last sequence and tap the first one.
You can still select sequences and chains on the fly as the sequencer is playing. You can also chain and unchain on the fly. 
Straight/dotted/triplet/swing and quarter/8th/16th are still set individually for each sequence, so 1 chain can mix these together.
To clear all chains in a split, tap the hidden switch immediately to the left of the 4 selector pads. Or just unplug the Linnstrument.

SEQUENCER PEDALS: When playing in one split and using the other split as a sequencer, it's no longer necessary to switch 
to the other split before using the following pedals (or switches or midi NRPN messages): PLAY, PREV, NEXT and MUTE.
Outside of a chain, the NEXT and PREV footswitches take you to the next/previous sequence as before. 
You can double-tap or triple-tap the NEXT and PREV footswitches to skip forward/backward multiple sequences.
Thus triple-tapping NEXT is the same as single-tapping PREV, which means you only need one pedal to go anywhere.
From within a chain, NEXT and PREV operate relative to the upcoming sequence in the chain, not the current one.
Thus pressing PREV repeats the current sequence and pressing NEXT goes forward two sequences, not one.
One exception: from the rightmost sequence of a chain, NEXT exits the chain. (Otherwise one could never exit.)

SEQUENCER PROJECTS: in the Load/Save screen, the projects now run top to bottom, with the top row being 1-4 not 13-16.
This is more intuitive because it matches how people read, for example how you are reading this very paragraph right now.
(These project numbers only matter when using the Updater app to export/import projects to/from your computer.)
As a result, projects you might have previously saved to the top row are now in the bottom row.
To move them, load them from the bottom row and save them to the top row.

BLINKING MODE: This is a variation of the SAME mode that shows you other occurences of the currently played note.

NEW SWITCH/FOOTSWITCH FUNCTIONS: SCL+ and SCL- cycle you through the 9 scales in Global Settings columns 2-4 (only affects the note lights).

COLUMN OFFSETS: The column offset can be set for each split independently. This allows Wicki-Hayden layout on the left for 
easy chord playing and normal layout on the right for easy melody playing. Access it through the microLinn menu, see below.

MULTI-COLORED NOTE LIGHTS: Each of the 12 notes can be any color. Transposable. Access it through the microLinn menu, see below.

DETUNING: Detune the entire Linnstrument up or down from A-440 to match a similarly detuned recording or instrument. 
(No guarantee that detuning to A-432 will heal your chakras lol.) Access it through microLinn's anchor cents, see below.

FOR CODERS: The Linnstrument can now send a locator CC message along with every note-on, indicating the row and column.
This lets code on your laptop assign a specific function to a specific pad, e.g. upper lefthand corner = all sound off.
Format: for cols 1-16, CC data value = (row - 1) + 8 * (col - 1). For cols 17-25, CC data value = (row - 1) + 8 * (col - 17).
The top row is row 1 and the leftmost column is column 1. Enable it and choose the CC through the microLinn menu, see below.
Thanks to KVR forum member vorp40 for the code!

-------- MICROTONAL FEATURES -------- 

On the Global Settings screen, long-press col 1 bottom row (VIEW MAIN) to go to the main microLinn menu.
Once the edo (notes per octave) is set to anything other than OFF, VIEW MAIN turns light blue and you can short-press it.
The menu has 10 buttons. *Long-press each one* to see its function. Many don't work until you set the edo.

1) left column offset
2) right column offset

3) edo (notes per octave)
4) octave stretch in cents

5) anchor pad
6) anchor note
7) anchor cents

8) set note lights

9) set to 31edo Bosanquet (row offset 3, column offset 5)
10) set to 41edo Kite guitar (row offset 13, column offset 2)
11) reset to 12edo

The anchor pad is a specific pad or cell that doesn't change pitch when you change the notes per octave.
The anchor pad chooser displays the row and column of the current anchor pad, e.g. R3C6 means row 3 (from the top) and column 6. 
To choose a new anchor pad, tap the blue "R3C6" and then tap any pad. Change the anchor cents to detune the entire Linnstrument.
Each split can be transposed independently any number of edosteps from the anchor cell, allowing any tuning.

When the notes per octave is greater than 12, the OCTAVE/TRANSPOSE display shows two extra rows for transposing by edosteps.
The 2nd and 3rd rows now transpose not by semitones but by major 2nds (since most edos have several different semitones).
A major 2nd is defined as the interval between the 4th and the 5th, e.g. 3 edosteps for 15edo but 2 edosteps for 16edo.

MicroLinn's guitar tuning is completely independent of the standard Linnstrument guitar tuning.
The guitar tuning screen no longer adjusts the pitch of each "string". Instead it sets 7 independent row offsets.
The "anchor string" is the row that the anchor pad is on. Its pitch is determined solely by the anchor pad, note and cents.
On the far left, the anchor string is red, or orange if lit up. The others are green, or cyan if lit up.
Tap any button on the far left except the red one to select it. It lights up and sounds that open string.
One of the neighboring strings, whichever one is closest to the anchor, also lights up.
You won't see a note name with an octave number. Instead you'll see a row offset as a number (which can be negative). 
This is the row offset between the two lit-up strings. Swipe right or left on it as before to increase or decrease it.
(Selecting the anchor string doesn't show a row offset, but it does sound the pitch of the anchor open string.)
Changing one row offset doesn't affect the other six row offsets.
Thus increasing any row offset above the anchor string sharpens the current string and all strings above it.
And increasing any row offset below the anchor string flattens the current string and all strings below it.
(Of the two lit-up buttons, the current string is always the one furthest from the anchor string.)

If the guitar tuning is the standard tuning, the GLOBAL screen's GUITAR pad is dark blue, otherwise it's bright blue.
The exact notes don't matter, just that the intervals between open strings are all 4ths, except for that one major 3rd.
The edo's perfect 4th is its closest approximation to 4/3. The edo's major 3rd is two octaves minus four 4ths.
There's two possible 4ths for 13edo (5\13 and 6\13) and 18edo (7\18 and 8\18). Either 4th keeps the GUITAR pad dark blue.

Changing the edo adjusts the column offsets and row offset(s) so that their size in cents stays roughly the same.
So your bosanquet keyboard layout remains roughly bosanquet, and your F#BEADGBE guitar tuning remains roughly F#BEADGBE.

There are 4 new functions for switches 1 & 2 and footswitches 1 & 2: 
EDO+ and EDO- move up or down to the next EDO, wraps around (probably best you don't play while changing!)
SCL+ and SCL- move up or down to the next scale, wraps around (only affects the note lights).

The 9 scales in Global Settings cols 2-4 are now microtonal and change for each edo. You can edit them and their colors in microLinn. 
Shortcut: you can long-press the scale's pad in the Global Settings display to go directly to that scale.

The note lights display has 7 scale buttons plus the rainbow editor, the dots selector and the yellow rainbow-enabler button.
Tap any of these buttons except the rainbow-enabler to select it. Tap any selected button to backtrack to the previous button.
There are 7 rows of colored lights on the screen, which top to bottom are for unisons, 2nds, 3rds, 4ths, 5ths, 6ths and 7ths.
Tap a note in a scale to toggle it on or off. Like the guitar tuning screen, a midi note is sent when you tap. 
The 8th scale is the rainbow editor, in which all notes are always on. Tap a note to cycle it thru the rainbow. 
The 9th scale (guitar-like fretboard dots) isn't really a scale. It's a full-screen display like the custom light patterns.
Tapping the dots selector makes the dots appear in blue mid-screen. Tapping the dots lets you toggle them on or off. 
Long-press the scale buttons or the rainbow editor button or the dots selector button to reset the note lights to the default. 
Tap the yellow rainbow enabler button to turn off the rainbow and limit the note lights to the usual two colors.

The 3 custom light patterns are totally separate from all this and are still available for use!
This web browser lets you easily edit them: https://forrcaho.github.io/linnstrument_colorizer/

Of the 8 scales, the 1st and 2nd scales are 5-limit major and 5-limit minor.
Scales 3-6 are blank but for the tonic, so that you can create your own scales in the lights display
(For the smaller, weirder edos, scales 1-2 are also blank.)
The 7th scale is a partial rainbow, and the 8th scale is always the full rainbow.

The colors use the rainbow metaphor, red-yellow-green-blue = sharp to flat = supermajor to subminor
white = 12-edo-ish = 3-limit
yellow / green = downmajor / upminor =  5-over / 5-under (examples: 5/4 and 5/3 are 5-over, 6/5 and 16/15 are 5-under)
blue / red = downminor / upmajor = 7-over / 7-under
purple = neutral = 11-over or 11-under or 13-over or 13-under
pink is reserved for the exact half-octave of 600c, 12-edo-ish but not quite 3-limit, "off-white"
cyan / orange = a catch-all pair, e.g. 41edo 7/5 and 10/7, cyan is also for "outside" notes aka interordinals e.g. 24edo
orange is used in 55edo for upmid and downmid notes

The dot patterns tend to follow the conventional m3 P4 P5 M6 P8 guitar fret markers. Some edos add M2 and m7.
Edos above 24 approximate 12edo, in other words there are dots about every 100 cents.
41edo is an exception. It has kites like a Kite guitar.

All of microLinn's settings are remembered by the 6 presets except the scales, rainbows and dot patterns.

On the custom row offset screem, while microLinn is on, the "-GUI" option for reversed guitar tuning is no longer available.
To get this tuning, set the guitar tuning manually instead.

What advantages does microLinn's 12edo have over the standard, non-microLinn 12edo? 
It can be stretched and/or detuned, plus you get multi-colored note lights.

Suggestions for exploring:
The first few edos are pretty strange. Try starting with 15, 17, 19 or 22.
Be sure to try the Bosanquet layout for 31edo and the Kite guitar layout for 41edo.
The rainbow colors can be overwhelming. Try setting the played mode to BLNK.
Once you know an edo well, you'll probably want to switch to the fretboard dots display.

To create JI or rank-2 scales with N notes, set the edo to N. (Eventually: also turn on raw midi mode.)
Load a scala file into your synth or run alt-tuner to produce that N-note scale.
Each note is slightly sharper or flatter from N-edo, thus the pad's note will be different slid up to vs played directly.
However this is only a comma or so difference even on long slides, which might be tolerable.
Perhaps software on the PC could fix this problem by scaling the slides by adjusting the pitch bends?


features not yet implemented:
raw midi output means one mide note pe edostep, doesn't use tuning bends
good for older synths, good for creating JI scales via scala files
noOverLap mode gives ech pad its own unique midi note number (beware, 200 pads > 128 midi notes!)

============================= technical notes ==================================

To find all changes to the code, search for "microlinn" or "patternChain" or "playedBlink" or "control the sequencer"

"Skip-fretting" is a column offset of 2 - each subsequent pad represents every other MIDI note, so note 0 2 4 6 8 ... instead of 0 1 2 3 4. 
The name is in reference to the microtonal [kite guitar](https://kiteguitar.com/), which obviously uses frets and not keys, but the
Linnstrument's rows and columns work just like strings and frets.

Why would you want to skip half the notes? On the kite guitar, strings are 13 steps apart, which you can achieve on the Linnstrument by setting 
the Row Offset to a custom value of 13 (Global Settings -> Row Offset -> hold down "Octave" to get the hidden menu -> swipe right). 
Thirteen is an odd number, so that means that if the first row represented the _even_ scale degrees (0 2 4 6 8...), the next row will represent 
the _odd_ scale degrees (..13 15 17 19 21..)! Thus you actually have access to **all** the midi notes after all, with two neighboring rows
filling each other's gaps. 

This is handy, as 41 notes per octave would not otherwise fit on a single row. But with half that, you get at least an octave per row on a 200-pad 
instrument. Luckily, you typically only want the odds or evens at the same time on the same string/row anyway, as explained on the kite guitar website.

# Install
Follow the [standard directions](https://www.rogerlinndesign.com/support/support-linnstrument-update-software), but replace the `.bin` file in the
updater bundle with the one found in the Releases section of this project.

WARNING: this method deletes your Linnstrument's settings and calibration (for now).

If you're not on mac or windows but gnu/linux, you can still do it using `arduino-cli`, or by cloning this project, opening it in Arduino IDE 
(following the Dev section below), and hitting "Upload" - no coding or command line required. 

# Kite Guitar Mode
- Enables skip fretting (column offset of 2)
- Enables Custom Row Offset
- Sets the Custom Row Offset to 13 steps (kite tuning)
- Sets `PLAYED` note lighting to `SAME` mode, so you can see which other pads represent the same note as you learn the kite layout (optional)

# Development

These sources assume that you're using Arduino IDE v1.8.1 with SAM boards v1.6.11.
Different versions of this package might create unknown build and execution problems.
I use the flatpak version of Arduino IDE, which means the build environment is nicely sandboxed. This means you can just pick the versions 
as per the instructions and all other software will work the same.

Instructions from [Linnstrument support page](https://www.rogerlinndesign.com/support/support-linnstrument-source-code):
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
