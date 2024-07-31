/*
ANIMATED HEART by putsAdAm: working on Windows machines and Unix systems
*/
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

// Constants for cursor positioning and screen clearing
#define MOVE_CURSOR_HOME "\x1b[H" // Move cursor to the home position
#define CLEAR_SCREEN "\x1b[2J\x1b[?25l" // Clear screen and hide cursor

// ANSI color code constants for different colors
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK "\x1b[90m"
#define ANSI_COLOR_BRIGHT_RED "\x1b[91m"
#define ANSI_COLOR_BRIGHT_GREEN "\x1b[92m"
#define ANSI_COLOR_BRIGHT_YELLOW "\x1b[93m"
#define ANSI_COLOR_BRIGHT_BLUE "\x1b[94m"
#define ANSI_COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define ANSI_COLOR_BRIGHT_CYAN "\x1b[96m"
#define ANSI_COLOR_BRIGHT_WHITE "\x1b[97m"

// Constant to calculate the number of colors selected
#define NUM_COLORS (sizeof(colors) / sizeof(colors[0]))

// Pattern characters for drawing the function
#define THEME " .,-~:;!=*$#8@"

// Constants for the plane range, the radius and the screen definition
#define RANGE 0.5f // Range for x and y coordinates
#define STANDARD_RANGE 0.5f // Standard range for normalizing the radius
#define R_BASE 0.4f // Base radius
#define R_FACTOR 0.07f // Factor to modify the radius
#define DEFINITION 0.01f //Screen definition (0.01f recommended)

// Timing constants for animation
#define ANIMATION_TIME 0.02f // Time increment for animation
#define UPDATE_TIME 3000000L // Sleep time in nanoseconds for controlling animation speed
#define COLOR_TIME 0.03f //Time increment for color changes

// Functions and parameters
#define MUL 0.6f //decomment if you need the MUL constant for the toroid or the cube
#define AX 0.75f //decomment if you need the "a" value in the ellipse funcion
#define BX 0.25f //decomment if you need the "b" value in the ellipse funcion
#define CX 0.25f //decomment if you need the "c" value in the ellipse funcion
#define HEART_FUN -x*x - pow(1.2f*y - fabs(x)*2/3, 2) + r*r //Equation of the heart function
#define SPHERE_FUN sqrt(r*r - x*x - y*y) //Equation of the sphere function
#define TOROID_FUN sqrt(pow(0.5f*MUL*r, 2) - pow(MUL*r - sqrt(x*x + y*y), 2)) //Equation of the toroid function
#define ELLIPSE_FUN CX*sqrt(r - (x*x)/(AX*AX) - (y*y)/(BX*BX)) //Equation of the ellipse function
#define CUBE_FUN ((fabs(x) <= (MUL*r) && fabs(y) <= (MUL*r)) ? (MUL*r) : -1) //Equation of the cube function

//Chosen function
#define CURRENT_FUN TOROID_FUN

// Three different radius effects
#define EXP_RAD (R_BASE * RANGE)/STANDARD_RANGE + ((R_FACTOR * RANGE)/STANDARD_RANGE) * sin(t)
#define FIX_RAD (R_BASE * RANGE)/STANDARD_RANGE
#define PULS_RAD ((R_BASE * RANGE) / STANDARD_RANGE + (R_FACTOR * RANGE) / STANDARD_RANGE * pow(0.5f + 0.5f * sin(t * 12 + (y) * 2), 8))

// Chosen radius effect
#define CURRENT_RAD FIX_RAD

// Function prototypes
void initialize_screen();
const char* get_color(int t);
void calculate_depths(float t, float zvalues[], float* maxz);
void print_fun(float zvalues[], float maxz, const char* color);
void update_time(float* t, float* ct);
void sleep_for_animation();
bool check_exit_condition();

#ifndef _WIN32
void handle_signal(int signal);
#endif

// Global variable to control the loop
volatile bool running = true;

int main() {
#ifndef _WIN32
    // Register signal handler for a controlled exit on Unix-based systems
    signal(SIGINT, handle_signal);
#endif

    // Clear the screen and hide the cursor
    initialize_screen();

    // Initialize the animation time variable and the color change variable
    float t = 0;
    float ct = 0;

    // Infinite loop to continuously update the animation
    while (!check_exit_condition()) {
        // Array to store depth values for each point on the screen
        float zvalues[100 * 40] = {0}; // Initialize depth values to 0
        float maxz = 0; // Variable to keep track of the maximum depth value

        // Calculate the depth values of the function
        calculate_depths(t, zvalues, &maxz);

        // Get the current color based on time t
        const char* color = get_color(ct);

        // Print the function using the calculated depth values and the current color
        print_fun(zvalues, maxz, color);

        // Update the animation times
        update_time(&t, &ct);

        // Sleep to control the animation speed
        sleep_for_animation();
    }

    return 0;
}

