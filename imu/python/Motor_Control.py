import  RPi.GPIO as GPIO
import time 

PWM_ = 12
DIR = 16

GPIO.setmode(GPIO.BOARD)
GPIO.setup(PWM_,GPIO.OUT)
GPIO.setup(DIR,GPIO.OUT)

GPIO.output(DIR,True)
pwm=GPIO.PWM(PWM_,100)
pwm.start(20)
time.sleep(2)
pwm.stop()


time.sleep(3)

GPIO.output(DIR,False)


pwm=GPIO.PWM(PWM_,100)
pwm.start(10)
time.sleep(2)
pwm.stop()
