/**************************************** MICROLINN ****************************************/

Unfinished, still a few more features to add, plus a few features don't quite work right yet. 
Beta testers needed!

-------- INSTALLATION -------- 

WARNING: installing this fork deletes your Linnstrument's settings and calibration (for now, fix coming soon).

1) Go to https://www.rogerlinndesign.com/support/support-linnstrument-update-software and follow the "How to Check Your Software Version" instructions. If it's not 2.3.3, follow the "How to Update Your LinnStrument Software‍" instructions to update to 2.3.3.
2) Download the latest version of linnstrument-firmware.ino.bin from this github. Important: if on a mac, put the .bin file on your **desktop**. 
3) Follow the "How to Update Your LinnStrument Software‍" instructions, with one difference: after you download (and possibly unzip) the updater and before running it, put it in the same folder as the .bin file from step 2. Mac users: when you run the updater, if it asks for permission to read files from the desktop, say yes.
4) Check your OS version to confirm the update. It should be 234.072A. If not, reboot your computer and try again.
5) Important, read the next section about uninstalling!

-------- UPGRADING/UNINSTALLING -------- 

IMPORTANT: Later on when you update the OS to a different version, you'll be asked if you want to uninstall microLinn. Say no (the red button) if you're updating to a newer version of microLinn. This avoids needlessly deleting your microtonal user settings. Say yes (the green button) otherwise, e.g. if you're going back to an official (non-microtonal) version of the firmware. This does delete your microtonal user settings, which is necessary in order to avoid deleting all your other user settings plus your calibration data.

-------- NON-MICROTONAL FEATURES -------- 

COLUMN OFFSETS: Ranges from 1 (OFF) to 8. For negative offsets, use the hidden lefthanded setting in Global Settings column 1. The usual Linnstrument tuning is row offset +5 and column offset +1, or (+5 +1). The three most popular hexagonal-key layouts can each be translated to the Linnstrument's square keys in two ways:
* Wicki-Hayden layout translates to (+5 +2) or (+7 +2)
* Bosanquet or Janko layout translates to (-1 +2) or (+1 +2)
* Harmonic Table layout translates to (+4 +3) or (+7 +3)

The column offset can be set for each split independently. You can have one of these six layouts on the left for easy chord playing and the usual layout on the right for easy melody playing, somewhat analogous to an accordion's layout. Accessed through the microLinn menu, see below.

A 1mm slide normally bends the pitch about 6¢. But an offset of +2 makes it 12¢, +3 makes it 18¢, etc. Thus to play in tune with a large column offset you may need both Pitch/X Quantize and Pitch/X Quantize Hold to be on. But if your column offset is +2, setting Quantize on and Quantize Hold off lets you play the skipped notes fairly accurately by sliding into the gap between pads. In fact, one can play 24edo quartertones on a normal 12edo Linnstrument fairly easily this way.

Details: To set both column offsets at once, press both split buttons at once. If the offsets are different, doing so equates them. Beware, if the column offset is 2 and the row offset is an even number, you lose half the notes and only get a whole-tone scale. In general, the column offset and the row offset should not have any common factors.

PER-SPLIT ROW OFFSETS: Setting the row offset for a split overrides the Global Settings row offset for that split only. It ranges from -25 to +25, plus NO OVERLAP which appears as NOVR. A NOVR split can be used to launch audio or midi clips while you play normally in the other split. Accessed through the microLinn menu, see below. Guitar tunings are not supported, but are still available globally.

OCTAVE TOGGLE: A new function for switches and footswitches, 8VE± flips back and forth between Octave Up and normal. This lets you switch octaves while playing using only one footswitch, instead of two for Octave Up and Octave Down. Access it by long-pressing TAP TEMPO and swiping.

DRUM PAD MODE: Custom LED pattern #2 (the scale marked A#) is now 14 3x3 mega-pads that play the 14 drum sounds from the sequencer. Details: set PITCH/X on, otherwise a tap that hits two pads will send two notes. The sequencer has different drum sounds for each split. If you're using both splits, set the split point in between the pads. You can create your own mega-pad layout by using locator CCs (see below) and editing pattern #2 to match. Accessed through the microLinn menu, see below. To get the original Custom LED pattern #2 back, in the microLinn menu set the edo to 12 and adjust the note lights. Unlike the original LED pattern, these note lights can be transposed, recolored, made pentatonic or chromatic, and made lefthanded.

