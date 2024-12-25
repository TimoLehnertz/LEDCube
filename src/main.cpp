#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Function declarations
void testXPlanes();
void testYPlanes();
void testZPlanes();
void illuminatePlane(char plane, int index, uint32_t color);
struct Point3D;
Point3D indexToXYZ(int index);
void testMapping();
uint32_t gradientPattern(Point3D p);
uint32_t rainbowPattern(Point3D p, int offset);
uint32_t pulsePattern(Point3D p, int time);
uint32_t polarColorPattern(Point3D p, float rotation);
uint32_t spherePattern(Point3D p);
void initSpheres();
void updateSpheres();
uint32_t wavePattern(Point3D p, float time);
uint32_t creativePattern(Point3D p, float time);
uint32_t Wheel(byte WheelPos);
void setup();
void loop();

#define PIN 3
#define CUBE_SIZE 8
#define COUNT (CUBE_SIZE * CUBE_SIZE * 4) // 4 walls of 8x8

#define TRANSITION_DURATION 100 // Duration of transition in time units (10 seconds)
#define PATTERN_DURATION 100    // Duration of each pattern in time units (10 seconds)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(COUNT, PIN, NEO_GRB + NEO_KHZ800);

// Structure to hold 3D coordinates
struct Point3D
{
  int x;
  int y;
  int z;
};

// Structure for spheres
struct Sphere
{
  float x;
  float y;
  float z;
  uint32_t color;
  float velocity;
  bool active;
};

#define MAX_SPHERES 5
Sphere spheres[MAX_SPHERES];

// Function to map pixel index to 3D coordinates
Point3D indexToXYZ(int index)
{
  Point3D p;
  int wall = index / (CUBE_SIZE * CUBE_SIZE); // Which wall (0-3)
  int pos_in_wall = index % (CUBE_SIZE * CUBE_SIZE);
  int row = pos_in_wall / CUBE_SIZE;
  int col = pos_in_wall % CUBE_SIZE;

  // Invert column because first pixel is on the right
  col = CUBE_SIZE - 1 - col;

  switch (wall)
  {
  case 0: // North wall
    p.x = col;
    p.y = CUBE_SIZE - 1;
    p.z = row;
    break;
  case 1: // East wall
    p.x = 0;
    p.y = col;
    p.z = row;
    break;
  case 2: // South wall
    p.x = CUBE_SIZE - 1 - col;
    p.y = 0;
    p.z = row;
    break;
  case 3: // West wall
    p.x = CUBE_SIZE - 1;
    p.y = CUBE_SIZE - 1 - col;
    p.z = row;
    break;
  }
  return p;
}

// Function to illuminate a specific plane
void illuminatePlane(char plane, int index, uint32_t color)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    Point3D p = indexToXYZ(i);
    bool shouldLight = false;

    switch (plane)
    {
    case 'x':
      shouldLight = (p.x == index);
      break;
    case 'y':
      shouldLight = (p.y == index);
      break;
    case 'z':
      shouldLight = (p.z == index);
      break;
    }

    if (shouldLight)
    {
      strip.setPixelColor(i, color);
    }
  }
}

// Function to test X planes
void testXPlanes()
{
  for (int x = 0; x < CUBE_SIZE; x++)
  {
    strip.clear();
    illuminatePlane('x', x, strip.Color(255, 0, 0));
    strip.show();
    delay(50);
  }
}

// Function to test Y planes
void testYPlanes()
{
  for (int y = 0; y < CUBE_SIZE; y++)
  {
    strip.clear();
    illuminatePlane('y', y, strip.Color(0, 255, 0));
    strip.show();
    delay(50);
  }
}

// Function to test Z planes
void testZPlanes()
{
  for (int z = 0; z < CUBE_SIZE; z++)
  {
    strip.clear();
    illuminatePlane('z', z, strip.Color(0, 0, 255));
    strip.show();
    delay(50);
  }
}

// Function to test the mapping
void testMapping()
{
  // Test pattern: light up one wall at a time
  for (int wall = 0; wall < 4; wall++)
  {
    strip.clear();

    // Light up current wall in different colors
    for (int i = wall * CUBE_SIZE * CUBE_SIZE;
         i < (wall + 1) * CUBE_SIZE * CUBE_SIZE; i++)
    {
      Point3D p = indexToXYZ(i);

      // Different color for each wall
      uint32_t color;
      switch (wall)
      {
      case 0:
        color = strip.Color(255, 0, 0);
        break; // North - Red
      case 1:
        color = strip.Color(0, 255, 0);
        break; // East - Green
      case 2:
        color = strip.Color(0, 0, 255);
        break; // South - Blue
      case 3:
        color = strip.Color(255, 255, 0);
        break; // West - Yellow
      }

      strip.setPixelColor(i, color);
    }

    strip.show();
    delay(2000); // Show each wall for 2 seconds
  }
}

