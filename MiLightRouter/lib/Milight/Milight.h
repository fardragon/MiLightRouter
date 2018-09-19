#ifndef __MILIGHT__
#define __MILIGHT__

#include <Arduino.h>
#include <NRF24MiLightRadio.h>


enum class MilightCommand
{
    ALL_ON = 1,
    ALL_OFF,
    GROUP_1_ON,
    GROUP_1_OFF,
    GROUP_2_ON,
    GROUP_2_OFF,
    GROUP_3_ON,
    GROUP_3_OFF,
    GROUP_4_ON,
    GROUP_4_OFF,
    SPEED_PLUS,
    SPEED_MINUS,
    MODES,
    BRIGHTNESS,
    COLOR,
    ALL_WHITE,
    GROUP_1_UNPAIR_WHITE,
    GROUP_2_UNPAIR_WHITE,
    GROUP_3_UNPAIR_WHITE,
    GROUP_4_UNPAIR_WHITE,
    GROUP_1_PAIR,
    GROUP_2_PAIR,
    GROUP_3_PAIR,
    GROUP_4_PAIR,
    LAST
};


class Milight
{
    public:
    Milight(const uint8_t RF24_CE, const uint8_t RF24_CS);
    uint8_t init();

    void send_command(const MilightCommand command, uint8_t color, uint8_t brightess);
    void receive();

    private:

    void _send_command(uint8_t data[], uint8_t data_size);

    private:
    NRF24MiLightRadio m_radio;
    uint8_t m_device_id[2];
    uint8_t m_packet_id;




};

MilightCommand string_to_command(const std::string str);
uint8_t level_to_brightness(const uint8_t level);


#endif