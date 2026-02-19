#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "city.h"
#include "../vendor/cJSON.h"

#define DATA_FILE "data/regencies.json"

static void trim(char *s)
{
  char *p = s;
  while (*p && isspace((unsigned char)*p))
    p++;
  if (p != s)
    memmove(s, p, strlen(p) + 1);

  size_t len = strlen(s);
  while (len > 0 && isspace((unsigned char)s[len - 1]))
  {
    s[--len] = '\0';
  }
}

static void collapse_spaces(char *s)
{
  char *dst = s;
  int in_space = 0;
  for (; *s; s++)
  {
    if (isspace((unsigned char)*s))
    {
      if (!in_space)
      {
        *dst++ = ' ';
        in_space = 1;
      }
    }
    else
    {
      *dst++ = *s;
      in_space = 0;
    }
  }
  *dst = '\0';
}

static void to_upper(char *s)
{
  for (; *s; s++)
    *s = (char)toupper((unsigned char)*s);
}

static void strip_prefix(char *s)
{
  const char *p1 = "KOTA ";
  const char *p2 = "KABUPATEN ";

  if (strncmp(s, p1, strlen(p1)) == 0)
  {
    memmove(s, s + strlen(p1), strlen(s) - strlen(p1) + 1);
  }
  else if (strncmp(s, p2, strlen(p2)) == 0)
  {
    memmove(s, s + strlen(p2), strlen(s) - strlen(p2) + 1);
  }
}

static void normalize(char *s)
{
  trim(s);
  collapse_spaces(s);
  to_upper(s);
  strip_prefix(s);
  trim(s);
}

static char *read_file(const char *path)
{
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *buf = (char *)malloc(size + 1);
  if (!buf)
  {
    fclose(f);
    return NULL;
  }

  if (fread(buf, 1, size, f) != (size_t)size)
  {
    fclose(f);
    free(buf);
    return NULL;
  }
  buf[size] = '\0';
  fclose(f);
  return buf;
}

int find_city(const char *input, City *result)
{
  if (!input || !result)
    return 0;

  char needle[128];
  strncpy(needle, input, sizeof(needle) - 1);
  needle[sizeof(needle) - 1] = '\0';
  normalize(needle);

  char *json_text = read_file(DATA_FILE);
  if (!json_text)
    return 0;

  cJSON *root = cJSON_Parse(json_text);
  free(json_text);
  if (!root || !cJSON_IsArray(root))
  {
    if (root)
      cJSON_Delete(root);
    return 0;
  }

  int found = 0;
  cJSON *item = NULL;

  cJSON_ArrayForEach(item, root)
  {
    cJSON *name = cJSON_GetObjectItem(item, "name");
    cJSON *lat = cJSON_GetObjectItem(item, "latitude");
    cJSON *lon = cJSON_GetObjectItem(item, "longitude");

    if (!cJSON_IsString(name) || !cJSON_IsNumber(lat) || !cJSON_IsNumber(lon))
      continue;

    char candidate[128];
    strncpy(candidate, name->valuestring, sizeof(candidate) - 1);
    candidate[sizeof(candidate) - 1] = '\0';
    normalize(candidate);

    if (strcmp(candidate, needle) == 0)
    {
      strncpy(result->display_name, name->valuestring, sizeof(result->display_name) - 1);
      result->display_name[sizeof(result->display_name) - 1] = '\0';
      for (char *p = result->display_name; *p; p++)
      {
        if (p == result->display_name || *(p - 1) == ' ')
          *p = (char)toupper((unsigned char)*p);
        else
          *p = (char)tolower((unsigned char)*p);
      }

      result->latitude = lat->valuedouble;
      result->longitude = lon->valuedouble;

      found = 1;
      break;
    }
  }

  cJSON_Delete(root);
  return found;
}