# INSTRUCTIONS
Following is how to install dependencies that are not by default installed on a standard Linux system. We are working to reduce the number of dependencies.


## REQUIRED

### wiringPi - required for one version of PiMotor, required for encoders

```
cd
git clone git://git.drogon.net/wiringPi
cd wiringPi
git pull origin
./build
```

### bcm2835 - required for MPU6050

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


### pigpio - analogous to wiringPi, required for one (currently used) version of PiMotor, am trying to replace with wiringPi
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



## OPTIONAL


### libi2c-dev - not required, was used in attempt to communicate with MPU6050
```
sudo apt-get install libi2c-dev
```


