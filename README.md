<!-- TOC -->
* [The purpose of this project is to provide a UHD like implementation of Ipsolon SDR products](#the-purpose-of-this-project-is-to-provide-a-uhd-like-implementation-of-ipsolon-sdr-products)
  * [Example UHD Implementation](#example-uhd-implementation)
  * [Example IHD implementation](#example-ihd-implementation)
  * [UML Diagram](#uml-diagram)
  * [Docker container with the release version of GNU Radio in the latest stable Ubuntu](#docker-container-with-the-release-version-of-gnu-radio-in-the-latest-stable-ubuntu)
  * [Building IHD](#building-ihd)
  * [Example programs](#example-programs)
    * [Rx samples to file](#rx-samples-to-file)
    * [Example usage](#example-usage)
      * [PSD stream](#psd-stream)
        * [Note](#note)
    * [Packet check](#packet-check)
      * [Example usage PSD data](#example-usage-psd-data)
      * [Example usage IQ data](#example-usage-iq-data)
        * [Note](#note-1)
<!-- TOC -->

# The purpose of this project is to provide a UHD like implementation of Ipsolon SDR products

The goal is to minimize the number of changes necessary for a UHD user to implement an Ipsolon SDR.

- USRP - Universal Software Radio Protocol
- UHD - USRP Hardware Driver


- ISRP - Ipsolon Software Radio Protocol
- IHD - ISRP Hardware Driver

## Example UHD Implementation

Below is an example taken from UHD rx_samples_to_file.cpp:

``` C++
 // create a usrp device
uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);
...
// set the sample rate
usrp->set_rx_rate(rate, channel);

// set the center frequency

uhd::tune_request_t tune_request(freq, lo_offset);
if (vm.count("int-n")) {
    tune_request.args = uhd::device_addr_t("mode_n=integer");
}
usrp->set_rx_freq(tune_request, channel);
uhd::stream_args_t stream_args(cpu_format, wire_format);
std::vector<size_t> channel_nums;
channel_nums.push_back(channel);
stream_args.channels             = channel_nums;
uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);

uhd::rx_metadata_t md;
std::vector<samp_type> buff(samps_per_buff);

// setup streaming
uhd::stream_cmd_t stream_cmd((num_requested_samples == 0)
                                 ? uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS
                                 : uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
stream_cmd.num_samps  = size_t(num_requested_samples);
stream_cmd.stream_now = true;
stream_cmd.time_spec  = uhd::time_spec_t();
rx_stream->issue_stream_cmd(stream_cmd);
...
while (not stop_signal_called
       and (num_requested_samples != num_total_samps or num_requested_samples == 0)
       and (time_requested == 0.0 or std::chrono::steady_clock::now() <= stop_time)) {
         size_t num_rx_samps = rx_stream->recv(&buff.front(), buff.size(), md, 3.0, enable_size_map);
         /* --- Process Samples --- */
```

## Example IHD implementation

To replace the UHD implementation, simply replace `multi_usrp` with `ipsolon_isrp`

``` C++
 // create a isrp device
ihd::ipsolon_isrp::sptr isrp = ihd::ipsolon_isrp::make(args);

/*** Once the Ipsolon device is created, it is used exactly the same as the usrp device */
...
// set the sample rate
isrp->set_rx_rate(rate, channel);

// set the center frequency
set_rx_freq(tune_request, channel);
uhd::stream_args_t stream_args(cpu_format, wire_format);
std::vector<size_t> channel_nums;
channel_nums.push_back(channel);
stream_args.channels             = channel_nums;
uhd::rx_streamer::sptr rx_stream = ->get_rx_stream(stream_args);

uhd::rx_metadata_t md;
std::vector<samp_type> buff(samps_per_buff);

// setup streaming
uhd::stream_cmd_t stream_cmd((num_requested_samples == 0)
                                 ? uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS
                                 : uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
stream_cmd.num_samps  = size_t(num_requested_samples);
stream_cmd.stream_now = true;
stream_cmd.time_spec  = uhd::time_spec_t();
rx_stream->issue_stream_cmd(stream_cmd);

while (not stop_signal_called
       and (num_requested_samples != num_total_samps or num_requested_samples == 0)
       and (time_requested == 0.0 or std::chrono::steady_clock::now() <= stop_time)) {
         size_t num_rx_samps = rx_stream->recv(&buff.front(), buff.size(), md, 3.0, enable_size_map);
        /* --- Process Samples --- */
```

## UML Diagram

![IHD.png](doc/images/IHD.png)

## Docker container with the release version of GNU Radio in the latest stable Ubuntu

The Dockerfile is used to be able to manage versions and compatibility issues of the gnuradio and uhd utilities. The
resulting container has the GUI available (so you may use the gnuradio companion and QT widgets) as well as audio.
Several packages needed to compile and test are also installed and configured.

If you want the release version of GNU Radio (currently 3.10):

1. Install docker by following [Docker's installation guide](https://docs.docker.com/get-docker/) (though I've tested
   this on Ubuntu only, so for convenience a direct link to the
   corresponding [guide for Ubuntu](https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository)).
2. Clone this repo.
3. Enter the `docker/uhd-gnuradio-latest` folder and execute `./docker_build.sh` (this step is necessary only once, or
   every time you modify *Dockerfile*)
4. Run the container: `./docker_run.sh`

You will then be in a command line logged as user *titan* who is a sudoer (password: *titan*). The *ihd* folder in this
project will be mounted inside the docker container and will persist after you exit the container.  **Remember**:
everything else is erased after you exit the container. This means for instance that if you compile and install you will
have to do that again every time you run the container (unless for instance you compile in the *ihd* folder). You may
modify *run_docker.sh* script to avoid this by adding other mount points.

The host's USB/ethernet should be accessible from the docker too, meaning you should be able to use any SDR hardware you
plug into the PC.

## Building IHD

In the top level folder of this project:

```shell
mkdir build
cd build
cmake ..
make
# To install shard libraries:
sudo make install && sudo ldconfig
# To un-install:
sudo make uninstall
```

## Example programs

This project currently includes two example programs: rx_samples_to_file and packet_check.

### Rx samples to file

This program will connect to the Chameleon radio, start the stream, receive UDP packets and save them to a file. It
supports IQ and PSD streams. Only one channel at time is supported.

```shell
./rx_samples_to_file --help
Revision: 1.0.0 Jan  7 2025 14:58:00
IHD RX samples to file Allowed options:
  --help                         help message
  --file arg (=isrp_samples.dat) name of the file to write binary samples to
  --duration arg (=30)           total number of seconds to receive
  --gain arg (=0)                set RX gain
  --nsamps arg (=0)              total number of samples to receive
  --freq arg (=2400000000)       RF center frequency in Hz
  --channel arg (=1)             which channel to use
  --dest_ip arg (=0.0.0.0)       Destination IP address
  --dest_port arg (=9090)        Destination port
  --fft_size arg (=256)          FFT size (256, 512, 1024, 2048 or 4096
  --fft_avg arg (=105)           FFT averaging count
  --args arg                     ISRP device address args
  --stream_type arg (=psd)       Stream type - (psd or iq)


This application streams data from a single channel of a Ipsolon device to a file.
```

### Example usage

#### PSD stream

The example below show using rx_samples_to_file to test the PSD stream with different FFT sizes.

```shell
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./rx_samples_to_file --file=/tmp/isrp_samples.dat --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=psd --nsamps=9999872 --fft_size=256 --channel=1
Revision: 1.0.0 Jan  7 2025 14:58:00

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ll /tmp/isrp_samples.dat
-rw-r--r-- 1 ipsolon ipsolon 39999488 Jan  7 16:57 /tmp/isrp_samples.dat
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./rx_samples_to_file --file=/tmp/isrp_samples.dat --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=psd --nsamps=9999872 --fft_size=512 --channel=1
Revision: 1.0.0 Jan  7 2025 14:58:00

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ll /tmp/isrp_samples.dat
-rw-r--r-- 1 ipsolon ipsolon 39999488 Jan  7 16:57 /tmp/isrp_samples.dat
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./rx_samples_to_file --file=/tmp/isrp_samples.dat --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=psd --nsamps=9999360 --fft_size=1024 --channel=1
Revision: 1.0.0 Jan  7 2025 14:58:00

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ll /tmp/isrp_samples.dat
-rw-r--r-- 1 ipsolon ipsolon 39997440 Jan  7 16:58 /tmp/isrp_samples.dat
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./rx_samples_to_file --file=/tmp/isrp_samples.dat --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=psd --nsamps=9998336 --fft_size=2048 --channel=1
Revision: 1.0.0 Jan  7 2025 14:58:00

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ll /tmp/isrp_samples.dat
-rw-r--r-- 1 ipsolon ipsolon 39993344 Jan  7 16:58 /tmp/isrp_samples.dat
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./rx_samples_to_file --file=/tmp/isrp_samples.dat --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=psd --nsamps=9998336 --fft_size=4096 --channel=1
Revision: 1.0.0 Jan  7 2025 14:58:00

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ll /tmp/isrp_samples.dat
-rw-r--r-- 1 ipsolon ipsolon 39993344 Jan  7 16:59 /tmp/isrp_samples.dat
ipsolon@sdew-dev-superlogic:/ihd/build$ (master)

```

##### Note

The files size will be four time the number of samples request (4 bytes per sample). The number of samples will be a
multiple of the FFT size time four.

### Packet check

Packet check will start a stream of one or more channels and check the sequences numbers of every packet to test for
dropped packets. Each channel stream will have its own thread that runs in parallel. Only one stream type is
currently supported (either PSD or IQ, no mix and match).

```shell
 ./packet_check --help
Revision: 1.0.0 Jan  7 2025 14:58:21
IHD RX samples to file Allowed options:
  --help                   help message
  --duration arg (=30)     total number of seconds to receive
  --chan_mask arg (=1)     channel mask (chan 1 = 0x1, chan 2 = 0x2, chan 1 & 2
                           = 0x3)
  --dest_ip arg (=0.0.0.0) destination IP address
  --dest_port arg (=9090)  destination port
  --fft_size arg (=256)    FFT size (256, 512, 1024, 2048 or 4096
  --fft_avg arg (=105)     FFT averaging count
  --args arg               ISRP device address args
  --stream_type arg (=psd) Stream type - (psd or iq)

```

#### Example usage PSD data

Below is an example of using packet check to test PSD data. The packet check utility uses 'chan_mask' to test multiple
channels simultaneously. In the example, it sets each bit in the mask one-by-one starting with bit 0. In other words,
it starts with bit 0 (channel 1), then sets bit 0 & 1 (channels 1 & 2), and so forth until all four channels are running
in parallel.

```shell
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./packet_check --args=addr=10.75.42.208 --duration=10 --dest_ip=10.75.42.37 --dest_port=9090 --fft_size=4096  --fft_avg=130 --chan_mask=1
Revision: 1.0.0 Jan  7 2025 14:58:21

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
Run stream for channel:1
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./packet_check --args=addr=10.75.42.208 --duration=10 --dest_ip=10.75.42.37 --dest_port=9090 --fft_size=4096  --fft_avg=130 --chan_mask=3
Revision: 1.0.0 Jan  7 2025 14:58:21

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
Run stream for channel:1
Run stream for channel:2
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./packet_check --args=addr=10.75.42.208 --duration=10 --dest_ip=10.75.42.37 --dest_port=9090 --fft_size=4096  --fft_avg=130 --chan_mask=7
Revision: 1.0.0 Jan  7 2025 14:58:21

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
Run stream for channel:1
Run stream for channel:2
Run stream for channel:3
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
RESULT duration ms:10004 packets:4612 bytes:75636800 Mb/s:57.706299 errors:0
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./packet_check --args=addr=10.75.42.208 --duration=10 --dest_ip=10.75.42.37 --dest_port=9090 --fft_size=4096  --fft_avg=130 --chan_mask=15
Revision: 1.0.0 Jan  7 2025 14:58:21

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
Run stream for channel:1
Run stream for channel:2
Run stream for channel:3
Run stream for channel:4
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
RESULT duration ms:10000 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
RESULT duration ms:10000 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
RESULT duration ms:10001 packets:4610 bytes:75604000 Mb/s:57.681274 errors:0
ipsolon@sdew-dev-superlogic:/ihd/build$ (master)

```

#### Example usage IQ data

That same can be done with IQ. The example below shows channel 1 and then
channels 1 & 2 simultaneously.

```shell
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./packet_check --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=iq --chan_mask=1
Revision: 1.0.0 Jan  7 2025 14:58:21

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
Run stream for channel:1
RESULT duration ms:9400 packets:83499 bytes:684023808 Mb/s:173.956250 errors:0
ipsolon@sdew-dev-superlogic:/ihd/build$ (master) ./packet_check --args="addr=10.75.42.208" --dest_ip="10.75.42.37" --stream_type=iq --chan_mask=3
Revision: 1.0.0 Jan  7 2025 14:58:21

Creating the ISRP device with: addr=10.75.42.208...
[INFO] [UHD] linux; GNU C++ version 11.2.0; Boost_107400; UHD_4.1.0.5-3
Run stream for channel:1
Run stream for channel:2
RESULT duration ms:9934 packets:83499 bytes:684023808 Mb/s:173.956250 errors:0
RESULT duration ms:9864 packets:83499 bytes:684023808 Mb/s:173.956250 errors:0

```

##### Note

Currently, the chameleon only supports two IQ streams and the number of packets is limited to 83499. This is due
a UDP transfer rate issue and memory limits.