// Pattern 1: Polar color wheel
uint32_t polarColorPattern(Point3D p, float rotation)
{
  // Calculate angle between x and y coordinates
  float angle = atan2(p.y - CUBE_SIZE / 2.0, p.x - CUBE_SIZE / 2.0);
  // Convert angle to positive value (0 to 2π)
  if (angle < 0)
    angle += 2 * PI;
  // Map angle to color wheel (0-255)
  byte wheelPos = (byte)((angle / (2 * PI) * 255) + rotation) % 255;
  return Wheel(wheelPos);
}

float sphereVel = 0.1;

// Pattern 2: Falling spheres
void initSpheres()
{
  for (int i = 0; i < MAX_SPHERES; i++)
  {
    spheres[i].x = random(CUBE_SIZE);
    spheres[i].y = random(CUBE_SIZE);
    spheres[i].z = CUBE_SIZE + random(5); // Start above cube
    spheres[i].color = Wheel(random(255));
    spheres[i].velocity = sphereVel;
    spheres[i].active = true;
  }
}

void updateSpheres()
{
  const float gravity = 0.1;

  for (int i = 0; i < MAX_SPHERES; i++)
  {
    if (spheres[i].active)
    {
      // Update position
      // spheres[i].velocity += gravity;
      spheres[i].z -= spheres[i].velocity;

      // Check if sphere hit bottom
      if (spheres[i].z < -2)
      {
        // Respawn sphere
        spheres[i].x = random(CUBE_SIZE);
        spheres[i].y = random(CUBE_SIZE);
        spheres[i].z = CUBE_SIZE + 5;
        spheres[i].color = Wheel(random(255));
        spheres[i].velocity = sphereVel;
      }
    }
  }
}

uint32_t spherePattern(Point3D p)
{
  float influence = 0;
  uint32_t finalColor[3] = {0, 0, 0};

  for (int i = 0; i < MAX_SPHERES; i++)
  {
    if (spheres[i].active)
    {
      float dx = (float)p.x - spheres[i].x;
      float dy = (float)p.y - spheres[i].y;
      float dz = (float)p.z - spheres[i].z;
      float distance = sqrt(dx * dx + dy * dy + dz * dz);

      if (distance < 4)
      { // Influence radius
        float strength = 1.0 - (distance / 4.0);
        influence += strength;

        // Extract RGB components
        uint32_t color = spheres[i].color;
        finalColor[0] += ((color >> 16) & 0xFF) * strength;
        finalColor[1] += ((color >> 8) & 0xFF) * strength;
        finalColor[2] += (color & 0xFF) * strength;
      }
    }
  }

  if (influence > 0)
  {
    return strip.Color(
        min(255, (int)(finalColor[0] / influence)),
        min(255, (int)(finalColor[1] / influence)),
        min(255, (int)(finalColor[2] / influence)));
  }
  return 0;
}

uint32_t wavePattern(Point3D p, float time)
{
  float waveX = sin(time * 1) * (CUBE_SIZE - 1); // Wave position
  float height = sin((p.x + p.y + time * 1) * 0.5) * 2 + CUBE_SIZE / 2;

  // Calculate distance from wave height
  float distanceFromWave = p.z - height + 3;

  // Create a soft transition over 2 units
  float transitionZone = 3.0;
  float brightness = 1.0 - (distanceFromWave / transitionZone);
  brightness = constrain(brightness, 0.0, 1.0);

  // Smooth out the transition using a sine curve
  brightness = sin(brightness * PI / 2);

  // Apply brightness to blue color
  uint8_t blueValue = brightness * 255;
  return strip.Color(0, 0, blueValue);
}

// galaxySpiralPattern
uint32_t creativePattern(Point3D p, float time)
{
  // Center of the cube
  float centerX = CUBE_SIZE / 2.0;
  float centerY = CUBE_SIZE / 2.0;
  float centerZ = CUBE_SIZE / 2.0;

  // Calculate distance from center in XY plane
  float dx = p.x - centerX;
  float dy = p.y - centerY;
  float dz = p.z - centerZ;

  // Calculate angle in XY plane
  float angle = atan2(dy, dx);
  if (angle < 0)
    angle += 2 * PI;

  // Calculate radius from center
  float radius = sqrt(dx * dx + dy * dy);

  // Create spiral arms
  float numArms = 2;           // Number of spiral arms
  float armWidth = 0.8;        // Width of each arm
  float spiralTightness = 2.0; // How tightly wound the spiral is

  // Calculate spiral position
  float spiralAngle = angle + (radius * spiralTightness) + time * 0.1;
  float spiralValue = fmod(spiralAngle * numArms / (2 * PI), 1.0);

  // Calculate vertical fade
  float verticalFade = 1.0 - abs(dz) / (CUBE_SIZE / 2.0);
  verticalFade = max(0.0, verticalFade);

  // Check if point is within an arm
  if (abs(spiralValue - 0.5) < armWidth * (1.0 - radius / CUBE_SIZE))
  {
    // Calculate color based on radius and angle
    byte hue = (byte)((angle / (2 * PI) * 128) + time * 5) % 255;
    uint32_t armColor = Wheel(hue);

    // Apply distance and height fade
    float intensity = (1.0 - radius / CUBE_SIZE) * verticalFade;
    intensity = pow(intensity, 0.5); // Adjust brightness curve

    // Extract and modify RGB components
    uint8_t r = ((armColor >> 16) & 0xFF) * intensity;
    uint8_t g = ((armColor >> 8) & 0xFF) * intensity;
    uint8_t b = (armColor & 0xFF) * intensity;

    // Add sparkle effect
    if (random(100) < 2)
    {                                           // 2% chance of sparkle
      float sparkle = 0.5 + random(50) / 100.0; // Random boost between 50-100%
      r = min(255, (int)(r * sparkle));
      g = min(255, (int)(g * sparkle));
      b = min(255, (int)(b * sparkle));
    }

    return strip.Color(r, g, b);
  }

  // Add subtle background stars
  if (random(100) < 1)
  {                                          // 1% chance of background star
    uint8_t starBrightness = random(40, 80); // Dim background stars
    return strip.Color(starBrightness, starBrightness, starBrightness);
  }

  return 0;
}

