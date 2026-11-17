/*
 * Copyright (C) 2012-2013 Arctium Emulation <http://arctium.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

namespace Connection_Patcher.Patches
{
    class Windows
    {
        public static class x86
        {
            public static byte[] Send  = { 0x33, 0xC0, 0x90 };
            public static byte[] Email = { 0x90, 0x90, 0xEB };
            public static byte[] User  = { 0x00 };
            public static byte[] RaF   = { 0x33, 0xC0, 0x90, 0x90, 0x90 };
            public static byte[] Rcv   = { 0xFF, 0x0F, 0x084 };
            public static byte[] LoginEvent1 = { 0x68, 0x6F, 0x03, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90 };
            public static byte[] LoginEvent2 = { 0x68, 0x70, 0x03, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90 };
        }

        public static class x64
        {
            public static byte[] Send  = { 0x45, 0x31, 0xC0, 0x90, 0x90 };
            public static byte[] Email = { 0xEB };
            public static byte[] User  = { 0x00 };
            public static byte[] RaF   = { 0x33, 0xC0, 0x90, 0x90, 0x90 };
            public static byte[] Rcv   = { 0xFF, 0x0F, 0x84 };
            public static byte[] LoginEvent1 = { 0xB9, 0x6F, 0x03, 0x00, 0x00, 0x0F, 0x1F, 0x44, 0x00, 0x00 };
            public static byte[] LoginEvent2 = { 0xB9, 0x70, 0x03, 0x00, 0x00, 0x33, 0xD2, 0x0F, 0x1F, 0x44, 0x00, 0x00 };
        }
    }
}
