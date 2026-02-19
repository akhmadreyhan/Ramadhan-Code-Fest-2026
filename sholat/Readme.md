# Sholat CLI

CLI sederhana untuk menampilkan jadwal sholat harian beserta tanggal Hijriyah, termasuk Imsak, Subuh, Dzuhur, Ashar, Maghrib, dan Isya, berdasarkan kota yang dipilih.  

Jika waktu Isya sudah lewat, akan menampilkan **Next Imsak** dari hari berikutnya.  

---

## Fitur

- Menampilkan jadwal sholat harian sesuai kota.
- Menampilkan tanggal Hijriyah bersamaan dengan tanggal Gregorian.
- Menampilkan **Next Imsak** jika waktu Isya telah lewat di hari yang sama
- Mendukung input nama kota dengan spasi (`./sholat "Kota Besar"`).

---

## Instalasi

1. Pastikan sudah ada **clang** dan **libcurl** di sistem.
2. Clone repository:

```bash
git clone <repo-url>
cd Ramadhan-Code-Fest-2026/sholat
```

3. Compile Program
```bash
make
```
4. Jalankan Program
```bash
./sholat [nama kota]
```
Contoh : 
```bash
/sholat surabaya
```

### Contoh Output : 
```
20 - Feb - 2026 || 3 - Ramaḍān - 1447 Hijriyah

Imsak   : 04:05
Subuh   : 04:15
Dzuhur  : 11:43
Ashar   : 14:51
Maghrib : 17:52
Isya    : 19:03

Next Imsak : 04:15
```



