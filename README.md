# nhapometro

by Juan Ramón Sixto Anello

## Requirements

- Arduino IDE

- $ sudo apt install python-serial


# config

- clientAdaFruit.py

```python
# Set to your Adafruit IO key & username below.
ADAFRUIT_IO_KEY      = ''
ADAFRUIT_IO_USERNAME = ''  # See https://accounts.adafruit.com
                                                    # to find your username.
```

- nhapometro.py


```python
# Set to your Adafruit IO key & username below.
ADAFRUIT_IO_KEY      = ''
ADAFRUIT_IO_USERNAME = ''  # See https://accounts.adafruit.com
```

(twice)
```python
     respuesta = requests.post("https://xxxxxxxx/xxxxx/xxxxxx.php", data=payload)
```



# Install

- install logrotate file

```bash
aespinosa@xxxx:~/nhapometro/nhapometro (master) $ sudo cp nhapometro.log /etc/logrotate.d/ && sudo chown root: /etc/logrotate.d/nhapometro.log
```


- install systemd service

```bash
$ sudo cp nhapometro.service /etc/systemd/system/
$ sudo service nhapometro start
$ sudo service nhapometro status
```


# Disclaimer

All software and hardware by Juan Ramón Sixto Anello.

Only logrotate and systemd service by Alexandre Espinosa Menor.