DETUNING: Detune the entire Linnstrument up or down from A-440 to match a similarly detuned recording or instrument. No guarantee that detuning to A-432 will heal your chakras lol. Accessed through microLinn's anchor cents after setting the edo to 12, see below.

CHAINING SEQUENCES: hold the pad for sequence #2 and tap the pad for sequence #1. Now the two sequences are chained into a single sequence twice as long. This is indicated by sequence #1 being accented and sequence #2 blinking. You can chain #3 and #4 together as well. Or chain all 4 together. The lights for the entire chain will blink when it's playing. When making a chain, to start at the beginning of the chain not the end, hold the last sequence and tap the first one. 

You can select sequences and chains on the fly as the sequencer is playing. You can also chain and unchain on the fly. Straight/dotted/triplet/swing and quarter/8th/16th are still set individually for each sequence, so a chain can mix these together. To clear all chains in a split, tap the hidden switch immediately to the left of the 4 selector pads. Or just unplug the Linnstrument.

SEQUENCER PEDALS: When playing in one split and using the other split as a sequencer, it's no longer necessary to switch to the other split before using the following footpedals (or switches or midi NRPN messages): PLAY, PREV, NEXT and MUTE. 

Details: outside of a chain, the NEXT and PREV footswitches take you to the next/previous sequence as before. You can double-tap or triple-tap the NEXT and PREV footswitches to skip forward/backward multiple sequences. Thus triple-tapping NEXT is the same as single-tapping PREV, which means you only need one footswitches to go anywhere.

But from within a chain, NEXT and PREV operate relative to the upcoming sequence in the chain, not the current one. Thus pressing PREV repeats the current sequence and pressing NEXT goes forward two sequences, not one. One exception: from the rightmost sequence of a chain, NEXT exits the chain. (Otherwise one could never exit.)

SEQUENCER PROJECTS LOAD/SAVE SCREEN: When exporting/importing projects to/from your computer, the updater app refers to projects by number. These project numbers now run top to bottom, with the top row being 1-4 not 13-16. This is more intuitive because it matches how people read, for example how you are reading this very paragraph right now.

BLINKING MODE: Like the SAME mode, BLNK shows you other occurences of the currently played note. But instead of changing color, the other occurences blink. It's good for multi-colored displays like the custom note lights #2 (the one marked as A#) or certain microtonal displays. The BLNK option appears right after CELL and SAME. (Don't confuse BLNK for blinking with BLIN for blinders.)

SAME/BLINK CARRY OVER: If both splits are set to SAME or BLNK, playing in one split shows matching notes in the other split too.

MULTI-COLORED NOTE LIGHTS: Each of the 12 (or more!) notes can be any color. Transposable. Access it through the microLinn menu after setting the edo to 12, see below.

LOCATOR CCs: The Linnstrument can now send a locator CC message immediately after every note-on and note-off, indicating the row and column. Your DAW can assign a specific function to a specific pad, e.g. upper lefthand corner = all sound off. Or use this feature plus a custom LED pattern to create large drum pads for more easily playing drum midi. 

The XYZ data is not accompianied by locator CCs (to avoid midi glut), so this feature works best when repurposing a pad for percussion or for a non-musical purpose. For musical purposes, see Tuning Table mode below.

Details: one type of CC is sent for note-ons in cols 1-16 and another type of CC is sent for note-ons in cols 17-25. For cols 1-16, the data value = (row - 1) + 8 * (col - 1). For cols 17-25, the data value = (row - 1) + 8 * (col - 17). The top row is row 1 and the leftmost column is column 1. Choose the two CC types through the microLinn menu, see below. If your DAW isn't programmable, download locatorCC.jsfx from the github. It defines a rectangular region on the Linnstrument, within which it can either set notes to a fixed note or transform them into a CC message. It runs natively in Reaper and can run in any windows DAW using ReaJS, a free jsfx-to-VST wrapper. Thanks to KVR forum member vorp40 for the locator CC idea!

-------- LIKELY ADDITIONAL NON-MICROTONAL FEATURES (no promises) -------- 

HAMMER-ONS AND PULL-OFFS: Set the hammer-on window to say 150¢. When you play a note within 150¢ of another note on the same row, it will hammer-on and mute the other note, as if you were in channel-per-row mode. When you release it, the old note will be unmuted (pull-off). Access it through the microLinn menu, see below.

