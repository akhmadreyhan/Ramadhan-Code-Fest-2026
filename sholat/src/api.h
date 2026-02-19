#ifndef API_H
#define API_H

#define TIME_STR_LEN 16

typedef struct
{
  char imsak[TIME_STR_LEN];
  char fajr[TIME_STR_LEN];
  char dhuhr[TIME_STR_LEN];
  char asr[TIME_STR_LEN];
  char maghrib[TIME_STR_LEN];
  char isha[TIME_STR_LEN];

  char hijri_day[TIME_STR_LEN];
  char hijri_month[TIME_STR_LEN];
  char hijri_year[TIME_STR_LEN];
} PrayerTimes;

int get_today_date(char *buffer, size_t size);

int fetch_prayer_times(double lat, double lon, const char *date, PrayerTimes *out);

#endif
