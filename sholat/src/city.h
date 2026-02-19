#ifndef CITY_H
#define CITY_H

typedef struct
{
  char display_name[128];
  double latitude;
  double longitude;
} City;

int find_city(const char *input, City *result);

#endif
