/* 
 * WAV Recorder for Seeed XIAO ESP32S3 Sense 
*/

#include <I2S.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// make changes as needed
#define RECORD_TIME   5  // seconds, The maximum value is 240
#define WAV_FILE_NAME "/arduino_rec"

#define MAX_FILES 5

// do not change for best
#define SAMPLE_RATE 16000U
#define SAMPLE_BITS 16
#define WAV_HEADER_SIZE 44
#define VOLUME_GAIN 2

#define NO_OF_RECORDINGS 5
#define SLEEP_INTERVAL 100  


void setup() {
  Serial.begin(115200);
  // while (!Serial) ;
  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, SAMPLE_RATE, SAMPLE_BITS)) {
    Serial.println("Failed to initialize I2S!");
    while (1) ;
  }
  if(!SD.begin(21)){
    Serial.println("Failed to mount SD Card!");
    while (1) ;
  }

  Serial.println("ESP started Successfully ---------------------------------------------------- >");

  for (int i = 0; i < NO_OF_RECORDINGS ; i++) {
    String new_file_name = list_last_wav_files();
    record_wav_save_to_file(new_file_name);

    // Put the ESP32 into deep sleep for the specified interval
    // Serial.println("Going to sleep between recordings...");
    // esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL * 1000); // Convert milliseconds to microseconds
    // esp_light_sleep_start();  // Enter light sleep mode (use deep sleep for longer intervals)
    
    // Serial.println("Woke up for next recording...");


    delay(10000); // 1-second delay between prints
  }

  
  // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER); 
  // Once all recordings are done, go into deep sleep indefinitely
  Serial.println("All recordings complete. Going to deep sleep...");
  delay(100);
  esp_deep_sleep_start();  // Put the ESP32 into deep sleep indefinitely
}

void loop() {

}

// String get_new_wav_file_name() {
//   File root = SD.open("/");
//   if (!root) {
//     Serial.println("Failed to open directory!");
//     return "";
//   }

//   if (!root.isDirectory()) {
//     Serial.println("Not a directory!");
//     return "";
//   }

//   String fileName;
//   int maxNumber = 0;
//   File file = root.openNextFile();

//   // Iterate over all files and find the highest numbered .wav file
//   while (file) {
//     if (!file.isDirectory()) {
//       String currentFileName = file.name();
//       if (currentFileName.endsWith(".wav") && currentFileName.startsWith("/arduino_rec_")) {
//         // Extract the number part from the filename
//         String numberPart = currentFileName.substring(13, currentFileName.length() - 4);  // "arduino_rec_" is 12 characters long, ".wav" is 4 characters
//         Serial.println(numberPart);
//         int fileNumber = numberPart.toInt();  // Convert the number part to an integer
//         Serial.println(numberPart);
//         if (fileNumber > maxNumber) {
//           maxNumber = fileNumber;
//         }
//       }
//     }
//     file = root.openNextFile();
//   }

//   // Generate new filename with incremented number
//   int newFileNumber = maxNumber + 1;
//   fileName = "/arduino_rec_" + String(newFileNumber) + ".wav";

//   Serial.print("New file name: ");
//   Serial.println(fileName);

//   return fileName;
// }

String list_last_wav_files() {
  File root = SD.open("/");

  if (!root) {
    Serial.println("Failed to open directory!");
    return "";
  }

  if (!root.isDirectory()) {
    Serial.println("Not a directory!");
    return "";
  }

  String wavFiles[MAX_FILES]; // Array to hold the last 5 file names
  int fileCount = 0;

  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      String fileName = file.name();
      if (fileName.endsWith(".wav")) {
        // Shift files in the array if the limit is exceeded
        if (fileCount < MAX_FILES) {
          wavFiles[fileCount] = fileName;
          fileCount++;
        } else {
          // Shift elements left when more than 5 files are found
          for (int i = 0; i < MAX_FILES - 1; i++) {
            wavFiles[i] = wavFiles[i + 1];
          }
          wavFiles[MAX_FILES - 1] = fileName;
        }
      }
    }
    file = root.openNextFile();  // Move to the next file
  }
  String Last_File_name = "";
  Serial.println("Last 5 WAV Files:");
  for (int i = 0; i < fileCount; i++) {
    Serial.println(wavFiles[i]);  // Print the last 5 .wav file names
    Last_File_name = wavFiles[i] ; 
  }

  Serial.print("Last File name is => ");
  Serial.println(Last_File_name);

  
  String numberPart = Last_File_name.substring(12, 16);  // Extracts from index 11 to 14 (end index is exclusive)
  int fileNumber = numberPart.toInt();
  Serial.println(numberPart);

  // Convert to String with leading zeros
  String paddedNumber = String(fileNumber + 1);
    while (paddedNumber.length() < 4) {
      paddedNumber = "0" + paddedNumber;  // Add leading zeros until length is 4
    }

  Serial.print("Next File name will be =>");
  String Nxt_file_name = String(WAV_FILE_NAME) + "_" + paddedNumber + ".wav";

  Serial.println(Nxt_file_name);

  return Nxt_file_name;

}

