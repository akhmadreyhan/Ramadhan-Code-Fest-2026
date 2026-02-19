#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "city.h"
#include "api.h"

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"

#define MAX_CITY_LEN 256

int is_past_time(const char *hhmm)
{
  if (!hhmm)
    return 0;

  int h, m;
  if (sscanf(hhmm, "%d:%d", &h, &m) != 2)
    return 0;

  time_t now = time(NULL);
  struct tm t = *localtime(&now);

  if (t.tm_hour > h || (t.tm_hour == h && t.tm_min >= m))
    return 1;
  return 0;
}

int main(int argc, char *argv[])
{
  char city_input[MAX_CITY_LEN] = {0};

  if (argc < 2)
  {
    fprintf(stderr, "Usage: Sholat <nama_kota>\n");
    return 1;
  }

  for (int i = 1; i < argc; i++)
  {
    strncat(city_input, argv[i], MAX_CITY_LEN - strlen(city_input) - 1);
    if (i < argc - 1)
      strncat(city_input, " ", MAX_CITY_LEN - strlen(city_input) - 1);
  }

  City city;
  if (!find_city(city_input, &city))
  {
    fprintf(stderr, "Kota \"%s\" Tidak Ditemukan.\n", city_input);
    return 1;
  }

  char date[11];
  if (!get_today_date(date, sizeof(date)))
  {
    fprintf(stderr, "Gagal Mengambil Tanggal.\n");
    return 1;
  }

  int g_day, g_month, g_year;
  sscanf(date, "%02d-%02d-%04d", &g_day, &g_month, &g_year);
  const char *month_names[] = {"Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agu", "Sep", "Okt", "Nov", "Des"};
  const char *g_month_str = month_names[g_month - 1];

  PrayerTimes times;
  if (!fetch_prayer_times(city.latitude, city.longitude, date, &times))
  {
    fprintf(stderr, "Gagal mengambil jadwal sholat.\n");
    return 1;
  }

  printf("%02d - %s - %04d || %s - %s - %s Hijriyah\n\n", g_day, g_month_str, g_year, times.hijri_day, times.hijri_month, times.hijri_year);

  printf("Imsak   : %s\n", times.imsak);
  printf(RED BOLD "Subuh   : %s\n" RESET, times.fajr);
  printf("Dzuhur  : %s\n", times.dhuhr);
  printf("Ashar   : %s\n", times.asr);
  printf(GREEN BOLD "Maghrib : %s\n" RESET, times.maghrib);
  printf("Isya    : %s\n", times.isha);

  if (is_past_time(times.isha))
  {
    char next_date[11];
    time_t now = time(NULL) + 86400;
    struct tm tm_next;
    localtime_r(&now, &tm_next);
    snprintf(next_date, sizeof(next_date), "%02d-%02d-%04d", tm_next.tm_mday, tm_next.tm_mon + 1, tm_next.tm_year + 1900);

    PrayerTimes next_times;
    if (fetch_prayer_times(city.latitude, city.longitude, next_date, &next_times))
    {
      printf("\nNext Imsak : %s\n", next_times.imsak);
    }
  }

  return 0;
}
