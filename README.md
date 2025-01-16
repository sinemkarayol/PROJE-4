# PROJE-4
Tiva TM4C123GXL LCD-ADC-UART Projesi

Proje Açıklaması
Bu proje, Texas Instruments TM4C123GXL (Tiva C Series) mikrodenetleyicisini kullanarak bir LCD ekrana veri yazdırma, ADC ile sıcaklık sensörü okuma ve UART üzerinden bilgisayar ile seri haberleşme işlevlerini gerçekleştirmektedir.

Kullanılan Bileşenler ve Bağlantılar
1. Donanım Bileşenleri:
Tiva C Series TM4C123GXL LaunchPad
16x2 LCD ekran (RS: PB0, E: PB1, D4-D7: PB4-PB7)
LM35 Sıcaklık Sensörü (Bağlantı: PE3 - ADC0)
Bilgisayar (UART haberleşmesi için)
2. Yazılım Geliştirme Araçları:
Code Composer Studio (CCS) – Mikrodenetleyici programlaması için
Sharp Develop – Bilgisayar tarafında seri haberleşme için

Projenin Çalışma Mantığı
1. LCD Kullanımı
LCD, 4-bit modda çalışmaktadır.
Başlangıçta LCD'nin ayarları yapılır ve başlık mesajı ekrana yazdırılır.
ADC tarafından okunan sıcaklık değeri ekrana yazdırılır.
2. ADC Kullanımı
LM35 sıcaklık sensörü, ADC modülü aracılığıyla okunur.
ADC0 kanalında (PE3 pini) sıcaklık verisi işlenerek ekrana ve UART’a gönderilir.
3. UART Seri Haberleşme
UART ile bilgisayara sıcaklık verisi gönderilir.
UART üzerinden gelen veriler mikrodenetleyici tarafından okunup işlenebilir.

Projeyi Çalıştırma
Donanımı uygun şekilde bağlayın.
Code Composer Studio (CCS) ile kodu derleyin ve Tiva'ya yükleyin.
Bilgisayarda bir seri port haberleşme programı açarak (örneğin PuTTY), UART verilerini görüntüleyin.
LCD'de ve bilgisayar terminalinde sıcaklık değerini takip edin.
