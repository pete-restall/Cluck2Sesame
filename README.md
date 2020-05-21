# Cluck<sup>3</sup>Sesame - Yet Another Chicken Coop Door Opener (Revision II)

## What is it ?
It opens and closes the door of my wife's chicken coop based on Sunrise and
Sunset times so that she doesn't have to.  This is a refinement of the
[Cluck<sup>2</sup>Sesame](https://github.com/pete-restall/Cluck2Sesame/)
project, building on lessons learned from the original attempt.  Notably:

 - The parts are not based on what I have lying around, but rather on parts
   selected for the job or known to do the job.  This has also simplified the
   design somewhat - more pins on the PIC mean the shift register is no longer
   required, for example.  The newer PIC is smaller, integrates more
   peripherals and comes with more flash; the original project ran out of
   flash before all features could be implemented.

 - The shortcomings in the end-of-travel detection, namely measuring motor
   current for stall torque, hastened battery depletion.  A rotary-encoder
   based approach will be used instead, which will hopefully lengthen battery
   life.

 - A solar charger will be included as replacing the battery, fairly regularly
   as it turned out, meant unscrewing the enclosure.

 - The motor will be a 6V model rather than a 12V model and run directly from 
   the battery rather than off an SMPS.  No SMPS means a smaller BOM and no
   power wasted by the high motor currents.  This should extend battery life
   even further, at the expense of varying speed.  But that's not an issue.

 - The PCB will be made in China, and the board house charges the same price
   regardless of whether the PCB is the size of the enclosure or the size of
   the LCD module.  This means that the aluminium backplate can be replaced
   with a PCB, providing more flexibility and space for layout and mounting
   while reducing the cost and time of the build.

That said, some of the parts from the original have been incorporated into
the newer design since they are known to work and are already stocked in my
parts bins.

## Why 'Cluck<sup>3</sup>Sesame' ?
Because 'Cluck Cluck Sesame' is obviously what the chickens would say if
they needed to open a door but
[that name's already taken](https://github.com/pete-restall/Cluck2Sesame/).
The only completely logical alternative is thus 'Cluck Cluck Cluck Sesame',
conveying the increment in the version number while still being comprehensible
and pronouncible for garden-based fowl.

## The Hardware and Firmware
The hardware and firmware are all mine.  The board is based around a
[PIC16F15356](doc/datasheets/mcu/PIC16F15356.pdf).  It's a beefier evolution
of what was used last time that has allowed considerable simplification of the
board through more integration and a higher pin count.

## Pictures
None yet.  But there will be.

## Build
[![Build Status](https://travis-ci.org/pete-restall/Cluck3Sesame.svg?branch=master)](https://travis-ci.org/pete-restall/Cluck3Sesame)
