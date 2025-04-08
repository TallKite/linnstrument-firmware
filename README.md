/**************************************** MICROLINN ****************************************/

Unfinished, still a few more features to add, plus a few features don't quite work right yet. 
Beta testers needed!

The  Linnstrument is an amazing instrument, very feature-rich. If it's new to you, we strongly reccommend waiting to install microLinn until after you've explored all the standard features.

For experienced linnstrumentalists, microLinn makes exploring microtonality incredibly easy. Load up your usual MPE synths, set the notes per octave, and play! It's that simple!

-------- INSTALLATION -------- 

1) Go to https://www.rogerlinndesign.com/support/support-linnstrument-update-software and follow the "How to Check Your Software Version" instructions. If it's not 2.3.3, follow the "How to Update Your LinnStrument Software‍" instructions to update to 2.3.3.
2) Download the latest version of linnstrument-firmware.ino.bin from this github. Important: if on a mac, put the .bin file on your **desktop**. 
3) Follow the "How to Update Your LinnStrument Software‍" instructions, with one difference: after you download (and possibly unzip) the updater and before running it, put it in the same folder as the .bin file from step 2. Mac users: when you run the updater, if it asks for permission to read files from the desktop, say yes.
4) Check your OS version to confirm the update. It should be 234.072A. If not, reboot your computer and try again.
5) Important, read the next section about uninstalling!

-------- UPGRADING/UNINSTALLING -------- 

IMPORTANT: Later on when you update the OS to a different version, you'll be asked if you want to uninstall microLinn. Say no (the red button in the middle) if you're updating to a newer version of microLinn. This avoids needlessly deleting your microtonal user settings. Say yes (the blue button on the left) otherwise, e.g. if you're going back to an official (non-microtonal) version of the firmware. This does delete your microtonal user settings, which is necessary in order to avoid deleting all your other user settings plus your calibration data. If you're not sure, tap the green button on the right to exit the uninstall screen.

-------- NON-MICROTONAL FEATURES -------- 

COLUMN OFFSETS: Ranges from 1 (OFF) to 8. For negative offsets, use the hidden lefthanded setting in Global Settings column 1. The usual Linnstrument tuning is row offset +5 and column offset +1, or (+5 +1). The three most popular hexagonal-key layouts can each be translated to the Linnstrument's square keys in two ways:
* Wicki-Hayden layout translates to (+5 +2) or (+7 +2)
* Bosanquet or Janko layout translates to (-1 +2) or (+1 +2)
* Harmonic Table layout translates to (+4 +3) or (+7 +3)

The column offset can be set for each split independently. You can have one of these six layouts on the left for easy chord playing (probably with pitch bending off) and the usual layout on the right for easy melody playing, somewhat analogous to an accordion's layout. Accessed through the microLinn menu, see below.

