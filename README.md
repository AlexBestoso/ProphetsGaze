# ProphetsGaze
This is a cipher that I've developed for enciphering my hand written notes. You may use it. If anyone manages to break it, please share your attack!

This was built for linux. But it uses standard libraries, so you should be able to use the class in a windows application. 

To compile the code in Linux using the g++ compiler use
    g++ main.cc -o cipher

To encipher a message use the command like so:
    ./cipher -e "KeyGoesHere" "Secret Message Goes Here."

To decipher a message, use the command: 
     ./cipher -d "KeyGoesHere" "Encrypted Message Goes Here."

WARNING : THIS MAY BE ILLEGAL IN GERMANY AND EUROPE