void record_wav_save_to_file(String file_name)
{
  uint32_t sample_size = 0;
  uint32_t record_size = (SAMPLE_RATE * SAMPLE_BITS / 8) * RECORD_TIME;
  uint8_t *rec_buffer = NULL;
  Serial.printf("Ready to start recording ...\n");

  File file = SD.open(file_name, FILE_WRITE);

  // Check if file opened successfully
  if (!file) {
    Serial.println("Failed to open the file for writing!");
    return;
  }


  // Write the header to the WAV file
  uint8_t wav_header[WAV_HEADER_SIZE];
  generate_wav_header(wav_header, record_size, SAMPLE_RATE);
  file.write(wav_header, WAV_HEADER_SIZE);

  // PSRAM malloc for recording
  rec_buffer = (uint8_t *)ps_malloc(record_size);
  if (rec_buffer == NULL) {
    Serial.printf("malloc failed!\n");
    while(1) ;
  }
  Serial.printf("Buffer: %d bytes\n", ESP.getPsramSize() - ESP.getFreePsram());

  // Start recording
  esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, rec_buffer, record_size, &sample_size, portMAX_DELAY);
  if (sample_size == 0) {
    Serial.printf("Record Failed!\n");
  } else {
    Serial.printf("Record %d bytes\n", sample_size);
  }

  // Increase volume
  for (uint32_t i = 0; i < sample_size; i += SAMPLE_BITS/8) {
    (*(uint16_t *)(rec_buffer+i)) <<= VOLUME_GAIN;
  }

  // Write data to the WAV file
  Serial.printf("Writing to the file ...\n");
  if (file.write(rec_buffer, record_size) != record_size)
    Serial.printf("Write file Failed!\n");

  free(rec_buffer);
  file.close();
  Serial.printf("The recording is over.\n");
}

void record_wav()
{
  uint32_t sample_size = 0;
  uint32_t record_size = (SAMPLE_RATE * SAMPLE_BITS / 8) * RECORD_TIME;
  uint8_t *rec_buffer = NULL;
  Serial.printf("Ready to start recording ...\n");

  File file = SD.open("/"WAV_FILE_NAME".wav", FILE_WRITE);
  // Write the header to the WAV file
  uint8_t wav_header[WAV_HEADER_SIZE];
  generate_wav_header(wav_header, record_size, SAMPLE_RATE);
  file.write(wav_header, WAV_HEADER_SIZE);

  // PSRAM malloc for recording
  rec_buffer = (uint8_t *)ps_malloc(record_size);
  if (rec_buffer == NULL) {
    Serial.printf("malloc failed!\n");
    while(1) ;
  }
  Serial.printf("Buffer: %d bytes\n", ESP.getPsramSize() - ESP.getFreePsram());

  // Start recording
  esp_i2s::i2s_read(esp_i2s::I2S_NUM_0, rec_buffer, record_size, &sample_size, portMAX_DELAY);
  if (sample_size == 0) {
    Serial.printf("Record Failed!\n");
  } else {
    Serial.printf("Record %d bytes\n", sample_size);
  }

  // Increase volume
  for (uint32_t i = 0; i < sample_size; i += SAMPLE_BITS/8) {
    (*(uint16_t *)(rec_buffer+i)) <<= VOLUME_GAIN;
  }

  // Write data to the WAV file
  Serial.printf("Writing to the file ...\n");
  if (file.write(rec_buffer, record_size) != record_size)
    Serial.printf("Write file Failed!\n");

  free(rec_buffer);
  file.close();
  Serial.printf("The recording is over.\n");
}

void generate_wav_header(uint8_t *wav_header, uint32_t wav_size, uint32_t sample_rate)
{
  // See this for reference: http://soundfile.sapp.org/doc/WaveFormat/
  uint32_t file_size = wav_size + WAV_HEADER_SIZE - 8;
  uint32_t byte_rate = SAMPLE_RATE * SAMPLE_BITS / 8;
  const uint8_t set_wav_header[] = {
    'R', 'I', 'F', 'F', // ChunkID
    file_size, file_size >> 8, file_size >> 16, file_size >> 24, // ChunkSize
    'W', 'A', 'V', 'E', // Format
    'f', 'm', 't', ' ', // Subchunk1ID
    0x10, 0x00, 0x00, 0x00, // Subchunk1Size (16 for PCM)
    0x01, 0x00, // AudioFormat (1 for PCM)
    0x01, 0x00, // NumChannels (1 channel)
    sample_rate, sample_rate >> 8, sample_rate >> 16, sample_rate >> 24, // SampleRate
    byte_rate, byte_rate >> 8, byte_rate >> 16, byte_rate >> 24, // ByteRate
    0x02, 0x00, // BlockAlign
    0x10, 0x00, // BitsPerSample (16 bits)
    'd', 'a', 't', 'a', // Subchunk2ID
    wav_size, wav_size >> 8, wav_size >> 16, wav_size >> 24, // Subchunk2Size
  };
  memcpy(wav_header, set_wav_header, sizeof(set_wav_header));
}