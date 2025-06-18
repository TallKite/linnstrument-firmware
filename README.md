/**************************************** MICROLINN ****************************************/

Unfinished, still a few more features to add, plus a few features don't quite work right yet. 
Beta testers needed!

The  Linnstrument is an amazing instrument, very feature-rich. If it's new to you, we *strongly* reccommend waiting to install microLinn until after you've explored all the standard features.

For experienced linnstrumentalists, microLinn makes exploring microtonality very easy. Load up your usual MPE synths, set the notes per octave, and play! It's that simple!


================== INSTALLATION ================== 


1) Go to https://www.rogerlinndesign.com/support/support-linnstrument-update-software and follow the "How to Check Your Software Version" instructions. If it's not 2.3.3, follow the "How to Update Your LinnStrument Software‍" instructions to update to 2.3.3.
2) Download linnstrument-firmware-microLinn.ino.bin.zip from the Linnstrument Community wiki and unzip it. Important: if on a mac, put the .bin file on your **desktop**. 
3) Follow the "How to Update Your LinnStrument Software‍" instructions, with one difference: after you download (and possibly unzip) the updater and before running it, put it in the same folder as the .bin file from step 2. Mac users: when you run the updater, if it asks for permission to read files from the desktop, say yes.
4) Check your OS version to confirm the update. Tap twice to see all three numbers. It should be 234.072.001 or higher. If not, reboot your computer and try again. 
5) Important, read the next section about uninstalling!


================== UPGRADING / UNINSTALLING ================== 


IMPORTANT: Updating to a newer version of microLinn is done normally (altho you may have to unplug the Linnstrument and plug it back in afterwards). But if you want to uninstall microLinn and go back to an official (non-microtonal) version of the firmware, there's an extra step. Just before you run the updater, on the Global Settings screen, tap the pad to the right of the "Update OS" pad (17th column, 3rd row from the bottom) so that it turns dark blue. On a 128, tap the pad just above the calibration pad, 16th column, 5th row. This avoids deleting your calibration data and user settings.

If you see "Couldn't retrieve Linnstrument's settings, interrupting firmware upgrade. Go ahead with default settings?", quit the updater app and try updating with the opposite setting.


================== MAIN MENU ================== 


To go to the main microLinn menu, on the Global Settings screen, long-press the lower left pad (VIEW MAIN). Once the edo (notes per octave) is set to anything other than OFF, VIEW MAIN turns light blue and you can short-press it. On a LinnStrument 200, you can also tap the green button in column 17.

Main menu, *LONG-PRESS EACH BUTTON* to see its function

  col 2) Per-split column offset (OFF, 2 to 8)
  
  col 3) Per-split row offset (OFF, -25 to 25, NOVR = no overlap)
  
  col 5) Per-split non-microtonal settings
    row 1) Show custom light pattern (OFF, A, A#, B, A', A#', B')
    rows 2-5) (reserved for future use)
  
  col 7) Global non-microtonal settings
    row 1) Drum pad mode (OFF, ON)
    row 2) Locator CC #1 (OFF, 0 to 119)
    row 3) Locator CC #2 (OFF, 0 to 119)
    row 4) Allow Importing (OFF, ON)
  
  col 10) EDO (notes per octave) (OFF, 5 to 55)
  
  col 12) Microtonal note lights
    col 1, row 1-7) Select scales 1-7 (long-press to reset the notes)
    col 3, row 1) Rainbow editor (long-press to reset the colors)
    col 3, row 2) Rainbow enabler (yellow = ON, green = OFF)
    col 3, row 4) Fretboard editor (long-press to reset the dots)
    columns 5-16) (tap here to select notes, colors or fretboard dots)
  
  col 14) Per-split microtonal settings
    row 1) Condense to scale (OFF, VAR = variable bend per pad, 1 to N = bend per pad in edosteps)
    row 2) Default layout (OFF, Bosanquet, Wicki-Hayden, Harmonic Table, Accordion, Array Mbira)
    row 3) Tuning table mode (OFF, ON, RCH = rechannel)
  
  col 16) Global microtonal settings
    row 1) Anchor pad (row 1 to row 8, column 1 to column 25)
    row 2) Anchor note (C-2 to G8, middle-C is C3)
    row 3) Anchor cents (-60 to 60)
    row 4) Equave semitones (1 to 36)
    row 5) Equave cents (-60 to 60)


================== NON-MICROTONAL FEATURES ================== 


COLUMN OFFSETS

Ranges from 1 (OFF) to 8. For negative offsets, use the hidden lefthanded setting in Global Settings column 1. The usual Linnstrument tuning is row offset +5 and column offset +1, or (+5 +1). Each of the three most popular hexagonal-key layouts can be translated to the Linnstrument's square keys in two ways:
* Bosanquet or Janko layout translates to (-1 +2) or (+1 +2)
* Wicki-Hayden layout translates to (+5 +2) or (+7 +2)
* Harmonic Table layout translates to (+4 +3) or (+7 +3)

The column offset can be set for each split independently. You can have one of these six layouts on the left for easy chord playing (probably with pitch bending off) and the usual layout on the right for easy melody playing, somewhat analogous to an accordion's layout. Accessed through the microLinn menu.

