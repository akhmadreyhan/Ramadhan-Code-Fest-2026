#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>

#include "api.h"
#include "../vendor/cJSON.h"

#define API_URL_FMT \
  "https://api.aladhan.com/v1/timings/%s?latitude=%f&longitude=%f&method=20"

#define RESP_INIT_CAP 4096

struct buffer {
  char *data;
  size_t size;
};

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct buffer *buf = (struct buffer *)userp;

  char *ptr = realloc(buf->data, buf->size + realsize + 1);
  if (!ptr)
    return 0;

  buf->data = ptr;
  memcpy(buf->data + buf->size, contents, realsize);
  buf->size += realsize;
  buf->data[buf->size] = '\0';

  return realsize;
}

static void copy_time(char *dst, size_t dst_size, const char *src)
{
  if (!dst || dst_size == 0)
    return;

  dst[0] = '\0';
  if (!src)
    return;

  size_t i = 0;
  while (src[i] && i < dst_size - 1 && src[i] != ' ' && src[i] != '(')
  {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
}

int get_today_date(char *buffer, size_t size)
{
  if (!buffer || size < 11)
    return 0;

  time_t now = time(NULL);
  struct tm local;
  if (!localtime_r(&now, &local))
    return 0;

  snprintf(buffer, size, "%02d-%02d-%02d", local.tm_mday, local.tm_mon + 1, local.tm_year + 1900);
  return 1;
}

int fetch_prayer_times(double lat, double lon, const char *date, PrayerTimes *out)
{
  if (!date || !out)
    return 0;

  memset(out, 0, sizeof(*out));

  CURL *curl = curl_easy_init();
  if (!curl)
    return 0;

  char url[256];
  snprintf(url, sizeof(url), API_URL_FMT, date, lat, lon);

  struct buffer resp;
  resp.data = malloc(RESP_INIT_CAP);
  resp.size = 0;
  if (!resp.data)
  {
    curl_easy_cleanup(curl);
    return 0;
  }
  resp.data[0] = '\0';

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "sholat/1.0");

  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK)
  {
    free(resp.data);
    return 0;
  }

  cJSON *root = cJSON_Parse(resp.data);
  free(resp.data);
  if (!root)
    return 0;

  cJSON *data = cJSON_GetObjectItem(root, "data");
  cJSON *timings = data ? cJSON_GetObjectItem(data, "timings") : NULL;

  if (!cJSON_IsObject(timings))
  {
    cJSON_Delete(root);
    return 0;
  }

  copy_time(out->imsak, TIME_STR_LEN, cJSON_GetObjectItem(timings, "Imsak")->valuestring);
  copy_time(out->fajr, TIME_STR_LEN, cJSON_GetObjectItem(timings, "Fajr")->valuestring);
  copy_time(out->dhuhr, TIME_STR_LEN, cJSON_GetObjectItem(timings, "Dhuhr")->valuestring);
  copy_time(out->asr, TIME_STR_LEN, cJSON_GetObjectItem(timings, "Asr")->valuestring);
  copy_time(out->maghrib, TIME_STR_LEN, cJSON_GetObjectItem(timings, "Maghrib")->valuestring);
  copy_time(out->isha, TIME_STR_LEN, cJSON_GetObjectItem(timings, "Isha")->valuestring);

  cJSON *date_obj = cJSON_GetObjectItem(data, "date");
  cJSON *hijri = date_obj ? cJSON_GetObjectItem(date_obj, "hijri") : NULL;

  if (cJSON_IsObject(hijri))
  {
    cJSON *h_day = cJSON_GetObjectItem(hijri, "day");
    cJSON *h_year = cJSON_GetObjectItem(hijri, "year");
    cJSON *h_month = cJSON_GetObjectItem(hijri, "month");

    if (cJSON_IsString(h_day))
      strncpy(out->hijri_day, h_day->valuestring, TIME_STR_LEN - 1);

    if (cJSON_IsString(h_year))
      strncpy(out->hijri_year, h_year->valuestring, TIME_STR_LEN - 1);

    if (cJSON_IsObject(h_month))
    {
      cJSON *m_en = cJSON_GetObjectItem(h_month, "en");
      if (cJSON_IsString(m_en))
        strncpy(out->hijri_month, m_en->valuestring, TIME_STR_LEN - 1);
    }
  }

  cJSON_Delete(root);
  return 1;
}