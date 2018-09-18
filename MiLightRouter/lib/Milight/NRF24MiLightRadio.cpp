#include <PL1167_nRF24.h>
#include <NRF24MiLightRadio.h>

#define PACKET_ID(packet, packet_length) ( (packet[1] << 8) | packet[packet_length - 1] )

NRF24MiLightRadio::NRF24MiLightRadio(const uint8_t RF24_CE, const uint8_t RF24_CS)
  : _radio(RF24_CE, RF24_CS), _pl1167(PL1167_nRF24(_radio)), _waiting(false)
{ 
}

int NRF24MiLightRadio::begin()
{
  int retval = _pl1167.open();
  if (retval < 0) {
    return retval;
  }

  retval = configure();
  if (retval < 0) {
    return retval;
  }

  available();

  return 0;
}

int NRF24MiLightRadio::configure()
 {
  int retval = _pl1167.setCRC(true);
  if (retval < 0) {
    return retval;
  }

  retval = _pl1167.setPreambleLength(3);
  if (retval < 0) {
    return retval;
  }

  retval = _pl1167.setTrailerLength(4);
  if (retval < 0) {
    return retval;
  }

  retval = _pl1167.setSyncword(0x147A, 0x258B);
  if (retval < 0) {
    return retval;
  }

  retval = _pl1167.setMaxPacketLength(8);
  if (retval < 0) {
    return retval;
  }

  return 0;
}

bool NRF24MiLightRadio::available() 
{
  if (_waiting) {
    return true;
  }

  if (_pl1167.receive(_channels[0]) > 0) {

    size_t packet_length = sizeof(_packet);
    if (_pl1167.readFIFO(_packet, packet_length) < 0) {
      return false;
    }

    if (packet_length == 0 || packet_length != _packet[0] + 1U) {
      return false;
    }
    uint32_t packet_id = PACKET_ID(_packet, packet_length);

    if (packet_id == _prev_packet_id) {
      _dupes_received++;
    } else {
      _prev_packet_id = packet_id;
      _waiting = true;
    }
  }

  return _waiting;
}

int NRF24MiLightRadio::read(uint8_t frame[], size_t &frame_length)
{
  if (!_waiting) {
    frame_length = 0;
    return -1;
  }

  if (frame_length > sizeof(_packet) - 1) {
    frame_length = sizeof(_packet) - 1;
  }

  if (frame_length > _packet[0]) {
    frame_length = _packet[0];
  }

  memcpy(frame, _packet + 1, frame_length);
  _waiting = false;

  return _packet[0];
}

int NRF24MiLightRadio::write(uint8_t frame[], size_t frame_length) 
{
  if (frame_length > sizeof(_out_packet) - 1) {
    return -1;
  }

  memcpy(_out_packet + 1, frame, frame_length);
  _out_packet[0] = frame_length;

  resend();
  return frame_length;
}

void NRF24MiLightRadio::resend() 
{
  for (size_t i = 0; i < 3; i++) {
    _pl1167.writeFIFO(_out_packet, _out_packet[0] + 1);
    _pl1167.transmit(_channels[i]);
  }
}