You can set both column offsets at once by linking them. In the upper right there are two split buttons. Hold the right one and tap the left one. You'll be switched to the left split, and the left offset will change to match the right offset. Both split buttons remain lit up, and any change affects both offsets. To unlink them, tap either split button. 

----->  All microLinn per-split settings can be linked this way.  <------

*Details: Beware, if the column offset is 2 and the row offset is an even number, you lose half the notes and only get a whole-tone scale. In general, the column offset and the row offset should not have any common factors. If they do, both the row offset and the column offset will be displayed in red.*

*Playing melodies with pitch bending can be tricky. A 1mm slide normally bends the pitch about 6¢. But an offset of +2 makes it 12¢, +3 makes it 18¢, etc. Thus to play in tune with a large column offset you may need both Pitch/X Quantize and Pitch/X Quantize Hold to be on. But if your column offset is +2, setting Quantize on and Quantize Hold off lets you play the skipped notes fairly accurately by sliding into the gap between pads. In fact, you can play 24edo quartertones on a normal 12edo Linnstrument fairly easily this way.*

PER-SPLIT ROW OFFSETS

Setting the row offset for a split overrides the Global Settings row offset for that split only. It ranges from -25 to +25, plus OFF and No Overlap which appears as NOVR. A No Overlap split can be used to launch audio or midi clips while you play normally in the other split. Accessed through the microLinn menu. 

CONDENSE TO SCALE

Make the unlit pads go away! Bending still works. Accessed through the microLinn menu.

*Details: Condensing uses various microtonal features. Set the edo to 12 and select one of the 7 microtonal scales. If your scale is all one color (e.g. the 12edo major scale is all white notes), fix that in the note lights screen. Adjust the color of one or more notes with the rainbow editor, or simply turn off the rainbow enabler to use the usual 2 colors.*

*In the Per-split Microtonal Settings menu, top row, change Condense To Scale from OFF to VAR, which stands for variable bending. When you slide up to a note and then play it again without sliding, the bent pitch matches the unbent pitch. The bend slope is usually a constant 1 semitone per pad, which is about 6¢ per mm. But now the slope is lower when traversing a small scale step and higher when traversing a large scale step. Over a 12edo minor 2nd it's the usual 6¢/mm, but over a 12edo major 2nd it doubles to 12¢/mm. The bend slope changes sharply at the center of the pad.*

*This variable bend slope causes certain issues. (1) It makes vibrato on certain notes lopsided. For example, a normal vibrato on C of a C major scale will tend to be wider when sharpening and narrower when flattening, making the C note sound slightly sharp. So you might want to do a slightly off-center vibrato, going closer to B than to D. (2) When sliding slowly at a steady speed through a large interval, you may notice the bending becoming faster and slower. So you might want to slide through minor 2nds faster than major 2nds.*

*If you rarely do large bends, you might prefer a consistent bend slope. This avoids lopsided vibrato. You can set the slope to a specific number of edosteps per pad, ranging from 1 to whatever the largest step is. For example, the largest step in a 12edo major scale is 2 semitones (or "edosteps"), so you can swipe Condense To Scale past VAR to either 1 or 2. You can still do large bends, but they can be tricky. If you choose 1 edostep, overshoot and bend by ear. If you choose 2 edosteps, set QUANTIZE HOLD off and you can slide into the gap between pads to reach all the notes of the scale.*

*You can also experiment with changing the pitch bend range on the Linnstrument to the number of notes in your scale, or some multiple of that. For a 7 note scale, change 12 to 7, 24 to 14, etc. With Condense To Scale set to 1, this makes your bend slope 1.7 semitones per pad = 10¢/mm. This is good for very large bends, because the octaves are easy to slide to.*

*If you turn off pitch bending (thus avoiding all these issues), there's no difference between VAR, 1, and 2.*

Condensing can be combined with column offsets. First the scale is condensed, then columns are omitted.

There's a school of thought that says there's only 12 notes, it's not that hard to learn your way around, and removing 5 of the 12 notes doesn't make the scale all that much more compact. So is it really worth condensing if it causes bending issues? Perhaps not if one plays only in 12edo. But if playing microtonally, condensing a large edo to a smaller scale can be very useful. For example, you can condense a 31edo chain-of-5ths scale of 12 notes to get quarter-comma meantone. But even in 12edo, condensing has its uses. For example, you can use condensing to create a vertical Wicki-Hayden layout. (Playing vertically means rotating the Linnstrument 90 degrees.) The range is huge, almost 8 octaves on a Linnstrument 128!

*Details: Condensing usually affects the row offsets as well as the column offsets. It makes the row offsets an inconsistent number of semitones but a consistent number of scale steps, e.g. sometimes 3 semitones, sometimes 4, but always a third. However, if using a guitar tuning, this is not always true.*

*In the guitar tuning dispaly, when you select the anchor row, a row offset is not displayed. Usually nothing is displayed. But when condensing is on, you'll see DIA for diatonic/condensed. Swipe right to get CHRO for chromatic/uncondensed. In diatonic mode, the row offsets are condensed as usual. But in chromatic mode, the row offsets don't get condensed, and the notes in the anchor column don't change. Each row is an exact chromatic transposition of the anchor row. Thus while the anchor row has no unlit pads, the other rows usually do.*

