# ArduFocuser
*An electronic telescope focuser*

## Motivation
In the past I have used Bahtinov masks and while they work and are tremendously helpful, there are limitations.
When using a single speed focuser it is still difficult to get perfect focus, even with the mask.

Since I do not have a many-thousand-dollar telescope there is slight chromatic abberation, but this doesn't immediately produce an issue since I use a monochrome camera and in one frame there is no abberation. If I want a color image (who wouldn't!) I need to capture frames with multiple filters of different wavelength. This means when I change the filter, for example from red to blue, the chromatic abberation presents itself because now the blue light focuses slightly differently, requiring me to adjust the focus again.

For visual use with multiple people, each person may need to adjust the focus to suit their eye. When 'zoomed' in on smaller objects, especially planets, the wobble when focusing makes the process a little difficult. With an electronic focuser and a remote, the user can look through the eyepiece and focus without touching the telescope.

Essentially, percise and automated focusing is the motivation behind the project, plus I get to have fun with robotic automation, how cool!

## How To Use

I have only written drivers for INDI, no ASCOM, as I use KStars/Ekos for telescope control.

When configuring an EKOS profile in KStars, the driver is `DIY > ArduFocuser`

To load the driver in INDIServer, the driver name is `indi_ardufocuser`

## Compiling

Compiling this driver first requires INDI to be installed to the local shared library.
The instructions on the INDI page should install it correctly.

Create a `build` folder in the source folder, move into it, then configure the project with cmake.

```
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ../
```

Build the ArduFocuser INDI driver.
```
make
```

Install the ArduFocuser INDI driver.

```
sudo make install
```

## Hardware

 * Arduino Leonardo / 'Pro Micro'
 * NEMA 17 bipolar stepper motor, 1.8deg/step, 26Ncm, 1.2A/phase
 * TMC2209 chopper drive controller on break-out board
 * 5.5mm x 2.1mm DC power jack
 * 20T 2mm GT pulley for motor
 * 60T 2mm GT pulley for focuser
 * 20mm length 2mm pitch GT2 belt

## INDI Technical Links

 * [INDI Driver Development](https://docs.indilib.org/drivers/)
 * [INDI Standard Properties](https://docs.indilib.org/drivers/standard-properties/)