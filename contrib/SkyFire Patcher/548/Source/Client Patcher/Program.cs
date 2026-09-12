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

using System;
using System.Threading;
using Connection_Patcher.Constants;

namespace Connection_Patcher
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 1)
            {
                using (var patcher = new Patcher(args[0]))
                {
                    Console.WriteLine("Arctium Connection Patcher");
                    Console.WriteLine("Press Enter to patch...");
                    Console.ReadKey(true);

                    switch (patcher.Type)
                    {
                        case BinaryTypes.Pe32:
                            patcher.Patch(Patches.Windows.x86.Send, Patterns.Windows.x86.Send);
                            patcher.Patch(Patches.Windows.x86.Email, Patterns.Windows.x86.Email);
                            patcher.Patch(Patches.Windows.x86.User, Patterns.Windows.x86.User);
                            patcher.Patch(Patches.Windows.x86.RaF, Patterns.Windows.x86.RaF);
                            patcher.Patch(Patches.Windows.x86.Rcv, Patterns.Windows.x86.Rcv);
                            patcher.Patch(Patches.Windows.x86.LoginEvent1, Patterns.Windows.x86.LoginEvent1);
                            patcher.Patch(Patches.Windows.x86.LoginEvent2, Patterns.Windows.x86.LoginEvent2);

                            patcher.Binary = patcher.Binary.Replace(".exe", "") + "_Patched.exe";

                            patcher.Finish();
                            break;
                        case BinaryTypes.Pe64:
                            patcher.Patch(Patches.Windows.x64.Send, Patterns.Windows.x64.Send);
                            patcher.Patch(Patches.Windows.x64.Email, Patterns.Windows.x64.Email);
                            patcher.Patch(Patches.Windows.x64.User, Patterns.Windows.x64.User);
                            patcher.Patch(Patches.Windows.x64.RaF, Patterns.Windows.x64.RaF);
                            patcher.Patch(Patches.Windows.x64.Rcv, Patterns.Windows.x64.Rcv);
                            patcher.Patch(Patches.Windows.x64.LoginEvent1, Patterns.Windows.x64.LoginEvent1);
                            patcher.Patch(Patches.Windows.x64.LoginEvent2, Patterns.Windows.x64.LoginEvent2);

                            patcher.Binary = patcher.Binary.Replace(".exe", "") + "_Patched.exe";

                            patcher.Finish();
                            break;
                        case BinaryTypes.Mach32:
                            patcher.Patch(Patches.Mac.x86.Send, Patterns.Mac.x86.Send);
                            patcher.Patch(Patches.Mac.x86.Email, Patterns.Mac.x86.Email);
                            patcher.Patch(Patches.Mac.x86.User, Patterns.Mac.x86.User);
                            patcher.Patch(Patches.Mac.x86.RaF, Patterns.Mac.x86.RaF);
                            patcher.Patch(Patches.Mac.x86.Rcv, Patterns.Mac.x86.Rcv);

                            patcher.Binary = patcher.Binary + " Patched";

                            patcher.Finish();
                            break;
                        case BinaryTypes.Mach64:
                            patcher.Patch(Patches.Mac.x64.Send, Patterns.Mac.x64.Send);
                            patcher.Patch(Patches.Mac.x64.Email, Patterns.Mac.x64.Email);
                            patcher.Patch(Patches.Mac.x64.User, Patterns.Mac.x64.User);
                            patcher.Patch(Patches.Mac.x64.RaF, Patterns.Mac.x64.RaF);
                            patcher.Patch(Patches.Mac.x64.Rcv, Patterns.Mac.x64.Rcv);

                            patcher.Binary = patcher.Binary + " Patched";
                             
                            patcher.Finish();
                            break;
                        default:
                            throw new NotSupportedException("Type: " + patcher.Type + " not supported!");
                    }

                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine("Patching done.");

                    Console.ForegroundColor = ConsoleColor.White;
                    Console.WriteLine("Writing patched file...");
                }

                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Successfully created your patched binary.");

                Thread.Sleep(5000);
                Environment.Exit(0);
            }
        }
    }
}
