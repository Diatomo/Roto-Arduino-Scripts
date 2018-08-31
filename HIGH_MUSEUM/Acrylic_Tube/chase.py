#!/usr/bin/env python

# Light each LED in sequence, and repeat.

import opc, time

numLEDs = 512
client = opc.Client('localhost:7890')

def signal():
    for i in range(numLEDs):
        pixels = [ (0,0,0) ] * numLEDs
        pixels[i] = (0, 255, 0)
        client.put_pixels(pixels)
        time.sleep(0.01)

signal()