*Vertical Wicki-Hayden layout:*
* *Set both the column offset and the per-split row offset to OFF*
* *Set the EDO to 12*
* *Set up one of the first 7 scales to contain only the tonic and the 4th*
* *Switch directly from that scale to the 8th scale with the rainbow enabled*
* *Set Condense To Scale to 1*
* *Set the anchor pad to row 3 col 8 (col 12 on a Linn 200) and the anchor note to C3*
* *Set the guitar tuning to -2 between all strings and change DIA to CHRO*
* *Optional: Condense the other split similarly, and in the left split turn off pitch bending, and in the right split turn on pitch quantization*

*You'll get a "squared-off" Wicki-Hayden that doesn't drift off sideways. The 7 white keys will be in rows 3-6. The 5 black keys (actually green) will be mostly in the top two rows and the bottom two rows. The row offset will be -2 and the column offset will alternate between +5 and +7. Now rotate the Linnstrument 90 degrees so that the Settings buttons are closest to you and play! Swipe vertically for pitch bends.*

*This layout is not isomorphic but it is dimorphic (two shapes). The restriction of only 3 notes per column becomes only 3 notes per row, much less of an issue.*

*(Alternative layout: set the scale to use the 5th not the 4th, and set the anchor pad to row 4 not 3.)*

CHANNEL PRESSURE FIX

The Linnstrument normally resets pressure data at note on/off, and only sends pressure data for the most recent pad held. For non-MPE synths (or any synth played in "One Chan" or "ChPerRow" mode), releasing a note causes an abrupt jump in z-values from 0 to the value of whatever prior pad is held. Y-values behave similarly. MicroLinn includes a fix by KVR forum member teknico that solves the problem.  

*Details: The output of both Y (timbre) and Z (loudness) when not using Polyphonic Aftertouch is thus changed:*
* *Zero-value messages before note-on and note-off are only sent for the first simultaneous note on the same channel*
* *Only the maximum value of all simultaneous notes on the same channel is sent*
*This allows smoother playing in One Channel and Channel Per Row modes and better compatibility with non-MPE synths, while not impacting Channel per Note (MPE) mode (except in the rare case of more that one note on the same channel). See https://www.kvraudio.com/forum/viewtopic.php?t=591770*

*This fix is permanently enabled. This is a rare instance of microLinn changing the behavior of the Linnstrument, as opposed to adding a new feature. If anyone prefers the old behavior, let us know, and we can make the new behavior optional.*

OCTAVE TOGGLE, QUANTIZE TOGGLE

Two new functions for switches and footswitches. 8VE± flips back and forth between Octave Up and normal. This lets you switch octaves while playing using only one footswitch, instead of two for Octave Up and Octave Down. QNT± toggles the Quantize option for the active split. Access them both by long-pressing TAP TEMPO and swiping.

DRUM PAD MODE

The note lights become 14 3x3 mega-pads (or 10 on the Linnstrument 128) that play the 14 drum sounds from the sequencer. Accessed through the microLinn menu.

*Details:*
* *Set PITCH/X on, otherwise a tap that hits two pads will send two notes. The drum sounds won't be accidentally pitch-bent because drum pad mode removes pitch bends.*
* *You can use the low row for pitch bending, restriking, etc.*
* *The midi mode should probably be One Channel.*
* *The sequencer has different drum sounds for each split. If you're using both splits, set the split point in between the pads.*
* *The two pad colors are the main/accent colors of the split, so you can adjust them if you want. Any custom light pattern you select will overlay the drum pads. So you can make the center of each pad a different color if you want.*
* *If you use musical sounds instead of drum sounds, and enter the appropriate pitches into the sequencer, you can make a sort of marimba.*
* *To create your own mega-pad layout, turn off Drum Pad mode, use locator CCs instead (see below) and edit one of the custom light patterns to match.*

DETUNING

Detune the entire Linnstrument up or down from A-440 to match a similarly detuned recording or instrument. No guarantee that detuning to A-432 will heal your chakras lol. Accessed through microLinn's anchor cents after setting the edo to 12.

CHAINING SEQUENCES

Hold the pad for sequence #2 and tap the pad for sequence #1. Now the two sequences are chained into a single sequence twice as long. This is indicated by sequence #1 being accented and sequence #2 blinking. You can chain #3 and #4 together as well. Or chain all 4 together. The lights for the entire chain will blink when it's playing. When making a chain, to start at the beginning of the chain not the end, hold the last sequence and tap the first one. 

You can select sequences and chains on the fly as the sequencer is playing. You can also chain and unchain on the fly. Straight/dotted/triplet/swing and quarter/8th/16th are still set individually for each sequence, so a chain can mix these together. To clear all chains in a split, tap the hidden switch immediately to the left of the 4 selector pads. Or just unplug the Linnstrument.

SEQUENCER PEDALS

When playing in one split and using the other split as a sequencer, it's no longer necessary to switch to the other split before using the following footpedals (or switches or midi NRPN messages): PLAY, PREV, NEXT and MUTE. 

