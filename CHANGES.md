# CHANGES / HISTORY of GPMFMetersGenerator

If you are updating from a previous version, pay particular attention to the changes with :loudspeaker: sign : they're highlighting an ascendant compatibility break.

## 4.xx

### 4.03

Display seconds in trekking statistics

### 4.02

Handle commulative distance on videos only story

### 4.01

Correcte a bug on 1st video generation

### 4.00

Create stories from videos only.

## 3.xx

### 3.05

Some GPX editors (like https://gpx.studio/) don't keep the timezone and force 'Z' or ... nothing.
As fallback, try to read the timestamp without timezone.

### 3.04

- Add '**-A**' flag to force GPX data usage on altitude graphic

### 3.03

- mkStory can guess video inclusion based on timestamps

### 3.02

- Add **quality enforcement** flag to remove samples when DoP > 500

### 3.01

- Add **hicking figures** (Distance & Duration)
In a story, figures cover the full journey

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
