# WS2812-LED-Ping-Pong-Board

In this project, I use **Ping Pong Balls** to cover a wooden board, use MIT APP Invenotr to bulid a **custom android app**, and you can also contorl this board through **Web UI**.

The baord features built-in animated effects, time display, scrolling date display!

## 1. Components needed

### 1.1 Electronics
- **DC005** X 1
- **Button** X 4
- **WS2812LED** X 135
- **DS3231 Module** X 1
- **100uF Capaciter** X 1
- **ESP32 DEV Board** X 1
- **5V 3A Power Adapter** X 1

### 1.2 Other parts
- **Ping Pong Balls** X 135
- **24.78mm X 84mm Ply Board** X 1
- **Some Other Decoration Parts** X N

## 2. Assemble your LED Board

You should solder your comonents like below schematic:

![Shcematic](PCB/LED%20Board%20Shcematic.png)

But you can also send my [custom PCB](PCB/LED%20Board%20Gerber.zip) gerber files to PCB factory which is at `PCB/LED Board Gerber`.

Then, layout your ping pong balls like below:

![Layout](Images/Board%20Layout.jpg)

The final project should be like this:

![Final](Images/Final%20Project.jpg)

### 3. Contorl Your Board

There are three methods to control your LED board:

- Buttons
- Android App
- Web UI

### 3.1 Buttons

| Button  |        Function        |
| :-----: | :--------------------: |
| Button1 |    Turn LED ON/OFF     |
| Button2 | Set Background Palette |
| Button3 | Previous Display Mode  |
| Button4 |   Next Display Mode    |

### 3.2 Android App

You can [download](Android%20App/LED%20Board.apk) my custom android app to contorl your board.

Note that you should connect your Board Bluetooth first whose deafult device name is `LED Board 507`.

![Android App](Images/Android%20App.png)

### 3.3 Web UI

You can also contorl your board through WIFI.

Because this LED Board is put in our office, so I use **STAION** mode to let it connect to our office's router.

In order to easily access it, I change it's hostname to `ledboard-507`, so I can access it through http://ledboard-507.

At the same time, I add login user authentication. It's default username is `admin`, dafault password is `dinghao666`.

![LED board login](Images/led%20board%20login.png)

All of these username, password, ssid and ssid password are put in a **secret header filed** in arduino library folder.

After login, the web UI should be like this.

![Web UI](Images/ledboard%20web%20ui.png)

This Web UI use websockets, so the board's data are Synchronized to all clients.

### 3.4 Special Function

DS3231 has two alarms, so I use them as two timer schedule to turn LED On and OFF.

Alarm1 will turn LED OFF at **00:00**, and Alarm2 will turn LED ON at **08:00**.

## 4. Preview Video

https://user-images.githubusercontent.com/75357598/121656129-bf379a80-cad1-11eb-9fc5-0b48965fc32e.mp4
