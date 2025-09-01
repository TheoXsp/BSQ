#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
  size_t size;
  char *map;
} map_t;

typedef struct {
  size_t x;
  size_t y;
  size_t size;
} res_t;

static inline map_t read_map(const char *path) {
  struct stat st;
  if (stat(path, &st) == -1) return (map_t){0};

  size_t size = (size_t)st.st_size;
  char *map = (char *)malloc(size);
  if (!map) return (map_t){0};

  FILE *f = fopen(path, "rb");
  if (!f || fread(map, 1, size, f) != size) {
    free(map);
    return (map_t){0};
  }
  fclose(f);
  return (map_t){size, map};
}

static res_t solve(const map_t *map, size_t grid_start, size_t line_width, size_t rows) {
  res_t result = {0};
  size_t pos = grid_start;
  size_t y = 0;
  uint16_t *prev = NULL;
  uint16_t *curr = NULL;

  prev = (uint16_t *)calloc(line_width + 1, sizeof(uint16_t));
  curr = (uint16_t *)calloc(line_width + 1, sizeof(uint16_t));
  if (!prev || !curr) {
    free(map->map);
    free(prev);
    free(curr);
    return (res_t){0};
  }

  while (pos < map->size && y < rows) {
    size_t x = 1;
    for (; x <= line_width; x++) {
      size_t idx = pos + x - 1;
      if (idx >= map->size || map->map[idx] == '\n') break;
      if (map->map[idx] != '.') {
        curr[x] = 0;
        continue;
      }
      uint16_t min = prev[x - 1] < prev[x] ? prev[x - 1] : prev[x];
      min = min < curr[x - 1] ? min : curr[x - 1];
      curr[x] = min + 1;
      if (curr[x] > result.size) {
        result.size = curr[x];
        result.y = y - (curr[x] - 1);
        result.x = x - 1 - (curr[x] - 1);
      }
    }
    for (; x <= line_width; x++) curr[x] = 0;
    uint16_t *tmp = prev;
    prev = curr;
    curr = tmp;
    pos += line_width;
    if (pos < map->size && map->map[pos] == '\n') pos++;
    y++;
  }
  free(prev);
  free(curr);
  return result;
}

int main(int ac, char **av) {
  if (ac != 2) return 1;
  map_t map = read_map(av[1]);
  char *endptr = NULL;

  if (!map.map) return 1;

  size_t rows = strtoul(map.map, &endptr, 10);
  if (endptr == map.map || *endptr != '\n') {
    free(map.map);
    return 1;
  }
  if (rows > UINT16_MAX) {
    fprintf(stderr,
            "Error: too many rows for uint16_t, please use larger type for this map. (Note: this "
            "may slown down the program a bit). (max %u)\n",
            UINT16_MAX);
    free(map.map);
    return 1;
  }

  size_t grid_start = (endptr - map.map) + 1;
  size_t line_width = 0;
  for (size_t i = grid_start; i < map.size && map.map[i] != '\n'; i++, line_width++);
  if (!line_width) {
    free(map.map);
    return 1;
  }

  res_t best = solve(&map, grid_start, line_width, rows);
  if (!best.size) {
    fwrite(map.map + grid_start, 1, map.size - grid_start, stdout);
    free(map.map);
    return 1;
  }

  size_t row_start = grid_start + best.y * (line_width + 1);
  for (size_t ry = best.y; ry < best.y + best.size; ry++) {
    memset(&map.map[row_start + best.x], 'x', best.size);
    row_start += line_width + 1;
  }
  fwrite(map.map + grid_start, 1, map.size - grid_start, stdout);
  free(map.map);
  return 0;
}