*Details: Outside of a chain, the NEXT and PREV footswitches take you to the next/previous sequence as before. You can double-tap or triple-tap the NEXT and PREV footswitches to skip forward/backward multiple sequences. Thus triple-tapping NEXT is the same as single-tapping PREV, which means you only need one footswitches to go anywhere.*

*But from within a chain, NEXT and PREV operate relative to the upcoming sequence in the chain, not the current one. Thus pressing PREV repeats the current sequence and pressing NEXT goes forward two sequences, not one. One exception: from the rightmost sequence of a chain, NEXT exits the chain. (Otherwise you could never exit.)*

DOUBLE VOLUME FADERS

The Volume display now has two horizontal faders, one for each split. Incidentally, the volume faders are and always have been more accurate than their appearance suggests. Slide within a pad for fine adjustments. They also do and always have done something similar to hammer-ons and pull-offs.

You can use a splitter on your computer's stereo headphones output to send one audio channel to your amp and the other to an earbud that you use as an in-ear monitor. You can control the volume of both from the Volume screen. Good for noisy gigs, good for chaotic jam sessions where you need to discretely find the key or the chords.

*Details: In your DAW, send the synth's output to two tracks, each hard-panned to opposite sides. Each track receives the Linnstrument's midi from one of the 2 main midi channels (usually 1 and 16). Each track has a gain effect which you midi-learn to the volume CC. Now one volume fader will control your amp and the other will control your earbud.*

BLINKING MODE

Like the SAME mode, BLNK shows you other occurences of the currently played note. But instead of changing color, the other occurences blink. It's good for multi-colored displays like custom light pattern #2 (the one marked as A#) or certain microtonal displays. 

*Details: In Per-split Settings, long-press the PLAYED color. The BLNK option appears right after CELL and SAME. Don't confuse BLNK for blinking with BLIN for blinders.*

SAME/BLINK CARRY OVER

If both splits are set to SAME or BLNK, playing in one split now shows matching notes in the other split too.

MULTI-COLORED NOTE LIGHTS

Each of the 12 notes can be any color. Transposable. Accessed through the microLinn menu after setting the edo to 12.

SHOW A CUSTOM LIGHT PATTERN IN ONE SPLIT ONLY

