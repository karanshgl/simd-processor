/*Copyright (C) 2016, IIT Ropar
 * This file is part of SimpleRISC assembler.
 *
 * SimpleRISC assembler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleRISC assembler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Neeraj Goel (email: neeraj@iitrpr.ac.in)
 */

#include <assembler.h>

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cout << "Command format is assembler <input asm file> <output hex file>" << std::endl;
        exit(1);
    }
    Assembler a(argv[1], argv[2]);
    return 0;
}
