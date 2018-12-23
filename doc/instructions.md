# INSTRUCTIONS
Following is how to install dependencies that are not by default installed on a standard Linux system. We are working to reduce the number of dependencies.


## REQUIRED

### bcm2835 - mpu6050

```
cd
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.58.tar.gz
tar zxvf bcm2835-1.58.tar.gz
cd bcm2835-1.58
./configure
make
sudo make check
sudo make install
```

## OPTIONAL

### pigpio - mdd10a, hc-sr04

```
cd
rm pigpio.zip
sudo rm -rf PIGPIO
wget abyz.me.uk/rpi/pigpio/pigpio.zip
unzip pigpio.zip
cd PIGPIO
make
sudo make install
```

### i2clib-dev - lcd-i2c
```
sudo apt-get install i2clib-dev
```


### wiringPi - not actually required for anything (is analogous to pigpio)

```
cd
git clone git://git.drogon.net/wiringPi
cd wiringPi
git pull origin
./build
```
