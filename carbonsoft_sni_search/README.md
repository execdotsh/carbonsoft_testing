# Задание

**Контекст**: к нам приходят все TCP пакеты, идущие на порт 443, нам нужны только ClientHello, их доля - около 3%, всё остальное нужно отбросить как можно раньше. Мы их анализируем, для анализа нам нужны данные после TCP заголовка. Для того чтобы получить указатель на данные после TCP-заголовка нам надо знать длину TCP-заголовка. В идеальном мире это вычисляется по полю TCP-заголовка - data offset (doff). Типовые длины TCP-заголовков - 79% - 20 байт, 19% - 32 байта, но бывают и другие. У 99.9% провайдеров в 99.9% случаев оно выставлено правильно. Но есть пара установок, в которых поле в 99% случаев приходит с длиной = 0, то есть в TCP заголовке сказано, что его нет. Клиент готов платить так, что хватит на хлеб с двумя порциями масла, так что говорить "иди чини свой трафик, меняй все свои маршрутизаторы-коммутаторы" нельзя.

**Дано**: очень обрезанная версия модуля анализа HTTPS-трафика, которой кормятся сохранённые из wireshark пакеты. main.c - основная программа, в ней же определена функция snimatch_mt, которая возвращает true, если пакет является SSL Client Hello и false в любом другом случае. Для упрощения считаем любой пакет с значением 0x16 после заголовка - SSL Client Hello. Совместимость с ядром находится с файлах sk_buff.c/h, специфика SSL/TLS в sni.c/h.

**Что сделать**: исправьте функцию snimatch_mt в файле таким образом, чтобы она корректно обрабатывала все тестовые пакеты и была приемлемой для того, чтобы включить её в продакшн, выкатив _всем_ клиентам (т.е. функция должна быть универсальной), чтобы не вспоминать о том что бывают глючные TCP-пакеты раз и навсегда. Важно - думайте о производительности всех систем в целом.

Решено ✓
