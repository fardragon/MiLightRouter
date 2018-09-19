#include "PL1167_nRF24.h"

PL1167_nRF24::PL1167_nRF24(RF24 &radio)
    : _radio(radio)
{
}

int PL1167_nRF24::open()
{
  _radio.begin();
  _radio.setAutoAck(false);
  _radio.setPALevel(RF24_PA_MAX);
  _radio.setDataRate(RF24_1MBPS);
  _radio.disableCRC();

  _syncwordLength = 5;
  _radio.setAddressWidth(_syncwordLength);

  return recalc_parameters();
}

int PL1167_nRF24::recalc_parameters()
{
  int packet_length = _maxPacketLength + 2;
  int nrf_address_pos = _syncwordLength;

  if (_syncword0 & 0x01)
  {
    _nrf_pipe[--nrf_address_pos] = reverse_bits(((_syncword0 << 4) & 0xf0) + 0x05);
  }
  else
  {
    _nrf_pipe[--nrf_address_pos] = reverse_bits(((_syncword0 << 4) & 0xf0) + 0x0a);
  }
  _nrf_pipe[--nrf_address_pos] = reverse_bits((_syncword0 >> 4) & 0xff);
  _nrf_pipe[--nrf_address_pos] = reverse_bits(((_syncword0 >> 12) & 0x0f) + ((_syncword3 << 4) & 0xf0));
  _nrf_pipe[--nrf_address_pos] = reverse_bits((_syncword3 >> 4) & 0xff);
  _nrf_pipe[--nrf_address_pos] = reverse_bits(((_syncword3 >> 12) & 0x0f) + 0x50);

  _receive_length = packet_length;

  _radio.openWritingPipe(_nrf_pipe);
  _radio.openReadingPipe(1, _nrf_pipe);

  _radio.setChannel(2 + _channel);

  _radio.setPayloadSize(packet_length);
  return 0;
}

int PL1167_nRF24::setSyncword(uint16_t syncword0, uint16_t syncword3)
{
  _syncwordLength = 5;
  _syncword0 = syncword0;
  _syncword3 = syncword3;
  return recalc_parameters();
}

int PL1167_nRF24::setCRC(bool crc)
{
  _crc = crc;
  return recalc_parameters();
}

int PL1167_nRF24::setMaxPacketLength(uint8_t maxPacketLength)
{
  _maxPacketLength = maxPacketLength;
  return recalc_parameters();
}

int PL1167_nRF24::receive(uint8_t channel)
{
  if (channel != _channel)
  {
    _channel = channel;
    int retval = recalc_parameters();
    if (retval < 0)
    {
      return retval;
    }
  }

  _radio.startListening();
  if (_radio.available())
  {
    internal_receive();
  }

  if (_received)
  {
    return _packet_length;
  }
  else
  {
    return 0;
  }
}

int PL1167_nRF24::readFIFO(uint8_t data[], size_t &data_length)
{
  if (data_length > _packet_length)
  {
    data_length = _packet_length;
  }
  memcpy(data, _packet, data_length);
  _packet_length -= data_length;
  if (_packet_length)
  {
    memmove(_packet, _packet + data_length, _packet_length);
  }
  return _packet_length;
}

int PL1167_nRF24::writeFIFO(const uint8_t data[], size_t data_length)
{
  if (data_length > sizeof(_packet))
  {
    data_length = sizeof(_packet);
  }
  memcpy(_packet, data, data_length);
  _packet_length = data_length;
  _received = false;

  return data_length;
}

int PL1167_nRF24::transmit(uint8_t channel)
{
  if (channel != _channel)
  {
    _channel = channel;
    int retval = recalc_parameters();
    if (retval < 0)
    {
      return retval;
    }
    yield();
  }

  _radio.stopListening();
  uint8_t tmp[sizeof(_packet)];
  int outp = 0;

  uint16_t crc;
  if (_crc)
  {
    crc = calc_crc(_packet, _packet_length);
  }

  for (int inp = 0; inp < _packet_length + (_crc ? 2 : 0) + 1; inp++)
  {
    if (inp < _packet_length)
    {
      tmp[outp++] = reverse_bits(_packet[inp]);
    }
    else if (_crc && inp < _packet_length + 2)
    {
      tmp[outp++] = reverse_bits((crc >> ((inp - _packet_length) * 8)) & 0xff);
    }
  }

  yield();

  _radio.write(tmp, outp);
  return 0;
}

int PL1167_nRF24::internal_receive()
{
  uint8_t tmp[sizeof(_packet)];
  int outp = 0;

  _radio.read(tmp, _receive_length);

  open();

  for (int inp = 0; inp < _receive_length; inp++)
  {
    tmp[outp++] = reverse_bits(tmp[inp]);
  }

  if (_crc)
  {
    if (outp < 2)
    {

      return 0;
    }
    uint16_t crc = calc_crc(tmp, outp - 2);
    if (((crc & 0xff) != tmp[outp - 2]) || (((crc >> 8) & 0xff) != tmp[outp - 1]))
    {
      return 0;
    }
    outp -= 2;
  }

  memcpy(_packet, tmp, outp);

  _packet_length = outp;
  _received = true;
  return outp;
}

#define CRC_POLY 0x8408

uint16_t PL1167_nRF24::calc_crc(uint8_t *data, size_t data_length)
{
  uint16_t state = 0;
  for (size_t i = 0; i < data_length; i++)
  {
    uint8_t byte = data[i];
    for (int j = 0; j < 8; j++)
    {
      if ((byte ^ state) & 0x01)
      {
        state = (state >> 1) ^ CRC_POLY;
      }
      else
      {
        state = state >> 1;
      }
      byte = byte >> 1;
    }
  }
  return state;
}

uint8_t PL1167_nRF24::reverse_bits(uint8_t data)
{
  uint8_t result = 0;
  for (int i = 0; i < 8; i++)
  {
    result <<= 1;
    result |= data & 1;
    data >>= 1;
  }
  return result;
}