A column offset of +2 can be used microtonally to emulate the "skip-fretting" of the [kite guitar](https://kiteguitar.com/) with 41 notes to the octave. See "MEMORIES" below.

Playing melodies with both pitch bending and column offsets can be tricky. *Details: A 1mm slide normally bends the pitch about 6¢. But an offset of +2 makes it 12¢, +3 makes it 18¢, etc. Thus to play in tune with a large column offset you may need both Pitch/X Quantize and Pitch/X Quantize Hold to be on. But if your column offset is +2, setting Quantize on and Quantize Hold off lets you play the skipped notes fairly accurately by sliding into the gap between pads. In fact, you can play 24edo quartertones on a normal 12edo Linnstrument fairly easily this way.*

*More details: (1) To set both column offsets at once, press both split buttons at once. If the offsets are different, doing so equates them. (2) Beware, if the column offset is 2 and the row offset is an even number, you lose half the notes and only get a whole-tone scale. In general, the column offset and the row offset should not have any common factors.*

PER-SPLIT ROW OFFSETS: Setting the row offset for a split overrides the Global Settings row offset for that split only. Unless OFF, it ranges from -25 to +25, plus No Overlap which appears as NOVR. A No Overlap split can be used to launch audio or midi clips while you play normally in the other split. Accessed through the microLinn menu, see below. Guitar tunings are not supported, but are still available globally.

OCTAVE TOGGLE: A new function for switches and footswitches, 8VE± flips back and forth between Octave Up and normal. This lets you switch octaves while playing using only one footswitch, instead of two for Octave Up and Octave Down. Access it by long-pressing TAP TEMPO and swiping.

DRUM PAD MODE: The note lights become 14 3x3 mega-pads that play the 14 drum sounds from the sequencer. Accessed through the microLinn menu, see below.

*Details: (1) Set PITCH/X on, otherwise a tap that hits two pads will send two notes. The drum sounds won't be accidentally pitch-bent because drum pad mode removes all pitch bends. (2) The midi mode should probably be One Channel. (3) The sequencer has different drum sounds for each split. If you're using both splits, set the split point in between the pads. (4) The two pad colors are the main/accent colors of the split, so you can adjust them if you want. Any custom light pattern you select will overlay the drum pads. So you can make the center of each pad a different color if you want. (5) If you use musical sounds instead of drum sounds, and enter the appropriate pitches into the sequencer, you can make a sort of marimba. (6) To create your own mega-pad layout, turn off Drum Pad mode, use locator CCs instead (see below) and edit one of the custom light patterns to match.*

DETUNING: Detune the entire Linnstrument up or down from A-440 to match a similarly detuned recording or instrument. No guarantee that detuning to A-432 will heal your chakras lol. Accessed through microLinn's anchor cents after setting the edo to 12, see below.

CHAINING SEQUENCES: hold the pad for sequence #2 and tap the pad for sequence #1. Now the two sequences are chained into a single sequence twice as long. This is indicated by sequence #1 being accented and sequence #2 blinking. You can chain #3 and #4 together as well. Or chain all 4 together. The lights for the entire chain will blink when it's playing. When making a chain, to start at the beginning of the chain not the end, hold the last sequence and tap the first one. 

You can select sequences and chains on the fly as the sequencer is playing. You can also chain and unchain on the fly. Straight/dotted/triplet/swing and quarter/8th/16th are still set individually for each sequence, so a chain can mix these together. To clear all chains in a split, tap the hidden switch immediately to the left of the 4 selector pads. Or just unplug the Linnstrument.

SEQUENCER PEDALS: When playing in one split and using the other split as a sequencer, it's no longer necessary to switch to the other split before using the following footpedals (or switches or midi NRPN messages): PLAY, PREV, NEXT and MUTE. 

*Details: Outside of a chain, the NEXT and PREV footswitches take you to the next/previous sequence as before. You can double-tap or triple-tap the NEXT and PREV footswitches to skip forward/backward multiple sequences. Thus triple-tapping NEXT is the same as single-tapping PREV, which means you only need one footswitches to go anywhere.*

*But from within a chain, NEXT and PREV operate relative to the upcoming sequence in the chain, not the current one. Thus pressing PREV repeats the current sequence and pressing NEXT goes forward two sequences, not one. One exception: from the rightmost sequence of a chain, NEXT exits the chain. (Otherwise you could never exit.)*

BLINKING MODE: Like the SAME mode, BLNK shows you other occurences of the currently played note. But instead of changing color, the other occurences blink. It's good for multi-colored displays like the custom note lights #2 (the one marked as A#) or certain microtonal displays. The BLNK option appears right after CELL and SAME. (Don't confuse BLNK for blinking with BLIN for blinders.)

SAME/BLINK CARRY OVER: If both splits are set to SAME or BLNK, playing in one split shows matching notes in the other split too.

MULTI-COLORED NOTE LIGHTS: Each of the 12 (or more!) notes can be any color. Transposable. Access it through the microLinn menu after setting the edo to 12, see below.

