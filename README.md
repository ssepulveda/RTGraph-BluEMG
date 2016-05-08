# RTGraph-BluEMG

Some of the files/instructions used in the video [E-textile mesuaring EMG](https://www.youtube.com/watch?v=6WxkOeTuX7w&google_comment_id=z125t515zmvuwh2wa04ccznpppjpjnri2d4&google_view_type#gpluscomments).

## HW
- Arduino Uno.
- [MOD-BT (Bluetooth)](https://www.olimex.com/Products/Modules/RF/MOD-BT/).
- [Shield EKG-EMG](https://www.olimex.com/Products/Duino/Shields/SHIELD-EKG-EMG/open-source-hardware).

## SW
- [RTGraph](https://github.com/ssepulveda/RTGraph)

## Instructions
###For connecting the Bluetooth as a serial port
1. Open a terminal and write `sudo nano /etc/bluetooth/rfcomm.conf`.
2. Add this lines to the file, replacing `xx:xx:xx:xx:xx:xx` with the MAC address of the bluetooth device.
```bash
rfcomm0 {
        bind no;
        device xx:xx:xx:xx:xx:xx;
        channel 1;
        comment "BluEMG Serial Port";
        }
```
3. The device specified in the mac will be know as `device 0` and binded to `rfcomm0`
that can be opened as a normal serial port.
4. Pair the bluetooth device with the computer.
5. Before using the serial port, the connection must be made: `sudo rfcomm connect 0`
6. Open RTGraph (modified as needed) to display the data stream as a plot.
