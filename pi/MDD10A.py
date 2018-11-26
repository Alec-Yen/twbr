1.  #!/usr/bin/env python   

2.  # coding: latin-1   

3.  # I am Mohammad Omar, this module is builded to interface with the Driver MDD10A, to control two DC motors.   

4.  # the original code designed by Ingmar Stapel ,www.raspberry-pi-car.com to control two motors with a L298N H-Bridge   

5.  # The pins configuration for Model B Revision 1.0    

6.     

7.  # How to Use this module: 1- creating an instance of the class. 2- call the Init function, 3- call commands functions   

8.  # Example:   

9.  # import MDD10A   

10. # Motors = MDD10A.MDD10A()   

11.    

12. # Import the libraries the class needs   

13. import RPi.GPIO as io   

14. io.setmode(io.BCM)  

15.   

16. # Constant values,   

17. PWM_MAX                 = 100  

18.   

19. # Disable warning from GPIO  

20. io.setwarnings(False)  

21.   

22. # Here we configure the GPIO settings for the left and right motors spinning direction.  

23. # as described in the user manual of MDD10A https://www.robotshop.com/media/files/pdf/user-manual-mdd10a.pdf  

24. # there are four input PWM1-DIR1-PWM2-DIR2  

25. # WITH MAX Frequency 20 Hz, and it works as follow,  

26. #       Input   DIR     Output-A    Output-B  

27. #   PWM  off    X         off         off  

28. #   PWM  on     off       on          off  

29. #   PWM  on     on        off         on  

30.   

31. # The pins configuration for Model B Revision 1.0   

32. leftMotor_DIR_pin = 22  

33. io.setup(leftMotor_DIR_pin, io.OUT)  

34.   

35. rightMotor_DIR_pin = 23  

36. io.setup(rightMotor_DIR_pin, io.OUT)  

37.   

38. io.output(leftMotor_DIR_pin, False)  

39.   

40. io.output(rightMotor_DIR_pin, False)  

41.   

42.   

43. # Here we configure the GPIO settings for the left and right motors spinning speed.   

44.   

45. leftMotor_PWM_pin = 17  

46. rightMotor_PWM_pin = 18  

47.   

48. io.setup(leftMotor_PWM_pin, io.OUT)  

49. io.setup(rightMotor_PWM_pin, io.OUT)  

50.   

51. # MAX Frequency 20 Hz  

52. leftMotorPWM = io.PWM(leftMotor_PWM_pin,20)  

53. rightMotorPWM = io.PWM(rightMotor_PWM_pin,20)  

54.   

55. leftMotorPWM.start(0)  

56. leftMotorPWM.ChangeDutyCycle(0)  

57.   

58. rightMotorPWM.start(0)  

59. rightMotorPWM.ChangeDutyCycle(0)  

60.   

61. leftMotorPower = 0  

62. rightMotorPower = 0  

63.   

64. def getMotorPowers():  

65.       

66.     return (leftMotorPower,rightMotorPower)       

67.   

68. def setMotorLeft(power):  

69.   

70. # SetMotorLeft(power)  

71.   

72. # Sets the drive level for the left motor, from +1 (max) to -1 (min).  

73.   

74. # This is a short explanation for a better understanding:  

75. # SetMotorLeft(0)     -> left motor is stopped  

76. # SetMotorLeft(0.75)  -> left motor moving forward at 75% power  

77. # SetMotorLeft(-0.5)  -> left motor moving reverse at 50% power  

78. # SetMotorLeft(1)     -> left motor moving forward at 100% power  

79.   

80.     if power < 0:  

81.         # Reverse mode for the left motor  

82.         io.output(leftMotor_DIR_pin, False)  

83.         pwm = -int(PWM_MAX * power)  

84.         if pwm > PWM_MAX:  

85.             pwm = PWM_MAX  

86.     elif power > 0:  

87.         # Forward mode for the left motor  

88.         io.output(leftMotor_DIR_pin, True)  

89.         pwm = int(PWM_MAX * power)  

90.         if pwm > PWM_MAX:  

91.             pwm = PWM_MAX  

92.     else:  

93.         # Stopp mode for the left motor  

94.         io.output(leftMotor_DIR_pin, False)  

95.         pwm = 0  

96. #   print "SetMotorLeft", pwm  

97.     leftMotorPower = pwm  

98.     leftMotorPWM.ChangeDutyCycle(pwm)  

99.   

100.          def setMotorRight(power):  

101.            

102.          # SetMotorRight(power)  

103.            

104.          # Sets the drive level for the right motor, from +1 (max) to -1 (min).  

105.            

106.          # This is a short explanation for a better understanding:  

107.          # SetMotorRight(0)     -> right motor is stopped  

108.          # SetMotorRight(0.75)  -> right motor moving forward at 75% power  

109.          # SetMotorRight(-0.5)  -> right motor moving reverse at 50% power  

110.          # SetMotorRight(1)     -> right motor moving forward at 100% power  

111.            

112.              if power < 0:  

113.                  # Reverse mode for the right motor  

114.                  io.output(rightMotor_DIR_pin, True)  

115.                  pwm = -int(PWM_MAX * power)  

116.                  if pwm > PWM_MAX:  

117.                      pwm = PWM_MAX  

118.              elif power > 0:  

119.                  # Forward mode for the right motor  

120.                  io.output(rightMotor_DIR_pin, False)  

121.                  pwm = int(PWM_MAX * power)  

122.                  if pwm > PWM_MAX:  

123.                      pwm = PWM_MAX  

124.              else:  

125.                  # Stopp mode for the right motor  

126.                  io.output(rightMotor_DIR_pin, False)  

127.                  pwm = 0  

128.          #   print "SetMotorRight", pwm  

129.              rightMotorPower = pwm  

130.              rightMotorPWM.ChangeDutyCycle(pwm)  

131.            

132.          def exit():  

133.          # Program will clean up all GPIO settings and terminates  

134.              io.output(leftMotor_DIR_pin, False)  

135.              io.output(rightMotor_DIR_pin, False)  

136.              io.cleanup()  

Test MDD10A
