#include "spi.h"
#define START_BIT   0x04
#define MODE_SINGLE 0x02    // Single-ended mode

/*
Init function.

Initializes the bcm2835 spi for sampling.
*/
int init() {
	if (!bcm2835_init()) {
		printf("oops, could not init bcm2835\n");
		return 1;
	} else {
		printf("Initialized\n");
		return 0;
	}
}

/*
Setup function.

Sets some of the parameters for SPI interface.
*/
void setup() {
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // Most significant bit fist
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // Clock polority and phase (default)
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);   // Clock
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // Chip Select
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
}

/*
end function.

deinitilizes the spi unit and ends the spi connection.
*/
void end() {
	bcm2835_spi_end();
	bcm2835_close();
}

/*
sample_deallocate function.

Deallocates the space allocated for sample data structure.
First of all, all of the channels are freed.
Then the structure itself is freed.
*/
void sample_deallocate(sample *s) {
	for (int i = 0; i < 8; i++) {
		free(s->channel_data[i]);
	}
	free(s);
}

/*
sample_allocate function.

Allocates the sample data structure.

Also allocates all of the specified channels.
*/
sample *sample_allocate(int *channels) {
    sample *s = malloc(sizeof(sample));
	int max_size = 1 << SIZE_LIMIT; 
	printf("Max Size: %d\n",max_size);
	for (int i = 0; i < 8; i++) {
		if (channels[i]) {
			printf("Allocating space for channel %d\n",i);
			int *data = malloc(sizeof(int) * max_size);
			s->channel_data[i] = data; 
		}
	}
	return s;
}


/*
sample_channel function.

Gets one sample from the specified channel.

The command is format from two half-bytes: high(four most significant bits) and low(four least significant bits).
*/
int sample_channel(uint8_t channel) {
	uint8_t miso[3] = {0}; // This is the input port, which will be used to receive data from ADC.
    uint8_t mosi[3] = {0}; // This is the output port, which will be used for communicating with ADC.

    // Spi communication command looks the following way:
    // There are eight channels total CH0 - CH7. Therefore we need three bits to represent them.
    // For example, channel 0 is '000', while channel 3 is '011'.
    // 0(first bit is always low), 1 (START_BIT), MODE (1 - single, 0 - differential), FIRST MS CHANNEL BIT <- command_high
    // (for example, if we were to select channel 7 - '111', our first most significant bit would be '1')
	uint8_t command_high = (START_BIT | MODE_SINGLE | ((channel & 0x04) >> 2)) << 4;
	// SECOND MS CHANNEL BIT, THIRST MS CHANNEL BIT, 0, 0 <- command high
   	uint8_t command_low = (channel & 0x03) << 2;
	mosi[0] = command_high | command_low; // combine them together into a single byte
	bcm2835_spi_transfernb(mosi, miso, 3); //send over the spi.
	int value = (miso[1] << 4) | (miso[2] >> 4);  // We extract bits here.
	return value;
}

/*
downsample function.

original - is the data to downsample
size - original data size
frequency - desired resulting frequency
*/
int *downsample(int *original,int size,int frequency) {
    clock_t start = clock();
	printf("Starting to downsample\n");
	int *downsampled_data = malloc(sizeof(int)*2*frequency); // allocate space for downsampled data.
	double number_of_samples = (double)size;
	double downsample_factor = number_of_samples / frequency; // the conversion factor, which tells us by how much we want to downsample
	// For example if our original data was a ~100k sample, but we want to get 12.5k sample downsample_factor would be equal to eight (100/12.5).

	// The downsampling
	// Each loop iteration we increase our index by downsample_factor
	// So if our factor was 8, we would select each 8th sample out of the original.
	for (double j = 0.0; j < number_of_samples; j += downsample_factor) {
		int index = (int)floor(j); 
		int down_index = (int)floor(j/downsample_factor); 
		downsampled_data[down_index] = original[index];
	}
	printf("Downsample factor: %f\n",downsample_factor);
    clock_t end = clock();
    float diff = ((float)(end - start) / CLOCKS_PER_SEC ); 
	printf("Downsampling time: %f\n",diff);
	return downsampled_data;
}

/*
sample_data function.

channels - binary channel vector that specifies which channels to sample.
For example, if we want sample channel 0 only:
channels = [1,0,0,0,0,0,0,0]
while if we wanted to sample channel 0 and channel 5:
channels = [1,0,0,0,0,1,0,0]
frequency - desired resulting frequency
*/
sample *sample_data(int *channels,int frequency) {
	printf("Starting sampling ...\n");
	printf("Sampling window size: %d\n",WINDOW_SIZE);
	printf("Max window size: %d\n",SIZE_LIMIT);
     
	sample *s = sample_allocate(channels);
    clock_t start = clock(); // We need to keep track of our clock cycles to determine when we are done, since we only want to sample for one second.
    float diff = 0.0; 
	int i;
	for (i = 0; diff < 1.0; i++) {
		for (int channel_index = 0; channel_index < 8; channel_index++) { // Go through all the channels
			if (channels[channel_index]) { // sample only the ones that we need
				int data = sample_channel(channel_index);
				s->channel_data[channel_index][i] = data; 
			}
		}
        clock_t current = clock();
        diff = ((float)(current - start) / CLOCKS_PER_SEC ); // Update time
	}  

	printf("Samples taken: %d\n",i); // Is the number of samples per channel taken.

	for (int channel_index = 0; channel_index < 8; channel_index++) { // Go through all the channels again
		if (channels[channel_index]) { // Pick the channels that we need and downsample them
			printf("Downsampling channel %d\n",channel_index); 
			int *original_data = s->channel_data[channel_index];
			int *downsampled_data = downsample(original_data,i,frequency);
			s->channel_data[channel_index] = downsampled_data;
			free(original_data); // Get rid of the higher frequency sample only keeping the downsampled version.
		}
	}
	return s;
}