Choose any of the 3 patterns (the scales marked A, A# and B) and it will replace the note lights. If you use one split as a clip launcher, you can color-code your clips. See also Locator CCs below. Choosing A', A#' or B' displays the note lights as well, on top of the light pattern. BTW this feature fixes a bug in which a custom light pattern would cover up a split set to CC faders or strum. Accessed through the microLinn menu.

DIM MODE

Normally, tapping the Low Power button (Global Settings column 15) dims the display but also increases the latency. MicroLinn adds the option to long-press the Low Power button to dim the display without adding latency (in effect a crude brightness knob).

LOCATOR CCs

A locator CC message can be sent immediately before every note-on, indicating the row and column. Your DAW can assign a specific function to a specific pad, e.g. upper lefthand corner = all sound off. Or use this feature plus a custom light pattern to create large drum pads for more easily playing drum midi. Or create a third split, perhaps a column of on/off buttons, or a vertical fader or two, or even a horizontal split. Accessed through the microLinn menu. 

*Details: One type of CC is sent for note-ons in cols 1-16 and another type of CC is sent for note-ons in cols 17-25. For cols 1-16, the data value = (row - 1) + 8 * (col - 1). For cols 17-25, the data value = (row - 1) + 8 * (col - 17). A Linnstrument 128 will never send the 2nd type. The top row is row 1 and the leftmost column is column 1. Don't choose a CC that might be used for other purposes, such as CC 74 for timbre or CC 64 for sustain.*

*Locator CCs are mainly for Channel Per Note mode. There's not much point in using them in Channel Per Row mode, since the channel serves to locate the note. In One Channel mode the CC can locate a note-on but if the same note is played twice on two separate pads, it can't locate the XYZ data.*

*If your DAW isn't programmable, download LinnstrumentLocatorCCs.jsfx from this github. It defines a rectangular region on the Linnstrument, within which it can either transpose each note to a specific note (good for drum pads) or transform it into a CC message in a variety of ways. It can also filter out other midi either inside or outside of this region. It runs natively in Reaper and can run in any windows DAW using ReaJS, a free jsfx-to-VST wrapper. (Hopefully someone can duplicate this effect in Max 4 Live and other platforms.) Thanks to KVR forum member vorp40 for the locator CC idea!*

IMPORTING/EXPORTING

You can back up various settings and/or share them with others via midi files. The 3 custom light patterns, the 16 audience messages, the 6 memories, microtonal data, and more! Check the Linnstrument Community Wiki for export-request files and importable settings files.

*Details: Importing: Download a settings file from the wiki. On your Linnstrument, set Allow Importing to ON. In your DAW, set the output of midi track A to your Linnstrument. Load the settings file into track A and press play. Your Linnstrument should scroll "IMPORT SUCCESS". If you see "IMPORT FAILURE", try again. If you don't see anything, see troubleshooting #8 below.*

*To import a custom light pattern, before you press play, you must first display the one you want to overwrite. To import an audience message, before you press play, you must first load for editing the one you want to overwrite. (You needn't actually edit it.)*

*All Settings for Current EDO imports/exports not only the scales, rainbow and fretboard, but also the equave semitones, the equave cents, the guitar tuning, and all row and column offsets.*

*Exporting: Download an export-request file from the wiki. In your DAW, set the input of midi track A and the output of midi track B to your Linnstrument. Put the export-request midi file at the start of track B. Then start recording on track A. When your Linnstrument stops sending midi, stop recording. Save the midi on track A to a midi file (i.e. export it from your DAW), preferably as format 0. Name it something informative such as "lightPattern31edoFretboard.mid" or "22edoScales.mid". Later on you can import this file to restore your settings. Or share it on the wiki.*

*Exporting multiple requests: 1st method: Send the first request and see in your DAW where the Linnstrument stops sending midi. Position the next request file in track B about 1/10 of a second after that. Add additional request files similarly. Record all requests at once, and save the midi to a file as before. 2nd method: Record each export individually. Then position those midi clips next to each other in your DAW, and save them all as one file.*

*Troubleshooting:*
* *(1) While importing or exporting, don't play your Linnstrument.*
* *(2) To import, in your DAW, your Linnstrument must be enabled as a midi output device (not just input).*
* *(3) Your DAW must be able to handle a midi file that uses multiple midi channels.*
* *(4) When importing, first exit all web browsers in case a webmidi page sends rouge NRPN or polypressure messages.*
* *(5) When importing, slowing down your DAW's playback speed sometimes helps. Likewise, when exporting, speeding up your DAW's recording speed can help.*
* *(6) The All User Settings export is meant for migrating from an old Linnstrument to a new one. It will only work if the OS version on the old one matches the OS version on the new one (or more precisely, if the data structure versions match).*
* *(7) If you get "IMPORT FAILURE" followed by 2 numbers, the 2nd number says which midi message in the import file caused the failure. For example, 7 means the 7th midi message.*
* *(8) MicroLinn imports data via polyphonic pressure messages. If you have connected something else to your Linnstrument that also sends polypressure messages, there is a small possibility of confusion. If after an import there is no sucess or failure scrolling message, to avoid confusion either set Allow Importing to OFF, or disconnect the Linnstrument's power and reconnect it.*

*The 16 sequencer projects can be imported and exported as usual via the updater app as *.lpr files. When you export, the filename of microtonal projects should probably contain the edo, since it won't sound right in the wrong edo. The AllUserSettings export does not export the current sequencer project, so to export it save it as one of the 16 projects.*

Import request files available on the Linnstrument Community Wiki:

* requestCurrentLightPattern.mid
* requestAllLightPatterns.mid
* requestCurrentAudienceMsg.mid
* requestAllAudienceMsgs.mid

* requestScalesCurrentEDO.mid
* requestRainbowCurrentEDO.mid
* requestFretboardCurrentEDO.mid
* requestAllSettingsCurrentEDO.mid

* requestScalesAllEDOs.mid
* requestRainbowsAllEDOs.mid
* requestFretboardsAllEDOs.mid
* requestScalesRainbowsFretboardsAllEDOs.mid

* requestCurrentMemory.mid
* requestAllMemories.mid
* requestAllUserSettings.mid


================== MICROTONAL FEATURES ================== 


EDOS

The edo (stands for Equal Division of an Octave, the notes per octave) ranges from 5edo to 55edo, plus "OFF" which makes the Linnstrument run normally. Change the edo by swiping sideways. Once microLinn is on (i.e. once an edo is chosen), you can cycle thru the edos by setting a switch or footswitch to EDO+ or EDO- (long-press TAP TEMPO and swipe). 

Changing the edo automatically adjusts all row offsets so that their size in cents stays roughly the same. So your fourths tuning will remain fourths, your fifths tuning will remain fifths, and your standard guitar tuning will remain standard. If either column offset is not OFF, it will get adjusted as well. Furthermore you can set the default layout for either split to be Bosanquet etc. and your layout will become (and remain) Bosanquet. 

*Details: A default layout is stored in the per-split row/col offsets, so it overrides the Global row offset. Bosanquet = m2/M2, Bosanquet 2 = A1/M2, Wicki-Hayden = P4/M2, Wicki-Hayden 2 = P5/M2, Harmonic Table = M3/m3, and Harmonic Table 2 = P5/m3. But this may vary from edo to edo, to ensure coprime row/col offsets. See also Guitar Tuning below.*

*The anchor pad is a specific pad that doesn't change pitch when you change the edo. The anchor pad chooser displays the row and column of the current anchor pad. For example, "R4C11" (or "4 11" on the Linnstrument 128) means row 4 (from the top) and column 11. Tap the blue "R4C11" anywhere and you'll see the normal display with the anchor pad blinking. Tap any pad to set it as the new anchor pad. This pad beomes the new tonic, and all the note lights will shift accordingly.*

Changing the anchor note transposes by 12edo semitones. Changing the anchor cents detunes the entire Linnstrument. Like the guitar tuning screen, a midi note is sent when you change either one. 

Besides transposing via the anchor note, each split can be transposed by edosteps via the Octave/Transpose screen. 

*Details: When the notes per octave is greater than 12, the screen shows two extra rows for transposing by edosteps. The 2nd and 3rd rows now transpose not by semitones but by major 2nds (since most edos have several different semitones). A major 2nd is defined as the interval between the 4th and the 5th, e.g. 3 edosteps for 15edo but only 2 edosteps for 16edo.*

It's possible to stretch the octave or even create non-octave scales like Bohlen-Pierce. 

*Details: The octave is really the equave, the interval of equivalence. It's defined as twelve 12edo semitones plus zero cents. Redefine it by swiping on Equave Semitones (1 to 36) and/or Equave Stretch (-60 to 60).*

MicroLinn can be set to 12edo. MicroLinn's 12edo has three advantages over the standard, non-microLinn 12edo. (1) It can be stretched and/or detuned. (2) You can have multi-colored note lights. (3) It can be condensed to the scale notes.

SUGGESTIONS FOR EXPLORING EDOS

* You can cycle through the edos by setting switch 1 to EDO+ and switch 2 to EDO- (won't work until you select an edo)
* Set the default layout to e.g. Bosanquet to stay in Bosanquet while you cycle
* The first few edos are pretty strange, so you may want to start with 15, 17, 19 or 22
* For nice-sounding chords, try the Bosanquet layout for 31edo and the Kite guitar layout for 41edo (memory #5)
* To avoid a touched pad turning red (or whatever) and obscuring the pad's usual color, set the played color to blank
* If the full rainbow scale seems overwhelming, try setting the played mode to blinking (BLNK)
* Once you know an edo and its layout well, you might want to switch to the fretboard dots display

GUITAR TUNING

MicroLinn's guitar tuning is completely independent of the usual one. The guitar tuning screen doesn't set the pitch of each "string". Instead it sets 7 independent row offsets. 

*Details: On the far left, there are 8 green buttons, one for each string. The "anchor string" is the row that the anchor pad is on. Its pitch is determined solely by the anchor pad, note and cents. The anchor string has a double button. Tap any button to select a string. The button turns light blue and sounds that open string. Assuming it's not the anchor string, one of the neighboring buttons, whichever one is closest to the anchor, turns dark blue. You won't see a note name with an octave number. Instead you'll see a row offset as a number, which can be negative. This is the interval between the two blue strings. Swipe right or left on it as before to increase or decrease it.*

*Changing one row offset doesn't affect the other six row offsets. Thus increasing any row offset above the anchor string sharpens the current string and all strings above it. And increasing any row offset below the anchor string _flattens_ the current string and all strings below it. To summarize, you're _seeing_ the offset between the two blue strings, but _hearing_ the pitch of the light blue string only.*

*If any of the 8 side buttons or 7 offsets is red, that means it's not coprime with one or both of the two column offsets.*

A guitar tuning is a standard tuning if the intervals between open strings are all 4ths, except for that one major 3rd between the 2nd and 3rd rows. The exact notes don't matter, just the intervals. If you switch edos while in a standard tuning, you'll stay in a standard tuning. 

*Details: When in a standard tuning, on the Global Settings screen the GUITAR pad is dark blue, otherwise it's bright blue (or possibly red). The edo's 4th is defined as its closest approximation to 4/3. The edo's major 3rd is defined as two octaves minus four 4ths. Thus 22edo's major 3rd is 8\22 = 436¢ not 7\22 = 382¢. This ensures a double octave from the 6th string to the 1st string. There's two possible 4ths for 13edo (5\13 and 6\13) and 18edo (7\18 and 8\18). Either 4th keeps the GUITAR pad dark blue.*

*In Global Settings, when you long-press the OCTAVE pad, the "-GUI" option for reversed guitar tuning is not available when microLinn is on. To get this reversed tuning, set the guitar tuning manually.*

NOTE LIGHTS

The 9 scales in Global Settings columns 2-4 are now microtonal and change for each edo. You can still select a scale using columns 2-4, but you can no longer edit a scale there, because for larger edos there are too many notes to fit into the 3x4 box. As a result, when microLinn is on, the VIEW MAIN and VIEW ACCENT buttons do not work, and the SCALE SELECT button is always on. To edit a scale and its colors, instead go to the main microLinn menu and go to the note lights screen. Shortcut: you can long-press the scale's pad in cols 2-4 in the Global Settings screen to go directly to that scale.

*Details: The note lights screen has 7 scale buttons plus the rainbow editor, the fretboard selector and the yellow rainbow-enabler button. Excluding the rainbow enabler, there are 9 buttons, corresponding to the 9 scales in cols 2-4 in the Global Settings screen. Tap any of these 9 buttons to select it. Tap any already-selected button to backtrack to the previous button. Repeatedly tap a button to quickly switch back and forth between two scales.*

*There are 7 rows of colored lights on the screen, which from top to bottom are for unisons, 2nds, 3rds, 4ths, 5ths, 6ths and 7ths. Tap a note in a scale to toggle it on or off. Like the guitar tuning screen, a midi note is sent when you tap. The 8th scale is the rainbow editor, in which all notes are always on. Tap a note to cycle it thru the rainbow. The 9th scale (guitar-like fretboard dots) isn't really a scale. It's a full-screen display like the custom light patterns. Tapping the fretboard selector makes the dots appear in dark blue mid-screen. Tapping the dots will toggle them on or off. The fretboard repeats at the octave. To change the color of the dots, after editing go to Per-Split Settings and in column 11 change the main color. Long-press the scale buttons or the rainbow editor button or the fretboard selector button to reset the note lights to the default. Tap the yellow rainbow enabler button to turn off the rainbow and limit the note lights to the usual two colors.*

The 3 custom light patterns are totally separate from all this and are still available for use!

Default scales: of the 8 scales, the 1st and 2nd scales are 5-limit major and 5-limit minor. Scales 3-6 are blank but for the tonic, so that you can create your own scales on the note lights screen. (For the smaller, weirder edos, scales 1-2 are also blank.) The 7th scale is a partial rainbow, and the 8th scale is always the full rainbow.

There are usually 7 white notes, corresponding to CDEFGAB. In a bosanquet layout, they are grouped CDE and FGAB. There are usually 5 yellow sharps and 5 green flats. Thus F# is always yellow and Bb is always green. Large edos have double sharps/flats, or even triple or quadruple.

           for most edos           for pentatonic and perfect edos
           white = natural         pink = tonic = anchor pad plus its octave-mates
           green = b              green = down
          yellow = #             yellow = up
            cyan = bb              cyan = dud (double down)
          orange = ## = x        orange = dup (double up)
            blue = bbb             blue = trup/trud (triple up/down)
            pink = x#
          purple = bbbb
             red = xx
          yellow/orange = bbbbb = xx#

* In perfect edos (7, 14, 21, 28 and 35) and pentatonic edos (5, 10, 15, 20, 25 and 30) the tonic is pink, to help it stand out
* In pentatonic edos and in supersharp edos (8, 13 and 18), there are only 5 white notes
* In superflat edos, green Bb is higher in pitch than white B
* Yellow is sometimes called lime, and yellow/orange is sometimes called yellow

The dot patterns tend to follow the conventional m3 P4 P5 M6 P8 guitar fret markers. Some edos add M2 and m7. Edos above 24 approximate 12edo, in other words there are dots about every 100 cents. 41edo is an exception. It has kites like a Kite guitar.

MEMORIES

All of microLinn's settings are stored in the 6 memories. If you load (or import via midi) a memory that has microLinn turned OFF, the microtonal data will not be altered. And if the column offsets and per-split row offsets are also OFF, they will not be altered either (because the edo often implies certain offsets). This lets you use certain memories to configure your synth-related settings only. Load such memories *after* loading a microtonal memory.

The memory on the bottom row is an exception to this. It *will* alter microtonal data even if it has microLinn turned OFF. You can load this memory to quickly return to 12edo.

The 2nd memory from the bottom emulates the [Kite guitar](https://KiteGuitar.com/). It's 41edo with a row/col offset of (+13 +2), with an alternating-3rds guitar tuning. This layout is both very playable and very well-tuned.

BEYOND EDOS: TUNING TABLE MODE 

Tuning table mode is meant for non-edo tunings such as just intonation or rank-2 temperaments. Each note of an N-note scale is sharp or flat from N-edo, thus the pad's pitch will be different slid up to vs played directly. However this is often only a comma difference even on long slides. You can hide the comma by turning on Quantize Hold. Otherwise you have to bend by ear, just like on a guitar.

Once an edo is selected, the Linnstrument outputs standard midi notes with tuning bends, and several edo notes will share the same midi note. But a split that's set to Tuning Table mode outputs edosteps instead. The lower left pad (or the lower right pad if the split is set to lefthanded) is midi note 0. The midi notes for the other pads increase from there according to the split's column and row offsets. Thus each edostep is a unique midi note. Certain synths need this format to play edos. But the main reason to use Tuninng Table mode is to retune each edostep individually to something else, by loading a tuning table in the form of a scala file into your synth (or running microtonal software such as alt-tuner).

The midi note number each pad outputs is determined entirely by the column and row offsets. The edo you select in theory doesn't matter. However when the edo is set correctly, transposing works better and the note lights make more sense.

There are only 128 midi notes. What if your tuning spans more than 128 edosteps? One solution is to use splits. Each split has up to 128 notes, so there can be a total of 256 pitches. But you can't slide across the split point. Also a large edo combined with large column and row offsets can actually exceed 256 edosteps. See the next two sections for two other solutions.

TUNING TABLE MODE WITH NO OVERLAP

If a split uses both No Overlap offset and Tuning Table mode, not only does each pitch have a unique midi note, each pad does too. Such a split can be used to launch audio or midi clips while you play normally in the other split.

No OverLap can only cover 128 pads per split. But No OverLap combined with Tuning Table mode can give each of the 200 pads its own unique midi note/channel combo, even with large row/column offsets. 

*Details: The row offset equals the width of the split, even if column offset is on. Midi notes above 127 wrap around and start over at zero. When both Tuning Table mode and locator CC #1 are turned on (but rechannelling is off), the locator CC value changes to the midi group number, where group 1 is notes 0-127, group 2 is notes 128-255, etc. Your DAW can then run a simple script that uses the locator CC to redirect notes of a different group to a track with a different instance of your softsynth, into which you have loaded a different scala file. See the TallKite github for example code. (to do: write this code)*

*Example: Suppose you want to detemper the Kite guitar layout into a JI tuning. (to do: complete this)*

TUNING TABLE MODE WITH RECHANNELLING

Rechannelling IDs the midi group by sending certain midi notes to channels beyond those selected. Rechannelling doesn't require programming your DAW like locator CCs do. But it does reduce the number of channels you can use, and hence reduces the polyphony. And because most or all of the 16 midi channels will often be used up, you might only be able to use one split. 

*Details: Turn on rechannelling by swiping past "ON" to "RCH". This will automatically set the split to use channel-per-note mode and a certain block of channels. Midi group 1 uses this block, and the other midi groups are sent to other blocks. Your DAW should send each block plus the main channel to a different synth with a different tuning table. The size of a channel block depends on the number of midi groups needed, which depends on how many midi notes your tuning uses. The number of groups is indicated by a vertical stack of 1, 2, 3 or 4 dots after the "RCH".*

For the left split,
* For up to 128 midi notes = 1 group, rechannelling doesn't happen ("RCH" acts the same as "ON")
* For 129-256 midi notes = 2 groups, the split will use channels 2-8. Midi in group 2 is sent to channels 9-15.
* For 257-384 midi notes = 3 groups, the split will use channels 2-6. Group 2 uses 7-11 and group 3 uses 12-16.
* For 385-512 midi notes = 4 groups, the split will use channels 1-4 (no Main channel). Group 2 uses 5-8, group 3 uses 9-12, and group 4 uses 13-16.

For the right split, 
* For 2 groups, the split will use channels 9-15. Midi in group 2 is sent to channels 2-8.
* For 3 groups, the split will use channels 11-15. Group 2 uses 6-10 and group 3 uses 1-5.
* For 4 groups, the split will use channels 13-16 (no Main channel). Group 2 uses 9-12, group 3 uses 5-8, and group 4 uses 1-4.

*If the left split only uses two groups, you can use the right split with channel 16. Furthermore, you can set the split to use fewer channels and the blocks will change to match. The channels should go up from 2 or down from 15. Or if there's no Main channel, up from 1 or down from 16. If you have N channels, the 2nd block is N channels higher (or lower for the right split) than the 1st block, the 3rd block is 2xN channels higher/lower, etc. For example, with 2 groups, setting the left split to channels 2-7 (6 channels) makes the 2nd block be 8-13, leaving 3 channels available for the right split.*


============================= Technical notes ==================================


KNOWN ISSUES:

* Immediately after updating microLinn to another version of microLinn, you must unplug the Linnstrument and plug it back in
* Uninstalling doesn't always preserve calibration and user settings
* Arpegiator is not yet microtonal
* Strumming is not yet microtonal
* Same/blink carry-over leaves extra lights on
* Condensing to a scale makes the red playedSame dots appear in the wrong places
* Edos above 41 rainbows are not ideal
* Default scales are incomplete

To find all changes to the code, search for "microlinn" or "playedBlink" or "patternChain" or "control the sequencer" or "teknico"

"Skip-fretting" is a column offset of 2 - each subsequent pad represents every other MIDI note, so note 0 2 4 6 8 ... instead of 0 1 2 3 4. The name is in reference to the microtonal [kite guitar](https://kiteguitar.com/), which obviously uses frets and not keys, but the Linnstrument's rows and columns work just like strings and frets.

Why would you want to skip half the notes? On the kite guitar, strings are 13 steps apart, which you can achieve on the Linnstrument by setting the Row Offset to a custom value of 13 (Global Settings -> Row Offset -> hold down "Octave" to get the hidden menu -> swipe right). Thirteen is an odd number, so that means that if the first row represented the _even_ scale degrees (0 2 4 6 8...), the next row will represent the _odd_ scale degrees (..13 15 17 19 21..)! Thus you actually have access to **all** the midi notes after all, with two neighboring rows filling each other's gaps. 

This is handy, as 41 notes per octave would not otherwise fit on a single row. But with half that, you get at least an octave per row on a 200-pad instrument. Luckily, you typically only want the odds or evens at the same time on the same string/row anyway, as explained on the kite guitar website.

# Kite Guitar Mode
- Enables skip fretting (column offset of 2)
- Enables Custom Row Offset
- Sets the Custom Row Offset to 13 steps (kite tuning)
- Sets `PLAYED` note lighting to `SAME` mode, so you can see which other pads represent the same note as you learn the kite layout (optional)

# Support
For support with the official firmware, email Roger at support@rogerlinndesign.com.
For support for this fork, inquire at the unofficial Linnstrument discord at https://discord.com/channels/1094879990367133706/1094890657170063400. Please do NOT inquire at the official KVR forum.

Many heartfelt thanks to Roger Linn for making the Linnstrument firmware open source!!!