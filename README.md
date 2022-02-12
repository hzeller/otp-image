Optical One-Time Pad XOR encoding of images
===========================================

This little tool converts a image into a laser-cuttable vector file to overlay.

Only supported image format currently: ppm (raw encoded, `P6`).

Given a key and an image, this program emits two pages of vector cuts as
PostScript file to stdout (There is a convenient Makefile rule to convert to
DXF).

_Note, this is a demo and just uses srand(); don't use as actual OTP_

Each optical bit is represented in a square area, in which either the left
or right half is cut out: [█&nbsp;&nbsp;]=0,  [&nbsp;&nbsp;█]=1

If overlayed, cut-outs that overlap let light shine through, while if they
are on opposing blocks, nothing can be seen. So only if both bits are the
same, the overlayed output will let light through, behaving as optical
`XNOR` operation.

### Build
```
make
```

### Use

Synopsis

```
Usage:
./otp-image <key> <ppm-file>
Reasonable image size is somewhere around <= 60x40px
```

Example

```bash
# Encode image with key "5ecr3t"
./otp-image "5ecr3t" someimage.ppm > my-encoded.ps

# Empty key, key-page is all zeroes, Pictures come out as-is,
./otp-image "" someimage.ppm > my-encoded.ps
```

You can edit the PostScript file (it allows for some parameters to be
tweaked). For ease of laser-cutting, convert to DXF:

```bash
# The following actually creates my-encoded-1.dxf and my-encoded-2.dxf
# for the both pages created
make my-encoded-1.dxf
```

### Example
Here an example of a 40x40 pixel image.
This is how these files look like; they have convenient holes at the top
for aligning. Each of the pages on their own look entirely random:

Key                | Img
-------------------|--------------------
![](./img/key.png) | ![](./img/image.png)


#### Each cutout
![](./img/each.jpg)

#### Overlay result
![](./img/result.jpg)
