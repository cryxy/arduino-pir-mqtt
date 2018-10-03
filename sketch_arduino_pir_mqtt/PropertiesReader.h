#include <SPI.h>
#include <SD.h>

const int chipSelect = 4; //Chip Pin für die SD Karte(bei UNO 4,bei MEGA 53)

File configFile;
uint8_t filePosition;
uint8_t workingIndex = 0;
uint8_t settingNameValuePositions[16][3];
uint8_t temporaryBufferPosition = 0;
char temporaryBuffer[34];

void setPosition(uint8_t mrk)
{
  settingNameValuePositions[workingIndex][mrk] = filePosition;
}

uint8_t getPosition(uint8_t idx, uint8_t mrk)
{
  return settingNameValuePositions[idx][mrk];
}

void addToBuffer(char inp)
{
  temporaryBuffer[temporaryBufferPosition] = inp;
  temporaryBufferPosition++;
}

void resetBuffer()
{
  temporaryBufferPosition = 0;
  memset(temporaryBuffer, 0, sizeof(temporaryBuffer));
}

char *requestSetting(char *settingName)
{
  Serial.println("Request setting: " + String(settingName));
  for (int i = 0; i < workingIndex; i++)
  {
    uint8_t nS = getPosition(i, 0); //$
    uint8_t eQ = getPosition(i, 1); //=
    uint8_t vE = getPosition(i, 2); //;
    bool isFound = false;
    resetBuffer();
    for (int g = nS; g < eQ - 1; g++)
    {
      configFile.seek(g);
      addToBuffer(configFile.read());
    }
    if (strcmp(temporaryBuffer, settingName) == 0)
    {
      isFound = true;
      resetBuffer();
    }
    for (int g = eQ; g < vE - 1; g++)
    {
      configFile.seek(g);
      addToBuffer(configFile.read());
    }
    if (isFound)
    {
      isFound = false;
      Serial.println("Return: " + String(temporaryBuffer));
      return temporaryBuffer;
    }
  }
  return "0";
}

bool readConfigFile(char *fileName)
{
  Serial.println("INITIALIZING SD CARD");
  pinMode(chipSelect, OUTPUT); 
  digitalWrite(chipSelect, HIGH);
  if (!SD.begin(chipSelect))
  {
    Serial.println("INITIALIZING SD CARD FAILED");
    return false;
  }
  else
  {
    Serial.println("INITIALIZING SD CARD COMPLETED");
    Serial.println("OPENING FILE: " + String(fileName));
    configFile = SD.open(fileName);
    if (!configFile)
    {
      Serial.println("ERROR OPENING FILE: " + String(fileName));
      return false;
    }
    else
    {
      Serial.println("FILE " + String(fileName) + " OPENED");
      Serial.println("SEARCHING FOR MARKER POSITIONS");
      while (configFile.available())
      {
        char fileChar = configFile.read();
        Serial.print(fileChar);
        filePosition = configFile.position();
        switch (fileChar)
        {
        case '$':
          setPosition(0);
          break;
        case '=':
          setPosition(1);
          break;
        case ';':
          setPosition(2);
          workingIndex++;
        }
      }
    }
    return true;
  }
}