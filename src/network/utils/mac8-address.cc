/*
 * Copyright (c) 2009 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Leonard Tracy <lentracy@gmail.com>
 */

#include "mac8-address.h"

#include "ns3/address.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("Mac8Address");
//ATTRIBUTE_HELPER_CPP(Mac8Address);

#define ASCII_a (0x41)
#define ASCII_z (0x5a)
#define ASCII_A (0x61)
#define ASCII_Z (0x7a)
#define ASCII_COLON (0x3a)
#define ASCII_ZERO (0x30)

/**
 * Converts a char to lower case.
 * \param c the char
 * \returns the lower case
 */
static char
AsciiToLowCase(char c)
{
    NS_LOG_FUNCTION(c);
    if (c >= ASCII_a && c <= ASCII_z)
    {
        return c;
    }
    else if (c >= ASCII_A && c <= ASCII_Z)
    {
        return c + (ASCII_a - ASCII_A);
    }
    else
    {
        return c;
    }
}

uint8_t Mac8Address::m_allocationIndex = 0;

Mac8Address::Mac8Address()
{
    NS_LOG_FUNCTION(this);
    std::memset(&m_address, 0, 1);
}

Mac8Address::Mac8Address(uint8_t addr)
    :m_address(addr)
{
}

Mac8Address::Mac8Address(const char* str)
{
    NS_LOG_FUNCTION(this << str);
    int i = 0;
    while (*str != 0 && i < 1)
    {
        uint8_t byte = 0;
        while (*str != ASCII_COLON && *str != 0)
        {
            byte <<= 4;
            char low = AsciiToLowCase(*str);
            if (low >= ASCII_a)
            {
                byte |= low - ASCII_a + 10;
            }
            else
            {
                byte |= low - ASCII_ZERO;
            }
            str++;
        }
        m_address = byte;
        i++;
        if (*str == 0)
        {
            break;
        }
        str++;
    }
    NS_ASSERT(i == 1);
}

void
Mac8Address::CopyFrom(const uint8_t* pBuffer)
{
    NS_LOG_FUNCTION(this << &pBuffer);
    m_address = *pBuffer;
}

void
Mac8Address::CopyTo(uint8_t* pBuffer) const
{
    NS_LOG_FUNCTION(this << &pBuffer);
    *pBuffer = m_address;
}

bool
Mac8Address::IsMatchingType(const Address& address)
{
    NS_LOG_FUNCTION(&address);
    return address.CheckCompatible(GetType(), 1);
}

Mac8Address::operator Address() const
{
    return ConvertTo();
}

Mac8Address
Mac8Address::ConvertFrom(const Address& address)
{
    NS_ASSERT(IsMatchingType(address));
    NS_ASSERT(address.CheckCompatible(GetType(), 1));
    Mac8Address retval;
    address.CopyTo(&retval.m_address);
    return retval;
}

Address
Mac8Address::ConvertTo() const
{
    NS_LOG_FUNCTION(this);
    return Address(GetType(), &m_address, 1);
}

Mac8Address
Mac8Address::Allocate()
{
    NS_LOG_FUNCTION_NOARGS();

    if (m_allocationIndex == 0)
    {
        Simulator::ScheduleDestroy(Mac8Address::ResetAllocationIndex);
    }

    m_allocationIndex++;
    Mac8Address address;
    address.m_address = m_allocationIndex;
    if (m_allocationIndex == 255)
    {
        m_allocationIndex = 0;
    }

    return address;
}

void
Mac8Address::ResetAllocationIndex()
{
    NS_LOG_FUNCTION_NOARGS();
    m_allocationIndex = 0;
}

uint8_t
Mac8Address::GetType()
{
    NS_LOG_FUNCTION_NOARGS();
    static uint8_t type = Address::Register();
    return type;
}

Mac8Address
Mac8Address::GetBroadcast()
{
    NS_LOG_FUNCTION_NOARGS();
    static Mac8Address broadcast = Mac8Address("ff");
    return broadcast;
}

bool
operator<(const Mac8Address& a, const Mac8Address& b)
{
    return a.m_address < b.m_address;
}

bool
operator==(const Mac8Address& a, const Mac8Address& b)
{
    return a.m_address == b.m_address;
}

bool
operator!=(const Mac8Address& a, const Mac8Address& b)
{
    return !(a == b);
}

std::ostream&
operator<<(std::ostream& os, const Mac8Address& address)
{
    os << (int)address.m_address;
    return os;
}

std::istream&
operator>>(std::istream& is, Mac8Address& address)
{
    uint8_t x;
    is >> x;
    NS_ASSERT(0 <= x);
    NS_ASSERT(x <= 255);
    address.m_address = x;
    return is;
}

} // namespace ns3