SHOW A CUSTOM LIGHT PATTERN IN ONE SPLIT ONLY: Choose any of the 3 patterns (the scales marked A, A# and B) and it will replace the note lights. If you use one split as a clip launcher, you can color-code your clips. See also Locator CCs below. BTW this feature fixes a bug in which the custom light pattern would cover up a split set to CC faders or strum.

LOCATOR CCs: A locator CC message can be sent immediately before every note-on, indicating the row and column. Your DAW can assign a specific function to a specific pad, e.g. upper lefthand corner = all sound off. Or use this feature plus a custom light pattern to create large drum pads for more easily playing drum midi. Or create a third split, perhaps a column of on/off buttons, or a vertical fader or two.

*Details: One type of CC is sent for note-ons in cols 1-16 and another type of CC is sent for note-ons in cols 17-25. For cols 1-16, the data value = (row - 1) + 8 * (col - 1). For cols 17-25, the data value = (row - 1) + 8 * (col - 17). The top row is row 1 and the leftmost column is column 1. Choose the two CC types through the microLinn menu, see below.*

*There's not much point in using locator CCs in Channel Per Row mode, since the channel serves to locate the note. In One Channel mode the CC can locate a note-on but if the same note is played twice on two separate pads, it can't locate the XYZ data.*

*If your DAW isn't programmable, download LinnstrumentLocatorCCs.jsfx from this github. It defines a rectangular region on the Linnstrument, within which it can either transpose each note to a specific note (good for drum pads) or transform it into a CC message in a variety of ways. It can also filter out other midi either inside or outside of this region. It runs natively in Reaper and can run in any windows DAW using ReaJS, a free jsfx-to-VST wrapper. (Hopefully someone can duplicate this effect in Max 4 Live and other platforms.) Thanks to KVR forum member vorp40 for the locator CC idea!*

-------- LIKELY ADDITIONAL NON-MICROTONAL FEATURES (no promises) -------- 

DIM MODE: The Low Power pad (col 15 in Global Settings) dims the display but also increases the latency. Long-press the Low Power pad to dim the display with no latency increase, good for tired eyes.

DOUBLE VOLUME FADERS: On the Volume screen, press both split pads at once. There will be two horizontal faders, one for each split. You can use a splitter on your computer's stereo headphones output and send one side to your amp and the other to an earbud that you wear as you play as an in-ear monitor. You can control the volume of both from the Volume screen. Good for noisy gigs, good for chaotic jam sessions where you need to discretely find the key or the chords. *Details: In your DAW, send the synth's output to two tracks, each hard-panned to opposite sides. Each track receives the Linnstrument's midi from one of the 2 main channels (usually 1 and 16). Each track has a gain effect which you midi-learn to the volume CC. Now one volume fader will control your amp and the other will control your earbud.*

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

The edo (the notes per octave) ranges from 5edo to 55edo, plus "OFF" which makes the Linnstrument run normally. Change the edo by swiping. Once microLinn is on (i.e. once an edo is chosen), you can cycle thru the edos by setting a switch or footswitch to EDO+ or EDO- (long-press TAP TEMPO and swipe). Changing the edo automatically adjusts all column offsets and row offsets so that their size in cents stays roughly the same. So your bosanquet keyboard layout remains bosanquet, and your standard guitar tuning (F#BEADGBE) remains standard.

The anchor pad is a specific pad that doesn't change pitch when you change the edo. *Details: The anchor pad chooser displays the row and column of the current anchor pad. For example, "R4C11" (or "4 11" on the Linnstrument 128) means row 4 (from the top) and column 11. Tap the blue "R4C11" anywhere and you'll see the normal display with the anchor pad blinking. Tap any pad to set it as the new anchor pad. This pad beomes the new tonic, and all the note lights will shift accordingly.*

Changing the anchor note transposes by 12edo semitones. Changing the anchor cents detunes the entire Linnstrument. Like the guitar tuning screen, a midi note is sent when you change either one. 

Besides transposing via the anchor note, each split can be transposed by edosteps via the Octave/Transpose screen. *Details: When the notes per octave is greater than 12, the screen shows two extra rows for transposing by edosteps. The 2nd and 3rd rows now transpose not by semitones but by major 2nds (since most edos have several different semitones). A major 2nd is defined as the interval between the 4th and the 5th, e.g. 3 edosteps for 15edo but only 2 edosteps for 16edo.*

It's possible to stretch the octave or even create non-octave scales like Bohlen-Pierce. *Details: The octave is really the equave, the interval of equivalence. It's defined as twelve 12edo semitones plus zero cents. Redefine it by swiping on Equave Semitones (1 to 48) and/or Equave Cents (-50 to 50).*

MicroLinn can be set to 12edo. What advantages does microLinn's 12edo have over the standard, non-microLinn 12edo? It can be stretched and/or detuned, plus you can have multi-colored note lights.

SUGGESTIONS FOR EXPLORING EDOS:

* The first few edos are pretty strange. You may want to start with 15, 17, 19 or 22.
* Be sure to try the Bosanquet layout for 31edo and the Kite guitar layout for 41edo.
* The full rainbow scale can be overwhelming. Try setting the played mode to BLNK.
* To avoid a touched pad turning red (or whatever) and obscuring the pad's usual color, set the played color to blank.
* Once you know an edo well, you'll probably want to switch to the fretboard dots display.

GUITAR TUNING

MicroLinn uses a completely independent guitar tuning than the usual one. MicroLinn's guitar tuning screen doesn't adjust the pitch of each "string". Instead it sets 7 independent row offsets. 

*Details: The "anchor string" is the row that the anchor pad is on. Its pitch is determined solely by the anchor pad, note and cents. On the far left, the anchor string is red, or orange if lit up. The others are green, or cyan if lit up. Tap any button on the far left to select a string. The button lights up and sounds that open string. Assuming it's not the anchor string, one of the neighboring strings, whichever one is closest to the anchor, also lights up. You won't see a note name with an octave number. Instead you'll see a row offset as a number, which can be negative. This is the row offset between the two lit-up strings. Swipe right or left on it as before to increase or decrease it. (Selecting the anchor string doesn't show a row offset.)*

*Changing one row offset doesn't affect the other six row offsets. Thus increasing any row offset above the anchor string sharpens the current string and all strings above it. And increasing any row offset below the anchor string _flattens_ the current string and all strings below it. (Of the two lit-up buttons, the current string is always the one furthest from the anchor string.) To summarize, you're _seeing_ the offset between the two lit-up strings, but _hearing_ the pitch of the outermost lit-up string.*

A guitar tuning is a standard tuning if the intervals between open strings are all 4ths, except for that one major 3rd between the 2nd and 3rd rows. The exact notes don't matter, just the intervals. If you switch edos while in a standard tuning, you'll stay in a standard tuning. 

*Details: When in a standard tuning, on the Global Settings screen the GUITAR pad is dark blue, otherwise it's bright blue. The edo's 4th is defined as its closest approximation to 4/3. The edo's major 3rd is defined as two octaves minus four 4ths. Thus 22edo's major 3rd is 8\22 = 436¢ not 7\22 = 382¢. There's two possible 4ths for 13edo (5\13 and 6\13) and 18edo (7\18 and 8\18). Either 4th keeps the GUITAR pad dark blue.*

In Global Settings, when you long-press the OCTAVE pad, the "-GUI" option for reversed guitar tuning is not available when microLinn is on. To get this reversed tuning, set the guitar tuning manually.

NOTE LIGHTS

The 9 scales in Global Settings columns 2-4 are now microtonal and change for each edo. You can still select a scale using columns 2-4, but you can no longer edit a scale there, because for larger edos there are too many notes to fit into the 3x4 box. As a result, when microLinn is on, the VIEW MAIN and VIEW ACCENT buttons do not work, and the SCALE SELECT button is always on. To edit a scale and its colors, instead go to the main microLinn menu and go to the note lights screen. Shortcut: you can long-press the scale's pad in cols 2-4 in the Global Settings screen to go directly to that scale.

*Details: The note lights screen has 7 scale buttons plus the rainbow editor, the dots selector and the yellow rainbow-enabler button. Excluding the rainbow enabler, there are 9 buttons, corresponding to the 9 scales in cols 2-4 in the Global Settings screen. Tap any of these 9 buttons to select it. Tap any already-selected button to backtrack to the previous button. There are 7 rows of colored lights on the screen, which top to bottom are for unisons, 2nds, 3rds, 4ths, 5ths, 6ths and 7ths. Tap a note in a scale to toggle it on or off. Like the guitar tuning screen, a midi note is sent when you tap. The 8th scale is the rainbow editor, in which all notes are always on. Tap a note to cycle it thru the rainbow. The 9th scale (guitar-like fretboard dots) isn't really a scale. It's a full-screen display like the custom light patterns. Tapping the dots selector makes the dots appear in blue mid-screen. Tapping the dots lets you toggle them on or off. To change the color of the dots, after editing go to Per-Split Settings and in column 11 change the main color. Long-press the scale buttons or the rainbow editor button or the dots selector button to reset the note lights to the default. Tap the yellow rainbow enabler button to turn off the rainbow and limit the note lights to the usual two colors.*

The 3 custom light patterns are totally separate from all this and are still available for use! This web browser lets you easily edit them: https://forrcaho.github.io/linnstrument_colorizer/

Default scales: of the 8 scales, the 1st and 2nd scales are 5-limit major and 5-limit minor. Scales 3-6 are blank but for the tonic, so that you can create your own scales on the note lights screen. (For the smaller, weirder edos, scales 1-2 are also blank.) The 7th scale is a partial rainbow, and the 8th scale is always the full rainbow.

The colors use the rainbow metaphor, red-yellow-green-blue = sharp-to-flat = supermajor-to-subminor. There's a rainbow of 2nds, a rainbow of 3rds, etc.
* white = 12-edo-ish = 3-limit
* yellow / green = downmajor / upminor =  5-over / 5-under (examples: 5/4 and 5/3 are 5-over, 6/5 and 16/15 are 5-under)
* blue / red = downminor / upmajor = 7-over / 7-under
* purple = neutral = 11-over or 11-under or 13-over or 13-under
* pink is reserved for the exact half-octave of 600c, 12-edo-ish but not quite 3-limit, "off-white"
* cyan / orange = a catch-all pair, e.g. 41edo 7/5 and 10/7, cyan is also for "outside" notes aka interordinals e.g. 24edo
* orange is used in 55edo for upmid and downmid notes

The dot patterns tend to follow the conventional m3 P4 P5 M6 P8 guitar fret markers. Some edos add M2 and m7. Edos above 24 approximate 12edo, in other words there are dots about every 100 cents. 41edo is an exception. It has kites like a Kite guitar.

MEMORIES

All of microLinn's settings are remembered by the 6 memories. If you load a memory that has microLinn turned OFF, the microtonal data will not be altered. And if the column offsets and per-split row offsets are also OFF, they will not be altered either (because the edo often implies certain offsets). This lets you use certain memories to configure your synth-related settings only. Load such memories *after* loading a microtonal memory.

The memory on the bottom row is an exception to this. It *will* alter microtonal data even if it has microLinn turned OFF. You can load this memory to quickly return to 12edo. 

The 2nd memory from the bottom emulates the [Kite guitar](https://KiteGuitar.com/). It's 41edo with a row/col offset of (+13 +2), with an alternating-3rds guitar tuning and with MPE midi settings. This layout is both very playable and very well-tuned.

BEYOND EDOS: TUNING TABLE MODE 

Tuning table mode is meant for non-edo tunings such as just intonation or rank-2 temperaments. Each note of an N-note scale is sharp or flat from N-edo, thus the pad's pitch will be different slid up to vs played directly. However this is often only a comma difference even on long slides. You can hide the comma by turning on Quantize Hold. Otherwise you have to bend by ear, just like on a guitar.

Once an edo is selected, the Linnstrument outputs standard midi notes with tuning bends, and several edo notes will share the same midi note. But a split that's set to Tuning Table mode outputs edosteps instead. The lower left pad (or the lower right pad if the split is set to lefthanded) is midi note 0. The midi notes for the other pads increase from there according to the split's column and row offsets. Thus each edostep is a unique midi note. Certain synths need this format to play edos. But the main reason to use Tuninng Table mode is to retune each edostep individually to something else, by loading a tuning table in the form of a scala file into your synth (or running microtonal software such as alt-tuner).

The midi note number each pad outputs is determined entirely by the column and row offsets. The edo you select in theory doesn't matter. However when the edo is set correctly, transposing works better and and the note lights make more sense.

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

*If the left split only uses two groups, you can use the right split with channel 16. Also, you can set the split to use fewer channels and the blocks will change to match. The channels should go up from 2 or down from 15. Or if there's no Main channel, up from 1 or down from 16. If you have N channels, the 2nd block is N channels higher (or lower for the right split) than the 1st block, the 3rd block is 2xN channels higher/lower, etc. For example, with 2 groups, setting the left split to channels 2-7 (6 channels) makes the 2nd block be 8-13, leaving 3 channels available for the right split.*

============================= technical notes ==================================

To find all changes to the code, search for "microlinn" or "patternChain" or "playedBlink" or "control the sequencer"

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
For support for this fork, open a new github issue, or inquire at the unofficial Linnstrument discord.
