#include "PL1167_nRF24.h"

PL1167_nRF24::PL1167_nRF24(RF24 &radio)
    : m_radio(radio), m_packet_length(0), m_receive_length(0), m_received(false), m_channel(0)
{
}

int PL1167_nRF24::open()
{
  m_radio.begin();
  m_radio.setAutoAck(false);
  m_radio.setPALevel(RF24_PA_MAX);
  m_radio.setDataRate(RF24_1MBPS);
  m_radio.disableCRC();

  m_syncwordLength = 5;
  m_radio.setAddressWidth(m_syncwordLength);

  return recalc_parameters();
}

int PL1167_nRF24::recalc_parameters()
{
  int packet_length = m_maxPacketLength + 2;
  int nrf_address_pos = m_syncwordLength;

  if (m_syncword0 & 0x01)
  {
    m_nrf_pipe[--nrf_address_pos] = reverse_bits(((m_syncword0 << 4) & 0xf0) + 0x05);
  }
  else
  {
    m_nrf_pipe[--nrf_address_pos] = reverse_bits(((m_syncword0 << 4) & 0xf0) + 0x0a);
  }
  m_nrf_pipe[--nrf_address_pos] = reverse_bits((m_syncword0 >> 4) & 0xff);
  m_nrf_pipe[--nrf_address_pos] = reverse_bits(((m_syncword0 >> 12) & 0x0f) + ((m_syncword3 << 4) & 0xf0));
  m_nrf_pipe[--nrf_address_pos] = reverse_bits((m_syncword3 >> 4) & 0xff);
  m_nrf_pipe[--nrf_address_pos] = reverse_bits(((m_syncword3 >> 12) & 0x0f) + 0x50);

  m_receive_length = packet_length;

  m_radio.openWritingPipe(m_nrf_pipe);
  m_radio.openReadingPipe(1, m_nrf_pipe);

  m_radio.setChannel(2 + m_channel);

  m_radio.setPayloadSize(packet_length);
  return 0;
}

int PL1167_nRF24::setSyncword(uint16_t syncword0, uint16_t syncword3)
{
  m_syncwordLength = 5;
  m_syncword0 = syncword0;
  m_syncword3 = syncword3;
  return recalc_parameters();
}

int PL1167_nRF24::setCRC(bool crc)
{
  m_crc = crc;
  return recalc_parameters();
}

int PL1167_nRF24::setMaxPacketLength(uint8_t maxPacketLength)
{
  m_maxPacketLength = maxPacketLength;
  return recalc_parameters();
}

int PL1167_nRF24::receive(uint8_t channel)
{
  if (channel != m_channel)
  {
    m_channel = channel;
    int retval = recalc_parameters();
    if (retval < 0)
    {
      return retval;
    }
  }

  m_radio.startListening();
  if (m_radio.available())
  {
    internal_receive();
  }

  if (m_received)
  {
    return m_packet_length;
  }
  else
  {
    return 0;
  }
}

int PL1167_nRF24::readFIFO(uint8_t data[], size_t &data_length)
{
  if (data_length > m_packet_length)
  {
    data_length = m_packet_length;
  }

  std::copy(m_packet, m_packet+data_length, data);
  m_packet_length -= data_length;
  if (m_packet_length)
  {
    memmove(m_packet, m_packet + data_length, m_packet_length);
  }
  return m_packet_length;
}

int PL1167_nRF24::writeFIFO(const uint8_t data[], size_t data_length)
{
  if (data_length > sizeof(m_packet))
  {
    data_length = sizeof(m_packet);
  }
  std::copy(data, data+data_length, m_packet);
  m_packet_length = data_length;
  m_received = false;

  return data_length;
}

int PL1167_nRF24::transmit(uint8_t channel)
{
  if (channel != m_channel)
  {
    m_channel = channel;
    int retval = recalc_parameters();
    if (retval < 0)
    {
      return retval;
    }
    yield();
  }

  m_radio.stopListening();
  uint8_t tmp[sizeof(m_packet)];
  int outp = 0;

  uint16_t crc;
  if (m_crc)
  {
    crc = calc_crc(m_packet, m_packet_length);
  }

  for (int inp = 0; inp < m_packet_length + (m_crc ? 2 : 0) + 1; inp++)
  {
    if (inp < m_packet_length)
    {
      tmp[outp++] = reverse_bits(m_packet[inp]);
    }
    else if (m_crc && inp < m_packet_length + 2)
    {
      tmp[outp++] = reverse_bits((crc >> ((inp - m_packet_length) * 8)) & 0xff);
    }
  }

  yield();

  m_radio.write(tmp, outp);
  return 0;
}

int PL1167_nRF24::internal_receive()
{
  uint8_t tmp[sizeof(m_packet)];
  int outp = 0;

  m_radio.read(tmp, m_receive_length);

  open();

  for (int inp = 0; inp < m_receive_length; inp++)
  {
    tmp[outp++] = reverse_bits(tmp[inp]);
  }

  if (m_crc)
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

  std::copy(tmp, tmp+outp, m_packet);

  m_packet_length = outp;
  m_received = true;
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
