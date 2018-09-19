#include <PL1167_nRF24.h>
#include <NRF24MiLightRadio.h>
#include <algorithm>

#define PACKET_ID(packet, packet_length) ((packet[1] << 8) | packet[packet_length - 1])

NRF24MiLightRadio::NRF24MiLightRadio(const uint8_t RF24_CE, const uint8_t RF24_CS)
    : m_radio(RF24_CE, RF24_CS), m_pl1167(PL1167_nRF24(m_radio)), m_waiting(false)
{
}

uint8_t NRF24MiLightRadio::begin()
{
  auto retval = m_pl1167.open();
  if (retval < 0)
  {
    return 1;
  }

  retval = configure();
  if (retval < 0)
  {
    return 2;
  }

  available();

  return 0;
}

int NRF24MiLightRadio::configure()
{
  int retval = m_pl1167.setCRC(true);
  if (retval < 0)
  {
    return retval;
  }

  retval = m_pl1167.setSyncword(0x147A, 0x258B);
  if (retval < 0)
  {
    return retval;
  }

  retval = m_pl1167.setMaxPacketLength(8);
  if (retval < 0)
  {
    return retval;
  }

  return 0;
}

bool NRF24MiLightRadio::available()
{
  if (m_waiting)
  {
    return true;
  }

  if (m_pl1167.receive(m_channels[0]) > 0)
  {

    size_t packet_length = sizeof(m_packet);
    if (m_pl1167.readFIFO(m_packet, packet_length) < 0)
    {
      return false;
    }

    if (packet_length == 0 || packet_length != m_packet[0] + 1U)
    {
      return false;
    }
    uint32_t packet_id = PACKET_ID(m_packet, packet_length);

    if (packet_id == m_prev_packet_id)
    {
      m_dupes_received++;
    }
    else
    {
      m_prev_packet_id = packet_id;
      m_waiting = true;
    }
  }

  return m_waiting;
}

int NRF24MiLightRadio::read(uint8_t frame[], size_t &frame_length)
{
  if (!m_waiting)
  {
    frame_length = 0;
    return -1;
  }

  if (frame_length > sizeof(m_packet) - 1)
  {
    frame_length = sizeof(m_packet) - 1;
  }

  if (frame_length > m_packet[0])
  {
    frame_length = m_packet[0];
  }

  std::copy(m_packet + 1, m_packet + 1 + frame_length, frame);
  m_waiting = false;

  return m_packet[0];
}

int NRF24MiLightRadio::write(uint8_t frame[], size_t frame_length)
{
  if (frame_length > sizeof(m_out_packet) - 1)
  {
    return -1;
  }

  std::copy(frame, frame+frame_length, m_out_packet+1);
  m_out_packet[0] = frame_length;

  resend();
  return frame_length;
}

void NRF24MiLightRadio::resend()
{
  for (size_t i = 0; i < 3; i++)
  {
    m_pl1167.writeFIFO(m_out_packet, m_out_packet[0] + 1);
    m_pl1167.transmit(m_channels[i]);
  }
}