DIM MODE: The Low Power pad (col 15 in Global Settings) dims the display but also increases the latency. Long-press the Low Power pad to dim the display with no latency increase, good for tired eyes.

DOUBLE VOLUME FADERS: On the Volume screen, press both split pads at once. There will be two horizontal faders, one for each split. You can use a splitter on your computer's stereo headphones output and send one side to your amp and the other to an earbud that you wear as you play as an in-ear monitor. In your DAW, send the synth's output to two tracks, each hard-panned to opposite sides. Each track receives the Linnstrument's midi from one of the 2 main channels (usually 1 and 16). Each track has a gain effect which you midi-learn to the volume CC. Now one volume fader will control your amp and the other will control your earbud. Good for noisy gigs, good for chaotic jam sessions where you need to discretely find the key.

"YIP" FIX: see https://www.kvraudio.com/forum/viewtopic.php?p=8885419#p8885419

CHANNEL PRESSURE FIX: see https://www.kvraudio.com/forum/viewtopic.php?t=591770


-------- MICROTONAL FEATURES -------- 

On the Global Settings screen, long-press the lower left pad (VIEW MAIN) to go to the main microLinn menu. Once the edo (notes per octave) is set to anything other than OFF, VIEW MAIN turns light blue and you can short-press it.

Main menu, *LONG-PRESS EACH BUTTON* to see its function.

1) set left/right column offset
2) set left/right row offset

3) set edo (notes per octave)
4) set octave stretch in cents

5) choose anchor pad
6) set anchor note
7) set anchor cents

8) choose note lights

9) set additional features

10) set drum pad mode, locator CCs, and Tuning Table mode

EDOS

The edo (the notes per octave) ranges from 5edo to 55edo, plus "OFF" which makes the Linnstrument run normally. Change the edo by swiping. Once microLinn is on (i.e. once an edo is chosen), you can cycle thru the edos by setting a switch or footswitch to EDO+ or EDO- (long-press TAP TEMPO and swipe). Changing the edo automatically adjusts all column offsets and row offsets so that their size in cents stays roughly the same. So your bosanquet keyboard layout remains roughly bosanquet, and your F#BEADGBE guitar tuning remains roughly F#BEADGBE.

The anchor pad is a specific pad that doesn't change pitch when you change the edo. The anchor pad chooser displays the row and column of the current anchor pad. For example, "R4C11" (or "4 11" on the Linnstrument 128) means row 4 (from the top) and column 11. Tap the blue "R4C11" anywhere and you'll see the normal display with the anchor pad blinking. Tap any pad to set it as the new anchor pad. This pad beomes the new tonic, and all the lights will shift accordingly.

An edo can be stretched, allowing non-octave scales like Bohlen-Pierce.

Changing the anchor note transposes by 12edo semitones. Changing the anchor cents detunes the entire Linnstrument. Each split can be transposed by edosteps via the Octave/Transpose screen. Like the guitar tuning screen, a midi note is sent when you change either the anchor note or the anchor cents.

When the notes per octave is greater than 12, the OCTAVE/TRANSPOSE screen shows two extra rows for transposing by edosteps. The 2nd and 3rd rows now transpose not by semitones but by major 2nds (since most edos have several different semitones). A major 2nd is defined as the interval between the 4th and the 5th, e.g. 3 edosteps for 15edo but only 2 edosteps for 16edo.

MicroLinn can be set to 12edo. What advantages does microLinn's 12edo have over the standard, non-microLinn 12edo? It can be stretched and/or detuned, plus you can have multi-colored note lights.

GUITAR TUNING