// Function to clear the screen and hide the cursor
void initialize_screen() {
    printf(CLEAR_SCREEN); // Send ANSI escape codes to clear the screen and hide the cursor
}

// Function to get the current color based on time t
const char* get_color(int t) {
    static const char* colors[] = {
        ANSI_COLOR_RED,
        ANSI_COLOR_BRIGHT_RED,
        ANSI_COLOR_YELLOW,
        ANSI_COLOR_BRIGHT_YELLOW,
        ANSI_COLOR_WHITE,
        ANSI_COLOR_MAGENTA,
        ANSI_COLOR_BRIGHT_MAGENTA,
        // Additional colors commented out
        // ANSI_COLOR_GREEN,
        // ANSI_COLOR_BRIGHT_GREEN,
        // ANSI_COLOR_BLUE,
        // ANSI_COLOR_BRIGHT_BLUE,
        // ANSI_COLOR_CYAN,
        // ANSI_COLOR_BRIGHT_CYAN,
        // ANSI_COLOR_BLACK,
        // ANSI_COLOR_BRIGHT_BLACK,
        // ANSI_COLOR_WHITE,
        // ANSI_COLOR_BRIGHT_WHITE,
    };
    int num_colors = NUM_COLORS; // Calculate the number of colors available
    int color_index = ((int)(t)) % num_colors; // Determine the color index based on time t
    return colors[color_index]; // Return the current color
}

// Function to calculate depth values for the function
void calculate_depths(float t, float zvalues[], float* maxz) {
    float c = cos(t), s = sin(t); // Calculate cosine and sine of time t for rotation
    for (float y = -RANGE; y <= RANGE; y += DEFINITION) {
        // Calculate the radius according to the chosen effect
        float r = CURRENT_RAD;
        for (float x = -RANGE; x <= RANGE; x += DEFINITION) {
            // Calculate the z value according to the chosen function
            float z = CURRENT_FUN;
            if (z < 0) continue; // Skip if z is negative 
            for (float tz = -z; tz <= z; tz += z / 6) {
                // Rotate the point around the z-axis using the calculated cosine and sine
                float rotx = x * c - tz * s;
                float roty = x * s + tz * c;
                // Add perspective to the rotated coordinates
                float p = 1 + roty / 2;
                int screen_x = lroundf((rotx * p + RANGE) * 80 + 10); // Calculate x screen coordinate
                int screen_y = lroundf((-y * p + RANGE) * 39 + 2); // Calculate y screen coordinate
                int ind = screen_x + screen_y * 100; // Calculate index in zvalues array
                if (zvalues[ind] <= roty) { // Update depth value if the new depth is greater
                    zvalues[ind] = roty;
                    if (*maxz <= roty) *maxz = roty; // Update maxz if necessary
                }
            }
        }
    }
}

// Function to print the function
void print_fun(float zvalues[], float maxz, const char* color) {
    printf(MOVE_CURSOR_HOME); // Move cursor to the home position
    for (int i = 0; i < 100 * 40; i++) {
        if (i % 100 == 0) {
            putchar(10); // Print newline every 100 characters
        } else {
            // Print heart character with color and reset color
            printf("%s%c%s", color, THEME[lroundf(zvalues[i] / maxz * 13)], ANSI_COLOR_RESET);
        }
    }
}

// Function to update the animation time
void update_time(float* t, float* ct) {
    *t += ANIMATION_TIME; // Increment time by animation time step
    *ct += COLOR_TIME; // Increment time by color change step
}

// Function to sleep for controlling the animation speed
void sleep_for_animation() {
    struct timespec req = {0};
    req.tv_sec = 0; // Set sleep seconds to 0
    req.tv_nsec = UPDATE_TIME; // Set sleep nanoseconds to UPDATE_TIME
    nanosleep(&req, NULL); // Sleep for the specified duration
}

// Function to check if a key has been pressed for exiting the loop
bool check_exit_condition() {
#ifdef _WIN32
    // On Windows, check if a key has been pressed
    if (_kbhit()) {
        _getch(); // Consume the key press
        return true; // Return true to exit the loop
    }
    return false; // Return false to continue the loop
#else
    // On Unix-based systems, check the global running variable
    return !running; // Return true if running is false to exit the loop
#endif
}

#ifndef _WIN32
// Signal handler to exit the loop on Unix-based systems
void handle_signal(int signal) {
    if (signal == SIGINT) {
        running = false; // Set running to false to exit the loop
    }
}
#endif
