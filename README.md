# High Flyers Data Protocol Version 2.0
Software to use WiFi cards to transmit peer to peer data. Updated HFDP.

High Flyers Data Protocol is a software inspired by [WifiBroadcast](https://github.com/rodizio1/ez-wifibroadcast/wiki).
Using Pcap library and of-the-shelf wifi modules (like [this](https://www.alfa.com.tw/service_1_detail/5.htm)) we send any udp data peer-to-peer.
This software was developed on ubuntu and rasbian, requiers just pcaplib-dev and pthread!

HFDP include many options usefull for autonomous vehicles:

  - Can send data from any local UDP client/server (like telemetry or video)
  - Two modules doesn't need to have bidirectional communication
  - Each packet can be pass via man-in-the-middle (you can have two planes and one can act as a retransmitter)
  - Any wifi module that supports packet injection and monitor mode can be used

## Why 2.0?

Many mistakes were made during draft of the first version. After some thought I have came to conclusion,
that updating code would take more time than rewriting. Now with little more skill I will try to make it better :upside_down_face:.
Besides I want a cpp project to make myself better :sunglasses:.

## List of things I want to change:

### 1. Modern C++ instead of C.
With C++ smart pointers, thread managment and class architecture things should be easier.
Previously I wanted to keep the project in C because PCAP library is in C but I think more gain will be with using C++.

### 2. Buffer based Tx/Rx.
Not using buffers was a huge mistake in HFDPv1. Packets were blocking arrival on another when waiting for transmitting or recieving sockets.

### 3. FEC in mind.
FEC (forward error correction) should give benefit of capability of repairing broken recived packets based on fec data.
To use it, protocol should be rewritten.

### 4. JSON configs.
Random .txt files for keeping MAC adresses, and configurations sucks. Even more sucks to write dump function to read this data.
That is why I want to keep everything in nice JSON file for easy readability and implementation.

### 5. Moar threads.
Everyone likes threads (especially when u have to pass data between them), that is why I want to add more of them, so CPU load will be better distributed :fire::fire::fire:.

### 6. Advanced PCAP.
PCAP library offers low-level packets filtering which can decrease CPU load. Some of this functionality was in one of the branches of HFDPv1.