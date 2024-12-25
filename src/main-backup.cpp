// #include <Arduino.h>
// #include <Adafruit_NeoPixel.h>
// #ifdef __AVR__
// #include <avr/power.h>
// #endif

// #define PIN 3

// // Function declarations
// void testXPlanes();
// void testYPlanes();
// void testZPlanes();
// void illuminatePlane(char plane, int index, uint32_t color);
// struct Point3D;
// Point3D indexToXYZ(int index);
// void testMapping();
// uint32_t gradientPattern(Point3D p);
// uint32_t rainbowPattern(Point3D p, int offset);
// uint32_t pulsePattern(Point3D p, int time);
// uint32_t Wheel(byte WheelPos);
// void setup();
// void loop();
// #define CUBE_SIZE 8
// #define COUNT (CUBE_SIZE * CUBE_SIZE * 4) // 4 walls of 8x8

// Adafruit_NeoPixel strip = Adafruit_NeoPixel(COUNT, PIN, NEO_GRB + NEO_KHZ800);

// // Structure to hold 3D coordinates
// struct Point3D
// {
//   int x;
//   int y;
//   int z;
// };

// // Function to map pixel index to 3D coordinates
// Point3D indexToXYZ(int index)
// {
//   Point3D p;
//   int wall = index / (CUBE_SIZE * CUBE_SIZE); // Which wall (0-3)
//   int pos_in_wall = index % (CUBE_SIZE * CUBE_SIZE);
//   int row = pos_in_wall / CUBE_SIZE;
//   int col = pos_in_wall % CUBE_SIZE;

//   // Invert column because first pixel is on the right
//   col = CUBE_SIZE - 1 - col;

//   switch (wall)
//   {
//   case 0: // North wall
//     p.x = col;
//     p.y = CUBE_SIZE - 1;
//     p.z = row;
//     break;
//   case 1: // East wall
//     p.x = 0;
//     p.y = col;
//     p.z = row;
//     break;
//   case 2: // South wall
//     p.x = CUBE_SIZE - 1 - col;
//     p.y = 0;
//     p.z = row;
//     break;
//   case 3: // West wall
//     p.x = CUBE_SIZE - 1;
//     p.y = CUBE_SIZE - 1 - col;
//     p.z = row;
//     break;
//   }
//   return p;
// }

// // Function to test the mapping
// void testMapping()
// {
//   // Test pattern: light up one wall at a time
//   for (int wall = 0; wall < 4; wall++)
//   {
//     strip.clear();

//     // Light up current wall in different colors
//     for (int i = wall * CUBE_SIZE * CUBE_SIZE;
//          i < (wall + 1) * CUBE_SIZE * CUBE_SIZE; i++)
//     {
//       Point3D p = indexToXYZ(i);

//       // Different color for each wall
//       uint32_t color;
//       switch (wall)
//       {
//       case 0:
//         color = strip.Color(255, 0, 0);
//         break; // North - Red
//       case 1:
//         color = strip.Color(0, 255, 0);
//         break; // East - Green
//       case 2:
//         color = strip.Color(0, 0, 255);
//         break; // South - Blue
//       case 3:
//         color = strip.Color(255, 255, 0);
//         break; // West - Yellow
//       }

//       strip.setPixelColor(i, color);
//     }

//     strip.show();
//     delay(2000); // Show each wall for 2 seconds
//   }
// }

// // Color pattern functions
// uint32_t gradientPattern(Point3D p)
// {
//   // Creates a gradient based on height
//   int brightness = map(p.y, 0, CUBE_SIZE - 1, 20, 255);
//   return strip.Color(brightness, 0, brightness);
// }

// uint32_t rainbowPattern(Point3D p, int offset)
// {
//   // Creates a rainbow pattern that can be animated
//   int hue = (p.x + p.y + p.z + offset) * 32;
//   return Wheel(hue & 255);
// }

// uint32_t pulsePattern(Point3D p, int time)
// {
//   // Creates a pulsing pattern from the center
//   float center_x = CUBE_SIZE / 2.0;
//   float center_z = CUBE_SIZE / 2.0;
//   float dx = p.x - center_x;
//   float dz = p.z - center_z;
//   float distance = sqrt(dx * dx + dz * dz);

//   float wave = sin(distance * 0.5 + time * 0.1) * 127 + 128;
//   return strip.Color(wave, 0, wave);
// }

// // Function to illuminate a specific plane
// void illuminatePlane(char plane, int index, uint32_t color)
// {
//   for (uint16_t i = 0; i < strip.numPixels(); i++)
//   {
//     Point3D p = indexToXYZ(i);
//     bool shouldLight = false;

//     switch (plane)
//     {
//     case 'x':
//       shouldLight = (p.x == index);
//       break;
//     case 'y':
//       shouldLight = (p.y == index);
//       break;
//     case 'z':
//       shouldLight = (p.z == index);
//       break;
//     }

//     if (shouldLight)
//     {
//       strip.setPixelColor(i, color);
//     }
//   }
// }

// // Function to test X planes
// void testXPlanes()
// {
//   for (int x = 0; x < CUBE_SIZE; x++)
//   {
//     strip.clear();
//     illuminatePlane('x', x, strip.Color(255, 0, 0));
//     strip.show();
//     delay(500);
//   }
// }

// // Function to test Y planes
// void testYPlanes()
// {
//   for (int y = 0; y < CUBE_SIZE; y++)
//   {
//     strip.clear();
//     illuminatePlane('y', y, strip.Color(0, 255, 0));
//     strip.show();
//     delay(500);
//   }
// }

// // Function to test Z planes
// void testZPlanes()
// {
//   for (int z = 0; z < CUBE_SIZE; z++)
//   {
//     strip.clear();
//     illuminatePlane('z', z, strip.Color(0, 0, 255));
//     strip.show();
//     delay(500);
//   }
// }

// void setup()
// {
//   strip.begin();
//   strip.setBrightness(5); // Keep brightness low
//   strip.show();

//   // testMapping();
// }

// void loop()
// {
//   testXPlanes();
//   testYPlanes();
//   testZPlanes();
//   delay(1000);

//   // static int time = 0;

//   // // Animate different patterns
//   // for (uint16_t i = 0; i < strip.numPixels(); i++)
//   // {
//   //   Point3D p = indexToXYZ(i);

//   //   // Uncomment one of these patterns:
//   //   // strip.setPixelColor(i, gradientPattern(p));
//   //   // strip.setPixelColor(i, rainbowPattern(p, time));
//   //   strip.setPixelColor(i, pulsePattern(p, time));
//   // }

//   // strip.show();
//   // delay(50);
//   // time++;
// }

// // Helper function from original code
// uint32_t Wheel(byte WheelPos)
// {
//   WheelPos = 255 - WheelPos;
//   if (WheelPos < 85)
//   {
//     return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//   }
//   if (WheelPos < 170)
//   {
//     WheelPos -= 85;
//     return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//   }
//   WheelPos -= 170;
//   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
// }