#include "pcd/qt-serial_communicator/message.h"

#include <QtEndian>
#include <cstring>

using namespace serial_communicator;

// CONSTRUCTORS
message::message(unsigned short id)
{
    message::m_id = id;
    message::m_priority = 0;
    message::m_data_length = 0;
    message::m_data = nullptr;
}
message::message(unsigned short id, unsigned short data_length)
{
    message::m_id = id;
    message::m_priority = 0;
    message::m_data_length = data_length;
    message::m_data = new unsigned char[data_length];
}
message::message(const unsigned char* byte_array)
{
    // Deserialize the message from the byte array.
    // Read the ID.
    message::m_id = qFromBigEndian<unsigned short>(*reinterpret_cast<const unsigned short*>(&byte_array[0]));
    // Read the priority.
    message::m_priority = byte_array[2];
    // Read the data length.
    message::m_data_length = qFromBigEndian<unsigned short>(*reinterpret_cast<const unsigned short*>(&byte_array[3]));
    // Read the data.
    message::m_data = new unsigned char[message::m_data_length];
    std::memcpy(message::m_data, &byte_array[5], message::m_data_length);
}
message::~message()
{
    // Clean up data array.
    if(message::m_data)
    {
        delete [] message::m_data;
    }
}

// METHODS
template <typename T>
void message::set_field(unsigned short address, T data)
{
    message::set_field(address, sizeof(data), &data);
}
template void message::set_field<unsigned char>(unsigned short address, unsigned char data);
template void message::set_field<char>(unsigned short address, char data);
template void message::set_field<unsigned short>(unsigned short address, unsigned short data);
template void message::set_field<short>(unsigned short address, short data);
template void message::set_field<unsigned int>(unsigned short address, unsigned int data);
template void message::set_field<int>(unsigned short address, int data);
template void message::set_field<unsigned long>(unsigned short address, unsigned long data);
template void message::set_field<long>(unsigned short address, long data);
template void message::set_field<float>(unsigned short address, float data);
template void message::set_field<double>(unsigned short address, double data);

void message::set_field(unsigned short address, unsigned int size, void *data)
{
    switch(size)
    {
    case 1:
    {
        message::m_data[address] = *static_cast<unsigned char*>(data);
        break;
    }
    case 2:
    {
        unsigned short value = qToBigEndian<unsigned short>(*static_cast<unsigned short*>(data));
        std::memcpy(&message::m_data[address], & value, 2);
        break;
    }
    case 4:
    {
        unsigned int value = qToBigEndian<unsigned int>(*static_cast<unsigned int*>(data));
        std::memcpy(&message::m_data[address], & value, 4);
        break;
    }
    case 8:
    {
        unsigned long long value = qToBigEndian<unsigned long long>(*static_cast<unsigned long long*>(data));
        std::memcpy(&message::m_data[address], & value, 8);
        break;
    }
    }
}

template <typename T>
T message::get_field(unsigned short address) const
{
    T output;
    message::get_field(address, sizeof(output), &output);
    return output;
}
template unsigned char message::get_field<unsigned char>(unsigned short address) const;
template char message::get_field<char>(unsigned short address) const;
template unsigned short message::get_field<unsigned short>(unsigned short address) const;
template short message::get_field<short>(unsigned short address) const;
template unsigned int message::get_field<unsigned int>(unsigned short address) const;
template int message::get_field<int>(unsigned short address) const;
template unsigned long message::get_field<unsigned long>(unsigned short address) const;
template long message::get_field<long>(unsigned short address) const;
template float message::get_field<float>(unsigned short address) const;
template double message::get_field<double>(unsigned short address) const;

void message::get_field(unsigned short address, unsigned int size, void *data) const
{
    switch(size)
    {
    case 1:
    {
        *static_cast<unsigned char*>(data) = static_cast<unsigned char>(message::m_data[address]);
        break;
    }
    case 2:
    {
        unsigned short value = qFromBigEndian<unsigned short>(*reinterpret_cast<unsigned short*>(&message::m_data[address]));
        std::memcpy(data, &value, 2);
        break;
    }
    case 4:
    {
        unsigned int value = qFromBigEndian<unsigned int>(*reinterpret_cast<unsigned int*>(&message::m_data[address]));
        std::memcpy(data, &value, 4);
        break;
    }
    case 8:
    {
        unsigned long long value = qFromBigEndian<unsigned long long>(*reinterpret_cast<unsigned long long*>(&message::m_data[address]));
        std::memcpy(data, &value, 8);
        break;
    }
    }
}

void message::serialize(unsigned char *byte_array) const
{
    // Serialize the ID first.
    unsigned short be_id = qToBigEndian<unsigned short>(message::m_id);
    std::memcpy(&byte_array[0], &be_id, 2);
    // Serialize priority.
    byte_array[2] = message::m_priority;
    // Serialize data length.
    unsigned short be_data_length = qToBigEndian<unsigned short>(message::m_data_length);
    std::memcpy(&byte_array[3], &be_data_length, 2);
    // Copy in data, as it is already guaranteed big endian.
    std::memcpy(&byte_array[5], message::m_data, message::m_data_length);
}


// PROPERTIES
unsigned short message::p_id() const
{
    return message::m_id;
}
unsigned char message::p_priority() const
{
    return message::m_priority;
}
unsigned short message::p_data_length() const
{
    return message::m_data_length;
}
unsigned int message::p_message_length() const
{
    return message::m_data_length + 5;
}
