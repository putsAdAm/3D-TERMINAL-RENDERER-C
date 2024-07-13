#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Costante per il posizionamento del cursore all'inizio dello schermo
#define MOVE_CURSOR_HOME "\x1b[H"

// Costante per la cancellazione dello schermo e nascondere il cursore
#define CLEAR_SCREEN "\x1b[2J\x1b[?25l"

// Costanti per i codici di colore ANSI
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

// Costante per il numero di colori disponibili
#define NUM_COLORS (sizeof(colors) / sizeof(colors[0]))

// Costante per il pattern di caratteri del cuore
#define THEME " .,-~:;=!*#$@@"

int main() {
  // Cancella schermo e nasconde il cursore
  printf(CLEAR_SCREEN);

  // Definisci array di codici di colore per il ciclo
  const char *colors[] = {
    ANSI_COLOR_RED,
    ANSI_COLOR_BRIGHT_RED,
    ANSI_COLOR_YELLOW,
    ANSI_COLOR_BRIGHT_YELLOW,
    ANSI_COLOR_WHITE,
    // ANSI_COLOR_GREEN,
    // ANSI_COLOR_CYAN,
    // ANSI_COLOR_BLUE,
    ANSI_COLOR_MAGENTA,
    ANSI_COLOR_BRIGHT_MAGENTA,
    // ANSI_COLOR_BRIGHT_GREEN,
    // ANSI_COLOR_BRIGHT_CYAN,
    // ANSI_COLOR_BRIGHT_BLUE,
    // ANSI_COLOR_BRIGHT_WHITE,
    // ANSI_COLOR_BLACK,
    // ANSI_COLOR_BRIGHT_BLACK
  };

  int num_colors = NUM_COLORS;

  float t = 0;
  while (1) {
    float zb[100 * 40] = {0};
    float maxz = 0, c = cos(t), s = sin(t);
    for (float y = -0.5f; y <= 0.5f; y += 0.01f) {
      // Amplifica l'effetto pulsante
      float r = 0.4f + 0.1f * pow(0.5f + 0.5f * sin(t * 12 + y * 2), 8);
      for (float x = -0.5f; x <= 0.5f; x += 0.01f) {
        // Formula del cuore
        float z = -x * x - pow(1.2f * y - fabs(x) * 2 / 3, 2) + r * r;
        if (z < 0)
          continue;
        z = sqrt(z) / (2 - y);
        for (float tz = -z; tz <= z; tz += z / 6) {
          // Ruota
          float nx = x * c - tz * s;
          float nz = x * s + tz * c;

          // Aggiungi prospettiva
          float p = 1 + nz / 2;
          int vx = lroundf((nx * p + 0.5f) * 80 + 10);
          int vy = lroundf((-y * p + 0.5f) * 39 + 2);
          int idx = vx + vy * 100;
          if (zb[idx] <= nz) {
            zb[idx] = nz;
            if (maxz <= nz)
              maxz = nz;
          }
        }
      }
    }

    printf(MOVE_CURSOR_HOME); // Posiziona il cursore all'inizio dello schermo

    int color_index = ((int)(t * 10)) % num_colors; // Ciclo attraverso i colori
    const char *color = colors[color_index];

    for (int i = 0; i < 100 * 40; i++) {
      if (i % 100 == 0) {
        putchar(10); // Stampa un carattere di nuova linea ogni 100 caratteri
      } else {
        // Imposta il colore variabile per il cuore e reimposta il colore dopo la stampa
        printf("%s%c%s", color, THEME [lroundf(zb[i] / maxz * 13)], ANSI_COLOR_RESET);
      }
    }

    t += 0.005f;

    // Ritardo utilizzando nanosleep per controllare la velocità di aggiornamento
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = 3000000L; // 3 millisecondi
    nanosleep(&req, NULL);
  }

  return 0;
}