Once an edo is chosen, the Linnstrument uses a completely independent guitar tuning than the usual one. Changing one tuning doesn't affect the other. MicroLinn's guitar tuning screen doesn't adjust the pitch of each "string". Instead it sets 7 independent row offsets. The "anchor string" is the row that the anchor pad is on. Its pitch is determined solely by the anchor pad, note and cents. On the far left, the anchor string is red, or orange if lit up. The others are green, or cyan if lit up. Tap any button on the far left to select a string. The button lights up and sounds that open string. Assuming it's not the anchor string, one of the neighboring strings, whichever one is closest to the anchor, also lights up. You won't see a note name with an octave number. Instead you'll see a row offset as a number, which can be negative. This is the row offset between the two lit-up strings. Swipe right or left on it as before to increase or decrease it. (Selecting the anchor string doesn't show a row offset.) 

Changing one row offset doesn't affect the other six row offsets. Thus increasing any row offset above the anchor string sharpens the current string and all strings above it. And increasing any row offset below the anchor string *flattens* the current string and all strings below it. (Of the two lit-up buttons, the current string is always the one furthest from the anchor string.) To summarize, you're *seeing* the offset between the two lit-up strings, but *hearing* the pitch of the outermost lit-up string.

If the guitar tuning is the standard tuning, on the Global Settings screen the GUITAR pad is dark blue, otherwise it's bright blue. The exact notes don't matter, just that the intervals between open strings are all 4ths, except for that one major 3rd. Details: The edo's perfect 4th is its closest approximation to 4/3. The edo's major 3rd is two octaves minus four 4ths. There's two possible 4ths for 13edo (5\13 and 6\13) and 18edo (7\18 and 8\18). Either 4th keeps the GUITAR pad dark blue.

In Global Settings, when you long-press the OCTAVE pad, the "-GUI" option for reversed guitar tuning is not available when microLinn is on. To get this reversed tuning, set the guitar tuning manually.

NOTE LIGHTS

The 9 scales in Global Settings columns 2-4 are now microtonal and change for each edo. You can still select a scale using columns 2-4, but you can no longer edit a scale there, because for larger edos there are too many notes to fit. As a result, when microLinn is on, the VIEW MAIN and VIEW ACCENT buttons do not work, and the SCALE SELECT button is always on. To edit a scale and its colors, instead go to the main microLinn menu and go to the note lights screen. Shortcut: you can long-press the scale's pad in cols 2-4 in the Global Settings screen to go directly to that scale.

The note lights screen has 7 scale buttons plus the rainbow editor, the dots selector and the yellow rainbow-enabler button. Excluding the rainbow enabler, there are 9 buttons, corresponding to the 9 scales in cols 2-4 in the Global Settings screen. Tap any of these 9 buttons to select it. Tap any already-selected button to backtrack to the previous button. There are 7 rows of colored lights on the screen, which top to bottom are for unisons, 2nds, 3rds, 4ths, 5ths, 6ths and 7ths. Tap a note in a scale to toggle it on or off. Like the guitar tuning screen, a midi note is sent when you tap. The 8th scale is the rainbow editor, in which all notes are always on. Tap a note to cycle it thru the rainbow. The 9th scale (guitar-like fretboard dots) isn't really a scale. It's a full-screen display like the custom light patterns. Tapping the dots selector makes the dots appear in blue mid-screen. Tapping the dots lets you toggle them on or off. To change the color of the dots, after editing go to Per-Split Settings and in column 11 change the main color. Long-press the scale buttons or the rainbow editor button or the dots selector button to reset the note lights to the default. Tap the yellow rainbow enabler button to turn off the rainbow and limit the note lights to the usual two colors.

The 3 custom light patterns are totally separate from all this and are still available for use! This web browser lets you easily edit them: https://forrcaho.github.io/linnstrument_colorizer/

Default scales: of the 8 scales, the 1st and 2nd scales are 5-limit major and 5-limit minor. Scales 3-6 are blank but for the tonic, so that you can create your own scales on the note lights screen. (For the smaller, weirder edos, scales 1-2 are also blank.) The 7th scale is a partial rainbow, and the 8th scale is always the full rainbow.

The colors use the rainbow metaphor, red-yellow-green-blue = sharp to flat = supermajor to subminor
* white = 12-edo-ish = 3-limit
* yellow / green = downmajor / upminor =  5-over / 5-under (examples: 5/4 and 5/3 are 5-over, 6/5 and 16/15 are 5-under)
* blue / red = downminor / upmajor = 7-over / 7-under
* purple = neutral = 11-over or 11-under or 13-over or 13-under
* pink is reserved for the exact half-octave of 600c, 12-edo-ish but not quite 3-limit, "off-white"
* cyan / orange = a catch-all pair, e.g. 41edo 7/5 and 10/7, cyan is also for "outside" notes aka interordinals e.g. 24edo
* orange is used in 55edo for upmid and downmid notes

The dot patterns tend to follow the conventional m3 P4 P5 M6 P8 guitar fret markers. Some edos add M2 and m7. Edos above 24 approximate 12edo, in other words there are dots about every 100 cents. 41edo is an exception. It has kites like a Kite guitar.

MEMORIES

All of microLinn's settings are remembered by the 6 memories. If you load a memory that has microLinn turned OFF, the microtonal data will not be altered. (And if the column offsets and per-split row offsets are also OFF, they will not be altered either.) This lets you use certain memories to configure your synth-related settings only. Load such memories *after* loading a microtonal memory.

The memory on the bottom row is an exception to this. It *will* alter microtonal data even if it has microLinn turned OFF. You can use this memory to quickly reset everything to 12edo, or perhaps to your favorite edo. 

The 2nd memory from the bottom is 41edo Kite guitar (row offset 13, column offset 2), with an alternating-3rds guitar tuning.

TUNING TABLE MODE 
(this section is a work in progress)

When an edo is selected, the Linnstrument outputs standard midi notes with tuning bends. But Tuning Table mode outputs edosteps instead. The lower left pad (or the lower right pad if lefthanded) is midi note 0. The midi notes for the other pads on that row increase from there according to the split's column offset. Thus each pitch is a unique midi note. (When in 12edo, each pitch already has its own unique midi note, and Tuning Table mode has no effect.)

Each split can be set independently to Tuning Table mode. If a split uses both No Overlap offset and Tuning Table mode, not only does each pitch have a unique midi note, each pad does too. Such a split can be used to launch audio or midi clips while you play normally in the other split.

To create a JI or rank-2 scale with N notes, set the edo to N and turn on Tuning Table mode. Load a scala file into your synth or run microtonal software such as alt-tuner to produce that N-note scale. Each note is slightly sharper or flatter from N-edo, thus the pad's pitch will be different slid up to vs played directly. However this is only a comma or so difference even on long slides, which might be tolerable. (Possibly software on the computer could fix this problem by scaling the slides by adjusting the pitch bends?) Suppose you want to detemper the Kite guitar layout into a JI tuning. If you use column offsets in No OverLap mode, the row offset isn't affected by increasing the column offset, it remains the physical width of the split.

The Global Settings No OverLap offset can only cover 128 pads. But the per-split No OverLap offset combined with Tuning Table mode can give each of the 200 pads its own unique midi note / channel combo, even with large row/column offsets. Details: the row offset equals the width of the split, 25. Midi notes above 127 wrap around and start over at zero. When both Tuning Table mode and locator CC #1 are turned on, the locator CC value changes to the midi group number, where group 1 is notes 0-127, group 2 is notes 128-255, etc. You can have up to 4 groups. The group number indicates whether note 0 is really note 128, note 1 is really note 129, etc. Your DAW can then run a simple script that redirects notes of a different group to a track with a different instance of your softsynth, into which you have loaded a different scala file. See the TallKite github for example code. 

Under certain conditions, microLinn will ID the midi group by automatically rechannelizing the midi. The midi channels come in blocks of 7, 5 or 4 channels each, corresponding to one of 2, 3 or 4 midi groups. Each block is sent to a different synth.
* Set the midi mode to channel per note
* Turn on Tuning Table mode
* Set locator CC #1 to off
* Probably use only one split, because most or all of the 16 midi channels will be used up
* Set that split to use MPE midi channels according to the number of midi groups needed:
* * If numGroups = 2, use channels 2-8 (the channel for group 2 increases by 7 and becomes channels 9-15)
* * If numGroups = 3, use channels 2-6 (the channel for group 2 increases by 5, and group 3 increases by 10)
* * If numGroups = 4, use channels 1-4, no Main channel (group 2 increases by 4, group 3 by 8, and group 4 by 12)
If you want to switch quickly between col/row offsets while performing, you can set the midi channels of the right split identical to those of the left split. For example if there are two midi groups, set both splits to use channels 2-8 plus main channel 1.

SUGGESTIONS FOR EXPLORING EDOS:

* The first few edos are pretty strange. You may want to start with 15, 17, 19 or 22.
* Be sure to try the Bosanquet layout for 31edo and the Kite guitar layout for 41edo (see KiteGuitar.com).
* The full rainbow scale can be overwhelming. Try setting the played mode to BLNK.
* To avoid a touched pad turning red (or whatever) and obscuring the pad's usual color, set the played color to blank.
* Once you know an edo well, you'll probably want to switch to the fretboard dots display.

KNOWN ISSUES

These are all minor, and the solution is "don't do that"!
* When microLinn is on, don't set the pitch bend range small like 2 semitones, because tuning bends of up to 50¢ will reduce that even further.
* In drum pad mode, don't slide from one pad to the next, because you'll get hanging notes.

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
