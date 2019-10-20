固件使用方法：把程序文件夹里的Adafruit_GFX和Adafruit_SSD1306放入arduino IDE的库文件夹，然后打开nano.ino文件就可以编译通过了，推荐使用最新版本IDE

硬件连接可以参考固件里的引脚定义，按照说明把模块和主控板的对应引脚连上就行，需要自己调整的是pid参数，这个推荐初学者先熟悉一下pid的整定方式，可以参考这篇帖子入门：
http://www.geek-workshop.com/forum.php?mod=viewthread&tid=12538&highlight=%D7%D4%C6%BD%BA%E2

所用到的模块有：mpu6050（8元），OLED（SPI接口版20元），超声波传感器（3元），蜂鸣器（0.5元），摄像头（可选），基本都是网上很容易买到的元件