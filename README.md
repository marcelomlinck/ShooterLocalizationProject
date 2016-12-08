# ShooterLocalizationProject
This project was developed at MSU in 2013. It is a shooter localization system that uses arduinos in coordination with a central CPU to calculate the location of the shot. This system was validated outdoors with outstanding results. It was intended to work indoors too, but further implementations are necessary.

ShooterLocalizationProject/
  |
  |
  |_src/
  |  |_serial.c
  |  |_node.c
  |  |_server.c
  |
  |_map/
  |  |_map.txt
  |  |_map.pdf
  |
  |_vid0/
  |  |_file.txt
  |  |_VID0-2shots-file0-2013-07-12 12.28.26.mp4
  |
  |_vid1/
     |_file.txt
     |_VID1-7shots-file1-2013-07-12 12.32.11.mp4


The arduino nodes use the **node.c** codes in order to hear the gunshot sound. A customized shield was developed using an a microphone, an analog filter and a peak detector to detect the sound and convert it via the analog pins of the arduino. All arduinos were connected via serial cable and used a TDM communication protocol to send the data to the central CPU. Whenever a high amplitude sound is detected, the arduinos digitalize it, wait its send time slice (according to its ID), and send it to the central, where all the calculation is done.

The **server.c** program receives the arduino data and calculate the location of the shooter according to a predetermined map of the testing location. This map describes the location limits and obstacles. The calculation is made based on a pre-calculated matrix of the map. The average error percentage of each matrix element determine the location of the shooter.

The **serial.c** program is am interpreter program for the FTDI RS485 device used to communicate the arduinos with the central CPU via serial to USB conversion.

The **map.txt** file describes the map, where each character represents a meter squared. The * represents obstacle areas (i.e. walls) and the empty spaces are free areas.

Finally, the **vid0** and **vid1** folders contain a file.txt that presents the traced data from the arduinos in a live experiment. Both files a video related to it, that plays the moment the data was recorded. The link to these videos is presented below.
- vid0:
<iframe width="560" height="315" src="https://www.youtube.com/embed/lFRdnB4KtSY" frameborder="0" allowfullscreen></iframe>
- vid1:
<iframe width="560" height="315" src="https://www.youtube.com/embed/P0RbY1kCd94" frameborder="0" allowfullscreen></iframe>

###Compiling

On the project root folder run the following commands:

For file of vid0: $ ./compile 0 
For file of vid1: $ ./compile 1
 
For cleaning the compilation files run on the root folder:
$ ./compile -clean

###Running

$ cd sim
$ findshooter map.txt file.txt results.txt

The results will be printed on the results.txt file and on the screen.
