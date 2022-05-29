# CHANGES / HISTORY of GPMFMetersGenerator

If you are updating from a previous version, pay particular attention to the changes with :loudspeaker: sign : they're highlighting an ascendant compatibility break.

## 3.xx

### 3.00

- Add **mkStory**
- Add story support in **GPMFMetersGenerator**
- Altitude and path stickers improved when using a story
- Add **GPS quality** sticker

## 2.xx

### 2.03

Detect automatically if a video is multi-parts<br>
:loudspeaker: Consequently : **Only one video can be provided**

Handle AVC encoded video as well (GX and GH videos supported)

### 2.02

Finalize C++ migration : redesign storage class

### 2.01

GPX files are back.

### 2.00

Code has been totally redesigned in C++ to ease implementation of future new features.
As of v2.0, GPX import is temporary disabled.

:loudspeaker: **:warning: CAUTION :warning: : Arguments changed and are not compatible with 1.xx.** Have a look on `GPMFMetersGenerator -h` for the new syntax.

## 1.xx
Changes not tracked
