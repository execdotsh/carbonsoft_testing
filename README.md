## Сети и виртуализация в Linux

Виртуальная машина KVM и LXC-контейнер c CentOS 6.7 на борту находятся в одном бридже br1. Назовём их X (KVM) и Y (LXC).

    [root@x ~]# ip -4 a show eth1
    3: eth1: <BROADCAST,MULTICAST,NOARP,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
        inet 10.30.30.1/24 brd 10.30.30.255 scope global eth1

    [root@y ~]# ip -4 a show eth1
    21: eth1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
        inet 10.30.30.3/24 brd 10.30.30.255 scope global eth1

Проблема: они недоступны друг для друга.

    [root@x ~]# ping -c 1 10.30.30.3
    PING 10.30.30.3 (10.30.30.3) 56(84) bytes of data.
    --- 10.30.30.3 ping statistics ---
    1 packets transmitted, 0 received, 100% packet loss, time 10000ms

    [root@y ~]# ping -c 1 10.30.30.1
    PING 10.30.30.1 (10.30.30.1) 56(84) bytes of data.
    From 10.30.30.3 icmp_seq=1 Destination Host Unreachable
    --- 10.30.30.1 ping statistics ---
    1 packets transmitted, 0 received, +1 errors, 100% packet loss, time 3000ms

Вопросы:
> из-за чего они недоступны?

в виртуальной машине на интерфейсе eth1 включен флаг NOARP из за этого контейнер не может обнаружить виртуальную машину и виртуальная машина не может обнаружить контейнер.

> как это исправить?

```
[root@x ~]# ip link set eth1 arp on
```

## С и тестирование

Задание и шаблон проекта в папке [unit_testing_example](https://github.com/carbonsoft/test_sys_dev/tree/master/reductor_developer/unit_testing_example).

Нужно написать unit-тест к функции func и добавить вывод подсказки по аргументам, при передаче `--help` или просто неправильных аргументов.

[Решено ✓](https://github.com/execdotsh/carbonsoft_testing/tree/master/carbonsoft_unit_testing)

## Shell-скриптинг

>1. Выведите IP адрес под которым машина отправляет запросы к 8.8.8.8 (учитывать NAT после выхода пакета из машины не нужно, нужен IP адрес с которым пакет покидает машину).

```bash
#!/bin/bash

ip route get to 8.8.8.8 | sed -n 's/.*src[[:space:]]*\([^[:space:]]*\).*/\1/p'
```

>2. Напишите команду для поиска в текстовом файле (абстрактном в вакууме) всех нечётных чисел от 1 до 100, записанных отдельным словом. Можно использовать заранее сгенерированные временные файлы.

```bash
#!/bin/bash

nums=`tr -s '[[:space:]]' '\n' | grep '^[[:digit:]]*$'`

for n in $nums
do
        if test "$n" -lt "100" -a `expr $n % 2` == 1
        then
                echo $n
        fi
done
```

>3. Выведите список сетевых карт в формате: "название сетевой карты в системе, например eth1" "модель сетевой карты, например Intel Corporation 82540EM Gigabit Ethernet Controller"

```bash
#!/bin/bash

pci=`lspci -D`

for dev_path in /sys/class/net/*
do
        dev_name=`basename "$dev_path"`
        dev_pci=`realpath "$dev_path/device" | sed -n 's/.*\([[:xdigit:]]\{4\}:[[:xdigit:]]\{2\}:[[:xdigit:]]\{2\}\.[[:xdigit:]]\).*/\1/p'`
        dev_hw_name=`echo "$pci" | sed -n "s/^$dev_pci[[:space:]]\+\(.*\)$/\1/p"`
        if test -z "$dev_hw_name"
        then
                dev_hw_name="<virtual>"
        fi
        echo -e "$dev_name\t$dev_hw_name"
done
```

## Python 2

Напишите утилиту, которая принимает на stdin кириллические домены в кодировках utf-8 и cp1251 (заранее неизвестно какая именно, при этом разные строки могут быть в разных кодировках) и выводят их закодированными по IDNA (плохойсайтик.рф -> xn--80aqbbgbvagyn4a.xn--p1ai). Учтите, что домены могут быть не только в зоне .рф.

## PCAP, трафик

Напишите неинтерактивную программу на любом языке программирования, который покажется наиболее подходящим для этой задачи, которая анализирует трафик на вашем компьютере, собирает 5 доменных имён из запросов к DNS-серверам, а затем логирует последующие 10 пакетов с HTTP и HTTPS запросами к этим доменам.

Bonus point: воспроизведите те пойманные 10 HTTP и HTTPS запросов.

## Базы данных, SQLAlchemy

Создайте таблицу в любой базе данных (рекомендую sqlite).

``` sql
CREATE TABLE history (date timestamp, counter integer);
```
Напишите две программы:

1. Заполняет таблицу случайными данными за последнюю неделю с интервалом в 1 час. Любой язык.
2. С помощью SQLAlchemy получает отчёт вида: дата, максимальное значение counter за день, сумма всех counter за день. Python.

## Гляньте документацию

Вообще о том, чем придётся заниматься в проекте довольно хорошо говорит документация. Можете её глянуть, она в открытом доступе - https://docs.carbonsoft.ru/113115207

В принципе и сам продукт над которым предстоит работать тоже можно взять и установить, пощупать. Разве что, возможно, настройка схемы [тестовый абонент] -> [carbon reductor с режиме роутера с nat] -> [интернет] займёт много времени.

Подумайте заранее, точно ли это то, чем вы готовы заниматься хотя бы полгода-год.