// Helper function for color transitions
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void setup()
{
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(10); // Keep brightness low
  strip.show();

  // Run plane tests first
  // testXPlanes(); // Red planes
  // testYPlanes(); // Green planes
  // testZPlanes(); // Blue planes

  initSpheres(); // Initialize spheres for later patterns
}

// Helper function to blend between two colors based on a transition factor (0.0 to 1.0)
uint32_t blendColors(uint32_t color1, uint32_t color2, float factor)
{
  uint8_t r1 = (color1 >> 16) & 0xFF;
  uint8_t g1 = (color1 >> 8) & 0xFF;
  uint8_t b1 = color1 & 0xFF;

  uint8_t r2 = (color2 >> 16) & 0xFF;
  uint8_t g2 = (color2 >> 8) & 0xFF;
  uint8_t b2 = color2 & 0xFF;

  uint8_t r = r1 + (r2 - r1) * factor;
  uint8_t g = g1 + (g2 - g1) * factor;
  uint8_t b = b1 + (b2 - b1) * factor;

  return strip.Color(r, g, b);
}

// Helper function to get pattern color with transition
uint32_t getPatternColor(Point3D p, int patternIndex, float time)
{
  switch (patternIndex)
  {
  case 0:
    return polarColorPattern(p, time * 30);
  case 1:
    return spherePattern(p);
  case 2:
    return wavePattern(p, time);
  case 3:
    return creativePattern(p, time);
  case 4:
  {
    uint32_t c1 = polarColorPattern(p, time * 0.02);
    uint32_t c2 = spherePattern(p);
    uint32_t c3 = wavePattern(p, time);
    uint32_t c4 = creativePattern(p, time);

    uint32_t r = (((c1 >> 16) & 0xFF) + ((c2 >> 16) & 0xFF) +
                  ((c3 >> 16) & 0xFF) + ((c4 >> 16) & 0xFF)) /
                 4;
    uint32_t g = (((c1 >> 8) & 0xFF) + ((c2 >> 8) & 0xFF) +
                  ((c3 >> 8) & 0xFF) + ((c4 >> 8) & 0xFF)) /
                 4;
    uint32_t b = ((c1 & 0xFF) + (c2 & 0xFF) +
                  (c3 & 0xFF) + (c4 & 0xFF)) /
                 4;

    return strip.Color(r, g, b);
  }
  default:
    return 0;
  }
}

void loop()
{
  static float time = 0;
  static int pattern = 0;

  // Update sphere positions
  updateSpheres();

  // Clear all pixels
  strip.clear();

  // Fill pixels based on current pattern
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    Point3D p = indexToXYZ(i);
    uint32_t color = 0;

    switch (pattern)
    {
    case 0: // Polar color pattern
      color = wavePattern(p, time);
      color = polarColorPattern(p, time * 30);
      break;
    case 1: // Spheres pattern
      color = spherePattern(p);
      break;
    case 2: // Wave pattern
      color = polarColorPattern(p, time * 30);
      break;
    case 3: // Creative pattern
      color = wavePattern(p, time);
      break;
    case 4:
    { // Mixed pattern
      uint32_t c1 = polarColorPattern(p, time * 0.02);
      uint32_t c2 = spherePattern(p);
      uint32_t c3 = wavePattern(p, time);
      uint32_t c4 = creativePattern(p, time);

      // Average all colors
      uint32_t r = (((c1 >> 16) & 0xFF) + ((c2 >> 16) & 0xFF) +
                    ((c3 >> 16) & 0xFF) + ((c4 >> 16) & 0xFF)) /
                   4;
      uint32_t g = (((c1 >> 8) & 0xFF) + ((c2 >> 8) & 0xFF) +
                    ((c3 >> 8) & 0xFF) + ((c4 >> 8) & 0xFF)) /
                   4;
      uint32_t b = ((c1 & 0xFF) + (c2 & 0xFF) +
                    (c3 & 0xFF) + (c4 & 0xFF)) /
                   4;

      color = strip.Color(r, g, b);
      break;
    }
    }

    strip.setPixelColor(i, color);
  }

  strip.show();
  time += 0.1;
  Serial.println(time);

  // Change pattern every 10 seconds
  if ((time > 100))
  {
    time = 0;
    pattern = (pattern + 1) % 5;
  }
}