// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
//
// Simple example how to write a client.
// This sets two points. A red at (0,0); a blue dot at (5,5)
//
// By default, connects to the installation at Noisebridge. If using a
// different display (e.g. a local terminal display)
// pass the hostname as parameter:
//
//  ./simple-example localhost
//
// .. or set the environment variable FT_DISPLAY to not worry about it
//
//  export FT_DISPLAY=localhost
//  ./simple-example

#include "udp-flaschen-taschen.h"

#include <stdio.h>

#define DISPLAY_WIDTH  12
#define DISPLAY_HEIGHT 8

int main(int argc, char *argv[]) {
    const char *hostname = NULL;   // Will use default if not set otherwise.
    if (argc > 1) {
        hostname = argv[1];        // Hostname can be supplied as first arg
    }

    // Open socket and create our canvas.
    const int socket = OpenFlaschenTaschenSocket(hostname);
    UDPFlaschenTaschen canvas(socket, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    const Color black(0, 0, 0);
    const Color blue(0, 0, 255);
    const Color red(255, 0, 0);
    const Color green(0, 255, 0);
    const Color white(255, 255, 255);
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            canvas.SetPixel(x, y, black);
        }
    }

    canvas.SetPixel(0, 0, red);              // Sample with color variable.
    canvas.SetPixel(5, 5, blue); // or just use inline (here: blue).
    canvas.SetPixel(0, DISPLAY_HEIGHT - 1, green);
    canvas.SetPixel(DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, white);

    canvas.Send();                           // Send the framebuffer.
}
